/*************************************************
* VERTVER, 2018 (C)
* X-RAY OXYGEN 1.7 PROJECT
*
* Edited: 28 March, 2018
* aboutlauncher.cpp - soruce for "About" dialog
*************************************************/

/////////////////////////////////////////
#include "xrMain.h"
/////////////////////////////////////////
AboutLauncher::AboutLauncher(QWidget *parent) :
    QDialog(parent),
    about_ui(new Ui::AboutLauncher)
{
    about_ui->setupUi(this);
}

AboutLauncher::~AboutLauncher()
{
	delete about_ui;
}
