/****************************************************************************
** Meta object code from reading C++ file 'peripspiled.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.13)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../../src/machine/memory/backend/peripspiled.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'peripspiled.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.13. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_machine__PeripSpiLed_t {
    QByteArrayData data[19];
    char stringdata0[246];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_machine__PeripSpiLed_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_machine__PeripSpiLed_t qt_meta_stringdata_machine__PeripSpiLed = {
    {
QT_MOC_LITERAL(0, 0, 20), // "machine::PeripSpiLed"
QT_MOC_LITERAL(1, 21, 18), // "write_notification"
QT_MOC_LITERAL(2, 40, 0), // ""
QT_MOC_LITERAL(3, 41, 6), // "Offset"
QT_MOC_LITERAL(4, 48, 7), // "address"
QT_MOC_LITERAL(5, 56, 8), // "uint32_t"
QT_MOC_LITERAL(6, 65, 5), // "value"
QT_MOC_LITERAL(7, 71, 17), // "read_notification"
QT_MOC_LITERAL(8, 89, 16), // "led_line_changed"
QT_MOC_LITERAL(9, 106, 3), // "val"
QT_MOC_LITERAL(10, 110, 16), // "led_rgb1_changed"
QT_MOC_LITERAL(11, 127, 16), // "led_rgb2_changed"
QT_MOC_LITERAL(12, 144, 15), // "red_knob_update"
QT_MOC_LITERAL(13, 160, 17), // "green_knob_update"
QT_MOC_LITERAL(14, 178, 16), // "blue_knob_update"
QT_MOC_LITERAL(15, 195, 13), // "red_knob_push"
QT_MOC_LITERAL(16, 209, 5), // "state"
QT_MOC_LITERAL(17, 215, 15), // "green_knob_push"
QT_MOC_LITERAL(18, 231, 14) // "blue_knob_push"

    },
    "machine::PeripSpiLed\0write_notification\0"
    "\0Offset\0address\0uint32_t\0value\0"
    "read_notification\0led_line_changed\0"
    "val\0led_rgb1_changed\0led_rgb2_changed\0"
    "red_knob_update\0green_knob_update\0"
    "blue_knob_update\0red_knob_push\0state\0"
    "green_knob_push\0blue_knob_push"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_machine__PeripSpiLed[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   69,    2, 0x06 /* Public */,
       7,    2,   74,    2, 0x06 /* Public */,
       8,    1,   79,    2, 0x06 /* Public */,
      10,    1,   82,    2, 0x06 /* Public */,
      11,    1,   85,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      12,    1,   88,    2, 0x0a /* Public */,
      13,    1,   91,    2, 0x0a /* Public */,
      14,    1,   94,    2, 0x0a /* Public */,
      15,    1,   97,    2, 0x0a /* Public */,
      17,    1,  100,    2, 0x0a /* Public */,
      18,    1,  103,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 5,    4,    6,
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 5,    4,    6,
    QMetaType::Void, QMetaType::UInt,    9,
    QMetaType::Void, QMetaType::UInt,    9,
    QMetaType::Void, QMetaType::UInt,    9,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    9,
    QMetaType::Void, QMetaType::Int,    9,
    QMetaType::Void, QMetaType::Int,    9,
    QMetaType::Void, QMetaType::Bool,   16,
    QMetaType::Void, QMetaType::Bool,   16,
    QMetaType::Void, QMetaType::Bool,   16,

       0        // eod
};

void machine::PeripSpiLed::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<PeripSpiLed *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->write_notification((*reinterpret_cast< Offset(*)>(_a[1])),(*reinterpret_cast< uint32_t(*)>(_a[2]))); break;
        case 1: _t->read_notification((*reinterpret_cast< Offset(*)>(_a[1])),(*reinterpret_cast< uint32_t(*)>(_a[2]))); break;
        case 2: _t->led_line_changed((*reinterpret_cast< uint(*)>(_a[1]))); break;
        case 3: _t->led_rgb1_changed((*reinterpret_cast< uint(*)>(_a[1]))); break;
        case 4: _t->led_rgb2_changed((*reinterpret_cast< uint(*)>(_a[1]))); break;
        case 5: _t->red_knob_update((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: _t->green_knob_update((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->blue_knob_update((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: _t->red_knob_push((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 9: _t->green_knob_push((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 10: _t->blue_knob_push((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (PeripSpiLed::*)(Offset , uint32_t ) const;
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PeripSpiLed::write_notification)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (PeripSpiLed::*)(Offset , uint32_t ) const;
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PeripSpiLed::read_notification)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (PeripSpiLed::*)(uint ) const;
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PeripSpiLed::led_line_changed)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (PeripSpiLed::*)(uint ) const;
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PeripSpiLed::led_rgb1_changed)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (PeripSpiLed::*)(uint ) const;
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PeripSpiLed::led_rgb2_changed)) {
                *result = 4;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject machine::PeripSpiLed::staticMetaObject = { {
    QMetaObject::SuperData::link<BackendMemory::staticMetaObject>(),
    qt_meta_stringdata_machine__PeripSpiLed.data,
    qt_meta_data_machine__PeripSpiLed,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *machine::PeripSpiLed::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *machine::PeripSpiLed::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_machine__PeripSpiLed.stringdata0))
        return static_cast<void*>(this);
    return BackendMemory::qt_metacast(_clname);
}

int machine::PeripSpiLed::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = BackendMemory::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 11;
    }
    return _id;
}

// SIGNAL 0
void machine::PeripSpiLed::write_notification(Offset _t1, uint32_t _t2)const
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(const_cast< machine::PeripSpiLed *>(this), &staticMetaObject, 0, _a);
}

// SIGNAL 1
void machine::PeripSpiLed::read_notification(Offset _t1, uint32_t _t2)const
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(const_cast< machine::PeripSpiLed *>(this), &staticMetaObject, 1, _a);
}

// SIGNAL 2
void machine::PeripSpiLed::led_line_changed(uint _t1)const
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(const_cast< machine::PeripSpiLed *>(this), &staticMetaObject, 2, _a);
}

// SIGNAL 3
void machine::PeripSpiLed::led_rgb1_changed(uint _t1)const
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(const_cast< machine::PeripSpiLed *>(this), &staticMetaObject, 3, _a);
}

// SIGNAL 4
void machine::PeripSpiLed::led_rgb2_changed(uint _t1)const
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(const_cast< machine::PeripSpiLed *>(this), &staticMetaObject, 4, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
