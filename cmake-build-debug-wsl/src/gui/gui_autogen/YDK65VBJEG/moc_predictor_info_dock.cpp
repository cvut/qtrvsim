/****************************************************************************
** Meta object code from reading C++ file 'predictor_info_dock.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.13)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../../src/gui/windows/predictor/predictor_info_dock.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'predictor_info_dock.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.13. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_DockPredictorInfo_t {
    QByteArrayData data[28];
    char stringdata0[397];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_DockPredictorInfo_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_DockPredictorInfo_t qt_meta_stringdata_DockPredictorInfo = {
    {
QT_MOC_LITERAL(0, 0, 17), // "DockPredictorInfo"
QT_MOC_LITERAL(1, 18, 10), // "update_bhr"
QT_MOC_LITERAL(2, 29, 0), // ""
QT_MOC_LITERAL(3, 30, 7), // "uint8_t"
QT_MOC_LITERAL(4, 38, 18), // "number_of_bhr_bits"
QT_MOC_LITERAL(5, 57, 8), // "uint16_t"
QT_MOC_LITERAL(6, 66, 14), // "register_value"
QT_MOC_LITERAL(7, 81, 19), // "show_new_prediction"
QT_MOC_LITERAL(8, 101, 9), // "btb_index"
QT_MOC_LITERAL(9, 111, 9), // "bht_index"
QT_MOC_LITERAL(10, 121, 24), // "machine::PredictionInput"
QT_MOC_LITERAL(11, 146, 5), // "input"
QT_MOC_LITERAL(12, 152, 21), // "machine::BranchResult"
QT_MOC_LITERAL(13, 174, 6), // "result"
QT_MOC_LITERAL(14, 181, 19), // "machine::BranchType"
QT_MOC_LITERAL(15, 201, 11), // "branch_type"
QT_MOC_LITERAL(16, 213, 15), // "show_new_update"
QT_MOC_LITERAL(17, 229, 27), // "machine::PredictionFeedback"
QT_MOC_LITERAL(18, 257, 8), // "feedback"
QT_MOC_LITERAL(19, 266, 12), // "update_stats"
QT_MOC_LITERAL(20, 279, 29), // "machine::PredictionStatistics"
QT_MOC_LITERAL(21, 309, 5), // "stats"
QT_MOC_LITERAL(22, 315, 12), // "reset_colors"
QT_MOC_LITERAL(23, 328, 11), // "clear_stats"
QT_MOC_LITERAL(24, 340, 9), // "clear_bhr"
QT_MOC_LITERAL(25, 350, 20), // "clear_predict_widget"
QT_MOC_LITERAL(26, 371, 19), // "clear_update_widget"
QT_MOC_LITERAL(27, 391, 5) // "clear"

    },
    "DockPredictorInfo\0update_bhr\0\0uint8_t\0"
    "number_of_bhr_bits\0uint16_t\0register_value\0"
    "show_new_prediction\0btb_index\0bht_index\0"
    "machine::PredictionInput\0input\0"
    "machine::BranchResult\0result\0"
    "machine::BranchType\0branch_type\0"
    "show_new_update\0machine::PredictionFeedback\0"
    "feedback\0update_stats\0"
    "machine::PredictionStatistics\0stats\0"
    "reset_colors\0clear_stats\0clear_bhr\0"
    "clear_predict_widget\0clear_update_widget\0"
    "clear"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_DockPredictorInfo[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    2,   64,    2, 0x0a /* Public */,
       7,    5,   69,    2, 0x0a /* Public */,
      16,    3,   80,    2, 0x0a /* Public */,
      19,    1,   87,    2, 0x0a /* Public */,
      22,    0,   90,    2, 0x0a /* Public */,
      23,    0,   91,    2, 0x0a /* Public */,
      24,    0,   92,    2, 0x0a /* Public */,
      25,    0,   93,    2, 0x0a /* Public */,
      26,    0,   94,    2, 0x0a /* Public */,
      27,    0,   95,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 5,    4,    6,
    QMetaType::Void, 0x80000000 | 5, 0x80000000 | 5, 0x80000000 | 10, 0x80000000 | 12, 0x80000000 | 14,    8,    9,   11,   13,   15,
    QMetaType::Void, 0x80000000 | 5, 0x80000000 | 5, 0x80000000 | 17,    8,    9,   18,
    QMetaType::Void, 0x80000000 | 20,   21,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void DockPredictorInfo::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<DockPredictorInfo *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->update_bhr((*reinterpret_cast< uint8_t(*)>(_a[1])),(*reinterpret_cast< uint16_t(*)>(_a[2]))); break;
        case 1: _t->show_new_prediction((*reinterpret_cast< uint16_t(*)>(_a[1])),(*reinterpret_cast< uint16_t(*)>(_a[2])),(*reinterpret_cast< machine::PredictionInput(*)>(_a[3])),(*reinterpret_cast< machine::BranchResult(*)>(_a[4])),(*reinterpret_cast< machine::BranchType(*)>(_a[5]))); break;
        case 2: _t->show_new_update((*reinterpret_cast< uint16_t(*)>(_a[1])),(*reinterpret_cast< uint16_t(*)>(_a[2])),(*reinterpret_cast< machine::PredictionFeedback(*)>(_a[3]))); break;
        case 3: _t->update_stats((*reinterpret_cast< machine::PredictionStatistics(*)>(_a[1]))); break;
        case 4: _t->reset_colors(); break;
        case 5: _t->clear_stats(); break;
        case 6: _t->clear_bhr(); break;
        case 7: _t->clear_predict_widget(); break;
        case 8: _t->clear_update_widget(); break;
        case 9: _t->clear(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject DockPredictorInfo::staticMetaObject = { {
    QMetaObject::SuperData::link<QDockWidget::staticMetaObject>(),
    qt_meta_stringdata_DockPredictorInfo.data,
    qt_meta_data_DockPredictorInfo,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *DockPredictorInfo::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DockPredictorInfo::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_DockPredictorInfo.stringdata0))
        return static_cast<void*>(this);
    return QDockWidget::qt_metacast(_clname);
}

int DockPredictorInfo::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDockWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 10;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
