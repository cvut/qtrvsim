/****************************************************************************
** Meta object code from reading C++ file 'cacheview.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.13)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../../src/gui/windows/cache/cacheview.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'cacheview.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.13. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_CacheAddressBlock_t {
    QByteArrayData data[13];
    char stringdata0[101];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CacheAddressBlock_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CacheAddressBlock_t qt_meta_stringdata_CacheAddressBlock = {
    {
QT_MOC_LITERAL(0, 0, 17), // "CacheAddressBlock"
QT_MOC_LITERAL(1, 18, 12), // "cache_update"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 8), // "associat"
QT_MOC_LITERAL(4, 41, 3), // "set"
QT_MOC_LITERAL(5, 45, 3), // "col"
QT_MOC_LITERAL(6, 49, 5), // "valid"
QT_MOC_LITERAL(7, 55, 5), // "dirty"
QT_MOC_LITERAL(8, 61, 8), // "uint32_t"
QT_MOC_LITERAL(9, 70, 3), // "tag"
QT_MOC_LITERAL(10, 74, 15), // "const uint32_t*"
QT_MOC_LITERAL(11, 90, 4), // "data"
QT_MOC_LITERAL(12, 95, 5) // "write"

    },
    "CacheAddressBlock\0cache_update\0\0"
    "associat\0set\0col\0valid\0dirty\0uint32_t\0"
    "tag\0const uint32_t*\0data\0write"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CacheAddressBlock[] = {

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
       1,    8,   19,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::UInt, QMetaType::UInt, QMetaType::UInt, QMetaType::Bool, QMetaType::Bool, 0x80000000 | 8, 0x80000000 | 10, QMetaType::Bool,    3,    4,    5,    6,    7,    9,   11,   12,

       0        // eod
};

void CacheAddressBlock::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CacheAddressBlock *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->cache_update((*reinterpret_cast< uint(*)>(_a[1])),(*reinterpret_cast< uint(*)>(_a[2])),(*reinterpret_cast< uint(*)>(_a[3])),(*reinterpret_cast< bool(*)>(_a[4])),(*reinterpret_cast< bool(*)>(_a[5])),(*reinterpret_cast< uint32_t(*)>(_a[6])),(*reinterpret_cast< const uint32_t*(*)>(_a[7])),(*reinterpret_cast< bool(*)>(_a[8]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject CacheAddressBlock::staticMetaObject = { {
    QMetaObject::SuperData::link<QGraphicsObject::staticMetaObject>(),
    qt_meta_stringdata_CacheAddressBlock.data,
    qt_meta_data_CacheAddressBlock,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CacheAddressBlock::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CacheAddressBlock::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CacheAddressBlock.stringdata0))
        return static_cast<void*>(this);
    return QGraphicsObject::qt_metacast(_clname);
}

int CacheAddressBlock::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGraphicsObject::qt_metacall(_c, _id, _a);
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
struct qt_meta_stringdata_CacheViewBlock_t {
    QByteArrayData data[13];
    char stringdata0[98];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CacheViewBlock_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CacheViewBlock_t qt_meta_stringdata_CacheViewBlock = {
    {
QT_MOC_LITERAL(0, 0, 14), // "CacheViewBlock"
QT_MOC_LITERAL(1, 15, 12), // "cache_update"
QT_MOC_LITERAL(2, 28, 0), // ""
QT_MOC_LITERAL(3, 29, 8), // "associat"
QT_MOC_LITERAL(4, 38, 3), // "set"
QT_MOC_LITERAL(5, 42, 3), // "col"
QT_MOC_LITERAL(6, 46, 5), // "valid"
QT_MOC_LITERAL(7, 52, 5), // "dirty"
QT_MOC_LITERAL(8, 58, 8), // "uint32_t"
QT_MOC_LITERAL(9, 67, 3), // "tag"
QT_MOC_LITERAL(10, 71, 15), // "const uint32_t*"
QT_MOC_LITERAL(11, 87, 4), // "data"
QT_MOC_LITERAL(12, 92, 5) // "write"

    },
    "CacheViewBlock\0cache_update\0\0associat\0"
    "set\0col\0valid\0dirty\0uint32_t\0tag\0"
    "const uint32_t*\0data\0write"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CacheViewBlock[] = {

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
       1,    8,   19,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::UInt, QMetaType::UInt, QMetaType::UInt, QMetaType::Bool, QMetaType::Bool, 0x80000000 | 8, 0x80000000 | 10, QMetaType::Bool,    3,    4,    5,    6,    7,    9,   11,   12,

       0        // eod
};

void CacheViewBlock::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CacheViewBlock *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->cache_update((*reinterpret_cast< uint(*)>(_a[1])),(*reinterpret_cast< uint(*)>(_a[2])),(*reinterpret_cast< uint(*)>(_a[3])),(*reinterpret_cast< bool(*)>(_a[4])),(*reinterpret_cast< bool(*)>(_a[5])),(*reinterpret_cast< uint32_t(*)>(_a[6])),(*reinterpret_cast< const uint32_t*(*)>(_a[7])),(*reinterpret_cast< bool(*)>(_a[8]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject CacheViewBlock::staticMetaObject = { {
    QMetaObject::SuperData::link<QGraphicsObject::staticMetaObject>(),
    qt_meta_stringdata_CacheViewBlock.data,
    qt_meta_data_CacheViewBlock,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CacheViewBlock::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CacheViewBlock::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CacheViewBlock.stringdata0))
        return static_cast<void*>(this);
    return QGraphicsObject::qt_metacast(_clname);
}

int CacheViewBlock::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGraphicsObject::qt_metacall(_c, _id, _a);
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
struct qt_meta_stringdata_CacheViewScene_t {
    QByteArrayData data[1];
    char stringdata0[15];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CacheViewScene_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CacheViewScene_t qt_meta_stringdata_CacheViewScene = {
    {
QT_MOC_LITERAL(0, 0, 14) // "CacheViewScene"

    },
    "CacheViewScene"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CacheViewScene[] = {

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

void CacheViewScene::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

QT_INIT_METAOBJECT const QMetaObject CacheViewScene::staticMetaObject = { {
    QMetaObject::SuperData::link<QGraphicsScene::staticMetaObject>(),
    qt_meta_stringdata_CacheViewScene.data,
    qt_meta_data_CacheViewScene,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CacheViewScene::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CacheViewScene::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CacheViewScene.stringdata0))
        return static_cast<void*>(this);
    return QGraphicsScene::qt_metacast(_clname);
}

int CacheViewScene::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGraphicsScene::qt_metacall(_c, _id, _a);
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
