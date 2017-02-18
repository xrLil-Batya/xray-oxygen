#include "stdafx.h"
#pragma hdrstop

#include "xrdebug.h"
#include "os_clipboard.h"

#include <sal.h>
//#include "dxerr.h"

#pragma warning(push)
#pragma warning(disable:4995)
#include <malloc.h>
#include <direct.h>
#pragma warning(pop)

extern bool shared_str_initialized;

#ifdef __BORLANDC__
    #	include "d3d9.h"
    #	include "d3dx9.h"
    #	include "D3DX_Wrapper.h"
    #	pragma comment(lib,"EToolsB.lib")
        static BOOL			bException	= TRUE;
#else
        static BOOL			bException	= FALSE;
#endif

#include <exception>
#include <new.h>							// for _set_new_mode
#include <signal.h>							// for signals

#ifndef _M_AMD64
#	define DEBUG_INVOKE	__asm int 3
#else
#	define DEBUG_INVOKE	DebugBreak()
#endif

#ifndef __BORLANDC__
#	pragma comment(lib,"dxerr2015.lib")
#endif


XRCORE_API	xrDebug		Debug;

static bool	error_after_dialog = false;

extern const char* BuildStackTrace();
extern char g_stackTrace[100][4096];
extern int	g_stackTraceCount;

void LogStackTrace	(LPCSTR header)
{
	bool ss_init = shared_str_initialized; // alpet: при некоторых сбоях это все-равно дает исключение в shared_str::doc
	shared_str_initialized = false;
	__try
	{
		Msg("%s", header);
		BuildStackTrace();

		for (int i = 1; i < g_stackTraceCount; ++i)
			Msg(" %s", g_stackTrace[i]);

		FlushLog();
	}
	__finally
	{
		shared_str_initialized = ss_init;
	}
}

void xrDebug::gather_info(const char *expression, const char *description, const char *argument0, const char *argument1, const char *file, int line, const char *function, LPSTR assertion_info, u32 const assertion_info_size)
{
	LPSTR				buffer_base = assertion_info;
	LPSTR				buffer = assertion_info;
	int assertion_size = (int)assertion_info_size;
	LPCSTR				endline = "\n";
	LPCSTR				prefix = "[error]";
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

	if (!IsDebuggerPresent() && !strstr(GetCommandLine(), "-no_call_stack_assert")) {
		if (shared_str_initialized)
			Msg("stack trace:\n");

		BuildStackTrace();

		for (int i = 2; i < g_stackTraceCount; ++i) {
			if (shared_str_initialized)
				Msg("%s", g_stackTrace[i]);

			os_clipboard::copy_to_clipboard(assertion_info);
		}
	}
}
void xrDebug::do_exit	(const std::string &message)
{
	FlushLog			();
	MessageBox			(NULL,message.c_str(),"Error",MB_OK|MB_ICONERROR|MB_SYSTEMMODAL);
	TerminateProcess	(GetCurrentProcess(),1);
}

void xrDebug::backend	(const char *expression, const char *description, const char *argument0, const char *argument1, const char *file, int line, const char *function, bool &ignore_always)
{
	static std::recursive_mutex CS;
    std::lock_guard<decltype(CS)> lock(CS);

	error_after_dialog	= true;

	string4096			assertion_info;

	gather_info			(expression, description, argument0, argument1, file, line, function, assertion_info, sizeof(assertion_info) );

	if (handler)
		handler			();

	if (get_on_dialog())
		get_on_dialog()	(true);

	FlushLog			();

#ifdef XRCORE_STATIC
	MessageBox			(NULL,assertion_info,"X-Ray error",MB_OK|MB_ICONERROR|MB_SYSTEMMODAL);
#endif
	DEBUG_INVOKE;

	if (get_on_dialog())
		get_on_dialog()	(false);
}

LPCSTR xrDebug::error2string	(long code)
{
	LPCSTR				result	= 0;
	static	string1024	desc_storage;

	if (0==result)
	{
		FormatMessage	(FORMAT_MESSAGE_FROM_SYSTEM,0,code,0,desc_storage,sizeof(desc_storage)-1,0);
		result			= desc_storage;
	}
	return		result	;
}

