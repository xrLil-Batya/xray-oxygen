/*************************************************
* VERTVER, 2018 (C)
* X-RAY OXYGEN 1.7 PROJECT
*
* Edited: 26 March, 2018
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
	//#some part of matrix and vectors use SSE3. It's can be difficult!
	LaunchWIN.show();
	return a.exec();
}

