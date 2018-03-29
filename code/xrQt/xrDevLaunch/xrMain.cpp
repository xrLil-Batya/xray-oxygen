/*************************************************
* VERTVER, 2018 (C)
* X-RAY OXYGEN 1.7 PROJECT
*
* Edited: 28 March, 2018
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
Init UI 
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
Init parameters dialog
***********************************************/
xrDialogParam::xrDialogParam(QWidget *parent) :
	QDialog(parent),
	uiDialog(new Ui::Dialog)
{
	uiDialog->setupUi(this);
}


/***********************************************
remove the parameters dialog
***********************************************/
xrDialogParam::~xrDialogParam()
{
	delete uiDialog;
}


/***********************************************
remove the main thread
***********************************************/
xrLaunch::~xrLaunch() 
{
    delete ui;
}


/***********************************************
Running the DLL (xrEngine or xrEditor)
***********************************************/
DLL_API int RunApplication(char* commandLine);


/***********************************************
Method for run xrEngine
***********************************************/
void xrLaunch::on_pushButton_clicked() 
{
	run_xrEngineRun();
}


/***********************************************
add string if pressed
***********************************************/
void xrLaunch::on_listWidget_itemPressed(QListWidgetItem *item)
{
	add_stringToList();
}


std::string params;


/***********************************************
add string to buffer
************************************************/
void xrLaunch::add_stringToList() 
{
	//QString rendered = ui->listWidget->currentItem()->text();
	//params = rendered.toLocal8Bit();
}


void xrLaunch::init_xrCore() 
{
	try
	{
		// Init X-ray core
		Debug._initialize(false);
		Core._initialize("X-Ray Oxygen", nullptr, TRUE, "fsgame.ltx");
	}
	catch (...)
	{
		MessageBox(NULL, "Can't load xrCore!", "Init error", MB_OK | MB_ICONWARNING);
	}
}


/***********************************************
Method for Launch xrEngine.dll
***********************************************/
void xrLaunch::run_xrEngineRun() 
{
	QString rendered = ui->listWidget->currentItem()->text();
	params = rendered.toLocal8Bit();
	init_xrCore();
	CreateRendererList();
	//#VERTVER: Don't use here toLatin1(). Crash on Release
	RunApplication(params.data());
}


/***********************************************
method for close the MainWindow
***********************************************/
void xrLaunch::on_actionExit_triggered() 
{
	xrLaunch::close();
}


/***********************************************
Method for run xrEngine
***********************************************/
void xrLaunch::on_actionxrEngine_triggered() 
{
	run_xrEngineRun();
}


/***********************************************
link to Oxygen Team page
***********************************************/
void xrLaunch::on_actionOxygen_Team_triggered()
{
	QString oxylink = "https://github.com/xrOxygen";
	QDesktopServices::openUrl(QUrl(oxylink));
}


/***********************************************
link to Vertver page
***********************************************/
void xrLaunch::on_actionVertver_triggered()
{
	QString oxylink = "https://github.com/Vertver";
	QDesktopServices::openUrl(QUrl(oxylink));
}


/***********************************************
link to Giperion page
***********************************************/
void xrLaunch::on_actionGiperion_2_triggered()
{
	QString oxylink = "https://github.com/Giperionn";
	QDesktopServices::openUrl(QUrl(oxylink));
}


/***********************************************
link to ForserX page
***********************************************/
void xrLaunch::on_actionForserX_triggered()
{
	QString oxylink = "https://github.com/ForserX";
	QDesktopServices::openUrl(QUrl(oxylink));
}


/***********************************************
open the new window with xrEngine params (doesn't works)
***********************************************/
void xrLaunch::on_actionParametres_triggered()
{
	xrDialogParam *dlg = new xrDialogParam;
	dlg->show();
}


/***********************************************
send the string of params to xrEngineRun();
***********************************************/
void xrDialogParam::on_buttonBox_accepted()
{
	//uiDialog->textEdit
}


/***********************************************
create a new window with "About" dialog
***********************************************/
void xrLaunch::on_actionVertver_Github_triggered()
{
	AboutLauncher *dlg = new AboutLauncher;
	dlg->show();
}

