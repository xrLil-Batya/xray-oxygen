// xrCore.cpp : Defines the entry point for the DLL application.
//
#include "stdafx.h"
#pragma hdrstop

#include <mmsystem.h>
#include <objbase.h>
#include "xrCore.h"
 
#pragma comment(lib,"winmm.lib")

#ifdef DEBUG
#	include	<malloc.h>
#endif // DEBUG

XRCORE_API		xrCore	Core;
XRCORE_API		u32		build_id;
XRCORE_API		const char*	build_date;

namespace CPU
{
	extern	void			Detect	();
};

static u32	init_counter	= 0;
//extern char g_application_path[256];

#include "DateTime.hpp"
void xrCore::_initialize	(const char* _ApplicationName, LogCallback cb, BOOL init_fs, const char* fs_fname)
{
	std::set_terminate(abort);
	xr_strcpy					(ApplicationName,_ApplicationName);
	if (!init_counter) {
#ifdef XRCORE_STATIC	
		_clear87	();
		_control87	( _PC_53,   MCW_PC );
		_control87	( _RC_CHOP, MCW_RC );
		_control87	( _RC_NEAR, MCW_RC );
		_control87	( _MCW_EM,  MCW_EM );
#endif
		// Init COM so we can use CoCreateInstance
		xr_strcpy			(Params,sizeof(Params),GetCommandLine());
//		_strlwr_s			(Params,sizeof(Params));
		string_path		fn,dr,di;

		// application path
		 GetModuleFileName(GetModuleHandle(MODULE_NAME),fn,sizeof(fn));
		_splitpath(fn,dr,di,0,0);
		strconcat(sizeof(ApplicationPath),ApplicationPath,dr,di);

		GetCurrentDirectory(sizeof(WorkingPath),WorkingPath);

		// User/Comp Name
		string64 _uname;
		DWORD	sz_user		= sizeof(_uname);
		GetUserName			(_uname, &sz_user);
		strconcat(sizeof(UserName), UserName, _uname, "_");
		
		DWORD	sz_comp		= sizeof(CompName);
		GetComputerName		(CompName,&sz_comp);
		
		//Date
		auto *time = new Time();
		strconcat(sizeof(UserDate), UserDate, time->GetDay().c_str(), ".", time->GetMonth().c_str(), ".", time->GetYear().c_str(), " ");
		
		//Time
		strconcat(sizeof(UserTime), UserTime, time->GetHours().c_str(), ".", time->GetMinutes().c_str(), ".", time->GetSeconds().c_str());
		xr_delete(time);

		// Mathematics & PSI detection
		CPU::Detect			();
		
		Memory._initialize	(strstr(Params,"-mem_debug") ? TRUE : FALSE);

		DUMP_PHASE;

		InitLog				();
		_initialize_cpu		();
		rtc_initialize		();

		xr_FS				= new CLocatorAPI();
		xr_EFS				= new EFS_Utils();
	}
	if (init_fs){
		u32 flags			= 0;
		if (0!=strstr(Params,"-build"))	 flags |= CLocatorAPI::flBuildCopy;
		if (0!=strstr(Params,"-ebuild")) flags |= CLocatorAPI::flBuildCopy|CLocatorAPI::flEBuildCopy;
#ifdef DEBUG
		if (strstr(Params,"-cache"))  flags |= CLocatorAPI::flCacheFiles;
		else flags &= ~CLocatorAPI::flCacheFiles;
#endif // DEBUG
		flags |= CLocatorAPI::flScanAppRoot;

#ifndef ELocatorAPIH
		if (strstr(Params,"-file_activity"))	 
			flags |= CLocatorAPI::flDumpFileActivity;
#endif
		FS._initialize		(flags,0,fs_fname);
		Msg					("'%s' build %d, %s\n","xrCore",build_id, build_date);
		EFS._initialize		();
#ifdef DEBUG
		Msg					("CRT heap 0x%08x",_get_heap_handle());
		Msg					("Process heap 0x%08x",GetProcessHeap());
#endif // DEBUG
	}
	SetLogCB				(cb);
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
#ifdef USE_MEMORY_MONITOR
		memory_monitor::flush_each_time	(true);
#endif // USE_MEMORY_MONITOR
		break;
	}
    return TRUE;
}
