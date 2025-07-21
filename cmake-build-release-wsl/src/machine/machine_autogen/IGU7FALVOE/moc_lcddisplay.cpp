/****************************************************************************
** Meta object code from reading C++ file 'lcddisplay.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.13)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../../src/machine/memory/backend/lcddisplay.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'lcddisplay.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.13. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_machine__LcdDisplay_t {
    QByteArrayData data[15];
    char stringdata0[117];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_machine__LcdDisplay_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_machine__LcdDisplay_t qt_meta_stringdata_machine__LcdDisplay = {
    {
QT_MOC_LITERAL(0, 0, 19), // "machine::LcdDisplay"
QT_MOC_LITERAL(1, 20, 18), // "write_notification"
QT_MOC_LITERAL(2, 39, 0), // ""
QT_MOC_LITERAL(3, 40, 6), // "Offset"
QT_MOC_LITERAL(4, 47, 6), // "offset"
QT_MOC_LITERAL(5, 54, 8), // "uint32_t"
QT_MOC_LITERAL(6, 63, 5), // "value"
QT_MOC_LITERAL(7, 69, 17), // "read_notification"
QT_MOC_LITERAL(8, 87, 12), // "pixel_update"
QT_MOC_LITERAL(9, 100, 6), // "size_t"
QT_MOC_LITERAL(10, 107, 1), // "x"
QT_MOC_LITERAL(11, 109, 1), // "y"
QT_MOC_LITERAL(12, 111, 1), // "r"
QT_MOC_LITERAL(13, 113, 1), // "g"
QT_MOC_LITERAL(14, 115, 1) // "b"

    },
    "machine::LcdDisplay\0write_notification\0"
    "\0Offset\0offset\0uint32_t\0value\0"
    "read_notification\0pixel_update\0size_t\0"
    "x\0y\0r\0g\0b"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_machine__LcdDisplay[] = {

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
       1,    2,   29,    2, 0x06 /* Public */,
       7,    2,   34,    2, 0x06 /* Public */,
       8,    5,   39,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 5,    4,    6,
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 5,    4,    6,
    QMetaType::Void, 0x80000000 | 9, 0x80000000 | 9, QMetaType::UInt, QMetaType::UInt, QMetaType::UInt,   10,   11,   12,   13,   14,

       0        // eod
};

void machine::LcdDisplay::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<LcdDisplay *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->write_notification((*reinterpret_cast< Offset(*)>(_a[1])),(*reinterpret_cast< uint32_t(*)>(_a[2]))); break;
        case 1: _t->read_notification((*reinterpret_cast< Offset(*)>(_a[1])),(*reinterpret_cast< uint32_t(*)>(_a[2]))); break;
        case 2: _t->pixel_update((*reinterpret_cast< size_t(*)>(_a[1])),(*reinterpret_cast< size_t(*)>(_a[2])),(*reinterpret_cast< uint(*)>(_a[3])),(*reinterpret_cast< uint(*)>(_a[4])),(*reinterpret_cast< uint(*)>(_a[5]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (LcdDisplay::*)(Offset , uint32_t ) const;
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&LcdDisplay::write_notification)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (LcdDisplay::*)(Offset , uint32_t ) const;
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&LcdDisplay::read_notification)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (LcdDisplay::*)(size_t , size_t , uint , uint , uint );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&LcdDisplay::pixel_update)) {
                *result = 2;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject machine::LcdDisplay::staticMetaObject = { {
    QMetaObject::SuperData::link<BackendMemory::staticMetaObject>(),
    qt_meta_stringdata_machine__LcdDisplay.data,
    qt_meta_data_machine__LcdDisplay,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *machine::LcdDisplay::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *machine::LcdDisplay::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_machine__LcdDisplay.stringdata0))
        return static_cast<void*>(this);
    return BackendMemory::qt_metacast(_clname);
}

int machine::LcdDisplay::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = BackendMemory::qt_metacall(_c, _id, _a);
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
void machine::LcdDisplay::write_notification(Offset _t1, uint32_t _t2)const
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(const_cast< machine::LcdDisplay *>(this), &staticMetaObject, 0, _a);
}

// SIGNAL 1
void machine::LcdDisplay::read_notification(Offset _t1, uint32_t _t2)const
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(const_cast< machine::LcdDisplay *>(this), &staticMetaObject, 1, _a);
}

// SIGNAL 2
void machine::LcdDisplay::pixel_update(size_t _t1, size_t _t2, uint _t3, uint _t4, uint _t5)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t4))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t5))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
