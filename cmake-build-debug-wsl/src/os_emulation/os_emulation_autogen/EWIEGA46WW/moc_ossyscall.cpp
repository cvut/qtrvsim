/****************************************************************************
** Meta object code from reading C++ file 'ossyscall.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.13)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../../src/os_emulation/ossyscall.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ossyscall.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.13. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_osemu__OsSyscallExceptionHandler_t {
    QByteArrayData data[10];
    char stringdata0[94];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_osemu__OsSyscallExceptionHandler_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_osemu__OsSyscallExceptionHandler_t qt_meta_stringdata_osemu__OsSyscallExceptionHandler = {
    {
QT_MOC_LITERAL(0, 0, 32), // "osemu::OsSyscallExceptionHandler"
QT_MOC_LITERAL(1, 33, 12), // "char_written"
QT_MOC_LITERAL(2, 46, 0), // ""
QT_MOC_LITERAL(3, 47, 2), // "fd"
QT_MOC_LITERAL(4, 50, 3), // "val"
QT_MOC_LITERAL(5, 54, 12), // "rx_byte_pool"
QT_MOC_LITERAL(6, 67, 5), // "uint&"
QT_MOC_LITERAL(7, 73, 4), // "data"
QT_MOC_LITERAL(8, 78, 5), // "bool&"
QT_MOC_LITERAL(9, 84, 9) // "available"

    },
    "osemu::OsSyscallExceptionHandler\0"
    "char_written\0\0fd\0val\0rx_byte_pool\0"
    "uint&\0data\0bool&\0available"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_osemu__OsSyscallExceptionHandler[] = {

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
       5,    3,   29,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::UInt,    3,    4,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 6, 0x80000000 | 8,    3,    7,    9,

       0        // eod
};

void osemu::OsSyscallExceptionHandler::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<OsSyscallExceptionHandler *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->char_written((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< uint(*)>(_a[2]))); break;
        case 1: _t->rx_byte_pool((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< uint(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (OsSyscallExceptionHandler::*)(int , unsigned int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&OsSyscallExceptionHandler::char_written)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (OsSyscallExceptionHandler::*)(int , unsigned int & , bool & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&OsSyscallExceptionHandler::rx_byte_pool)) {
                *result = 1;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject osemu::OsSyscallExceptionHandler::staticMetaObject = { {
    QMetaObject::SuperData::link<machine::ExceptionHandler::staticMetaObject>(),
    qt_meta_stringdata_osemu__OsSyscallExceptionHandler.data,
    qt_meta_data_osemu__OsSyscallExceptionHandler,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *osemu::OsSyscallExceptionHandler::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *osemu::OsSyscallExceptionHandler::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_osemu__OsSyscallExceptionHandler.stringdata0))
        return static_cast<void*>(this);
    return machine::ExceptionHandler::qt_metacast(_clname);
}

int osemu::OsSyscallExceptionHandler::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = machine::ExceptionHandler::qt_metacall(_c, _id, _a);
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
void osemu::OsSyscallExceptionHandler::char_written(int _t1, unsigned int _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void osemu::OsSyscallExceptionHandler::rx_byte_pool(int _t1, unsigned int & _t2, bool & _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
