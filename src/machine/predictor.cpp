#include "predictor.h"

LOG_CATEGORY("machine.BranchPredictor");

using namespace machine;

const char *machine::int_to_bit_string(const uint64_t value, const uint8_t size) {
    std::string string = "";
    for (int8_t i = size - 1; i >= 0; i--) {
        string.append((value >> i) & 0x1 ? "1" : "0");
    }
    return string.c_str();
}

const char *machine::branch_result_to_string(const BranchResult result) {
    if (result == BranchResult::NOT_TAKEN) {
        return "NOT TAKEN";
    } else if (result == BranchResult::TAKEN) {
        return "TAKEN";
    } else {
        return "";
    }
}

const char *machine::predictor_state_to_string(const PredictorState state) {
    if (state == PredictorState::NOT_TAKEN) {
        return "NT";
    } else if (state == PredictorState::TAKEN) {
        return "T";
    } else if (state == PredictorState::STRONGLY_NOT_TAKEN) {
        return "SNT";
    } else if (state == PredictorState::WEAKLY_NOT_TAKEN) {
        return "WNT";
    } else if (state == PredictorState::WEAKLY_TAKEN) {
        return "WT";
    } else if (state == PredictorState::STRONGLY_TAKEN) {
        return "ST";
    } else {
        return "";
    }
}

QString machine::addr_to_hex_str(const machine::Address address) {
    QString hex_addr, zero_padding;
    hex_addr = QString::number(address.get_raw(), 16);
    zero_padding.fill('0', 8 - hex_addr.count());
    return "0x" + zero_padding + hex_addr;
}

/////////////////////////////////
// BranchHistoryRegister class //
/////////////////////////////////

// Constructor
BranchHistoryRegister::BranchHistoryRegister(const uint8_t number_of_bits)
    : number_of_bits(init_number_of_bits(number_of_bits))
    , register_mask(init_register_mask(number_of_bits)) {}

// Destructor
BranchHistoryRegister::~BranchHistoryRegister() {}

// Init helper function to check the number of bits
uint8_t BranchHistoryRegister::init_number_of_bits(const uint8_t number_of_bits) const {
    if (number_of_bits > PREDICTOR_MAX_TABLE_BITS) {
        WARN(
            "Number of BHR bits (%u) was larger than %u during init", number_of_bits,
            PREDICTOR_MAX_TABLE_BITS);
        return PREDICTOR_MAX_TABLE_BITS;
    }
    return number_of_bits;
}

// Init helper function to create the register mask used for masking unused bits
uint16_t BranchHistoryRegister::init_register_mask(const uint8_t number_of_bits) const {
    if (number_of_bits >= PREDICTOR_MAX_TABLE_BITS) { return ~0x0; }
    if (number_of_bits == 0) { return 0x0; }
    return (1 << number_of_bits) - 1;
}

uint8_t BranchHistoryRegister::get_number_of_bits() const {
    return number_of_bits;
}

uint16_t BranchHistoryRegister::get_register_mask() const {
    return register_mask;
}

uint16_t BranchHistoryRegister::get_value() const {
    return value;
}

// Pushes new value into the register
void BranchHistoryRegister::update(const BranchResult result) {
    // Shift all bits to the left
    value = value << 1;

    // Add the result as the new least significant bit
    // By default the new LSB is 0, only set to 1 if branch was taken
    if (result == BranchResult::TAKEN) { value |= 0x1; }

    // Set all bits outside of the scope of the register to zero
    value = value & register_mask;

    emit updated(number_of_bits, value);
}

/////////////////////////////
// BranchTargetTable class //
/////////////////////////////

// Constructor
BranchTargetTable::BranchTargetTable(uint8_t number_of_bits)
    : number_of_bits(init_number_of_bits(number_of_bits)) {
    data.resize(qPow(2, number_of_bits));
}

// Destructor
BranchTargetTable::~BranchTargetTable() {}

uint8_t BranchTargetTable::init_number_of_bits(const uint8_t number_of_bits) const {
    if (number_of_bits > PREDICTOR_MAX_TABLE_BITS) {
        WARN(
            "Number of BTT bits (%u) was larger than %u during init", number_of_bits,
            PREDICTOR_MAX_TABLE_BITS);
        return PREDICTOR_MAX_TABLE_BITS;
    }
    return number_of_bits;
}

