#include "memorytableview.h"

#include "common/polyfills/qt5/qfontmetrics.h"
#include "hinttabledelegate.h"
#include "memorymodel.h"

#include <QApplication>
#include <QClipboard>
#include <QFontMetrics>
#include <QHeaderView>
#include <QKeyEvent>
#include <QScrollBar>
#include <QtGlobal>

MemoryTableView::MemoryTableView(QWidget *parent, QSettings *settings) : Super(parent) {
    setItemDelegate(new HintTableDelegate(this));
    connect(
        verticalScrollBar(), &QAbstractSlider::valueChanged, this,
        &MemoryTableView::adjust_scroll_pos_check);
    connect(
        this, &MemoryTableView::adjust_scroll_pos_queue, this,
        &MemoryTableView::adjust_scroll_pos_process, Qt::QueuedConnection);
    this->settings = settings;
    initial_address = machine::Address(settings->value("DataViewAddr0", 0).toULongLong());
    adjust_scroll_pos_in_progress = false;
    setTextElideMode(Qt::ElideNone);
}

void MemoryTableView::addr0_save_change(machine::Address val) {
    settings->setValue("DataViewAddr0", qint64(val.get_raw()));
}

void MemoryTableView::adjustColumnCount() {
    auto *m = dynamic_cast<MemoryModel *>(model());
    if (m == nullptr) { return; }

    auto *delegate = dynamic_cast<HintTableDelegate *>(itemDelegate());
    if (delegate == nullptr) { return; }

    if (horizontalHeader()->count() >= 2) {
        QModelIndex idx;
        QFontMetrics fm(*m->getFont());
        idx = m->index(0, 0);

        QStyleOptionViewItem viewOpts;

        initViewItemOption(&viewOpts);

        // int width0_dh = itemDelegate(idx)->sizeHint(viewOptions(),
        // idx).get_width() + 2;
        int width0_dh = delegate->sizeHintForText(viewOpts, idx, "0x00000000").width() + 2;
        horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
        horizontalHeader()->resizeSection(0, width0_dh);

        idx = m->index(0, 1);
        QString t = "";
        t.fill(QChar('0'), m->cellSizeBytes() * 2);
        int width1_dh = delegate->sizeHintForText(viewOpts, idx, t).width() + 2;
        if (width1_dh < QFontMetrics_horizontalAdvance(fm, "+99")) {
            width1_dh = QFontMetrics_horizontalAdvance(fm, "+99");
        }
        horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);
        horizontalHeader()->resizeSection(1, width1_dh);

        int w = verticalHeader()->width() + 4;
        unsigned int cells;
        int width0 = columnWidth(0);
        int width1 = columnWidth(1);
        w = width() - w - width0;
        if (w < width1 + 4) {
            cells = 1;
        } else {
            cells = w / (width1 + 4);
        }
        if (cells != m->cellsPerRow()) { m->setCellsPerRow(cells); }
        for (unsigned int i = 1; i < m->cellsPerRow() + 1; i++) {
            horizontalHeader()->setSectionResizeMode(i, QHeaderView::Fixed);
            horizontalHeader()->resizeSection(i, width1);
        }
        if (!initial_address.is_null()) {
            go_to_address(initial_address);
            initial_address = machine::Address::null();
        }
    }
}

void MemoryTableView::recompute_columns() {
    adjustColumnCount();
}

void MemoryTableView::set_cell_size(int index) {
    machine::Address address;
    int row;
    bool keep_row0 = false;
    auto *m = dynamic_cast<MemoryModel *>(model());
    if (m != nullptr) {
        keep_row0 = m->get_row_address(address, rowAt(0));
        m->set_cell_size(index);
    }
    adjustColumnCount();
    if (keep_row0) {
        m->adjustRowAndOffset(row, address);
        scrollTo(m->index(row, 0), QAbstractItemView::PositionAtTop);
    }
}

void MemoryTableView::adjust_scroll_pos_check() {
    if (!adjust_scroll_pos_in_progress) {
        adjust_scroll_pos_in_progress = true;
        emit adjust_scroll_pos_queue();
    }
}

void MemoryTableView::adjust_scroll_pos_process() {
    adjust_scroll_pos_in_progress = false;
    machine::Address address;
    auto *m = dynamic_cast<MemoryModel *>(model());
    if (m == nullptr) { return; }

    QModelIndex prev_index = currentIndex();
    auto row_bytes = machine::Address(m->cellSizeBytes() * m->cellsPerRow());
    machine::Address index0_offset = m->getIndex0Offset();

    do {
        int row = rowAt(0);
        int prev_row = row;
        if (row < m->rowCount() / 8) {
            if ((row == 0) && (index0_offset < row_bytes) && (!index0_offset.is_null())) {
                m->adjustRowAndOffset(row, machine::Address::null());
            } else if (index0_offset > row_bytes) {
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
    addr0_save_change(address);
    emit address_changed(address);
}

void MemoryTableView::resizeEvent(QResizeEvent *event) {
    auto *m = dynamic_cast<MemoryModel *>(model());
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

void MemoryTableView::go_to_address(machine::Address address) {
    auto *m = dynamic_cast<MemoryModel *>(model());
    int row;
    if (m == nullptr) { return; }
    m->adjustRowAndOffset(row, address);
    scrollTo(m->index(row, 0), QAbstractItemView::PositionAtTop);
    setCurrentIndex(m->index(row, 1));
    addr0_save_change(address);
    emit m->update_all();
}

void MemoryTableView::focus_address(machine::Address address) {
    int row;
    auto *m = dynamic_cast<MemoryModel *>(model());
    if (m == nullptr) { return; }
    if (!m->get_row_for_address(row, address)) { go_to_address(address); }
    if (!m->get_row_for_address(row, address)) { return; }
    setCurrentIndex(m->index(row, 1));
}

void MemoryTableView::keyPressEvent(QKeyEvent *event) {
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
