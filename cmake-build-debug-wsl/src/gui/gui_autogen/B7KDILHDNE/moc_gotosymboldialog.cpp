/****************************************************************************
** Meta object code from reading C++ file 'gotosymboldialog.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.13)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../../src/gui/dialogs/gotosymbol/gotosymboldialog.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'gotosymboldialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.13. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_GoToSymbolDialog_t {
    QByteArrayData data[11];
    char stringdata0[134];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_GoToSymbolDialog_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_GoToSymbolDialog_t qt_meta_stringdata_GoToSymbolDialog = {
    {
QT_MOC_LITERAL(0, 0, 16), // "GoToSymbolDialog"
QT_MOC_LITERAL(1, 17, 18), // "program_focus_addr"
QT_MOC_LITERAL(2, 36, 0), // ""
QT_MOC_LITERAL(3, 37, 16), // "machine::Address"
QT_MOC_LITERAL(4, 54, 17), // "memory_focus_addr"
QT_MOC_LITERAL(5, 72, 21), // "obtain_value_for_name"
QT_MOC_LITERAL(6, 94, 9), // "uint64_t&"
QT_MOC_LITERAL(7, 104, 5), // "value"
QT_MOC_LITERAL(8, 110, 4), // "name"
QT_MOC_LITERAL(9, 115, 9), // "show_prog"
QT_MOC_LITERAL(10, 125, 8) // "show_mem"

    },
    "GoToSymbolDialog\0program_focus_addr\0"
    "\0machine::Address\0memory_focus_addr\0"
    "obtain_value_for_name\0uint64_t&\0value\0"
    "name\0show_prog\0show_mem"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_GoToSymbolDialog[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   39,    2, 0x06 /* Public */,
       4,    1,   42,    2, 0x06 /* Public */,
       5,    2,   45,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       9,    0,   50,    2, 0x0a /* Public */,
      10,    0,   51,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Bool, 0x80000000 | 6, QMetaType::QString,    7,    8,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void GoToSymbolDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<GoToSymbolDialog *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->program_focus_addr((*reinterpret_cast< machine::Address(*)>(_a[1]))); break;
        case 1: _t->memory_focus_addr((*reinterpret_cast< machine::Address(*)>(_a[1]))); break;
        case 2: { bool _r = _t->obtain_value_for_name((*reinterpret_cast< uint64_t(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 3: _t->show_prog(); break;
        case 4: _t->show_mem(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< machine::Address >(); break;
            }
            break;
        case 1:
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
            using _t = void (GoToSymbolDialog::*)(machine::Address );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&GoToSymbolDialog::program_focus_addr)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (GoToSymbolDialog::*)(machine::Address );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&GoToSymbolDialog::memory_focus_addr)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = bool (GoToSymbolDialog::*)(uint64_t & , const QString & ) const;
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&GoToSymbolDialog::obtain_value_for_name)) {
                *result = 2;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject GoToSymbolDialog::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_meta_stringdata_GoToSymbolDialog.data,
    qt_meta_data_GoToSymbolDialog,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *GoToSymbolDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *GoToSymbolDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_GoToSymbolDialog.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int GoToSymbolDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void GoToSymbolDialog::program_focus_addr(machine::Address _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void GoToSymbolDialog::memory_focus_addr(machine::Address _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
bool GoToSymbolDialog::obtain_value_for_name(uint64_t & _t1, const QString & _t2)const
{
    bool _t0{};
    void *_a[] = { const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t0))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(const_cast< GoToSymbolDialog *>(this), &staticMetaObject, 2, _a);
    return _t0;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
