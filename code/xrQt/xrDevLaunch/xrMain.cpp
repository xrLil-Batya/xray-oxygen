/*************************************************
* VERTVER, 2018 (C)
* X-RAY OXYGEN 1.7 PROJECT
*
* Edited: 30 March, 2018
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
std::string params = "-nointro";
/////////////////////////////////////////
// In RenderList.cpp
void CreateRendererList();
/////////////////////////////////////////

/***********************************************
init UI 
***********************************************/
xrLaunch::xrLaunch(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::xrLaunch) {	// Init new UI   
	ui->setupUi(this);		// setup it
	// Checking for instructions
	//#VERTVER: SSE2 can't check: compiler already compile it at SSE2
	if (!CPUID::SSE3())
		statusBar()->showMessage(tr("Warning! Your CPU doesn't support SSE3 instructions."), 12000);
	else if (!CPUID::SSE41()) 
		statusBar()->showMessage(tr("Your CPU doesn't support SSE4.1 and AVX instructions!"), 6000);
	else if (!CPUID::AVX())
		statusBar()->showMessage(tr("Your CPU doesn't support AVX instructions!"), 6000);
	else
		statusBar()->showMessage(tr("All instructions are supported on your CPU!"), 6000);
	/////////////////////////////////////////
	//if (CPU::Info.hasFeature(CPUFeature::AVX)) {}			// Doesn't compile with it
	ui->listWidget->addItems ( LIST_ITEMS );
}


/***********************************************
init parameters dialog
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
running the DLL (xrEngine or xrEditor)
***********************************************/
DLL_API int RunApplication(char* commandLine);


/***********************************************
method for run xrEngine
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


/***********************************************
add string to buffer
************************************************/
void xrLaunch::add_stringToList() 
{
	QString rendered = ui->listWidget->currentItem()->text();
	//#VERTVER: Don't use here toLatin1(). Crash on Release
	params = rendered.toLocal8Bit();
	statusBar()->showMessage(tr("Added to string buffer"), 2000);
}


/***********************************************
init xrCore
************************************************/
void xrLaunch::init_xrCore() 
{
	try
	{
		// Init X-ray core
		statusBar()->showMessage(tr("Loading xrCore..."));
		Debug._initialize(false);
		Core._initialize("X-Ray Oxygen", nullptr, TRUE, "fsgame.ltx");
		statusBar()->showMessage(tr("Loading complete"), 4000);
	}
	catch (...)
	{
		statusBar()->showMessage(tr("Error! Can't load xrCore."));
		MessageBox(NULL, "Can't load xrCore!", "Init error", MB_OK | MB_ICONWARNING);
	}
}


/***********************************************
method for launch xrEngine.dll
***********************************************/
void xrLaunch::run_xrEngineRun() 
{
	try 
	{
		//#VERTVER: Critical moment: The compiler create code with SSE2 instructions 
		//#(only xrDevLauncher compiling with IA32-x86 instructions),
		//#some part of matrix and vectors use SSE3. It's can be difficult!
		if (CPUID::SSE2())
		{
			init_xrCore();
			statusBar()->showMessage(tr("Creating render list..."));
			CreateRendererList();
			statusBar()->showMessage(tr("Loading xrEngine..."), 4000);
			RunApplication(params.data());
#ifndef NOAWDA
			MessageBox(NULL,
				"Awda",
				"Awda",
				MB_OK | MB_ICONINFORMATION);
#endif
			xrLaunch::close();				// After closing xrCore main thread
		}
		else
		{
			statusBar()->showMessage(tr("Error! SSE2 is not supported on your CPU."));
			MessageBox(NULL,
				"Can't load xrEngine! SSE2 is not supported on your CPU.",
				"Init error",
				MB_OK | MB_ICONWARNING);
		}
	}
	catch (...)
	{
		statusBar()->showMessage(tr("Error! Can't load xrEngine."));
		MessageBox(NULL,
			"Can't load xrEngine",
			"Init error",
			MB_OK | MB_ICONWARNING);
	}
}


/***********************************************
method for close the MainWindow
***********************************************/
void xrLaunch::on_actionExit_triggered() 
{
	statusBar()->showMessage(tr("Closing launcher..."));
	xrLaunch::close();
}


/***********************************************
method for run xrEngine
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


/***********************************************
init xrCore without xrEngine
***********************************************/
void xrLaunch::on_actionxrCore_triggered()
{
	init_xrCore();
}
