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

QStringView machine::branch_type_to_string(const BranchType type) {
    switch (type) {
    case BranchType::JUMP: return u"JUMP";
    case BranchType::BRANCH: return u"BRANCH";
    default: return u"";
    }
}

QString machine::addr_to_hex_str(const machine::Address address) {
    QString hex_addr, zero_padding;
    hex_addr = QString::number(address.get_raw(), 16);
    zero_padding.fill('0', 8 - hex_addr.count());
    return "0x" + zero_padding + hex_addr;
}

bool machine::is_predictor_type_dynamic(const PredictorType type) {
    switch (type)
    {
    case PredictorType::ALWAYS_NOT_TAKEN:
    case PredictorType::ALWAYS_TAKEN:
    case PredictorType::BTFNT:
        return false;

    case PredictorType::SMITH_1_BIT:
    case PredictorType::SMITH_2_BIT:
    case PredictorType::SMITH_2_BIT_HYSTERESIS:
        return true;

    default:
        return false;
    }
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

    emit bhr_updated(number_of_bits, value);
}

void BranchHistoryRegister::clear() {
    value = 0x0;
    emit bhr_updated(number_of_bits, value);
}

//////////////////////////////
// BranchTargetBuffer class //
//////////////////////////////

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

uint8_t BranchTargetBuffer::get_number_of_bits() const {
    return number_of_bits;
}

// Calculate index for addressing Branch Target Buffer from instruction address
uint16_t BranchTargetBuffer::calculate_index(const Address instruction_address) const {
    return ((uint16_t)(instruction_address.get_raw() >> 2)) & ((1 << number_of_bits) - 1);
}

BranchTargetBufferEntry BranchTargetBuffer::get_entry(const Address instruction_address) const {
    // Get index from instruction address
    const uint16_t index { calculate_index(instruction_address) };

    // Check index validity
    if (index >= btb.capacity()) {
        WARN("Tried to read from BTB at invalid index: %u", index);
        return BranchTargetBufferEntry();
    }

    return btb.at(index);
}

// Update BTB entry with given values, at index computed from the instruction address
void BranchTargetBuffer::update(const Address instruction_address, const Address target_address, const BranchType branch_type) {
    // Get index from instruction address
    const uint16_t btb_index { calculate_index(instruction_address) };

    // Check index validity
    if (btb_index >= btb.capacity()) {
        WARN("Tried to update BTB at invalid index: %u", btb_index);
        return;
    }

    // Write new entry to the table
    const BranchTargetBufferEntry btb_entry = { 
        .entry_valid = true,
        .instruction_address = instruction_address, 
        .target_address = target_address,
        .branch_type = branch_type
    };
    btb.at(btb_index) = btb_entry;

    // Send signal with the data
    emit btb_row_updated(btb_index, btb_entry);
}

void BranchTargetBuffer::clear() {
    for (uint16_t i = 0; i < btb.capacity(); i++) {
        btb.at(i) = BranchTargetBufferEntry();
        emit btb_row_updated(i, btb.at(i));
    }
}

/////////////////////
// Predictor class //
/////////////////////

// Predictor Generic
// #################

Predictor::Predictor(
    uint8_t number_of_bht_addr_bits,
    uint8_t number_of_bht_bits,
    PredictorState initial_state)
    : number_of_bht_addr_bits(init_number_of_bht_addr_bits(number_of_bht_addr_bits))
    , number_of_bht_bits(init_number_of_bht_bits(number_of_bht_bits))
    , initial_state(initial_state) {
    bht.resize(qPow(2, number_of_bht_bits));
    clear_bht_state();
    clear_bht_stats();
}

uint8_t Predictor::init_number_of_bht_addr_bits(const uint8_t b) const {
    if (b > BP_MAX_BHT_ADDR_BITS) {
        WARN(
            "Number of BHT bits from incstruction address (%u) was larger than %d during init", b,
            BP_MAX_BHT_ADDR_BITS);
        return BP_MAX_BHT_ADDR_BITS;
    }
    return b;
}

uint8_t Predictor::init_number_of_bht_bits(const uint8_t b) const {
    if (b > BP_MAX_BHT_BITS) {
        WARN("Number of BHT bits (%u) was larger than %d during init", b, BP_MAX_BHT_BITS);
        return BP_MAX_BHT_BITS;
    }
    return b;
}

