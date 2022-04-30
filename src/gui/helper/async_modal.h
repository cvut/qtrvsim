/**
 * This file provides API for modal windows (dialogs, message boxes) which runs asynchronously
 * and therefore is safe in WASM, where blocking will cause the app to freeze!
 *
 * None of these classes shall be used directly, unless special callback is required. In such
 * cases, high caution needs to be applied.
 *
 * NOTE: This code could be optimized by adding async behavior only to emscripten, however modals
 * are typically not performance sensitive and an special case would complicate debugging.
 *
 * @file
 */

#ifndef ASYNC_MODAL_H
#define ASYNC_MODAL_H

#include <QMessageBox>

inline void showAsyncMessageBox(
    QWidget *parent,
    QMessageBox::Icon icon,
    const QString &title,
    const QString &text,
    const QString &detailed_text = QString(),
    const QString &tooltip_text = QString()) {
    auto msg = new QMessageBox(icon, title, text, QMessageBox::Ok, parent);
    msg->setDetailedText(detailed_text);
    msg->setToolTip(tooltip_text);
    // This is necessary as WASM does not support blocking APIs.
    msg->setAttribute(Qt::WA_DeleteOnClose);
    msg->open();
}

inline void showAsyncCriticalBox(
    QWidget *parent,
    const QString &title,
    const QString &text,
    const QString &detailed_text = QString(),
    const QString &tooltip_text = QString()) {
    showAsyncMessageBox(parent, QMessageBox::Critical, title, text, detailed_text, tooltip_text);
}

#endif