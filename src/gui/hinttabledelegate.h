#ifndef HINTTABLEDELEGATE_H
#define HINTTABLEDELEGATE_H

#include <QObject>
#include <QStyledItemDelegate>

class HintTableDelegate : public QStyledItemDelegate {
    Q_OBJECT

    using Super = QStyledItemDelegate;

public:
    explicit HintTableDelegate(QWidget *parent = nullptr) : Super(parent) {}

    [[nodiscard]] QSize sizeHintForText(
        const QStyleOptionViewItem &option,
        const QModelIndex &index,
        const QString &str) const;
};

#endif // HINTTABLEDELEGATE_H
