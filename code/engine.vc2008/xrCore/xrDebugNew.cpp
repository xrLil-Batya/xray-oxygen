#include "stdafx.h"

#include "xrdebug.h"
#include "os_clipboard.h"

#pragma warning(push)
#pragma warning(disable:4995)
#include <malloc.h>
#include <direct.h>
#pragma warning(pop)

#include <exception>
#include <new.h>		// for _set_new_mode
#include <signal.h>		// for signals
#include <sal.h>
#include "cpuid.h"
#include <DbgHelp.h>
#include "xrDebugSymbol.h"
#include "resource.h"
#include <CommCtrl.h>
#include <shellapi.h>
#include <tlhelp32.h>
#include "oxy_version.h"

#include <filesystem>

#if PLATFORM == _WINDOWS
	#include <VersionHelpers.h>
#endif

/////////////////////////////////////
XRCORE_API DWORD gMainThreadId = 0xFFFFFFFF;
XRCORE_API DWORD gSecondaryThreadId = 0xFFFFFFFF;
/////////////////////////////////////


XRCORE_API bool IsMainThread()
{
	return GetCurrentThreadId() == gMainThreadId;
}

XRCORE_API bool IsSecondaryThread()
{
	return GetCurrentThreadId() == gSecondaryThreadId;
}

XRCORE_API	xrDebug		Debug;

XRCORE_API HWND gGameWindow = nullptr;

string_path DumpFilePath = { 0 };

static bool error_after_dialog = false;
static bool bException = false;

LONG WriteMinidump(struct _EXCEPTION_POINTERS* pExceptionInfo);

struct CrashDialogParam
{
	bool bCanContinue;
	LPCSTR CrashReport;
} sCrashReport;

// static memory for writting report in it (since we can't use memory allocation paths)
constexpr int ReportMemSize = 1024 * 1024;
char ReportMem[ReportMemSize];

void xrDebug::gather_info(const char *expression, const char *description, const char *argument0, const char *argument1, const char *file, int line, const char *function, char* assertion_info, u32 const assertion_info_size)
{
	char* buffer_base = assertion_info;
	char* buffer = assertion_info;
	int assertion_size = (int)assertion_info_size;
	const char*	endline = "\n";
	const char*	prefix = "[error]";
	bool extended_description = (description && !argument0 && strchr(description, '\n'));
	buffer += xr_sprintf(buffer, assertion_size - u32(buffer - buffer_base), "%sFATAL ERROR%s%s", endline, endline, endline);

	for (int i = 0; i < 2; ++i) 
	{
		buffer += xr_sprintf(buffer, assertion_size - u32(buffer - buffer_base), "%sExpression    : %s%s", prefix, expression, endline);
		buffer += xr_sprintf(buffer, assertion_size - u32(buffer - buffer_base), "%sFunction      : %s%s", prefix, function, endline);
		buffer += xr_sprintf(buffer, assertion_size - u32(buffer - buffer_base), "%sFile          : %s%s", prefix, file, endline);
		buffer += xr_sprintf(buffer, assertion_size - u32(buffer - buffer_base), "%sLine          : %d%s", prefix, line, endline);

		if (extended_description) 
		{
			buffer += xr_sprintf(buffer, assertion_size - u32(buffer - buffer_base), "%s%s%s", endline, description, endline);
			if (argument0) 
			{
				if (argument1) 
				{
					buffer += xr_sprintf(buffer, assertion_size - u32(buffer - buffer_base), "%s%s", argument0, endline);
					buffer += xr_sprintf(buffer, assertion_size - u32(buffer - buffer_base), "%s%s", argument1, endline);
				}
				else
					buffer += xr_sprintf(buffer, assertion_size - u32(buffer - buffer_base), "%s%s", argument0, endline);
			}
		}
		else 
		{
			buffer += xr_sprintf(buffer, assertion_size - u32(buffer - buffer_base), "%sDescription   : %s%s", prefix, description, endline);
			if (argument0) 
			{
				if (argument1) 
				{
					buffer += xr_sprintf(buffer, assertion_size - u32(buffer - buffer_base), "%sArgument 0    : %s%s", prefix, argument0, endline);
					buffer += xr_sprintf(buffer, assertion_size - u32(buffer - buffer_base), "%sArgument 1    : %s%s", prefix, argument1, endline);
				}
				else
					buffer += xr_sprintf(buffer, assertion_size - u32(buffer - buffer_base), "%sArguments     : %s%s", prefix, argument0, endline);
			}
		}

		buffer += xr_sprintf(buffer, assertion_size - u32(buffer - buffer_base), "%s", endline);
		if (!i) 
		{
			Msg("%s", assertion_info);
			xrLogger::FlushLog();

			buffer = assertion_info;
			endline = "\r\n";
			prefix = "";
		}
	}

	xrLogger::FlushLog();

	os_clipboard::copy_to_clipboard(assertion_info);
}

