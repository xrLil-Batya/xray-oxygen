// xrCore.cpp : Defines the entry point for the DLL application.
//
#include "stdafx.h"
#include <objbase.h>
#include "../FrayBuildConfig.hpp"
#include "oxy_version.h"
#pragma comment(lib,"winmm.lib")

#ifdef DEBUG
#	include	<malloc.h>
#endif // DEBUG

XRCORE_API xrCore Core;
XRCORE_API u32 build_id;
XRCORE_API const char*	build_date;

//indicate that we reach WinMain, and all static variables are initialized
XRCORE_API bool	gModulesLoaded = false;
XRCORE_API xr_vector<xr_token> vid_quality_token;

static u32	init_counter = 0;
void compute_build_id();

#include "DateTime.hpp"
void xrCore::_initialize(const char* _ApplicationName, xrLogger::LogCallback cb, BOOL init_fs, const char* fs_fname)
{
	if (!init_counter)
	{
		PluginMode = false;
		xr_strcpy(ApplicationName, _ApplicationName);
        ZeroMemory(Params, sizeof(Params));
        xr_strcpy(Params, GetCommandLine());
		_strlwr_s(Params,sizeof(Params));
		string_path fn, dr, di;

		// application path
		GetModuleFileName(GetModuleHandle("xrCore"), fn, sizeof(fn));
		_splitpath(fn, dr, di, nullptr, nullptr);
		xr_strconcat(ApplicationPath, dr, di);

		GetCurrentDirectory(sizeof(WorkingPath), WorkingPath);

		// User/Comp Name
		ZeroMemory(UserName, sizeof(UserName));
		ZeroMemory(CompName, sizeof(CompName));

		PlatformUtils.GetUsername(UserName);
		PlatformUtils.GetComputerName(CompName);

		// Mathematics & PSI detection
		Memory._initialize();

		xrLogger::InitLog();
		_initialize_cpu();
		XRay::Compress::RT::RtcInitialize();

		xr_FS = new CLocatorAPI();
#ifdef WITH_EFS
		xr_EFS = new EFS_Utils();
#endif
	}
	if (init_fs)
	{
		u32 flags = 0;
		if (nullptr != strstr(Params, "-build"))	 flags |= CLocatorAPI::flBuildCopy;
		if (nullptr != strstr(Params, "-ebuild")) flags |= CLocatorAPI::flBuildCopy | CLocatorAPI::flEBuildCopy;
#ifdef DEBUG
		if (strstr(Params, "-cache"))  flags |= CLocatorAPI::flCacheFiles;
		else flags &= ~CLocatorAPI::flCacheFiles;
#endif // DEBUG
		flags |= CLocatorAPI::flScanAppRoot;

#ifndef ELocatorAPIH
		if (strstr(Params, "-file_activity"))
			flags |= CLocatorAPI::flDumpFileActivity;
#endif
		FS._initialize(flags, nullptr, fs_fname);

        // FS is valid at this point, signal to debug system
        Debug._initializeAfterFS();

		compute_build_id();
		Msg("xrCore build %d, %s\n", build_id, build_date);
#ifdef _AUTHOR
		Msg("xrOxygen Version: branch[%s], commit[%s: %s]", _BRANCH, _AUTHOR, _HASH);
#else
		Msg("xrOxygen Version: branch[%s], commit[%s]", _BRANCH, _HASH); 
#endif

		EFS._initialize();
	}
	xrLogger::AddLogCallback(cb);
	bSpectreEnabled = !strstr(/*Core.*/Params, "-no_spectre");
	init_counter++;
}

void xrCore::_destroy()
{
	--init_counter;
	if (!init_counter)
	{
		FS._destroy			();
		EFS._destroy		();
		xr_delete			(xr_FS);
#ifdef WITH_EFS
		xr_delete			(xr_EFS);
#endif

		Memory._destroy		();
	}
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD ul_reason_for_call, LPVOID lpvReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		{
			_clearfp();
			_controlfp(_RC_CHOP, _MCW_RC);
			_controlfp(_RC_NEAR, _MCW_RC);
			_control87(_MCW_EM, MCW_EM);
		}
		break;
	case DLL_PROCESS_DETACH:
		{
			_clearfp();
		}
		break;
	}
    return TRUE;
}

extern "C"
{
	__declspec(dllexport) const char* GetCurrentHash()
	{
		return _HASH;
	}
}