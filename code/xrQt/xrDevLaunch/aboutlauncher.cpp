#include "aboutlauncher.h"
#include "ui_aboutlauncher.h"

AboutLauncher::AboutLauncher(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutLauncher)
{
    ui->setupUi(this);
}

AboutLauncher::~AboutLauncher()
{
    delete ui;
}
