/*************************************************
* X-ray Oxygen, 2018 (C)
*
* xrMain.h - Main header for compilation with Qt
*
* main namespace, classes and vars.
*************************************************/

#pragma once
#include <QMainWindow>
#include <QPushButton>
#include "minimal_CPUID.h"
//#define DLL_API __declspec(dllimport)

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
    void on_pushButton_clicked();

    void on_actionExit_triggered();

private:
    Ui::xrLaunch *ui;
};