void xrDebug::do_exit(HWND hWnd, LPCSTR message)
{
	xrLogger::FlushLog();

	string2048 finalMessage;
	xr_sprintf(finalMessage, "%s \n %s", message, "Do you want to interrupt the game?");

	if (MessageBoxA(nullptr, finalMessage, "X-Ray Error", MB_YESNO | MB_TOPMOST) == IDYES)
	{
		DebugBreak();
        ExitProcess(1);
	}
#ifdef AWDA
	// Пусть тут хранится
	MessageBoxA(NULL, "awda", "awda", MB_OK | MB_ICONASTERISK);
#endif
}

void xrDebug::backend(const char* reason, const char* expression, const char *argument0, const char *argument1, const char* file, int line, const char *function)
{
	xrCriticalSectionGuard guard(Lock);
	error_after_dialog = true;

	string4096 assertion_info;

	gather_info(reason, expression, argument0, argument1, file, line, function, assertion_info, sizeof(assertion_info));

	if (handler)
		handler();

	// free cursor from any clipping
	ClipCursor(NULL);

	// Sometimes if we crashed not in main thread, we can stuck at ShowWindow
	HWND gameWindow = NULL;

	if (GetCurrentThreadId() == m_mainThreadId)
	{
		ShowWindow(gGameWindow, SW_HIDE);
		gameWindow = gGameWindow;
	}
	while (PlatformUtils.ShowCursor(true) < 0);

	if (crashhandler * handlerFuncPtr = Debug.get_crashhandler())
	{
		handlerFuncPtr();
	}
	if (!IsDebuggerPresent())
	{
		WriteMinidump(nullptr);
	}

//#if !defined(DEBUG) && !defined(MIXED_NEW)
//	do_exit(gameWindow, assertion_info);
//#else
	//#GIPERION: Don't crash on DEBUG, we have some VERIFY that sometimes failed, but it's not so critical
	do_exit2(gameWindow, assertion_info);
//#endif

	// And we should show window again, damn pause manager
	if (GetCurrentThreadId() == m_mainThreadId)
	{
		ShowWindow(gGameWindow, SW_SHOW);
	}
}

const char* xrDebug::error2string(long code)
{
    static	string1024	desc_storage;
    FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, code, 0, desc_storage, sizeof(desc_storage) - 1, nullptr);
    return desc_storage;
}


void xrDebug::do_exit2(HWND hwnd, const string4096& message)
{
#if 0
    int MsgRet = MessageBox(hwnd, message, "Error", MB_ABORTRETRYIGNORE | MB_ICONERROR);

    switch (MsgRet)
    {
    case IDABORT:
		DebugBreak();
        ExitProcess(1);
        break;
    case IDIGNORE:
        bIgnoreAlways = true;
        break;
    case IDRETRY:
    default:
		DebugBreak();
        break;
    }
#endif
	if (!ShowCrashDialog(nullptr, true, message))
	{
		ExitProcess(1);
	}
}

BOOL CALLBACK CrashDialogProc(HWND hwndDlg,
	UINT message,
	WPARAM wParam,
	LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
	{
		if (sCrashReport.CrashReport != nullptr)
		{
			SetDlgItemText(hwndDlg, IDC_CRASHREPORT, sCrashReport.CrashReport);
		}

		HWND hOkBtn = GetDlgItem(hwndDlg, IDOK);
		if (sCrashReport.bCanContinue)
		{
			EnableWindow(hOkBtn, TRUE);
		}
		else
		{
			EnableWindow(hOkBtn, FALSE);
		}
	}
	return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			// Continue
			EndDialog(hwndDlg, TRUE);
			break;
		case IDCANCEL:
			// Break
			EndDialog(hwndDlg, FALSE);
			break;
		}

		return TRUE;
	case WM_NOTIFY:
		switch (((LPNMHDR)lParam)->code)
		{
		case NM_CLICK:
		case NM_RETURN:
		{
			PNMLINK pNMLink = (PNMLINK)lParam;
			LITEM   item = pNMLink->item;

			if (item.iLink == 0)
			{
				ShellExecuteW(NULL, L"open", item.szUrl, NULL, NULL, SW_SHOW);
			}

			break;
		}
		}
		break;
	}
	return FALSE;
}

