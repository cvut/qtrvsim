/****************************************************************************
** Meta object code from reading C++ file 'predictor.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.13)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../../src/machine/predictor.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'predictor.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.13. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_machine__BranchHistoryRegister_t {
    QByteArrayData data[7];
    char stringdata0[95];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_machine__BranchHistoryRegister_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_machine__BranchHistoryRegister_t qt_meta_stringdata_machine__BranchHistoryRegister = {
    {
QT_MOC_LITERAL(0, 0, 30), // "machine::BranchHistoryRegister"
QT_MOC_LITERAL(1, 31, 11), // "bhr_updated"
QT_MOC_LITERAL(2, 43, 0), // ""
QT_MOC_LITERAL(3, 44, 7), // "uint8_t"
QT_MOC_LITERAL(4, 52, 18), // "number_of_bhr_bits"
QT_MOC_LITERAL(5, 71, 8), // "uint16_t"
QT_MOC_LITERAL(6, 80, 14) // "register_value"

    },
    "machine::BranchHistoryRegister\0"
    "bhr_updated\0\0uint8_t\0number_of_bhr_bits\0"
    "uint16_t\0register_value"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_machine__BranchHistoryRegister[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   19,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 5,    4,    6,

       0        // eod
};

void machine::BranchHistoryRegister::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<BranchHistoryRegister *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->bhr_updated((*reinterpret_cast< uint8_t(*)>(_a[1])),(*reinterpret_cast< uint16_t(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (BranchHistoryRegister::*)(uint8_t , uint16_t );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&BranchHistoryRegister::bhr_updated)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject machine::BranchHistoryRegister::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_machine__BranchHistoryRegister.data,
    qt_meta_data_machine__BranchHistoryRegister,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *machine::BranchHistoryRegister::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *machine::BranchHistoryRegister::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_machine__BranchHistoryRegister.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int machine::BranchHistoryRegister::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
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

// SIGNAL 0
void machine::BranchHistoryRegister::bhr_updated(uint8_t _t1, uint16_t _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
struct qt_meta_stringdata_machine__BranchTargetBuffer_t {
    QByteArrayData data[7];
    char stringdata0[94];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_machine__BranchTargetBuffer_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_machine__BranchTargetBuffer_t qt_meta_stringdata_machine__BranchTargetBuffer = {
    {
QT_MOC_LITERAL(0, 0, 27), // "machine::BranchTargetBuffer"
QT_MOC_LITERAL(1, 28, 15), // "btb_row_updated"
QT_MOC_LITERAL(2, 44, 0), // ""
QT_MOC_LITERAL(3, 45, 8), // "uint16_t"
QT_MOC_LITERAL(4, 54, 5), // "index"
QT_MOC_LITERAL(5, 60, 23), // "BranchTargetBufferEntry"
QT_MOC_LITERAL(6, 84, 9) // "btb_entry"

    },
    "machine::BranchTargetBuffer\0btb_row_updated\0"
    "\0uint16_t\0index\0BranchTargetBufferEntry\0"
    "btb_entry"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_machine__BranchTargetBuffer[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   19,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 5,    4,    6,

       0        // eod
};

void machine::BranchTargetBuffer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<BranchTargetBuffer *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->btb_row_updated((*reinterpret_cast< uint16_t(*)>(_a[1])),(*reinterpret_cast< BranchTargetBufferEntry(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (BranchTargetBuffer::*)(uint16_t , BranchTargetBufferEntry ) const;
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&BranchTargetBuffer::btb_row_updated)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject machine::BranchTargetBuffer::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_machine__BranchTargetBuffer.data,
    qt_meta_data_machine__BranchTargetBuffer,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *machine::BranchTargetBuffer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *machine::BranchTargetBuffer::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_machine__BranchTargetBuffer.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int machine::BranchTargetBuffer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
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

// SIGNAL 0
void machine::BranchTargetBuffer::btb_row_updated(uint16_t _t1, BranchTargetBufferEntry _t2)const
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(const_cast< machine::BranchTargetBuffer *>(this), &staticMetaObject, 0, _a);
}
struct qt_meta_stringdata_machine__Predictor_t {
    QByteArrayData data[10];
    char stringdata0[122];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_machine__Predictor_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_machine__Predictor_t qt_meta_stringdata_machine__Predictor = {
    {
QT_MOC_LITERAL(0, 0, 18), // "machine::Predictor"
QT_MOC_LITERAL(1, 19, 13), // "stats_updated"
QT_MOC_LITERAL(2, 33, 0), // ""
QT_MOC_LITERAL(3, 34, 20), // "PredictionStatistics"
QT_MOC_LITERAL(4, 55, 5), // "stats"
QT_MOC_LITERAL(5, 61, 15), // "bht_row_updated"
QT_MOC_LITERAL(6, 77, 8), // "uint16_t"
QT_MOC_LITERAL(7, 86, 5), // "index"
QT_MOC_LITERAL(8, 92, 23), // "BranchHistoryTableEntry"
QT_MOC_LITERAL(9, 116, 5) // "entry"

    },
    "machine::Predictor\0stats_updated\0\0"
    "PredictionStatistics\0stats\0bht_row_updated\0"
    "uint16_t\0index\0BranchHistoryTableEntry\0"
    "entry"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_machine__Predictor[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   24,    2, 0x06 /* Public */,
       5,    2,   27,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 6, 0x80000000 | 8,    7,    9,

       0        // eod
};

