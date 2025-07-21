/****************************************************************************
** Meta object code from reading C++ file 'memorymodel.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.13)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../../src/gui/windows/memory/memorymodel.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'memorymodel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.13. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MemoryModel_t {
    QByteArrayData data[12];
    char stringdata0[133];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MemoryModel_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MemoryModel_t qt_meta_stringdata_MemoryModel = {
    {
QT_MOC_LITERAL(0, 0, 11), // "MemoryModel"
QT_MOC_LITERAL(1, 12, 17), // "cell_size_changed"
QT_MOC_LITERAL(2, 30, 0), // ""
QT_MOC_LITERAL(3, 31, 10), // "setup_done"
QT_MOC_LITERAL(4, 42, 5), // "setup"
QT_MOC_LITERAL(5, 48, 17), // "machine::Machine*"
QT_MOC_LITERAL(6, 66, 7), // "machine"
QT_MOC_LITERAL(7, 74, 13), // "set_cell_size"
QT_MOC_LITERAL(8, 88, 5), // "index"
QT_MOC_LITERAL(9, 94, 17), // "check_for_updates"
QT_MOC_LITERAL(10, 112, 13), // "cached_access"
QT_MOC_LITERAL(11, 126, 6) // "cached"

    },
    "MemoryModel\0cell_size_changed\0\0"
    "setup_done\0setup\0machine::Machine*\0"
    "machine\0set_cell_size\0index\0"
    "check_for_updates\0cached_access\0cached"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MemoryModel[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   44,    2, 0x06 /* Public */,
       3,    0,   45,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    1,   46,    2, 0x0a /* Public */,
       7,    1,   49,    2, 0x0a /* Public */,
       9,    0,   52,    2, 0x0a /* Public */,
      10,    1,   53,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 5,    6,
    QMetaType::Void, QMetaType::Int,    8,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   11,

       0        // eod
};

void MemoryModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MemoryModel *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->cell_size_changed(); break;
        case 1: _t->setup_done(); break;
        case 2: _t->setup((*reinterpret_cast< machine::Machine*(*)>(_a[1]))); break;
        case 3: _t->set_cell_size((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->check_for_updates(); break;
        case 5: _t->cached_access((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 2:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< machine::Machine* >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (MemoryModel::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MemoryModel::cell_size_changed)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (MemoryModel::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MemoryModel::setup_done)) {
                *result = 1;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject MemoryModel::staticMetaObject = { {
    QMetaObject::SuperData::link<QAbstractTableModel::staticMetaObject>(),
    qt_meta_stringdata_MemoryModel.data,
    qt_meta_data_MemoryModel,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *MemoryModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MemoryModel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MemoryModel.stringdata0))
        return static_cast<void*>(this);
    return QAbstractTableModel::qt_metacast(_clname);
}

int MemoryModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractTableModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void MemoryModel::cell_size_changed()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void MemoryModel::setup_done()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
