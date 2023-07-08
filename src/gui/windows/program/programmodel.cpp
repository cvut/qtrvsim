#include "programmodel.h"

#include <QtGui/qbrush.h>

using ae = machine::AccessEffects; // For enum values, the type is obvious from context.

ProgramModel::ProgramModel(QObject *parent) : Super(parent), data_font("Monospace") {
    index0_offset = machine::Address::null();
    data_font.setStyleHint(QFont::TypeWriter);
    machine = nullptr;
    memory_change_counter = 0;
    cache_program_change_counter = 0;
    for (auto &i : stage_addr) {
        i = machine::STAGEADDR_NONE;
    }
    stages_need_update = false;
}

const machine::FrontendMemory *ProgramModel::mem_access() const {
    if (machine == nullptr) { return nullptr; }
    if (machine->memory_data_bus() != nullptr) { return machine->memory_data_bus(); }
    throw std::logic_error("Use of backend memory in frontend."); // TODO
    //    return machine->memory();
}

machine::FrontendMemory *ProgramModel::mem_access_rw() const {
    if (machine == nullptr) { return nullptr; }
    if (machine->memory_data_bus_rw() != nullptr) { return machine->memory_data_bus_rw(); }
    throw std::logic_error("Use of backend memory in frontend."); // TODO
    //    return machine->memory_rw();
}

int ProgramModel::rowCount(const QModelIndex & /*parent*/) const {
    return 750;
}

int ProgramModel::columnCount(const QModelIndex & /*parent*/) const {
    return 4;
}
QVariant ProgramModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation == Qt::Horizontal) {
        if (role == Qt::DisplayRole) {
            switch (section) {
            case 0: return tr("BP");
            case 1: return tr("Address");
            case 2: return tr("Code");
            case 3: return tr("Instruction");
            default: return tr("");
            }
        }
    }
    return Super::headerData(section, orientation, role);
}

QVariant ProgramModel::data(const QModelIndex &index, int role) const {
    const machine::FrontendMemory *mem;

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        QString s, t;
        machine::Address address;
        if (!get_row_address(address, index.row())) { return QString(""); }

        if (index.column() == 1) {
            t = QString::number(address.get_raw(), 16);
            s.fill('0', 8 - t.count());
            return "0x" + s + t;
        }

        mem = mem_access();
        if (mem == nullptr) { return QString(" "); }

        machine::Instruction inst(mem->read_u32(address));

        switch (index.column()) {
        case 0:
            if (machine->is_hwbreak(address)) {
                return QString("B");
            } else {
                return QString(" ");
            }
        case 2:
            t = QString::number(inst.data(), 16);
            s.fill('0', 8 - t.count());
            return s + t;
        case 3: return inst.to_str(address);
        default: return tr("");
        }
    }
    if (role == Qt::BackgroundRole) {
        machine::Address address;
        if (!get_row_address(address, index.row()) || machine == nullptr) { return {}; }
        if (index.column() == 2 && machine->cache_program() != nullptr) {
            machine::LocationStatus loc_stat;
            loc_stat = machine->cache_program()->location_status(address);
            if (loc_stat & machine::LOCSTAT_CACHED) {
                QBrush bgd(Qt::lightGray);
                return bgd;
            }
        } else if (index.column() == 0 && machine->is_hwbreak(address)) {
            QBrush bgd(Qt::red);
            return bgd;
        } else if (index.column() == 3) {
            if (address == stage_addr[STAGEADDR_WRITEBACK]) {
                QBrush bgd(QColor(255, 173, 230));
                return bgd;
            } else if (address == stage_addr[STAGEADDR_MEMORY]) {
                QBrush bgd(QColor(173, 255, 229));
                return bgd;
            } else if (address == stage_addr[STAGEADDR_EXECUTE]) {
                QBrush bgd(QColor(193, 255, 173));
                return bgd;
            } else if (address == stage_addr[STAGEADDR_DECODE]) {
                QBrush bgd(QColor(255, 212, 173));
                return bgd;
            } else if (address == stage_addr[STAGEADDR_FETCH]) {
                QBrush bgd(QColor(255, 173, 173));
                return bgd;
            }
        }
        return {};
    }
    if (role == Qt::FontRole) { return data_font; }
    if (role == Qt::TextAlignmentRole) {
        if (index.column() == 0) { return Qt::AlignCenter; }
        return Qt::AlignLeft;
    }
    return {};
}

