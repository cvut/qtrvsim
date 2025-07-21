/****************************************************************************
** Meta object code from reading C++ file 'controlstate.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.13)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../../src/machine/csr/controlstate.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'controlstate.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.13. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_machine__CSR__ControlState_t {
    QByteArrayData data[16];
    char stringdata0[204];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_machine__CSR__ControlState_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_machine__CSR__ControlState_t qt_meta_stringdata_machine__CSR__ControlState = {
    {
QT_MOC_LITERAL(0, 0, 26), // "machine::CSR::ControlState"
QT_MOC_LITERAL(1, 27, 12), // "write_signal"
QT_MOC_LITERAL(2, 40, 0), // ""
QT_MOC_LITERAL(3, 41, 6), // "size_t"
QT_MOC_LITERAL(4, 48, 15), // "internal_reg_id"
QT_MOC_LITERAL(5, 64, 13), // "RegisterValue"
QT_MOC_LITERAL(6, 78, 3), // "val"
QT_MOC_LITERAL(7, 82, 11), // "read_signal"
QT_MOC_LITERAL(8, 94, 20), // "set_interrupt_signal"
QT_MOC_LITERAL(9, 115, 7), // "irq_num"
QT_MOC_LITERAL(10, 123, 6), // "active"
QT_MOC_LITERAL(11, 130, 18), // "exception_initiate"
QT_MOC_LITERAL(12, 149, 14), // "PrivilegeLevel"
QT_MOC_LITERAL(13, 164, 11), // "act_privlev"
QT_MOC_LITERAL(14, 176, 10), // "to_privlev"
QT_MOC_LITERAL(15, 187, 16) // "exception_return"

    },
    "machine::CSR::ControlState\0write_signal\0"
    "\0size_t\0internal_reg_id\0RegisterValue\0"
    "val\0read_signal\0set_interrupt_signal\0"
    "irq_num\0active\0exception_initiate\0"
    "PrivilegeLevel\0act_privlev\0to_privlev\0"
    "exception_return"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_machine__CSR__ControlState[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   39,    2, 0x06 /* Public */,
       7,    2,   44,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       8,    2,   49,    2, 0x0a /* Public */,
      11,    2,   54,    2, 0x0a /* Public */,
      15,    1,   59,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 5,    4,    6,
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 5,    4,    6,

 // slots: parameters
    QMetaType::Void, QMetaType::UInt, QMetaType::Bool,    9,   10,
    QMetaType::Void, 0x80000000 | 12, 0x80000000 | 12,   13,   14,
    0x80000000 | 12, 0x80000000 | 12,   13,

       0        // eod
};

void machine::CSR::ControlState::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ControlState *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->write_signal((*reinterpret_cast< size_t(*)>(_a[1])),(*reinterpret_cast< RegisterValue(*)>(_a[2]))); break;
        case 1: _t->read_signal((*reinterpret_cast< size_t(*)>(_a[1])),(*reinterpret_cast< RegisterValue(*)>(_a[2]))); break;
        case 2: _t->set_interrupt_signal((*reinterpret_cast< uint(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 3: _t->exception_initiate((*reinterpret_cast< PrivilegeLevel(*)>(_a[1])),(*reinterpret_cast< PrivilegeLevel(*)>(_a[2]))); break;
        case 4: { PrivilegeLevel _r = _t->exception_return((*reinterpret_cast< PrivilegeLevel(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< PrivilegeLevel*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ControlState::*)(size_t , RegisterValue );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ControlState::write_signal)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ControlState::*)(size_t , RegisterValue ) const;
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ControlState::read_signal)) {
                *result = 1;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject machine::CSR::ControlState::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_machine__CSR__ControlState.data,
    qt_meta_data_machine__CSR__ControlState,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *machine::CSR::ControlState::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *machine::CSR::ControlState::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_machine__CSR__ControlState.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int machine::CSR::ControlState::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void machine::CSR::ControlState::write_signal(size_t _t1, RegisterValue _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void machine::CSR::ControlState::read_signal(size_t _t1, RegisterValue _t2)const
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(const_cast< machine::CSR::ControlState *>(this), &staticMetaObject, 1, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
