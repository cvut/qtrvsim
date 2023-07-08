#ifndef TEXTSIGNALACTION_H
#define TEXTSIGNALACTION_H

#include <QAction>
#include <QObject>

class TextSignalAction : public QAction {
    Q_OBJECT

    using Super = QAction;

public:
    explicit TextSignalAction(QObject *parent = nullptr);
    explicit TextSignalAction(const QString &text, QObject *parent = nullptr);
    TextSignalAction(
        const QString &text,
        QString signal_text,
        QObject *parent = nullptr);
    TextSignalAction(
        const QIcon &icon,
        const QString &text,
        QObject *parent = nullptr);
    TextSignalAction(
        const QIcon &icon,
        const QString &text,
        QString signal_text,
        QObject *parent = nullptr);
signals:
    void activated(QString signal_text);

protected slots:
    void process_triggered(bool checked);

protected:
    QString signal_text;
};

#endif // TEXTSIGNALACTION_H
