/****************************************************************************
** Meta object code from reading C++ file 'programdock.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.13)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../../src/gui/windows/program/programdock.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'programdock.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.13. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ProgramDock_t {
    QByteArrayData data[24];
    char stringdata0[328];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ProgramDock_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ProgramDock_t qt_meta_stringdata_ProgramDock = {
    {
QT_MOC_LITERAL(0, 0, 11), // "ProgramDock"
QT_MOC_LITERAL(1, 12, 13), // "machine_setup"
QT_MOC_LITERAL(2, 26, 0), // ""
QT_MOC_LITERAL(3, 27, 17), // "machine::Machine*"
QT_MOC_LITERAL(4, 45, 7), // "machine"
QT_MOC_LITERAL(5, 53, 10), // "jump_to_pc"
QT_MOC_LITERAL(6, 64, 16), // "machine::Address"
QT_MOC_LITERAL(7, 81, 10), // "focus_addr"
QT_MOC_LITERAL(8, 92, 20), // "focus_addr_with_save"
QT_MOC_LITERAL(9, 113, 18), // "stage_addr_changed"
QT_MOC_LITERAL(10, 132, 5), // "stage"
QT_MOC_LITERAL(11, 138, 4), // "addr"
QT_MOC_LITERAL(12, 143, 18), // "request_update_all"
QT_MOC_LITERAL(13, 162, 15), // "set_follow_inst"
QT_MOC_LITERAL(14, 178, 15), // "fetch_inst_addr"
QT_MOC_LITERAL(15, 194, 16), // "decode_inst_addr"
QT_MOC_LITERAL(16, 211, 17), // "execute_inst_addr"
QT_MOC_LITERAL(17, 229, 16), // "memory_inst_addr"
QT_MOC_LITERAL(18, 246, 19), // "writeback_inst_addr"
QT_MOC_LITERAL(19, 266, 12), // "report_error"
QT_MOC_LITERAL(20, 279, 5), // "error"
QT_MOC_LITERAL(21, 285, 21), // "update_pipeline_addrs"
QT_MOC_LITERAL(22, 307, 18), // "machine::CoreState"
QT_MOC_LITERAL(23, 326, 1) // "p"

    },
    "ProgramDock\0machine_setup\0\0machine::Machine*\0"
    "machine\0jump_to_pc\0machine::Address\0"
    "focus_addr\0focus_addr_with_save\0"
    "stage_addr_changed\0stage\0addr\0"
    "request_update_all\0set_follow_inst\0"
    "fetch_inst_addr\0decode_inst_addr\0"
    "execute_inst_addr\0memory_inst_addr\0"
    "writeback_inst_addr\0report_error\0error\0"
    "update_pipeline_addrs\0machine::CoreState\0"
    "p"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ProgramDock[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       6,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   84,    2, 0x06 /* Public */,
       5,    1,   87,    2, 0x06 /* Public */,
       7,    1,   90,    2, 0x06 /* Public */,
       8,    1,   93,    2, 0x06 /* Public */,
       9,    2,   96,    2, 0x06 /* Public */,
      12,    0,  101,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      13,    1,  102,    2, 0x0a /* Public */,
      14,    1,  105,    2, 0x0a /* Public */,
      15,    1,  108,    2, 0x0a /* Public */,
      16,    1,  111,    2, 0x0a /* Public */,
      17,    1,  114,    2, 0x0a /* Public */,
      18,    1,  117,    2, 0x0a /* Public */,
      19,    1,  120,    2, 0x0a /* Public */,
      21,    1,  123,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 6,    2,
    QMetaType::Void, 0x80000000 | 6,    2,
    QMetaType::Void, 0x80000000 | 6,    2,
    QMetaType::Void, QMetaType::UInt, 0x80000000 | 6,   10,   11,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, 0x80000000 | 6,   11,
    QMetaType::Void, 0x80000000 | 6,   11,
    QMetaType::Void, 0x80000000 | 6,   11,
    QMetaType::Void, 0x80000000 | 6,   11,
    QMetaType::Void, 0x80000000 | 6,   11,
    QMetaType::Void, QMetaType::QString,   20,
    QMetaType::Void, 0x80000000 | 22,   23,

       0        // eod
};

void ProgramDock::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ProgramDock *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->machine_setup((*reinterpret_cast< machine::Machine*(*)>(_a[1]))); break;
        case 1: _t->jump_to_pc((*reinterpret_cast< machine::Address(*)>(_a[1]))); break;
        case 2: _t->focus_addr((*reinterpret_cast< machine::Address(*)>(_a[1]))); break;
        case 3: _t->focus_addr_with_save((*reinterpret_cast< machine::Address(*)>(_a[1]))); break;
        case 4: _t->stage_addr_changed((*reinterpret_cast< uint(*)>(_a[1])),(*reinterpret_cast< machine::Address(*)>(_a[2]))); break;
        case 5: _t->request_update_all(); break;
        case 6: _t->set_follow_inst((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->fetch_inst_addr((*reinterpret_cast< machine::Address(*)>(_a[1]))); break;
        case 8: _t->decode_inst_addr((*reinterpret_cast< machine::Address(*)>(_a[1]))); break;
        case 9: _t->execute_inst_addr((*reinterpret_cast< machine::Address(*)>(_a[1]))); break;
        case 10: _t->memory_inst_addr((*reinterpret_cast< machine::Address(*)>(_a[1]))); break;
        case 11: _t->writeback_inst_addr((*reinterpret_cast< machine::Address(*)>(_a[1]))); break;
        case 12: _t->report_error((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 13: _t->update_pipeline_addrs((*reinterpret_cast< const machine::CoreState(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< machine::Machine* >(); break;
            }
            break;
        case 1:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< machine::Address >(); break;
            }
            break;
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
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< machine::Address >(); break;
            }
            break;
        case 7:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< machine::Address >(); break;
            }
            break;
        case 8:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< machine::Address >(); break;
            }
            break;
        case 9:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< machine::Address >(); break;
            }
            break;
        case 10:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< machine::Address >(); break;
            }
            break;
        case 11:
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
            using _t = void (ProgramDock::*)(machine::Machine * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ProgramDock::machine_setup)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ProgramDock::*)(machine::Address );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ProgramDock::jump_to_pc)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (ProgramDock::*)(machine::Address );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ProgramDock::focus_addr)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (ProgramDock::*)(machine::Address );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ProgramDock::focus_addr_with_save)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (ProgramDock::*)(uint , machine::Address );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ProgramDock::stage_addr_changed)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (ProgramDock::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ProgramDock::request_update_all)) {
                *result = 5;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject ProgramDock::staticMetaObject = { {
    QMetaObject::SuperData::link<QDockWidget::staticMetaObject>(),
    qt_meta_stringdata_ProgramDock.data,
    qt_meta_data_ProgramDock,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *ProgramDock::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ProgramDock::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ProgramDock.stringdata0))
        return static_cast<void*>(this);
    return QDockWidget::qt_metacast(_clname);
}

int ProgramDock::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDockWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 14)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 14)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    }
    return _id;
}

// SIGNAL 0
void ProgramDock::machine_setup(machine::Machine * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ProgramDock::jump_to_pc(machine::Address _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void ProgramDock::focus_addr(machine::Address _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void ProgramDock::focus_addr_with_save(machine::Address _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void ProgramDock::stage_addr_changed(uint _t1, machine::Address _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void ProgramDock::request_update_all()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
