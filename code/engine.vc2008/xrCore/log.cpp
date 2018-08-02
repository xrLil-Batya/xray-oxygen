#include "stdafx.h"
#pragma hdrstop

#include <time.h>
#include "resource.h"
#include "log.h"
#include "../FrayBuildConfig.hpp"
extern BOOL					LogExecCB		= TRUE;
static string_path			log_file_name;
static bool 				no_log			= true;
static std::recursive_mutex	logCS;

bool __declspec(dllexport) force_flush_log = false;	// alpet: выставить в true если лог все-же записывается плохо при вылете. 
//RvP													// Слишком частая запись лога вредит SSD и снижает производительность.
IWriter *LogWriter;
size_t cached_log = 0;

xr_vector<shared_str>*		LogFile			= NULL;
static LogCallback			LogCB			= 0;

inline const size_t FlushTreshold = 32768;

void FlushLog()
{
	try
	{
		if (!no_log) 
		{
			std::lock_guard<decltype(logCS)> lock(logCS);
			IWriter *f = FS.w_open(log_file_name);
			if (f)
			{
				for (u32 it = 0; it < LogFile->size(); it++) 
				{
					const char* s = *((*LogFile)[it]);
					f->w_string(s ? s : "");
				}
				FS.w_close(f);
			}
		}
	}
	catch (...)
	{
		;
	}
}

void AddOne(const char *split)
{
	if (!LogFile)
		return;

	std::lock_guard<decltype(logCS)> lock(logCS);

	OutputDebugString(split);
	OutputDebugString("\n");

	shared_str temp = shared_str(split);
	LogFile->push_back(temp);
	if (LogWriter)
	{
		switch (*split)
		{
		case 0x21:
		case 0x23:
		case 0x25:
			split++; // пропустить первый символ, т.к. это вероятно цветовой тег
			break;
		}

        string256 buf;
        SYSTEMTIME localTime;
        GetLocalTime(&localTime);
        int bufSize = xr_sprintf(buf, "[%hu.%hu.%hu %hu.%hu.%hu]", localTime.wDay, localTime.wMonth, localTime.wYear, localTime.wHour, localTime.wMinute, localTime.wSecond);

		sprintf_s(buf, 64, "[%s %s] ", Core.UserDate, Core.UserTime);
		LogWriter->w_printf("%s%s\r\n", buf, split);
		cached_log += bufSize;
		cached_log += xr_strlen(split) + 2;
        if (force_flush_log || cached_log >= FlushTreshold)
        {
			FlushLog();
            cached_log = 0;
        }

		if (LogExecCB&&LogCB)LogCB(split);
	}

	FlushLog();
}

void Log(const char *s) 
{
	int i,j;

	u32 length = xr_strlen( s );
	PSTR split = (PSTR)_alloca((length + 1) * sizeof(char));

	for (i = 0, j = 0; s[i] != 0; i++) 
	{
		if (s[i] == '\n') {
			split[j] = 0;	// end of line
			if (split[0] == 0) { split[0] = ' '; split[1] = 0; }
			AddOne(split);
			j = 0;
		}
		else 
		{
			split[j++] = s[i];
		}
	}
	split[j]=0;
	AddOne(split);
}

void __cdecl Msg(const char *format, ...)
{
	va_list		mark;
	string2048	buf;
	va_start	(mark, format );
	int sz		= _vsnprintf(buf, sizeof(buf)-1, format, mark ); 
	ULONG		bufSize = sizeof(buf) - 1;
	buf[bufSize]= 0;
    va_end		(mark);
	if (sz)		Log(buf);
}

void Log(const char *msg, const char *dop) 
{
	if (!dop)
	{
		Log(msg);
		return;
	}

	u32			buffer_size = (xr_strlen(msg) + 1 + xr_strlen(dop) + 1) * sizeof(char);
	PSTR buf	= (PSTR)_alloca( buffer_size );
	strconcat	(buffer_size, buf, msg, " ", dop);
	Log			(buf);
}

void Log(const char *msg, u32 dop) 
{
	u32			buffer_size = (xr_strlen(msg) + 1 + 10 + 1) * sizeof(char);
	PSTR buf	= (PSTR)_alloca( buffer_size );

	xr_sprintf	(buf, buffer_size, "%s %d", msg, dop);
	Log			(buf);
}

