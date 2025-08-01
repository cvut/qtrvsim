/****************************************************************************
** Meta object code from reading C++ file 'programmodel.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.13)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../../src/gui/windows/program/programmodel.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'programmodel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.13. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ProgramModel_t {
    QByteArrayData data[16];
    char stringdata0[174];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ProgramModel_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ProgramModel_t qt_meta_stringdata_ProgramModel = {
    {
QT_MOC_LITERAL(0, 0, 12), // "ProgramModel"
QT_MOC_LITERAL(1, 13, 12), // "report_error"
QT_MOC_LITERAL(2, 26, 0), // ""
QT_MOC_LITERAL(3, 27, 5), // "error"
QT_MOC_LITERAL(4, 33, 5), // "setup"
QT_MOC_LITERAL(5, 39, 17), // "machine::Machine*"
QT_MOC_LITERAL(6, 57, 7), // "machine"
QT_MOC_LITERAL(7, 65, 17), // "check_for_updates"
QT_MOC_LITERAL(8, 83, 15), // "toggle_hw_break"
QT_MOC_LITERAL(9, 99, 11), // "QModelIndex"
QT_MOC_LITERAL(10, 111, 5), // "index"
QT_MOC_LITERAL(11, 117, 17), // "update_stage_addr"
QT_MOC_LITERAL(12, 135, 5), // "stage"
QT_MOC_LITERAL(13, 141, 16), // "machine::Address"
QT_MOC_LITERAL(14, 158, 4), // "addr"
QT_MOC_LITERAL(15, 163, 10) // "update_all"

    },
    "ProgramModel\0report_error\0\0error\0setup\0"
    "machine::Machine*\0machine\0check_for_updates\0"
    "toggle_hw_break\0QModelIndex\0index\0"
    "update_stage_addr\0stage\0machine::Address\0"
    "addr\0update_all"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ProgramModel[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   44,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    1,   47,    2, 0x0a /* Public */,
       7,    0,   50,    2, 0x0a /* Public */,
       8,    1,   51,    2, 0x0a /* Public */,
      11,    2,   54,    2, 0x0a /* Public */,
      15,    0,   59,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 5,    6,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 9,   10,
    QMetaType::Void, QMetaType::UInt, 0x80000000 | 13,   12,   14,
    QMetaType::Void,

       0        // eod
};

void ProgramModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ProgramModel *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->report_error((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: _t->setup((*reinterpret_cast< machine::Machine*(*)>(_a[1]))); break;
        case 2: _t->check_for_updates(); break;
        case 3: _t->toggle_hw_break((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 4: _t->update_stage_addr((*reinterpret_cast< uint(*)>(_a[1])),(*reinterpret_cast< machine::Address(*)>(_a[2]))); break;
        case 5: _t->update_all(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 1:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< machine::Machine* >(); break;
            }
            break;
        case 4:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< machine::Address >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ProgramModel::*)(QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ProgramModel::report_error)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject ProgramModel::staticMetaObject = { {
    QMetaObject::SuperData::link<QAbstractTableModel::staticMetaObject>(),
    qt_meta_stringdata_ProgramModel.data,
    qt_meta_data_ProgramModel,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *ProgramModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ProgramModel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ProgramModel.stringdata0))
        return static_cast<void*>(this);
    return QAbstractTableModel::qt_metacast(_clname);
}

int ProgramModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void ProgramModel::report_error(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
