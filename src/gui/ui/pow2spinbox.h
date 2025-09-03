#ifndef POW2SPINBOX_H
#define POW2SPINBOX_H

#include <QSpinBox>
#include <QValidator>

class Pow2SpinBox : public QSpinBox {
    Q_OBJECT
public:
    explicit Pow2SpinBox(QWidget *parent = nullptr);

protected:
    QValidator::State validate(QString &input, int &pos) const override;
    int valueFromText(const QString &text) const override;
    QString textFromValue(int value) const override;
    void stepBy(int steps) override;
};

#endif // POW2SPINBOX_H
