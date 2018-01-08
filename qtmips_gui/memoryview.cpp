#include "memoryview.h"

MemoryView::MemoryView(QWidget *parent) : QWidget(parent) {
    layout = new QVBoxLayout(this);

    frame = new QFrame(this);
    frame->setFrameShadow(QFrame::Sunken);
    frame->setFrameShape(QFrame::StyledPanel);
    frame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    // TODO
    layout->addWidget(frame);

    go_edit = new QLineEdit(this);
    go_edit->setText("0x00000000");
    go_edit->setInputMask("\\0\\xHHHHHHHH");
    layout->addWidget(go_edit);
    connect(go_edit, SIGNAL(editingFinished()), this, SLOT(go_edit_finish()));

}

MemoryView::~MemoryView() {
    delete go_edit;
    delete frame;
    delete layout;
}

void MemoryView::set_center(std::uint32_t address) {
    center_addr = address;
    // TODO update view
}

std::uint32_t MemoryView::center() {
    return center_addr;
}

void MemoryView::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    // TODO
}

void MemoryView::wheelEvent(QWheelEvent *event) {
    // TODO
}

void MemoryView::go_edit_finish() {
    // TODO
}