bool xrDebug::ShowCrashDialog(_EXCEPTION_POINTERS* ExceptionInfo, bool bCanContinue, const char* message)
{
	constexpr u16 StackSize = 64u;
	void* pStack[StackSize];
	ZeroMemory(&pStack, sizeof(pStack));
	u16 framesNum = DebugSymbols.GetCurrentStack(&pStack[0], StackSize);

	int WriteCursor = 0;

	// I hate macros, but I have no choice
#define WriteToReportMacro(format, ...) WriteCursor += xr_sprintf(&ReportMem[WriteCursor], ReportMemSize - WriteCursor, format, __VA_ARGS__)

	if (message != nullptr)
	{
		WriteToReportMacro("%s", message);
		WriteToReportMacro("\r\n\r\n");
	}

	WriteToReportMacro("Oxygen \"%s\" branch, builted in %s\r\n", _BRANCH, __DATE__);
	if (ExceptionInfo != nullptr)
	{
		if (ExceptionInfo->ExceptionRecord != nullptr)
		{
			PEXCEPTION_RECORD Record = ExceptionInfo->ExceptionRecord;
			WriteToReportMacro("Exception code ");
			if (Record->ExceptionCode == STATUS_ACCESS_VIOLATION)
			{
				WriteToReportMacro("\"ACCESS VIOLATION\"");
			}
			else if (Record->ExceptionCode == STATUS_INVALID_HANDLE)
			{
				WriteToReportMacro("\"INVALID HANDLE\"");
			}
			else if (Record->ExceptionCode == STATUS_ILLEGAL_INSTRUCTION)
			{
				WriteToReportMacro("\"ILLEGAL INSTRUCTION\" CPU not supported?");
			}
			else if (Record->ExceptionCode == STATUS_STACK_OVERFLOW)
			{
				WriteToReportMacro("\"STACK OVERFLOW\"");
			}
			else
			{
				WriteToReportMacro("\"%x\"", Record->ExceptionCode);
			}

			WriteToReportMacro(" at location \"%p\"\r\n", Record->ExceptionAddress);

			if (Record->NumberParameters > 0)
			{
				WriteToReportMacro("Params: ");
				for (DWORD i = 0; i < Record->NumberParameters && i < 12; i++)
				{
					WriteToReportMacro("Param %u: \"%x\" ", i, Record->ExceptionInformation[i]);
				}
				WriteToReportMacro("\r\n");
			}
		}
	}
	WriteToReportMacro("Console Params: %s\r\n", Core.Params);
	WriteToReportMacro("\r\n");
#if PLATFORM == _WINDOWS
	using fnGetThreadDescription = HRESULT(*)(HANDLE hThread, PWSTR * ppszThreadDescription);
	fnGetThreadDescription instGetThreadDescription = nullptr;

	auto TryGetThreadNameLambda = [&instGetThreadDescription](HANDLE hThread, string64& OutThreadName)
	{
		if (instGetThreadDescription != nullptr)
		{
			string64 AnsiThreadName = { 0 };
			PWSTR pThreadName = nullptr;
			if (SUCCEEDED(instGetThreadDescription(hThread, &pThreadName)))
			{
				if (pThreadName != nullptr)
				{
					size_t StrLen = wcslen(pThreadName);
					WideCharToMultiByte(CP_OEMCP, 0, pThreadName, StrLen, OutThreadName, sizeof(OutThreadName), 0, 0);
				}
			}
		}
	};

	if (IsWindows10OrGreater())
	{
		HMODULE KernelLib = GetModuleHandle("kernel32.dll");
		instGetThreadDescription = (fnGetThreadDescription)GetProcAddress(KernelLib, "GetThreadDescription");
		if (instGetThreadDescription != nullptr)
		{
			string64 AnsiThreadName = { 0 };
			TryGetThreadNameLambda(GetCurrentThread(), AnsiThreadName);
			u32 LenThreadName = xr_strlen(AnsiThreadName);
			if (LenThreadName > 0)
			{
				WriteToReportMacro("Crashed thread name '%s'\r\n", AnsiThreadName);
			}
		}
	}
#endif

	WriteToReportMacro("Crashed thread id '%u'\r\n", GetCurrentThreadId());
	WriteToReportMacro("Crash thread stack\r\n");

	auto WriteStackInfoToReportLambda = [&WriteCursor](void** pStack, u16 framesNum)
	{
		for (u16 i = 0; i < framesNum; ++i)
		{
			string1024 SymbolInfo;
			DebugSymbols.ResolveFrame(pStack[i], SymbolInfo);
			WriteToReportMacro("%s\r\n", SymbolInfo);
		}
	};

	WriteStackInfoToReportLambda(pStack, framesNum);

	// get other's thread stacks
	constexpr size_t ThreadArraySize = 128;
	DWORD ThreadIDs[ThreadArraySize];
	auto GetThreadIDListLambda = [&ThreadIDs, ThreadArraySize]() -> DWORD
	{
		DWORD ThreadCount = 0;
		WinScopeHandle hThreadTraversal = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
		if (hThreadTraversal.IsValid())
		{
			THREADENTRY32 threadEntry;
			ZeroMemory(&threadEntry, sizeof(threadEntry));
			threadEntry.dwSize = sizeof(THREADENTRY32);
			if (!Thread32First(hThreadTraversal.Get(), &threadEntry))
			{
				return ThreadCount;
			}

			while (Thread32Next(hThreadTraversal, &threadEntry))
			{
				if (threadEntry.th32OwnerProcessID == GetCurrentProcessId())
				{
					ThreadIDs[ThreadCount++] = threadEntry.th32ThreadID;
					if (ThreadCount == ThreadArraySize)
					{
						// Thread list in process more then 128. 
						// Consider to increase the list
						if (IsDebuggerPresent())
						{
							DebugBreak();
						}
						return ThreadCount;
					}
				}
			}
		}

		return ThreadCount;
	};

	WriteToReportMacro("\r\n\r\n");

	DWORD AliveThreads = GetThreadIDListLambda();
	WriteToReportMacro("Total Threads: %u\r\n", AliveThreads);

	for (DWORD thrIndx = 0; thrIndx < AliveThreads; thrIndx++)
	{
		DWORD ThreadID = ThreadIDs[thrIndx];
		if (ThreadID == GetCurrentThreadId())
		{
			continue;
		}
		WinScopeHandle ThreadHandle = OpenThread(THREAD_ALL_ACCESS, FALSE, ThreadIDs[thrIndx]);
		if (ThreadHandle.IsValid())
		{
			if (instGetThreadDescription != nullptr)
			{
				string64 AnsiThreadName = { 0 };
				TryGetThreadNameLambda(ThreadHandle, AnsiThreadName);
				u32 LenThreadName = xr_strlen(AnsiThreadName);
				if (LenThreadName > 0)
				{
					WriteToReportMacro("Thread name '%s'\r\n", AnsiThreadName);
				}
			}
			WriteToReportMacro("Thread \"%u\"\r\n", ThreadIDs[thrIndx]);
			u16 ThreadStackSize = DebugSymbols.GetCallStack(ThreadHandle, &pStack[0], StackSize);
			WriteStackInfoToReportLambda(pStack, ThreadStackSize);
			WriteToReportMacro("\r\n\r\n", ThreadIDs[thrIndx]);
		}
	}

	//#TODO: Write lua stack

	//#TODO: Write Spectre stack

	sCrashReport.bCanContinue = bCanContinue;
	sCrashReport.CrashReport = &ReportMem[0];
	// show dialog
	// invoke separate process for crash dialog
	INT_PTR DialogResult = 0;
	PROCESS_INFORMATION CrashStackProcessInfo;
	STARTUPINFO cif;
	ZeroMemory(&cif, sizeof(STARTUPINFO));
	ZeroMemory(&CrashStackProcessInfo, sizeof(CrashStackProcessInfo));

	string_path CoreModulePath;
	GetModuleFileName(GetModuleHandle(NULL), CoreModulePath, sizeof(CoreModulePath));
	std::filesystem::path CoreModulePath2(CoreModulePath);
	std::filesystem::path binPathCleared = CoreModulePath2.parent_path();
	binPathCleared = binPathCleared / "CrashStack.exe";
	if (std::filesystem::exists(binPathCleared))
	{
		// save text info to external file
		string_path TempPath;
		GetTempPath(sizeof(TempPath), TempPath);
		xr_strcat(TempPath, "OxygenLastCrash.txt");

		HANDLE hTempInfoFile = CreateFile(TempPath, GENERIC_ALL, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hTempInfoFile == INVALID_HANDLE_VALUE)
		{
			goto Fallback;
		}

		u32 ReportSize = xr_strlen(&ReportMem[0]);
		DWORD bytesWritten = 0;
		WriteFile(hTempInfoFile, &ReportMem[0], ReportSize, &bytesWritten, 0);
		CloseHandle(hTempInfoFile);

		string_path CommandLine;
		ZeroMemory(CommandLine, sizeof(CommandLine));
		xr_strcat(CommandLine, "\"");
		xr_strcat(CommandLine, binPathCleared.string().c_str());
		xr_strcat(CommandLine, "\" ");
		xr_strcat(CommandLine, TempPath);

		if (!CreateProcess(binPathCleared.string().c_str(), CommandLine, NULL, NULL, FALSE, 0, nullptr, nullptr, &cif, &CrashStackProcessInfo))
		{
			goto Fallback;
		}
		else
		{
			WaitForSingleObject(CrashStackProcessInfo.hProcess, INFINITE);

			DWORD ExitCode = 0;
			GetExitCodeProcess(CrashStackProcessInfo.hProcess, &ExitCode);
			DialogResult = !ExitCode;
			CloseHandle(CrashStackProcessInfo.hThread);
			CloseHandle(CrashStackProcessInfo.hProcess);
		}
	}
	else
	{
		Fallback:
		// CrashStack.exe not found, fallback to internal dialog
		static HMODULE hCoreModule = GetModuleHandle("xrCore.dll");
		DialogResult = DialogBoxA(hCoreModule, MAKEINTRESOURCE(IDD_CRASH2), NULL, (DLGPROC)CrashDialogProc);
	}


#undef WriteToReportMacro
	// INT_PTR -> bool
	return !!((BOOL)DialogResult);
}

