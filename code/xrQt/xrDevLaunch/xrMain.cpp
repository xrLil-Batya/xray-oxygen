/*************************************************
* VERTVER, 2018 (C)
* X-RAY OXYGEN 1.7 PROJECT
*
* Edited: 20 March, 2018
* xrMain.cpp - Main source file for compilation with Qt
* xrLaunch
*************************************************/
#include "xrMain.h"
/////////////////////////////////////////
#pragma comment(lib, "xrEngine.lib")
/////////////////////////////////////////
unsigned int type_ptr;
char const* params_list;
char const* string_accept;
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
	//#NOTE: Don't use here CPUID::SSE2
	if (!CPUID::SSE3())
	{
		statusBar()->showMessage(tr("Warning! Your CPU doesn't support SSE3 instructions."));
	}
	else if (!CPUID::SSE41()) 
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

/***********************************************
#VERTVER: If you wan't to check AMD - use that
if(CPUID::AMD || CPUID::AMDelse) { your code }
***********************************************/

	ui->listWidget->addItems ( LIST_ITEMS );
}


/***********************************************
* explicit xrLaunch() - Init parameters dialog
***********************************************/
xrDialogParam::xrDialogParam(QWidget *parent) :
	QDialog(parent),
	uiDialog(new Ui::Dialog)
{
	uiDialog->setupUi(this);
}


/***********************************************
* ~xrLaunch() - remove the parameters dialog
***********************************************/
xrDialogParam::~xrDialogParam()
{
	delete uiDialog;
}


/***********************************************
* ~xrLaunch() - remove the main thread
***********************************************/
xrLaunch::~xrLaunch() 
{
    delete ui;
}


/***********************************************
* int RunApplication() - Running the DLL 
* (xrEngine or xrEditor)
***********************************************/
DLL_API int RunApplication(char* commandLine);


/***********************************************
* void on_pushButton_clicked() - Method for run 
* xrEngine
***********************************************/
void xrLaunch::on_pushButton_clicked() 
{
	run_xrEngineRun();
}


/***********************************************
* void run_xrEngine() - add string if pressed
***********************************************/
void xrLaunch::on_listWidget_itemPressed(QListWidgetItem *item)
{
	add_stringToList();
}



/***********************************************
* void run_xrEngine() - add string to buffer
************************************************/
void xrLaunch::add_stringToList() 
{
	
}


/***********************************************
* void run_xrEngine() - Method for Launch 
* xrEngine.dll
***********************************************/
void xrLaunch::run_xrEngineRun() 
{
	//#NOTE: QString can't be public variable.
	QString rendered = ui->listWidget->currentItem()->text();
	CreateRendererList();
	//#VERTVER: Don't use here toLatin1(). Crash on Release
	RunApplication(rendered.toLocal8Bit().data());
}


/***********************************************
* void on_actionExit_triggered() - method for 
* close the MainWindow
***********************************************/
void xrLaunch::on_actionExit_triggered() 
{
	xrLaunch::close();
}


/***********************************************
* void on_actionxrEngine_triggered - Method
* for run xrEngine
***********************************************/
void xrLaunch::on_actionxrEngine_triggered() 
{
	run_xrEngineRun();
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


/***********************************************
* void on_actionParametres_triggered - open the
* new window with xrEngine params (doesn't works)
***********************************************/
void xrLaunch::on_actionParametres_triggered()
{
	xrDialogParam *dlg = new xrDialogParam;
	dlg->show();
}


/***********************************************
* void on_buttonBox_accepted() - send the string
* of params to xrEngineRun();
***********************************************/
void xrDialogParam::on_buttonBox_accepted()
{
	//uiDialog->textEdit
}

void xrLaunch::on_actionVertver_Github_triggered()
{
	AboutLauncher *dlg = new AboutLauncher;
	dlg->show();
}
