/*************************************************
* VERTVER, 2018 (C)
* X-RAY OXYGEN 1.7 PROJECT
*
* Edited: 19 March, 2018
* xrMain.cpp - Main source file for compilation with Qt
* xrLaunch
*************************************************/
#include "xrMain.h"
/////////////////////////////////////////
#pragma comment(lib, "xrEngine.lib")
/////////////////////////////////////////
unsigned int type_ptr;
char const* params_list;
/////////////////////////////////////////
// In RenderList.cpp
void CreateRendererList();
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

	ui->listWidget->addItems ( LIST_ITEMS );
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
* void on_pushButton_clicked() - Method for run button
***********************************************/
void xrLaunch::on_pushButton_clicked() {
	run_xrEngine();
}


/***********************************************
* void run_xrEngine() - Method for Launch 
* xrEngine.dll
***********************************************/
void xrLaunch::run_xrEngine()
{
	QString rendered = ui->listWidget->currentItem()->text();
	//QString launchParams = " " + rendered;
	CreateRendererList();
	//#VERTVER: Don't use here toLatin1(). 
	//#It's can crush compiler on Release/Release_IA32 configuration
	RunApplication(rendered.toLocal8Bit().data());
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
	run_xrEngine();
}


/***********************************************
* void on_actionForserX_triggered() - link to
* Oxygen Team page
***********************************************/
void xrLaunch::on_actionOxygen_Team_triggered()
{
	QString oxylink = "https://github.com/xrOxygen";
	QDesktopServices::openUrl(QUrl(oxylink));
}


/***********************************************
* void on_actionForserX_triggered() - link to
* Vertver page
***********************************************/
void xrLaunch::on_actionVertver_triggered()
{
	QString oxylink = "https://github.com/Vertver";
	QDesktopServices::openUrl(QUrl(oxylink));
}


/***********************************************
* void on_actionForserX_triggered() - link to
* Giperion page
***********************************************/
void xrLaunch::on_actionGiperion_2_triggered()
{
	QString oxylink = "https://github.com/Giperionn";
	QDesktopServices::openUrl(QUrl(oxylink));
}


/***********************************************
* void on_actionForserX_triggered() - link to
* ForserX page
***********************************************/
void xrLaunch::on_actionForserX_triggered()
{
	QString oxylink = "https://github.com/ForserX";
	QDesktopServices::openUrl(QUrl(oxylink));
}
