/*************************************************
* VERTVER, 2018 (C)
* X-RAY OXYGEN 1.7 PROJECT
*
* Edited: 30 April, 2018
* xrSettings - Settings menu for launcher
* xrSettings
*************************************************/
#include "xrFS.h"
#include "xrMain.h"
#include "qrect.h"
/////////////////////////////////////////
FileSystem fs;
xrSettings::xrSettings		(QWidget *parent) 
:	QDialog					(parent),
	settings_ui				(new Ui::xrSettings)
{
    settings_ui->setupUi		(this);
	loadSettings();
}

xrSettings::~xrSettings()
{
    delete settings_ui;
}


void xrSettings::loadSettings()
{
	LPCSTR szSetting		= "xrCoreInit";
	QSettings qs			("OxyTeam", "xrDevLaunch");
	qs.beginGroup			("main");
	i1						= qs.value(szSetting).toInt();
	qs.endGroup				();
}


void xrSettings::saveSettings()
{
	LPCSTR szSetting		= "xrCoreInit";
	QSettings qs			("OxyTeam", "xrDevLaunch");
	qs.beginGroup			("main");
	qs.setValue				(szSetting, i1);
	qs.endGroup				();
}


void xrSettings::on_buttonBox_accepted()
{
	saveSettings			();
}


void xrSettings::on_checkBox_stateChanged(int arg1)
{
	i1 = arg1;
}