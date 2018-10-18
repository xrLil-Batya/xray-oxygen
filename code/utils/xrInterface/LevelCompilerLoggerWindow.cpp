#include "pch.hpp"
#include "LevelCompilerLoggerWindow.hpp"
#include "resource.h"
#include "xrCore/xrCore.h"
#include <time.h>
#include <mmsystem.h>
#include <CommCtrl.h>
#include "UIParams.hpp"

UIParams* pUIParams = nullptr;

LevelCompilerLoggerWindow::LevelCompilerLoggerWindow()
{
	*status = 0;
	*phase = 0;
}

void LevelCompilerLoggerWindow::Initialize(const char* name)
{
	if (initialized)
		return;
	InitCommonControls();
	Sleep(150);
	xr_strcpy(this->name, name);
	thread_spawn(LogThreadProc, "log-update", 1024 * 1024, this);
	while (!logWindow)
		Sleep(150);
	initialized = true;
}

void LevelCompilerLoggerWindow::Destroy()
{
	close = true;
	Sleep(500);
}

void LevelCompilerLoggerWindow::LogThreadProc(void* context)
{
    LevelCompilerLoggerWindow* ptr = static_cast<LevelCompilerLoggerWindow*>(context);
	ptr->LogThreadProc();
}

static INT_PTR CALLBACK LevelCompilerLoggerWindowDlgProc(HWND hw, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
	case WM_DESTROY: break;
	case WM_CLOSE: ExitProcess(0); break;
	case WM_COMMAND:
		if (LOWORD(wp) == IDCANCEL)
			ExitProcess(0);
		break;
	default: return FALSE;
	}
	return TRUE;
}

