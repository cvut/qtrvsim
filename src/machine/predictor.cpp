#include "predictor.h"

LOG_CATEGORY("machine.BranchPredictor");

using namespace machine;

QStringView machine::branch_result_to_string(const BranchResult result, const bool abbrv) {
    switch (result) {
    case BranchResult::NOT_TAKEN: return abbrv ? u"NT" : u"Not taken";
    case BranchResult::TAKEN: return abbrv ? u"T" : u"Taken";
    default: return u"";
    }
}

QStringView machine::predictor_state_to_string(const PredictorState state, const bool abbrv) {
    switch (state) {
    case PredictorState::NOT_TAKEN: return abbrv ? u"NT" : u"Not taken";
    case PredictorState::TAKEN: return abbrv ? u"T" : u"Taken";
    case PredictorState::STRONGLY_NOT_TAKEN: return abbrv ? u"SNT" : u"Strongly not taken";
    case PredictorState::WEAKLY_NOT_TAKEN: return abbrv ? u"WNT" : u"Weakly not taken";
    case PredictorState::WEAKLY_TAKEN: return abbrv ? u"WT" : u"Weakly taken";
    case PredictorState::STRONGLY_TAKEN: return abbrv ? u"ST" : u"Strongly taken";
    default: return u"";
    }
}

