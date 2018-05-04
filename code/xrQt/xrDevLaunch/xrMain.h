void on_actionxrEngine_2_triggered();
/*************************************************
* VERTVER, 2018 (C)
* X-RAY OXYGEN 1.7 PROJECT
*
* Edited: 28 April, 2018
* xrMain.h - Main header for compilation with Qt
* main namespace, classes and vars.
*************************************************/

#pragma once

/////////////////////////////////////////
#include "ui_xrMain.h"
#include "ui_parametersdialog.h"
#include "ui_aboutlauncher.h"
#include "ui_xrSettings.h"
#include "xrFS.h"
#include "xrCPUID.h"
#include "xrSettings.h"
#include "xrException.h"
#include "aboutlauncher.h"
#include "../../engine.vc2008/xrCore/xrCore.h"
#include "../../engine.vc2008/xrCore/cpuid.h"

/////////////////////////////////////////
#include <stdio.h>
#include <QMainWindow>
#include <QPushButton>
#include <windows.h>
#include <QApplication>
#include <QDesktopServices>
#include <QUrl>
#include <QDialog>
#include <QMessageBox>
/////////////////////////////////////////
#define INIT_ERROR		"Init error: "
#define DLL_API			__declspec(dllimport)
#define NOAWDA			1
#define NOXRCORE_LOAD	1s
#ifdef DEBUG
#define LAUNCHER_DEBUG	1
#endif
#ifdef __cplusplus
/////////////////////////////////////////
typedef std::string		xrString;
typedef LPCSTR			xrConstChar;
typedef LPSTR			xrChar;
typedef QString			xrQString;
typedef QStringList		xrQStringList;
/////////////////////////////////////////
static const xrQStringList LIST_ITEMS			= xrQStringList() 
/////////////////////////////////////////
<< "-r2a" 
<< "-r2" 
<< "-r2.5" 
<< "-r3" 
<< "-r4" 
#ifdef DEBUG_LAUNCHER
<< ""
#endif
/////////////////////////////////////////
<< "-renderdebug" 
<< "-nointro"
<< "-mblur";
/////////////////////////////////////////

static const xrQStringList LIST_ITEMS_SETTINGS	= xrQStringList()
/////////////////////////////////////////
<< "-$_preset Minimum "
<< "-$_preset Low "
<< "-$_preset Default "
<< "-$_preset High "
<< "-$_preset Extreme "
/////////////////////////////////////////
<< "-$rs_fullscreen 1 "
<< "-$rs_fullscreen 0 "
<< "-$rs_draw_fps 1 "
<< "-$rs_draw_fps 0 "
<< "-$rs_detail 1 "
<< "-$rs_detail 0 "
/////////////////////////////////////////
<< "-$g_god 1 "
<< "-$g_god 0 "
/////////////////////////////////////////
<< "-$hud_crosshair 1 "
<< "-$hud_crosshair 0 "
<< "-$hud_crosshair_dist 1 "
<< "-$hud_crosshair_dist 0 "
<< "-$hud_draw 1 "
<< "-$hud_draw 0 "
<< "-$hud_info 1 "
<< "-$hud_info 0 "
<< "-$hud_weapon 1 "
<< "-$hud_weapon 0 "
/////////////////////////////////////////
<< "-$r2_fxaa 1 "
<< "-$r2_fxaa 0 "
<< "-$r2_gi 1 "
<< "-$r2_gi 0 "
<< "-$r2_ssao_blur 1 "
<< "-$r2_ssao_blur 0 "
<< "-$r2_ssao st_opt_off "
<< "-$r2_ssao st_opt_low "
<< "-$r2_ssao st_opt_medium "
<< "-$r2_ssao st_opt_high "
<< "-$r2_ssao_hbao 1 "
<< "-$r2_ssao_hbao 0 "
<< "-$r2_detail_bump 1 "
<< "-$r2_detail_bump 0 "
<< "-$r2_mblur_enabled 1 "
<< "-$r2_mblur_enabled 0 "
<< "-$r2_soft_particles 1 "
<< "-$r2_soft_particles 0 "
<< "-$r2_soft_water 1 "
<< "-$r2_soft_water 0 "
<< "-$r2_sun 1 "
<< "-$r2_sun 0 "
<< "-$r2_tonemap 1 "
<< "-$r2_tonemap 0 "
<< "-$r2_volumetric_lights 1 "
<< "-$r2_volumetric_lights 0 "
/////////////////////////////////////////
<< "-$r3_gbuffer_opt 1 "
<< "-$r3_gbuffer_opt 0 "
<< "-$r3_msaa st_opt_off "
<< "-$r3_msaa 2x "
<< "-$r3_msaa 4x "
<< "-$r3_use_dx10_1 1 "
<< "-$r3_use_dx10_1 0 "
<< "-$r3_volumetric_smoke 1 "
<< "-$r3_volumetric_smoke 0 "
/////////////////////////////////////////
<< "-$r4_enable_tessellation 1 "
<< "-$r4_enable_tessellation 0 "
<< "-$r4_wireframe 1 "
<< "-$r4_wireframe 0 ";
/////////////////////////////////////////

namespace Ui {
class xrLaunch;			// main window
class xrDialogParam;	// parameters dialog
}

class xrDialogParam 
: public QDialog 
{
	Q_OBJECT
public: 	
	explicit xrDialogParam						(QWidget *parent = NULL);
	~xrDialogParam								();

private slots:
    void on_buttonBox_accepted					();

private:
	Ui::Dialog		*uiDialog;
};

class xrLaunch 
: public QMainWindow
{
    Q_OBJECT

public:
    explicit xrLaunch							(QWidget *parent = 0);
	~xrLaunch									();

private slots:
    void on_pushButton_clicked					();		// Run
	void run_xrEngine							();		// Run the xrEngine
	void on_actionExit_triggered				();		// Menu->Launch->Exit
    void on_actionxrEngine_triggered			();		// Menu->Launch->Game->xrEngine
    void on_actionxrCore_triggered				();
    void on_actionOxygen_Team_triggered			();
    void on_actionVertver_triggered				();
    void on_actionGiperion_2_triggered			();
    void on_actionForserX_triggered				();
    void on_actionParametres_triggered			();	
	void on_actionVertver_Github_triggered		();
    void on_actionAbout_Oxygen_Team_triggered	();
	void on_actionxrEngine_2_triggered			();
	void clean_buffers							();
	void on_pushButton_2_clicked				();
    void on_listWidget_itemPressed				(QListWidgetItem *item);
    void on_listWidget_2_itemPressed			(QListWidgetItem *item);
	void status_render							();
	void add_stringToList						();
	void add_paramsToList						();
	void init_xrCore							();

private:
    Ui::xrLaunch	*ui;

};
#endif
