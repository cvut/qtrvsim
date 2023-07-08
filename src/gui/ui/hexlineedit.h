#ifndef HEXLINEEDIT_H
#define HEXLINEEDIT_H

#include <QLineEdit>
#include <QObject>

class HexLineEdit : public QLineEdit {
    Q_OBJECT

    using Super = QLineEdit;

public:
    explicit HexLineEdit(
        QWidget *parent = nullptr,
        int digits = 8,
        int base = 0,
        const QString &prefix = "0x");

public slots:
    void set_value(uint32_t value);

signals:
    void value_edit_finished(uint32_t value);

private slots:
    void on_edit_finished();

private:
    int base;
    int digits;
    QString prefix;
    uint32_t last_set;
};

#endif // HEXLINEEDIT_H
