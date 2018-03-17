/*************************************************
* X-ray Oxygen, 2018 (C)
*
* xrMain.cxx - Main source file for compilation with Qt
*
* xrLaunch
*************************************************/
#include "xrMain.h"
#include "ui_xrMain.h"
#include <windows.h>
void CreateRendererList();

xrLaunch::xrLaunch(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::xrLaunch)
{
    ui->setupUi(this);
	if (!CPUID::SSE41()) 
	{
		statusBar()->showMessage(tr("Your CPU doesn't support SSE4.1 and AVX instructions!"));
	}
	else if (!CPUID::AVX())
	{
		statusBar()->showMessage(tr("Your CPU doesn't support AVX instructions!"));
	}
	else
	{
		statusBar()->showMessage(tr("All instructions are supported on your CPU!"));
	}
}

xrLaunch::~xrLaunch()
{
    delete ui;
}
