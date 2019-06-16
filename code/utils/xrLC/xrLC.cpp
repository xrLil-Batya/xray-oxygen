// xrLC.cpp : Defines the entry point for the application.
//
#include "stdafx.h"
#include "math.h"
#include "build.h"
#include "../xrLC_Light/xrLC_GlobalData.h"
#include "../xrInterface/cl_cast.hpp"
#include "../xrInterface/UIParams.hpp"

#pragma comment(lib,"comctl32.lib")
#pragma comment(lib,"d3dx9.lib")
#pragma comment(lib,"IMAGEHLP.LIB")
#pragma comment(lib,"winmm.LIB")
#pragma comment(lib,"xrCDB.lib")
#pragma comment(lib,"FreeImage.lib")
#pragma comment(lib,"xrCore.lib")
#pragma comment(lib,"xrLC_Light.lib")
#pragma comment(lib, "xrLCUtil.lib")
#pragma comment(lib, "XTools.lib")
#define PROTECTED_BUILD

#ifdef PROTECTED_BUILD
#	define TRIVIAL_ENCRYPTOR_ENCODER
#	define TRIVIAL_ENCRYPTOR_DECODER
#	include "../../xrEngine/trivial_encryptor.h"
#	undef TRIVIAL_ENCRYPTOR_ENCODER
#	undef TRIVIAL_ENCRYPTOR_DECODER
#endif // PROTECTED_BUILD
extern XRCORE_API bool bStartedThread;
CBuild*	pBuild		= nullptr;
u32		version		= 0;

static constexpr char* h_str =
	"The following keys are supported / required:\n"
	"-skip	== Skip the test invalid face\n"
	"-? or -h		 == this help\n"
	"-skip			 == skip crash if invalid faces exists\n"
	"-nosun			 == disable sun-lighting\n"
	"-norgb			 == disable common lightmap calculating\n"
	"-notessellation == skip tessellation for lightmap"
	"-sp<1-4>		 == the level of priority\n"
	"-f<NAME>		 == compile level in GameData\\Levels\\<NAME>\\\n"
	"\n"
	"NOTE: The last key is required for any functionality\n";

void WinShutdown()
{
	HANDLE hToken;
	TOKEN_PRIVILEGES tkp;

	// Get a token for this process. 
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
		return;

	// Get the LUID for the shutdown privilege. 
	LookupPrivilegeValue(nullptr, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);

	tkp.PrivilegeCount = 1;  // one privilege to set    
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	// Get the shutdown privilege for this process.
	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)nullptr, nullptr);

	if (GetLastError() != ERROR_SUCCESS)
		return;

	// Shut down the system and force all applications to close. 
	ExitWindowsEx(EWX_SHUTDOWN | EWX_FORCE, SHTDN_REASON_MAJOR_OPERATINGSYSTEM | SHTDN_REASON_MINOR_UPGRADE | SHTDN_REASON_FLAG_PLANNED);
}

inline void Help()
{
	MessageBox(nullptr,h_str,"Command line options",MB_OK|MB_ICONINFORMATION);
}

using xrOptions = int __cdecl(b_params* params, u32 version, bool bRunBuild);

void Startup(char* lpCmdLine)
{
	bStartedThread = true;
	create_global_data();
	char cmd[512], name[256];

	xr_strcpy(cmd, lpCmdLine);
	xr_strcpy(Core.Params, lpCmdLine);

	strlwr(cmd);

	if (strstr(cmd, "-?") || !strstr(cmd, "-f"))
	{
		Help();
		return;
	}

	g_build_options.b_optix_accel = (strstr(cmd, "-hardware_light"));
	bool bModifyOptions = (strstr(cmd, "-o"));
	g_build_options.b_radiosity = (strstr(cmd, "-gi"));
	g_build_options.b_noise = (strstr(cmd, "-noise"));
	g_build_options.b_skipinvalid = (strstr(cmd, "-skip"));
	g_build_options.b_notessellation = (strstr(cmd, "-notessellation"));
	g_build_options.b_mxthread = (strstr(cmd, "-mxthread"));

	// Added priority setting
	const char* isSp = strstr(cmd, "-sp");
	if (isSp)
	{
		g_build_options.Priority = isSp[3] - '0';
	}
	// end

	VERIFY(lc_global_data());
	lc_global_data()->b_nosun_set(!!strstr(cmd, "-nosun"));
	lc_global_data()->b_skiplmap_set(!!strstr(cmd, "-norgb"));

	// Give a LOG-thread a chance to startup
	// Faster FPU 
	switch (g_build_options.Priority)
	{
	case 1: SetPriorityClass(GetCurrentProcess(), IDLE_PRIORITY_CLASS);
	case 2: SetPriorityClass(GetCurrentProcess(), NORMAL_PRIORITY_CLASS);
	case 3: SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
	case 4: SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
	}

	// Load project
	name[0] = 0;
	sscanf(strstr(cmd, "-f") + 2, "%s", name);
	string256 temp;
	xr_sprintf(temp, "%s - Levels Compiler", name);
	Logger.Initialize(temp);

	string_path prjName;
	FS.update_path(prjName, "$game_levels$", xr_strconcat(prjName, name, "\\build.prj"));
	string256 PhaseName;
	Logger.Phase(xr_strconcat(PhaseName, "Reading project [", name, "]..."));

	string256 inf;
	IReader* F = FS.r_open(prjName);
	if (!F) {
		xr_sprintf(inf, "Build failed!\nCan't find level: '%s'", name);
		Logger.clMsg(inf);
		Logger.Failure(inf);
		Logger.Destroy();
		return;
	}

	// Version
	F->r_chunk(EB_Version, &version);
	Logger.clMsg("version: %d", version);

	bool oxyVersion = (XRCL_CURRENT_VERSION == version) || (version == 18);
	R_ASSERT2(oxyVersion, "xrLC don't support a current version. Sorry.");

	// Header
	b_params Params;
	F->r_chunk(EB_Parameters, &Params);

	// Show options if needed
	if (bModifyOptions)
	{
		Logger.Phase("Project options...");
		HMODULE		L = LoadLibrary("xrLC_Options");
		void*		P = GetProcAddress(L, "_frmScenePropertiesRun");
		R_ASSERT(P);
		xrOptions*	O = (xrOptions*)P;
		int			R = O(&Params, version, false);
		FreeLibrary(L);
		if (R == 2) {
			ExitProcess(0);
		}
	}

	// Conversion
	Logger.Phase("Converting data structures...");
	pBuild = xr_new<CBuild>();
	pBuild->Load(Params, *F);
	FS.r_close(F);

	// Call for builder
	string_path lfn;
	CTimer	dwStartupTime;	dwStartupTime.Start();
	FS.update_path(lfn, _game_levels_, name);
	pBuild->Run(lfn);
	xr_delete(pBuild);

	// Show statistic
	extern xr_string make_time(u32 sec);

	u32	dwEndTime = dwStartupTime.GetElapsed_ms();
	xr_sprintf(inf, "Time elapsed: %s", make_time(dwEndTime / 1000).c_str());
	Logger.clMsg("Build succesful!\n%s", inf);

	if (!strstr(cmd, "-silent") && !pUIParams->isShutDown)
	{
		Logger.Success(inf);
	}

	delete pUIParams;
	Logger.Destroy();

	if (pUIParams->isShutDown)
		WinShutdown();
}

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// Initialize debugging
	Debug._initialize	();
	Core._initialize	("xrLC");
	
	if(strstr(Core.Params,"-nosmg"))
		g_using_smooth_groups = false;

	Startup				(lpCmdLine);
	Core._destroy		();
	
	return 0;
}
