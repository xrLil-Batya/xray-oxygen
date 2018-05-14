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
#define MINIMUM_WIN_MEMORY	0x0a00000
#define MAXIMUM_WIN_MEMORY	0x1000000
#define DLL_API __declspec(dllimport)
HINSTANCE	g_hInstance;
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
/// Return the list of parametres
/// </summary>
const char* GetParams()
{
	return xrPlay::ret_values.params_list;
}


/// <summary>
/// Dll import
/// </summary>
DLL_API int RunApplication(char* commandLine);


/// <summary>
/// Main method for initialize xrEngine
/// </summary>
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

	if (hPrevInstance)				//#VERTVER: Previous Instance can't be in WinNT 
		return 0;

	g_hInstance = hInstance;

	std::string params = lpCmdLine;

	try
	{
		// Init X-ray core
		Debug._initialize(false);
		Core._initialize("X-Ray Oxygen", nullptr, TRUE, "fsgame.ltx");
	}
	catch (...)
	{
		MessageBox(NULL, "Can't load xrCore!", "Init error", MB_OK | MB_ICONWARNING);
	}

	const bool launch = strstr(lpCmdLine, "-launcher");

	////////////////////////////////////////////////////
	// If we don't needy for a excetions - we can 
	// delete exceptions with option "-silent"
	////////////////////////////////////////////////////
<<<<<<< HEAD

#ifndef DEBUG
=======
>>>>>>> parent of 31343e9e... * FIX: LtPetrov OOPS 2
	if (!strstr(lpCmdLine, "-silent") && !launch)
	{
		// Checking for SSE2
		if (!CPU::Info.hasFeature(CPUFeature::SSE2))
		{
			return 0;
		}
		// Checking for SSE3
<<<<<<< HEAD
		else if (!CPU::Info.hasFeature(CPUFeature::SSE3))
=======
		if (!CPU::Info.hasFeature(CPUFeature::SSE3))
>>>>>>> parent of 31343e9e... * FIX: LtPetrov OOPS 2
		{
			MessageBox(NULL,
				"It's can affect on the stability of the game.",
				"SSE3 isn't supported on your CPU",
				MB_OK | MB_ICONASTERISK);
			//#VERTVER: some part of vectors use SSE3 instructions
		}
		// Checking for AVX
<<<<<<< HEAD
#ifndef RELEASE_IA32
=======
>>>>>>> parent of 31343e9e... * FIX: LtPetrov OOPS 2
		else if (!CPU::Info.hasFeature(CPUFeature::AVX))
		{
			MessageBox(NULL,
				"It's can affect on the stability of the game.",
				"AVX isn't supported on your CPU!",
				MB_OK | MB_ICONWARNING);
		}
	}
<<<<<<< HEAD
#endif
#endif
=======
>>>>>>> parent of 31343e9e... * FIX: LtPetrov OOPS 2

	// If we want to start launcher
	if (launch)
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
	RunApplication(params.data());
	return 0;
}
