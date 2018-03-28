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
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
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
    QAction *actionExit;
    QAction *actionMain;
    QAction *actionParametres;
    QAction *actionVertver_Github;
    QAction *actionGiperion;
    QAction *actionMain_parameters;
    QAction *actionAbout_Oxygen_Team;
    QAction *actionOxygen_Team;
    QAction *actionVertver;
    QAction *actionGiperion_2;
    QAction *actionForserX;
    QWidget *centralWidget;
    QPushButton *pushButton;
    QListWidget *listWidget;
    QLineEdit *lineEdit;
    QMenuBar *menuBar;
    QMenu *menu;
    QMenu *menuGame;
    QMenu *menuxrEngine_with_options;
    QMenu *menuEditors;
    QMenu *menuOptions;
    QMenu *menuAbout;
    QMenu *menuGithub_Pages;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *xrLaunch)
    {
        if (xrLaunch->objectName().isEmpty())
            xrLaunch->setObjectName(QStringLiteral("xrLaunch"));
        xrLaunch->setWindowModality(Qt::WindowModal);
        xrLaunch->resize(800, 480);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(xrLaunch->sizePolicy().hasHeightForWidth());
        xrLaunch->setSizePolicy(sizePolicy);
        xrLaunch->setMinimumSize(QSize(800, 480));
        xrLaunch->setMaximumSize(QSize(16777215, 16777215));
        xrLaunch->setBaseSize(QSize(0, 0));
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
        QBrush brush4(QColor(30, 30, 30, 255));
        brush4.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Dark, brush4);
        QBrush brush5(QColor(57, 57, 57, 255));
        brush5.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Mid, brush5);
        QBrush brush6(QColor(25, 147, 184, 255));
        brush6.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Text, brush6);
        QBrush brush7(QColor(32, 181, 230, 255));
        brush7.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::BrightText, brush7);
        QBrush brush8(QColor(25, 86, 126, 255));
        brush8.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::ButtonText, brush8);
        palette.setBrush(QPalette::Active, QPalette::Base, brush4);
        palette.setBrush(QPalette::Active, QPalette::Window, brush4);
        QBrush brush9(QColor(41, 41, 41, 255));
        brush9.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Shadow, brush9);
        palette.setBrush(QPalette::Active, QPalette::HighlightedText, brush8);
        palette.setBrush(QPalette::Active, QPalette::Link, brush);
        palette.setBrush(QPalette::Active, QPalette::AlternateBase, brush4);
        QBrush brush10(QColor(25, 25, 25, 255));
        brush10.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::NoRole, brush10);
        palette.setBrush(QPalette::Active, QPalette::ToolTipText, brush7);
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
        palette.setBrush(QPalette::Inactive, QPalette::HighlightedText, brush8);
        palette.setBrush(QPalette::Inactive, QPalette::Link, brush);
        palette.setBrush(QPalette::Inactive, QPalette::AlternateBase, brush4);
        palette.setBrush(QPalette::Inactive, QPalette::NoRole, brush10);
        palette.setBrush(QPalette::Inactive, QPalette::ToolTipText, brush7);
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
        palette.setBrush(QPalette::Disabled, QPalette::HighlightedText, brush8);
        palette.setBrush(QPalette::Disabled, QPalette::Link, brush);
        palette.setBrush(QPalette::Disabled, QPalette::AlternateBase, brush4);
        palette.setBrush(QPalette::Disabled, QPalette::NoRole, brush10);
        palette.setBrush(QPalette::Disabled, QPalette::ToolTipText, brush7);
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
        actionExit = new QAction(xrLaunch);
        actionExit->setObjectName(QStringLiteral("actionExit"));
        actionMain = new QAction(xrLaunch);
        actionMain->setObjectName(QStringLiteral("actionMain"));
        actionParametres = new QAction(xrLaunch);
        actionParametres->setObjectName(QStringLiteral("actionParametres"));
        actionVertver_Github = new QAction(xrLaunch);
        actionVertver_Github->setObjectName(QStringLiteral("actionVertver_Github"));
        actionGiperion = new QAction(xrLaunch);
        actionGiperion->setObjectName(QStringLiteral("actionGiperion"));
        actionMain_parameters = new QAction(xrLaunch);
        actionMain_parameters->setObjectName(QStringLiteral("actionMain_parameters"));
        actionAbout_Oxygen_Team = new QAction(xrLaunch);
        actionAbout_Oxygen_Team->setObjectName(QStringLiteral("actionAbout_Oxygen_Team"));
        actionOxygen_Team = new QAction(xrLaunch);
        actionOxygen_Team->setObjectName(QStringLiteral("actionOxygen_Team"));
        actionVertver = new QAction(xrLaunch);
        actionVertver->setObjectName(QStringLiteral("actionVertver"));
        actionGiperion_2 = new QAction(xrLaunch);
        actionGiperion_2->setObjectName(QStringLiteral("actionGiperion_2"));
        actionForserX = new QAction(xrLaunch);
        actionForserX->setObjectName(QStringLiteral("actionForserX"));
        centralWidget = new QWidget(xrLaunch);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        pushButton = new QPushButton(centralWidget);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setGeometry(QRect(380, 10, 81, 21));
        QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy1.setHorizontalStretch(5);
        sizePolicy1.setVerticalStretch(5);
        sizePolicy1.setHeightForWidth(pushButton->sizePolicy().hasHeightForWidth());
        pushButton->setSizePolicy(sizePolicy1);
        QPalette palette1;
        palette1.setBrush(QPalette::Active, QPalette::WindowText, brush);
        QBrush brush11(QColor(203, 219, 235, 255));
        brush11.setStyle(Qt::SolidPattern);
        palette1.setBrush(QPalette::Active, QPalette::Button, brush11);
        QBrush brush12(QColor(8, 29, 43, 255));
        brush12.setStyle(Qt::SolidPattern);
        palette1.setBrush(QPalette::Active, QPalette::Dark, brush12);
        palette1.setBrush(QPalette::Active, QPalette::Mid, brush12);
        palette1.setBrush(QPalette::Active, QPalette::Text, brush6);
        palette1.setBrush(QPalette::Active, QPalette::ButtonText, brush8);
        palette1.setBrush(QPalette::Active, QPalette::Base, brush12);
        palette1.setBrush(QPalette::Active, QPalette::Window, brush12);
        QBrush brush13(QColor(8, 29, 43, 102));
        brush13.setStyle(Qt::SolidPattern);
        palette1.setBrush(QPalette::Active, QPalette::Highlight, brush13);
        palette1.setBrush(QPalette::Active, QPalette::AlternateBase, brush12);
        palette1.setBrush(QPalette::Active, QPalette::NoRole, brush12);
        palette1.setBrush(QPalette::Active, QPalette::ToolTipBase, brush13);
        palette1.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        palette1.setBrush(QPalette::Inactive, QPalette::Button, brush11);
        palette1.setBrush(QPalette::Inactive, QPalette::Dark, brush12);
        palette1.setBrush(QPalette::Inactive, QPalette::Mid, brush12);
        palette1.setBrush(QPalette::Inactive, QPalette::Text, brush6);
        palette1.setBrush(QPalette::Inactive, QPalette::ButtonText, brush8);
        palette1.setBrush(QPalette::Inactive, QPalette::Base, brush12);
        palette1.setBrush(QPalette::Inactive, QPalette::Window, brush12);
        palette1.setBrush(QPalette::Inactive, QPalette::Highlight, brush13);
        palette1.setBrush(QPalette::Inactive, QPalette::AlternateBase, brush12);
        palette1.setBrush(QPalette::Inactive, QPalette::NoRole, brush12);
        palette1.setBrush(QPalette::Inactive, QPalette::ToolTipBase, brush13);
        palette1.setBrush(QPalette::Disabled, QPalette::WindowText, brush12);
        palette1.setBrush(QPalette::Disabled, QPalette::Button, brush11);
        palette1.setBrush(QPalette::Disabled, QPalette::Dark, brush12);
        palette1.setBrush(QPalette::Disabled, QPalette::Mid, brush12);
        palette1.setBrush(QPalette::Disabled, QPalette::Text, brush12);
        palette1.setBrush(QPalette::Disabled, QPalette::ButtonText, brush12);
        palette1.setBrush(QPalette::Disabled, QPalette::Base, brush12);
        palette1.setBrush(QPalette::Disabled, QPalette::Window, brush12);
        palette1.setBrush(QPalette::Disabled, QPalette::Highlight, brush13);
        palette1.setBrush(QPalette::Disabled, QPalette::AlternateBase, brush12);
        palette1.setBrush(QPalette::Disabled, QPalette::NoRole, brush12);
        palette1.setBrush(QPalette::Disabled, QPalette::ToolTipBase, brush13);
        pushButton->setPalette(palette1);
        pushButton->setAutoDefault(false);
        pushButton->setFlat(false);
        listWidget = new QListWidget(centralWidget);
        listWidget->setObjectName(QStringLiteral("listWidget"));
        listWidget->setGeometry(QRect(0, 0, 251, 3011));
        lineEdit = new QLineEdit(centralWidget);
        lineEdit->setObjectName(QStringLiteral("lineEdit"));
        lineEdit->setGeometry(QRect(260, 10, 113, 21));
        QPalette palette2;
        palette2.setBrush(QPalette::Active, QPalette::WindowText, brush);
        palette2.setBrush(QPalette::Active, QPalette::Dark, brush4);
        palette2.setBrush(QPalette::Active, QPalette::Text, brush6);
        palette2.setBrush(QPalette::Active, QPalette::ButtonText, brush8);
        palette2.setBrush(QPalette::Active, QPalette::Base, brush4);
        palette2.setBrush(QPalette::Active, QPalette::Window, brush4);
        palette2.setBrush(QPalette::Active, QPalette::AlternateBase, brush4);
        palette2.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        palette2.setBrush(QPalette::Inactive, QPalette::Dark, brush4);
        palette2.setBrush(QPalette::Inactive, QPalette::Text, brush6);
        palette2.setBrush(QPalette::Inactive, QPalette::ButtonText, brush8);
        palette2.setBrush(QPalette::Inactive, QPalette::Base, brush4);
        palette2.setBrush(QPalette::Inactive, QPalette::Window, brush4);
        palette2.setBrush(QPalette::Inactive, QPalette::AlternateBase, brush4);
        palette2.setBrush(QPalette::Disabled, QPalette::WindowText, brush4);
        palette2.setBrush(QPalette::Disabled, QPalette::Dark, brush4);
        palette2.setBrush(QPalette::Disabled, QPalette::Text, brush4);
        palette2.setBrush(QPalette::Disabled, QPalette::ButtonText, brush4);
        palette2.setBrush(QPalette::Disabled, QPalette::Base, brush4);
        palette2.setBrush(QPalette::Disabled, QPalette::Window, brush4);
        palette2.setBrush(QPalette::Disabled, QPalette::AlternateBase, brush4);
        lineEdit->setPalette(palette2);
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
        menuAbout = new QMenu(menuBar);
        menuAbout->setObjectName(QStringLiteral("menuAbout"));
        menuGithub_Pages = new QMenu(menuAbout);
        menuGithub_Pages->setObjectName(QStringLiteral("menuGithub_Pages"));
        xrLaunch->setMenuBar(menuBar);
        mainToolBar = new QToolBar(xrLaunch);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        xrLaunch->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(xrLaunch);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        xrLaunch->setStatusBar(statusBar);

        menuBar->addAction(menu->menuAction());
        menuBar->addAction(menuOptions->menuAction());
        menuBar->addAction(menuAbout->menuAction());
        menu->addAction(menuGame->menuAction());
        menu->addAction(menuEditors->menuAction());
        menu->addSeparator();
        menu->addAction(actionOptions);
        menu->addSeparator();
        menu->addAction(actionExit);
        menuGame->addAction(actionxrEngine);
        menuGame->addAction(menuxrEngine_with_options->menuAction());
        menuGame->addAction(actionxrPlay_2);
        menuxrEngine_with_options->addAction(action_renderdebug);
        menuxrEngine_with_options->addAction(actionOther);
        menuEditors->addAction(actionxrEditorTools_in_dev);
        menuEditors->addAction(actionxrAI);
        menuOptions->addAction(actionMain);
        menuOptions->addAction(actionParametres);
        menuAbout->addAction(actionMain_parameters);
        menuAbout->addSeparator();
        menuAbout->addAction(actionVertver_Github);
        menuAbout->addAction(actionAbout_Oxygen_Team);
        menuAbout->addAction(menuGithub_Pages->menuAction());
        menuAbout->addSeparator();
        menuGithub_Pages->addAction(actionOxygen_Team);
        menuGithub_Pages->addAction(actionVertver);
        menuGithub_Pages->addAction(actionGiperion_2);
        menuGithub_Pages->addAction(actionForserX);

        retranslateUi(xrLaunch);

        pushButton->setDefault(false);


        QMetaObject::connectSlotsByName(xrLaunch);
    } // setupUi

    void retranslateUi(QMainWindow *xrLaunch)
    {
        xrLaunch->setWindowTitle(QApplication::translate("xrLaunch", "Oxygen Development Launcher", nullptr));
        actionxrEngine->setText(QApplication::translate("xrLaunch", "xrEngine", nullptr));
#ifndef QT_NO_SHORTCUT
        actionxrEngine->setShortcut(QApplication::translate("xrLaunch", "Alt+Shift+R", nullptr));
#endif // QT_NO_SHORTCUT
        actionxrPlay_2->setText(QApplication::translate("xrLaunch", "xrPlay (Old oxygen)", nullptr));
        action_silent->setText(QApplication::translate("xrLaunch", "-silent", nullptr));
        action_renderdebug->setText(QApplication::translate("xrLaunch", "-renderdebug", nullptr));
        actionOther->setText(QApplication::translate("xrLaunch", "Other...", nullptr));
        actionxrEditorTools_in_dev->setText(QApplication::translate("xrLaunch", "xrEditorTools (in dev)", nullptr));
        actionxrAI->setText(QApplication::translate("xrLaunch", "xrAI", nullptr));
        actionOptions->setText(QApplication::translate("xrLaunch", "Parametres", nullptr));
        actionExit->setText(QApplication::translate("xrLaunch", "Exit", nullptr));
#ifndef QT_NO_SHORTCUT
        actionExit->setShortcut(QApplication::translate("xrLaunch", "Ctrl+Q", nullptr));
#endif // QT_NO_SHORTCUT
        actionMain->setText(QApplication::translate("xrLaunch", "Main", nullptr));
        actionParametres->setText(QApplication::translate("xrLaunch", "xrEngine parameters...", nullptr));
        actionVertver_Github->setText(QApplication::translate("xrLaunch", "About xrDevLaunch...", nullptr));
        actionGiperion->setText(QApplication::translate("xrLaunch", "Giperion", nullptr));
        actionMain_parameters->setText(QApplication::translate("xrLaunch", "Main parameters", nullptr));
        actionAbout_Oxygen_Team->setText(QApplication::translate("xrLaunch", "About Oxygen Team...", nullptr));
        actionOxygen_Team->setText(QApplication::translate("xrLaunch", "Oxygen Team", nullptr));
        actionVertver->setText(QApplication::translate("xrLaunch", "Vertver", nullptr));
        actionGiperion_2->setText(QApplication::translate("xrLaunch", "Giperion", nullptr));
        actionForserX->setText(QApplication::translate("xrLaunch", "ForserX", nullptr));
        pushButton->setText(QApplication::translate("xrLaunch", "Run", nullptr));
        menu->setTitle(QApplication::translate("xrLaunch", "Launch", nullptr));
        menuGame->setTitle(QApplication::translate("xrLaunch", "Game...", nullptr));
        menuxrEngine_with_options->setTitle(QApplication::translate("xrLaunch", "xrEngine (with options)", nullptr));
        menuEditors->setTitle(QApplication::translate("xrLaunch", "Editors", nullptr));
        menuOptions->setTitle(QApplication::translate("xrLaunch", "Options", nullptr));
        menuAbout->setTitle(QApplication::translate("xrLaunch", "Help", nullptr));
        menuGithub_Pages->setTitle(QApplication::translate("xrLaunch", "Github Pages....", nullptr));
    } // retranslateUi

};

namespace Ui {
    class xrLaunch: public Ui_xrLaunch {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_XRMAIN_H
