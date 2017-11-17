#include "stdafx.h"
#pragma hdrstop

#include "xrdebug.h"
#include "os_clipboard.h"
#include "DebugCore\StackTrace.hpp"

#pragma warning(push)
#pragma warning(disable:4995)
#include <malloc.h>
#include <direct.h>
#pragma warning(pop)

#include <exception>
#include <new.h>							// for _set_new_mode
#include <signal.h>							// for signals
#include <sal.h>
#include <intrin.h> // for __debugbreak

#define DEBUG_INVOKE	__debugbreak()
#pragma comment(lib,"dxerr2015.lib")

XRCORE_API	xrDebug		Debug;

static bool	error_after_dialog = false;
//extern char g_stackTrace[100][4096];
extern int	g_stackTraceCount;
extern bool shared_str_initialized;
static bool bException = false;

#include "BuildStackTraceInline.hpp"

void LogStackTrace(const char* header)
{
	if (!shared_str_initialized)
		return;

	StackTrace.Count = BuildStackTrace(StackTrace.Frames, StackTrace.Capacity, StackTrace.LineCapacity);
	Msg("%s", header);
	for (size_t i = 1; i < StackTrace.Count; i++)
		Msg("%s", StackTrace[i]);
}

void xrDebug::gather_info(const char *expression, const char *description, const char *argument0, const char *argument1, const char *file, int line, const char *function, char* assertion_info, u32 const assertion_info_size)
{
	char*				buffer_base = assertion_info;
	char*				buffer = assertion_info;
	int assertion_size = (int)assertion_info_size;
	const char*				endline = "\n";
	const char*				prefix = "[error]";
	bool				extended_description = (description && !argument0 && strchr(description, '\n'));
	for (int i = 0; i < 2; ++i) {
		if (!i)
			buffer += xr_sprintf(buffer, assertion_size - u32(buffer - buffer_base), "%sFATAL ERROR%s%s", endline, endline, endline);
		buffer += xr_sprintf(buffer, assertion_size - u32(buffer - buffer_base), "%sExpression    : %s%s", prefix, expression, endline);
		buffer += xr_sprintf(buffer, assertion_size - u32(buffer - buffer_base), "%sFunction      : %s%s", prefix, function, endline);
		buffer += xr_sprintf(buffer, assertion_size - u32(buffer - buffer_base), "%sFile          : %s%s", prefix, file, endline);
		buffer += xr_sprintf(buffer, assertion_size - u32(buffer - buffer_base), "%sLine          : %d%s", prefix, line, endline);

		if (extended_description) {
			buffer += xr_sprintf(buffer, assertion_size - u32(buffer - buffer_base), "%s%s%s", endline, description, endline);
			if (argument0) {
				if (argument1) {
					buffer += xr_sprintf(buffer, assertion_size - u32(buffer - buffer_base), "%s%s", argument0, endline);
					buffer += xr_sprintf(buffer, assertion_size - u32(buffer - buffer_base), "%s%s", argument1, endline);
				}
				else
					buffer += xr_sprintf(buffer, assertion_size - u32(buffer - buffer_base), "%s%s", argument0, endline);
			}
		}
		else {
			buffer += xr_sprintf(buffer, assertion_size - u32(buffer - buffer_base), "%sDescription   : %s%s", prefix, description, endline);
			if (argument0) {
				if (argument1) {
					buffer += xr_sprintf(buffer, assertion_size - u32(buffer - buffer_base), "%sArgument 0    : %s%s", prefix, argument0, endline);
					buffer += xr_sprintf(buffer, assertion_size - u32(buffer - buffer_base), "%sArgument 1    : %s%s", prefix, argument1, endline);
				}
				else
					buffer += xr_sprintf(buffer, assertion_size - u32(buffer - buffer_base), "%sArguments     : %s%s", prefix, argument0, endline);
			}
		}

		buffer += xr_sprintf(buffer, assertion_size - u32(buffer - buffer_base), "%s", endline);
		if (!i) {
			if (shared_str_initialized) {
				Msg("%s", assertion_info);
				FlushLog();
			}
			buffer = assertion_info;
			endline = "\r\n";
			prefix = "";
		}
	}

#ifdef USE_MEMORY_MONITOR
	memory_monitor::flush_each_time(true);
	memory_monitor::flush_each_time(false);
#endif // USE_MEMORY_MONITOR

	if (IsDebuggerPresent() || !strstr(GetCommandLine(), "-no_call_stack_assert"))
		return;
	if (shared_str_initialized)
		Log("stack trace:\n");

	//BuildStackTrace();
	BuildStackTrace(StackTrace.Frames, StackTrace.Capacity, StackTrace.LineCapacity);

	for (size_t i = 2; i < StackTrace.Count; i++)
	{
		if (shared_str_initialized)
			Log(StackTrace[i]);
	}
	if (shared_str_initialized)
		FlushLog();

	os_clipboard::copy_to_clipboard(assertion_info);
}

