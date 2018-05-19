/*************************************************
* VERTVER, 2018 (C)
* X-RAY OXYGEN 1.7 PROJECT
*
* Edited: 10 May, 2018
* xrMain.cpp - Main source file for compilation with Qt
* xrLaunch
*************************************************/
#include "xrMain.h"
/////////////////////////////////////////
#ifdef __cplusplus
#pragma comment(lib, "xrEngine.lib")
/////////////////////////////////////////
xrString		szParams;
xrString		szStringAccept;
xrString		szParamsList;
xrString		szParamsLine;
xrString		szParamsString;
xrString		szParamsSettings;
xrString		szParamsBox;
FileSystem		fss;
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
	ui->setupUi(this);
	init_xrCore();
	if (CPUID::AMD() | CPUID::AMDelse())
	{
		Msg("xrDev: CPU Vendor: AMD");
	}
	else
	{
		Msg("xrDev: CPU Vendor: Intel");
	}
	if (!CPUID::SSE3())
	{
		Msg								("xrDev: unsupported instructions: SSE3, SSE4.1, AVX");
		statusBar()->showMessage		(tr("Warning! Your CPU doesn't support SSE3 instructions."), 12000);
	}
	else if (!CPUID::SSE41())
	{
		Msg								("xrDev: unsupported instructions: SSE4.1, AVX");
		statusBar()->showMessage		(tr("Your CPU doesn't support SSE4.1 and AVX instructions!"), 6000);
	}
	else if (!CPUID::AVX())
	{
		Msg								("xrDev: unsupported instructions: AVX");
		statusBar()->showMessage		(tr("Your CPU doesn't support AVX instructions!"), 6000);
	}
	else
	{
		statusBar()->showMessage		(tr("All instructions are supported on your CPU!"), 6000);
	}

		/////////////////////////////////////////
		ui->listWidget->addItems	(LIST_ITEMS);
		ui->listWidget_2->addItems	(LIST_ITEMS_SETTINGS);
}



