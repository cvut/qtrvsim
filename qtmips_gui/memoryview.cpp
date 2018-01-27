#include "memoryview.h"

///////////////////////////
// Minimal reserved range in pixels of scroll area (otherwise 10% of height are used)
#define MIN_OFF 10
// Focus point (this is multiplied with height of widget to know position where we want to focus)
#define FOCUS 0.25
// How angle maps to pixels when and scroll is used
#define ANGLE_SCROLL 4
///////////////////////////

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
    connect(up, SIGNAL(clicked(bool)), this, SLOT(prev_section()));
    ctl_layout->addWidget(up);
    down = new QToolButton(ctl_widg);
    down->setArrowType(Qt::DownArrow);
    connect(down, SIGNAL(clicked(bool)), this, SLOT(next_section()));
    ctl_layout->addWidget(down);
}

void MemoryView::setup(machine::QtMipsMachine *machine) {
    memory = (machine == nullptr) ? nullptr : machine->memory();
    reload_content();
}

void MemoryView::set_focus(std::uint32_t address) {
    if (address < addr_0 || (address - addr_0)/4 > (unsigned)memf->widg->count()) {
        // This is outside of loaded area so just move it and reload everything
        addr_0 = address - 4*memf->focussed();
        reload_content();
    } else {
        memf->focus((address - addr_0) / 4);
    }
}

std::uint32_t MemoryView::focus() {
    return addr_0 + 4*memf->focussed();
}

void MemoryView::edit_load_focus() {
    go_edit->setText(QString("0x%1").arg(focus(), 8, 16, QChar('0')));
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
    QString hex = go_edit->text();
    hex.remove(0, 2);

    bool ok;
    std::uint32_t nw = hex.toUInt(&ok, 16);
    if (ok) {
        set_focus(nw);
    } else
        edit_load_focus();
}

void MemoryView::next_section() {
    if (memory == nullptr)
        return;
    set_focus(memory->next_allocated(focus()));
}

void MemoryView::prev_section() {
    if (memory == nullptr)
        return;
    set_focus(memory->prev_allocated(focus()));
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
    content_y = (FOCUS*height()) - widg->row_size()*i - widg->row_size()/2;
    viewport()->move(0, content_y);
    viewport()->repaint(0, content_y, width(), height());
    check_update();
}

// Calculate which row is in focus at the moment
unsigned MemoryView::Frame::focussed() {
    int h = (FOCUS*height() - content_y) / widg->row_size();
    return qMax(h, 0);
}

// This verifies that we are not scrolled too far away down or up and that we have enought height
// We make 10% of height as buffer zone with fixed minimum in pixels
void MemoryView::Frame::check_update() {
    int hpart = qMax(height()/10, MIN_OFF);
    int req_height = height() + 2*hpart;

    while (!((content_y <= -hpart) && (content_y >= -2*hpart)) || (widg->height() <= req_height)) {
        int row_h = widg->row_size();
        // Calculate how many we need and how much we need to move and update content accordingly
        int count = (req_height / row_h) + 1;
        int shift = (content_y + hpart + hpart/2)/row_h;
        mv->update_content(count * widg->columns(), shift * widg->columns());
        // Move and resize widget
        content_y -= shift * row_h;
        widg->setGeometry(0, content_y, width(), widg->heightForWidth(width()));
    }

    mv->edit_load_focus();
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
        content_y += pix.ry();
    else if (!ang.isNull())
        content_y += ang.ry() * ANGLE_SCROLL;

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