void xrDebug::error		(long hr, const char* expr, const char *file, int line, const char *function, bool &ignore_always)
{
	backend		(error2string(hr),expr,0,0,file,line,function,ignore_always);
}

void xrDebug::error		(long hr, const char* expr, const char* e2, const char *file, int line, const char *function, bool &ignore_always)
{
	backend		(error2string(hr),expr,e2,0,file,line,function,ignore_always);
}

void xrDebug::fail		(const char *e1, const char *file, int line, const char *function, bool &ignore_always)
{
	backend		("assertion failed",e1,0,0,file,line,function,ignore_always);
}

void xrDebug::fail		(const char *e1, const std::string &e2, const char *file, int line, const char *function, bool &ignore_always)
{
	backend		(e1,e2.c_str(),0,0,file,line,function,ignore_always);
}

void xrDebug::fail		(const char *e1, const char *e2, const char *file, int line, const char *function, bool &ignore_always)
{
	backend		(e1,e2,0,0,file,line,function,ignore_always);
}

void xrDebug::fail		(const char *e1, const char *e2, const char *e3, const char *file, int line, const char *function, bool &ignore_always)
{
	backend		(e1,e2,e3,0,file,line,function,ignore_always);
}

void xrDebug::fail		(const char *e1, const char *e2, const char *e3, const char *e4, const char *file, int line, const char *function, bool &ignore_always)
{
	backend		(e1,e2,e3,e4,file,line,function,ignore_always);
}

void __cdecl xrDebug::fatal(const char *file, int line, const char *function, const char* F,...)
{
	string1024	buffer;

	va_list		p;
	va_start	(p,F);
	vsprintf	(buffer,F,p);
	va_end		(p);

	bool		ignore_always = true;

	backend		("fatal error","<no expression>",buffer,0,file,line,function,ignore_always);
}

typedef void (*full_memory_stats_callback_type) ( );
XRCORE_API full_memory_stats_callback_type g_full_memory_stats_callback = 0;

int out_of_memory_handler	(size_t size)
{
	if ( g_full_memory_stats_callback )
		g_full_memory_stats_callback	( );
	else {
		Memory.mem_compact	();
#ifndef _EDITOR
		u32					crt_heap		= mem_usage_impl((HANDLE)_get_heap_handle(),0,0);
#else // _EDITOR
		u32					crt_heap		= 0;
#endif // _EDITOR
		u32					process_heap	= mem_usage_impl(GetProcessHeap(),0,0);
		int					eco_strings		= (int)g_pStringContainer->stat_economy			();
		int					eco_smem		= (int)g_pSharedMemoryContainer->stat_economy	();
		Msg					("* [x-ray]: crt heap[%d K], process heap[%d K]",crt_heap/1024,process_heap/1024);
		Msg					("* [x-ray]: economy: strings[%d K], smem[%d K]",eco_strings/1024,eco_smem);
	}

	Debug.fatal				(DEBUG_INFO,"Out of memory. Memory request: %d K",size/1024);
	return					1;
}

extern LPCSTR log_name();

XRCORE_API string_path g_bug_report_file;

void CALLBACK PreErrorHandler	(INT_PTR)
{
}


extern void BuildStackTrace(struct _EXCEPTION_POINTERS *pExceptionInfo);
typedef LONG WINAPI UnhandledExceptionFilterType(struct _EXCEPTION_POINTERS *pExceptionInfo);
typedef LONG ( __stdcall *PFNCHFILTFN ) ( EXCEPTION_POINTERS * pExPtrs ) ;
extern "C" BOOL __stdcall SetCrashHandlerFilter ( PFNCHFILTFN pFn );

static UnhandledExceptionFilterType	*previous_filter = 0;

void format_message	(LPSTR buffer, const u32 &buffer_size)
{
    LPVOID		message;
    DWORD		error_code = GetLastError(); 

	if (!error_code) {
		*buffer	= 0;
		return;
	}

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM,
        NULL,
        error_code,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPSTR)&message,
        0,
		NULL
	);

	xr_sprintf	(buffer,buffer_size,"[error][%8d]    : %s",error_code,message);
    LocalFree	(message);
}