void xrDebug::do_exit(const std::string &message)
{
	FlushLog();
	MessageBox(nullptr, message.c_str(), "Error", MB_OK | MB_ICONERROR | MB_SYSTEMMODAL);

	DEBUG_INVOKE;

	TerminateProcess(GetCurrentProcess(), 1);
}

void xrDebug::backend(const char *expression, const char *description, const char *argument0, const char *argument1, const char *file, int line, const char *function, bool &ignore_always)
{
	static std::recursive_mutex CS;
	std::lock_guard<decltype(CS)> lock(CS);

	error_after_dialog = true;

	string4096			assertion_info;

	gather_info(expression, description, argument0, argument1, file, line, function, assertion_info, sizeof(assertion_info));

	if (handler)
		handler();

	HWND wnd = GetActiveWindow();
	if (!wnd) wnd = GetForegroundWindow();

	ShowWindow(wnd, SW_MINIMIZE);
	while (ShowCursor(TRUE) < 0);

#if !defined(DEBUG) && !defined(MIXED_NEW)
	do_exit("Please, see log-file for details.");
#else
	//#GIPERION: Don't crash on DEBUG, we have some VERIFY that sometimes failed, but it's not so critical

	//THIS IS FINE!
	DebugBreak();
#endif
}

const char* xrDebug::error2string(long code)
{
	const char*				result = 0;
	static	string1024	desc_storage;

	if (!result)
	{
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, code, 0, desc_storage, sizeof(desc_storage) - 1, 0);
		result = desc_storage;
	}
	return result;
}

void xrDebug::error(long hr, const char* expr, const char *file, int line, const char *function, bool &ignore_always)
{
	backend(error2string(hr), expr, 0, 0, file, line, function, ignore_always);
}

void xrDebug::error(long hr, const char* expr, const char* e2, const char *file, int line, const char *function, bool &ignore_always)
{
	backend(error2string(hr), expr, e2, 0, file, line, function, ignore_always);
}

void xrDebug::fail(const char *e1, const char *file, int line, const char *function, bool &ignore_always)
{
	backend("assertion failed", e1, 0, 0, file, line, function, ignore_always);
}

void xrDebug::fail(const char *e1, const std::string &e2, const char *file, int line, const char *function, bool &ignore_always)
{
	backend(e1, e2.c_str(), 0, 0, file, line, function, ignore_always);
}

void xrDebug::fail(const char *e1, const char *e2, const char *file, int line, const char *function, bool &ignore_always)
{
	backend(e1, e2, 0, 0, file, line, function, ignore_always);
}

void xrDebug::fail(const char *e1, const char *e2, const char *e3, const char *file, int line, const char *function, bool &ignore_always)
{
	backend(e1, e2, e3, 0, file, line, function, ignore_always);
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

	backend("fatal error", "<no expression>", buffer, 0, file, line, function, ignore_always);
}

typedef void(*full_memory_stats_callback_type) ();
XRCORE_API full_memory_stats_callback_type g_full_memory_stats_callback = 0;

int out_of_memory_handler(size_t size)
{
	if (g_full_memory_stats_callback)
		g_full_memory_stats_callback();
	else
	{
		Memory.mem_compact();

		u32					process_heap = mem_usage_impl();
		int					eco_strings = g_pStringContainer->stat_economy();
		int					eco_smem = g_pSharedMemoryContainer->stat_economy();
		Msg("* [x-ray]: process heap[%d K]", process_heap / 1024);
		Msg("* [x-ray]: economy: strings[%d K], smem[%d K]", eco_strings / 1024, eco_smem);
	}

	Debug.fatal(DEBUG_INFO, "Out of memory. Memory request: %d K", size / 1024);
	return					1;
}

extern const char* log_name();

XRCORE_API string_path g_bug_report_file;

void CALLBACK PreErrorHandler(INT_PTR)
{
}


extern void BuildStackTrace(struct _EXCEPTION_POINTERS *pExceptionInfo);
typedef LONG WINAPI UnhandledExceptionFilterType(struct _EXCEPTION_POINTERS *pExceptionInfo);
typedef LONG(__stdcall *PFNCHFILTFN) (EXCEPTION_POINTERS * pExPtrs);

static UnhandledExceptionFilterType	*previous_filter = 0;

void format_message(char* buffer, const u32 &buffer_size)
{
	char*		message = nullptr;
	DWORD		error_code = GetLastError();

	if (!error_code) {
		*buffer = 0;
		return;
	}

	FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, 0, error_code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), message, 0, 0);

	xr_sprintf(buffer, buffer_size, "[error][%8d]    : %s", error_code, message);
	LocalFree(message);
}

