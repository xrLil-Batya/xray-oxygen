/********************************************************************************
** Form generated from reading UI file 'xrMain.ui'
**
** Created by: Qt User Interface Compiler version 5.10.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_XRMAIN_H
#define UI_XRMAIN_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_xrLaunch
{
public:
    QAction *actionxrEngine;
    QAction *actionxrPlay_2;
    QAction *action_silent;
    QAction *action_renderdebug;
    QAction *actionOther;
    QAction *actionxrEditorTools_in_dev;
    QAction *actionxrAI;
    QAction *actionOptions;
    QAction *actionView;
    QAction *actionSave_and_Exit;
    QAction *actionExit;
    QAction *actionMain;
    QAction *actionParametres;
    QWidget *centralWidget;
    QProgressBar *progressRunningBar;
    QPushButton *pushButton;
    QListWidget *listWidget;
    QMenuBar *menuBar;
    QMenu *menu;
    QMenu *menuGame;
    QMenu *menuxrEngine_with_options;
    QMenu *menuEditors;
    QMenu *menuOptions;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *xrLaunch)
    {
        if (xrLaunch->objectName().isEmpty())
            xrLaunch->setObjectName(QStringLiteral("xrLaunch"));
        xrLaunch->resize(800, 480);
        xrLaunch->setMinimumSize(QSize(800, 480));
        xrLaunch->setMaximumSize(QSize(800, 480));
        xrLaunch->setBaseSize(QSize(800, 800));
        QPalette palette;
        QBrush brush(QColor(23, 121, 157, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::WindowText, brush);
        QBrush brush1(QColor(18, 97, 126, 255));
        brush1.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Button, brush1);
        QBrush brush2(QColor(126, 126, 126, 255));
        brush2.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Light, brush2);
        QBrush brush3(QColor(122, 122, 122, 255));
        brush3.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Midlight, brush3);
        QBrush brush4(QColor(25, 25, 25, 255));
        brush4.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Dark, brush4);
        QBrush brush5(QColor(57, 57, 57, 255));
        brush5.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Mid, brush5);
        QBrush brush6(QColor(208, 208, 208, 255));
        brush6.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Text, brush6);
        QBrush brush7(QColor(26, 142, 184, 255));
        brush7.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::BrightText, brush7);
        QBrush brush8(QColor(202, 202, 202, 255));
        brush8.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::ButtonText, brush8);
        palette.setBrush(QPalette::Active, QPalette::Base, brush4);
        palette.setBrush(QPalette::Active, QPalette::Window, brush4);
        QBrush brush9(QColor(41, 41, 41, 255));
        brush9.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Shadow, brush9);
        QBrush brush10(QColor(222, 222, 222, 255));
        brush10.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::HighlightedText, brush10);
        palette.setBrush(QPalette::Active, QPalette::Link, brush);
        palette.setBrush(QPalette::Active, QPalette::AlternateBase, brush4);
        palette.setBrush(QPalette::Active, QPalette::NoRole, brush4);
        palette.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Button, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::Light, brush2);
        palette.setBrush(QPalette::Inactive, QPalette::Midlight, brush3);
        palette.setBrush(QPalette::Inactive, QPalette::Dark, brush4);
        palette.setBrush(QPalette::Inactive, QPalette::Mid, brush5);
        palette.setBrush(QPalette::Inactive, QPalette::Text, brush6);
        palette.setBrush(QPalette::Inactive, QPalette::BrightText, brush7);
        palette.setBrush(QPalette::Inactive, QPalette::ButtonText, brush8);
        palette.setBrush(QPalette::Inactive, QPalette::Base, brush4);
        palette.setBrush(QPalette::Inactive, QPalette::Window, brush4);
        palette.setBrush(QPalette::Inactive, QPalette::Shadow, brush9);
        palette.setBrush(QPalette::Inactive, QPalette::HighlightedText, brush10);
        palette.setBrush(QPalette::Inactive, QPalette::Link, brush);
        palette.setBrush(QPalette::Inactive, QPalette::AlternateBase, brush4);
        palette.setBrush(QPalette::Inactive, QPalette::NoRole, brush4);
        palette.setBrush(QPalette::Disabled, QPalette::WindowText, brush4);
        palette.setBrush(QPalette::Disabled, QPalette::Button, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Light, brush2);
        palette.setBrush(QPalette::Disabled, QPalette::Midlight, brush3);
        palette.setBrush(QPalette::Disabled, QPalette::Dark, brush4);
        palette.setBrush(QPalette::Disabled, QPalette::Mid, brush5);
        palette.setBrush(QPalette::Disabled, QPalette::Text, brush4);
        palette.setBrush(QPalette::Disabled, QPalette::BrightText, brush7);
        palette.setBrush(QPalette::Disabled, QPalette::ButtonText, brush4);
        palette.setBrush(QPalette::Disabled, QPalette::Base, brush4);
        palette.setBrush(QPalette::Disabled, QPalette::Window, brush4);
        palette.setBrush(QPalette::Disabled, QPalette::Shadow, brush9);
        palette.setBrush(QPalette::Disabled, QPalette::HighlightedText, brush10);
        palette.setBrush(QPalette::Disabled, QPalette::Link, brush);
        palette.setBrush(QPalette::Disabled, QPalette::AlternateBase, brush4);
        palette.setBrush(QPalette::Disabled, QPalette::NoRole, brush4);
        xrLaunch->setPalette(palette);
        actionxrEngine = new QAction(xrLaunch);
        actionxrEngine->setObjectName(QStringLiteral("actionxrEngine"));
        actionxrEngine->setCheckable(false);
        actionxrPlay_2 = new QAction(xrLaunch);
        actionxrPlay_2->setObjectName(QStringLiteral("actionxrPlay_2"));
        actionxrPlay_2->setCheckable(false);
        action_silent = new QAction(xrLaunch);
        action_silent->setObjectName(QStringLiteral("action_silent"));
        action_silent->setCheckable(false);
        action_renderdebug = new QAction(xrLaunch);
        action_renderdebug->setObjectName(QStringLiteral("action_renderdebug"));
        action_renderdebug->setCheckable(false);
        actionOther = new QAction(xrLaunch);
        actionOther->setObjectName(QStringLiteral("actionOther"));
        actionOther->setCheckable(false);
        actionxrEditorTools_in_dev = new QAction(xrLaunch);
        actionxrEditorTools_in_dev->setObjectName(QStringLiteral("actionxrEditorTools_in_dev"));
        actionxrEditorTools_in_dev->setCheckable(false);
        actionxrAI = new QAction(xrLaunch);
        actionxrAI->setObjectName(QStringLiteral("actionxrAI"));
        actionxrAI->setCheckable(false);
        actionOptions = new QAction(xrLaunch);
        actionOptions->setObjectName(QStringLiteral("actionOptions"));
        actionOptions->setCheckable(false);
        actionView = new QAction(xrLaunch);
        actionView->setObjectName(QStringLiteral("actionView"));
        actionView->setCheckable(false);
        actionSave_and_Exit = new QAction(xrLaunch);
        actionSave_and_Exit->setObjectName(QStringLiteral("actionSave_and_Exit"));
        actionSave_and_Exit->setEnabled(true);
        actionExit = new QAction(xrLaunch);
        actionExit->setObjectName(QStringLiteral("actionExit"));
        actionMain = new QAction(xrLaunch);
        actionMain->setObjectName(QStringLiteral("actionMain"));
        actionParametres = new QAction(xrLaunch);
        actionParametres->setObjectName(QStringLiteral("actionParametres"));
        centralWidget = new QWidget(xrLaunch);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        progressRunningBar = new QProgressBar(centralWidget);
        progressRunningBar->setObjectName(QStringLiteral("progressRunningBar"));
        progressRunningBar->setGeometry(QRect(430, 400, 271, 23));
        progressRunningBar->setMinimumSize(QSize(200, 23));
        progressRunningBar->setValue(0);
        progressRunningBar->setTextVisible(false);
        pushButton = new QPushButton(centralWidget);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setGeometry(QRect(710, 400, 80, 21));
        pushButton->setAutoDefault(false);
        pushButton->setFlat(false);
        listWidget = new QListWidget(centralWidget);
        listWidget->setObjectName(QStringLiteral("listWidget"));
        listWidget->setGeometry(QRect(10, 10, 251, 411));
        xrLaunch->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(xrLaunch);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 800, 20));
        menu = new QMenu(menuBar);
        menu->setObjectName(QStringLiteral("menu"));
        menuGame = new QMenu(menu);
        menuGame->setObjectName(QStringLiteral("menuGame"));
        menuxrEngine_with_options = new QMenu(menuGame);
        menuxrEngine_with_options->setObjectName(QStringLiteral("menuxrEngine_with_options"));
        menuEditors = new QMenu(menu);
        menuEditors->setObjectName(QStringLiteral("menuEditors"));
        menuOptions = new QMenu(menuBar);
        menuOptions->setObjectName(QStringLiteral("menuOptions"));
        xrLaunch->setMenuBar(menuBar);
        mainToolBar = new QToolBar(xrLaunch);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        xrLaunch->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(xrLaunch);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        xrLaunch->setStatusBar(statusBar);

        menuBar->addAction(menu->menuAction());
        menuBar->addAction(menuOptions->menuAction());
        menu->addAction(menuGame->menuAction());
        menu->addAction(menuEditors->menuAction());
        menu->addSeparator();
        menu->addAction(actionOptions);
        menu->addAction(actionView);
        menu->addSeparator();
        menu->addAction(actionExit);
        menuGame->addAction(actionxrEngine);
        menuGame->addAction(menuxrEngine_with_options->menuAction());
        menuGame->addAction(actionxrPlay_2);
        menuxrEngine_with_options->addAction(action_silent);
        menuxrEngine_with_options->addAction(action_renderdebug);
        menuxrEngine_with_options->addAction(actionOther);
        menuEditors->addAction(actionxrEditorTools_in_dev);
        menuEditors->addAction(actionxrAI);
        menuOptions->addAction(actionMain);
        menuOptions->addAction(actionParametres);

        retranslateUi(xrLaunch);

        pushButton->setDefault(false);


        QMetaObject::connectSlotsByName(xrLaunch);
    } // setupUi

    void retranslateUi(QMainWindow *xrLaunch)
    {
        xrLaunch->setWindowTitle(QApplication::translate("xrLaunch", "xrLaunch", nullptr));
        actionxrEngine->setText(QApplication::translate("xrLaunch", "xrEngine", nullptr));
        actionxrPlay_2->setText(QApplication::translate("xrLaunch", "xrPlay (Old oxygen)", nullptr));
        action_silent->setText(QApplication::translate("xrLaunch", "-silent", nullptr));
        action_renderdebug->setText(QApplication::translate("xrLaunch", "-renderdebug", nullptr));
        actionOther->setText(QApplication::translate("xrLaunch", "Other...", nullptr));
        actionxrEditorTools_in_dev->setText(QApplication::translate("xrLaunch", "xrEditorTools (in dev)", nullptr));
        actionxrAI->setText(QApplication::translate("xrLaunch", "xrAI", nullptr));
        actionOptions->setText(QApplication::translate("xrLaunch", "Parametres", nullptr));
        actionView->setText(QApplication::translate("xrLaunch", "View", nullptr));
        actionSave_and_Exit->setText(QApplication::translate("xrLaunch", "Save and Exit", nullptr));
        actionExit->setText(QApplication::translate("xrLaunch", "Exit", nullptr));
        actionMain->setText(QApplication::translate("xrLaunch", "Main", nullptr));
        actionParametres->setText(QApplication::translate("xrLaunch", "Parametres of load", nullptr));
        pushButton->setText(QApplication::translate("xrLaunch", "Run", nullptr));
        menu->setTitle(QApplication::translate("xrLaunch", "Launch", nullptr));
        menuGame->setTitle(QApplication::translate("xrLaunch", "Game...", nullptr));
        menuxrEngine_with_options->setTitle(QApplication::translate("xrLaunch", "xrEngine (with options)", nullptr));
        menuEditors->setTitle(QApplication::translate("xrLaunch", "Editors", nullptr));
        menuOptions->setTitle(QApplication::translate("xrLaunch", "Options", nullptr));
    } // retranslateUi

};

namespace Ui {
    class xrLaunch: public Ui_xrLaunch {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_XRMAIN_H