void xrDebug::error(long hr, const char* expr, const char *file, int line, const char *function)
{
	backend(error2string(hr), expr, nullptr, nullptr, file, line, function);
}

void xrDebug::error(long hr, const char* expr, const char* e2, const char *file, int line, const char *function)
{
	backend(error2string(hr), expr, e2, nullptr, file, line, function);
}

void xrDebug::fail(const char *e1, const char *file, int line, const char *function)
{
	backend("assertion failed", e1, nullptr, nullptr, file, line, function);
}

void xrDebug::fail(const char *e1, const char *e2, const char *file, int line, const char *function)
{
	backend(e1, e2, nullptr, nullptr, file, line, function);
}

void xrDebug::fail(const char *e1, const char *e2, const char *e3, const char *file, int line, const char *function)
{
	backend(e1, e2, e3, nullptr, file, line, function);
}

void xrDebug::fail(const char *e1, const char *e2, const char *e3, const char *e4, const char *file, int line, const char *function)
{
	backend(e1, e2, e3, e4, file, line, function);
}

void __cdecl xrDebug::fatal(const char *file, int line, const char *function, const char* F, ...)
{
	if (IsDebuggerPresent())
	{
		DebugBreak();
	}
	string1024	buffer;

	va_list		p;
	va_start(p, F);
	vsprintf(buffer, F, p);
	va_end(p);

	backend("Fatal error", "<no expression>", buffer, nullptr, file, line, function);
}

