/********************************************************************************
** Form generated from reading UI file 'parametersdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.10.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PARAMETERSDIALOG_H
#define UI_PARAMETERSDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QTextEdit>

QT_BEGIN_NAMESPACE

class Ui_Dialog
{
public:
    QDialogButtonBox *buttonBox;
    QTextEdit *textEdit;
    QLabel *label;

    void setupUi(QDialog *Dialog)
    {
        if (Dialog->objectName().isEmpty())
            Dialog->setObjectName(QStringLiteral("Dialog"));
        Dialog->resize(380, 90);
        Dialog->setMinimumSize(QSize(380, 90));
        Dialog->setMaximumSize(QSize(380, 90));
        Dialog->setBaseSize(QSize(380, 90));
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
        QBrush brush11(QColor(90, 57, 255, 255));
        brush11.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::LinkVisited, brush11);
        QBrush brush12(QColor(25, 25, 25, 255));
        brush12.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::AlternateBase, brush12);
        QBrush brush13(QColor(32, 181, 230, 102));
        brush13.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::ToolTipBase, brush13);
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
        palette.setBrush(QPalette::Inactive, QPalette::LinkVisited, brush11);
        palette.setBrush(QPalette::Inactive, QPalette::AlternateBase, brush12);
        palette.setBrush(QPalette::Inactive, QPalette::ToolTipBase, brush13);
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
        palette.setBrush(QPalette::Disabled, QPalette::LinkVisited, brush11);
        palette.setBrush(QPalette::Disabled, QPalette::AlternateBase, brush12);
        palette.setBrush(QPalette::Disabled, QPalette::ToolTipBase, brush13);
        Dialog->setPalette(palette);
        buttonBox = new QDialogButtonBox(Dialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setGeometry(QRect(290, 20, 81, 241));
        buttonBox->setOrientation(Qt::Vertical);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        textEdit = new QTextEdit(Dialog);
        textEdit->setObjectName(QStringLiteral("textEdit"));
        textEdit->setGeometry(QRect(10, 29, 271, 51));
        label = new QLabel(Dialog);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(10, 10, 201, 16));

        retranslateUi(Dialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), Dialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), Dialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(Dialog);
    } // setupUi

    void retranslateUi(QDialog *Dialog)
    {
        Dialog->setWindowTitle(QApplication::translate("Dialog", "xrEngine parameters", nullptr));
        label->setText(QApplication::translate("Dialog", "Parameters for xrEngine...", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Dialog: public Ui_Dialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PARAMETERSDIALOG_H
