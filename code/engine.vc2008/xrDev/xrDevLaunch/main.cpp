/*************************************************
* VERTVER, 2018 (C)
* X-RAY OXYGEN 1.7 PROJECT
*
* Edited: 20 March, 2018
* main.cxx - Main source file for compilation with Qt
* int main()
*************************************************/
#include "xrMain.h"
/***********************************************
* int main() - entry-point for application.
***********************************************/
int WINAPI main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    xrLaunch LaunchWIN;
	//#VERTVER: Critical moment: The compiler create code with SSE2 instructions 
	//#(only xrDevLauncher compiling with IA32-x86 instructions),
	//#some part of matrix and vectors use SSE3. Be difficult!
	if (!CPUID::SSE2) 
	{
		MessageBox(NULL, "xrDevLaunch must be closed", "Your CPU doesn't support SSE2", MB_ICONSTOP);
	}
	else if (!CPUID::SSE3) 
	{
		MessageBox(NULL, "xrDevLaunch can work unstability", "Your CPU doesn't support SSE3", MB_ICONWARNING);
		LaunchWIN.show();
		return a.exec();
	}
	else
	{
		LaunchWIN.show();
		return a.exec();
	}
}

