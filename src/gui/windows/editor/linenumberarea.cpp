#include "linenumberarea.h"

#include "srceditor.h"

#include <cmath>
#include <qpainter.h>
#include <qsize.h>

constexpr int RIGHT_MARGIN = 5;
constexpr int RIGHT_PADDING = 5;
constexpr int LEFT_PADDING = 5;

LineNumberArea::LineNumberArea(SrcEditor *editor_) : QWidget(editor_), editor(editor_) {}

QSize LineNumberArea::sizeHint() const {
    if (!line_numbers_visible) { return { 0, 0 }; }

    int digits = std::log10(std::max(1, editor->blockCount())) + 2;
    int space = editor->fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits + LEFT_PADDING
                + RIGHT_PADDING + RIGHT_MARGIN;
    return { space, 0 };
}
void LineNumberArea::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.fillRect(event->rect(), palette().base());
    painter.drawLine(
        event->rect().right() - RIGHT_MARGIN, 0, event->rect().right() - RIGHT_MARGIN,
        event->rect().bottom());

    QTextBlock block = editor->firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top
        = qRound(editor->blockBoundingGeometry(block).translated(editor->contentOffset()).top());
    int bottom = top + qRound(editor->blockBoundingRect(block).height());
    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(palette().windowText().color());
            painter.drawText(
                0, top, this->sizeHint().width() - RIGHT_PADDING - RIGHT_MARGIN,
                editor->fontMetrics().height(), Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + qRound(editor->blockBoundingRect(block).height());
        ++blockNumber;
    }
}
void LineNumberArea::set(bool visible) {
    QWidget::setVisible(visible);
    line_numbers_visible = visible;
}
