#include "programtableview.h"

#include "hinttabledelegate.h"
#include "programmodel.h"

#include <QApplication>
#include <QClipboard>
#include <QHeaderView>
#include <QKeyEvent>
#include <QScrollBar>
#include <QtGlobal>

ProgramTableView::ProgramTableView(QWidget *parent, QSettings *settings) : Super(parent) {
    setItemDelegate(new HintTableDelegate(this));
    connect(
        verticalScrollBar(), &QAbstractSlider::valueChanged, this,
        &ProgramTableView::adjust_scroll_pos_check);
    connect(
        this, &ProgramTableView::adjust_scroll_pos_queue, this,
        &ProgramTableView::adjust_scroll_pos_process, Qt::QueuedConnection);
    this->settings = settings;
    initial_address = machine::Address(settings->value("ProgramViewAddr0", 0).toULongLong());
    adjust_scroll_pos_in_progress = false;
    need_addr0_save = false;
    setTextElideMode(Qt::ElideNone);
}

void ProgramTableView::addr0_save_change(machine::Address val) {
    need_addr0_save = false;
    settings->setValue("ProgramViewAddr0", qint64(val.get_raw()));
}

void ProgramTableView::adjustColumnCount() {
    QModelIndex idx;

    auto *m = dynamic_cast<ProgramModel *>(model());

    if (m == nullptr) { return; }

    auto *delegate = dynamic_cast<HintTableDelegate *>(itemDelegate());
    if (delegate == nullptr) { return; }

    QStyleOptionViewItem viewOpts;

    initViewItemOption(&viewOpts);

    int totwidth = 0;
    idx = m->index(0, 0);
    auto cwidth_dh0 = delegate->sizeHintForText(viewOpts, idx, "BP").width() + 2;
    horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    horizontalHeader()->resizeSection(0, cwidth_dh0);
    totwidth += cwidth_dh0;

    idx = m->index(0, 1);
    auto cwidth_dh1 = delegate->sizeHintForText(viewOpts, idx, "0x00000000").width() + 2;
    horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);
    horizontalHeader()->resizeSection(1, cwidth_dh1);
    totwidth += cwidth_dh1;

    idx = m->index(0, 2);
    auto cwidth_dh2 = delegate->sizeHintForText(viewOpts, idx, "00000000").width() + 2;
    horizontalHeader()->setSectionResizeMode(2, QHeaderView::Fixed);
    horizontalHeader()->resizeSection(2, cwidth_dh2);
    totwidth += cwidth_dh2;

    horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    idx = m->index(0, 3);
    totwidth += delegate->sizeHintForText(viewOpts, idx, "BEQ $18, $17, 0x00000258").width() + 2;
    totwidth += verticalHeader()->width();
    setColumnHidden(2, totwidth > width());
    setColumnHidden(1, totwidth - cwidth_dh2 > width());
    setColumnHidden(0, totwidth - cwidth_dh2 - cwidth_dh1 > width());

    if (!initial_address.is_null()) {
        go_to_address(initial_address);
        initial_address = machine::Address::null();
    }
}

void ProgramTableView::adjust_scroll_pos_check() {
    if (!adjust_scroll_pos_in_progress) {
        adjust_scroll_pos_in_progress = true;
        emit adjust_scroll_pos_queue();
    }
}

void ProgramTableView::adjust_scroll_pos_process() {
    adjust_scroll_pos_in_progress = false;
    machine::Address address;
    auto *m = dynamic_cast<ProgramModel *>(model());
    if (m == nullptr) { return; }

    QModelIndex prev_index = currentIndex();
    auto row_bytes = machine::Address(ProgramModel::cellSizeBytes());
    machine::Address index0_offset = m->getIndex0Offset();

    do {
        int row = rowAt(0);
        int prev_row = row;
        if (row < m->rowCount() / 8) {
            if ((row == 0) && (index0_offset < row_bytes) && (!index0_offset.is_null())) {
                m->adjustRowAndOffset(row, machine::Address::null());
            } else if (index0_offset >= row_bytes) {
                m->get_row_address(address, row);
                m->adjustRowAndOffset(row, address);
            } else {
                break;
            }
        } else if (row > m->rowCount() - m->rowCount() / 8) {
            m->get_row_address(address, row);
            m->adjustRowAndOffset(row, address);
        } else {
            break;
        }
        scrollTo(m->index(row, 0), QAbstractItemView::PositionAtTop);
        setCurrentIndex(m->index(prev_index.row() + row - prev_row, prev_index.column()));
        emit m->update_all();
    } while (false);
    m->get_row_address(address, rowAt(0));
    if (need_addr0_save) { addr0_save_change(address); }
    emit address_changed(address.get_raw());
}

void ProgramTableView::resizeEvent(QResizeEvent *event) {
    auto *m = dynamic_cast<ProgramModel *>(model());
    machine::Address address;
    bool keep_row0 = false;

    if (m != nullptr) {
        if (initial_address.is_null()) {
            keep_row0 = m->get_row_address(address, rowAt(0));
        } else {
            address = initial_address;
        }
    }
    Super::resizeEvent(event);
    adjustColumnCount();
    if (keep_row0) {
        initial_address = machine::Address::null();
        go_to_address(address);
    }
}

void ProgramTableView::go_to_address_priv(machine::Address address) {
    auto *m = dynamic_cast<ProgramModel *>(model());
    int row;
    if (m == nullptr) { return; }
    m->adjustRowAndOffset(row, address);
    scrollTo(m->index(row, 0), QAbstractItemView::PositionAtTop);
    setCurrentIndex(m->index(row, 1));
    if (need_addr0_save) { addr0_save_change(address); }
    emit m->update_all();
}

void ProgramTableView::go_to_address(machine::Address address) {
    need_addr0_save = true;
    go_to_address_priv(address);
}

void ProgramTableView::focus_address(machine::Address address) {
    int row;
    auto *m = dynamic_cast<ProgramModel *>(model());
    if (m == nullptr) { return; }
    if (!m->get_row_for_address(row, address)) { go_to_address_priv(address); }
    if (!m->get_row_for_address(row, address)) { return; }
    setCurrentIndex(m->index(row, 3));
}

void ProgramTableView::focus_address_with_save(machine::Address address) {
    need_addr0_save = true;
    focus_address(address);
}

void ProgramTableView::keyPressEvent(QKeyEvent *event) {
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
