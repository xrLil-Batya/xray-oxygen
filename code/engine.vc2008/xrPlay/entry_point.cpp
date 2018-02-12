#include <string>
#include <windows.h>
#include "xrLauncherWnd.h"

#pragma comment(lib, "xrEngine.lib")
#define DLL_API __declspec(dllimport)

int RunXRLauncher()
{
	xrPlay::Application::EnableVisualStyles();
	xrPlay::Application::SetCompatibleTextRenderingDefault(false);
	xrPlay::Application::Run(gcnew xrPlay::xrLauncherWnd);
	return xrPlay::type_ptr;
}

const char* GetParams()
{
	return xrPlay::params_list;
}
extern DLL_API int RunApplication(char* commandLine);

int APIENTRY WinMain(HINSTANCE hInsttance, HINSTANCE hPrevInstance, char* lpCmdLine, int nCmdShow)
{
    std::string params = lpCmdLine;
    if (strstr(lpCmdLine, "-launcher"))
    {
        const int l_res = RunXRLauncher();
        switch (l_res)
        {
        case 0: return 0;
        }
        params = GetParams();
    }

    RunApplication(params.data());

    return 0;
}