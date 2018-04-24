/*************************************************
* VERTVER, 2018 (C)
* X-RAY OXYGEN 1.7 PROJECT
*
* Edited: 24 April, 2018
* xrMain.cpp - Main source file for compilation with Qt
* xrLaunch
*************************************************/
#include "xrMain.h"
/////////////////////////////////////////
#pragma comment(lib, "xrEngine.lib")
/////////////////////////////////////////
xrConstChar		inerr			= "Init error";
xrConstChar		params_list;
xrConstChar		string_accept;
xrString		params;
xrString		params_line;
xrString		params_string;
xrString		params_settings;
xrString		params_box;
/////////////////////////////////////////
void CreateRendererList			();
bool SupportsAdvancedRendering	();
bool SupportsDX10Rendering		();
bool SupportsDX11Rendering		();
/////////////////////////////////////////

/***********************************************
init UI 
***********************************************/
xrLaunch::xrLaunch				(QWidget *parent) 
:	QMainWindow					(parent),
    ui							(new Ui::xrLaunch) 
{
	ui->setupUi							(this);
	if (!CPUID::SSE3())
		statusBar()->showMessage		(tr("Warning! Your CPU doesn't support SSE3 instructions."), 12000);
	else if (!CPUID::SSE41()) 
		statusBar()->showMessage		(tr("Your CPU doesn't support SSE4.1 and AVX instructions!"), 6000);
	else if (!CPUID::AVX())
		statusBar()->showMessage		(tr("Your CPU doesn't support AVX instructions!"), 6000);
	else
		statusBar()->showMessage		(tr("All instructions are supported on your CPU!"), 6000);
	/////////////////////////////////////////
	ui->listWidget	->addItems		( LIST_ITEMS );
	ui->listWidget_2->addItems		( LIST_ITEMS_SETTINGS );
}


/***********************************************
available renders
***********************************************/
void xrLaunch::status_render()
{
	char const* renders;
	if (SupportsDX11Rendering())
		renders = "Supported renders: R4, R3, R2.5, R2, R2a";
	else if (SupportsDX10Rendering())
		renders = "Supported renders: R3, R2.5, R2, R2a";
	else if (SupportsAdvancedRendering())
		renders = "Supported renders: R2.5, R2, R2a";
	else
	{
		renders = "Error! Your GPU doesn't supported";
		QMessageBox::critical		(this, inerr, "Error! Your GPU doesn't supported (DX9 init error)");
	}
	statusBar()->showMessage		(tr(renders));
}


