#include "editortab.h"

#include "srceditor.h"

#include <QtGui/qwindowdefs.h>
#include <QtWidgets/qboxlayout.h>

EditorTab::EditorTab(bool show_line_numbers, QWidget *parent)
    : Super(parent)
    , layout(new QVBoxLayout(this))
    , editor(new SrcEditor(this))
    , status_bar_layout(new QHBoxLayout())
    , status_bar_path(new QLabel("Unknown", this))
    , status_bar_location(new QLabel("0:0", this)) {
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(reinterpret_cast<QWidget *>(editor));
    status_bar_layout->setSpacing(0);
    status_bar_layout->setContentsMargins(5, 0, 5, 0);
    status_bar_layout->addWidget(status_bar_path);
    status_bar_layout->addWidget(status_bar_location);
    layout->addLayout(status_bar_layout);
    status_bar_path->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
    status_bar_location->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    connect(editor, &SrcEditor::cursorPositionChanged, this, [this]() {
        auto cursor = editor->textCursor();
        status_bar_location->setText(
            QString("%1:%2").arg(cursor.blockNumber() + 1).arg(cursor.columnNumber() + 1));
    });
    connect(editor, &SrcEditor::file_name_change, this, [this]() { elide_file_name(); });
    set_show_line_number(show_line_numbers);
}

QString EditorTab::title() {
    return editor->title();
}

void EditorTab::set_show_line_number(bool visible) {
    editor->setShowLineNumbers(visible);
}

void EditorTab::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    elide_file_name();
}

void EditorTab::elide_file_name() {
    auto filename = editor->filename().isEmpty() ? "Unknown" : editor->filename();
    QFontMetrics metrics(status_bar_path->font());
    int width = status_bar_layout->geometry().width() - status_bar_location->geometry().width() - 10;
    QString clippedText = metrics.elidedText(filename, Qt::ElideMiddle, width);
    status_bar_path->setText(clippedText);
}