void machine::Predictor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Predictor *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->stats_updated((*reinterpret_cast< PredictionStatistics(*)>(_a[1]))); break;
        case 1: _t->bht_row_updated((*reinterpret_cast< uint16_t(*)>(_a[1])),(*reinterpret_cast< BranchHistoryTableEntry(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (Predictor::*)(PredictionStatistics ) const;
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Predictor::stats_updated)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (Predictor::*)(uint16_t , BranchHistoryTableEntry ) const;
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Predictor::bht_row_updated)) {
                *result = 1;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject machine::Predictor::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_machine__Predictor.data,
    qt_meta_data_machine__Predictor,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *machine::Predictor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *machine::Predictor::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_machine__Predictor.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int machine::Predictor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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

// SIGNAL 0
void machine::Predictor::stats_updated(PredictionStatistics _t1)const
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(const_cast< machine::Predictor *>(this), &staticMetaObject, 0, _a);
}

// SIGNAL 1
void machine::Predictor::bht_row_updated(uint16_t _t1, BranchHistoryTableEntry _t2)const
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(const_cast< machine::Predictor *>(this), &staticMetaObject, 1, _a);
}
struct qt_meta_stringdata_machine__BranchPredictor_t {
    QByteArrayData data[33];
    char stringdata0[441];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_machine__BranchPredictor_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_machine__BranchPredictor_t qt_meta_stringdata_machine__BranchPredictor = {
    {
QT_MOC_LITERAL(0, 0, 24), // "machine::BranchPredictor"
QT_MOC_LITERAL(1, 25, 19), // "total_stats_updated"
QT_MOC_LITERAL(2, 45, 0), // ""
QT_MOC_LITERAL(3, 46, 20), // "PredictionStatistics"
QT_MOC_LITERAL(4, 67, 11), // "total_stats"
QT_MOC_LITERAL(5, 79, 15), // "prediction_done"
QT_MOC_LITERAL(6, 95, 8), // "uint16_t"
QT_MOC_LITERAL(7, 104, 9), // "btb_index"
QT_MOC_LITERAL(8, 114, 9), // "bht_index"
QT_MOC_LITERAL(9, 124, 15), // "PredictionInput"
QT_MOC_LITERAL(10, 140, 5), // "input"
QT_MOC_LITERAL(11, 146, 12), // "BranchResult"
QT_MOC_LITERAL(12, 159, 6), // "result"
QT_MOC_LITERAL(13, 166, 10), // "BranchType"
QT_MOC_LITERAL(14, 177, 11), // "branch_type"
QT_MOC_LITERAL(15, 189, 11), // "update_done"
QT_MOC_LITERAL(16, 201, 18), // "PredictionFeedback"
QT_MOC_LITERAL(17, 220, 8), // "feedback"
QT_MOC_LITERAL(18, 229, 23), // "predictor_stats_updated"
QT_MOC_LITERAL(19, 253, 5), // "stats"
QT_MOC_LITERAL(20, 259, 25), // "predictor_bht_row_updated"
QT_MOC_LITERAL(21, 285, 5), // "index"
QT_MOC_LITERAL(22, 291, 23), // "BranchHistoryTableEntry"
QT_MOC_LITERAL(23, 315, 5), // "entry"
QT_MOC_LITERAL(24, 321, 11), // "bhr_updated"
QT_MOC_LITERAL(25, 333, 7), // "uint8_t"
QT_MOC_LITERAL(26, 341, 18), // "number_of_bhr_bits"
QT_MOC_LITERAL(27, 360, 14), // "register_value"
QT_MOC_LITERAL(28, 375, 15), // "btb_row_updated"
QT_MOC_LITERAL(29, 391, 23), // "BranchTargetBufferEntry"
QT_MOC_LITERAL(30, 415, 9), // "btb_entry"
QT_MOC_LITERAL(31, 425, 7), // "cleared"
QT_MOC_LITERAL(32, 433, 7) // "flushed"

    },
    "machine::BranchPredictor\0total_stats_updated\0"
    "\0PredictionStatistics\0total_stats\0"
    "prediction_done\0uint16_t\0btb_index\0"
    "bht_index\0PredictionInput\0input\0"
    "BranchResult\0result\0BranchType\0"
    "branch_type\0update_done\0PredictionFeedback\0"
    "feedback\0predictor_stats_updated\0stats\0"
    "predictor_bht_row_updated\0index\0"
    "BranchHistoryTableEntry\0entry\0bhr_updated\0"
    "uint8_t\0number_of_bhr_bits\0register_value\0"
    "btb_row_updated\0BranchTargetBufferEntry\0"
    "btb_entry\0cleared\0flushed"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_machine__BranchPredictor[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       9,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   59,    2, 0x06 /* Public */,
       5,    5,   62,    2, 0x06 /* Public */,
      15,    3,   73,    2, 0x06 /* Public */,
      18,    1,   80,    2, 0x06 /* Public */,
      20,    2,   83,    2, 0x06 /* Public */,
      24,    2,   88,    2, 0x06 /* Public */,
      28,    2,   93,    2, 0x06 /* Public */,
      31,    0,   98,    2, 0x06 /* Public */,
      32,    0,   99,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 6, 0x80000000 | 6, 0x80000000 | 9, 0x80000000 | 11, 0x80000000 | 13,    7,    8,   10,   12,   14,
    QMetaType::Void, 0x80000000 | 6, 0x80000000 | 6, 0x80000000 | 16,    7,    8,   17,
    QMetaType::Void, 0x80000000 | 3,   19,
    QMetaType::Void, 0x80000000 | 6, 0x80000000 | 22,   21,   23,
    QMetaType::Void, 0x80000000 | 25, 0x80000000 | 6,   26,   27,
    QMetaType::Void, 0x80000000 | 6, 0x80000000 | 29,   21,   30,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void machine::BranchPredictor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<BranchPredictor *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->total_stats_updated((*reinterpret_cast< PredictionStatistics(*)>(_a[1]))); break;
        case 1: _t->prediction_done((*reinterpret_cast< uint16_t(*)>(_a[1])),(*reinterpret_cast< uint16_t(*)>(_a[2])),(*reinterpret_cast< PredictionInput(*)>(_a[3])),(*reinterpret_cast< BranchResult(*)>(_a[4])),(*reinterpret_cast< BranchType(*)>(_a[5]))); break;
        case 2: _t->update_done((*reinterpret_cast< uint16_t(*)>(_a[1])),(*reinterpret_cast< uint16_t(*)>(_a[2])),(*reinterpret_cast< PredictionFeedback(*)>(_a[3]))); break;
        case 3: _t->predictor_stats_updated((*reinterpret_cast< PredictionStatistics(*)>(_a[1]))); break;
        case 4: _t->predictor_bht_row_updated((*reinterpret_cast< uint16_t(*)>(_a[1])),(*reinterpret_cast< BranchHistoryTableEntry(*)>(_a[2]))); break;
        case 5: _t->bhr_updated((*reinterpret_cast< uint8_t(*)>(_a[1])),(*reinterpret_cast< uint16_t(*)>(_a[2]))); break;
        case 6: _t->btb_row_updated((*reinterpret_cast< uint16_t(*)>(_a[1])),(*reinterpret_cast< BranchTargetBufferEntry(*)>(_a[2]))); break;
        case 7: _t->cleared(); break;
        case 8: _t->flushed(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (BranchPredictor::*)(PredictionStatistics );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&BranchPredictor::total_stats_updated)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (BranchPredictor::*)(uint16_t , uint16_t , PredictionInput , BranchResult , BranchType ) const;
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&BranchPredictor::prediction_done)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (BranchPredictor::*)(uint16_t , uint16_t , PredictionFeedback ) const;
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&BranchPredictor::update_done)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (BranchPredictor::*)(PredictionStatistics ) const;
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&BranchPredictor::predictor_stats_updated)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (BranchPredictor::*)(uint16_t , BranchHistoryTableEntry ) const;
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&BranchPredictor::predictor_bht_row_updated)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (BranchPredictor::*)(uint8_t , uint16_t ) const;
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&BranchPredictor::bhr_updated)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (BranchPredictor::*)(uint16_t , BranchTargetBufferEntry ) const;
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&BranchPredictor::btb_row_updated)) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (BranchPredictor::*)() const;
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&BranchPredictor::cleared)) {
                *result = 7;
                return;
            }
        }
        {
            using _t = void (BranchPredictor::*)() const;
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&BranchPredictor::flushed)) {
                *result = 8;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject machine::BranchPredictor::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_machine__BranchPredictor.data,
    qt_meta_data_machine__BranchPredictor,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *machine::BranchPredictor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *machine::BranchPredictor::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_machine__BranchPredictor.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int machine::BranchPredictor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void machine::BranchPredictor::total_stats_updated(PredictionStatistics _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void machine::BranchPredictor::prediction_done(uint16_t _t1, uint16_t _t2, PredictionInput _t3, BranchResult _t4, BranchType _t5)const
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t4))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t5))) };
    QMetaObject::activate(const_cast< machine::BranchPredictor *>(this), &staticMetaObject, 1, _a);
}

