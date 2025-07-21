/****************************************************************************
** Meta object code from reading C++ file 'cache.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.13)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../../src/machine/memory/cache/cache.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'cache.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.13. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_machine__Cache_t {
    QByteArrayData data[22];
    char stringdata0[219];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_machine__Cache_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_machine__Cache_t qt_meta_stringdata_machine__Cache = {
    {
QT_MOC_LITERAL(0, 0, 14), // "machine::Cache"
QT_MOC_LITERAL(1, 15, 10), // "hit_update"
QT_MOC_LITERAL(2, 26, 0), // ""
QT_MOC_LITERAL(3, 27, 8), // "uint32_t"
QT_MOC_LITERAL(4, 36, 11), // "miss_update"
QT_MOC_LITERAL(5, 48, 17), // "statistics_update"
QT_MOC_LITERAL(6, 66, 14), // "stalled_cycles"
QT_MOC_LITERAL(7, 81, 12), // "speed_improv"
QT_MOC_LITERAL(8, 94, 8), // "hit_rate"
QT_MOC_LITERAL(9, 103, 12), // "cache_update"
QT_MOC_LITERAL(10, 116, 6), // "size_t"
QT_MOC_LITERAL(11, 123, 3), // "way"
QT_MOC_LITERAL(12, 127, 3), // "row"
QT_MOC_LITERAL(13, 131, 3), // "col"
QT_MOC_LITERAL(14, 135, 5), // "valid"
QT_MOC_LITERAL(15, 141, 5), // "dirty"
QT_MOC_LITERAL(16, 147, 3), // "tag"
QT_MOC_LITERAL(17, 151, 15), // "const uint32_t*"
QT_MOC_LITERAL(18, 167, 4), // "data"
QT_MOC_LITERAL(19, 172, 5), // "write"
QT_MOC_LITERAL(20, 178, 20), // "memory_writes_update"
QT_MOC_LITERAL(21, 199, 19) // "memory_reads_update"

    },
    "machine::Cache\0hit_update\0\0uint32_t\0"
    "miss_update\0statistics_update\0"
    "stalled_cycles\0speed_improv\0hit_rate\0"
    "cache_update\0size_t\0way\0row\0col\0valid\0"
    "dirty\0tag\0const uint32_t*\0data\0write\0"
    "memory_writes_update\0memory_reads_update"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_machine__Cache[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       6,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   44,    2, 0x06 /* Public */,
       4,    1,   47,    2, 0x06 /* Public */,
       5,    3,   50,    2, 0x06 /* Public */,
       9,    8,   57,    2, 0x06 /* Public */,
      20,    1,   74,    2, 0x06 /* Public */,
      21,    1,   77,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void, 0x80000000 | 3, QMetaType::Double, QMetaType::Double,    6,    7,    8,
    QMetaType::Void, 0x80000000 | 10, 0x80000000 | 10, 0x80000000 | 10, QMetaType::Bool, QMetaType::Bool, 0x80000000 | 10, 0x80000000 | 17, QMetaType::Bool,   11,   12,   13,   14,   15,   16,   18,   19,
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void, 0x80000000 | 3,    2,

       0        // eod
};

void machine::Cache::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Cache *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->hit_update((*reinterpret_cast< uint32_t(*)>(_a[1]))); break;
        case 1: _t->miss_update((*reinterpret_cast< uint32_t(*)>(_a[1]))); break;
        case 2: _t->statistics_update((*reinterpret_cast< uint32_t(*)>(_a[1])),(*reinterpret_cast< double(*)>(_a[2])),(*reinterpret_cast< double(*)>(_a[3]))); break;
        case 3: _t->cache_update((*reinterpret_cast< size_t(*)>(_a[1])),(*reinterpret_cast< size_t(*)>(_a[2])),(*reinterpret_cast< size_t(*)>(_a[3])),(*reinterpret_cast< bool(*)>(_a[4])),(*reinterpret_cast< bool(*)>(_a[5])),(*reinterpret_cast< size_t(*)>(_a[6])),(*reinterpret_cast< const uint32_t*(*)>(_a[7])),(*reinterpret_cast< bool(*)>(_a[8]))); break;
        case 4: _t->memory_writes_update((*reinterpret_cast< uint32_t(*)>(_a[1]))); break;
        case 5: _t->memory_reads_update((*reinterpret_cast< uint32_t(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (Cache::*)(uint32_t ) const;
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Cache::hit_update)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (Cache::*)(uint32_t ) const;
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Cache::miss_update)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (Cache::*)(uint32_t , double , double ) const;
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Cache::statistics_update)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (Cache::*)(size_t , size_t , size_t , bool , bool , size_t , const uint32_t * , bool ) const;
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Cache::cache_update)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (Cache::*)(uint32_t ) const;
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Cache::memory_writes_update)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (Cache::*)(uint32_t ) const;
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Cache::memory_reads_update)) {
                *result = 5;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject machine::Cache::staticMetaObject = { {
    QMetaObject::SuperData::link<FrontendMemory::staticMetaObject>(),
    qt_meta_stringdata_machine__Cache.data,
    qt_meta_data_machine__Cache,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *machine::Cache::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *machine::Cache::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_machine__Cache.stringdata0))
        return static_cast<void*>(this);
    return FrontendMemory::qt_metacast(_clname);
}

int machine::Cache::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = FrontendMemory::qt_metacall(_c, _id, _a);
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
void machine::Cache::hit_update(uint32_t _t1)const
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(const_cast< machine::Cache *>(this), &staticMetaObject, 0, _a);
}

// SIGNAL 1
void machine::Cache::miss_update(uint32_t _t1)const
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(const_cast< machine::Cache *>(this), &staticMetaObject, 1, _a);
}

// SIGNAL 2
void machine::Cache::statistics_update(uint32_t _t1, double _t2, double _t3)const
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(const_cast< machine::Cache *>(this), &staticMetaObject, 2, _a);
}

// SIGNAL 3
void machine::Cache::cache_update(size_t _t1, size_t _t2, size_t _t3, bool _t4, bool _t5, size_t _t6, const uint32_t * _t7, bool _t8)const
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t4))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t5))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t6))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t7))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t8))) };
    QMetaObject::activate(const_cast< machine::Cache *>(this), &staticMetaObject, 3, _a);
}

// SIGNAL 4
void machine::Cache::memory_writes_update(uint32_t _t1)const
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(const_cast< machine::Cache *>(this), &staticMetaObject, 4, _a);
}

// SIGNAL 5
void machine::Cache::memory_reads_update(uint32_t _t1)const
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(const_cast< machine::Cache *>(this), &staticMetaObject, 5, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