#ifndef _EDITOR
    #include <errorrep.h>
    #pragma comment( lib, "faultrep.lib" )
#endif

LONG WINAPI UnhandledFilter	(_EXCEPTION_POINTERS *pExceptionInfo)
{
	string256				error_message;
	format_message			(error_message,sizeof(error_message));

	if (!error_after_dialog && !strstr(GetCommandLine(),"-no_call_stack_assert")) {
		CONTEXT				save = *pExceptionInfo->ContextRecord;
		BuildStackTrace		(pExceptionInfo);
		*pExceptionInfo->ContextRecord = save;

		if (shared_str_initialized)
			Msg				("stack trace:\n");

		if (!IsDebuggerPresent())
		{
			os_clipboard::copy_to_clipboard	("stack trace:\r\n\r\n");
		}

		string4096			buffer;
		for (int i=0; i<g_stackTraceCount; ++i) {
			if (shared_str_initialized)
				Msg			("%s",g_stackTrace[i]);
			xr_sprintf			(buffer, sizeof(buffer), "%s\r\n",g_stackTrace[i]);
#ifdef DEBUG
			if (!IsDebuggerPresent())
				os_clipboard::update_clipboard(buffer);
#endif // #ifdef DEBUG
		}

		if (*error_message) {
			if (shared_str_initialized)
				Msg			("\n%s",error_message);

			xr_strcat			(error_message,sizeof(error_message),"\r\n");
#ifdef DEBUG
			if (!IsDebuggerPresent())
				os_clipboard::update_clipboard(buffer);
#endif // #ifdef DEBUG
		}
	}

	if (shared_str_initialized)
		FlushLog			();

#ifndef _EDITOR
	ReportFault				( pExceptionInfo, 0 );
#endif

	if (!previous_filter)
		return EXCEPTION_CONTINUE_SEARCH;
	

	previous_filter			(pExceptionInfo);

	return					(EXCEPTION_CONTINUE_SEARCH) ;
}

//////////////////////////////////////////////////////////////////////
#ifdef M_BORLAND
	namespace std{
		extern new_handler _RTLENTRY _EXPFUNC set_new_handler( new_handler new_p );
	};

	static void __cdecl def_new_handler() 
    {
		FATAL		("Out of memory.");
    }

    void	xrDebug::_initialize		(const bool &dedicated)
    {
		handler							= 0;
		m_on_dialog						= 0;
        std::set_new_handler			(def_new_handler);	// exception-handler for 'out of memory' condition
    }
#else
typedef int(__cdecl * _PNH)(size_t);

static void handler_base(LPCSTR reason_string)
{
	bool							ignore_always = false;
	Debug.backend(
		"error handler is invoked!",
		reason_string,
		0,
		0,
		DEBUG_INFO,
		ignore_always
	);
}

static void invalid_parameter_handler(
	const wchar_t *expression,
	const wchar_t *function,
	const wchar_t *file,
	unsigned int line,
	uintptr_t reserved
)
{
	bool		ignore_always = false;

	string4096	expression_,
				function_,
				file_;

	size_t		converted_chars = 0;

	if (expression)
		wcstombs_s(
			&converted_chars,
			expression_,
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

static void pure_call_handler()
{
	handler_base("pure virtual function call");
}

#ifdef XRAY_USE_EXCEPTIONS
static void unexpected_handler()
{
	handler_base("unexpected program termination");
}
#endif // XRAY_USE_EXCEPTIONS

static void abort_handler(int signal)
{
	handler_base("application is aborting");
}

static void floating_point_handler(int signal)
{
	handler_base("floating point error");
}

static void illegal_instruction_handler(int signal)
{
	handler_base("illegal instruction");
}


static void termination_handler(int signal)
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

void	xrDebug::_initialize(const bool &dedicated)
{
	static bool is_dedicated = dedicated;

	*g_bug_report_file = 0;

	debug_on_thread_spawn();

	previous_filter = ::SetUnhandledExceptionFilter(UnhandledFilter);	// exception handler to all "unhandled" exceptions

}
#endif