uint8_t BranchTargetTable::get_number_of_bits() const {
    return number_of_bits;
}

void BranchTargetTable::print() const {
    LOG("----- BTT -----");
    for (const BranchTargetTableEntry &entry : data) {
        LOG("INSTR: %3lu, TARGET: %3lu", entry.instruction_address.get_raw(),
            entry.target_address.get_raw());
    }
    LOG("---------------");
}

// Find provided address in the BTT and return its target, return 0 address if not found
Address BranchTargetTable::get_target_address(const Address instruction_address) const {
    for (const BranchTargetTableEntry &entry : data) {
        if (entry.instruction_address == instruction_address) { return entry.target_address; }
    }
    return Address::null();
}

// Check if instruction address is in the BTT
bool BranchTargetTable::is_address_in_table(const Address instruction_address) const {
    return !(get_target_address(instruction_address).is_null());
}

// Update BTT entry at index with given values
void BranchTargetTable::update(
    const uint16_t index,
    const Address instruction_address,
    const Address target_address) {
    if (index >= data.capacity()) {
        WARN("Tried to update BTT at invalid index: %u", index);
        return;
    }

    // Do not update if the address is already in the table
    // TODO: Maybe move the address to the new index ?
    if (is_address_in_table(instruction_address)) { return; }

    // Prepare new BTT entry
    const BranchTargetTableEntry new_entry { .instruction_address = instruction_address,
                                             .target_address = target_address };

    // Write new entry to the table
    data.at(index) = new_entry;

    emit updated_row(index, instruction_address, target_address);
}

/////////////////////
// Predictor class //
/////////////////////

// Predictor Generic
// #################

void Predictor::update_stats(PredictionFeedback feedback) {
    stats.last_result = feedback.result;
    if (stats.last_prediction == stats.last_result) {
        stats.number_of_correct_predictions += 1;
    } else {
        stats.number_of_wrong_predictions += 1;
    }

    if ((stats.number_of_correct_predictions + stats.number_of_wrong_predictions) > 0) {
        stats.accuracy
            = (100 * stats.number_of_correct_predictions)
              / (stats.number_of_correct_predictions + stats.number_of_wrong_predictions);
    } else {
        stats.accuracy = 0;
    }
}

BranchResult Predictor::predict(PredictionInput input) {
    BranchResult result { make_prediction(input) };
    stats.last_prediction = result;
    emit new_prediction(input, result);
    return result;
}

void Predictor::update(PredictionFeedback feedback) {
    update_stats(feedback);
    emit updated_stats(stats);
}

// Always Not Taken
// ################

PredictorAlwaysNotTaken::PredictorAlwaysNotTaken() {
    stats.last_prediction = BranchResult::NOT_TAKEN;
}

BranchResult PredictorAlwaysNotTaken::make_prediction(PredictionInput input) const {
    UNUSED(input);
    return BranchResult::NOT_TAKEN;
}

// Always Taken
// ############

PredictorAlwaysTaken::PredictorAlwaysTaken() {
    stats.last_prediction = BranchResult::TAKEN;
}

BranchResult PredictorAlwaysTaken::make_prediction(PredictionInput input) const {
    UNUSED(input);
    return BranchResult::TAKEN;
}

// Backward Taken Forward Not Taken
// ################################

PredictorBTFNT::PredictorBTFNT() {
    // TODO figure out how to best update first prediction
    stats.last_prediction = BranchResult::UNDEFINED;
}

BranchResult PredictorBTFNT::make_prediction(PredictionInput input) const {
    if (input.target_address > input.instruction_address) {
        // If target address is larger than jump instruction address (forward jump), predict not
        // taken
        return BranchResult::NOT_TAKEN;
    } else {
        // Otherwise (backward jump) predict taken
        return BranchResult::TAKEN;
    }
}

void PredictorBTFNT::update(PredictionFeedback feedback) {
    if (stats.last_prediction == BranchResult::UNDEFINED) {
        stats.last_prediction
            = make_prediction({ .instruction = feedback.instruction,
                                .instruction_address = feedback.instruction_address,
                                .target_address = feedback.target_address,
                                .bht_index = feedback.bht_index });
    }
    update_stats(feedback);
    emit updated_stats(stats);
}

