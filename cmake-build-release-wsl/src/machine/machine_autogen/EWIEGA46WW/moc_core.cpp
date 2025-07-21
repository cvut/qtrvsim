/****************************************************************************
** Meta object code from reading C++ file 'core.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.13)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../../src/machine/core.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'core.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.13. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_machine__Core_t {
    QByteArrayData data[6];
    char stringdata0[74];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_machine__Core_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_machine__Core_t qt_meta_stringdata_machine__Core = {
    {
QT_MOC_LITERAL(0, 0, 13), // "machine::Core"
QT_MOC_LITERAL(1, 14, 25), // "stop_on_exception_reached"
QT_MOC_LITERAL(2, 40, 0), // ""
QT_MOC_LITERAL(3, 41, 12), // "step_started"
QT_MOC_LITERAL(4, 54, 9), // "step_done"
QT_MOC_LITERAL(5, 64, 9) // "CoreState"

    },
    "machine::Core\0stop_on_exception_reached\0"
    "\0step_started\0step_done\0CoreState"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_machine__Core[] = {

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
       1,    0,   29,    2, 0x06 /* Public */,
       3,    0,   30,    2, 0x06 /* Public */,
       4,    1,   31,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 5,    2,

       0        // eod
};

void machine::Core::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Core *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->stop_on_exception_reached(); break;
        case 1: _t->step_started(); break;
        case 2: _t->step_done((*reinterpret_cast< const CoreState(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (Core::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Core::stop_on_exception_reached)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (Core::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Core::step_started)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (Core::*)(const CoreState & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Core::step_done)) {
                *result = 2;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject machine::Core::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_machine__Core.data,
    qt_meta_data_machine__Core,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *machine::Core::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *machine::Core::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_machine__Core.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int machine::Core::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void machine::Core::stop_on_exception_reached()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void machine::Core::step_started()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void machine::Core::step_done(const CoreState & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
struct qt_meta_stringdata_machine__ExceptionHandler_t {
    QByteArrayData data[1];
    char stringdata0[26];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_machine__ExceptionHandler_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_machine__ExceptionHandler_t qt_meta_stringdata_machine__ExceptionHandler = {
    {
QT_MOC_LITERAL(0, 0, 25) // "machine::ExceptionHandler"

    },
    "machine::ExceptionHandler"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_machine__ExceptionHandler[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

void machine::ExceptionHandler::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

QT_INIT_METAOBJECT const QMetaObject machine::ExceptionHandler::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_machine__ExceptionHandler.data,
    qt_meta_data_machine__ExceptionHandler,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *machine::ExceptionHandler::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *machine::ExceptionHandler::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_machine__ExceptionHandler.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int machine::ExceptionHandler::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    return _id;
}
struct qt_meta_stringdata_machine__StopExceptionHandler_t {
    QByteArrayData data[1];
    char stringdata0[30];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_machine__StopExceptionHandler_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_machine__StopExceptionHandler_t qt_meta_stringdata_machine__StopExceptionHandler = {
    {
QT_MOC_LITERAL(0, 0, 29) // "machine::StopExceptionHandler"

    },
    "machine::StopExceptionHandler"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_machine__StopExceptionHandler[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

void machine::StopExceptionHandler::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

QT_INIT_METAOBJECT const QMetaObject machine::StopExceptionHandler::staticMetaObject = { {
    QMetaObject::SuperData::link<ExceptionHandler::staticMetaObject>(),
    qt_meta_stringdata_machine__StopExceptionHandler.data,
    qt_meta_data_machine__StopExceptionHandler,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *machine::StopExceptionHandler::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *machine::StopExceptionHandler::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_machine__StopExceptionHandler.stringdata0))
        return static_cast<void*>(this);
    return ExceptionHandler::qt_metacast(_clname);
}

int machine::StopExceptionHandler::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ExceptionHandler::qt_metacall(_c, _id, _a);
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
