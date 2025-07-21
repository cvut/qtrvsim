/****************************************************************************
** Meta object code from reading C++ file 'editordock.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.13)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../../src/gui/windows/editor/editordock.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'editordock.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.13. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_EditorDock_t {
    QByteArrayData data[20];
    char stringdata0[258];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_EditorDock_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_EditorDock_t qt_meta_stringdata_EditorDock = {
    {
QT_MOC_LITERAL(0, 0, 10), // "EditorDock"
QT_MOC_LITERAL(1, 11, 24), // "editor_available_changed"
QT_MOC_LITERAL(2, 36, 0), // ""
QT_MOC_LITERAL(3, 37, 9), // "available"
QT_MOC_LITERAL(4, 47, 21), // "set_show_line_numbers"
QT_MOC_LITERAL(5, 69, 7), // "visible"
QT_MOC_LITERAL(6, 77, 16), // "open_file_dialog"
QT_MOC_LITERAL(7, 94, 8), // "save_tab"
QT_MOC_LITERAL(8, 103, 5), // "index"
QT_MOC_LITERAL(9, 109, 16), // "save_current_tab"
QT_MOC_LITERAL(10, 126, 11), // "save_tab_as"
QT_MOC_LITERAL(11, 138, 19), // "save_current_tab_as"
QT_MOC_LITERAL(12, 158, 11), // "save_tab_to"
QT_MOC_LITERAL(13, 170, 8), // "filename"
QT_MOC_LITERAL(14, 179, 19), // "save_current_tab_to"
QT_MOC_LITERAL(15, 199, 9), // "close_tab"
QT_MOC_LITERAL(16, 209, 17), // "close_current_tab"
QT_MOC_LITERAL(17, 227, 17), // "close_tab_by_name"
QT_MOC_LITERAL(18, 245, 8), // "QString&"
QT_MOC_LITERAL(19, 254, 3) // "ask"

    },
    "EditorDock\0editor_available_changed\0"
    "\0available\0set_show_line_numbers\0"
    "visible\0open_file_dialog\0save_tab\0"
    "index\0save_current_tab\0save_tab_as\0"
    "save_current_tab_as\0save_tab_to\0"
    "filename\0save_current_tab_to\0close_tab\0"
    "close_current_tab\0close_tab_by_name\0"
    "QString&\0ask"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_EditorDock[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   79,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    1,   82,    2, 0x0a /* Public */,
       6,    0,   85,    2, 0x0a /* Public */,
       7,    1,   86,    2, 0x0a /* Public */,
       9,    0,   89,    2, 0x0a /* Public */,
      10,    1,   90,    2, 0x0a /* Public */,
      11,    0,   93,    2, 0x0a /* Public */,
      12,    2,   94,    2, 0x0a /* Public */,
      14,    1,   99,    2, 0x0a /* Public */,
      15,    1,  102,    2, 0x0a /* Public */,
      16,    0,  105,    2, 0x0a /* Public */,
      17,    2,  106,    2, 0x0a /* Public */,
      17,    1,  111,    2, 0x2a /* Public | MethodCloned */,

 // signals: parameters
    QMetaType::Void, QMetaType::Bool,    3,

 // slots: parameters
    QMetaType::Void, QMetaType::Bool,    5,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    8,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    8,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::QString,    8,   13,
    QMetaType::Void, QMetaType::QString,   13,
    QMetaType::Void, QMetaType::Int,    8,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 18, QMetaType::Bool,   13,   19,
    QMetaType::Void, 0x80000000 | 18,   13,

       0        // eod
};

void EditorDock::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<EditorDock *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->editor_available_changed((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->set_show_line_numbers((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: _t->open_file_dialog(); break;
        case 3: _t->save_tab((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->save_current_tab(); break;
        case 5: _t->save_tab_as((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: _t->save_current_tab_as(); break;
        case 7: _t->save_tab_to((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 8: _t->save_current_tab_to((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 9: _t->close_tab((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: _t->close_current_tab(); break;
        case 11: _t->close_tab_by_name((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 12: _t->close_tab_by_name((*reinterpret_cast< QString(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (EditorDock::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&EditorDock::editor_available_changed)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject EditorDock::staticMetaObject = { {
    QMetaObject::SuperData::link<HidingTabWidget::staticMetaObject>(),
    qt_meta_stringdata_EditorDock.data,
    qt_meta_data_EditorDock,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *EditorDock::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *EditorDock::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_EditorDock.stringdata0))
        return static_cast<void*>(this);
    return HidingTabWidget::qt_metacast(_clname);
}

int EditorDock::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = HidingTabWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 13)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 13;
    }
    return _id;
}

// SIGNAL 0
void EditorDock::editor_available_changed(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
