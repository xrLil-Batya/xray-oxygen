/*************************************************
* VERTVER, 2018 (C)
* X-RAY OXYGEN 1.7 PROJECT
*
* Edited: 30 April, 2018
* xrSettings - Settings menu for launcher
* xrSettings
*************************************************/
#pragma once
/////////////////////////////////////////
#include "xrMain.h"
/////////////////////////////////////////

namespace Ui {
class xrSettings;
}

class xrSettings : public QDialog
{
    Q_OBJECT

public:
    explicit xrSettings					(QWidget *parent = 0);
    ~xrSettings							();
	void loadSettings					();
	void saveSettings					();
	int  setxrCore()					{ return i1; }
	int	i1, i2, i3, i4, i5, i6, i7, i8, i9;


private slots:
    void on_checkBox_stateChanged		(int arg1);
    void on_buttonBox_accepted			();

private:
    Ui::xrSettings *settings_ui;
};
