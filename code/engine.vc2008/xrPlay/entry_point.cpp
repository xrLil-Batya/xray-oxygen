// entry_point.cpp - Main methods for initialize.

#include <string>
#include <windows.h>
#include "xrLauncherWnd.h"
#include <intrin.h>  
#include "minimal_CPUID.h"
#include "../xrCore/cpuid.h"
#include "../xrCore/_math.h"
#pragma comment(lib, "xrEngine.lib")
#define DLL_API __declspec(dllimport)

void CreateRendererList();					// In RenderList.cpp

/***********************************************
* RunXRLauncher() - main method for initialize 
* launcher and run all elements.
***********************************************/
int RunXRLauncher()
{
	// Get initialize launcher
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
	if (!strstr(lpCmdLine, "-silent")) 
	{
		//#VERTVER: We're using our CPUID cuz using the xrCore CPUID may affect on stability 
		if (!CPUID::AVX())
		{
			MessageBox(NULL, "It's can affect on the stability of the game.", "AVX isn't supported on your CPU!", MB_OK | MB_ICONWARNING);
		}

		// Checking for SSE4.1
		if (!CPUID::HighEndCPU())
		{
			MessageBox(NULL, "It's can affect on the stability of the game.", "SSE4.1 isn't supported on your CPU", MB_OK | MB_ICONASTERISK);
			//#VERTVER: We're checking for SSE4.1 instructions cuz MSVC compiler use the SSE4.1 for CPU manipulation 
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
	// Unused method
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