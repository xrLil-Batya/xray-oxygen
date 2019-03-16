// xrAI.cpp : Defines the entry point for the application.
#include "stdafx.h"
#include "process.h"

#include "../xrlc_light/xrlc_light.h"
#pragma comment(lib,"comctl32.lib")
#pragma comment(lib,"winmm.LIB")
#pragma comment(lib,"xrCDB.lib")
#pragma comment(lib,"xrCore.lib")
#pragma comment(lib,"xrLC_Light.lib")
#pragma comment(lib,"xrLCUtil.lib")

static const char* h_str = 
	"The following keys are supported / required:\n"
	"-? or -h	== this help\n"
	"-f<NAME>	== compile level in gamedata\\levels\\<NAME>\\\n"
	"-norgb		== disable common lightmap calculating\n"
	"-nosun		== disable sun-lighting\n"
	"-o			== modify build options\n"
	"\n"
	"NOTE: The last key is required for any functionality\n";

inline void Help()
{	
	MessageBox(0,h_str,"Command line options",MB_OK|MB_ICONINFORMATION);
}

#include "../xrInterface/cl_cast.hpp"
#include "../xrInterface/UIParams.hpp"

ILevelCompilerLogger& Logger = LevelCompilerLoggerWindow::instance();

CThread::LogFunc ProxyMsg = cdecl_cast([](const char* format, ...) 
{
	va_list args;
	va_start(args, format);
	Logger.clMsgV(format, args);
	va_end(args);
});

CThreadManager::ReportStatusFunc ProxyStatus = cdecl_cast([](const char* format, ...) 
{
	va_list args;
	va_start(args, format);
	Logger.StatusV(format, args);
	va_end(args);
});

CThreadManager::ReportProgressFunc ProxyProgress = cdecl_cast([](float progress) { Logger.Progress(progress); });

void WinShutdown()
{
	HANDLE hToken;
	TOKEN_PRIVILEGES tkp;

	// Get a token for this process. 
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
		return;

	// Get the LUID for the shutdown privilege. 
	LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);

	tkp.PrivilegeCount = 1;  // one privilege to set    
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	// Get the shutdown privilege for this process.
	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);

	if (GetLastError() != ERROR_SUCCESS)
		return;

	// Shut down the system and force all applications to close. 
	ExitWindowsEx(EWX_SHUTDOWN | EWX_FORCE, SHTDN_REASON_MAJOR_OPERATINGSYSTEM | SHTDN_REASON_MINOR_UPGRADE | SHTDN_REASON_FLAG_PLANNED);
}

void Startup(LPSTR lpCmdLine)
{
	char cmd[512], name[256];
	xr_strcpy(cmd, lpCmdLine);
	strlwr(cmd);

	if (strstr(cmd, "-?") || strstr(cmd, "-h") || strstr(cmd, "-f") == 0)
	{
		Help();
		return;
	}

	bool noRgb = (strstr(cmd, "-norgb"));
	bool noSun = (strstr(cmd, "-nosun"));

	// Load project
	name[0] = 0; 
	sscanf(strstr(cmd, "-f") + 2, "%s", name);

	// Give a LOG-thread a chance to startup
	string256 temp;
	xr_sprintf(temp, "%s - Detail Compiler", name);
	Logger.Initialize(temp);

	FS.get_path("$level$")->_set(name);

	CTimer dwStartupTime;
	dwStartupTime.Start();

	xrCompileDO(noRgb, noSun);

	// Show statistic
	char stats[256];
	xr_sprintf(stats, "Time elapsed: %s", make_time((dwStartupTime.GetElapsed_ms()) / 1000).c_str());

	if (!strstr(cmd, "-silent") && !pUIParams->isShutDown)
	{
		Logger.Success(stats);
	}

	delete pUIParams;
	Logger.Destroy();
	xrLogger::FlushLog();

	if (pUIParams->isShutDown)
		WinShutdown();
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// Initialize debugging
	Debug._initialize	();
	Core._initialize	("xrDO");
	Startup				(lpCmdLine);
	
	return 0;
}