using full_memory_stats_callback_type = void(*) ();
XRCORE_API full_memory_stats_callback_type g_full_memory_stats_callback = nullptr;

int out_of_memory_handler(size_t size)
{
	if (g_full_memory_stats_callback)
		g_full_memory_stats_callback();
	else
	{
		Memory.mem_compact();

		u32					process_heap = mem_usage_impl();
		int					eco_strings = g_pStringContainer.stat_economy();
		int					eco_smem = g_pSharedMemoryContainer->stat_economy();
	    Msg("* [X-ray]: Process heap[%d K]", process_heap / 1024);
	    Msg("* [X-ray]: Economy: strings[%d K], smem[%d K]", eco_strings / 1024, eco_smem);
	}

	Debug.fatal(DEBUG_INFO, "Out of memory. Memory request: %d K", size / 1024);
	return					1;
}

XRCORE_API string_path g_bug_report_file;

using UnhandledExceptionFilterType = LONG WINAPI(struct _EXCEPTION_POINTERS *pExceptionInfo);

static UnhandledExceptionFilterType	*previous_filter = nullptr;

void format_message(char* buffer, const u32 &buffer_size)
{
	string512 SystemMessage = {0};
	DWORD error_code = GetLastError();

	if (!error_code) 
	{
		*buffer = 0;
		return;
	}

	if (!FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, error_code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), SystemMessage, sizeof(SystemMessage), nullptr))
	{
		xr_sprintf(buffer, buffer_size, "[error][%8d]    : %s", error_code, "FormatMessage fails to get error");
	}
	else
	{
		xr_sprintf(buffer, buffer_size, "[error][%8d]    : %s", error_code, SystemMessage);
	}
}