/***********************************************
available renders
***********************************************/
void xrLaunch::status_render()
{
	xrConstChar renders;
	if (SupportsDX11Rendering())
		renders							= "Supported renders: R4, R3, R2.5, R2, R2a";
	else if (SupportsDX10Rendering())
		renders							= "Supported renders: R3, R2.5, R2, R2a";
	else if (SupportsAdvancedRendering())
		renders							= "Supported renders: R2.5, R2, R2a";
	else
	{
		renders							= "Error! Your GPU doesn't supported";
		QMessageBox::critical			(this, INIT_ERROR, "Error! Your GPU doesn't supported (DX9 init error)");
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
	run_xrEngine 					();
}


/***********************************************
add parameter if pressed
***********************************************/
void xrLaunch::add_paramsToList()
{

	try
	{
		xrQString qszListSettings			= ui->listWidget_2->currentItem()->text();
		szParamsSettings					= qszListSettings.toLocal8Bit();
		if (szParamsSettings.empty())
		{
			throw new EcxeptionOpSet		("std::string can't be nullptr",
											OpSet::STRING_NULLPTR);
		}
		statusBar()->showMessage		(tr("Added to settings buffer"), 2000);
	}
	catch							(const EcxeptionOpSet& ex)
	{
		xrConstChar lcOpset;
		switch (ex.opset())
		{
			case OpSet::STRING_NULLPTR:
				lcOpset				= "string_nullptr";
				break;
			default:
				lcOpset = "default";
				break;
		}
			statusBar()->showMessage		(tr(ex.what(), lcOpset));
	}
	catch (...)
	{
#ifdef DEBUG_LAUNCHER
		MessageBox						(NULL, "ERROR", "Error: Can't add string to string buffer", MB_OK | MB_ICONINFORMATION);
#else
		statusBar()->showMessage		(tr("Error: Can't add string to settings buffer"));
		Msg								("xrDev: string_settings buffer error");
#endif
	}
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
	try 
	{
		xrQString qszString				= ui->listWidget->currentItem()->text();
		szParamsString					= qszString.toLocal8Bit();	//#VERTVER: Don't use here toLatin1(). Crash on Release
		if (szParamsString.empty())
		{
			throw new EcxeptionOpSet	("std::string can't be nullptr",
										OpSet::STRING_NULLPTR);
		}
		statusBar()->showMessage		(tr("Added to string buffer"), 2000);
	}
	catch (const EcxeptionOpSet& ex)
	{
		xrConstChar lcOpset;
		switch (ex.opset())
		{
			case OpSet::STRING_NULLPTR:
				lcOpset				= "string_nullptr";
				break;
			default:
				lcOpset				= "default";
				break;
		}
		statusBar()->showMessage		(tr(ex.what(), lcOpset));

	}
	catch (...)
	{

#ifdef DEBUG_LAUNCHER
		MessageBox						(NULL, "ERROR", "Error: Can't add string to string buffer", MB_OK | MB_ICONHAND);
#else
		statusBar()->showMessage		(tr("Error: Can't add string to string buffer"));
		Msg								("xrDev: string buffer error");
#endif
	}

}


/***********************************************
clean the buffers of params
************************************************/
void xrLaunch::clean_buffers()
{
	if (!szParamsSettings.empty() && !szParamsString.empty())
	{
		szParamsSettings.clear			();
		szParamsString.clear			();
		statusBar()->showMessage		(tr("The string buffers were cleaned"), 2000);
		Msg								("xrDev: The string buffers were cleaned");
	}
	else if (!szParamsSettings.empty())
	{ 
		szParamsSettings.clear			();
		statusBar()->showMessage		(tr("The string buffer was cleaned"), 2000);
		Msg								("xrDev: The string buffer was cleaned");
	}
	else if (!szParamsString.empty())
	{ 
		szParamsString.clear			();
		statusBar()->showMessage		(tr("The string buffer was cleaned"), 2000);
		Msg								("xrDev: The string buffer was cleaned");
	}
	else
		statusBar()->showMessage		(tr("There is nothing to clean"), 2000);
}


/***********************************************
method for button
************************************************/
void xrLaunch::on_pushButton_2_clicked()
{
	clean_buffers					();
}


/***********************************************
init xrCore
************************************************/
void xrLaunch::init_xrCore() 
{
	try
	{
		if (!CPUID::SSE3()) 
		{
			throw new EcxeptionOpSet	("Your CPU doesn't support SSE3 instructions",
										OpSet::SSE3);
		}
		// Init X-ray core

		
		statusBar()->showMessage		(tr("Loading xrCore..."));
		Msg								("xrDev: Loading xrCore...");
		Debug._initialize				(false);
		Core._initialize				("X-Ray Oxygen", nullptr, TRUE, "fsgame.ltx");
		statusBar()->showMessage		(tr("Loading complete"), 4000);
		Msg								("xrDev: xrCore already loaded");
	}
	catch (const EcxeptionOpSet& ex)
	{
		switch (ex.opset())
		{
		case OpSet::SSE3:
			statusBar()->showMessage		(tr(INIT_ERROR, ex.what()));
			QMessageBox::critical			(this, INIT_ERROR, ex.what());
			break;
		default:
			statusBar()->showMessage		(tr(INIT_ERROR, ex.what()));
			QMessageBox::critical			(this, INIT_ERROR, ex.what());
			break;
		}
	}
	catch (...)
	{
		Msg								("xrDev: Engine init error (Unknown Error)");
		statusBar()->showMessage		(tr(INIT_ERROR, "Engine init error (Unknown Error)"));
		QMessageBox::critical			(this, INIT_ERROR, "Engine init error (Unknown Error)");
	}
}


/***********************************************
method for launch xrEngine.dll
***********************************************/
void xrLaunch::run_xrEngine() 
{
	try
	{
		if (!CPUID::SSE3())
		{
			Msg								("xrDev: exception: SSE3 isn't supported");
			throw new EcxeptionOpSet		("Your CPU doesn't support SSE3 instructions", OpSet::SSE3);
		}
		QString rendered_line			= ui->lineEdit->text();
		szParamsLine					= rendered_line.toLocal8Bit();
#ifdef DEBUG_LAUNCHER
		xrConstChar c1					= szParamsLine.c_str();
		xrConstChar c2					= szParamsSettings.c_str();
		xrConstChar c3					= szParamsString.c_str();
		xrConstChar c4					= szParams.c_str();
		MessageBox						(NULL, c1, "params_line",		MB_OK | MB_ICONINFORMATION);
		MessageBox						(NULL, c2, "params_settings",	MB_OK | MB_ICONINFORMATION);
		MessageBox						(NULL, c3, "params_string",		MB_OK | MB_ICONINFORMATION);
		MessageBox						(NULL, c4, "params_string",		MB_OK | MB_ICONINFORMATION);
#endif
		szParams						= szParamsString + " " + szParamsLine + " " + szParamsSettings + " " + szParamsBox;
		init_xrCore						();
		ui->progressBar->setValue		(33);
		Msg								("xrDev: Creating render list...");
		statusBar()->showMessage		(tr("Creating render list..."));
		CreateRendererList				();
		ui->progressBar->setValue		(66);
		Msg								("xrDev: Loading xrEngine...");
		statusBar()->showMessage		(tr("Loading xrEngine..."), 4000);
		ui->progressBar->setValue		(100);
		RunApplication					(szParams.data());
#ifndef NOAWDA
		QMessageBox::information		(this, "Awda", "Awda");
#endif
		// After closing xrEngine thread
		xrLaunch::close					();				

	}
	catch (const EcxeptionOpSet& ex)
	{
		switch (ex.opset())
		{
		case OpSet::SSE3:	
			Msg								(INIT_ERROR, ex.what());
			statusBar()->showMessage		(tr(INIT_ERROR, ex.what()));
			QMessageBox::critical			(this, INIT_ERROR, ex.what());
			break;
		default:
			Msg								(INIT_ERROR, ex.what());
			statusBar()->showMessage		(tr(INIT_ERROR, ex.what()));
			QMessageBox::critical			(this, INIT_ERROR, ex.what());
			break;
		}
	}
	catch (...)
	{
		Msg								(INIT_ERROR, "Engine init error (Unknown Error)");
		statusBar()->showMessage		(tr(INIT_ERROR, "Engine init error (Unknown Error)"));
		QMessageBox::critical			(this, INIT_ERROR, "Engine init error (Unknown Error)");
	}
}


/***********************************************
method for close the MainWindow
***********************************************/
void xrLaunch::on_actionExit_triggered() 
{
	Msg								("xrDev: closing...");
	statusBar()->showMessage		(tr("Closing launcher..."));
	xrLaunch::close					();
}


/***********************************************
method for run xrEngine
***********************************************/
void xrLaunch::on_actionxrEngine_triggered() 
{
	run_xrEngine					();
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
	Msg								("xrDev: aboutlauncher loaded");
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
	szParamsBox						= Qparams.toLocal8Bit();
}


/***********************************************
link to OxyTeam
***********************************************/
void xrLaunch::on_actionAbout_Oxygen_Team_triggered()
{
    xrQString oxylink				= "https://github.com/xrOxygen";
    QDesktopServices::openUrl		(QUrl(oxylink));
}


/***********************************************
launch without params
***********************************************/
void xrLaunch::on_actionxrEngine_2_triggered()
{
    /////////////////////////////////////////
    szParamsSettings				= " ";
	szParamsLine.clear				();
	szParamsSettings.clear			();
	szParamsBox.clear				();
    /////////////////////////////////////////
    run_xrEngine					();
}
#endif

