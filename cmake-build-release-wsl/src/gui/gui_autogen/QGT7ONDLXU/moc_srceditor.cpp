/****************************************************************************
** Meta object code from reading C++ file 'srceditor.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.13)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../../src/gui/windows/editor/srceditor.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'srceditor.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.13. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_SrcEditor_t {
    QByteArrayData data[10];
    char stringdata0[112];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_SrcEditor_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_SrcEditor_t qt_meta_stringdata_SrcEditor = {
    {
QT_MOC_LITERAL(0, 0, 9), // "SrcEditor"
QT_MOC_LITERAL(1, 10, 16), // "file_name_change"
QT_MOC_LITERAL(2, 27, 0), // ""
QT_MOC_LITERAL(3, 28, 18), // "setShowLineNumbers"
QT_MOC_LITERAL(4, 47, 7), // "visible"
QT_MOC_LITERAL(5, 55, 13), // "updateMargins"
QT_MOC_LITERAL(6, 69, 13), // "newBlockCount"
QT_MOC_LITERAL(7, 83, 20), // "updateLineNumberArea"
QT_MOC_LITERAL(8, 104, 4), // "rect"
QT_MOC_LITERAL(9, 109, 2) // "dy"

    },
    "SrcEditor\0file_name_change\0\0"
    "setShowLineNumbers\0visible\0updateMargins\0"
    "newBlockCount\0updateLineNumberArea\0"
    "rect\0dy"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SrcEditor[] = {

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
       1,    0,   34,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    1,   35,    2, 0x0a /* Public */,
       5,    1,   38,    2, 0x08 /* Private */,
       7,    2,   41,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::Bool,    4,
    QMetaType::Void, QMetaType::Int,    6,
    QMetaType::Void, QMetaType::QRect, QMetaType::Int,    8,    9,

       0        // eod
};

void SrcEditor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<SrcEditor *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->file_name_change(); break;
        case 1: _t->setShowLineNumbers((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: _t->updateMargins((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->updateLineNumberArea((*reinterpret_cast< const QRect(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (SrcEditor::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SrcEditor::file_name_change)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject SrcEditor::staticMetaObject = { {
    QMetaObject::SuperData::link<QPlainTextEdit::staticMetaObject>(),
    qt_meta_stringdata_SrcEditor.data,
    qt_meta_data_SrcEditor,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *SrcEditor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SrcEditor::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_SrcEditor.stringdata0))
        return static_cast<void*>(this);
    return QPlainTextEdit::qt_metacast(_clname);
}

int SrcEditor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QPlainTextEdit::qt_metacall(_c, _id, _a);
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
void SrcEditor::file_name_change()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
