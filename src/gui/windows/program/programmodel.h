#ifndef PROGRAMMODEL_H
#define PROGRAMMODEL_H

#include "machine/machine.h"

#include <QAbstractTableModel>
#include <QFont>

class ProgramModel : public QAbstractTableModel {
    Q_OBJECT

    using Super = QAbstractTableModel;

public:
    explicit ProgramModel(QObject *parent);
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

    [[nodiscard]] inline const QFont *getFont() const {
        return &data_font;
    }

    [[nodiscard]] inline machine::Address getIndex0Offset() const {
        return index0_offset;
    }

    [[nodiscard]] static inline unsigned int cellSizeBytes() {
        return 4;
    }
    inline bool get_row_address(machine::Address &address, int row) const {
        address = index0_offset + row * cellSizeBytes();
        return address >= index0_offset;
    }
    inline bool get_row_for_address(int &row, machine::Address address) const {
        if (address < index0_offset) {
            row = -1;
            return false;
        }
        row = (address - index0_offset) / cellSizeBytes();
        if (((address - index0_offset) > 0x80000000) || row > rowCount()) {
            row = rowCount();
            return false;
        }
        return true;
    }

    enum StageAddress {
        STAGEADDR_FETCH,
        STAGEADDR_DECODE,
        STAGEADDR_EXECUTE,
        STAGEADDR_MEMORY,
        STAGEADDR_WRITEBACK,
        STAGEADDR_COUNT,
    };

signals:
    void report_error(QString error);
public slots:
    void setup(machine::Machine *machine);
    void check_for_updates();
    void toggle_hw_break(const QModelIndex &index);
    void update_stage_addr(uint stage, machine::Address addr);
    void update_all();

private:
    [[nodiscard]] const machine::FrontendMemory *mem_access() const;
    [[nodiscard]] machine::FrontendMemory *mem_access_rw() const;
    machine::Address index0_offset;
    QFont data_font;
    machine::Machine *machine;
    uint32_t memory_change_counter;
    uint32_t cache_program_change_counter;
    machine::Address stage_addr[STAGEADDR_COUNT] {};
    bool stages_need_update;
};

#endif // PROGRAMMODEL_H
