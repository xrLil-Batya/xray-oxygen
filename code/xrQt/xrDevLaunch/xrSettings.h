#ifndef XRSETTINGS_H
#define XRSETTINGS_H

#include <QDialog>

namespace Ui {
class xrSettings;
}

class xrSettings : public QDialog
{
    Q_OBJECT

public:
    explicit xrSettings(QWidget *parent = 0);
    ~xrSettings();

private:
    Ui::xrSettings *ui;
};

#endif // XRSETTINGS_H
