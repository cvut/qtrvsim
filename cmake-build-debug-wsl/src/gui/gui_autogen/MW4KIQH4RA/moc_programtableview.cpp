/****************************************************************************
** Meta object code from reading C++ file 'programtableview.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.13)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../../src/gui/windows/program/programtableview.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'programtableview.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.13. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ProgramTableView_t {
    QByteArrayData data[12];
    char stringdata0[194];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ProgramTableView_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ProgramTableView_t qt_meta_stringdata_ProgramTableView = {
    {
QT_MOC_LITERAL(0, 0, 16), // "ProgramTableView"
QT_MOC_LITERAL(1, 17, 15), // "address_changed"
QT_MOC_LITERAL(2, 33, 0), // ""
QT_MOC_LITERAL(3, 34, 8), // "uint32_t"
QT_MOC_LITERAL(4, 43, 7), // "address"
QT_MOC_LITERAL(5, 51, 23), // "adjust_scroll_pos_queue"
QT_MOC_LITERAL(6, 75, 13), // "go_to_address"
QT_MOC_LITERAL(7, 89, 16), // "machine::Address"
QT_MOC_LITERAL(8, 106, 13), // "focus_address"
QT_MOC_LITERAL(9, 120, 23), // "focus_address_with_save"
QT_MOC_LITERAL(10, 144, 23), // "adjust_scroll_pos_check"
QT_MOC_LITERAL(11, 168, 25) // "adjust_scroll_pos_process"

    },
    "ProgramTableView\0address_changed\0\0"
    "uint32_t\0address\0adjust_scroll_pos_queue\0"
    "go_to_address\0machine::Address\0"
    "focus_address\0focus_address_with_save\0"
    "adjust_scroll_pos_check\0"
    "adjust_scroll_pos_process"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ProgramTableView[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   49,    2, 0x06 /* Public */,
       5,    0,   52,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       6,    1,   53,    2, 0x0a /* Public */,
       8,    1,   56,    2, 0x0a /* Public */,
       9,    1,   59,    2, 0x0a /* Public */,
      10,    0,   62,    2, 0x08 /* Private */,
      11,    0,   63,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 7,    4,
    QMetaType::Void, 0x80000000 | 7,    4,
    QMetaType::Void, 0x80000000 | 7,    4,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void ProgramTableView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ProgramTableView *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->address_changed((*reinterpret_cast< uint32_t(*)>(_a[1]))); break;
        case 1: _t->adjust_scroll_pos_queue(); break;
        case 2: _t->go_to_address((*reinterpret_cast< machine::Address(*)>(_a[1]))); break;
        case 3: _t->focus_address((*reinterpret_cast< machine::Address(*)>(_a[1]))); break;
        case 4: _t->focus_address_with_save((*reinterpret_cast< machine::Address(*)>(_a[1]))); break;
        case 5: _t->adjust_scroll_pos_check(); break;
        case 6: _t->adjust_scroll_pos_process(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 2:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< machine::Address >(); break;
            }
            break;
        case 3:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< machine::Address >(); break;
            }
            break;
        case 4:
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
            using _t = void (ProgramTableView::*)(uint32_t );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ProgramTableView::address_changed)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ProgramTableView::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ProgramTableView::adjust_scroll_pos_queue)) {
                *result = 1;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject ProgramTableView::staticMetaObject = { {
    QMetaObject::SuperData::link<Poly_QTableView::staticMetaObject>(),
    qt_meta_stringdata_ProgramTableView.data,
    qt_meta_data_ProgramTableView,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *ProgramTableView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ProgramTableView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ProgramTableView.stringdata0))
        return static_cast<void*>(this);
    return Poly_QTableView::qt_metacast(_clname);
}

int ProgramTableView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Poly_QTableView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void ProgramTableView::address_changed(uint32_t _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ProgramTableView::adjust_scroll_pos_queue()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
