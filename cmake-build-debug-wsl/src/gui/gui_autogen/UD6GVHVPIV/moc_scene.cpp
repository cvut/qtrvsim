/****************************************************************************
** Meta object code from reading C++ file 'scene.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.13)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../../src/gui/windows/coreview/scene.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'scene.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.13. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_CoreViewScene_t {
    QByteArrayData data[13];
    char stringdata0[222];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CoreViewScene_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CoreViewScene_t qt_meta_stringdata_CoreViewScene = {
    {
QT_MOC_LITERAL(0, 0, 13), // "CoreViewScene"
QT_MOC_LITERAL(1, 14, 17), // "request_registers"
QT_MOC_LITERAL(2, 32, 0), // ""
QT_MOC_LITERAL(3, 33, 19), // "request_data_memory"
QT_MOC_LITERAL(4, 53, 22), // "request_program_memory"
QT_MOC_LITERAL(5, 76, 31), // "request_jump_to_program_counter"
QT_MOC_LITERAL(6, 108, 16), // "machine::Address"
QT_MOC_LITERAL(7, 125, 4), // "addr"
QT_MOC_LITERAL(8, 130, 21), // "request_cache_program"
QT_MOC_LITERAL(9, 152, 18), // "request_cache_data"
QT_MOC_LITERAL(10, 171, 19), // "request_peripherals"
QT_MOC_LITERAL(11, 191, 16), // "request_terminal"
QT_MOC_LITERAL(12, 208, 13) // "update_values"

    },
    "CoreViewScene\0request_registers\0\0"
    "request_data_memory\0request_program_memory\0"
    "request_jump_to_program_counter\0"
    "machine::Address\0addr\0request_cache_program\0"
    "request_cache_data\0request_peripherals\0"
    "request_terminal\0update_values"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CoreViewScene[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       8,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   59,    2, 0x06 /* Public */,
       3,    0,   60,    2, 0x06 /* Public */,
       4,    0,   61,    2, 0x06 /* Public */,
       5,    1,   62,    2, 0x06 /* Public */,
       8,    0,   65,    2, 0x06 /* Public */,
       9,    0,   66,    2, 0x06 /* Public */,
      10,    0,   67,    2, 0x06 /* Public */,
      11,    0,   68,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      12,    0,   69,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 6,    7,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

void CoreViewScene::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CoreViewScene *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->request_registers(); break;
        case 1: _t->request_data_memory(); break;
        case 2: _t->request_program_memory(); break;
        case 3: _t->request_jump_to_program_counter((*reinterpret_cast< machine::Address(*)>(_a[1]))); break;
        case 4: _t->request_cache_program(); break;
        case 5: _t->request_cache_data(); break;
        case 6: _t->request_peripherals(); break;
        case 7: _t->request_terminal(); break;
        case 8: _t->update_values(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 3:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< machine::Address >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (CoreViewScene::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CoreViewScene::request_registers)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (CoreViewScene::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CoreViewScene::request_data_memory)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (CoreViewScene::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CoreViewScene::request_program_memory)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (CoreViewScene::*)(machine::Address );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CoreViewScene::request_jump_to_program_counter)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (CoreViewScene::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CoreViewScene::request_cache_program)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (CoreViewScene::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CoreViewScene::request_cache_data)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (CoreViewScene::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CoreViewScene::request_peripherals)) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (CoreViewScene::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CoreViewScene::request_terminal)) {
                *result = 7;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject CoreViewScene::staticMetaObject = { {
    QMetaObject::SuperData::link<svgscene::SvgGraphicsScene::staticMetaObject>(),
    qt_meta_stringdata_CoreViewScene.data,
    qt_meta_data_CoreViewScene,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CoreViewScene::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CoreViewScene::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CoreViewScene.stringdata0))
        return static_cast<void*>(this);
    return svgscene::SvgGraphicsScene::qt_metacast(_clname);
}

int CoreViewScene::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = svgscene::SvgGraphicsScene::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void CoreViewScene::request_registers()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void CoreViewScene::request_data_memory()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void CoreViewScene::request_program_memory()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void CoreViewScene::request_jump_to_program_counter(machine::Address _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void CoreViewScene::request_cache_program()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void CoreViewScene::request_cache_data()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void CoreViewScene::request_peripherals()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}

// SIGNAL 7
void CoreViewScene::request_terminal()
{
    QMetaObject::activate(this, &staticMetaObject, 7, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
