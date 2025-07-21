/****************************************************************************
** Meta object code from reading C++ file 'serialport.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.13)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../../src/machine/memory/backend/serialport.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'serialport.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.13. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_machine__SerialPort_t {
    QByteArrayData data[19];
    char stringdata0[188];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_machine__SerialPort_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_machine__SerialPort_t qt_meta_stringdata_machine__SerialPort = {
    {
QT_MOC_LITERAL(0, 0, 19), // "machine::SerialPort"
QT_MOC_LITERAL(1, 20, 7), // "tx_byte"
QT_MOC_LITERAL(2, 28, 0), // ""
QT_MOC_LITERAL(3, 29, 4), // "data"
QT_MOC_LITERAL(4, 34, 12), // "rx_byte_pool"
QT_MOC_LITERAL(5, 47, 2), // "fd"
QT_MOC_LITERAL(6, 50, 5), // "uint&"
QT_MOC_LITERAL(7, 56, 5), // "bool&"
QT_MOC_LITERAL(8, 62, 9), // "available"
QT_MOC_LITERAL(9, 72, 18), // "write_notification"
QT_MOC_LITERAL(10, 91, 6), // "Offset"
QT_MOC_LITERAL(11, 98, 7), // "address"
QT_MOC_LITERAL(12, 106, 8), // "uint32_t"
QT_MOC_LITERAL(13, 115, 5), // "value"
QT_MOC_LITERAL(14, 121, 17), // "read_notification"
QT_MOC_LITERAL(15, 139, 16), // "signal_interrupt"
QT_MOC_LITERAL(16, 156, 9), // "irq_level"
QT_MOC_LITERAL(17, 166, 6), // "active"
QT_MOC_LITERAL(18, 173, 14) // "rx_queue_check"

    },
    "machine::SerialPort\0tx_byte\0\0data\0"
    "rx_byte_pool\0fd\0uint&\0bool&\0available\0"
    "write_notification\0Offset\0address\0"
    "uint32_t\0value\0read_notification\0"
    "signal_interrupt\0irq_level\0active\0"
    "rx_queue_check"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_machine__SerialPort[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   44,    2, 0x06 /* Public */,
       4,    3,   47,    2, 0x06 /* Public */,
       9,    2,   54,    2, 0x06 /* Public */,
      14,    2,   59,    2, 0x06 /* Public */,
      15,    2,   64,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      18,    0,   69,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::UInt,    3,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 6, 0x80000000 | 7,    5,    3,    8,
    QMetaType::Void, 0x80000000 | 10, 0x80000000 | 12,   11,   13,
    QMetaType::Void, 0x80000000 | 10, 0x80000000 | 12,   11,   13,
    QMetaType::Void, QMetaType::UInt, QMetaType::Bool,   16,   17,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

void machine::SerialPort::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<SerialPort *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->tx_byte((*reinterpret_cast< uint(*)>(_a[1]))); break;
        case 1: _t->rx_byte_pool((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< uint(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 2: _t->write_notification((*reinterpret_cast< Offset(*)>(_a[1])),(*reinterpret_cast< uint32_t(*)>(_a[2]))); break;
        case 3: _t->read_notification((*reinterpret_cast< Offset(*)>(_a[1])),(*reinterpret_cast< uint32_t(*)>(_a[2]))); break;
        case 4: _t->signal_interrupt((*reinterpret_cast< uint(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 5: _t->rx_queue_check(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (SerialPort::*)(unsigned int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SerialPort::tx_byte)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (SerialPort::*)(int , unsigned int & , bool & ) const;
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SerialPort::rx_byte_pool)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (SerialPort::*)(Offset , uint32_t );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SerialPort::write_notification)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (SerialPort::*)(Offset , uint32_t ) const;
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SerialPort::read_notification)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (SerialPort::*)(uint , bool ) const;
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SerialPort::signal_interrupt)) {
                *result = 4;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject machine::SerialPort::staticMetaObject = { {
    QMetaObject::SuperData::link<BackendMemory::staticMetaObject>(),
    qt_meta_stringdata_machine__SerialPort.data,
    qt_meta_data_machine__SerialPort,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *machine::SerialPort::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *machine::SerialPort::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_machine__SerialPort.stringdata0))
        return static_cast<void*>(this);
    return BackendMemory::qt_metacast(_clname);
}

int machine::SerialPort::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = BackendMemory::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void machine::SerialPort::tx_byte(unsigned int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void machine::SerialPort::rx_byte_pool(int _t1, unsigned int & _t2, bool & _t3)const
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(const_cast< machine::SerialPort *>(this), &staticMetaObject, 1, _a);
}

// SIGNAL 2
void machine::SerialPort::write_notification(Offset _t1, uint32_t _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void machine::SerialPort::read_notification(Offset _t1, uint32_t _t2)const
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(const_cast< machine::SerialPort *>(this), &staticMetaObject, 3, _a);
}

// SIGNAL 4
void machine::SerialPort::signal_interrupt(uint _t1, bool _t2)const
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(const_cast< machine::SerialPort *>(this), &staticMetaObject, 4, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
