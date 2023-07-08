#ifndef MEMORYMODEL_H
#define MEMORYMODEL_H

#include "machine/machine.h"

#include <QAbstractTableModel>
#include <QFont>

class MemoryModel : public QAbstractTableModel {
    Q_OBJECT

    using Super = QAbstractTableModel;

public:
    enum MemoryCellSize {
        CELLSIZE_BYTE,
        CELLSIZE_HWORD,
        CELLSIZE_WORD,
    };

    explicit MemoryModel(QObject *parent);
    [[nodiscard]] int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    [[nodiscard]] int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation, int role)
        const override;
    [[nodiscard]] QVariant
    data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    [[nodiscard]] Qt::ItemFlags flags(const QModelIndex &index) const override;
    bool
    setData(const QModelIndex &index, const QVariant &value, int role) override;
    bool adjustRowAndOffset(int &row, machine::Address address);
    void update_all();

    void setCellsPerRow(unsigned int cells);

    [[nodiscard]] inline unsigned int cellsPerRow() const {
        return cells_per_row;
    }

    [[nodiscard]] inline const QFont *getFont() const {
        return &data_font;
    }

    [[nodiscard]] inline machine::Address getIndex0Offset() const {
        return index0_offset;
    }

    [[nodiscard]] inline unsigned int cellSizeBytes() const {
        switch (cell_size) {
        case CELLSIZE_BYTE: return 1;
        case CELLSIZE_HWORD: return 2;
        case CELLSIZE_WORD: return 4;
        }
        return 0;
    }
    inline bool get_row_address(machine::Address &address, int row) const {
        address = index0_offset + (row * cells_per_row * cellSizeBytes());
        return address >= index0_offset;
    }
    inline bool get_row_for_address(int &row, machine::Address address) const {
        if (address < index0_offset) {
            row = -1;
            return false;
        }
        row = (address - index0_offset) / (cells_per_row * cellSizeBytes());
        if ((address - index0_offset > 0x80000000) || row > rowCount()) {
            row = rowCount();
            return false;
        }
        return true;
    }

public slots:
    void setup(machine::Machine *machine);
    void set_cell_size(int index);
    void check_for_updates();
    void cached_access(int cached);

signals:
    void cell_size_changed();
    void setup_done();

private:
    [[nodiscard]] const machine::FrontendMemory *mem_access() const;
    [[nodiscard]] machine::FrontendMemory *mem_access_rw() const;
    enum MemoryCellSize cell_size;
    unsigned int cells_per_row;
    machine::Address index0_offset;
    QFont data_font;
    machine::Machine *machine;
    uint32_t memory_change_counter;
    uint32_t cache_data_change_counter;
    int access_through_cache;
};

#endif // MEMORYMODEL_H
