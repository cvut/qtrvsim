/****************************************************************************
** Meta object code from reading C++ file 'memory_bus.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.13)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../../src/machine/memory/memory_bus.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'memory_bus.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.13. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_machine__MemoryDataBus_t {
    QByteArrayData data[10];
    char stringdata0[133];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_machine__MemoryDataBus_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_machine__MemoryDataBus_t qt_meta_stringdata_machine__MemoryDataBus = {
    {
QT_MOC_LITERAL(0, 0, 22), // "machine::MemoryDataBus"
QT_MOC_LITERAL(1, 23, 29), // "range_backend_external_change"
QT_MOC_LITERAL(2, 53, 0), // ""
QT_MOC_LITERAL(3, 54, 20), // "const BackendMemory*"
QT_MOC_LITERAL(4, 75, 6), // "device"
QT_MOC_LITERAL(5, 82, 6), // "Offset"
QT_MOC_LITERAL(6, 89, 12), // "start_offset"
QT_MOC_LITERAL(7, 102, 11), // "last_offset"
QT_MOC_LITERAL(8, 114, 13), // "AccessEffects"
QT_MOC_LITERAL(9, 128, 4) // "type"

    },
    "machine::MemoryDataBus\0"
    "range_backend_external_change\0\0"
    "const BackendMemory*\0device\0Offset\0"
    "start_offset\0last_offset\0AccessEffects\0"
    "type"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_machine__MemoryDataBus[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    4,   19,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 5, 0x80000000 | 5, 0x80000000 | 8,    4,    6,    7,    9,

       0        // eod
};

void machine::MemoryDataBus::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MemoryDataBus *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->range_backend_external_change((*reinterpret_cast< const BackendMemory*(*)>(_a[1])),(*reinterpret_cast< Offset(*)>(_a[2])),(*reinterpret_cast< Offset(*)>(_a[3])),(*reinterpret_cast< AccessEffects(*)>(_a[4]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject machine::MemoryDataBus::staticMetaObject = { {
    QMetaObject::SuperData::link<FrontendMemory::staticMetaObject>(),
    qt_meta_stringdata_machine__MemoryDataBus.data,
    qt_meta_data_machine__MemoryDataBus,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *machine::MemoryDataBus::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *machine::MemoryDataBus::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_machine__MemoryDataBus.stringdata0))
        return static_cast<void*>(this);
    return FrontendMemory::qt_metacast(_clname);
}

int machine::MemoryDataBus::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = FrontendMemory::qt_metacall(_c, _id, _a);
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
QT_WARNING_POP
QT_END_MOC_NAMESPACE
