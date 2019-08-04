#include "stdafx.h"
#pragma hdrstop

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
#include <intrin.h>		// for __debugbreak
#include "cpuid.h"
#include <DbgHelp.h>

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

void xrDebug::backend(const char* expression, const char* description, const char* argument0, const char* argument1, const char* file, int line, const char* function, bool &ignore_always)
{
	xrCriticalSectionGuard guard(Lock);
	error_after_dialog = true;

	string4096 assertion_info;

	gather_info(expression, description, argument0, argument1, file, line, function, assertion_info, sizeof(assertion_info));

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


//#if !defined(DEBUG) && !defined(MIXED_NEW)
//	do_exit(gameWindow, assertion_info);
//#else
	//#GIPERION: Don't crash on DEBUG, we have some VERIFY that sometimes failed, but it's not so critical
	//[FX]: Why release don't have skip option? 
	do_exit2(gameWindow, assertion_info, ignore_always);
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


void xrDebug::do_exit2(HWND hwnd, const string4096& message, bool& ignore_always)
{
    int MsgRet = MessageBox(hwnd, message, "Error", MB_ABORTRETRYIGNORE | MB_ICONERROR);

    switch (MsgRet)
    {
    case IDABORT:
		DebugBreak();
        ExitProcess(1);
        break;
    case IDIGNORE:
        ignore_always = true;
        break;
    case IDRETRY:
    default:
		DebugBreak();
        break;
    }
}

void xrDebug::error(long hr, const char* expr, const char *file, int line, const char *function, bool &ignore_always)
{
	backend(error2string(hr), expr, nullptr, nullptr, file, line, function, ignore_always);
}

void xrDebug::error(long hr, const char* expr, const char* e2, const char *file, int line, const char *function, bool &ignore_always)
{
	backend(error2string(hr), expr, e2, nullptr, file, line, function, ignore_always);
}

void xrDebug::fail(const char *e1, const char *file, int line, const char *function, bool &ignore_always)
{
	backend("assertion failed", e1, nullptr, nullptr, file, line, function, ignore_always);
}

void xrDebug::fail(const char *e1, const char *e2, const char *file, int line, const char *function, bool &ignore_always)
{
	backend(e1, e2, nullptr, nullptr, file, line, function, ignore_always);
}

void xrDebug::fail(const char *e1, const char *e2, const char *e3, const char *file, int line, const char *function, bool &ignore_always)
{
	backend(e1, e2, e3, nullptr, file, line, function, ignore_always);
}

void xrDebug::fail(const char *e1, const char *e2, const char *e3, const char *e4, const char *file, int line, const char *function, bool &ignore_always)
{
	backend(e1, e2, e3, e4, file, line, function, ignore_always);
}

void __cdecl xrDebug::fatal(const char *file, int line, const char *function, const char* F, ...)
{
	string1024	buffer;

	va_list		p;
	va_start(p, F);
	vsprintf(buffer, F, p);
	va_end(p);

	bool		ignore_always = true;

	backend("Fatal error", "<no expression>", buffer, nullptr, file, line, function, ignore_always);
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
	ClipCursor(NULL);
    if (crashhandler* handlerFuncPtr = Debug.get_crashhandler())
    {
        handlerFuncPtr();
    }
	bool alw_ignored = false;
    if (!IsDebuggerPresent())
    {
        WriteMinidump(nullptr);
    }
	Debug.backend("Error handler is invoked!", reason_string, nullptr, nullptr, DEBUG_INFO, alw_ignored);
}

static void invalid_parameter_handler(const wchar_t *expression, const wchar_t *function, const wchar_t *file, unsigned int line, uintptr_t reserved)
{
	bool ignore_always = false;

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

	Debug.backend("Error handler is invoked!", expression_, nullptr, nullptr, file_, line, function_, ignore_always);
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
	Log("[FAIL] DBG Ver: X-Ray Oxygen crash handler ver. 1.3f");
	Log("[FAIL] Report: To https://discord.gg/NAp6ZtX");

	crashhandler* pCrashHandler = Debug.get_crashhandler();
	if (pCrashHandler != nullptr)
	{
		pCrashHandler();
	}

    return WriteMinidump(pExceptionInfo);
}
