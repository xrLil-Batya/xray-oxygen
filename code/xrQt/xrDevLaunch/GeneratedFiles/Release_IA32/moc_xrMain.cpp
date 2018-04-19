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
    QByteArrayData data[20];
    char stringdata0[433];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_xrLaunch_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_xrLaunch_t qt_meta_stringdata_xrLaunch = {
    {
QT_MOC_LITERAL(0, 0, 8), // "xrLaunch"
QT_MOC_LITERAL(1, 9, 21), // "on_pushButton_clicked"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 15), // "run_xrEngineRun"
QT_MOC_LITERAL(4, 48, 23), // "on_actionExit_triggered"
QT_MOC_LITERAL(5, 72, 27), // "on_actionxrEngine_triggered"
QT_MOC_LITERAL(6, 100, 25), // "on_actionxrCore_triggered"
QT_MOC_LITERAL(7, 126, 30), // "on_actionOxygen_Team_triggered"
QT_MOC_LITERAL(8, 157, 26), // "on_actionVertver_triggered"
QT_MOC_LITERAL(9, 184, 29), // "on_actionGiperion_2_triggered"
QT_MOC_LITERAL(10, 214, 26), // "on_actionForserX_triggered"
QT_MOC_LITERAL(11, 241, 29), // "on_actionParametres_triggered"
QT_MOC_LITERAL(12, 271, 33), // "on_actionVertver_Github_trigg..."
QT_MOC_LITERAL(13, 305, 36), // "on_actionAbout_Oxygen_Team_tr..."
QT_MOC_LITERAL(14, 342, 25), // "on_listWidget_itemPressed"
QT_MOC_LITERAL(15, 368, 16), // "QListWidgetItem*"
QT_MOC_LITERAL(16, 385, 4), // "item"
QT_MOC_LITERAL(17, 390, 13), // "status_render"
QT_MOC_LITERAL(18, 404, 16), // "add_stringToList"
QT_MOC_LITERAL(19, 421, 11) // "init_xrCore"

    },
    "xrLaunch\0on_pushButton_clicked\0\0"
    "run_xrEngineRun\0on_actionExit_triggered\0"
    "on_actionxrEngine_triggered\0"
    "on_actionxrCore_triggered\0"
    "on_actionOxygen_Team_triggered\0"
    "on_actionVertver_triggered\0"
    "on_actionGiperion_2_triggered\0"
    "on_actionForserX_triggered\0"
    "on_actionParametres_triggered\0"
    "on_actionVertver_Github_triggered\0"
    "on_actionAbout_Oxygen_Team_triggered\0"
    "on_listWidget_itemPressed\0QListWidgetItem*\0"
    "item\0status_render\0add_stringToList\0"
    "init_xrCore"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_xrLaunch[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      16,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   94,    2, 0x08 /* Private */,
       3,    0,   95,    2, 0x08 /* Private */,
       4,    0,   96,    2, 0x08 /* Private */,
       5,    0,   97,    2, 0x08 /* Private */,
       6,    0,   98,    2, 0x08 /* Private */,
       7,    0,   99,    2, 0x08 /* Private */,
       8,    0,  100,    2, 0x08 /* Private */,
       9,    0,  101,    2, 0x08 /* Private */,
      10,    0,  102,    2, 0x08 /* Private */,
      11,    0,  103,    2, 0x08 /* Private */,
      12,    0,  104,    2, 0x08 /* Private */,
      13,    0,  105,    2, 0x08 /* Private */,
      14,    1,  106,    2, 0x08 /* Private */,
      17,    0,  109,    2, 0x08 /* Private */,
      18,    0,  110,    2, 0x08 /* Private */,
      19,    0,  111,    2, 0x08 /* Private */,

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
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 15,   16,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void xrLaunch::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        xrLaunch *_t = static_cast<xrLaunch *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->on_pushButton_clicked(); break;
        case 1: _t->run_xrEngineRun(); break;
        case 2: _t->on_actionExit_triggered(); break;
        case 3: _t->on_actionxrEngine_triggered(); break;
        case 4: _t->on_actionxrCore_triggered(); break;
        case 5: _t->on_actionOxygen_Team_triggered(); break;
        case 6: _t->on_actionVertver_triggered(); break;
        case 7: _t->on_actionGiperion_2_triggered(); break;
        case 8: _t->on_actionForserX_triggered(); break;
        case 9: _t->on_actionParametres_triggered(); break;
        case 10: _t->on_actionVertver_Github_triggered(); break;
        case 11: _t->on_actionAbout_Oxygen_Team_triggered(); break;
        case 12: _t->on_listWidget_itemPressed((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 13: _t->status_render(); break;
        case 14: _t->add_stringToList(); break;
        case 15: _t->init_xrCore(); break;
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
        if (_id < 16)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 16;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 16)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 16;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
