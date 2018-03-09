// entry_point.cpp - Main methods for initialize.

#include <string>
#include <windows.h>
#include "xrLauncherWnd.h"
#include <intrin.h>  
#include "minimal_CPUID.h"
#pragma comment(lib, "xrEngine.lib")

#define DLL_API __declspec(dllimport)

void CreateRendererList();					// In RenderList.cpp

/***********************************************
* RunXRLauncher() - main method for initialize launcher
* and run all elements.
***********************************************/
int RunXRLauncher()
{
	// Get initialize
	xrPlay::Application::EnableVisualStyles();
	xrPlay::Application::SetCompatibleTextRenderingDefault(false);
	xrPlay::Application::Run(gcnew xrPlay::xrLauncherWnd);
	return xrPlay::type_ptr;
}

/***********************************************
* GetParams() - return the list of parametres
***********************************************/
const char* GetParams()
{
	return xrPlay::params_list;
}

extern DLL_API int RunApplication(char* commandLine);

/***********************************************
* WinMain() - Parametres for starting launcher
***********************************************/
int APIENTRY WinMain(HINSTANCE hInsttance, HINSTANCE hPrevInstance, char* lpCmdLine, int nCmdShow)
{

	std::string params = lpCmdLine;

	// If we don't needy for a excetions - we can delete exceptions with option "-silent"
	if (!strstr(lpCmdLine, "-silent")) {
		if (!CPUID::AVX())
		{
			MessageBox(NULL, "It's can affect on the stability of the game.", "AVX is not a support!", MB_OK | MB_ICONWARNING);
		}

		if (!CPUID::HighEndCPU())
		{
			MessageBox(NULL, "AES was appeared after AMD Bulldozers\nand Intel Westmere", "Your CPU doesn't have new instructions", MB_OK | MB_ICONASTERISK);
		}
	}

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
	/*
	if (!strstr(lpCmdLine, "-launcher")) {
		if (strstr(lpCmdLine, "r2")) {
			xrPlay::type_ptr = 1;
			msclr::interop::marshal_context marsh;
			System::String^ rendered = "-r2";
			xrPlay::params_list = marsh.marshal_as<char const*>(textBox1->Text + " " + rendered);

		}
	}
	*/
	CreateRendererList();
	RunApplication(params.data());

	return 0;

}