void Log(const char *msg, int dop)
{
	u32			buffer_size = (xr_strlen(msg) + 1 + 11 + 1) * sizeof(char);
	PSTR buf	= (PSTR)_alloca( buffer_size );

	xr_sprintf	(buf, buffer_size, "%s %i", msg, dop);
	Log			(buf);
}

void Log(const char* msg, const unsigned long long dop)
{
    const auto buffer_size = (std::strlen(msg) + 1 + 10 + 1) * sizeof(char);
    char* buf = static_cast<char*>(_alloca(buffer_size));

    xr_sprintf(buf, buffer_size, "%s %zu", msg, dop);
    Log(buf);
}
 
void Log				(const char *msg, float dop) {
	// actually, float string representation should be no more, than 40 characters,
	// but we will count with slight overhead
	u32			buffer_size = (xr_strlen(msg) + 1 + 64 + 1) * sizeof(char);
	PSTR buf	= (PSTR)_alloca( buffer_size );

	xr_sprintf	(buf, buffer_size, "%s %f", msg, dop);
	Log			(buf);
}

void Log				(const char *msg, const Fvector &dop) {
	u32			buffer_size = (xr_strlen(msg) + 2 + 3*(64 + 1) + 1) * sizeof(char);
	PSTR buf	= (PSTR)_alloca( buffer_size );

	xr_sprintf	(buf, buffer_size,"%s (%f,%f,%f)",msg, VPUSH(dop) );
	Log			(buf);
}

void Log				(const char *msg, const Fmatrix &dop)	{
	u32			buffer_size = (xr_strlen(msg) + 2 + 4*( 4*(64 + 1) + 1 ) + 1) * sizeof(char);
	PSTR buf	= (PSTR)_alloca( buffer_size );

	xr_sprintf	(buf, buffer_size,"%s:\n%f,%f,%f,%f\n%f,%f,%f,%f\n%f,%f,%f,%f\n%f,%f,%f,%f\n",
		msg,
		dop.i.x, dop.i.y, dop.i.z, dop._14_,
		dop.j.x, dop.j.y, dop.j.z, dop._24_,
		dop.k.x, dop.k.y, dop.k.z, dop._34_,
		dop.c.x, dop.c.y, dop.c.z, dop._44_
	);
	Log			(buf);
}

void LogWinErr			(const char *msg, long err_code)	{
	Msg					("%s: %s",msg,Debug.error2string(err_code)	);
}

LogCallback SetLogCB	(LogCallback cb)
{
	LogCallback	result	= LogCB;
	LogCB				= cb;
	return				(result);
}

const char* log_name			()
{
	return				(log_file_name);
}

void InitLog()
{
	R_ASSERT			(LogFile==NULL);
	LogFile				= new xr_vector<shared_str>();
	LogFile->reserve	(1000);
}

void CreateLog(BOOL nl)
{

    no_log = !!nl;
	strconcat(sizeof(log_file_name),	 log_file_name,	"[", Core.UserDate, Core.UserTime, "]", ".log");
	strconcat(sizeof(lua_log_file_name), lua_log_file_name, Core.ApplicationName, "_", Core.UserName, "_lua.log");

	if (FS.path_exist("$logs$"))
	{
		FS.update_path(log_file_name, "$logs$", log_file_name);
		FS.update_path(lua_log_file_name, "$logs$", lua_log_file_name);
	}
	if (!no_log)
	{
        IWriter *f = FS.w_open	(log_file_name);
        if (!f)
		{
        	MessageBox	(nullptr,"Can't create log file.","Error",MB_ICONERROR);
        	abort();
        }
        FS.w_close		(f);
    }
}

void CloseLog(void)
{
	FlushLog		();
 	LogFile->clear	();
	xr_delete		(LogFile);
}
typedef void (WINAPI *OFFSET_UPDATER)(const char* key, u32 ofs);
//LuaICP_API only
#pragma warning(disable: 4311 4302)
void LogXrayOffset(const char* key, LPVOID base, LPVOID pval)
{
	u32 ofs = (u32)pval - (u32)base;
	Msg("XRAY_OFFSET: %30s = 0x%x base = 0x%p, pval = 0x%p ", key, ofs, base, pval);
	static OFFSET_UPDATER cbUpdater = nullptr;
	HMODULE hDLL = GetModuleHandle("luaicp.dll");
	if (!cbUpdater && hDLL)
		cbUpdater = (OFFSET_UPDATER) GetProcAddress(hDLL, "UpdateXrayOffset");
	if (cbUpdater)
		cbUpdater(key, ofs);
}