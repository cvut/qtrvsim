/****************************************************************************
** Meta object code from reading C++ file 'symboltable.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.13)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../../src/machine/symboltable.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'symboltable.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.13. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_machine__SymbolTable_t {
    QByteArrayData data[9];
    char stringdata0[98];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_machine__SymbolTable_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_machine__SymbolTable_t qt_meta_stringdata_machine__SymbolTable = {
    {
QT_MOC_LITERAL(0, 0, 20), // "machine::SymbolTable"
QT_MOC_LITERAL(1, 21, 13), // "name_to_value"
QT_MOC_LITERAL(2, 35, 0), // ""
QT_MOC_LITERAL(3, 36, 12), // "SymbolValue&"
QT_MOC_LITERAL(4, 49, 5), // "value"
QT_MOC_LITERAL(5, 55, 4), // "name"
QT_MOC_LITERAL(6, 60, 16), // "location_to_name"
QT_MOC_LITERAL(7, 77, 8), // "QString&"
QT_MOC_LITERAL(8, 86, 11) // "SymbolValue"

    },
    "machine::SymbolTable\0name_to_value\0\0"
    "SymbolValue&\0value\0name\0location_to_name\0"
    "QString&\0SymbolValue"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_machine__SymbolTable[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    2,   24,    2, 0x0a /* Public */,
       6,    2,   29,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Bool, 0x80000000 | 3, QMetaType::QString,    4,    5,
    QMetaType::Bool, 0x80000000 | 7, 0x80000000 | 8,    5,    4,

       0        // eod
};

void machine::SymbolTable::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<SymbolTable *>(_o);
        (void)_t;
        switch (_id) {
        case 0: { bool _r = _t->name_to_value((*reinterpret_cast< SymbolValue(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 1: { bool _r = _t->location_to_name((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< SymbolValue(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject machine::SymbolTable::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_machine__SymbolTable.data,
    qt_meta_data_machine__SymbolTable,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *machine::SymbolTable::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *machine::SymbolTable::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_machine__SymbolTable.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int machine::SymbolTable::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
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
QT_WARNING_POP
QT_END_MOC_NAMESPACE