// Smith Generic
// #############

PredictorSmith::PredictorSmith(uint8_t number_of_bht_bits, PredictorState initial_state)
    : number_of_bht_bits(init_number_of_bht_bits(number_of_bht_bits)) {
    bht.resize(qPow(2, number_of_bht_bits));
    for (uint16_t i = 0; i < bht.capacity(); i++) {
        bht.at(i).state = initial_state;
        bht.at(i).stats.last_prediction = state_to_prediction(initial_state);
    }
    stats.last_prediction = state_to_prediction(initial_state);
}

uint8_t PredictorSmith::init_number_of_bht_bits(uint8_t number_of_bht_bits) const {
    if (number_of_bht_bits > PREDICTOR_MAX_TABLE_BITS) {
        WARN(
            "Number of BHT bits (%u) was larger than %d during init", number_of_bht_bits,
            PREDICTOR_MAX_TABLE_BITS);
        return PREDICTOR_MAX_TABLE_BITS;
    }
    return number_of_bht_bits;
}

void PredictorSmith::update_stats(PredictionFeedback feedback) {
    PredictionStatistics row_stats { bht.at(feedback.bht_index).stats };

    row_stats.last_result = feedback.result;
    stats.last_result = feedback.result;

    if (row_stats.last_prediction == row_stats.last_result) {
        row_stats.number_of_correct_predictions += 1;
        stats.number_of_correct_predictions += 1;
    } else {
        row_stats.number_of_wrong_predictions += 1;
        stats.number_of_wrong_predictions += 1;
    }

    if ((row_stats.number_of_correct_predictions + row_stats.number_of_wrong_predictions) > 0) {
        row_stats.accuracy
            = (100 * row_stats.number_of_correct_predictions)
              / (row_stats.number_of_correct_predictions + row_stats.number_of_wrong_predictions);
        stats.accuracy
            = (100 * stats.number_of_correct_predictions)
              / (stats.number_of_correct_predictions + stats.number_of_wrong_predictions);
    } else {
        row_stats.accuracy = 100;
        stats.accuracy = 100;
    }

    bht.at(feedback.bht_index).stats = row_stats;
}

BranchResult PredictorSmith::state_to_prediction(PredictorState state) const {
    if (state == PredictorState::NOT_TAKEN) {
        return BranchResult::NOT_TAKEN;
    } else if (state == PredictorState::TAKEN) {
        return BranchResult::TAKEN;
    } else if (state == PredictorState::WEAKLY_NOT_TAKEN) {
        return BranchResult::NOT_TAKEN;
    } else if (state == PredictorState::STRONGLY_NOT_TAKEN) {
        return BranchResult::NOT_TAKEN;
    } else if (state == PredictorState::WEAKLY_TAKEN) {
        return BranchResult::TAKEN;
    } else if (state == PredictorState::STRONGLY_TAKEN) {
        return BranchResult::TAKEN;
    } else {
        WARN("Smith predictor was provided invalid state");
        return BranchResult::NOT_TAKEN;
    }
}

BranchResult PredictorSmith::predict(PredictionInput input) {
    BranchResult result { make_prediction(input) };
    stats.last_prediction = result;
    bht.at(input.bht_index).stats.last_prediction = result;
    emit new_prediction(input, result);
    return result;
}

void PredictorSmith::update(PredictionFeedback feedback) {
    update_bht(feedback);
    update_stats(feedback);
    emit new_update(feedback);
    emit updated_stats(stats);
    emit updated_bht_row(feedback.bht_index, bht.at(feedback.bht_index));
}

// Smith 1 Bit
// ###########

PredictorSmith1Bit::PredictorSmith1Bit(uint8_t number_of_bht_bits, PredictorState initial_state)
    : PredictorSmith(number_of_bht_bits, initial_state) {};

BranchResult PredictorSmith1Bit::make_prediction(PredictionInput input) const {
    if (input.bht_index >= bht.capacity()) {
        WARN("Tried to read BHT at invalid index: %u", input.bht_index);
        return BranchResult::NOT_TAKEN;
    }

    // Decide prediction
    return state_to_prediction(bht.at(input.bht_index).state);
}

