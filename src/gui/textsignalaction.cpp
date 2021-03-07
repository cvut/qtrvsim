#include "textsignalaction.h"

#include <QApplication>
#include <utility>

TextSignalAction::TextSignalAction(QObject *parent) : Super(parent) {
    connect(
        this, &TextSignalAction::triggered, this,
        &TextSignalAction::process_triggered);
}

TextSignalAction::TextSignalAction(const QString &text, QObject *parent)
    : Super(text, parent)
    , signal_text(text) {
    connect(
        this, &TextSignalAction::triggered, this,
        &TextSignalAction::process_triggered);
}

TextSignalAction::TextSignalAction(
    const QString &text,
    QString signal_text,
    QObject *parent)
    : Super(text, parent)
    , signal_text(std::move(signal_text)) {
    connect(
        this, &TextSignalAction::triggered, this,
        &TextSignalAction::process_triggered);
}

TextSignalAction::TextSignalAction(
    const QIcon &icon,
    const QString &text,
    QObject *parent)
    : Super(icon, text, parent)
    , signal_text(text) {
    connect(
        this, &TextSignalAction::triggered, this,
        &TextSignalAction::process_triggered);
}

TextSignalAction::TextSignalAction(
    const QIcon &icon,
    const QString &text,
    QString signal_text,
    QObject *parent)
    : Super(icon, text, parent)
    , signal_text(std::move(signal_text)) {
    connect(
        this, &TextSignalAction::triggered, this,
        &TextSignalAction::process_triggered);
}

void TextSignalAction::process_triggered(bool checked) {
    (void)checked;
    emit activated(signal_text);
}