// SIGNAL 2
void machine::BranchPredictor::update_done(uint16_t _t1, uint16_t _t2, PredictionFeedback _t3)const
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(const_cast< machine::BranchPredictor *>(this), &staticMetaObject, 2, _a);
}

// SIGNAL 3
void machine::BranchPredictor::predictor_stats_updated(PredictionStatistics _t1)const
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(const_cast< machine::BranchPredictor *>(this), &staticMetaObject, 3, _a);
}

// SIGNAL 4
void machine::BranchPredictor::predictor_bht_row_updated(uint16_t _t1, BranchHistoryTableEntry _t2)const
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(const_cast< machine::BranchPredictor *>(this), &staticMetaObject, 4, _a);
}

// SIGNAL 5
void machine::BranchPredictor::bhr_updated(uint8_t _t1, uint16_t _t2)const
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(const_cast< machine::BranchPredictor *>(this), &staticMetaObject, 5, _a);
}

// SIGNAL 6
void machine::BranchPredictor::btb_row_updated(uint16_t _t1, BranchTargetBufferEntry _t2)const
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(const_cast< machine::BranchPredictor *>(this), &staticMetaObject, 6, _a);
}

// SIGNAL 7
void machine::BranchPredictor::cleared()const
{
    QMetaObject::activate(const_cast< machine::BranchPredictor *>(this), &staticMetaObject, 7, nullptr);
}

// SIGNAL 8
void machine::BranchPredictor::flushed()const
{
    QMetaObject::activate(const_cast< machine::BranchPredictor *>(this), &staticMetaObject, 8, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
