/*************************************************
* VERTVER, 2018 (C)
* X-RAY OXYGEN 1.7 PROJECT
*
* Edited: 14 May, 2018
* xrEntryPoint.cpp - entry point for application
* int WINAPI WinMain()
*************************************************/
#include "xrMain.h"

HINSTANCE	g_hInstance;
int			argc;
LPSTR		argv[MAX_NUM_ARGVS];
/***********************************************
entry-point for application.
***********************************************/
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	if (hPrevInstance)
		return 0;

	g_hInstance = hInstance;

#ifndef NULL_ENTRY
	QStringList paths			= QCoreApplication::libraryPaths();
	paths.append				(".");
	paths.append				("platforms");
	QCoreApplication::setLibraryPaths	(paths);
	if (!strstr(lpCmdLine, "-nolauncher"))
	{
		QApplication a				(argc, argv);
		xrLaunch LaunchWIN;
		LaunchWIN.show				();
		return a.exec				();
	}
	else
	{
		DLL_API int RunApplication	(char* commandLine);
		RunApplication				(lpCmdLine);
	}
	return 0;
}
#else
	nCmdShow				= 1;
	LPCSTR err				= "Visual C compiler doesn't supported by xrDevLaunch";
	printf					(err);
	return 0;
}
#endif
