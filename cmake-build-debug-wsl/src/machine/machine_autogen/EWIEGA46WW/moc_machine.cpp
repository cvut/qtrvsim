/****************************************************************************
** Meta object code from reading C++ file 'machine.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.13)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../../src/machine/machine.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'machine.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.13. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_machine__Machine_t {
    QByteArrayData data[22];
    char stringdata0[252];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_machine__Machine_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_machine__Machine_t qt_meta_stringdata_machine__Machine = {
    {
QT_MOC_LITERAL(0, 0, 16), // "machine::Machine"
QT_MOC_LITERAL(1, 17, 12), // "program_exit"
QT_MOC_LITERAL(2, 30, 0), // ""
QT_MOC_LITERAL(3, 31, 12), // "program_trap"
QT_MOC_LITERAL(4, 44, 28), // "machine::SimulatorException&"
QT_MOC_LITERAL(5, 73, 1), // "e"
QT_MOC_LITERAL(6, 75, 13), // "status_change"
QT_MOC_LITERAL(7, 89, 24), // "machine::Machine::Status"
QT_MOC_LITERAL(8, 114, 2), // "st"
QT_MOC_LITERAL(9, 117, 4), // "tick"
QT_MOC_LITERAL(10, 122, 9), // "post_tick"
QT_MOC_LITERAL(11, 132, 20), // "set_interrupt_signal"
QT_MOC_LITERAL(12, 153, 7), // "irq_num"
QT_MOC_LITERAL(13, 161, 6), // "active"
QT_MOC_LITERAL(14, 168, 14), // "play_initiated"
QT_MOC_LITERAL(15, 183, 11), // "play_paused"
QT_MOC_LITERAL(16, 195, 21), // "report_core_frequency"
QT_MOC_LITERAL(17, 217, 4), // "play"
QT_MOC_LITERAL(18, 222, 5), // "pause"
QT_MOC_LITERAL(19, 228, 4), // "step"
QT_MOC_LITERAL(20, 233, 7), // "restart"
QT_MOC_LITERAL(21, 241, 10) // "step_timer"

    },
    "machine::Machine\0program_exit\0\0"
    "program_trap\0machine::SimulatorException&\0"
    "e\0status_change\0machine::Machine::Status\0"
    "st\0tick\0post_tick\0set_interrupt_signal\0"
    "irq_num\0active\0play_initiated\0play_paused\0"
    "report_core_frequency\0play\0pause\0step\0"
    "restart\0step_timer"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_machine__Machine[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       9,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   84,    2, 0x06 /* Public */,
       3,    1,   85,    2, 0x06 /* Public */,
       6,    1,   88,    2, 0x06 /* Public */,
       9,    0,   91,    2, 0x06 /* Public */,
      10,    0,   92,    2, 0x06 /* Public */,
      11,    2,   93,    2, 0x06 /* Public */,
      14,    0,   98,    2, 0x06 /* Public */,
      15,    0,   99,    2, 0x06 /* Public */,
      16,    1,  100,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      17,    0,  103,    2, 0x0a /* Public */,
      18,    0,  104,    2, 0x0a /* Public */,
      19,    0,  105,    2, 0x0a /* Public */,
      20,    0,  106,    2, 0x0a /* Public */,
      21,    0,  107,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 4,    5,
    QMetaType::Void, 0x80000000 | 7,    8,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::UInt, QMetaType::Bool,   12,   13,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Double,    2,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void machine::Machine::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Machine *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->program_exit(); break;
        case 1: _t->program_trap((*reinterpret_cast< machine::SimulatorException(*)>(_a[1]))); break;
        case 2: _t->status_change((*reinterpret_cast< machine::Machine::Status(*)>(_a[1]))); break;
        case 3: _t->tick(); break;
        case 4: _t->post_tick(); break;
        case 5: _t->set_interrupt_signal((*reinterpret_cast< uint(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 6: _t->play_initiated(); break;
        case 7: _t->play_paused(); break;
        case 8: _t->report_core_frequency((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 9: _t->play(); break;
        case 10: _t->pause(); break;
        case 11: _t->step(); break;
        case 12: _t->restart(); break;
        case 13: _t->step_timer(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (Machine::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Machine::program_exit)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (Machine::*)(machine::SimulatorException & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Machine::program_trap)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (Machine::*)(machine::Machine::Status );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Machine::status_change)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (Machine::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Machine::tick)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (Machine::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Machine::post_tick)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (Machine::*)(uint , bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Machine::set_interrupt_signal)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (Machine::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Machine::play_initiated)) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (Machine::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Machine::play_paused)) {
                *result = 7;
                return;
            }
        }
        {
            using _t = void (Machine::*)(double );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Machine::report_core_frequency)) {
                *result = 8;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject machine::Machine::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_machine__Machine.data,
    qt_meta_data_machine__Machine,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *machine::Machine::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *machine::Machine::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_machine__Machine.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int machine::Machine::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 14)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 14)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 14;
    }
    return _id;
}

// SIGNAL 0
void machine::Machine::program_exit()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void machine::Machine::program_trap(machine::SimulatorException & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void machine::Machine::status_change(machine::Machine::Status _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void machine::Machine::tick()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void machine::Machine::post_tick()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void machine::Machine::set_interrupt_signal(uint _t1, bool _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void machine::Machine::play_initiated()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}

// SIGNAL 7
void machine::Machine::play_paused()
{
    QMetaObject::activate(this, &staticMetaObject, 7, nullptr);
}

// SIGNAL 8
void machine::Machine::report_core_frequency(double _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