/***********************************************
init parameters dialog
***********************************************/
xrDialogParam::xrDialogParam	(QWidget *parent) 
: 	QDialog						(parent),
	uiDialog					(new Ui::Dialog)
{
	uiDialog->setupUi				(this);
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
running the DLL (xrEngine)
***********************************************/
DLL_API int RunApplication(char* commandLine);


/***********************************************
method for run xrEngine
***********************************************/
void xrLaunch::on_pushButton_clicked() 
{
	run_xrEngineRun					();
}




/***********************************************
add parameter if pressed
***********************************************/
void xrLaunch::add_paramsToList()
{
	xrQString list_settins			= ui->listWidget_2->currentItem()->text();
	params_settings					= list_settins.toLocal8Bit();
	statusBar()->showMessage		(tr("Added to settings buffer"), 2000);
}


void xrLaunch::on_listWidget_2_itemPressed(QListWidgetItem *item)
{
	add_paramsToList				();
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
	//#VERTVER: Don't use here toLatin1(). Crash on Release
	xrQString list_string				= ui->listWidget->currentItem()->text();
	params_string					= list_string.toLocal8Bit();
	statusBar()->showMessage		(tr("Added to string buffer"), 2000);
}


/***********************************************
init xrCore
************************************************/
void xrLaunch::init_xrCore() 
{
	try
	{
		// Init X-ray core
		statusBar()->showMessage		(tr("Loading xrCore..."));
		Debug._initialize				(false);
		Core._initialize				("X-Ray Oxygen", nullptr, TRUE, "fsgame.ltx");
		statusBar()->showMessage		(tr("Loading complete"), 4000);
	}
	catch (...)
	{
		if (!CPUID::SSE3())
		{
			statusBar()->showMessage	(tr("Error! SSE3 is not supported on your CPU."));
			QMessageBox::critical		(this, inerr, "Can't load xrCore (SSE3 is not supported on your CPU");
		}
		else
		{
			statusBar()->showMessage	(tr("Error! Can't load xrEngine."));
			QMessageBox::critical		(this, inerr, "Can't load xrCore (Unknown Error)");
		}
	}
}


/***********************************************
method for launch xrEngine.dll
***********************************************/
void xrLaunch::run_xrEngineRun() 
{
	try 
	{
		//ui->progressBar->setValue		(5);
		xrQString rendered_line			= ui->lineEdit->text();
		params_line						= rendered_line.toLocal8Bit();
#ifdef DEBUG_LAUNCHER
		xrConstChar c1					= params_line.c_str();
		xrConstChar c2					= params_settings.c_str();
		xrConstChar c3					= params_string.c_str();
		xrConstChar c4					= params.c_str();
		//MessageBox						(NULL, c1, "params_line",		MB_OK | MB_ICONINFORMATION);
		MessageBox						(NULL, c2, "params_settings",	MB_OK | MB_ICONINFORMATION);
		//MessageBox						(NULL, c3, "params_string",		MB_OK | MB_ICONINFORMATION);
#endif
		//#VERTVER: Короче, идите нахуй. params_settings правильно определяется, но движку похуй.
		params							= params_string + " " +  params_line + params_settings + " " + params_box ;
		init_xrCore						();
		ui->progressBar->setValue		(30);
		statusBar()->showMessage		(tr("Creating render list..."));
		CreateRendererList				();
		ui->progressBar->setValue		(45);
		statusBar()->showMessage(tr		("Loading xrEngine..."), 4000);
		ui->progressBar->setValue		(100);
		RunApplication					(params.data());
#ifndef NOAWDA
		QMessageBox::information		(this, "Awda", "Awda");
#endif
		// After closing xrEngine thread
		xrLaunch::close					();				
	}
	catch (...)
	{
		ui->progressBar->setValue		(0);
		statusBar()->showMessage		(tr("Error! Can't load xrEngine."));
		QMessageBox::critical			(this, inerr, "Can't load xrEngine (Unknown Error)");
	}
}


/***********************************************
method for close the MainWindow
***********************************************/
void xrLaunch::on_actionExit_triggered() 
{
	statusBar()->showMessage		(tr("Closing launcher..."));
	xrLaunch::close					();
}


/***********************************************
method for run xrEngine
***********************************************/
void xrLaunch::on_actionxrEngine_triggered() 
{
	run_xrEngineRun					();
}


/***********************************************
link to Oxygen Team page
***********************************************/
void xrLaunch::on_actionOxygen_Team_triggered()
{
	xrQString oxylink				= "https://github.com/xrOxygen";
	QDesktopServices::openUrl		(QUrl(oxylink));
}


/***********************************************
link to Vertver page
***********************************************/
void xrLaunch::on_actionVertver_triggered()
{
	xrQString oxylink				= "https://github.com/Vertver";
	QDesktopServices::openUrl		(QUrl(oxylink));
}


/***********************************************
link to Giperion page
***********************************************/
void xrLaunch::on_actionGiperion_2_triggered()
{
	xrQString oxylink				= "https://github.com/Giperionn";
	QDesktopServices::openUrl		(QUrl(oxylink));
}


/***********************************************
link to ForserX page
***********************************************/
void xrLaunch::on_actionForserX_triggered()
{
    xrQString oxylink				= "https://github.com/ForserX";
	QDesktopServices::openUrl		(QUrl(oxylink));
}


/***********************************************
open the new window with xrEngine params (doesn't works)
***********************************************/
void xrLaunch::on_actionParametres_triggered()
{
	xrDialogParam *dlg				= new xrDialogParam;
	dlg->show();
}


/***********************************************
create a new window with "About" dialog
***********************************************/
void xrLaunch::on_actionVertver_Github_triggered()
{
	AboutLauncher *dlg				= new AboutLauncher;
	dlg->setWindowFlags				(Qt::WindowStaysOnTopHint);
	dlg->setWindowTitle				("About xrDev");
	dlg->show();
	
}


/***********************************************
init xrCore without xrEngine
***********************************************/
void xrLaunch::on_actionxrCore_triggered()
{
	init_xrCore						();
	QMessageBox::information		(this, "Init xrCore complete!", "Please, close the window to continue.");
}



/***********************************************
if pressed "Accept"
***********************************************/
void xrDialogParam::on_buttonBox_accepted()
{
	xrQString Qparams				= uiDialog->lineEdit->text();
	params_box						= Qparams.toLocal8Bit();
}


/***********************************************
link to OxyTeam
***********************************************/
void xrLaunch::on_actionAbout_Oxygen_Team_triggered()
{
    xrQString oxylink				= "https://github.com/xrOxygen";
    QDesktopServices::openUrl		(QUrl(oxylink));
}
