/****************************************************************************
** Meta object code from reading C++ file 'registersdock.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.13)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../../src/gui/windows/registers/registersdock.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'registersdock.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.13. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_RegistersDock_t {
    QByteArrayData data[11];
    char stringdata0[128];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_RegistersDock_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_RegistersDock_t qt_meta_stringdata_RegistersDock = {
    {
QT_MOC_LITERAL(0, 0, 13), // "RegistersDock"
QT_MOC_LITERAL(1, 14, 10), // "pc_changed"
QT_MOC_LITERAL(2, 25, 0), // ""
QT_MOC_LITERAL(3, 26, 16), // "machine::Address"
QT_MOC_LITERAL(4, 43, 3), // "val"
QT_MOC_LITERAL(5, 47, 10), // "gp_changed"
QT_MOC_LITERAL(6, 58, 19), // "machine::RegisterId"
QT_MOC_LITERAL(7, 78, 1), // "i"
QT_MOC_LITERAL(8, 80, 22), // "machine::RegisterValue"
QT_MOC_LITERAL(9, 103, 7), // "gp_read"
QT_MOC_LITERAL(10, 111, 16) // "clear_highlights"

    },
    "RegistersDock\0pc_changed\0\0machine::Address\0"
    "val\0gp_changed\0machine::RegisterId\0i\0"
    "machine::RegisterValue\0gp_read\0"
    "clear_highlights"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_RegistersDock[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   34,    2, 0x08 /* Private */,
       5,    2,   37,    2, 0x08 /* Private */,
       9,    2,   42,    2, 0x08 /* Private */,
      10,    0,   47,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 6, 0x80000000 | 8,    7,    4,
    QMetaType::Void, 0x80000000 | 6, 0x80000000 | 8,    7,    4,
    QMetaType::Void,

       0        // eod
};

void RegistersDock::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<RegistersDock *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->pc_changed((*reinterpret_cast< machine::Address(*)>(_a[1]))); break;
        case 1: _t->gp_changed((*reinterpret_cast< machine::RegisterId(*)>(_a[1])),(*reinterpret_cast< machine::RegisterValue(*)>(_a[2]))); break;
        case 2: _t->gp_read((*reinterpret_cast< machine::RegisterId(*)>(_a[1])),(*reinterpret_cast< machine::RegisterValue(*)>(_a[2]))); break;
        case 3: _t->clear_highlights(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< machine::Address >(); break;
            }
            break;
        case 1:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< machine::RegisterValue >(); break;
            }
            break;
        case 2:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< machine::RegisterValue >(); break;
            }
            break;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject RegistersDock::staticMetaObject = { {
    QMetaObject::SuperData::link<QDockWidget::staticMetaObject>(),
    qt_meta_stringdata_RegistersDock.data,
    qt_meta_data_RegistersDock,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *RegistersDock::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *RegistersDock::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_RegistersDock.stringdata0))
        return static_cast<void*>(this);
    return QDockWidget::qt_metacast(_clname);
}

int RegistersDock::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDockWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
