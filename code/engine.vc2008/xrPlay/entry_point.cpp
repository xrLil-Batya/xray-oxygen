// entry_point.cpp - Main methods for initialize.

#include <string>
#include <windows.h>
#include "xrLauncherWnd.h"
#include <intrin.h>  
#include "minimal_CPUID.h"
#pragma comment(lib, "xrEngine.lib")

#define DLL_API __declspec(dllimport)

void CreateRendererList						( );					// In RenderList.cpp

/***********************************************
*
* RunXRLauncher() - main method for initialize 
* and run all elements. 
*
***********************************************/
int RunXRLauncher							( )
{
	// Get initialize
	if (CPUID::AVX())
	{

	}
	else
	{

	}
	xrPlay::Application::EnableVisualStyles					( );
	xrPlay::Application::SetCompatibleTextRenderingDefault	(false);
	xrPlay::Application::Run(gcnew xrPlay::xrLauncherWnd);
	return xrPlay::type_ptr;								// return the type of Render 
															// (-r2.5, r2 and etc.)
}
/***********************************************
*
* GetParams() - return the list pf parametres 
*
***********************************************/
const char* GetParams						( )
{
	return xrPlay::params_list;
}

extern DLL_API int RunApplication(char* commandLine);
/***********************************************
*
* WinMain() - main init
*
***********************************************/
int APIENTRY WinMain(
	HINSTANCE hInsttance,
	HINSTANCE hPrevInstance,
	char* lpCmdLine,
	int nCmdShow
)

{
	
    std::string params = lpCmdLine;
    if (
		strstr(lpCmdLine, "-launcher"))
    {
        const int l_res			=	RunXRLauncher	( );
        switch										( l_res )
        {
        case 0: 
			return 0;
        }
        params 					=	GetParams		( );
    }
	CreateRendererList								( );
    RunApplication(
		params.data()
	);

    return 0;

}