void PredictorSmith1Bit::update_bht(PredictionFeedback feedback) {
    if (feedback.bht_index >= bht.capacity()) {
        WARN("Tried to update BHT at invalid index: %u", feedback.bht_index);
        return;
    }

    // Update internal state
    if (feedback.result == BranchResult::NOT_TAKEN) {
        bht.at(feedback.bht_index).state = PredictorState::NOT_TAKEN;
    } else if (feedback.result == BranchResult::TAKEN) {
        bht.at(feedback.bht_index).state = PredictorState::TAKEN;
    } else {
        WARN("Smith 1 bit predictor has received invalid prediction result");
    }
}

// Smith 2 Bit
// ###########

PredictorSmith2Bit::PredictorSmith2Bit(uint8_t number_of_bht_bits, PredictorState initial_state)
    : PredictorSmith(number_of_bht_bits, initial_state) {};

BranchResult PredictorSmith2Bit::make_prediction(PredictionInput input) const {
    if (input.bht_index >= bht.capacity()) {
        WARN("Tried to read BHT at invalid index: %u", input.bht_index);
        return BranchResult::NOT_TAKEN;
    }

    // Decide prediction
    return state_to_prediction(bht.at(input.bht_index).state);
}

void PredictorSmith2Bit::update_bht(PredictionFeedback feedback) {
    if (feedback.bht_index >= bht.capacity()) {
        WARN("Tried to update BHT at invalid index: %u", feedback.bht_index);
        return;
    }

    // Read value from BHT at correct index
    const PredictorState state = bht.at(feedback.bht_index).state;

    // Update internal state
    if (feedback.result == BranchResult::NOT_TAKEN) {
        if (state == PredictorState::STRONGLY_TAKEN) {
            bht.at(feedback.bht_index).state = PredictorState::WEAKLY_TAKEN;
        } else if (state == PredictorState::WEAKLY_TAKEN) {
            bht.at(feedback.bht_index).state = PredictorState::WEAKLY_NOT_TAKEN;
        } else if (state == PredictorState::WEAKLY_NOT_TAKEN) {
            bht.at(feedback.bht_index).state = PredictorState::STRONGLY_NOT_TAKEN;
        } else if (state == PredictorState::STRONGLY_NOT_TAKEN) {
            bht.at(feedback.bht_index).state = PredictorState::STRONGLY_NOT_TAKEN;
        } else {
            WARN("Smith 2 bit predictor BHT has returned invalid state");
        }
    } else if (feedback.result == BranchResult::TAKEN) {
        if (state == PredictorState::STRONGLY_TAKEN) {
            bht.at(feedback.bht_index).state = PredictorState::STRONGLY_TAKEN;
        } else if (state == PredictorState::WEAKLY_TAKEN) {
            bht.at(feedback.bht_index).state = PredictorState::STRONGLY_TAKEN;
        } else if (state == PredictorState::WEAKLY_NOT_TAKEN) {
            bht.at(feedback.bht_index).state = PredictorState::WEAKLY_TAKEN;
        } else if (state == PredictorState::STRONGLY_NOT_TAKEN) {
            bht.at(feedback.bht_index).state = PredictorState::WEAKLY_NOT_TAKEN;
        } else {
            WARN("Smith 2 bit predictor BHT has returned invalid state");
        }
    } else {
        WARN("Smith 2 bit predictor has received invalid prediction result");
    }
}

// Smith 2 Bit with hysteresis
// ###########################

PredictorSmith2BitHysteresis::PredictorSmith2BitHysteresis(
    uint8_t number_of_bht_bits,
    PredictorState initial_state)
    : PredictorSmith(number_of_bht_bits, initial_state) {};

BranchResult PredictorSmith2BitHysteresis::make_prediction(PredictionInput input) const {
    if (input.bht_index >= bht.capacity()) {
        WARN("Tried to read BHT at invalid index: %u", input.bht_index);
        return BranchResult::NOT_TAKEN;
    }

    // Decide prediction
    return state_to_prediction(bht.at(input.bht_index).state);
}

void PredictorSmith2BitHysteresis::update_bht(PredictionFeedback feedback) {
    if (feedback.bht_index >= bht.capacity()) {
        WARN("Tried to update BHT at invalid index: %u", feedback.bht_index);
        return;
    }

    if (stats.last_prediction == BranchResult::UNDEFINED
        || bht.at(feedback.bht_index).stats.last_prediction == BranchResult::UNDEFINED) {
        const PredictionInput prediction_input { .instruction = feedback.instruction,
                                                 .instruction_address
                                                 = feedback.instruction_address,
                                                 .target_address = feedback.target_address,
                                                 .bht_index = feedback.bht_index };
        const BranchResult prediction { make_prediction(prediction_input) };
        stats.last_prediction = prediction;
        bht.at(feedback.bht_index).stats.last_prediction = prediction;
    }

    // Read value from BHT at correct index
    const PredictorState state = bht.at(feedback.bht_index).state;

    // Update internal state
    if (feedback.result == BranchResult::NOT_TAKEN) {
        if (state == PredictorState::STRONGLY_TAKEN) {
            bht.at(feedback.bht_index).state = PredictorState::WEAKLY_TAKEN;
        } else if (state == PredictorState::WEAKLY_TAKEN) {
            bht.at(feedback.bht_index).state = PredictorState::STRONGLY_NOT_TAKEN;
        } else if (state == PredictorState::WEAKLY_NOT_TAKEN) {
            bht.at(feedback.bht_index).state = PredictorState::STRONGLY_NOT_TAKEN;
        } else if (state == PredictorState::STRONGLY_NOT_TAKEN) {
            bht.at(feedback.bht_index).state = PredictorState::STRONGLY_NOT_TAKEN;
        } else {
            WARN("Smith 2 bit hysteresis predictor BHT has returned invalid state");
        }
    } else if (feedback.result == BranchResult::TAKEN) {
        if (state == PredictorState::STRONGLY_TAKEN) {
            bht.at(feedback.bht_index).state = PredictorState::STRONGLY_TAKEN;
        } else if (state == PredictorState::WEAKLY_TAKEN) {
            bht.at(feedback.bht_index).state = PredictorState::STRONGLY_TAKEN;
        } else if (state == PredictorState::WEAKLY_NOT_TAKEN) {
            bht.at(feedback.bht_index).state = PredictorState::STRONGLY_TAKEN;
        } else if (state == PredictorState::STRONGLY_NOT_TAKEN) {
            bht.at(feedback.bht_index).state = PredictorState::WEAKLY_NOT_TAKEN;
        } else {
            WARN("Smith 2 bit hysteresis predictor BHT has returned invalid state");
        }
    } else {
        WARN("Smith 2 bit hysteresis predictor has received invalid prediction result");
    }
}

///////////////////////////
// BranchPredictor class //
///////////////////////////

// Number of BTT bits always has to be greater or equal to number of BHR bits,
// Any remaining BTT addressing bits not taken from BHR are taken from the instruction address
BranchPredictor::BranchPredictor(
    bool enabled,
    PredictorType predictor_type,
    PredictorState initial_state,
    uint8_t number_of_bhr_bits,
    uint8_t number_of_address_bits)
    : enabled(enabled)
    , initial_state(initial_state)
    , number_of_bhr_bits(number_of_bhr_bits)
    , number_of_address_bits(number_of_address_bits)
    , number_of_table_bits(number_of_bhr_bits + number_of_address_bits) {
    // Input checking
    if (number_of_bhr_bits > PREDICTOR_MAX_TABLE_BITS) {
        throw std::invalid_argument("Branch predictor initialization: Invalid number of BHR bits");
    }

    if (number_of_address_bits > PREDICTOR_MAX_TABLE_BITS) {
        throw std::invalid_argument(
            "Branch predictor initialization: Invalid number of address index bits");
    }

    if (number_of_table_bits > PREDICTOR_MAX_TABLE_BITS) {
        throw std::invalid_argument(
            "Branch predictor initialization: Invalid sum of number of BHR and address bits");
    }

    // Create predicotr
    switch (predictor_type) {
    case PredictorType::ALWAYS_NOT_TAKEN:
        predictor = new PredictorAlwaysNotTaken();
        LOG("Initialized branch predictor: %s", predictor->get_name());
        break;

    case PredictorType::ALWAYS_TAKEN:
        predictor = new PredictorAlwaysTaken();
        LOG("Initialized branch predictor: %s", predictor->get_name());
        break;

    case PredictorType::BTFNT:
        predictor = new PredictorBTFNT();
        LOG("Initialized branch predictor: %s", predictor->get_name());
        break;

    case PredictorType::SMITH_1_BIT:
        predictor = new PredictorSmith1Bit(number_of_table_bits, initial_state);
        LOG("Initialized branch predictor: %s, with %u BHT bits, and initial state at: %s",
            predictor->get_name(), number_of_table_bits, predictor_state_to_string(initial_state));
        break;

    case PredictorType::SMITH_2_BIT:
        predictor = new PredictorSmith2Bit(number_of_table_bits, initial_state);
        LOG("Initialized branch predictor: %s, with %u BHT bits, and initial state at: %s",
            predictor->get_name(), number_of_table_bits, predictor_state_to_string(initial_state));
        break;

    case PredictorType::SMITH_2_BIT_HYSTERESIS:
        predictor = new PredictorSmith2BitHysteresis(number_of_table_bits, initial_state);
        LOG("Initialized branch predictor: %s, with %u BHT bits, and initial state at: %s",
            predictor->get_name(), number_of_table_bits, predictor_state_to_string(initial_state));
        break;

    default: throw std::invalid_argument("Invalid predictor type selected");
    }

    bhr = new BranchHistoryRegister(number_of_bhr_bits);
    btt = new BranchTargetTable(number_of_table_bits);

    if (enabled) {
        connect(btt, &BranchTargetTable::updated_row, this, &BranchPredictor::updated_btt_row);
        connect(bhr, &BranchHistoryRegister::updated, this, &BranchPredictor::updated_bhr);
        connect(predictor, &Predictor::new_prediction, this, &BranchPredictor::new_prediction);
        connect(predictor, &Predictor::new_update, this, &BranchPredictor::new_update);
        connect(
            predictor, &Predictor::updated_stats, this, &BranchPredictor::updated_predictor_stats);
        connect(predictor, &Predictor::updated_bht_row, this, &BranchPredictor::updated_bht_row);
    }
}

BranchPredictor::~BranchPredictor() {
    delete predictor;
    predictor = nullptr;
    delete bhr;
    bhr = nullptr;
    delete btt;
    btt = nullptr;
}

uint8_t BranchPredictor::init_number_of_bhr_bits(const uint8_t number_of_bhr_bits) const {
    if (number_of_bhr_bits > PREDICTOR_MAX_TABLE_BITS) {
        WARN(
            "Number of BHR bits (%u) was larger than %d during init", number_of_bhr_bits,
            PREDICTOR_MAX_TABLE_BITS);
        return PREDICTOR_MAX_TABLE_BITS;
    }
    return number_of_bhr_bits;
}

uint8_t BranchPredictor::init_number_of_address_bits(
    const uint8_t number_of_bhr_bits,
    const uint8_t number_of_address_bits) const {
    // Clamp number of BHR bits
    uint8_t checked_number_of_bhr_bits = number_of_bhr_bits;
    if (number_of_bhr_bits > PREDICTOR_MAX_TABLE_BITS) {
        checked_number_of_bhr_bits = PREDICTOR_MAX_TABLE_BITS;
    }

    // Clamp number of address index bits
    uint8_t checked_number_of_address_bits = number_of_address_bits;
    if (number_of_address_bits > PREDICTOR_MAX_TABLE_BITS) {
        WARN(
            "Number of address index bits (%u) was larger than %d during init",
            number_of_address_bits, PREDICTOR_MAX_TABLE_BITS);
        checked_number_of_address_bits = PREDICTOR_MAX_TABLE_BITS;
    }

    // Check sum
    if ((checked_number_of_bhr_bits + checked_number_of_address_bits) > PREDICTOR_MAX_TABLE_BITS) {
        WARN(
            "Sum of BHR and address index bits (%u) was larger than %d during init",
            number_of_address_bits, PREDICTOR_MAX_TABLE_BITS);
        return PREDICTOR_MAX_TABLE_BITS - checked_number_of_bhr_bits;
    }

    return checked_number_of_address_bits;
}

uint8_t BranchPredictor::init_number_of_table_bits(
    const uint8_t number_of_bhr_bits,
    const uint8_t number_of_address_bits) const {
    // Clamp number of BHR bits
    uint8_t checked_number_of_bhr_bits = number_of_bhr_bits;
    if (number_of_bhr_bits > PREDICTOR_MAX_TABLE_BITS) {
        checked_number_of_bhr_bits = PREDICTOR_MAX_TABLE_BITS;
    }

    // Clamp number of address index bits
    uint8_t checked_number_of_address_bits = number_of_address_bits;
    if (number_of_address_bits > PREDICTOR_MAX_TABLE_BITS) {
        checked_number_of_address_bits = PREDICTOR_MAX_TABLE_BITS;
    }

    // Check sum
    if ((checked_number_of_bhr_bits + checked_number_of_address_bits) > PREDICTOR_MAX_TABLE_BITS) {
        WARN(
            "Number of BTT and BHT index bits (%u) was larger than %d during init",
            number_of_address_bits, PREDICTOR_MAX_TABLE_BITS);
        return PREDICTOR_MAX_TABLE_BITS;
    }

    return checked_number_of_bhr_bits + checked_number_of_address_bits;
}

bool BranchPredictor::get_enabled() const {
    return enabled;
}

PredictorType BranchPredictor::get_predictor_type() const {
    if (!enabled) { return PredictorType::UNDEFINED; }
    return predictor->get_type();
}

const char *BranchPredictor::get_predictor_name() const {
    if (!enabled) { return "None"; }
    return predictor->get_name();
}

PredictorState BranchPredictor::get_initial_state() const {
    if (!enabled) { return PredictorState::UNDEFINED; }
    return initial_state;
}

uint8_t BranchPredictor::get_number_of_bhr_bits() const {
    if (!enabled) { return 0; }
    return number_of_bhr_bits;
}

uint8_t BranchPredictor::get_number_of_address_bits() const {
    if (!enabled) { return 0; }
    return number_of_address_bits;
}

uint8_t BranchPredictor::get_number_of_table_bits() const {
    if (!enabled) { return 0; }
    return number_of_table_bits;
}

// Function to calculate BTT and BHT index from BHR and instruction address
uint16_t BranchPredictor::calculate_index(const Address instruction_address) const {
    const uint16_t bhr_part = bhr->get_value() << number_of_address_bits;
    const uint16_t address_mask = (1 << number_of_address_bits) - 1;
    const uint16_t address_part = ((uint16_t)(instruction_address.get_raw() >> 2)) & address_mask;
    const uint16_t index = bhr_part | address_part;
    return index;
}

Address BranchPredictor::predict_next_pc_address(
    const Instruction instruction,
    const Address instruction_address) const {
    // Check if predictor is enabled
    if (!enabled) { return instruction_address + 4; }

    // Get target address from BTT
    Address target_address = btt->get_target_address(instruction_address);
    if (target_address.is_null()) { return instruction_address + 4; }

    // Make prediction
    const PredictionInput prediction_input { .instruction = instruction,
                                             .instruction_address = instruction_address,
                                             .target_address = target_address,
                                             .bht_index = calculate_index(instruction_address) };
    const BranchResult predicted_result = predictor->predict(prediction_input);

    if (predicted_result == BranchResult::TAKEN) { return target_address; }

    // Default prediction
    return instruction_address + 4;
}

// Function for updating the Branch Target Table (BTT)
void BranchPredictor::update_target(const Address instruction_address, const Address target_address) {
    // Check if predictor is enabled
    if (!enabled) { return; }

    btt->update(calculate_index(instruction_address), instruction_address, target_address);
}

// Function for updating the predictor and the Branch History Register (BHR)
void BranchPredictor::update_result(
    const Instruction instruction,
    const Address instruction_address,
    const BranchResult result) {
    // Check if predictor is enabled
    if (!enabled) { return; }

    // Update global branch history
    bhr->update(result);

    // Check if address exists in BTT
    Address target_address = btt->get_target_address(instruction_address);
    if (target_address.is_null()) { return; }

    // Update predictor
    const PredictionFeedback prediction_feedback { .instruction = instruction,
                                                   .instruction_address = instruction_address,
                                                   .bht_index
                                                   = calculate_index(instruction_address),
                                                   .result = result };
    predictor->update(prediction_feedback);
}
