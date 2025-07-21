/****************************************************************************
** Meta object code from reading C++ file 'predictor_btb_dock.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.13)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../../src/gui/windows/predictor/predictor_btb_dock.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'predictor_btb_dock.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.13. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_DockPredictorBTB_t {
    QByteArrayData data[13];
    char stringdata0[190];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_DockPredictorBTB_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_DockPredictorBTB_t qt_meta_stringdata_DockPredictorBTB = {
    {
QT_MOC_LITERAL(0, 0, 16), // "DockPredictorBTB"
QT_MOC_LITERAL(1, 17, 14), // "update_btb_row"
QT_MOC_LITERAL(2, 32, 0), // ""
QT_MOC_LITERAL(3, 33, 8), // "uint16_t"
QT_MOC_LITERAL(4, 42, 9), // "row_index"
QT_MOC_LITERAL(5, 52, 32), // "machine::BranchTargetBufferEntry"
QT_MOC_LITERAL(6, 85, 9), // "btb_entry"
QT_MOC_LITERAL(7, 95, 29), // "highligh_row_after_prediction"
QT_MOC_LITERAL(8, 125, 9), // "btb_index"
QT_MOC_LITERAL(9, 135, 25), // "highligh_row_after_update"
QT_MOC_LITERAL(10, 161, 12), // "reset_colors"
QT_MOC_LITERAL(11, 174, 9), // "clear_btb"
QT_MOC_LITERAL(12, 184, 5) // "clear"

    },
    "DockPredictorBTB\0update_btb_row\0\0"
    "uint16_t\0row_index\0machine::BranchTargetBufferEntry\0"
    "btb_entry\0highligh_row_after_prediction\0"
    "btb_index\0highligh_row_after_update\0"
    "reset_colors\0clear_btb\0clear"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_DockPredictorBTB[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    2,   44,    2, 0x0a /* Public */,
       7,    1,   49,    2, 0x0a /* Public */,
       9,    1,   52,    2, 0x0a /* Public */,
      10,    0,   55,    2, 0x0a /* Public */,
      11,    0,   56,    2, 0x0a /* Public */,
      12,    0,   57,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 5,    4,    6,
    QMetaType::Void, 0x80000000 | 3,    8,
    QMetaType::Void, 0x80000000 | 3,    8,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void DockPredictorBTB::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<DockPredictorBTB *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->update_btb_row((*reinterpret_cast< uint16_t(*)>(_a[1])),(*reinterpret_cast< machine::BranchTargetBufferEntry(*)>(_a[2]))); break;
        case 1: _t->highligh_row_after_prediction((*reinterpret_cast< uint16_t(*)>(_a[1]))); break;
        case 2: _t->highligh_row_after_update((*reinterpret_cast< uint16_t(*)>(_a[1]))); break;
        case 3: _t->reset_colors(); break;
        case 4: _t->clear_btb(); break;
        case 5: _t->clear(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject DockPredictorBTB::staticMetaObject = { {
    QMetaObject::SuperData::link<QDockWidget::staticMetaObject>(),
    qt_meta_stringdata_DockPredictorBTB.data,
    qt_meta_data_DockPredictorBTB,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *DockPredictorBTB::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DockPredictorBTB::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_DockPredictorBTB.stringdata0))
        return static_cast<void*>(this);
    return QDockWidget::qt_metacast(_clname);
}

int DockPredictorBTB::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDockWidget::qt_metacall(_c, _id, _a);
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
QT_WARNING_POP
QT_END_MOC_NAMESPACE