void ProgramModel::setup(machine::Machine *machine) {
    this->machine = machine;
    for (auto &i : stage_addr) {
        i = machine::STAGEADDR_NONE;
    }
    if (machine != nullptr) {
        connect(machine, &machine::Machine::post_tick, this, &ProgramModel::check_for_updates);
    }
    if (mem_access() != nullptr) {
        connect(
            mem_access(), &machine::FrontendMemory::external_change_notify, this,
            &ProgramModel::check_for_updates);
    }
    emit update_all();
}

void ProgramModel::update_all() {
    const machine::FrontendMemory *mem;
    mem = mem_access();
    if (mem != nullptr) {
        memory_change_counter = mem->get_change_counter();
        if (machine->cache_program() != nullptr) {
            cache_program_change_counter = machine->cache_program()->get_change_counter();
        }
    }
    stages_need_update = false;
    emit dataChanged(index(0, 0), index(rowCount() - 1, columnCount() - 1));
}

void ProgramModel::check_for_updates() {
    bool need_update = stages_need_update;
    const machine::FrontendMemory *mem;
    mem = mem_access();
    if (mem == nullptr) { return; }

    if (memory_change_counter != mem->get_change_counter()) { need_update = true; }
    if (machine->cache_data() != nullptr) {
        if (cache_program_change_counter != machine->cache_program()->get_change_counter()) {
            need_update = true;
        }
    }
    if (!need_update) { return; }
    update_all();
}

bool ProgramModel::adjustRowAndOffset(int &row, machine::Address address) {
    row = rowCount() / 2;
    address -= address.get_raw() % cellSizeBytes();
    uint32_t row_bytes = cellSizeBytes();
    uint32_t diff = row * row_bytes;
    if (diff > address.get_raw()) {
        row = address.get_raw() / row_bytes;
        if (row == 0) {
            index0_offset = machine::Address::null();
        } else {
            index0_offset = address - row * row_bytes;
        }
    } else {
        index0_offset = address - diff;
    }
    return get_row_for_address(row, address);
}

void ProgramModel::toggle_hw_break(const QModelIndex &index) {
    machine::Address address;
    if (index.column() != 0 || machine == nullptr) { return; }

    if (!get_row_address(address, index.row())) { return; }

    if (machine->is_hwbreak(address)) {
        machine->remove_hwbreak(address);
    } else {
        machine->insert_hwbreak(address);
    }
    update_all();
}

Qt::ItemFlags ProgramModel::flags(const QModelIndex &index) const {
    if (index.column() != 2 && index.column() != 3) {
        return QAbstractTableModel::flags(index);
    } else {
        return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
    }
}

bool ProgramModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if (role == Qt::EditRole) {
        bool ok;
        QString error;
        machine::Address address;
        uint32_t data;
        machine::FrontendMemory *mem;
        if (!get_row_address(address, index.row())) { return false; }
        if (index.column() == 0 || machine == nullptr) { return false; }
        mem = mem_access_rw();
        if (mem == nullptr) { return false; }
        switch (index.column()) {
        case 2:
            data = value.toString().toULong(&ok, 16);
            if (!ok) { return false; }
            mem->write_u32(address, data, ae::INTERNAL);
            break;
        case 3: try { machine::Instruction::code_from_string(&data, 4, value.toString(), address);
            } catch (machine::Instruction::ParseError &e) {
                emit report_error(tr("instruction 1 parse error - %2.").arg(e.message));
                return false;
            }
            mem->write_u32(address, data, ae::INTERNAL);
            break;
        default: return false;
        }
    }
    return true;
}

void ProgramModel::update_stage_addr(uint stage, machine::Address addr) {
    if (stage < STAGEADDR_COUNT) {
        if (stage_addr[stage] != addr) {
            stage_addr[stage] = addr;
            stages_need_update = true;
        }
    }
}