//////////////////////////////////////////////////////////////////////
using _PNH = int(__cdecl *)(size_t);

IC void handler_base(const char* reason_string)
{
	Debug.backend("Error handler is invoked!", reason_string, nullptr, nullptr, DEBUG_INFO);
}

static void invalid_parameter_handler(const wchar_t *expression, const wchar_t *function, const wchar_t *file, unsigned int line, uintptr_t reserved)
{
	string4096	expression_,
		function_,
		file_;

	size_t converted_chars = 0;

	if (expression)
		wcstombs_s(&converted_chars, expression_, sizeof(expression_), expression, (wcslen(expression) + 1) * 2 * sizeof(char));
	else
		xr_strcpy(expression_, "");

	if (function)
		wcstombs_s(&converted_chars, function_, sizeof(function_), function, (wcslen(function) + 1) * 2 * sizeof(char));
	else
		xr_strcpy(function_, __FUNCTION__);

	if (file)
		wcstombs_s(&converted_chars, file_, sizeof(file_), file, (wcslen(file) + 1) * 2 * sizeof(char));
	else 
	{
		line = __LINE__;
		xr_strcpy(file_, __FILE__);
	}

	Debug.backend("Error handler is invoked!", expression_, nullptr, nullptr, file_, line, function_);
}

IC void pure_call_handler()
{
	handler_base("Pure virtual function call");
}

#ifdef XRAY_USE_EXCEPTIONS
IC void unexpected_handler()
{
	handler_base("Unexpected program termination");
}
#endif // XRAY_USE_EXCEPTIONS

IC void abort_handler(int signal)
{
	handler_base("Application is aborting");
}

IC void floating_point_handler(int signal)
{
	handler_base("Floating point error");
}

IC void illegal_instruction_handler(int signal)
{
	if (!CPU::Info.hasFeature(CPUFeature::SSE42))
	{
		handler_base("SSE4.2 and AVX instructions isn't legal on your CPU");
	}
	else if (!CPU::Info.hasFeature(CPUFeature::AVX)) 
	{
		handler_base("AVX instructions isn't legal on your CPU");
	}
	else 
	{
		handler_base("Illegal instruction");
	}
}

IC void termination_handler(int signal)
{
	handler_base("Termination with exit code 3");
}

void debug_on_thread_spawn()
{
 	signal(SIGABRT, abort_handler);
	signal(SIGFPE, floating_point_handler);
	signal(SIGILL, illegal_instruction_handler);

	_set_invalid_parameter_handler(&invalid_parameter_handler);

	_set_new_mode(1);
	_set_new_handler(&out_of_memory_handler);

	_set_purecall_handler(&pure_call_handler);
	std::set_terminate(abort);
}

