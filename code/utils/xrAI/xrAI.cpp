// xrAI.cpp : Defines the entry point for the application.
#include "stdafx.h"
#include "../../xrcore/xr_ini.h"
#include "../xrInterface/UIParams.hpp"
#include "process.h"
#include "xrAI.h"

#include "xr_graph_merge.h"
#include "game_spawn_constructor.h"
#include "xrCrossTable.h"
#include "game_graph_builder.h"
#include <mmsystem.h>
#include "spawn_patcher.h"

#pragma comment(lib,"comctl32.lib")
#pragma comment(lib,"d3dx9.lib")
#pragma comment(lib,"IMAGEHLP.LIB")
#pragma comment(lib,"winmm.LIB")
#pragma comment(lib,"xrcdb.LIB")
#pragma comment(lib,"MagicFM.LIB")
#pragma comment(lib,"xrCore.LIB")

extern LPCSTR LEVEL_GRAPH_NAME;
LevelCompilerLoggerWindow &Logger = LevelCompilerLoggerWindow::instance();

extern void	xrCompiler			(LPCSTR name, bool draft_mode, bool pure_covers, LPCSTR out_name);
extern void	verify_level_graph(LPCSTR name, bool verbose);

static const char* h_str = 
	"The following keys are supported / required:\n"
	"-? or -h   == this help\n"
	"-f<NAME>   == compile level in gamedata/levels/<NAME>/\n"
	"-o         == modify build options\n"
	"-s         == build game spawn data\n"
	"\n"
	"NOTE: The last key is required for any functionality\n";

void Help()
{	MessageBox(0,h_str,"Command line options",MB_OK|MB_ICONINFORMATION); }

string_path INI_FILE;
extern LPCSTR GAME_CONFIG;
void xrDisplay();

extern void clear_temp_folder	();
bool isVerify = false;

void execute(LPSTR cmd)
{
	// Load project
	string4096 name;
	name[0] = 0;
	if (strstr(cmd, "-f"))
	{
		sscanf(strstr(cmd, "-f") + 2, "%s", name);
	}
	else if (strstr(cmd, "-s"))
	{
		sscanf(strstr(cmd, "-s") + 2, "%s", name);
	}
	else if (strstr(cmd, "-t"))
	{
		sscanf(strstr(cmd, "-t") + 2, "%s", name);
	}
	else if (strstr(cmd, "-verify"))
	{
		sscanf(strstr(cmd, "-verify") + xr_strlen("-verify"), "%s", name);
		isVerify = true;
	}

	if (xr_strlen(name))
		xr_strcat(name, "\\");

	string_path			prjName;
	prjName[0] = 0;
	bool				can_use_name = false;
	if (xr_strlen(name) < sizeof(string_path)) {
		can_use_name = true;
		FS.update_path(prjName, "$game_levels$", name);
	}

	FS.update_path(INI_FILE, "$game_config$", GAME_CONFIG);

	if (strstr(cmd, "-f")) {
		R_ASSERT3(can_use_name, "Too big level name", name);

		char			*output = strstr(cmd, "-out");
		string256		temp0;
		if (output) {
			output += xr_strlen("-out");
			sscanf(output, "%s", temp0);
			_TrimLeft(temp0);
			output = temp0;
		}
		else
			output = (pstr)LEVEL_GRAPH_NAME;

		xrCompiler(prjName, !!strstr(cmd, "-draft"), !!strstr(cmd, "-pure_covers"), output);
	}
	else {
		if (strstr(cmd, "-s")) {
			if (xr_strlen(name))
				name[xr_strlen(name) - 1] = 0;
			char				*output = strstr(cmd, "-out");
			string256			temp0, temp1;
			if (output) {
				output += xr_strlen("-out");
				sscanf(output, "%s", temp0);
				_TrimLeft(temp0);
				output = temp0;
			}
			char				*start = strstr(cmd, "-start");
			if (start) {
				start += xr_strlen("-start");
				sscanf(start, "%s", temp1);
				_TrimLeft(temp1);
				start = temp1;
			}
			char *no_separator_check = strstr(cmd, "-no_separator_check");
			clear_temp_folder();
			CGameSpawnConstructor(name, output, start, !!no_separator_check);
		}
		else if (strstr(cmd, "-verify")) 
		{
				R_ASSERT3(can_use_name, "Too big level name", name);
				verify_level_graph(prjName, !strstr(cmd, "-noverbose"));
		}
	}
}

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
	string4096 cmd;
	
	xr_strcpy(cmd,lpCmdLine);
	strlwr(cmd);
	if (strstr(cmd,"-?") || strstr(cmd,"-h"))			{ Help(); return; }
	if ((strstr(cmd,"-f")==0) && (strstr(cmd,"-g")==0) && (strstr(cmd,"-m")==0) && (strstr(cmd,"-s")==0) && (strstr(cmd,"-t")==0) && (strstr(cmd,"-c")==0) && (strstr(cmd,"-verify")==0) && (strstr(cmd,"-patch")==0))	{ Help(); return; }

	// Give a LOG-thread a chance to startup
	Logger.Initialize("xrAI");	
	u32 dwStartupTime	= timeGetTime();

	execute(cmd);
	
	char stats[256];
	u32 dwEndTime = timeGetTime();
	xr_sprintf(stats, "Time elapsed: %s", make_time((dwEndTime - dwStartupTime) / 1000).c_str());
	
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

#include "factory_api.h"
#include "quadtree.h"
#include "../xrInterface/cl_cast.hpp"

void buffer_vector_test();

CThread::LogFunc ProxyMsg = cdecl_cast([](const char* format, ...) {
	va_list args;
	va_start(args, format);
	Logger.clMsgV(format, args);
	va_end(args);
});

CThreadManager::ReportStatusFunc ProxyStatus = cdecl_cast([](const char* format, ...) {
	va_list args;
	va_start(args, format);
	Logger.StatusV(format, args);
	va_end(args);
});

CThreadManager::ReportProgressFunc ProxyProgress = cdecl_cast([](float progress) { Logger.Progress(progress); });


int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	Debug._initialize		();
	Core._initialize		("xrai",0);

	buffer_vector_test		();
	Startup					(lpCmdLine);

	Core._destroy			();

	return					(0);
}

CSE_Abstract *F_entity_Create(LPCSTR section)
{
	ISE_Abstract	*i = create_entity(section);
	CSE_Abstract	*j = smart_cast<CSE_Abstract*>(i);
	return			(j);
}

void F_entity_Destroy(CSE_Abstract *&i)
{
	ISE_Abstract	*j = i;
	destroy_entity(j);
	i = 0;
}