BranchResult Predictor::convert_state_to_prediction(PredictorState state) const {
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

void Predictor::update_stats(bool prediction_was_correct) {
    stats.total += 1;
    if (prediction_was_correct) {
        stats.correct += 1;
    } else {
        stats.wrong += 1;
    }
    stats.accuracy = ((stats.correct * 100) / stats.total);
    emit stats_updated(stats);
}

void Predictor::update_bht_stats(uint16_t bht_index, bool prediction_was_correct) {
    if (bht_index >= bht.capacity()) {
        WARN("Tried to access BHT at invalid index: %u", bht_index);
        return;
    }

    bht.at(bht_index).stats.total += 1;
    if (prediction_was_correct) {
        bht.at(bht_index).stats.correct += 1;
    } else {
        bht.at(bht_index).stats.wrong += 1;
    }
    bht.at(bht_index).stats.accuracy = ((bht.at(bht_index).stats.correct * 100) / bht.at(bht_index).stats.total);
    emit bht_row_updated(bht_index, bht.at(bht_index));
}

// Calculate index for addressing Branch History Table from BHR and instruction address
uint16_t Predictor::calculate_bht_index(
    const uint16_t bhr_value,
    const Address instruction_address) const {
    const uint16_t bhr_part = bhr_value << number_of_bht_addr_bits;
    const uint16_t address_mask = (1 << number_of_bht_addr_bits) - 1;
    const uint16_t address_part = ((uint16_t)(instruction_address.get_raw() >> 2)) & address_mask;
    const uint16_t index = bhr_part | address_part;
    return index;
}

void Predictor::clear_stats() {
    stats = PredictionStatistics();
    emit stats_updated(stats);
}

void Predictor::clear_bht_stats() {
    for (uint16_t i = 0; i < bht.capacity(); i++) {
        bht.at(i).stats = PredictionStatistics();
        emit bht_row_updated(i, bht.at(i));
    }
}

void Predictor::clear_bht_state() {
    for (uint16_t i = 0; i < bht.capacity(); i++) {
        bht.at(i).state = initial_state;
        emit bht_row_updated(i, bht.at(i));
    }
}

void Predictor::clear() {
    clear_stats();
    clear_bht_stats();
    clear_bht_state();
}

void Predictor::flush() {
    clear_bht_state();
}

// Always Not Taken
// ################

PredictorAlwaysNotTaken::PredictorAlwaysNotTaken() : Predictor(0, 0, PredictorState::UNDEFINED) {}

BranchResult PredictorAlwaysNotTaken::predict(PredictionInput input) {
    UNUSED(input);
    return BranchResult::NOT_TAKEN;
}

void PredictorAlwaysNotTaken::update(PredictionFeedback feedback) {
    update_stats(feedback.result == BranchResult::NOT_TAKEN);
}

// Always Taken
// ############

PredictorAlwaysTaken::PredictorAlwaysTaken() : Predictor(0, 0, PredictorState::UNDEFINED) {}

BranchResult PredictorAlwaysTaken::predict(PredictionInput input) {
    UNUSED(input);
    return BranchResult::NOT_TAKEN;
}

void PredictorAlwaysTaken::update(PredictionFeedback feedback) {
    update_stats(feedback.result == BranchResult::TAKEN);
}

// Backward Taken Forward Not Taken
// ################################

PredictorBTFNT::PredictorBTFNT() : Predictor(0, 0, PredictorState::UNDEFINED) {}

BranchResult PredictorBTFNT::predict(PredictionInput input) {
    if (input.target_address > input.instruction_address) {
        // If target address is larger than jump instruction address (forward jump),
        // predict not taken
        return BranchResult::NOT_TAKEN;
    } else {
        // Otherwise (backward jump) predict taken
        return BranchResult::TAKEN;
    }
}

void PredictorBTFNT::update(PredictionFeedback feedback) {
    if (feedback.target_address > feedback.instruction_address) {
        update_stats(feedback.result == BranchResult::NOT_TAKEN);
    } else {
        update_stats(feedback.result == BranchResult::TAKEN);
    }
}

// Smith 1 Bit
// ###########

PredictorSmith1Bit::PredictorSmith1Bit(
    uint8_t number_of_bht_addr_bits,
    uint8_t number_of_bht_bits,
    PredictorState initial_state)
    : Predictor(number_of_bht_addr_bits, number_of_bht_bits, initial_state) {};

BranchResult PredictorSmith1Bit::predict(PredictionInput input) {
    const uint16_t index { calculate_bht_index(input.bhr_value, input.instruction_address) };
    if (index >= bht.capacity()) {
        WARN("Tried to access BHT at invalid index: %u", index);
        return BranchResult::NOT_TAKEN;
    }

    // Decide prediction
    return convert_state_to_prediction(bht.at(index).state);
}

void PredictorSmith1Bit::update(PredictionFeedback feedback) {
    const uint16_t index { calculate_bht_index(feedback.bhr_value, feedback.instruction_address) };
    if (index >= bht.capacity()) {
        WARN("Tried to access BHT at invalid index: %u", index);
        return;
    }

    update_bht_stats(index, feedback.result == convert_state_to_prediction(bht.at(index).state));
    update_stats(feedback.result == convert_state_to_prediction(bht.at(index).state));

    // Update internal state
    if (feedback.result == BranchResult::NOT_TAKEN) {
        bht.at(index).state = PredictorState::NOT_TAKEN;
    } else if (feedback.result == BranchResult::TAKEN) {
        bht.at(index).state = PredictorState::TAKEN;
    } else {
        WARN("Smith 1 bit predictor has received invalid prediction result");
    }
    emit bht_row_updated(index, bht.at(index));
}

// Smith 2 Bit
// ###########

PredictorSmith2Bit::PredictorSmith2Bit(
    uint8_t number_of_bht_addr_bits,
    uint8_t number_of_bht_bits,
    PredictorState initial_state)
    : Predictor(number_of_bht_addr_bits, number_of_bht_bits, initial_state) {};

BranchResult PredictorSmith2Bit::predict(PredictionInput input) {
    const uint16_t index { calculate_bht_index(input.bhr_value, input.instruction_address) };
    if (index >= bht.capacity()) {
        WARN("Tried to access BHT at invalid index: %u", index);
        return BranchResult::NOT_TAKEN;
    }

    // Decide prediction
    return convert_state_to_prediction(bht.at(index).state);
}

void PredictorSmith2Bit::update(PredictionFeedback feedback) {
    const uint16_t index { calculate_bht_index(feedback.bhr_value, feedback.instruction_address) };
    if (index >= bht.capacity()) {
        WARN("Tried to access BHT at invalid index: %u", index);
        return;
    }

    update_bht_stats(index, feedback.result == convert_state_to_prediction(bht.at(index).state));
    update_stats(feedback.result == convert_state_to_prediction(bht.at(index).state));

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
    emit bht_row_updated(index, bht.at(index));
}

// Smith 2 Bit with hysteresis
// ###########################

PredictorSmith2BitHysteresis::PredictorSmith2BitHysteresis(
    uint8_t number_of_bht_addr_bits,
    uint8_t number_of_bht_bits,
    PredictorState initial_state)
    : Predictor(number_of_bht_addr_bits, number_of_bht_bits, initial_state) {};

BranchResult PredictorSmith2BitHysteresis::predict(PredictionInput input) {
    const uint16_t index { calculate_bht_index(input.bhr_value, input.instruction_address) };
    if (index >= bht.capacity()) {
        WARN("Tried to access BHT at invalid index: %u", index);
        return BranchResult::NOT_TAKEN;
    }

    // Decide prediction
    return convert_state_to_prediction(bht.at(index).state);
}

void PredictorSmith2BitHysteresis::update(PredictionFeedback feedback) {
    const uint16_t index { calculate_bht_index(feedback.bhr_value, feedback.instruction_address) };
    if (index >= bht.capacity()) {
        WARN("Tried to access BHT at invalid index: %u", index);
        return;
    }

    update_bht_stats(index, feedback.result == convert_state_to_prediction(bht.at(index).state));
    update_stats(feedback.result == convert_state_to_prediction(bht.at(index).state));

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
    emit bht_row_updated(index, bht.at(index));
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
        break;

    case PredictorType::ALWAYS_TAKEN:
        predictor = new PredictorAlwaysTaken();
        break;

    case PredictorType::BTFNT:
        predictor = new PredictorBTFNT();
        break;

    case PredictorType::SMITH_1_BIT:
        predictor = new PredictorSmith1Bit(number_of_bht_addr_bits, number_of_bht_bits, initial_state);
        break;

    case PredictorType::SMITH_2_BIT:
        predictor = new PredictorSmith2Bit(number_of_bht_addr_bits, number_of_bht_bits, initial_state);
        break;

    case PredictorType::SMITH_2_BIT_HYSTERESIS:
        predictor = new PredictorSmith2BitHysteresis(number_of_bht_addr_bits, number_of_bht_bits, initial_state);
        break;

    default: throw std::invalid_argument("Invalid predictor type selected");
    }

    LOG("Initialized branch predictor: %s", qPrintable(get_predictor_name().toString()));

    bhr = new BranchHistoryRegister(number_of_bhr_bits);
    btb = new BranchTargetBuffer(number_of_btb_bits);

    if (enabled) {
        // Pass through BTB signals
        connect(
            btb, &BranchTargetBuffer::btb_row_updated,
            this, &BranchPredictor::btb_row_updated
        );

        // Pass through BHR signals
        connect(
            bhr, &BranchHistoryRegister::bhr_updated,
            this, &BranchPredictor::bhr_updated
        );

        // Pass through predictor signals
        connect(
            predictor, &Predictor::stats_updated,
            this, &BranchPredictor::predictor_stats_updated
        );
        connect(
            predictor, &Predictor::bht_row_updated,
            this, &BranchPredictor::predictor_bht_row_updated
        );
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

void BranchPredictor::increment_jumps() {
    total_stats.total += 1;
    total_stats.correct = total_stats.total - total_stats.wrong;
    if (total_stats.total > 0) {
        total_stats.accuracy = ((total_stats.correct * 100) / total_stats.total);
    }
    emit total_stats_updated(total_stats);
}

void BranchPredictor::increment_mispredictions() {
    total_stats.wrong += 1;
    total_stats.correct = total_stats.total - total_stats.wrong;
    if (total_stats.total > 0) {
        total_stats.accuracy = ((total_stats.correct * 100) / total_stats.total);
    }
    emit total_stats_updated(total_stats);
}

Address BranchPredictor::predict_next_pc_address(
    const Instruction instruction,
    const Address instruction_address) const {
    // Check if predictor is enabled
    if (!enabled) { return instruction_address + 4; }

    // Read entry from BTB
    const BranchTargetBufferEntry btb_entry = btb->get_entry(instruction_address);
    if (!btb_entry.entry_valid) { return instruction_address + 4; }
    if (btb_entry.instruction_address != instruction_address) { return instruction_address + 4; }

    // Make prediction
    const PredictionInput prediction_input {
        .instruction = instruction,
        .bhr_value = bhr->get_value(),
        .instruction_address = instruction_address,
        .target_address = btb_entry.target_address,
    };
    BranchResult predicted_result{ BranchResult::UNDEFINED };
    if (btb_entry.branch_type == BranchType::BRANCH) {
        predicted_result = predictor->predict(prediction_input);
    } else {
        predicted_result = BranchResult::TAKEN;
    }
    
    emit prediction_done(
        btb->calculate_index(instruction_address),
        predictor->calculate_bht_index(bhr->get_value(), instruction_address),
        prediction_input,
        predicted_result,
        btb_entry.branch_type);

    // If the branch was predicted Taken
    if (predicted_result == BranchResult::TAKEN) { return btb_entry.target_address; }

    // Default prediction - Not Taken
    return instruction_address + 4;
}

// Function for updating the predictor and the Branch History Register (BHR)
void BranchPredictor::update(
    const Instruction instruction,
    const Address instruction_address,
    const Address target_address,
    const BranchType branch_type,
    const BranchResult result) {
    // Check if predictor is enabled
    if (!enabled) { return; }

    // Update Branch Target Buffer
    btb->update(instruction_address, target_address, branch_type);

    // Update predictor only for conditional branches
    const PredictionFeedback prediction_feedback { 
        .instruction = instruction,
        .bhr_value = bhr->get_value(),
        .instruction_address = instruction_address,
        .target_address = target_address,
        .result = result,
        .branch_type = branch_type
    };
    if (branch_type == BranchType::BRANCH) {
        predictor->update(prediction_feedback);
    }

    increment_jumps();

    emit update_done(
        btb->calculate_index(instruction_address),
        predictor->calculate_bht_index(bhr->get_value(), instruction_address),
        prediction_feedback);

    // Update global branch history
    bhr->update(result);
}

void BranchPredictor::clear() {
    bhr->clear();
    btb->clear();
    predictor->clear();
    emit cleared();
}

void BranchPredictor::flush() {
    bhr->clear();
    btb->clear();
    predictor->flush();
    emit flushed();
}