void xrDebug::_initialize()
{
	if (gMainThreadId == 0xFFFFFFFF)
	{
		gMainThreadId = GetCurrentThreadId();
		m_mainThreadId = gMainThreadId;
	}
	*g_bug_report_file = 0;
	debug_on_thread_spawn();
	previous_filter = ::SetUnhandledExceptionFilter(UnhandledFilter);	// exception handler to all "unhandled" exceptions
	DebugSymbols.Initialize();
}


void xrDebug::_initializeAfterFS()
{
    FS.update_path(DumpFilePath, "$dump$", "");
    if (!FS.path_exist(DumpFilePath))
    {
        createPath(DumpFilePath, false, true);
    }
}


// based on dbghelp.h
using MINIDUMPWRITEDUMP = BOOL(WINAPI*)(HANDLE hProcess, DWORD dwPid, HANDLE hFile, MINIDUMP_TYPE DumpType,
	CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
	CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
	CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam
	);

LONG WriteMinidump(struct _EXCEPTION_POINTERS* pExceptionInfo)
{
    // Flush, after crashhandler. We include log file in a minidump
	xrLogger::FlushLog();

    long retval = EXCEPTION_CONTINUE_SEARCH;
    bException = true;

    // firstly see if dbghelp.dll is around and has the function we need
    // look next to the EXE first, as the one in System32 might be old
    // (e.g. Windows 2000)
    HMODULE hDll = nullptr;
    string_path szDbgHelpPath;

    if (GetModuleFileName(nullptr, szDbgHelpPath, _MAX_PATH))
    {
        char* pSlash = strchr(szDbgHelpPath, '\\');
        if (pSlash)
        {
            const char dbgHelpStr[] = "DBGHELP.DLL";
            xr_strcpy(pSlash + 1, sizeof(dbgHelpStr), dbgHelpStr);
            hDll = LoadLibraryA(szDbgHelpPath);
        }
    }

    if (!hDll)
    {
        // load any version we can
        hDll = LoadLibraryA("DBGHELP.DLL");
    }

    LPCSTR szResult = nullptr;

    if (hDll)
    {
        MINIDUMPWRITEDUMP pDump = (MINIDUMPWRITEDUMP)::GetProcAddress(hDll, "MiniDumpWriteDump");
        if (pDump)
        {
			string512 ErrorString = { NULL };
			DWORD ErrorSysCode = NULL;
			DWORD ErrorStringSize = NULL;

            string_path szDumpPath = { 0 };
            string_path szFilename = { 0 };
            string_path szScratch = { 0 };
            string64 t_stemp = { 0 };

            // work out a good place for the dump file
            timestamp(t_stemp);
            xr_strcat(szDumpPath, DumpFilePath); // can be empty, if we crash before FS was initialized

            xr_strcat(szFilename, Core.ApplicationName);
            xr_strcat(szFilename, "_");
            xr_strcat(szFilename, Core.UserName);
            xr_strcat(szFilename, "_");
            xr_strcat(szFilename, t_stemp);
            xr_strcat(szFilename, ".mdmp");

            xr_strcat(szDumpPath, szFilename);

            // create the file
            HANDLE hFile = CreateFileA(szDumpPath, GENERIC_WRITE, FILE_SHARE_WRITE, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
            if (INVALID_HANDLE_VALUE == hFile)
            {
                // try to place into current directory
                ZeroMemory(szDumpPath, sizeof(szDumpPath));
                xr_strcat(szDumpPath, szFilename);
                hFile = CreateFileA(szDumpPath, GENERIC_WRITE, FILE_SHARE_WRITE, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
            }
            if (hFile != INVALID_HANDLE_VALUE)
            {
                _MINIDUMP_EXCEPTION_INFORMATION ExInfo;

                ExInfo.ThreadId = GetCurrentThreadId();
                ExInfo.ExceptionPointers = pExceptionInfo;
                ExInfo.ClientPointers = NULL;

                // write the dump
                MINIDUMP_TYPE dump_flags = MINIDUMP_TYPE(MiniDumpNormal | MiniDumpFilterMemory | MiniDumpScanMemory);

                //try include LogFile
                char* logFileContent = nullptr;
                DWORD logFileContentSize = 0;

                __try
                {
                    do
                    {
						const string_path& logFileName2 = xrLogger::GetLogPath();

                        // Don't use X-Ray FS - it can be corrupted at this point
                        HANDLE hLogFile = CreateFileA(logFileName2, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
                        if (hLogFile == INVALID_HANDLE_VALUE) break;

                        LARGE_INTEGER FileSize;
                        BOOL bResult = GetFileSizeEx(hLogFile, &FileSize);
                        if (!bResult)
                        {
                            CloseHandle(hLogFile);
                            break;
                        }

                        logFileContent = (char*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, FileSize.LowPart);
                        if (logFileContent == nullptr)
                        {
                            CloseHandle(hLogFile);
                            break;
                        }

                        logFileContentSize = FileSize.LowPart;

                        DWORD TotalBytesReaded = 0;

                        do
                        {
                            DWORD BytesReaded = 0;
                            bResult = ReadFile(hLogFile, logFileContent, FileSize.LowPart, &BytesReaded, nullptr);
                            if (!bResult)
                            {
                                CloseHandle(hLogFile);
                                break;
                            }
                            TotalBytesReaded += BytesReaded;
                        } while (TotalBytesReaded < FileSize.LowPart);

                        CloseHandle(hLogFile);
                    } while (false);
                }
                __except (EXCEPTION_EXECUTE_HANDLER)
                {
                    // better luck next time
                }

				MINIDUMP_USER_STREAM_INFORMATION UserStreamsInfo = { NULL };
                MINIDUMP_USER_STREAM LogFileUserStream = { NULL };

                if (logFileContent != nullptr)
                {
                    UserStreamsInfo.UserStreamCount = 1;
                    LogFileUserStream.Buffer = logFileContent;
                    LogFileUserStream.BufferSize = logFileContentSize;
                    LogFileUserStream.Type = MINIDUMP_STREAM_TYPE::CommentStreamA;
                    UserStreamsInfo.UserStreamArray = &LogFileUserStream;
                }

                BOOL bOK = pDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, dump_flags, &ExInfo, &UserStreamsInfo, nullptr);
                if (bOK)
                {
                    xr_sprintf(szScratch, "Saved dump file to '%s'", szDumpPath);
                    szResult = szScratch;
                    retval = EXCEPTION_EXECUTE_HANDLER;
                }
                else
                {
					ErrorSysCode = GetLastError();
					ErrorStringSize = FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, ErrorSysCode, 0, ErrorString, sizeof(ErrorString) - 1, nullptr);

					if (!!ErrorString && ErrorSysCode && ErrorStringSize)
					{
						xr_sprintf(szScratch, "Failed to save dump file to '%s' (error %d '%s')", szDumpPath, ErrorSysCode, ErrorString);
					}
					else
					{
						xr_sprintf(szScratch, "Failed to save dump file to '%s' (No system error)", szDumpPath);
					}
                    szResult = szScratch;

                }
                CloseHandle(hFile);
            }
            else
			{
				ErrorSysCode = GetLastError();
				ErrorStringSize = FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, ErrorSysCode, 0, ErrorString, sizeof(ErrorString) - 1, nullptr);

				if (!!ErrorString && ErrorSysCode && ErrorStringSize)
				{
					xr_sprintf(szScratch, "Failed to create dump file '%s' (error %d '%s')", szDumpPath, ErrorSysCode, ErrorString);
				}
				else
				{
					xr_sprintf(szScratch, "Failed to create dump file '%s' (No system error)", szDumpPath);
				}
				szResult = szScratch;
            }
        }
        else
        {
            szResult = "DBGHELP.DLL too old";
        }
    }
    else
    {
        szResult = "DBGHELP.DLL not found";
    }

    Log(szResult);
	xrLogger::FlushLog();

    return retval;
}

LONG WINAPI UnhandledFilter(struct _EXCEPTION_POINTERS* pExceptionInfo)
{
	Log("[FAIL] Type: UNHANDLED EXCEPTION");
	Log("[FAIL] DBG Ver: X-Ray Oxygen crash handler ver. 2.0f");
	Log("[FAIL] Report: To https://discord.gg/NAp6ZtX");

	crashhandler* pCrashHandler = Debug.get_crashhandler();
	if (pCrashHandler != nullptr)
	{
		pCrashHandler();
	}

	Debug.ShowCrashDialog(pExceptionInfo, false, nullptr);

    return WriteMinidump(pExceptionInfo);
}