#ifndef _EDITOR
#include <errorrep.h>
#pragma comment( lib, "faultrep.lib" )
#endif

LONG WINAPI UnhandledFilter(_EXCEPTION_POINTERS *pExceptionInfo)
{
	string256				error_message;
	format_message(error_message, sizeof(error_message));

	if (!error_after_dialog && !strstr(GetCommandLine(), "-no_call_stack_assert")) {
		CONTEXT				save = *pExceptionInfo->ContextRecord;
		StackTrace.Count = BuildStackTrace(pExceptionInfo, StackTrace.Frames, StackTrace.Capacity, StackTrace.LineCapacity);
		*pExceptionInfo->ContextRecord = save;

		if (shared_str_initialized)
			Msg("stack trace:\n");

		if (!IsDebuggerPresent())
			os_clipboard::copy_to_clipboard("stack trace:\r\n\r\n");

		string4096			buffer;
		for (size_t i = 0; i < StackTrace.Count; i++)
		{
			if (shared_str_initialized)
				Msg("%s", StackTrace[i]);
			xr_sprintf(buffer, sizeof(buffer), "%s\r\n", StackTrace[i]);
#ifdef DEBUG
			if (!IsDebuggerPresent())
				os_clipboard::update_clipboard(buffer);
#endif // #ifdef DEBUG
		}

		if (*error_message) {
			if (shared_str_initialized)
				Msg("\n%s", error_message);

			xr_strcat(error_message, sizeof(error_message), "\r\n");
#ifdef DEBUG
			if (!IsDebuggerPresent())
				os_clipboard::update_clipboard(buffer);
#endif // #ifdef DEBUG
		}
	}

	if (shared_str_initialized)
		FlushLog();

	ReportFault(pExceptionInfo, 0);
	
	return EXCEPTION_EXECUTE_HANDLER;
}

//////////////////////////////////////////////////////////////////////
typedef int(__cdecl * _PNH)(size_t);

IC void handler_base(const char* reason_string)
{
	bool alw_ignored = false;
	Debug.backend("error handler is invoked!", reason_string, 0, 0, DEBUG_INFO, alw_ignored);
}

static void invalid_parameter_handler(const wchar_t *expression, const wchar_t *function, const wchar_t *file, unsigned int line, uintptr_t reserved
)
{
	bool		ignore_always = false;

	string4096	expression_,
		function_,
		file_;

	size_t		converted_chars = 0;

	if (expression)
		wcstombs_s(&converted_chars, expression_,
			sizeof(expression_),
			expression,
			(wcslen(expression) + 1) * 2 * sizeof(char)
		);
	else
		xr_strcpy(expression_, "");

	if (function)
		wcstombs_s(&converted_chars, function_, sizeof(function_), function, (wcslen(function) + 1) * 2 * sizeof(char));
	else
		xr_strcpy(function_, __FUNCTION__);

	if (file)
		wcstombs_s(&converted_chars, file_, sizeof(file_), file, (wcslen(file) + 1) * 2 * sizeof(char));
	else {
		line = __LINE__;
		xr_strcpy(file_, __FILE__);
	}

	Debug.backend("error handler is invoked!", expression_, 0, 0, file_, line, function_, ignore_always);
}

IC void pure_call_handler()
{
	handler_base("pure virtual function call");
}

#ifdef XRAY_USE_EXCEPTIONS
IC void unexpected_handler()
{
	handler_base("unexpected program termination");
}
#endif // XRAY_USE_EXCEPTIONS

IC void abort_handler(int signal)
{
	handler_base("application is aborting");
}

IC void floating_point_handler(int signal)
{
	handler_base("floating point error");
}

IC void illegal_instruction_handler(int signal)
{
	handler_base("illegal instruction");
}


IC void termination_handler(int signal)
{
	handler_base("termination with exit code 3");
}

void debug_on_thread_spawn()
{
	//::set_terminate(_terminate);

	_set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);
	signal(SIGABRT, abort_handler);
	signal(SIGABRT_COMPAT, abort_handler);
	signal(SIGFPE, floating_point_handler);
	signal(SIGILL, illegal_instruction_handler);
	signal(SIGINT, 0);
	signal(SIGTERM, termination_handler);

	_set_invalid_parameter_handler(&invalid_parameter_handler);

	_set_new_mode(1);
	_set_new_handler(&out_of_memory_handler);

	_set_purecall_handler(&pure_call_handler);
}

void xrDebug::_initialize(const bool &dedicated)
{
	//static bool is_dedicated = dedicated;
	*g_bug_report_file = 0;
	debug_on_thread_spawn();
	previous_filter = ::SetUnhandledExceptionFilter(UnhandledFilter);	// exception handler to all "unhandled" exceptions
}