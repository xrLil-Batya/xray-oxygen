#include "stdafx.h"
#include "resource.h"
#include "../../xrcore/log.h"
#include <time.h>
#include <mmsystem.h>

//************************* Log-thread data
static xrCriticalSection	csLog;

volatile bool				bClose				= false;

static char					status	[1024	]	="";
static char					phase	[1024	]	="";
static u32					phase_start_time	= 0;
static bool					bStatusChange		= false;
static bool					bPhaseChange		= false;
static bool					bStartPhase			= false;
static u32					phase_total_time	= 0;

static HWND hwLog		= 0;
static HWND hwProgress	= 0;
static HWND hwInfo		= 0;
static HWND hwStage		= 0;
static HWND hwTime		= 0;
static HWND hwPText		= 0;
static HWND hwPhaseTime	= 0;

//************************* Log-thread data
static INT_PTR CALLBACK logDlgProc( HWND hw, UINT msg, WPARAM wp, LPARAM lp )
{
	switch( msg ){
		case WM_DESTROY:
			break;
		case WM_CLOSE:
			ExitProcess		(0);
//			bClose = TRUE;
			break;
		case WM_COMMAND:
			if( LOWORD(wp)==IDCANCEL )
			{
				ExitProcess	(0);
//				bClose = TRUE;
			}
			break;
		default:
			return FALSE;
	}
	return TRUE;
}
static void _process_messages(void)
{
	MSG msg;
	if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void __cdecl Status	(const char *format, ...)
{
	xrCriticalSectionGuard guard(csLog);
	va_list				mark;
	va_start			( mark, format );
	vsprintf			( status, format, mark );
	bStatusChange		= true;
	Msg					("    | %s",status);
}
//Old
std::string make_time(u32 sec)
{
	char		buf[64];
	xr_sprintf(buf, "%2.0d:%2.0d:%2.0d", sec / 3600, (sec % 3600) / 60, sec % 60);
	int len = int(xr_strlen(buf));
	for (int i = 0; i<len; i++) if (buf[i] == ' ') buf[i] = '0';
	return std::string(buf);
}
//New
#include "../../xrCore/DateTime.hpp"
auto MakeTime(u32 sec_, u32 min_, u32 hour_, bool isString = true)
{
	Time tm;
	u32 sec = tm.GetSecond(),
		min = tm.GetMin(),
		hour = tm.GetHour();

	auto GetTime = [](u32 a, u32 &b, int c) { (a > b) ? b = c - (a - b) : b -= a; };

	GetTime(sec_, sec, 60);
	GetTime(min_, min, 60);
	GetTime(hour_, hour, 24);

	return
		((hour < 10) ? "0" : "") + std::to_string(hour) + ":" + ((min < 10) ? "0" : "") + std::to_string(min) + ":" + ((sec < 10) ? "0" : "") + std::to_string(sec);
}
u32 _sec = 0, _min = 0, _hour = 0;

void SetTime()
{
	Time tm;
	_sec = tm.GetSecond();
	_min = tm.GetMin();
	_hour = tm.GetHour();
}

void Phase(const char *phase_name)
{
	while (!(hwPhaseTime && hwStage)) Sleep(1);

	xrCriticalSectionGuard guard(csLog);
	try
	{
		// Replace phase name with TIME:Name 
		bPhaseChange = TRUE;
		char	tbuf_[512];
		//SendMessage(hwPhaseTime, LB_DELETESTRING, SendMessage(hwPhaseTime, LB_GETCOUNT, 0, 0) - 1, 0);
		//SendMessage(hwPhaseTime, LB_ADDSTRING, 0, (LPARAM)tbuf_);

		xr_sprintf(tbuf_, "%s", MakeTime(_sec, _min, _hour).c_str());

		// Start _new phase
		SetTime();
		xr_strcpy(phase, phase_name);
		SetWindowText(hwStage, phase_name);
		strconcat(sizeof(tbuf_), tbuf_, tbuf_, ": ", phase);
		//xr_sprintf(tbuf, "--:--:--: %s", phase);
		
		SendMessage(hwPhaseTime, LB_ADDSTRING, 0, (LPARAM)tbuf_);
		//SendMessage(hwPhaseTime, LB_SETTOPINDEX, SendMessage(hwPhaseTime, LB_GETCOUNT, 0, 0) - 1, 0);
		Progress(0);

		// Release focus
		Msg(" %s Done...\n", phase_name);
	}
	catch (...) 
	{
		Debug.do_exit("The end...");
	}
}

HWND logWindow=0;
void logThread(void *dummy)
{
	SetProcessPriorityBoost	(GetCurrentProcess(),TRUE);

	logWindow = CreateDialog(
		HINSTANCE(GetModuleHandle(0)),
	 	MAKEINTRESOURCE(IDD_LOG),
		0, logDlgProc );
	if (!logWindow) {
		R_CHK			(GetLastError());
	};
	SetWindowPos(logWindow,HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW);
	hwLog		= GetDlgItem(logWindow, IDC_LOG);
	hwProgress	= GetDlgItem(logWindow, IDC_PROGRESS);
	hwInfo		= GetDlgItem(logWindow, IDC_INFO);
	hwStage		= GetDlgItem(logWindow, IDC_STAGE);
	hwTime		= GetDlgItem(logWindow, IDC_TIMING);
	hwPText		= GetDlgItem(logWindow, IDC_P_TEXT);
	hwPhaseTime	= GetDlgItem(logWindow, IDC_PHASE_TIME);

	SendMessage(hwProgress, PBM_SETRANGE,	0, MAKELPARAM(0, 1000)); 
	SendMessage(hwProgress, PBM_SETPOS,		0, 0); 

	Msg("\"LevelBuilderFX v1.1\"\n beta build\nCompilation date: %s\n",__DATE__);
	{
		char tmpbuf[128];
		Msg("Startup time: %s",_strtime(tmpbuf));
	}
	// Main cycle
	u32		LogSize = 0;
	float	PrSave	= 0;
	while (true)
	{
		switch (g_build_options.Priority)
		{
			case 1: SetPriorityClass(GetCurrentProcess(), IDLE_PRIORITY_CLASS); 
			case 2: SetPriorityClass(GetCurrentProcess(), NORMAL_PRIORITY_CLASS); 
			case 3: SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS); 
			case 4: SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS); 
		}
		// transfer data
		while (!csLog.try_lock())	{
			_process_messages	( );
			Sleep				(1);
		}
		if (progress>1.f)		progress = 1.f;
		else if (progress<0)	progress = 0;

		BOOL bWasChanges = FALSE;
		char tbuf		[256];

		{
			xrCriticalSectionGuard guard(csLog);
            if (LogSize != LogFile->size())
            {
                bWasChanges = TRUE;
                for (; LogSize<LogFile->size(); LogSize++)
                {
                    const char *S = *(*LogFile)[LogSize];
                    if (0 == S)	S = "";
                    SendMessage(hwLog, LB_ADDSTRING, 0, (LPARAM) S);
                }
                SendMessage(hwLog, LB_SETTOPINDEX, LogSize - 1, 0);
                //FlushLog		( );
            }
		}
		
		if (_abs(PrSave-progress)>EPS_L) {
			bWasChanges = true;
			PrSave = progress;
			SendMessage(hwProgress, PBM_SETPOS, u32(progress*1000.f), 0);

			// timing
			if (progress>0.5f) {
				//MakeTime(_sec, _min, _hour);
				//u32 dwCurrentTime = timeGetTime();
				//u32 dwTimeDiff	= time[0] + time[1] * 60 + time[2] * 60 *60;
				//u32 secElapsed	= dwTimeDiff/1000;
				//u32 secRemain		= u32((float(time[0] + time[1] * 60 + time[2] * 60 * 60)/1000)/progress)-secElapsed;
				int next_time = 100 - (int)progress;
				xr_sprintf(tbuf, "Elapsed: %s\n Remain: ~%s",
					MakeTime(_sec, _min, _hour).c_str(),
					MakeTime(_sec / next_time, _min / next_time, _hour / next_time).c_str());
				SetWindowText(hwTime, tbuf);
				
			}
			else
			{
				xr_sprintf(tbuf, "Elapsed: %s\n", MakeTime(_sec, _min, _hour).c_str());
				SetWindowText(hwTime, tbuf);
			}
			// percentage text
			xr_sprintf(tbuf,"%3.2f%%",progress*100.f);
			SetWindowText(hwPText, tbuf);
		}

		if (bStatusChange) {
			bWasChanges		= true;
			bStatusChange	= false;
			SetWindowText(hwInfo, status);
		}
		if (bWasChanges) {
			UpdateWindow	( logWindow);
			bWasChanges		= false;
		}
		csLog.unlock			();

		_process_messages	();
		if (bClose)			break;
		Sleep				(200);
	}

	// Cleanup
	DestroyWindow(logWindow);
}

void clLog( LPCSTR msg )
{
	xrCriticalSectionGuard guard(csLog);
	Log				(msg);
}

void __cdecl clMsg( const char *format, ...)
{
	va_list		mark;
	char buf	[4*256];
	va_start	( mark, format );
	vsprintf	( buf, format, mark );


	string1024		_out_;
	strconcat		(sizeof(_out_),_out_,"    |    | ", buf );
	clLog			(_out_);

}


class client_log_impl : public i_lc_log
{
	virtual void clMsg		( LPCSTR msg )			{::clMsg(msg);}
	virtual void clLog		( LPCSTR msg )			{::clLog(msg);}
	virtual void Status		( LPCSTR msg )			{::Status(msg);}
	virtual	void Progress	( const float F )		{::Progress( F );}
	virtual	void Phase		( LPCSTR phase_name )	{::Phase( phase_name );}
public:
	client_log_impl(){ lc_log = this;}
} client_log_impl;
