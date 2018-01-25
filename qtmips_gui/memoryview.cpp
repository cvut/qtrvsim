#include "memoryview.h"

///////////////////////////
// Minimal reserved range in pixels of scroll area (otherwise 10% of height are used)
#define MIN_OFF 10
///////////////////////////

#include <iostream>
using namespace std;

MemoryView::MemoryView(QWidget *parent) : QWidget(parent) {
    memory = nullptr;
    addr_0 = 0;

    layout = new QVBoxLayout(this);

    memf= new Frame(this);
    layout->addWidget(memf);


    ctl_widg = new QWidget(this);
    layout->addWidget(ctl_widg);

    ctl_layout = new QHBoxLayout(ctl_widg);
    go_edit = new QLineEdit(ctl_widg);
    go_edit->setText("0x00000000");
    go_edit->setInputMask("\\0\\xHHHHHHHH");
    ctl_layout->addWidget(go_edit);
    connect(go_edit, SIGNAL(editingFinished()), this, SLOT(go_edit_finish()));
    up = new QToolButton(ctl_widg);
    up->setArrowType(Qt::UpArrow);
    ctl_layout->addWidget(up);
    down = new QToolButton(ctl_widg);
    down->setArrowType(Qt::DownArrow);
    ctl_layout->addWidget(down);
}

void MemoryView::setup(machine::QtMipsMachine *machine) {
    memory = (machine == nullptr) ? nullptr : machine->memory();
    reload_content();
}

void MemoryView::set_focus(std::uint32_t address) {
    // TODO center
    // TODO update view
}

std::uint32_t MemoryView::focus() {
    // TODO
    return 0;
}

void MemoryView::reload_content() {
    int count = memf->widg->count();
    memf->widg->clearRows();
    update_content(count, 0);
}

void MemoryView::update_content(int count, int shift) {
    if (abs(shift) >= memf->widg->count()) {
        // This shifts more than we have so just reload whole content
        memf->widg->clearRows();
        addr_0 += 4*shift;
        for (int i = 0; i <= count; i++)
            memf->widg->addRow(row_widget(addr_0 + 4*i, memf->widg));
        return;
    }

    int diff = count - memf->widg->count();
    int d_b = shift;
    int d_e = diff - shift;
    cout << "count:" << memf->widg->count() << " tocount:" << count << " d_b:" << d_b << " d_e:" << d_e << endl;

    if (d_b > 0)
        for (int i = 0; i < d_b; i++) {
            addr_0 -= 4;
            memf->widg->insertRow(row_widget(addr_0, memf->widg), 0);
        }
    else
        for (int i = 0; i > d_b; i--) {
            addr_0 += 4;
            memf->widg->removeRow(0);
        }
    if (d_e > 0)
        for (int i = 0; i < d_e; i++)
            memf->widg->addRow(row_widget(addr_0 + 4*memf->widg->count(), memf->widg));
    else
        for (int i = 0; i > d_e; i--)
            memf->widg->removeRow(memf->widg->count() - 1);
}

void MemoryView::go_edit_finish() {
    // TODO
}

MemoryView::Frame::Frame(MemoryView *parent) : QAbstractScrollArea(parent) {
    mv = parent;
    content_y = -3*MIN_OFF/2; // When this is initialized the width is 0 so this uses just min to inialize it

    widg = new StaticTable(this);
    setViewport(widg);

    setFrameShadow(QFrame::Sunken);
    setFrameShape(QFrame::StyledPanel);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setContentsMargins(0, 0, 0, 0);
}

void MemoryView::Frame::focus(unsigned i) {
    // TODO
}

unsigned MemoryView::Frame::focussed() {
    // TODO
    return 0;
}

// This verifies that we are not scrolled too far away down or up and that we have enought height
// We make 10% of height as buffer zone with fixed minimum in pixels
void MemoryView::Frame::check_update() {
    int hpart = qMax(height()/10, MIN_OFF);
    int req_height = height() + 2*hpart;

    while (!((content_y <= -hpart) && (content_y >= -2*hpart)) || (widg->height() < req_height)) {
        cout << "widg.h:" << widg->height() << " req_h:" << req_height << endl;
        cout << "content_y:" << content_y << " hpart:" << hpart << endl;
        int row_h = widg->row_size();
        cout << "row_h:" << row_h << " columns:" << widg->columns() << endl;
        // Calculate how many we need and how much we need to move and update content accordingly
        int count = (req_height / row_h) + 1;
        int shift = (content_y + hpart + hpart/2)/row_h;
        mv->update_content(count * widg->columns(), shift * widg->columns());
        cout << "count:" << count << endl;
        // Move and resize widget
        content_y -= shift * row_h;
        widg->setGeometry(0, content_y, width(), widg->heightForWidth(width()));
    }
}

void MemoryView::Frame::resizeEvent(QResizeEvent *e) {
    QAbstractScrollArea::resizeEvent(e);
    widg->setGeometry(0, content_y, e->size().width(), widg->heightForWidth(e->size().width()));
    check_update();
}

void MemoryView::Frame::wheelEvent(QWheelEvent *e) {
    QPoint pix = e->pixelDelta();
    QPoint ang = e->angleDelta();

    if (!pix.isNull())
        content_y += e->pixelDelta().ry();
    // TODO angle scroll

    // TODO smooth scroll
    viewport()->move(0, content_y);
    viewport()->repaint(0, content_y, width(), height());

    check_update();
}

bool MemoryView::Frame::viewportEvent(QEvent *e) {
    bool p = QAbstractScrollArea::viewportEvent(e);
    // Pass paint event to viewport widget
    if (e->type() == QEvent::Paint)
        p = false;
    return p;
}
