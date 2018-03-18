/*************************************************
* Oxygen Team, 2018 (C)
*
* xrMain.h - Main header for compilation with Qt
*
* main namespace, classes and vars.
*************************************************/

#pragma once
/////////////////////////////////////////
#include <QMainWindow>
#include <QPushButton>
#include <windows.h>
#include <thread>
#include "minimal_CPUID.h"
#include "ui_xrMain.h"
#include <QApplication>
/////////////////////////////////////////
#define DLL_API __declspec(dllimport)
#define CXX 199711L
/////////////////////////////////////////

void CreateRendererList();

namespace Ui {
class xrLaunch;
}

class xrLaunch : public QMainWindow
{
    Q_OBJECT

public:
    explicit xrLaunch(QWidget *parent = 0);
    ~xrLaunch();

private slots:
    void on_pushButton_clicked();			// Run
    void on_actionExit_triggered();			// Menu->Exit
    void on_actionxrEngine_triggered();		// Menu->Launch->Game->xrEngine

private:
    Ui::xrLaunch *ui;
};
