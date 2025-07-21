/****************************************************************************
** Meta object code from reading C++ file 'hyperlinkitem.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.13)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../../external/svgscene/src/svgscene/components/hyperlinkitem.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'hyperlinkitem.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.13. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_svgscene__HyperlinkItem_t {
    QByteArrayData data[3];
    char stringdata0[35];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_svgscene__HyperlinkItem_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_svgscene__HyperlinkItem_t qt_meta_stringdata_svgscene__HyperlinkItem = {
    {
QT_MOC_LITERAL(0, 0, 23), // "svgscene::HyperlinkItem"
QT_MOC_LITERAL(1, 24, 9), // "triggered"
QT_MOC_LITERAL(2, 34, 0) // ""

    },
    "svgscene::HyperlinkItem\0triggered\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_svgscene__HyperlinkItem[] = {

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
       1,    0,   19,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void,

       0        // eod
};

void svgscene::HyperlinkItem::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<HyperlinkItem *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->triggered(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (HyperlinkItem::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&HyperlinkItem::triggered)) {
                *result = 0;
                return;
            }
        }
    }
    (void)_a;
}

QT_INIT_METAOBJECT const QMetaObject svgscene::HyperlinkItem::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_svgscene__HyperlinkItem.data,
    qt_meta_data_svgscene__HyperlinkItem,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *svgscene::HyperlinkItem::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *svgscene::HyperlinkItem::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_svgscene__HyperlinkItem.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "GroupItem"))
        return static_cast< GroupItem*>(this);
    return QObject::qt_metacast(_clname);
}

int svgscene::HyperlinkItem::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void svgscene::HyperlinkItem::triggered()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
