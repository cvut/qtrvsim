#ifndef HINTTABLEDELEGATE_H
#define HINTTABLEDELEGATE_H

#include <QObject>
#include <QStyledItemDelegate>

class HintTableDelegate : public QStyledItemDelegate {
    Q_OBJECT

    using Super = QStyledItemDelegate;

public:
    HintTableDelegate(QWidget *parent = nullptr) : Super(parent) {}

    QSize sizeHintForText(
        const QStyleOptionViewItem &option,
        const QModelIndex &index,
        const QString &str) const;
};

#endif // HINTTABLEDELEGATE_H
