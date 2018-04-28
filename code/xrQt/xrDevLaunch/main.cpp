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
entry-point for application.
***********************************************/
#ifdef __cplusplus
int WINAPI main(int argc,
				char *argv[])
{
	QApplication a				(argc, argv);
	xrLaunch LaunchWIN;
	LaunchWIN.show				();
	return a.exec				();

}
#else
{

	int main();
	int main()
	{
		const char* err			= "Visual C compiler doesn't supported by xrDevLaunch";
		printf					(err);
		Sleep					(2500);
		return 0;
	}


}
#endif
