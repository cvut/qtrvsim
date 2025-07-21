/****************************************************************************
** Meta object code from reading C++ file 'registers.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.13)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../../src/machine/registers.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'registers.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.13. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_machine__Registers_t {
    QByteArrayData data[10];
    char stringdata0[89];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_machine__Registers_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_machine__Registers_t qt_meta_stringdata_machine__Registers = {
    {
QT_MOC_LITERAL(0, 0, 18), // "machine::Registers"
QT_MOC_LITERAL(1, 19, 9), // "pc_update"
QT_MOC_LITERAL(2, 29, 0), // ""
QT_MOC_LITERAL(3, 30, 7), // "Address"
QT_MOC_LITERAL(4, 38, 3), // "val"
QT_MOC_LITERAL(5, 42, 9), // "gp_update"
QT_MOC_LITERAL(6, 52, 10), // "RegisterId"
QT_MOC_LITERAL(7, 63, 3), // "reg"
QT_MOC_LITERAL(8, 67, 13), // "RegisterValue"
QT_MOC_LITERAL(9, 81, 7) // "gp_read"

    },
    "machine::Registers\0pc_update\0\0Address\0"
    "val\0gp_update\0RegisterId\0reg\0RegisterValue\0"
    "gp_read"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_machine__Registers[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   29,    2, 0x06 /* Public */,
       5,    2,   32,    2, 0x06 /* Public */,
       9,    2,   37,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 6, 0x80000000 | 8,    7,    4,
    QMetaType::Void, 0x80000000 | 6, 0x80000000 | 8,    7,    4,

       0        // eod
};

void machine::Registers::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Registers *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->pc_update((*reinterpret_cast< Address(*)>(_a[1]))); break;
        case 1: _t->gp_update((*reinterpret_cast< RegisterId(*)>(_a[1])),(*reinterpret_cast< RegisterValue(*)>(_a[2]))); break;
        case 2: _t->gp_read((*reinterpret_cast< RegisterId(*)>(_a[1])),(*reinterpret_cast< RegisterValue(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (Registers::*)(Address );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Registers::pc_update)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (Registers::*)(RegisterId , RegisterValue );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Registers::gp_update)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (Registers::*)(RegisterId , RegisterValue ) const;
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Registers::gp_read)) {
                *result = 2;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject machine::Registers::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_machine__Registers.data,
    qt_meta_data_machine__Registers,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *machine::Registers::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *machine::Registers::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_machine__Registers.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int machine::Registers::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void machine::Registers::pc_update(Address _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void machine::Registers::gp_update(RegisterId _t1, RegisterValue _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void machine::Registers::gp_read(RegisterId _t1, RegisterValue _t2)const
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(const_cast< machine::Registers *>(this), &staticMetaObject, 2, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
