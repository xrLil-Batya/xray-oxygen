/*************************************************
* VERTVER, 2018 (C)
* X-RAY OXYGEN 1.7 PROJECT
*
* Edited: 28 March, 2018
* aboutlauncher.cpp - source for "About" dialog
*************************************************/

/////////////////////////////////////////
#include "xrMain.h"
#ifdef __cplusplus
/////////////////////////////////////////
AboutLauncher::AboutLauncher		(QWidget *parent) 
:	QDialog							(parent),
    about_ui						(new Ui::AboutLauncher)
{
    about_ui->setupUi					(this);
	QPixmap myPixmap("icon2.ico");
	about_ui->label_8->setPixmap(myPixmap);
}


AboutLauncher::~AboutLauncher		()
{
	delete about_ui;
}
#endif
