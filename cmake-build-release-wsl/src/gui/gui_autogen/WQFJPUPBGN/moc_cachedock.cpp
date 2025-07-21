/****************************************************************************
** Meta object code from reading C++ file 'cachedock.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.13)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../../src/gui/windows/cache/cachedock.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'cachedock.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.13. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_CacheDock_t {
    QByteArrayData data[11];
    char stringdata0[134];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CacheDock_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CacheDock_t qt_meta_stringdata_CacheDock = {
    {
QT_MOC_LITERAL(0, 0, 9), // "CacheDock"
QT_MOC_LITERAL(1, 10, 10), // "hit_update"
QT_MOC_LITERAL(2, 21, 0), // ""
QT_MOC_LITERAL(3, 22, 11), // "miss_update"
QT_MOC_LITERAL(4, 34, 19), // "memory_reads_update"
QT_MOC_LITERAL(5, 54, 3), // "val"
QT_MOC_LITERAL(6, 58, 20), // "memory_writes_update"
QT_MOC_LITERAL(7, 79, 17), // "statistics_update"
QT_MOC_LITERAL(8, 97, 14), // "stalled_cycles"
QT_MOC_LITERAL(9, 112, 12), // "speed_improv"
QT_MOC_LITERAL(10, 125, 8) // "hit_rate"

    },
    "CacheDock\0hit_update\0\0miss_update\0"
    "memory_reads_update\0val\0memory_writes_update\0"
    "statistics_update\0stalled_cycles\0"
    "speed_improv\0hit_rate"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CacheDock[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   39,    2, 0x08 /* Private */,
       3,    1,   42,    2, 0x08 /* Private */,
       4,    1,   45,    2, 0x08 /* Private */,
       6,    1,   48,    2, 0x08 /* Private */,
       7,    3,   51,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::UInt,    2,
    QMetaType::Void, QMetaType::UInt,    2,
    QMetaType::Void, QMetaType::UInt,    5,
    QMetaType::Void, QMetaType::UInt,    5,
    QMetaType::Void, QMetaType::UInt, QMetaType::Double, QMetaType::Double,    8,    9,   10,

       0        // eod
};

void CacheDock::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CacheDock *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->hit_update((*reinterpret_cast< uint(*)>(_a[1]))); break;
        case 1: _t->miss_update((*reinterpret_cast< uint(*)>(_a[1]))); break;
        case 2: _t->memory_reads_update((*reinterpret_cast< uint(*)>(_a[1]))); break;
        case 3: _t->memory_writes_update((*reinterpret_cast< uint(*)>(_a[1]))); break;
        case 4: _t->statistics_update((*reinterpret_cast< uint(*)>(_a[1])),(*reinterpret_cast< double(*)>(_a[2])),(*reinterpret_cast< double(*)>(_a[3]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject CacheDock::staticMetaObject = { {
    QMetaObject::SuperData::link<QDockWidget::staticMetaObject>(),
    qt_meta_stringdata_CacheDock.data,
    qt_meta_data_CacheDock,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CacheDock::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CacheDock::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CacheDock.stringdata0))
        return static_cast<void*>(this);
    return QDockWidget::qt_metacast(_clname);
}

int CacheDock::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDockWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
