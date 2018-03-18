/*************************************************
* Vertver, 2018 (C)
*
* Edited: 18 March, 03/18
* xrMain.cpp - Main source file for compilation with Qt
* xrLaunch
*************************************************/
#include "xrMain.h"
/////////////////////////////////////////
#pragma comment(lib, "xrEngine.lib")
/////////////////////////////////////////

/***********************************************
* explicit xrLaunch() - Init UI 
***********************************************/
xrLaunch::xrLaunch(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::xrLaunch) {	// Init new UI   
	ui->setupUi(this);		// setup it

	// Checking for instructions
	if (!CPUID::SSE41()) 
	{
		statusBar()->showMessage(tr("Your CPU doesn't support SSE4.1 and AVX instructions!"));
	}
	else if (!CPUID::AVX())
	{
		statusBar()->showMessage(tr("Your CPU doesn't support AVX instructions!"));
	}
	// if all instructions are supported
	else
	{
		statusBar()->showMessage(tr("All instructions are supported on your CPU!"));
	}
}

/***********************************************
* ~xrLaunch() - remove the main thread
***********************************************/
xrLaunch::~xrLaunch() {
    delete ui;
}

/***********************************************
* int RunApplication() - Running the DLL 
* (xrEngine or xrEditor)
***********************************************/
DLL_API int RunApplication(char* commandLine);

/***********************************************
* void RunApplication() - Method for Button "Run"
***********************************************/
void xrLaunch::on_pushButton_clicked() {
	char const* params_list;

	std::string rendered = " -r2";
	RunApplication(rendered.data());
}

/***********************************************
* void on_actionExit_triggered() - method for 
* close the MainWindow
***********************************************/
void xrLaunch::on_actionExit_triggered() {
	xrLaunch::close();
}

/***********************************************
* void RunApplication() - Method for menu 
* (Run xrEngine.dll)
***********************************************/
void xrLaunch::on_actionxrEngine_triggered() {
	std::string params;
	RunApplication(params.data());
}
