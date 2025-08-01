/****************************************************************************
** Meta object code from reading C++ file 'backend_memory.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.13)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../../src/machine/memory/backend/backend_memory.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'backend_memory.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.13. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_machine__BackendMemory_t {
    QByteArrayData data[10];
    char stringdata0[136];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_machine__BackendMemory_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_machine__BackendMemory_t qt_meta_stringdata_machine__BackendMemory = {
    {
QT_MOC_LITERAL(0, 0, 22), // "machine::BackendMemory"
QT_MOC_LITERAL(1, 23, 30), // "external_backend_change_notify"
QT_MOC_LITERAL(2, 54, 0), // ""
QT_MOC_LITERAL(3, 55, 20), // "const BackendMemory*"
QT_MOC_LITERAL(4, 76, 10), // "mem_access"
QT_MOC_LITERAL(5, 87, 8), // "uint32_t"
QT_MOC_LITERAL(6, 96, 10), // "start_addr"
QT_MOC_LITERAL(7, 107, 9), // "last_addr"
QT_MOC_LITERAL(8, 117, 13), // "AccessEffects"
QT_MOC_LITERAL(9, 131, 4) // "type"

    },
    "machine::BackendMemory\0"
    "external_backend_change_notify\0\0"
    "const BackendMemory*\0mem_access\0"
    "uint32_t\0start_addr\0last_addr\0"
    "AccessEffects\0type"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_machine__BackendMemory[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    4,   19,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 5, 0x80000000 | 5, 0x80000000 | 8,    4,    6,    7,    9,

       0        // eod
};

void machine::BackendMemory::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<BackendMemory *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->external_backend_change_notify((*reinterpret_cast< const BackendMemory*(*)>(_a[1])),(*reinterpret_cast< uint32_t(*)>(_a[2])),(*reinterpret_cast< uint32_t(*)>(_a[3])),(*reinterpret_cast< AccessEffects(*)>(_a[4]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (BackendMemory::*)(const BackendMemory * , uint32_t , uint32_t , AccessEffects ) const;
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&BackendMemory::external_backend_change_notify)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject machine::BackendMemory::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_machine__BackendMemory.data,
    qt_meta_data_machine__BackendMemory,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *machine::BackendMemory::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *machine::BackendMemory::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_machine__BackendMemory.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int machine::BackendMemory::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
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

// SIGNAL 0
void machine::BackendMemory::external_backend_change_notify(const BackendMemory * _t1, uint32_t _t2, uint32_t _t3, AccessEffects _t4)const
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t4))) };
    QMetaObject::activate(const_cast< machine::BackendMemory *>(this), &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
