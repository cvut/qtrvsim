#pragma once

#include "svgmetadata.h"

#include <QGraphicsItem>

namespace svgscene {

template<typename TT>
class SvgDomTree {
public:
    explicit SvgDomTree(QGraphicsItem *root);

    TT *getElement() const;

    QString getAttrValueOr(const QString &attr_name, const QString &default_value = QString());
    QString getCssValueOr(const QString &attr_name, const QString &default_value = QString());

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

    template<typename T = QGraphicsItem>
    SvgDomTree<T> find(const QString &attr_name = QString(), const QString &attr_value = QString());

    template<typename T = QGraphicsItem>
    QList<SvgDomTree<T>>
    findAll(const QString &attr_name = QString(), const QString &attr_value = QString());

protected:
    /**
     * WASM does not allows exception catching so we have to handle
     * recoverable errors in a different way - using nullptr.
     */
    template<typename T>
    static T *findFromParentRaw(
        const QGraphicsItem *parent,
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
    if (item == nullptr) { throw std::out_of_range("Supplied item is nullptr."); }
    if (attr_name.isEmpty()) { return true; }

    auto attrs = getXmlAttributes(item);
    if (!attrs.contains(attr_name)) { return false; }
    return attr_value.isEmpty() || attrs.value(attr_name) == attr_value;
}

template<typename T>
SvgDomTree<T>::SvgDomTree(QGraphicsItem *root) : root(dynamic_cast<T *>(root)) {
    if (this->root == nullptr) {
        throw std::out_of_range("Cannot build dom tree with nullptr item.");
    }
}

template<typename TT>
TT *SvgDomTree<TT>::getElement() const {
    return root;
}

template<typename T>
QString SvgDomTree<T>::getAttrValueOr(const QString &attr_name, const QString &default_value) {
    svgscene::XmlAttributes attrs = getXmlAttributes(root);
    return attrs.value(attr_name, default_value);
}

template<typename T>
QString SvgDomTree<T>::getCssValueOr(const QString &attr_name, const QString &default_value) {
    return getCssAttributeOr(root, attr_name, default_value);
}

template<typename TT>
template<typename T>
SvgDomTree<T> SvgDomTree<TT>::findFromParent(
    const QGraphicsItem *parent,
    const QString &attr_name,
    const QString &attr_value) {
    if (!parent) { throw std::out_of_range("Current element is nullptr."); }

    for (QGraphicsItem *_child : parent->childItems()) {
        if (T *child = dynamic_cast<T *>(_child)) {
            if (itemMatchesSelector<T>(child, attr_name, attr_value)) {
                return SvgDomTree<T>(child);
            }
        }

        T *found = findFromParentRaw<T>(_child, attr_name, attr_value);
        if (found != nullptr) { return SvgDomTree<T>(found); }
    }
    throw std::out_of_range("Not found.");
}

template<typename TT>
template<typename T>
T *SvgDomTree<TT>::findFromParentRaw(
    const QGraphicsItem *parent,
    const QString &attr_name,
    const QString &attr_value) {
    if (!parent) { return nullptr; }

    for (QGraphicsItem *_child : parent->childItems()) {
        if (T *child = dynamic_cast<T *>(_child)) {
            if (itemMatchesSelector<T>(child, attr_name, attr_value)) { return child; }
        }
        T *found = findFromParentRaw<T>(_child, attr_name, attr_value);
        if (found != nullptr) { return found; }
    }
    return nullptr;
}
template<typename TT>
template<typename T>
QList<SvgDomTree<T>> SvgDomTree<TT>::findAllFromParent(
    const QGraphicsItem *parent,
    const QString &attr_name,
    const QString &attr_value) {
    QList<SvgDomTree<T>> ret;

    if (!parent) { return ret; }

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
SvgDomTree<T> SvgDomTree<TT>::find(const QString &attr_name, const QString &attr_value) {
    return findFromParent<T>(root, attr_name, attr_value);
}

template<typename TT>
template<typename T>
QList<SvgDomTree<T>> SvgDomTree<TT>::findAll(const QString &attr_name, const QString &attr_value) {
    return findAllFromParent<T>(root, attr_name, attr_value);
}

} // namespace svgscene
