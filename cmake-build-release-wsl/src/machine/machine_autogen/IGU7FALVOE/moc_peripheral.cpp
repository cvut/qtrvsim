/****************************************************************************
** Meta object code from reading C++ file 'peripheral.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.13)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../../src/machine/memory/backend/peripheral.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'peripheral.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.13. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_machine__SimplePeripheral_t {
    QByteArrayData data[8];
    char stringdata0[91];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_machine__SimplePeripheral_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_machine__SimplePeripheral_t qt_meta_stringdata_machine__SimplePeripheral = {
    {
QT_MOC_LITERAL(0, 0, 25), // "machine::SimplePeripheral"
QT_MOC_LITERAL(1, 26, 18), // "write_notification"
QT_MOC_LITERAL(2, 45, 0), // ""
QT_MOC_LITERAL(3, 46, 6), // "Offset"
QT_MOC_LITERAL(4, 53, 7), // "address"
QT_MOC_LITERAL(5, 61, 6), // "size_t"
QT_MOC_LITERAL(6, 68, 4), // "size"
QT_MOC_LITERAL(7, 73, 17) // "read_notification"

    },
    "machine::SimplePeripheral\0write_notification\0"
    "\0Offset\0address\0size_t\0size\0"
    "read_notification"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_machine__SimplePeripheral[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   24,    2, 0x06 /* Public */,
       7,    2,   29,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 5,    4,    6,
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 5,    4,    6,

       0        // eod
};

void machine::SimplePeripheral::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<SimplePeripheral *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->write_notification((*reinterpret_cast< Offset(*)>(_a[1])),(*reinterpret_cast< size_t(*)>(_a[2]))); break;
        case 1: _t->read_notification((*reinterpret_cast< Offset(*)>(_a[1])),(*reinterpret_cast< size_t(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (SimplePeripheral::*)(Offset , size_t ) const;
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SimplePeripheral::write_notification)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (SimplePeripheral::*)(Offset , size_t ) const;
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SimplePeripheral::read_notification)) {
                *result = 1;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject machine::SimplePeripheral::staticMetaObject = { {
    QMetaObject::SuperData::link<BackendMemory::staticMetaObject>(),
    qt_meta_stringdata_machine__SimplePeripheral.data,
    qt_meta_data_machine__SimplePeripheral,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *machine::SimplePeripheral::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *machine::SimplePeripheral::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_machine__SimplePeripheral.stringdata0))
        return static_cast<void*>(this);
    return BackendMemory::qt_metacast(_clname);
}

int machine::SimplePeripheral::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = BackendMemory::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void machine::SimplePeripheral::write_notification(Offset _t1, size_t _t2)const
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(const_cast< machine::SimplePeripheral *>(this), &staticMetaObject, 0, _a);
}

// SIGNAL 1
void machine::SimplePeripheral::read_notification(Offset _t1, size_t _t2)const
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(const_cast< machine::SimplePeripheral *>(this), &staticMetaObject, 1, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
