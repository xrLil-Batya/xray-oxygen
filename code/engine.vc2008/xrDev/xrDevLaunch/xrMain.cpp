/*************************************************
* X-ray Oxygen, 2018 (C)
*
* xrMain.cxx - Main source file for compilation with Qt
*
* xrLaunch
*************************************************/
#include "xrMain.h"
#include "ui_xrMain.h"

xrLaunch::xrLaunch(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::xrLaunch)
{
    ui->setupUi(this);
	statusBar()->showMessage(tr("Loading complete"));
}

xrLaunch::~xrLaunch()
{
    delete ui;
}
