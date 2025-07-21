/****************************************************************************
** Meta object code from reading C++ file 'extprocess.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.13)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../../src/gui/extprocess.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'extprocess.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.13. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ExtProcess_t {
    QByteArrayData data[16];
    char stringdata0[164];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ExtProcess_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ExtProcess_t qt_meta_stringdata_ExtProcess = {
    {
QT_MOC_LITERAL(0, 0, 10), // "ExtProcess"
QT_MOC_LITERAL(1, 11, 14), // "report_message"
QT_MOC_LITERAL(2, 26, 0), // ""
QT_MOC_LITERAL(3, 27, 17), // "messagetype::Type"
QT_MOC_LITERAL(4, 45, 4), // "type"
QT_MOC_LITERAL(5, 50, 4), // "file"
QT_MOC_LITERAL(6, 55, 4), // "line"
QT_MOC_LITERAL(7, 60, 6), // "column"
QT_MOC_LITERAL(8, 67, 4), // "text"
QT_MOC_LITERAL(9, 72, 4), // "hint"
QT_MOC_LITERAL(10, 77, 14), // "process_output"
QT_MOC_LITERAL(11, 92, 14), // "report_started"
QT_MOC_LITERAL(12, 107, 15), // "report_finished"
QT_MOC_LITERAL(13, 123, 8), // "exitCode"
QT_MOC_LITERAL(14, 132, 20), // "QProcess::ExitStatus"
QT_MOC_LITERAL(15, 153, 10) // "exitStatus"

    },
    "ExtProcess\0report_message\0\0messagetype::Type\0"
    "type\0file\0line\0column\0text\0hint\0"
    "process_output\0report_started\0"
    "report_finished\0exitCode\0QProcess::ExitStatus\0"
    "exitStatus"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ExtProcess[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    6,   34,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      10,    0,   47,    2, 0x09 /* Protected */,
      11,    0,   48,    2, 0x09 /* Protected */,
      12,    2,   49,    2, 0x09 /* Protected */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3, QMetaType::QString, QMetaType::Int, QMetaType::Int, QMetaType::QString, QMetaType::QString,    4,    5,    6,    7,    8,    9,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 14,   13,   15,

       0        // eod
};

void ExtProcess::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ExtProcess *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->report_message((*reinterpret_cast< messagetype::Type(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4])),(*reinterpret_cast< QString(*)>(_a[5])),(*reinterpret_cast< QString(*)>(_a[6]))); break;
        case 1: _t->process_output(); break;
        case 2: _t->report_started(); break;
        case 3: _t->report_finished((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QProcess::ExitStatus(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ExtProcess::*)(messagetype::Type , QString , int , int , QString , QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ExtProcess::report_message)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject ExtProcess::staticMetaObject = { {
    QMetaObject::SuperData::link<QProcess::staticMetaObject>(),
    qt_meta_stringdata_ExtProcess.data,
    qt_meta_data_ExtProcess,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *ExtProcess::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ExtProcess::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ExtProcess.stringdata0))
        return static_cast<void*>(this);
    return QProcess::qt_metacast(_clname);
}

int ExtProcess::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QProcess::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void ExtProcess::report_message(messagetype::Type _t1, QString _t2, int _t3, int _t4, QString _t5, QString _t6)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t4))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t5))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t6))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
