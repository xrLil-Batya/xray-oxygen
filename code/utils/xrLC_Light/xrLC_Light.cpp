// xrLC_Light.cpp : Defines the entry point for the DLL application.
//
#include "stdafx.h"
#include "xrLc_globaldata.h"
#pragma comment(lib,"xrCore.lib")
#pragma comment(lib,"xrCDB.lib")
#pragma comment(lib,"xrLCUtil.lib")
#pragma comment(lib,"FreeImage.lib")
#ifdef _MANAGED
#pragma managed(push, off)
#endif
#include "../xrInterface/cl_cast.hpp"

XRLC_LIGHT_API LevelCompilerLoggerWindow& Logger = LevelCompilerLoggerWindow::instance();

XRLC_LIGHT_API CThread::LogFunc ProxyMsg = cdecl_cast(
	[](const char *format, ...)
{
	va_list args;
	va_start(args, format);
	Logger.clMsgV(format, args);
	va_end(args);
}
);

XRLC_LIGHT_API CThreadManager::ReportStatusFunc ProxyStatus = cdecl_cast(
	[](const char *format, ...)
{
	va_list args;
	va_start(args, format);
	Logger.StatusV(format, args);
	va_end(args);
}
);

XRLC_LIGHT_API CThreadManager::ReportProgressFunc ProxyProgress = cdecl_cast(
	[](float progress)
{ Logger.Progress(progress); }
);

b_params &g_params()
{
	VERIFY(inlc_global_data());
	return inlc_global_data()->g_params();
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		Debug._initialize();
		bool init_log = (0 != xr_strcmp(Core.ApplicationName, "XRayEditorTools"));
		Core._initialize("xrLC_Light", 0, TRUE, "fsgame.ltx");
		if (init_log)
			xrLogger::InitLog();

		//FPU::m64r	();
		break;
	}
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		if (inlc_global_data())
			destroy_global_data();
		Core._destroy();
		break;
	}
	return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif

