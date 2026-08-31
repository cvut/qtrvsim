#include "memorymodel.h"

#include <QBrush>

using ae = machine::AccessEffects; // For enum values, the type is obvious from context.

MemoryModel::MemoryModel(QObject *parent) : Super(parent), data_font("Monospace") {
    cell_size = CELLSIZE_WORD;
    cells_per_row = 1;
    index0_offset = machine::Address::null();
    data_font.setStyleHint(QFont::TypeWriter);
    machine = nullptr;
    memory_change_counter = 0;
    cache_data_change_counter = 0;
    tlb_change_counter = 0;
    last_priv_level = machine::CSR::PrivilegeLevel::MACHINE;
    last_asid = 0;
    mem_acc_at_level = MEM_ACC_DIRECT;
}

const machine::FrontendMemory *MemoryModel::mem_access() const {
    if (machine == nullptr) { return nullptr; }
    if (machine->memory_data_bus() != nullptr) { return machine->memory_data_bus(); }
    // Direct access to memory is not allowed, data bus must be used. At least a
    // trivial one. If this occurred, there is a misconfigured machine.
    throw std::logic_error("No memory available on machine. This is a bug, please report it.");
}

machine::FrontendMemory *MemoryModel::mem_access_rw() const {
    if (machine == nullptr) { return nullptr; }
    if (machine->memory_data_bus_rw() != nullptr) { return machine->memory_data_bus_rw(); }
    // Direct access to memory is not allowed, data bus must be used. At least a
    // trivial one. If this occurred, there is a misconfigured machine.
    throw std::logic_error("No memory available on machine. This is u bug, please report it.");
}

const machine::FrontendMemory *MemoryModel::mem_access_at_level() const {
    if (!machine) return nullptr;
    if (mem_acc_at_level == MEM_ACC_AS_CPU && machine->config().get_vm_enabled()) {
        return machine->get_tlb_data();
    } else if (mem_acc_at_level >= MEM_ACC_CACHED && machine->cache_data()) {
        return machine->cache_data();
    } else {
        return mem_access();
    }
}

machine::FrontendMemory *MemoryModel::mem_access_at_level_rw() const {
    if (!machine) return nullptr;
    if (mem_acc_at_level == MEM_ACC_AS_CPU && machine->config().get_vm_enabled()) {
        return machine->get_tlb_data_rw();
    } else if (mem_acc_at_level >= MEM_ACC_CACHED && machine->cache_data_rw()) {
        return machine->cache_data_rw();
    } else {
        return mem_access_rw();
    }
}

int MemoryModel::rowCount(const QModelIndex & /*parent*/) const {
    return 750;
}

int MemoryModel::columnCount(const QModelIndex & /*parent*/) const {
    return cells_per_row + 1;
}

QVariant MemoryModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation == Qt::Horizontal) {
        if (role == Qt::DisplayRole) {
            if (section == 0) {
                return tr("Address");
            } else {
                uint64_t addr = (section - 1) * cellSizeBytes();
                QString ret = "+" + QString::number(addr, 10);
                return ret;
            }
        }
    }
    return Super::headerData(section, orientation, role);
}

QVariant MemoryModel::data(const QModelIndex &index, int role) const {
    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        QString s, t;
        machine::Address address;
        uint32_t data;
        const machine::FrontendMemory *mem = nullptr;
        if (!get_row_address(address, index.row())) { return QString(""); }
        if (index.column() == 0) {
            t = QString::number(address.get_raw(), 16);
            s.fill('0', 8 - t.count());
            return { QString("0x") + s + t };
        }
        if (machine == nullptr) { return QString(""); }
        mem = mem_access_at_level();
        if (mem == nullptr) { return QString(""); }
        address += cellSizeBytes() * (index.column() - 1);
        machine::AccessMode mode = machine::Core::make_access_mode(
            machine->core()->get_state(), machine::AccessOp::READ);
        machine::AddressWithMode awm(address, mode);
        if (address < index0_offset) { return QString(""); }
        try {
            switch (cell_size) {
            case CELLSIZE_BYTE: data = mem->read_u8(awm, ae::INTERNAL); break;
            case CELLSIZE_HWORD: data = mem->read_u16(awm, ae::INTERNAL); break;
            default:
            case CELLSIZE_WORD: data = mem->read_u32(awm, ae::INTERNAL); break;
            }

            t = QString::number(data, 16);
            s.fill('0', cellSizeBytes() * 2 - t.count());
            t = s + t;
        } catch (machine::SimulatorExceptionPageFault &e) { t = "--"; }
#if 0
        machine::LocationStatus loc_stat = machine::LOCSTAT_NONE;
        if (machine->cache_data() != nullptr) {
            loc_stat = machine->cache_data()->location_status(address);
            if (loc_stat & machine::LOCSTAT_DIRTY)
                t += " D";
            else if (loc_stat & machine::LOCSTAT_CACHED)
                t += " C";
        }
#endif
        return t;
    }
    if (role == Qt::BackgroundRole) {
        machine::Address address;
        if (!get_row_address(address, index.row()) || machine == nullptr || index.column() == 0) {
            return {};
        }
        address += cellSizeBytes() * (index.column() - 1);
        machine::AccessMode mode = machine::Core::make_access_mode(
            machine->core()->get_state(), machine::AccessOp::READ);
        machine::AddressWithMode awm(address, mode);
        if (address < index0_offset) { return {}; }
        machine::LocationStatus loc_stat;
        const machine::FrontendMemory *mem = nullptr;
        if (mem_acc_at_level < MEM_ACC_CACHED && machine->cache_data() != nullptr) {
            mem = machine->cache_data();
        } else {
            mem = mem_access_at_level();
        }
        if (mem == nullptr) { return {}; }
        try {
            loc_stat = mem->location_status(awm);
        } catch (machine::SimulatorExceptionPageFault &e) {
            QBrush bgd(Qt::darkRed);
            return bgd;
        }
        if (loc_stat & machine::LOCSTAT_DIRTY) {
            QBrush bgd(Qt::yellow);
            return bgd;
        } else if (loc_stat & machine::LOCSTAT_CACHED) {
            QBrush bgd(Qt::lightGray);
            return bgd;
        }
        return {};
    }
    if (role == Qt::FontRole) { return data_font; }
    return {};
}

void MemoryModel::setup(machine::Machine *machine) {
    this->machine = machine;
    if (machine != nullptr) {
        connect(machine, &machine::Machine::post_tick, this, &MemoryModel::check_for_updates);
    }
    if (mem_access() != nullptr) {
        connect(
            mem_access(), &machine::FrontendMemory::external_change_notify, this,
            &MemoryModel::check_for_updates);
    }
    emit update_all();
    emit setup_done();
}

void MemoryModel::setCellsPerRow(unsigned int cells) {
    beginResetModel();
    cells_per_row = cells;
    endResetModel();
}

void MemoryModel::set_cell_size(int index) {
    beginResetModel();
    cell_size = (enum MemoryCellSize)index;
    index0_offset -= index0_offset.get_raw() % cellSizeBytes();
    endResetModel();
    emit cell_size_changed();
}

void MemoryModel::update_all() {
    const machine::FrontendMemory *mem;
    mem = mem_access();
    if (mem != nullptr) {
        memory_change_counter = mem->get_change_counter();
        if (machine->cache_data() != nullptr) {
            cache_data_change_counter = machine->cache_data()->get_change_counter();
        }
        if (mem_acc_at_level == MEM_ACC_AS_CPU && machine->config().get_vm_enabled()
            && machine->get_tlb_data() != nullptr) {
            tlb_change_counter = machine->get_tlb_data()->get_change_counter();
            auto state = machine->core()->get_state();
            last_priv_level = state.current_privilege();
            last_asid = state.current_asid();
        }
    }
    emit dataChanged(index(0, 0), index(rowCount() - 1, columnCount() - 1));
}

void MemoryModel::check_for_updates() {
    bool need_update = false;
    const machine::FrontendMemory *mem;
    mem = mem_access();
    if (mem == nullptr) { return; }

    if (memory_change_counter != mem->get_change_counter()) { need_update = true; }
    if (machine->cache_data() != nullptr) {
        if (cache_data_change_counter != machine->cache_data()->get_change_counter()) {
            need_update = true;
        }
        if (mem_acc_at_level == MEM_ACC_AS_CPU && machine->config().get_vm_enabled()
            && machine->get_tlb_data() != nullptr) {
            if (tlb_change_counter != machine->get_tlb_data()->get_change_counter()) {
                need_update = true;
            }
            auto state = machine->core()->get_state();
            if (last_priv_level != state.current_privilege() || last_asid != state.current_asid()) {
                need_update = true;
            }
        }
    }
    if (!need_update) { return; }
    update_all();
}

bool MemoryModel::adjustRowAndOffset(int &row, machine::Address address) {
    row = rowCount() / 2;
    address -= address.get_raw() % cellSizeBytes();
    uint32_t row_bytes = cells_per_row * cellSizeBytes();
    uint32_t diff = row * row_bytes;
    if (machine::Address(diff) > address) {
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
void MemoryModel::set_access_at_level(int acc_level) {
    beginResetModel();
    mem_acc_at_level = (enum MemoryAccessAtLevel)acc_level;
    endResetModel();
}
Qt::ItemFlags MemoryModel::flags(const QModelIndex &index) const {
    if (index.column() == 0) {
        return QAbstractTableModel::flags(index);
    } else {
        return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
    }
}

bool MemoryModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if (role == Qt::EditRole) {
        bool ok;
        machine::Address address;
        machine::FrontendMemory *mem;
        uint32_t data = value.toString().toULong(&ok, 16);
        if (!ok) { return false; }
        if (!get_row_address(address, index.row())) { return false; }
        if (index.column() == 0 || machine == nullptr) { return false; }
        mem = mem_access_at_level_rw();
        if (mem == nullptr) { return false; }
        address += cellSizeBytes() * (index.column() - 1);
        machine::AccessMode mode = machine::Core::make_access_mode(
            machine->core()->get_state(), machine::AccessOp::READ);
        machine::AddressWithMode awm(address, mode);
        try {
            switch (cell_size) {
            case CELLSIZE_BYTE: mem->write_u8(awm, data, ae::INTERNAL); break;
            case CELLSIZE_HWORD: mem->write_u16(awm, data, ae::INTERNAL); break;
            default:
            case CELLSIZE_WORD: mem->write_u32(awm, data, ae::INTERNAL); break;
            }
        } catch (machine::SimulatorExceptionPageFault &e) {};
    }
    return true;
}
