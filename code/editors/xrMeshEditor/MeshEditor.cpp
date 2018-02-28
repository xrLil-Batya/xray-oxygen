#include "MainForm.h"
#include <Windows.h>

[System::STAThreadAttribute]
int APIENTRY WinMain(HINSTANCE hInsttance, HINSTANCE hPrevInstance, char* lpCmdLine, int nCmdShow)
{
	ECore::Application::EnableVisualStyles();
	ECore::Application::SetCompatibleTextRenderingDefault(false);
	ECore::Application::Run(gcnew ECore::MeshEdit);
	return 0;
//	System::STAThreadAttribute;
}