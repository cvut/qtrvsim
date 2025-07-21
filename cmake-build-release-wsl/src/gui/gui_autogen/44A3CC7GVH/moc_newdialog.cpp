/****************************************************************************
** Meta object code from reading C++ file 'newdialog.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.13)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../../src/gui/dialogs/new/newdialog.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'newdialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.13. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_NewDialog_t {
    QByteArrayData data[48];
    char stringdata0[854];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_NewDialog_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_NewDialog_t qt_meta_stringdata_NewDialog = {
    {
QT_MOC_LITERAL(0, 0, 9), // "NewDialog"
QT_MOC_LITERAL(1, 10, 6), // "cancel"
QT_MOC_LITERAL(2, 17, 0), // ""
QT_MOC_LITERAL(3, 18, 6), // "create"
QT_MOC_LITERAL(4, 25, 12), // "create_empty"
QT_MOC_LITERAL(5, 38, 14), // "create_example"
QT_MOC_LITERAL(6, 53, 10), // "browse_elf"
QT_MOC_LITERAL(7, 64, 10), // "elf_change"
QT_MOC_LITERAL(8, 75, 3), // "val"
QT_MOC_LITERAL(9, 79, 10), // "set_preset"
QT_MOC_LITERAL(10, 90, 17), // "xlen_64bit_change"
QT_MOC_LITERAL(11, 108, 17), // "isa_atomic_change"
QT_MOC_LITERAL(12, 126, 19), // "isa_multiply_change"
QT_MOC_LITERAL(13, 146, 16), // "pipelined_change"
QT_MOC_LITERAL(14, 163, 17), // "delay_slot_change"
QT_MOC_LITERAL(15, 181, 18), // "hazard_unit_change"
QT_MOC_LITERAL(16, 200, 22), // "mem_protec_exec_change"
QT_MOC_LITERAL(17, 223, 23), // "mem_protec_write_change"
QT_MOC_LITERAL(18, 247, 20), // "mem_time_read_change"
QT_MOC_LITERAL(19, 268, 21), // "mem_time_write_change"
QT_MOC_LITERAL(20, 290, 23), // "mem_enable_burst_change"
QT_MOC_LITERAL(21, 314, 21), // "mem_time_burst_change"
QT_MOC_LITERAL(22, 336, 22), // "mem_time_level2_change"
QT_MOC_LITERAL(23, 359, 19), // "osemu_enable_change"
QT_MOC_LITERAL(24, 379, 31), // "osemu_known_syscall_stop_change"
QT_MOC_LITERAL(25, 411, 33), // "osemu_unknown_syscall_stop_ch..."
QT_MOC_LITERAL(26, 445, 27), // "osemu_interrupt_stop_change"
QT_MOC_LITERAL(27, 473, 27), // "osemu_exception_stop_change"
QT_MOC_LITERAL(28, 501, 20), // "browse_osemu_fs_root"
QT_MOC_LITERAL(29, 522, 20), // "osemu_fs_root_change"
QT_MOC_LITERAL(30, 543, 23), // "reset_at_compile_change"
QT_MOC_LITERAL(31, 567, 11), // "switch2page"
QT_MOC_LITERAL(32, 579, 16), // "QTreeWidgetItem*"
QT_MOC_LITERAL(33, 596, 7), // "current"
QT_MOC_LITERAL(34, 604, 8), // "previous"
QT_MOC_LITERAL(35, 613, 17), // "bp_toggle_widgets"
QT_MOC_LITERAL(36, 631, 17), // "bp_enabled_change"
QT_MOC_LITERAL(37, 649, 14), // "bp_type_change"
QT_MOC_LITERAL(38, 664, 20), // "bp_init_state_change"
QT_MOC_LITERAL(39, 685, 23), // "bp_btb_addr_bits_change"
QT_MOC_LITERAL(40, 709, 22), // "bp_bht_bhr_bits_change"
QT_MOC_LITERAL(41, 732, 23), // "bp_bht_addr_bits_change"
QT_MOC_LITERAL(42, 756, 15), // "vm_mode_changed"
QT_MOC_LITERAL(43, 772, 17), // "vm_enabled_change"
QT_MOC_LITERAL(44, 790, 24), // "kernel_virt_base_changed"
QT_MOC_LITERAL(45, 815, 8), // "new_base"
QT_MOC_LITERAL(46, 824, 16), // "root_ppn_changed"
QT_MOC_LITERAL(47, 841, 12) // "new_root_ppn"

    },
    "NewDialog\0cancel\0\0create\0create_empty\0"
    "create_example\0browse_elf\0elf_change\0"
    "val\0set_preset\0xlen_64bit_change\0"
    "isa_atomic_change\0isa_multiply_change\0"
    "pipelined_change\0delay_slot_change\0"
    "hazard_unit_change\0mem_protec_exec_change\0"
    "mem_protec_write_change\0mem_time_read_change\0"
    "mem_time_write_change\0mem_enable_burst_change\0"
    "mem_time_burst_change\0mem_time_level2_change\0"
    "osemu_enable_change\0osemu_known_syscall_stop_change\0"
    "osemu_unknown_syscall_stop_change\0"
    "osemu_interrupt_stop_change\0"
    "osemu_exception_stop_change\0"
    "browse_osemu_fs_root\0osemu_fs_root_change\0"
    "reset_at_compile_change\0switch2page\0"
    "QTreeWidgetItem*\0current\0previous\0"
    "bp_toggle_widgets\0bp_enabled_change\0"
    "bp_type_change\0bp_init_state_change\0"
    "bp_btb_addr_bits_change\0bp_bht_bhr_bits_change\0"
    "bp_bht_addr_bits_change\0vm_mode_changed\0"
    "vm_enabled_change\0kernel_virt_base_changed\0"
    "new_base\0root_ppn_changed\0new_root_ppn"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_NewDialog[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      41,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,  219,    2, 0x08 /* Private */,
       3,    0,  220,    2, 0x08 /* Private */,
       4,    0,  221,    2, 0x08 /* Private */,
       5,    0,  222,    2, 0x08 /* Private */,
       6,    0,  223,    2, 0x08 /* Private */,
       7,    1,  224,    2, 0x08 /* Private */,
       9,    0,  227,    2, 0x08 /* Private */,
      10,    1,  228,    2, 0x08 /* Private */,
      11,    1,  231,    2, 0x08 /* Private */,
      12,    1,  234,    2, 0x08 /* Private */,
      13,    1,  237,    2, 0x08 /* Private */,
      14,    1,  240,    2, 0x08 /* Private */,
      15,    0,  243,    2, 0x08 /* Private */,
      16,    1,  244,    2, 0x08 /* Private */,
      17,    1,  247,    2, 0x08 /* Private */,
      18,    1,  250,    2, 0x08 /* Private */,
      19,    1,  253,    2, 0x08 /* Private */,
      20,    1,  256,    2, 0x08 /* Private */,
      21,    1,  259,    2, 0x08 /* Private */,
      22,    1,  262,    2, 0x08 /* Private */,
      23,    1,  265,    2, 0x08 /* Private */,
      24,    1,  268,    2, 0x08 /* Private */,
      25,    1,  271,    2, 0x08 /* Private */,
      26,    1,  274,    2, 0x08 /* Private */,
      27,    1,  277,    2, 0x08 /* Private */,
      28,    0,  280,    2, 0x08 /* Private */,
      29,    1,  281,    2, 0x08 /* Private */,
      30,    1,  284,    2, 0x08 /* Private */,
      31,    2,  287,    2, 0x08 /* Private */,
      31,    1,  292,    2, 0x28 /* Private | MethodCloned */,
      35,    0,  295,    2, 0x08 /* Private */,
      36,    1,  296,    2, 0x08 /* Private */,
      37,    0,  299,    2, 0x08 /* Private */,
      38,    0,  300,    2, 0x08 /* Private */,
      39,    1,  301,    2, 0x08 /* Private */,
      40,    1,  304,    2, 0x08 /* Private */,
      41,    1,  307,    2, 0x08 /* Private */,
      42,    0,  310,    2, 0x08 /* Private */,
      43,    1,  311,    2, 0x08 /* Private */,
      44,    1,  314,    2, 0x08 /* Private */,
      46,    1,  317,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    8,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    8,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, 0x80000000 | 32, 0x80000000 | 32,   33,   34,
    QMetaType::Void, 0x80000000 | 32,   33,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::LongLong,   45,
    QMetaType::Void, QMetaType::LongLong,   47,

       0        // eod
};

void NewDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<NewDialog *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->cancel(); break;
        case 1: _t->create(); break;
        case 2: _t->create_empty(); break;
        case 3: _t->create_example(); break;
        case 4: _t->browse_elf(); break;
        case 5: _t->elf_change((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 6: _t->set_preset(); break;
        case 7: _t->xlen_64bit_change((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 8: _t->isa_atomic_change((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 9: _t->isa_multiply_change((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 10: _t->pipelined_change((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 11: _t->delay_slot_change((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 12: _t->hazard_unit_change(); break;
        case 13: _t->mem_protec_exec_change((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 14: _t->mem_protec_write_change((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 15: _t->mem_time_read_change((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 16: _t->mem_time_write_change((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 17: _t->mem_enable_burst_change((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 18: _t->mem_time_burst_change((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 19: _t->mem_time_level2_change((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 20: _t->osemu_enable_change((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 21: _t->osemu_known_syscall_stop_change((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 22: _t->osemu_unknown_syscall_stop_change((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 23: _t->osemu_interrupt_stop_change((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 24: _t->osemu_exception_stop_change((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 25: _t->browse_osemu_fs_root(); break;
        case 26: _t->osemu_fs_root_change((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 27: _t->reset_at_compile_change((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 28: _t->switch2page((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< QTreeWidgetItem*(*)>(_a[2]))); break;
        case 29: _t->switch2page((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1]))); break;
        case 30: _t->bp_toggle_widgets(); break;
        case 31: _t->bp_enabled_change((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 32: _t->bp_type_change(); break;
        case 33: _t->bp_init_state_change(); break;
        case 34: _t->bp_btb_addr_bits_change((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 35: _t->bp_bht_bhr_bits_change((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 36: _t->bp_bht_addr_bits_change((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 37: _t->vm_mode_changed(); break;
        case 38: _t->vm_enabled_change((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 39: _t->kernel_virt_base_changed((*reinterpret_cast< qint64(*)>(_a[1]))); break;
        case 40: _t->root_ppn_changed((*reinterpret_cast< qint64(*)>(_a[1]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject NewDialog::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_meta_stringdata_NewDialog.data,
    qt_meta_data_NewDialog,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *NewDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *NewDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_NewDialog.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int NewDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 41)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 41;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 41)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 41;
    }
    return _id;
}
struct qt_meta_stringdata_NewDialogCacheHandler_t {
    QByteArrayData data[8];
    char stringdata0[91];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_NewDialogCacheHandler_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_NewDialogCacheHandler_t qt_meta_stringdata_NewDialogCacheHandler = {
    {
QT_MOC_LITERAL(0, 0, 21), // "NewDialogCacheHandler"
QT_MOC_LITERAL(1, 22, 7), // "enabled"
QT_MOC_LITERAL(2, 30, 0), // ""
QT_MOC_LITERAL(3, 31, 7), // "numsets"
QT_MOC_LITERAL(4, 39, 9), // "blocksize"
QT_MOC_LITERAL(5, 49, 19), // "degreeassociativity"
QT_MOC_LITERAL(6, 69, 11), // "replacement"
QT_MOC_LITERAL(7, 81, 9) // "writeback"

    },
    "NewDialogCacheHandler\0enabled\0\0numsets\0"
    "blocksize\0degreeassociativity\0replacement\0"
    "writeback"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_NewDialogCacheHandler[] = {

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
       1,    1,   44,    2, 0x08 /* Private */,
       3,    0,   47,    2, 0x08 /* Private */,
       4,    0,   48,    2, 0x08 /* Private */,
       5,    0,   49,    2, 0x08 /* Private */,
       6,    1,   50,    2, 0x08 /* Private */,
       7,    1,   53,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,

       0        // eod
};

void NewDialogCacheHandler::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<NewDialogCacheHandler *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->enabled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->numsets(); break;
        case 2: _t->blocksize(); break;
        case 3: _t->degreeassociativity(); break;
        case 4: _t->replacement((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->writeback((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject NewDialogCacheHandler::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_NewDialogCacheHandler.data,
    qt_meta_data_NewDialogCacheHandler,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *NewDialogCacheHandler::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *NewDialogCacheHandler::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_NewDialogCacheHandler.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int NewDialogCacheHandler::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
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
