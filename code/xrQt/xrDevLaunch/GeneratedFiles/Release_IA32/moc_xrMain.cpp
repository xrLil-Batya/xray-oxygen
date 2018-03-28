/****************************************************************************
** Meta object code from reading C++ file 'xrMain.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.10.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../xrMain.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'xrMain.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.10.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_xrDialogParam_t {
    QByteArrayData data[3];
    char stringdata0[37];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_xrDialogParam_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_xrDialogParam_t qt_meta_stringdata_xrDialogParam = {
    {
QT_MOC_LITERAL(0, 0, 13), // "xrDialogParam"
QT_MOC_LITERAL(1, 14, 21), // "on_buttonBox_accepted"
QT_MOC_LITERAL(2, 36, 0) // ""

    },
    "xrDialogParam\0on_buttonBox_accepted\0"
    ""
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_xrDialogParam[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   19,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

void xrDialogParam::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        xrDialogParam *_t = static_cast<xrDialogParam *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->on_buttonBox_accepted(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject xrDialogParam::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_xrDialogParam.data,
      qt_meta_data_xrDialogParam,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *xrDialogParam::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *xrDialogParam::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_xrDialogParam.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int xrDialogParam::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 1;
    }
    return _id;
}
struct qt_meta_stringdata_xrLaunch_t {
    QByteArrayData data[16];
    char stringdata0[344];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_xrLaunch_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_xrLaunch_t qt_meta_stringdata_xrLaunch = {
    {
QT_MOC_LITERAL(0, 0, 8), // "xrLaunch"
QT_MOC_LITERAL(1, 9, 16), // "add_stringToList"
QT_MOC_LITERAL(2, 26, 0), // ""
QT_MOC_LITERAL(3, 27, 21), // "on_pushButton_clicked"
QT_MOC_LITERAL(4, 49, 15), // "run_xrEngineRun"
QT_MOC_LITERAL(5, 65, 23), // "on_actionExit_triggered"
QT_MOC_LITERAL(6, 89, 27), // "on_actionxrEngine_triggered"
QT_MOC_LITERAL(7, 117, 30), // "on_actionOxygen_Team_triggered"
QT_MOC_LITERAL(8, 148, 26), // "on_actionVertver_triggered"
QT_MOC_LITERAL(9, 175, 29), // "on_actionGiperion_2_triggered"
QT_MOC_LITERAL(10, 205, 26), // "on_actionForserX_triggered"
QT_MOC_LITERAL(11, 232, 29), // "on_actionParametres_triggered"
QT_MOC_LITERAL(12, 262, 25), // "on_listWidget_itemPressed"
QT_MOC_LITERAL(13, 288, 16), // "QListWidgetItem*"
QT_MOC_LITERAL(14, 305, 4), // "item"
QT_MOC_LITERAL(15, 310, 33) // "on_actionVertver_Github_trigg..."

    },
    "xrLaunch\0add_stringToList\0\0"
    "on_pushButton_clicked\0run_xrEngineRun\0"
    "on_actionExit_triggered\0"
    "on_actionxrEngine_triggered\0"
    "on_actionOxygen_Team_triggered\0"
    "on_actionVertver_triggered\0"
    "on_actionGiperion_2_triggered\0"
    "on_actionForserX_triggered\0"
    "on_actionParametres_triggered\0"
    "on_listWidget_itemPressed\0QListWidgetItem*\0"
    "item\0on_actionVertver_Github_triggered"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_xrLaunch[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   74,    2, 0x08 /* Private */,
       3,    0,   75,    2, 0x08 /* Private */,
       4,    0,   76,    2, 0x08 /* Private */,
       5,    0,   77,    2, 0x08 /* Private */,
       6,    0,   78,    2, 0x08 /* Private */,
       7,    0,   79,    2, 0x08 /* Private */,
       8,    0,   80,    2, 0x08 /* Private */,
       9,    0,   81,    2, 0x08 /* Private */,
      10,    0,   82,    2, 0x08 /* Private */,
      11,    0,   83,    2, 0x08 /* Private */,
      12,    1,   84,    2, 0x08 /* Private */,
      15,    0,   87,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 13,   14,
    QMetaType::Void,

       0        // eod
};

void xrLaunch::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        xrLaunch *_t = static_cast<xrLaunch *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->add_stringToList(); break;
        case 1: _t->on_pushButton_clicked(); break;
        case 2: _t->run_xrEngineRun(); break;
        case 3: _t->on_actionExit_triggered(); break;
        case 4: _t->on_actionxrEngine_triggered(); break;
        case 5: _t->on_actionOxygen_Team_triggered(); break;
        case 6: _t->on_actionVertver_triggered(); break;
        case 7: _t->on_actionGiperion_2_triggered(); break;
        case 8: _t->on_actionForserX_triggered(); break;
        case 9: _t->on_actionParametres_triggered(); break;
        case 10: _t->on_listWidget_itemPressed((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 11: _t->on_actionVertver_Github_triggered(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject xrLaunch::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_xrLaunch.data,
      qt_meta_data_xrLaunch,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *xrLaunch::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *xrLaunch::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_xrLaunch.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int xrLaunch::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 12)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 12;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
