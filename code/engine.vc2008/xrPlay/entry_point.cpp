////////////////////////////////////////
// OXYGEN TEAM, 2018 (C) * X-RAY OXYGEN	
// entry_point.cpp - entry point of xrPlay
// Edited: 13 May, 2018						
////////////////////////////////////////
#include <string>
#include <intrin.h>  
#include <windows.h>
////////////////////////////////////
#include "xrLauncherWnd.h"
#include "../xrCore/xrCore.h"
////////////////////////////////////
#pragma comment(lib, "xrEngine.lib")
#define DLL_API __declspec(dllimport)
////////////////////////////////////

void CreateRendererList();					// In RenderList.cpp

/// <summary>
/// Method for init launcher
/// </summary>
int RunXRLauncher()
{
	// Get initialize launcher
	xrPlay::Application::EnableVisualStyles();
	xrPlay::Application::SetCompatibleTextRenderingDefault(false);
	xrPlay::Application::Run(gcnew xrPlay::xrLauncherWnd);
	return xrPlay::ret_values.type_ptr;
}


/// <summary>
/// Return the list of parameters
/// </summary>
const char* GetParams()
{
	return xrPlay::ret_values.params_list;
}


/// <summary>
/// Dll import
/// </summary>
DLL_API int RunApplication(LPCSTR commandLine);

/// <summary>
/// Main method for initialize xrEngine
/// </summary>
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	////////////////////////////////////////////////////
    gModulesLoaded = true;
	LPCSTR params = lpCmdLine;
	////////////////////////////////////////////////////
	try
	{
		// Init X-ray core
		Debug._initialize(false);
		Core._initialize("X-Ray Oxygen", nullptr, TRUE, "fsgame.ltx");
	}
	catch (...)
	{
		MessageBoxA(NULL, "Can't load xrCore!", "Init error", MB_OK | MB_ICONHAND);
	}

	////////////////////////////////////////////////////
	// If we don't needy for a exceptions - we can 
	// delete exceptions with option "-silent"
	////////////////////////////////////////////////////

#ifndef DEBUG
	if (!strstr(lpCmdLine, "-silent") && !strstr(lpCmdLine, "-launcher"))
	{
		// Checking for SSE2
		if (!CPU::Info.hasFeature(CPUFeature::SSE2))
		{
			return 0;
		}
		// Checking for SSE3
		else if (!CPU::Info.hasFeature(CPUFeature::SSE3))
		{
			MessageBoxA(NULL,
				"It's can affect on the stability of the game.",
				"SSE3 isn't supported on your CPU",
				MB_OK | MB_ICONASTERISK);
			//#VERTVER: some part of vectors use SSE3 instructions
		}
		// Checking for AVX
#ifndef RELEASE_IA32
		else if (!CPU::Info.hasFeature(CPUFeature::AVX))
		{
			MessageBoxA(NULL,
				"It's can affect on the stability of the game.",
				"AVX isn't supported on your CPU!",
				MB_OK | MB_ICONWARNING);
		}
#endif
	}
#endif

	// If we want to start launcher
	if (strstr(lpCmdLine, "-launcher"))
	{
		const int l_res = RunXRLauncher();
		switch (l_res)
		{
		case 0:
			return 0;
		}
		params = GetParams();
	}

	CreateRendererList();
	return RunApplication(params);
}