QStringView machine::predictor_type_to_string(const PredictorType type) {
    switch (type) {
    case PredictorType::ALWAYS_NOT_TAKEN: return u"Always not taken";
    case PredictorType::ALWAYS_TAKEN: return u"Always taken";
    case PredictorType::BTFNT: return u"Backward Taken Forward Not Taken";
    case PredictorType::SMITH_1_BIT: return u"Smith 1 bit";
    case PredictorType::SMITH_2_BIT: return u"Smith 2 bit";
    case PredictorType::SMITH_2_BIT_HYSTERESIS: return u"Smith 2 bit with hysteresis";
    default: return u"";
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

// Init helper function to check the number of bits
uint8_t BranchHistoryRegister::init_number_of_bits(const uint8_t b) const {
    if (b > BP_MAX_BHR_BITS) {
        WARN("Number of BHR bits (%u) was larger than %u during init", b, BP_MAX_BHR_BITS);
        return BP_MAX_BHR_BITS;
    }
    return b;
}

// Init helper function to create the register mask used for masking unused bits
uint16_t BranchHistoryRegister::init_register_mask(const uint8_t b) const {
    if (b >= BP_MAX_BHR_BITS) { return ~0x0; }
    if (b == 0) { return 0x0; }
    return (1 << b) - 1;
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

    emit update_done(number_of_bits, value);
}

/////////////////////////////
// BranchTargetBuffer class //
/////////////////////////////

// Constructor
BranchTargetBuffer::BranchTargetBuffer(uint8_t number_of_bits)
    : number_of_bits(init_number_of_bits(number_of_bits)) {
    btb.resize(qPow(2, number_of_bits));
}

uint8_t BranchTargetBuffer::init_number_of_bits(const uint8_t b) const {
    if (b > BP_MAX_BTB_BITS) {
        WARN("Number of BTB bits (%u) was larger than %u during init", b, BP_MAX_BTB_BITS);
        return BP_MAX_BTB_BITS;
    }
    return b;
}

// Calculate index for addressing Branch Target Buffer from instruction address
uint16_t BranchTargetBuffer::calculate_index(const Address instruction_address) const {
    return ((uint16_t)(instruction_address.get_raw() >> 2)) & ((1 << number_of_bits) - 1);
}

uint8_t BranchTargetBuffer::get_number_of_bits() const {
    return number_of_bits;
}

// Find instruction address in the BTB using the provided instruction address, return 0 address if
// not found
Address BranchTargetBuffer::get_instruction_address(const Address instruction_address) const {
    // Get index from instruction address
    const uint16_t index { calculate_index(instruction_address) };

    // Check index validity
    if (index >= btb.capacity()) {
        WARN("Tried to read from BTB at invalid index: %u", index);
        return Address::null();
    }

    // Return target address at index
    return btb.at(index).instruction_address;
}

// Find target address in the BTB using the provided instruction address, return 0 address if not
// found
Address BranchTargetBuffer::get_target_address(const Address instruction_address) const {
    // Get index from instruction address
    const uint16_t index { calculate_index(instruction_address) };

    // Check index validity
    if (index >= btb.capacity()) {
        WARN("Tried to read from BTB at invalid index: %u", index);
        return Address::null();
    }

    // Return target address at index
    emit requested_target_address(index);
    return btb.at(index).target_address;
}

// Update BTB entry with given values, at index computed from the instruction address
void BranchTargetBuffer::update(const Address instruction_address, const Address target_address) {
    // Get index from instruction address
    const uint16_t index { calculate_index(instruction_address) };

    // Check index validity
    if (index >= btb.capacity()) {
        WARN("Tried to update BTB at invalid index: %u", index);
        return;
    }

    // Write new entry to the table
    btb.at(index)
        = { .instruction_address = instruction_address, .target_address = target_address };

    // Send signal with the data
    emit update_row_done(index, instruction_address, target_address);
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
    const BranchResult result { make_prediction(input) };
    stats.last_prediction = result;
    emit prediction_done(0, input, result);
    return result;
}

void Predictor::update(PredictionFeedback feedback) {
    update_stats(feedback);
    emit update_stats_done(stats);
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
    stats.last_prediction = make_prediction({ .instruction = feedback.instruction,
                                              .bhr_value = feedback.bhr_value,
                                              .instruction_address = feedback.instruction_address,
                                              .target_address = feedback.target_address });
    update_stats(feedback);
    emit update_stats_done(stats);
}

// Smith Generic
// #############

PredictorSmith::PredictorSmith(
    uint8_t number_of_bht_addr_bits,
    uint8_t number_of_bht_bits,
    PredictorState initial_state)
    : number_of_bht_addr_bits(init_number_of_bht_addr_bits(number_of_bht_addr_bits))
    , number_of_bht_bits(init_number_of_bht_bits(number_of_bht_bits)) {
    bht.resize(qPow(2, number_of_bht_bits));
    for (uint16_t i = 0; i < bht.capacity(); i++) {
        bht.at(i).state = initial_state;
        bht.at(i).stats.last_prediction = convert_state_to_prediction(initial_state);
    }
    stats.last_prediction = convert_state_to_prediction(initial_state);
}

uint8_t PredictorSmith::init_number_of_bht_addr_bits(const uint8_t b) const {
    if (b > BP_MAX_BHT_ADDR_BITS) {
        WARN(
            "Number of BHT bits from incstruction address (%u) was larger than %d during init", b,
            BP_MAX_BHT_ADDR_BITS);
        return BP_MAX_BHT_ADDR_BITS;
    }
    return b;
}

uint8_t PredictorSmith::init_number_of_bht_bits(const uint8_t b) const {
    if (b > BP_MAX_BHT_BITS) {
        WARN("Number of BHT bits (%u) was larger than %d during init", b, BP_MAX_BHT_BITS);
        return BP_MAX_BHT_BITS;
    }
    return b;
}

// Calculate index for addressing Branch History Table from BHR and instruction address
uint16_t PredictorSmith::calculate_bht_index(
    const uint16_t bhr_value,
    const Address instruction_address) const {
    const uint16_t bhr_part = bhr_value << number_of_bht_addr_bits;
    const uint16_t address_mask = (1 << number_of_bht_addr_bits) - 1;
    const uint16_t address_part = ((uint16_t)(instruction_address.get_raw() >> 2)) & address_mask;
    const uint16_t index = bhr_part | address_part;
    return index;
}

void PredictorSmith::update_stats(PredictionFeedback feedback) {
    // Get and check BHT index
    const uint16_t index { calculate_bht_index(feedback.bhr_value, feedback.instruction_address) };
    if (index >= bht.capacity()) {
        WARN("Tried to access BHT at invalid index: %u", index);
        return;
    }

    // Get current statistics from BHT row
    PredictionStatistics row_stats { bht.at(index).stats };

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

    bht.at(index).stats = row_stats;
}

BranchResult PredictorSmith::convert_state_to_prediction(PredictorState state) const {
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
    const uint16_t index { calculate_bht_index(input.bhr_value, input.instruction_address) };
    if (index >= bht.capacity()) {
        WARN("Tried to access BHT at invalid index: %u", index);
        return BranchResult::NOT_TAKEN;
    }

    const BranchResult result { make_prediction(input) };

    stats.last_prediction = result;
    bht.at(index).stats.last_prediction = result;

    emit prediction_done(index, input, result);
    return result;
}

void PredictorSmith::update(PredictionFeedback feedback) {
    const uint16_t index { calculate_bht_index(feedback.bhr_value, feedback.instruction_address) };
    if (index >= bht.capacity()) {
        WARN("Tried to access BHT at invalid index: %u", index);
        return;
    }

    update_bht(feedback);
    update_stats(feedback);

    emit update_done(index, feedback);
    emit update_stats_done(stats);
    emit update_bht_row_done(index, bht.at(index));
}

// Smith 1 Bit
// ###########

PredictorSmith1Bit::PredictorSmith1Bit(
    uint8_t number_of_bht_addr_bits,
    uint8_t number_of_bht_bits,
    PredictorState initial_state)
    : PredictorSmith(number_of_bht_addr_bits, number_of_bht_bits, initial_state) {};

BranchResult PredictorSmith1Bit::make_prediction(PredictionInput input) const {
    const uint16_t index { calculate_bht_index(input.bhr_value, input.instruction_address) };
    if (index >= bht.capacity()) {
        WARN("Tried to access BHT at invalid index: %u", index);
        return BranchResult::NOT_TAKEN;
    }

    // Decide prediction
    return convert_state_to_prediction(bht.at(index).state);
}

void PredictorSmith1Bit::update_bht(PredictionFeedback feedback) {
    const uint16_t index { calculate_bht_index(feedback.bhr_value, feedback.instruction_address) };
    if (index >= bht.capacity()) {
        WARN("Tried to access BHT at invalid index: %u", index);
        return;
    }

    // Update internal state
    if (feedback.result == BranchResult::NOT_TAKEN) {
        bht.at(index).state = PredictorState::NOT_TAKEN;
    } else if (feedback.result == BranchResult::TAKEN) {
        bht.at(index).state = PredictorState::TAKEN;
    } else {
        WARN("Smith 1 bit predictor has received invalid prediction result");
    }
}

// Smith 2 Bit
// ###########

PredictorSmith2Bit::PredictorSmith2Bit(
    uint8_t number_of_bht_addr_bits,
    uint8_t number_of_bht_bits,
    PredictorState initial_state)
    : PredictorSmith(number_of_bht_addr_bits, number_of_bht_bits, initial_state) {};

BranchResult PredictorSmith2Bit::make_prediction(PredictionInput input) const {
    const uint16_t index { calculate_bht_index(input.bhr_value, input.instruction_address) };
    if (index >= bht.capacity()) {
        WARN("Tried to access BHT at invalid index: %u", index);
        return BranchResult::NOT_TAKEN;
    }

    // Decide prediction
    return convert_state_to_prediction(bht.at(index).state);
}

void PredictorSmith2Bit::update_bht(PredictionFeedback feedback) {
    const uint16_t index { calculate_bht_index(feedback.bhr_value, feedback.instruction_address) };
    if (index >= bht.capacity()) {
        WARN("Tried to access BHT at invalid index: %u", index);
        return;
    }

    // Read value from BHT at correct index
    const PredictorState state = bht.at(index).state;

    // Update internal state
    if (feedback.result == BranchResult::NOT_TAKEN) {
        if (state == PredictorState::STRONGLY_TAKEN) {
            bht.at(index).state = PredictorState::WEAKLY_TAKEN;
        } else if (state == PredictorState::WEAKLY_TAKEN) {
            bht.at(index).state = PredictorState::WEAKLY_NOT_TAKEN;
        } else if (state == PredictorState::WEAKLY_NOT_TAKEN) {
            bht.at(index).state = PredictorState::STRONGLY_NOT_TAKEN;
        } else if (state == PredictorState::STRONGLY_NOT_TAKEN) {
            bht.at(index).state = PredictorState::STRONGLY_NOT_TAKEN;
        } else {
            WARN("Smith 2 bit predictor BHT has returned invalid state");
        }
    } else if (feedback.result == BranchResult::TAKEN) {
        if (state == PredictorState::STRONGLY_TAKEN) {
            bht.at(index).state = PredictorState::STRONGLY_TAKEN;
        } else if (state == PredictorState::WEAKLY_TAKEN) {
            bht.at(index).state = PredictorState::STRONGLY_TAKEN;
        } else if (state == PredictorState::WEAKLY_NOT_TAKEN) {
            bht.at(index).state = PredictorState::WEAKLY_TAKEN;
        } else if (state == PredictorState::STRONGLY_NOT_TAKEN) {
            bht.at(index).state = PredictorState::WEAKLY_NOT_TAKEN;
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
    uint8_t number_of_bht_addr_bits,
    uint8_t number_of_bht_bits,
    PredictorState initial_state)
    : PredictorSmith(number_of_bht_addr_bits, number_of_bht_bits, initial_state) {};

BranchResult PredictorSmith2BitHysteresis::make_prediction(PredictionInput input) const {
    const uint16_t index { calculate_bht_index(input.bhr_value, input.instruction_address) };
    if (index >= bht.capacity()) {
        WARN("Tried to access BHT at invalid index: %u", index);
        return BranchResult::NOT_TAKEN;
    }

    // Decide prediction
    return convert_state_to_prediction(bht.at(index).state);
}

void PredictorSmith2BitHysteresis::update_bht(PredictionFeedback feedback) {
    const uint16_t index { calculate_bht_index(feedback.bhr_value, feedback.instruction_address) };
    if (index >= bht.capacity()) {
        WARN("Tried to access BHT at invalid index: %u", index);
        return;
    }

    // Read value from BHT at correct index
    const PredictorState state = bht.at(index).state;

    // Update internal state
    if (feedback.result == BranchResult::NOT_TAKEN) {
        if (state == PredictorState::STRONGLY_TAKEN) {
            bht.at(index).state = PredictorState::WEAKLY_TAKEN;
        } else if (state == PredictorState::WEAKLY_TAKEN) {
            bht.at(index).state = PredictorState::STRONGLY_NOT_TAKEN;
        } else if (state == PredictorState::WEAKLY_NOT_TAKEN) {
            bht.at(index).state = PredictorState::STRONGLY_NOT_TAKEN;
        } else if (state == PredictorState::STRONGLY_NOT_TAKEN) {
            bht.at(index).state = PredictorState::STRONGLY_NOT_TAKEN;
        } else {
            WARN("Smith 2 bit hysteresis predictor BHT has returned invalid state");
        }
    } else if (feedback.result == BranchResult::TAKEN) {
        if (state == PredictorState::STRONGLY_TAKEN) {
            bht.at(index).state = PredictorState::STRONGLY_TAKEN;
        } else if (state == PredictorState::WEAKLY_TAKEN) {
            bht.at(index).state = PredictorState::STRONGLY_TAKEN;
        } else if (state == PredictorState::WEAKLY_NOT_TAKEN) {
            bht.at(index).state = PredictorState::STRONGLY_TAKEN;
        } else if (state == PredictorState::STRONGLY_NOT_TAKEN) {
            bht.at(index).state = PredictorState::WEAKLY_NOT_TAKEN;
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

BranchPredictor::BranchPredictor(
    bool enabled,
    PredictorType predictor_type,
    PredictorState initial_state,
    uint8_t number_of_btb_bits,
    uint8_t number_of_bhr_bits,
    uint8_t number_of_bht_addr_bits)
    : enabled(enabled)
    , initial_state(initial_state)
    , number_of_btb_bits(init_number_of_btb_bits(number_of_btb_bits))
    , number_of_bhr_bits(init_number_of_bhr_bits(number_of_bhr_bits))
    , number_of_bht_addr_bits(init_number_of_bht_addr_bits(number_of_bht_addr_bits))
    , number_of_bht_bits(init_number_of_bht_bits(number_of_bhr_bits, number_of_bht_addr_bits)) {
    // Create predicotr
    switch (predictor_type) {
    case PredictorType::ALWAYS_NOT_TAKEN:
        predictor = new PredictorAlwaysNotTaken();
        LOG("Initialized branch predictor: %s", qPrintable(get_predictor_name().toString()));
        break;

    case PredictorType::ALWAYS_TAKEN:
        predictor = new PredictorAlwaysTaken();
        LOG("Initialized branch predictor: %s", qPrintable(get_predictor_name().toString()));
        break;

    case PredictorType::BTFNT:
        predictor = new PredictorBTFNT();
        LOG("Initialized branch predictor: %s", qPrintable(get_predictor_name().toString()));
        break;

    case PredictorType::SMITH_1_BIT:
        predictor
            = new PredictorSmith1Bit(number_of_bht_addr_bits, number_of_bht_bits, initial_state);
        LOG("Initialized branch predictor: %s, with %u BHT bits, and initial state at: %s",
            qPrintable(get_predictor_name().toString()), number_of_bht_bits,
            qPrintable(predictor_state_to_string(initial_state).toString()));
        break;

    case PredictorType::SMITH_2_BIT:
        predictor
            = new PredictorSmith2Bit(number_of_bht_addr_bits, number_of_bht_bits, initial_state);
        LOG("Initialized branch predictor: %s, with %u BHT bits, and initial state at: %s",
            qPrintable(get_predictor_name().toString()), number_of_bht_bits,
            qPrintable(predictor_state_to_string(initial_state).toString()));
        break;

    case PredictorType::SMITH_2_BIT_HYSTERESIS:
        predictor = new PredictorSmith2BitHysteresis(
            number_of_bht_addr_bits, number_of_bht_bits, initial_state);
        LOG("Initialized branch predictor: %s, with %u BHT bits, and initial state at: %s",
            qPrintable(get_predictor_name().toString()), number_of_bht_bits,
            qPrintable(predictor_state_to_string(initial_state).toString()));
        break;

    default: throw std::invalid_argument("Invalid predictor type selected");
    }

    bhr = new BranchHistoryRegister(number_of_bhr_bits);
    btb = new BranchTargetBuffer(number_of_btb_bits);

    if (enabled) {
        connect(
            btb, &BranchTargetBuffer::requested_target_address, this,
            &BranchPredictor::requested_bht_target_address);
        connect(
            btb, &BranchTargetBuffer::update_row_done, this, &BranchPredictor::update_btb_row_done);
        connect(bhr, &BranchHistoryRegister::update_done, this, &BranchPredictor::update_bhr_done);
        connect(predictor, &Predictor::prediction_done, this, &BranchPredictor::prediction_done);
        connect(predictor, &Predictor::update_done, this, &BranchPredictor::update_predictor_done);
        connect(
            predictor, &Predictor::update_stats_done, this,
            &BranchPredictor::update_predictor_stats_done);
        connect(
            predictor, &Predictor::update_bht_row_done, this,
            &BranchPredictor::update_predictor_bht_row_done);
    }
}

BranchPredictor::~BranchPredictor() {
    delete predictor;
    predictor = nullptr;
    delete bhr;
    bhr = nullptr;
    delete btb;
    btb = nullptr;
}

uint8_t BranchPredictor::init_number_of_btb_bits(const uint8_t b) const {
    if (b > BP_MAX_BTB_BITS) {
        WARN("Number of BTB bits (%u) was larger than %d during init", b, BP_MAX_BTB_BITS);
        return BP_MAX_BTB_BITS;
    }
    return b;
}

uint8_t BranchPredictor::init_number_of_bhr_bits(const uint8_t b) const {
    if (b > BP_MAX_BHR_BITS) {
        WARN("Number of BHR bits (%u) was larger than %d during init", b, BP_MAX_BHR_BITS);
        return BP_MAX_BHR_BITS;
    }
    return b;
}

uint8_t BranchPredictor::init_number_of_bht_addr_bits(const uint8_t b) const {
    if (b > BP_MAX_BHT_ADDR_BITS) {
        WARN(
            "Number of BHT instruction address bits (%u) was larger than %d during init", b,
            BP_MAX_BHT_ADDR_BITS);
        return BP_MAX_BHT_ADDR_BITS;
    }
    return b;
}

uint8_t BranchPredictor::init_number_of_bht_bits(const uint8_t b_bhr, const uint8_t b_addr) const {
    // Clamp number of BHR bits
    uint8_t checked_number_of_bits_bhr { b_bhr };
    if (checked_number_of_bits_bhr > BP_MAX_BHR_BITS) {
        checked_number_of_bits_bhr = BP_MAX_BHR_BITS;
    }

    // Clamp number of address index bits
    uint8_t checked_number_of_bits_addr { b_addr };
    if (checked_number_of_bits_addr > BP_MAX_BHT_ADDR_BITS) {
        checked_number_of_bits_addr = BP_MAX_BHT_ADDR_BITS;
    }

    // Check sum
    uint8_t b_sum { (uint8_t)(checked_number_of_bits_bhr + checked_number_of_bits_addr) };
    if (b_sum > BP_MAX_BHT_BITS) { b_sum = BP_MAX_BHT_BITS; }

    return b_sum;
}

bool BranchPredictor::get_enabled() const {
    return enabled;
}

PredictorType BranchPredictor::get_predictor_type() const {
    if (!enabled) { return PredictorType::UNDEFINED; }
    return predictor->get_type();
}

QStringView BranchPredictor::get_predictor_name() const {
    if (!enabled) { return u"None"; }
    return predictor_type_to_string(predictor->get_type());
}

PredictorState BranchPredictor::get_initial_state() const {
    if (!enabled) { return PredictorState::UNDEFINED; }
    return initial_state;
}

uint8_t BranchPredictor::get_number_of_btb_bits() const {
    if (!enabled) { return 0; }
    return number_of_btb_bits;
}

uint8_t BranchPredictor::get_number_of_bhr_bits() const {
    if (!enabled) { return 0; }
    return number_of_bhr_bits;
}

uint8_t BranchPredictor::get_number_of_bht_addr_bits() const {
    if (!enabled) { return 0; }
    return number_of_bht_addr_bits;
}

uint8_t BranchPredictor::get_number_of_bht_bits() const {
    if (!enabled) { return 0; }
    return number_of_bht_bits;
}

Address BranchPredictor::predict_next_pc_address(
    const Instruction instruction,
    const Address instruction_address) const {
    // Check if predictor is enabled
    if (!enabled) { return instruction_address + 4; }

    // Get instruction address from BTB
    Address instruction_address_from_btb = btb->get_instruction_address(instruction_address);
    if (instruction_address_from_btb.is_null()
        || instruction_address != instruction_address_from_btb) {
        return instruction_address + 4;
    }

    // Get target address from BTB
    Address target_address_from_btb = btb->get_target_address(instruction_address);
    if (target_address_from_btb.is_null()) { return instruction_address + 4; }

    // Make prediction
    const PredictionInput prediction_input {
        .instruction = instruction,
        .bhr_value = bhr->get_value(),
        .instruction_address = instruction_address,
        .target_address = target_address_from_btb,
    };
    const BranchResult predicted_result = predictor->predict(prediction_input);

    // If the branch was predicted taken
    if (predicted_result == BranchResult::TAKEN) { return target_address_from_btb; }

    // Default prediction - not taken
    return instruction_address + 4;
}

// Function for updating the predictor and the Branch History Register (BHR)
void BranchPredictor::update(
    const Instruction instruction,
    const Address instruction_address,
    const Address target_address,
    const BranchResult result) {
    // Check if predictor is enabled
    if (!enabled) { return; }

    // Update Branch Target Table
    if (result == BranchResult::TAKEN) { btb->update(instruction_address, target_address); }

    // Update predictor
    const PredictionFeedback prediction_feedback { .instruction = instruction,
                                                   .bhr_value = bhr->get_value(),
                                                   .instruction_address = instruction_address,
                                                   .result = result };
    predictor->update(prediction_feedback);

    // Update global branch history
    bhr->update(result);
}
