#include "messagesview.h"

#include "messagesmodel.h"

#include <QApplication>
#include <QClipboard>
#include <QFontMetrics>
#include <QHeaderView>
#include <QKeyEvent>
#include <QScrollBar>

MessagesView::MessagesView(QWidget *parent, QSettings *settings)
    : Super(parent) {
    this->settings = settings;
}

void MessagesView::resizeEvent(QResizeEvent *event) {
    Super::resizeEvent(event);
}

void MessagesView::keyPressEvent(QKeyEvent *event) {
    if (event->matches(QKeySequence::Copy)) {
        QString text;
        QItemSelectionRange range = selectionModel()->selection().first();
        for (auto i = range.top(); i <= range.bottom(); ++i) {
            QStringList rowContents;
            for (auto j = range.left(); j <= range.right(); ++j) {
                rowContents << model()->index(i, j).data().toString();
            }
            text += rowContents.join("\t");
            text += "\n";
        }
        QApplication::clipboard()->setText(text);
    } else {
        Super::keyPressEvent(event);
    }
}
