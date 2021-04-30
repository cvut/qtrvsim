#pragma once

#include "types.h"

#include <QGraphicsItem>

namespace svgscene {

template<typename T>
class SvgDomTree;

template<typename T>
static bool itemMatchesSelector(
    const QGraphicsItem *item,
    const QString &attr_name = QString(),
    const QString &attr_value = QString());

template<typename TT>
class SvgDomTree {
public:
    explicit SvgDomTree(QGraphicsItem *root);

    TT *getElement() const;

    QString getAttrValueOr(
        const QString &attr_name,
        const QString &default_value = QString());
    QString getCssValueOr(
        const QString &attr_name,
        const QString &default_value = QString());

    template<typename T>
    static SvgDomTree<T> findFromParent(
        const QGraphicsItem *parent,
        const QString &attr_name = QString(),
        const QString &attr_value = QString());

    template<typename T>
    static QList<SvgDomTree<T>> findAllFromParent(
        const QGraphicsItem *parent,
        const QString &attr_name = QString(),
        const QString &attr_value = QString());

    template<typename T>
    SvgDomTree<T> find(
        const QString &attr_name = QString(),
        const QString &attr_value = QString());

    template<typename T>
    QList<SvgDomTree<T>> findAll(
        const QString &attr_name = QString(),
        const QString &attr_value = QString());

protected:
    TT *root;
};

class SvgDocument {
public:
    explicit SvgDocument(QGraphicsItem *root);
    SvgDomTree<QGraphicsItem> getRoot() const;

protected:
    SvgDomTree<QGraphicsItem> root;
};

template<typename T>
bool itemMatchesSelector(
    const QGraphicsItem *item,
    const QString &attr_name,
    const QString &attr_value) {
    if (item == nullptr) {
        return false;
    }
    if (attr_name.isEmpty()) {
        return true;
    }
    auto attrs = qvariant_cast<svgscene::XmlAttributes>(
        item->data(Types::DataKey::XmlAttributes));
    if (!attrs.contains(attr_name)) {
        return false;
    }
    return attr_value.isEmpty() || attrs.value(attr_name) == attr_value;
}

template<typename TT>
template<typename T>
SvgDomTree<T> SvgDomTree<TT>::findFromParent(
    const QGraphicsItem *parent,
    const QString &attr_name,
    const QString &attr_value) {
    if (!parent) {
        return SvgDomTree<T>(nullptr);
    }

    for (QGraphicsItem *_child : parent->childItems()) {
        if (T *child = dynamic_cast<T *>(_child)) {
            if (itemMatchesSelector<T>(child, attr_name, attr_value)) {
                return SvgDomTree<T>(child);
            }
        }
        SvgDomTree<T> tit = findFromParent<T>(_child, attr_name, attr_value);
        if (tit.getElement() != nullptr) {
            return tit;
        }
    }
    return SvgDomTree<T>(nullptr);
}
template<typename TT>
template<typename T>
QList<SvgDomTree<T>> SvgDomTree<TT>::findAllFromParent(
    const QGraphicsItem *parent,
    const QString &attr_name,
    const QString &attr_value) {
    QList<SvgDomTree<T>> ret;

    if (!parent) {
        return ret;
    }

    for (QGraphicsItem *_child : parent->childItems()) {
        if (T *child = dynamic_cast<T *>(_child)) {
            if (itemMatchesSelector<T>(child, attr_name, attr_value)) {
                ret.append(SvgDomTree<T>(child));
            }
        }
        ret.append(findAllFromParent<T>(_child, attr_name, attr_value));
    }
    return ret;
}

template<typename TT>
template<typename T>
SvgDomTree<T>
SvgDomTree<TT>::find(const QString &attr_name, const QString &attr_value) {
    return findFromParent<T>(root, attr_name, attr_value);
}
template<typename TT>
template<typename T>
QList<SvgDomTree<T>>
SvgDomTree<TT>::findAll(const QString &attr_name, const QString &attr_value) {
    return findAllFromParent<T>(root, attr_name, attr_value);
}
template<typename TT>
TT *SvgDomTree<TT>::getElement() const {
    return root;
}

template<typename T>
SvgDomTree<T>::SvgDomTree(QGraphicsItem *root)
    : root(dynamic_cast<T *>(root)) {}

template<typename T>
QString SvgDomTree<T>::getAttrValueOr(
    const QString &attr_name,
    const QString &default_value) {
    svgscene::XmlAttributes attrs = qvariant_cast<svgscene::XmlAttributes>(
        root->data(Types::DataKey::XmlAttributes));
    return attrs.value(attr_name, default_value);
}

template<typename T>
QString SvgDomTree<T>::getCssValueOr(
    const QString &attr_name,
    const QString &default_value) {
    svgscene::CssAttributes attrs = qvariant_cast<svgscene::CssAttributes>(
        root->data(Types::DataKey::CssAttributes));
    return attrs.value(attr_name, default_value);
}

} // namespace svgscene
