#include "stdafx.h"
#include "LaunchWindow.h"
using namespace xrLauncher;

#define FRAY_API		__declspec(dllexport)

FRAY_API int RunXRLauncher()
{
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);
	Application::Run(gcnew xrLauncher::LaunchWindow);
	return type_ptr;
}

FRAY_API const char* GetParams()
{
	return params_list;
}