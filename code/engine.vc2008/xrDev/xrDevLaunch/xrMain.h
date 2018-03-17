/*************************************************
* X-ray Oxygen, 2018 (C)
*
* xrMain.h - Main header for compilation with Qt
*
* main namespace, classes and vars.
*************************************************/

#pragma once
#include <QMainWindow>

namespace Ui {
class xrLaunch;
}

class xrLaunch : public QMainWindow
{
    Q_OBJECT

public:
    explicit xrLaunch(QWidget *parent = 0);
    ~xrLaunch();

private:
    Ui::xrLaunch *ui;
};
