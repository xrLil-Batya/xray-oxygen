/*************************************************
* Oxygen Team, 2018 (C)
*
* main.cxx - Main source file for compilation with Qt
*
* int main()
*************************************************/
#include "xrMain.h"
/////////////////////////////////////////

/***********************************************
* int main() - entry-point for application.
***********************************************/
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    xrLaunch LaunchWIN;
	LaunchWIN.show();
    return a.exec();
// d
}

