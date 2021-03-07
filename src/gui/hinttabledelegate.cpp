#include "hinttabledelegate.h"

#include <QApplication>

QSize HintTableDelegate::sizeHintForText(
    const QStyleOptionViewItem &option,
    const QModelIndex &index,
    const QString &str) const {
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);
    const QWidget *widget = option.widget;
    QStyle *style = widget ? widget->style() : QApplication::style();

    opt.features |= QStyleOptionViewItem::HasDisplay;
    opt.text = str;

    return style->sizeFromContents(
        QStyle::CT_ItemViewItem, &opt, QSize(), widget);
}
