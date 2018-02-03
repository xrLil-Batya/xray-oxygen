// xrCore.cpp : Defines the entry point for the DLL application.
//
#include "stdafx.h"
#pragma hdrstop

#include <mmsystem.h>
#include <objbase.h>
#include "../FrayBuildConfig.hpp"
#pragma comment(lib,"winmm.lib")

#ifdef DEBUG
#	include	<malloc.h>
#endif // DEBUG

XRCORE_API		xrCore	Core;
XRCORE_API		u32		build_id;
XRCORE_API		const char*	build_date;


static u32	init_counter	= 0;
void compute_build_id();

#include "DateTime.hpp"
void xrCore::_initialize(const char* _ApplicationName, LogCallback cb, BOOL init_fs, const char* fs_fname)
{
	std::set_terminate(abort);
	if (!init_counter)
	{
		xr_strcpy(ApplicationName, _ApplicationName);
		// Init COM so we can use CoCreateInstance
		xr_strcpy(Params, sizeof(Params), GetCommandLine());
#ifdef _STR_LWRC_
		_strlwr_s			(Params,sizeof(Params));
#endif
		string_path		fn, dr, di;

		// application path
		GetModuleFileName(GetModuleHandle("xrCore"), fn, sizeof(fn));
		_splitpath(fn, dr, di, 0, 0);
		strconcat(sizeof(ApplicationPath), ApplicationPath, dr, di);

		GetCurrentDirectory(sizeof(WorkingPath), WorkingPath);

		// User/Comp Name
		string64 _uname;
		DWORD	sz_user = sizeof(_uname);
		GetUserName(_uname, &sz_user);
		xr_strcpy(UserName, _uname);

		DWORD	sz_comp = sizeof(CompName);
		GetComputerName(CompName, &sz_comp);

		//Date
		Time *time = new Time();
		strconcat(sizeof(UserDate), UserDate, time->GetDay().c_str(), ".", time->GetMonth().c_str(), ".", time->GetYear().c_str(), " ");

		//Time
		strconcat(sizeof(UserTime), UserTime, time->GetHours().c_str(), ".", time->GetMinutes().c_str(), ".", time->GetSeconds().c_str());
		xr_delete(time);

		// Mathematics & PSI detection
		Memory._initialize();

		InitLog();
		_initialize_cpu();
		rtc_initialize();

		xr_FS = new CLocatorAPI();
		xr_EFS = new EFS_Utils();
	}
	if (init_fs)
	{
		u32 flags = 0;
		if (0 != strstr(Params, "-build"))	 flags |= CLocatorAPI::flBuildCopy;
		if (0 != strstr(Params, "-ebuild")) flags |= CLocatorAPI::flBuildCopy | CLocatorAPI::flEBuildCopy;
#ifdef DEBUG
		if (strstr(Params, "-cache"))  flags |= CLocatorAPI::flCacheFiles;
		else flags &= ~CLocatorAPI::flCacheFiles;
#endif // DEBUG
		flags |= CLocatorAPI::flScanAppRoot;

#ifndef ELocatorAPIH
		if (strstr(Params, "-file_activity"))
			flags |= CLocatorAPI::flDumpFileActivity;
#endif
		FS._initialize(flags, 0, fs_fname);
		compute_build_id();
		Msg("xrCore build %d, %s\n", build_id, build_date);
		EFS._initialize();
	}
	SetLogCB(cb);
	init_counter++;
}

void xrCore::_destroy		()
{
	--init_counter;
	if (!init_counter){
		FS._destroy			();
		EFS._destroy		();
		xr_delete			(xr_FS);
		xr_delete			(xr_EFS);

		Memory._destroy		();
	}
}

//. why ??? 
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD ul_reason_for_call, LPVOID lpvReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		{
			_clear87		();
			_control87		( _PC_53,   MCW_PC );
			_control87		( _RC_CHOP, MCW_RC );
			_control87		( _RC_NEAR, MCW_RC );
			_control87		( _MCW_EM,  MCW_EM );
		}
		break;
	case DLL_THREAD_ATTACH:
		timeBeginPeriod	(1);
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
    return TRUE;
}