void LevelCompilerLoggerWindow::LogThreadProc()
{
	SetProcessPriorityBoost(GetCurrentProcess(), TRUE);
	logWindow =
		CreateDialog(HINSTANCE(GetModuleHandle("xrLCUtil")), MAKEINTRESOURCE(IDD_LOG), 0, LevelCompilerLoggerWindowDlgProc);
	if (!logWindow)
		R_CHK(GetLastError());
	SetWindowText(logWindow, name);
	SetWindowPos(logWindow, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
	hwLog = GetDlgItem(logWindow, IDC_LOG);
	hwProgress = GetDlgItem(logWindow, IDC_PROGRESS);
	hwShutdown = GetDlgItem(logWindow, IDC_CHECK2);
	hwInfo = GetDlgItem(logWindow, IDC_INFO);
	hwStage = GetDlgItem(logWindow, IDC_STAGE);
	hwTime = GetDlgItem(logWindow, IDC_TIMING);
	hwPText = GetDlgItem(logWindow, IDC_P_TEXT);
	hwPhaseTime = GetDlgItem(logWindow, IDC_PHASE_TIME);
	SendMessage(hwProgress, PBM_SETRANGE, 0, MAKELPARAM(0, 1000));
	SendMessage(hwProgress, PBM_SETPOS, 0, 0);
	Msg("\"LevelBuilder v5.0\" alpha build\nCompilation date: %s\n", __DATE__);
	{
		char tmpbuf[128];
		Msg("Startup time: %s", _strtime(tmpbuf));
	}

	string256 u_name;
	unsigned long u_size = sizeof(u_name) - 1;
	GetUserName(u_name, &u_size);
	xr_strlwr(u_name);

	pUIParams = new UIParams();
	// Main cycle
	u32 LogSize = 0;
	float PrSave = 0;
	while (true)
	{
		pUIParams->isShutDown = !!Button_GetCheck(hwShutdown) == BST_CHECKED;

		SetPriorityClass(GetCurrentProcess(), IDLE_PRIORITY_CLASS);
		// transfer data
		while (!csLog.try_lock())
		{
			ProcessMessages();
			Sleep(1);
		}
		if (progress > 1.f)
			progress = 1.f;
		else if (progress < 0)
			progress = 0;
		BOOL bWasChanges = FALSE;
		char tbuf[256];

		{
			std::lock_guard<decltype(csLog)> lock(csLog);
			if (LogSize != LogFile->size())
			{
				bWasChanges = TRUE;
				for (; LogSize < LogFile->size(); LogSize++)
				{
					const char* S = *(*LogFile)[LogSize];
					if (!S)
						S = "";
					SendMessage(hwLog, LB_ADDSTRING, 0, (LPARAM)S);
				}
				SendMessage(hwLog, LB_SETTOPINDEX, LogSize - 1, 0);
				FlushLog();
			}
		}

		if (_abs(PrSave - progress) > EPS_L)
		{
			bWasChanges = TRUE;
			PrSave = progress;
			SendMessage(hwProgress, PBM_SETPOS, u32(progress * 1000.f), 0);
			// timing
			if (progress > 0.005f)
			{
				u32 dwCurrentTime = timeGetTime();
				u32 dwTimeDiff = dwCurrentTime - phase_start_time;
				u32 secElapsed = dwTimeDiff / 1000;
				u32 secRemain = u32(float(secElapsed) / progress) - secElapsed;
				xr_sprintf(tbuf,
					"Elapsed: %s\n"
					"Remain:  %s",
					make_time(secElapsed).c_str(), make_time(secRemain).c_str());
				SetWindowText(hwTime, tbuf);
			}
			else
				SetWindowText(hwTime, "");
			// percentage text
			xr_sprintf(tbuf, "%3.2f%%", progress * 100.f);
			SetWindowText(hwPText, tbuf);
		}
		if (bStatusChange)
		{
			bWasChanges = TRUE;
			bStatusChange = FALSE;
			SetWindowText(hwInfo, status);
		}
		if (bWasChanges)
		{
			UpdateWindow(logWindow);
			bWasChanges = FALSE;
		}
		csLog.unlock();
		ProcessMessages();
		if (close)
			break;
		Sleep(200);
	}
	// Cleanup
	DestroyWindow(logWindow);
}

void LevelCompilerLoggerWindow::ProcessMessages()
{
	MSG msg;
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void LevelCompilerLoggerWindow::clMsg(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	clMsgV(format, args);
	va_end(args);
}

void LevelCompilerLoggerWindow::clMsgV(const char* format, va_list args)
{
	char buf[1024];
	vsprintf(buf, format, args);
	csLog.lock();
	string1024 msg;
	strconcat(sizeof(msg), msg, "    |    | ", buf);
	Log(msg);
	csLog.unlock();
}

void LevelCompilerLoggerWindow::clLog(const char* format, ...)
{
	va_list args;
	char buf[1024];
	va_start(args, format);
	vsprintf(buf, format, args);
	csLog.lock();
	Log(buf);
	csLog.unlock();
}

void LevelCompilerLoggerWindow::Status(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	StatusV(format, args);
	va_end(args);
}

void LevelCompilerLoggerWindow::StatusV(const char* format, va_list args)
{
	char buf[1024];
	vsprintf(buf, format, args);
	csLog.lock();
	xr_strcpy(status, buf);
	bStatusChange = true;
	Msg("    | %s", buf);
	csLog.unlock();
}

void LevelCompilerLoggerWindow::Progress(float progress) { this->progress = progress; }
void LevelCompilerLoggerWindow::Phase(const char* phaseName)
{
	while (!(hwPhaseTime && hwStage))
		Sleep(1);
	csLog.lock();
	// Replace phase name with TIME:Name
	char tbuf[512];
	bPhaseChange = TRUE;
	phase_total_time = timeGetTime() - phase_start_time;
	xr_sprintf(tbuf, "%s : %s", make_time(phase_total_time / 1000).c_str(), phase);
	SendMessage(hwPhaseTime, LB_DELETESTRING, SendMessage(hwPhaseTime, LB_GETCOUNT, 0, 0) - 1, 0);
	SendMessage(hwPhaseTime, LB_ADDSTRING, 0, (LPARAM)tbuf);
	// Start new phase
	phase_start_time = timeGetTime();
	xr_strcpy(phase, phaseName);
	SetWindowText(hwStage, phaseName);
	xr_sprintf(tbuf, "--:--:-- * %s", phase);
	SendMessage(hwPhaseTime, LB_ADDSTRING, 0, (LPARAM)tbuf);
	SendMessage(hwPhaseTime, LB_SETTOPINDEX, SendMessage(hwPhaseTime, LB_GETCOUNT, 0, 0) - 1, 0);
	progress = 0;
	// Release focus
	Msg("\n* New phase started: %s", phaseName);
	csLog.unlock();
}

void LevelCompilerLoggerWindow::Success(const char* msg)
{
	MessageBox(logWindow, msg, "Congratulation!", MB_OK | MB_ICONINFORMATION);
}

void LevelCompilerLoggerWindow::Failure(const char* msg) { MessageBox(logWindow, msg, "Error!", MB_OK | MB_ICONERROR); }
HWND LevelCompilerLoggerWindow::GetWindow() const { return logWindow; }

LevelCompilerLoggerWindow & LevelCompilerLoggerWindow::instance()
{
	static LevelCompilerLoggerWindow instance;
	return instance;
}