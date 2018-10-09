//#include "MainForm.h"
#include "..\..\engine.vc2008\xrCore\xrCore.h"
#include <Windows.h>

[System::STAThreadAttribute]
int APIENTRY WinMain(HINSTANCE hInsttance, HINSTANCE hPrevInstance, char* lpCmdLine, int nCmdShow)
{
	Core._initialize("Layout Editor", 0, true, "fsgame.ltx");
	//ECore::Application::EnableVisualStyles();
	//ECore::Application::SetCompatibleTextRenderingDefault(false);
	//ECore::Application::Run(gcnew ECore::MeshEdit);
	return 0;
	//	System::STAThreadAttribute;
}