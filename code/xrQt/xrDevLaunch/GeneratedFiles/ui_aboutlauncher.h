/********************************************************************************
** Form generated from reading UI file 'aboutlauncher.ui'
**
** Created by: Qt User Interface Compiler version 5.10.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ABOUTLAUNCHER_H
#define UI_ABOUTLAUNCHER_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>

QT_BEGIN_NAMESPACE

class Ui_AboutLauncher
{
public:
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QLabel *label_4;
    QLabel *label_5;
    QLabel *label_7;
    QLabel *label_6;

    void setupUi(QDialog *AboutLauncher)
    {
        if (AboutLauncher->objectName().isEmpty())
            AboutLauncher->setObjectName(QStringLiteral("AboutLauncher"));
        AboutLauncher->resize(440, 340);
        AboutLauncher->setMinimumSize(QSize(440, 340));
        AboutLauncher->setMaximumSize(QSize(440, 340));
        AboutLauncher->setBaseSize(QSize(440, 340));
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
        palette.setBrush(QPalette::Active, QPalette::HighlightedText, brush);
        QBrush brush10(QColor(165, 122, 255, 255));
        brush10.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Link, brush10);
        palette.setBrush(QPalette::Active, QPalette::AlternateBase, brush4);
        QBrush brush11(QColor(25, 25, 25, 255));
        brush11.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::NoRole, brush11);
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
        palette.setBrush(QPalette::Inactive, QPalette::HighlightedText, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Link, brush10);
        palette.setBrush(QPalette::Inactive, QPalette::AlternateBase, brush4);
        palette.setBrush(QPalette::Inactive, QPalette::NoRole, brush11);
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
        palette.setBrush(QPalette::Disabled, QPalette::HighlightedText, brush);
        palette.setBrush(QPalette::Disabled, QPalette::Link, brush10);
        palette.setBrush(QPalette::Disabled, QPalette::AlternateBase, brush4);
        palette.setBrush(QPalette::Disabled, QPalette::NoRole, brush11);
        palette.setBrush(QPalette::Disabled, QPalette::ToolTipText, brush7);
        AboutLauncher->setPalette(palette);
        label = new QLabel(AboutLauncher);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(10, 313, 111, 15));
        label_2 = new QLabel(AboutLauncher);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(310, 310, 121, 20));
        label_3 = new QLabel(AboutLauncher);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(340, 10, 91, 20));
        label_4 = new QLabel(AboutLauncher);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(20, 50, 191, 16));
        label_5 = new QLabel(AboutLauncher);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setGeometry(QRect(20, 70, 181, 16));
        label_7 = new QLabel(AboutLauncher);
        label_7->setObjectName(QStringLiteral("label_7"));
        label_7->setGeometry(QRect(20, 130, 291, 31));
        label_6 = new QLabel(AboutLauncher);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setGeometry(QRect(20, 90, 171, 16));

        retranslateUi(AboutLauncher);

        QMetaObject::connectSlotsByName(AboutLauncher);
    } // setupUi

    void retranslateUi(QDialog *AboutLauncher)
    {
        AboutLauncher->setWindowTitle(QApplication::translate("AboutLauncher", "Dialog", nullptr));
        label->setText(QApplication::translate("AboutLauncher", "About xrDevLaunch...", nullptr));
        label_2->setText(QApplication::translate("AboutLauncher", "Oxygen Team (C), 2018", nullptr));
        label_3->setText(QApplication::translate("AboutLauncher", "xrDevLaunch 0.4v", nullptr));
        label_4->setText(QApplication::translate("AboutLauncher", "Developer Launcher by Oxygen Team.", nullptr));
        label_5->setText(QApplication::translate("AboutLauncher", "Creators: Vertver, ForserX, Giperion", nullptr));
        label_7->setText(QApplication::translate("AboutLauncher", "Builded by Microsoft Visual Studio \n"
"C++ Compiler 2017, Qt 5.10.1", nullptr));
        label_6->setText(QApplication::translate("AboutLauncher", "Platform: Windows 7+", nullptr));
    } // retranslateUi

};

namespace Ui {
    class AboutLauncher: public Ui_AboutLauncher {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ABOUTLAUNCHER_H
