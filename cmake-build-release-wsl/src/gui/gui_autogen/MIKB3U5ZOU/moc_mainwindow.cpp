/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.13)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../../src/gui/mainwindow/mainwindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.13. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MainWindow_t {
    QByteArrayData data[75];
    char stringdata0[1109];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MainWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
QT_MOC_LITERAL(0, 0, 10), // "MainWindow"
QT_MOC_LITERAL(1, 11, 14), // "report_message"
QT_MOC_LITERAL(2, 26, 0), // ""
QT_MOC_LITERAL(3, 27, 17), // "messagetype::Type"
QT_MOC_LITERAL(4, 45, 4), // "type"
QT_MOC_LITERAL(5, 50, 4), // "file"
QT_MOC_LITERAL(6, 55, 4), // "line"
QT_MOC_LITERAL(7, 60, 6), // "column"
QT_MOC_LITERAL(8, 67, 4), // "text"
QT_MOC_LITERAL(9, 72, 4), // "hint"
QT_MOC_LITERAL(10, 77, 14), // "clear_messages"
QT_MOC_LITERAL(11, 92, 11), // "new_machine"
QT_MOC_LITERAL(12, 104, 14), // "machine_reload"
QT_MOC_LITERAL(13, 119, 18), // "force_memory_reset"
QT_MOC_LITERAL(14, 138, 14), // "force_elf_load"
QT_MOC_LITERAL(15, 153, 12), // "print_action"
QT_MOC_LITERAL(16, 166, 20), // "close_source_by_name"
QT_MOC_LITERAL(17, 187, 8), // "QString&"
QT_MOC_LITERAL(18, 196, 8), // "filename"
QT_MOC_LITERAL(19, 205, 3), // "ask"
QT_MOC_LITERAL(20, 209, 14), // "example_source"
QT_MOC_LITERAL(21, 224, 11), // "source_file"
QT_MOC_LITERAL(22, 236, 14), // "compile_source"
QT_MOC_LITERAL(23, 251, 13), // "build_execute"
QT_MOC_LITERAL(24, 265, 22), // "build_execute_no_check"
QT_MOC_LITERAL(25, 288, 23), // "build_execute_with_save"
QT_MOC_LITERAL(26, 312, 6), // "cancel"
QT_MOC_LITERAL(27, 319, 10), // "tosavelist"
QT_MOC_LITERAL(28, 330, 21), // "reset_state_registers"
QT_MOC_LITERAL(29, 352, 19), // "reset_state_program"
QT_MOC_LITERAL(30, 372, 18), // "reset_state_memory"
QT_MOC_LITERAL(31, 391, 25), // "reset_state_cache_program"
QT_MOC_LITERAL(32, 417, 22), // "reset_state_cache_data"
QT_MOC_LITERAL(33, 440, 24), // "reset_state_cache_level2"
QT_MOC_LITERAL(34, 465, 23), // "reset_state_peripherals"
QT_MOC_LITERAL(35, 489, 20), // "reset_state_terminal"
QT_MOC_LITERAL(36, 510, 23), // "reset_state_lcd_display"
QT_MOC_LITERAL(37, 534, 19), // "reset_state_csrdock"
QT_MOC_LITERAL(38, 554, 20), // "reset_state_messages"
QT_MOC_LITERAL(39, 575, 14), // "show_registers"
QT_MOC_LITERAL(40, 590, 12), // "show_program"
QT_MOC_LITERAL(41, 603, 11), // "show_memory"
QT_MOC_LITERAL(42, 615, 15), // "show_cache_data"
QT_MOC_LITERAL(43, 631, 18), // "show_cache_program"
QT_MOC_LITERAL(44, 650, 17), // "show_cache_level2"
QT_MOC_LITERAL(45, 668, 16), // "show_peripherals"
QT_MOC_LITERAL(46, 685, 13), // "show_terminal"
QT_MOC_LITERAL(47, 699, 16), // "show_lcd_display"
QT_MOC_LITERAL(48, 716, 12), // "show_csrdock"
QT_MOC_LITERAL(49, 729, 18), // "show_hide_coreview"
QT_MOC_LITERAL(50, 748, 4), // "show"
QT_MOC_LITERAL(51, 753, 13), // "show_messages"
QT_MOC_LITERAL(52, 767, 13), // "reset_windows"
QT_MOC_LITERAL(53, 781, 18), // "show_symbol_dialog"
QT_MOC_LITERAL(54, 800, 11), // "show_bp_btb"
QT_MOC_LITERAL(55, 812, 11), // "show_bp_bht"
QT_MOC_LITERAL(56, 824, 12), // "show_bp_info"
QT_MOC_LITERAL(57, 837, 18), // "reset_state_bp_btb"
QT_MOC_LITERAL(58, 856, 18), // "reset_state_bp_bht"
QT_MOC_LITERAL(59, 875, 19), // "reset_state_bp_info"
QT_MOC_LITERAL(60, 895, 13), // "about_program"
QT_MOC_LITERAL(61, 909, 8), // "about_qt"
QT_MOC_LITERAL(62, 918, 9), // "set_speed"
QT_MOC_LITERAL(63, 928, 14), // "machine_status"
QT_MOC_LITERAL(64, 943, 24), // "machine::Machine::Status"
QT_MOC_LITERAL(65, 968, 2), // "st"
QT_MOC_LITERAL(66, 971, 12), // "machine_exit"
QT_MOC_LITERAL(67, 984, 12), // "machine_trap"
QT_MOC_LITERAL(68, 997, 28), // "machine::SimulatorException&"
QT_MOC_LITERAL(69, 1026, 1), // "e"
QT_MOC_LITERAL(70, 1028, 24), // "view_mnemonics_registers"
QT_MOC_LITERAL(71, 1053, 6), // "enable"
QT_MOC_LITERAL(72, 1060, 16), // "message_selected"
QT_MOC_LITERAL(73, 1077, 21), // "update_core_frequency"
QT_MOC_LITERAL(74, 1099, 9) // "frequency"

    },
    "MainWindow\0report_message\0\0messagetype::Type\0"
    "type\0file\0line\0column\0text\0hint\0"
    "clear_messages\0new_machine\0machine_reload\0"
    "force_memory_reset\0force_elf_load\0"
    "print_action\0close_source_by_name\0"
    "QString&\0filename\0ask\0example_source\0"
    "source_file\0compile_source\0build_execute\0"
    "build_execute_no_check\0build_execute_with_save\0"
    "cancel\0tosavelist\0reset_state_registers\0"
    "reset_state_program\0reset_state_memory\0"
    "reset_state_cache_program\0"
    "reset_state_cache_data\0reset_state_cache_level2\0"
    "reset_state_peripherals\0reset_state_terminal\0"
    "reset_state_lcd_display\0reset_state_csrdock\0"
    "reset_state_messages\0show_registers\0"
    "show_program\0show_memory\0show_cache_data\0"
    "show_cache_program\0show_cache_level2\0"
    "show_peripherals\0show_terminal\0"
    "show_lcd_display\0show_csrdock\0"
    "show_hide_coreview\0show\0show_messages\0"
    "reset_windows\0show_symbol_dialog\0"
    "show_bp_btb\0show_bp_bht\0show_bp_info\0"
    "reset_state_bp_btb\0reset_state_bp_bht\0"
    "reset_state_bp_info\0about_program\0"
    "about_qt\0set_speed\0machine_status\0"
    "machine::Machine::Status\0st\0machine_exit\0"
    "machine_trap\0machine::SimulatorException&\0"
    "e\0view_mnemonics_registers\0enable\0"
    "message_selected\0update_core_frequency\0"
    "frequency"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      54,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    6,  284,    2, 0x06 /* Public */,
      10,    0,  297,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      11,    0,  298,    2, 0x0a /* Public */,
      12,    2,  299,    2, 0x0a /* Public */,
      12,    1,  304,    2, 0x2a /* Public | MethodCloned */,
      12,    0,  307,    2, 0x2a /* Public | MethodCloned */,
      15,    0,  308,    2, 0x0a /* Public */,
      16,    2,  309,    2, 0x0a /* Public */,
      16,    1,  314,    2, 0x2a /* Public | MethodCloned */,
      20,    1,  317,    2, 0x0a /* Public */,
      22,    0,  320,    2, 0x0a /* Public */,
      23,    0,  321,    2, 0x0a /* Public */,
      24,    0,  322,    2, 0x0a /* Public */,
      25,    2,  323,    2, 0x0a /* Public */,
      28,    0,  328,    2, 0x0a /* Public */,
      29,    0,  329,    2, 0x0a /* Public */,
      30,    0,  330,    2, 0x0a /* Public */,
      31,    0,  331,    2, 0x0a /* Public */,
      32,    0,  332,    2, 0x0a /* Public */,
      33,    0,  333,    2, 0x0a /* Public */,
      34,    0,  334,    2, 0x0a /* Public */,
      35,    0,  335,    2, 0x0a /* Public */,
      36,    0,  336,    2, 0x0a /* Public */,
      37,    0,  337,    2, 0x0a /* Public */,
      38,    0,  338,    2, 0x0a /* Public */,
      39,    0,  339,    2, 0x0a /* Public */,
      40,    0,  340,    2, 0x0a /* Public */,
      41,    0,  341,    2, 0x0a /* Public */,
      42,    0,  342,    2, 0x0a /* Public */,
      43,    0,  343,    2, 0x0a /* Public */,
      44,    0,  344,    2, 0x0a /* Public */,
      45,    0,  345,    2, 0x0a /* Public */,
      46,    0,  346,    2, 0x0a /* Public */,
      47,    0,  347,    2, 0x0a /* Public */,
      48,    0,  348,    2, 0x0a /* Public */,
      49,    1,  349,    2, 0x0a /* Public */,
      51,    0,  352,    2, 0x0a /* Public */,
      52,    0,  353,    2, 0x0a /* Public */,
      53,    0,  354,    2, 0x0a /* Public */,
      54,    0,  355,    2, 0x0a /* Public */,
      55,    0,  356,    2, 0x0a /* Public */,
      56,    0,  357,    2, 0x0a /* Public */,
      57,    0,  358,    2, 0x0a /* Public */,
      58,    0,  359,    2, 0x0a /* Public */,
      59,    0,  360,    2, 0x0a /* Public */,
      60,    0,  361,    2, 0x0a /* Public */,
      61,    0,  362,    2, 0x0a /* Public */,
      62,    0,  363,    2, 0x0a /* Public */,
      63,    1,  364,    2, 0x0a /* Public */,
      66,    0,  367,    2, 0x0a /* Public */,
      67,    1,  368,    2, 0x0a /* Public */,
      70,    1,  371,    2, 0x0a /* Public */,
      72,    6,  374,    2, 0x0a /* Public */,
      73,    1,  387,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3, QMetaType::QString, QMetaType::Int, QMetaType::Int, QMetaType::QString, QMetaType::QString,    4,    5,    6,    7,    8,    9,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool, QMetaType::Bool,   13,   14,
    QMetaType::Void, QMetaType::Bool,   13,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 17, QMetaType::Bool,   18,   19,
    QMetaType::Void, 0x80000000 | 17,   18,
    QMetaType::Void, QMetaType::QString,   21,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool, QMetaType::QStringList,   26,   27,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   50,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 64,   65,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 68,   69,
    QMetaType::Void, QMetaType::Bool,   71,
    QMetaType::Void, 0x80000000 | 3, QMetaType::QString, QMetaType::Int, QMetaType::Int, QMetaType::QString, QMetaType::QString,    4,    5,    6,    7,    8,    9,
    QMetaType::Void, QMetaType::Double,   74,

       0        // eod
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MainWindow *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->report_message((*reinterpret_cast< messagetype::Type(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4])),(*reinterpret_cast< QString(*)>(_a[5])),(*reinterpret_cast< QString(*)>(_a[6]))); break;
        case 1: _t->clear_messages(); break;
        case 2: _t->new_machine(); break;
        case 3: _t->machine_reload((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 4: _t->machine_reload((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 5: _t->machine_reload(); break;
        case 6: _t->print_action(); break;
        case 7: _t->close_source_by_name((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 8: _t->close_source_by_name((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 9: _t->example_source((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 10: _t->compile_source(); break;
        case 11: _t->build_execute(); break;
        case 12: _t->build_execute_no_check(); break;
        case 13: _t->build_execute_with_save((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< QStringList(*)>(_a[2]))); break;
        case 14: _t->reset_state_registers(); break;
        case 15: _t->reset_state_program(); break;
        case 16: _t->reset_state_memory(); break;
        case 17: _t->reset_state_cache_program(); break;
        case 18: _t->reset_state_cache_data(); break;
        case 19: _t->reset_state_cache_level2(); break;
        case 20: _t->reset_state_peripherals(); break;
        case 21: _t->reset_state_terminal(); break;
        case 22: _t->reset_state_lcd_display(); break;
        case 23: _t->reset_state_csrdock(); break;
        case 24: _t->reset_state_messages(); break;
        case 25: _t->show_registers(); break;
        case 26: _t->show_program(); break;
        case 27: _t->show_memory(); break;
        case 28: _t->show_cache_data(); break;
        case 29: _t->show_cache_program(); break;
        case 30: _t->show_cache_level2(); break;
        case 31: _t->show_peripherals(); break;
        case 32: _t->show_terminal(); break;
        case 33: _t->show_lcd_display(); break;
        case 34: _t->show_csrdock(); break;
        case 35: _t->show_hide_coreview((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 36: _t->show_messages(); break;
        case 37: _t->reset_windows(); break;
        case 38: _t->show_symbol_dialog(); break;
        case 39: _t->show_bp_btb(); break;
        case 40: _t->show_bp_bht(); break;
        case 41: _t->show_bp_info(); break;
        case 42: _t->reset_state_bp_btb(); break;
        case 43: _t->reset_state_bp_bht(); break;
        case 44: _t->reset_state_bp_info(); break;
        case 45: _t->about_program(); break;
        case 46: _t->about_qt(); break;
        case 47: _t->set_speed(); break;
        case 48: _t->machine_status((*reinterpret_cast< machine::Machine::Status(*)>(_a[1]))); break;
        case 49: _t->machine_exit(); break;
        case 50: _t->machine_trap((*reinterpret_cast< machine::SimulatorException(*)>(_a[1]))); break;
        case 51: _t->view_mnemonics_registers((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 52: _t->message_selected((*reinterpret_cast< messagetype::Type(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4])),(*reinterpret_cast< const QString(*)>(_a[5])),(*reinterpret_cast< const QString(*)>(_a[6]))); break;
        case 53: _t->update_core_frequency((*reinterpret_cast< double(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (MainWindow::*)(messagetype::Type , QString , int , int , QString , QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MainWindow::report_message)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (MainWindow::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MainWindow::clear_messages)) {
                *result = 1;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject MainWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_MainWindow.data,
    qt_meta_data_MainWindow,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 54)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 54;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 54)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 54;
    }
    return _id;
}

// SIGNAL 0
void MainWindow::report_message(messagetype::Type _t1, QString _t2, int _t3, int _t4, QString _t5, QString _t6)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t4))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t5))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t6))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void MainWindow::clear_messages()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}
struct qt_meta_stringdata_SimpleAsmWithEditorCheck_t {
    QByteArrayData data[1];
    char stringdata0[25];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_SimpleAsmWithEditorCheck_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_SimpleAsmWithEditorCheck_t qt_meta_stringdata_SimpleAsmWithEditorCheck = {
    {
QT_MOC_LITERAL(0, 0, 24) // "SimpleAsmWithEditorCheck"

    },
    "SimpleAsmWithEditorCheck"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SimpleAsmWithEditorCheck[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

void SimpleAsmWithEditorCheck::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

QT_INIT_METAOBJECT const QMetaObject SimpleAsmWithEditorCheck::staticMetaObject = { {
    QMetaObject::SuperData::link<SimpleAsm::staticMetaObject>(),
    qt_meta_stringdata_SimpleAsmWithEditorCheck.data,
    qt_meta_data_SimpleAsmWithEditorCheck,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *SimpleAsmWithEditorCheck::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SimpleAsmWithEditorCheck::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_SimpleAsmWithEditorCheck.stringdata0))
        return static_cast<void*>(this);
    return SimpleAsm::qt_metacast(_clname);
}

int SimpleAsmWithEditorCheck::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = SimpleAsm::qt_metacall(_c, _id, _a);
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
