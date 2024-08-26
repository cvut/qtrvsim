#ifndef PREDICTOR_H
#define PREDICTOR_H

#include "common/logging.h"
#include "instruction.h"
#include "memory/address.h"
#include "predictor_types.h"

#include <QObject>
#include <QtMath>

namespace machine {

QStringView branch_result_to_string(const BranchResult result, const bool abbrv = false);

QStringView predictor_state_to_string(const PredictorState state, const bool abbrv = false);

QStringView predictor_type_to_string(const PredictorType type);

QStringView branch_type_to_string(const BranchType type);

QString addr_to_hex_str(const machine::Address address);

bool is_predictor_type_dynamic(const PredictorType type);

/////////////////////////////////
// BranchHistoryRegister class //
/////////////////////////////////

class BranchHistoryRegister final : public QObject {
    Q_OBJECT

public: // Constructors & Destructor
    explicit BranchHistoryRegister(const uint8_t number_of_bits);

private: // Internal functions
    uint8_t init_number_of_bits(const uint8_t b) const;
    uint16_t init_register_mask(const uint8_t b) const;

public: // General functions
    uint8_t get_number_of_bits() const;
    uint16_t get_register_mask() const;
    uint16_t get_value() const;
    void update(const BranchResult result);
    void clear();

signals:
    void bhr_updated(uint8_t number_of_bhr_bits, uint16_t register_value);

private: // Internal variables
    const uint8_t number_of_bits;
    const uint16_t register_mask;
    uint16_t value { 0 };
};

/////////////////////////////
// BranchTargetBuffer class //
/////////////////////////////

struct BranchTargetBufferEntry {
    bool entry_valid{ false };
    Address instruction_address{ Address::null() };
    Address target_address{ Address::null() };
    BranchType branch_type{ BranchType::UNDEFINED };
};

class BranchTargetBuffer final : public QObject {
    Q_OBJECT

public: // Constructors & Destructor
    explicit BranchTargetBuffer(const uint8_t number_of_bits);

private: // Internal functions
    uint8_t init_number_of_bits(const uint8_t b) const;

public: // General functions
    uint8_t get_number_of_bits() const;
    uint16_t calculate_index(const Address instruction_address) const;
    BranchTargetBufferEntry get_entry(const Address instruction_address) const;
    void update(const Address instruction_address, const Address target_address, const BranchType branch_type);
    void clear();

signals:
    void btb_row_updated(uint16_t index, BranchTargetBufferEntry btb_entry) const;

private: // Internal variables
    const uint8_t number_of_bits;
    std::vector<BranchTargetBufferEntry> btb;
};

/////////////////////
// Predictor class //
/////////////////////

struct PredictionInput {
    Instruction instruction {};
    uint16_t bhr_value { 0 };
    Address instruction_address { Address::null() };
    Address target_address { Address::null() };
};

struct PredictionFeedback {
    Instruction instruction {};
    uint16_t bhr_value { 0 };
    Address instruction_address { Address::null() };
    Address target_address { Address::null() };
    BranchResult result { BranchResult::UNDEFINED };
    BranchType branch_type { BranchType::UNDEFINED };
};

struct PredictionStatistics {
    float accuracy { 0 }; // 0 - 100 %
    uint32_t total { 0 };
    uint32_t correct { 0 };
    uint32_t wrong { 0 };
};

struct BranchHistoryTableEntry {
    PredictorState state { PredictorState::UNDEFINED };
    PredictionStatistics stats {}; // Per-entry statistics
};

class Predictor : public QObject {
    Q_OBJECT

public: // Constructors & Destructor
    Predictor(
        uint8_t number_of_bht_addr_bits,
        uint8_t number_of_bht_bits,
        PredictorState initial_state);
    virtual ~Predictor() = default;

protected: // Internal functions
    uint8_t init_number_of_bht_addr_bits(uint8_t b) const;
    uint8_t init_number_of_bht_bits(uint8_t b) const;
    BranchResult convert_state_to_prediction(PredictorState state) const;
    void update_stats(bool prediction_was_correct);
    void update_bht_stats(uint16_t bht_index, bool prediction_was_correct);
    
public: // General functions
    uint16_t calculate_bht_index(const uint16_t bhr_value, const Address instruction_address) const;
    virtual PredictorType get_type() const = 0;
    virtual BranchResult predict(PredictionInput input) = 0; // Function which handles all actions ties
                                                             // to making a branch prediction
    virtual void update(PredictionFeedback feedback) = 0;    // Update predictor based on jump / branch
                                                             // result
    void clear_stats();
    void clear_bht_stats();
    void clear_bht_state();
    void clear();
    void flush();

signals:
    void stats_updated(PredictionStatistics stats) const;
    void bht_row_updated(uint16_t index, BranchHistoryTableEntry entry) const;

protected: // Internal variables
    const uint8_t number_of_bht_addr_bits; // Number of Branch History Table (BHT) bits taken from
                                           // instruction address
    const uint8_t number_of_bht_bits;      // Number of Branch History Table (BHT) bits
    const PredictorState initial_state;
    PredictionStatistics stats; // Total predictor statistics
    std::vector<BranchHistoryTableEntry> bht; // Branch History Table (BHT)
};

//  Static Predictor - Always predicts not taking the branch
class PredictorAlwaysNotTaken final : public Predictor {
public: // Constructors & Destructor
    PredictorAlwaysNotTaken();

public: // General functions
    PredictorType get_type() const override { return PredictorType::ALWAYS_NOT_TAKEN; };
    BranchResult predict(PredictionInput input) override;
    void update(PredictionFeedback feedback) override;
};

//  Static Predictor - Always predicts taking the branch
class PredictorAlwaysTaken final : public Predictor {
public: // Constructors & Destructor
    PredictorAlwaysTaken();

public: // General functions
    PredictorType get_type() const override { return PredictorType::ALWAYS_TAKEN; };
    BranchResult predict(PredictionInput input) override;
    void update(PredictionFeedback feedback) override;
};

// Static Predictor - Backward Taken Forward Not Taken
class PredictorBTFNT final : public Predictor {
public: // Constructors & Destructor
    PredictorBTFNT();

public: // General functions
    PredictorType get_type() const override { return PredictorType::BTFNT; };
    BranchResult predict(PredictionInput input) override;
    void update(PredictionFeedback feedback) override;
};

// Dynamic Predictor - Smith 1 Bit
class PredictorSmith1Bit final : public Predictor {
public: // Constructors & Destructor
    PredictorSmith1Bit(
        uint8_t number_of_bht_addr_bits,
        uint8_t number_of_bht_bits,
        PredictorState initial_state);

public: // General functions
    PredictorType get_type() const override { return PredictorType::SMITH_1_BIT; };
    BranchResult predict(PredictionInput input) override;
    void update(PredictionFeedback feedback) override;
};

// Dynamic Predictor - Smith 2 Bit
class PredictorSmith2Bit final : public Predictor {
public: // Constructors & Destructor
    PredictorSmith2Bit(
        uint8_t number_of_bht_addr_bits,
        uint8_t number_of_bht_bits,
        PredictorState initial_state);

public: // General functions
    PredictorType get_type() const override { return PredictorType::SMITH_2_BIT; };
    BranchResult predict(PredictionInput input) override;
    void update(PredictionFeedback feedback) override;
};

// Dynamic Predictor - Smith 2 Bit with hysteresis
class PredictorSmith2BitHysteresis final : public Predictor {
public: // Constructors & Destructor
    PredictorSmith2BitHysteresis(
        uint8_t number_of_bht_addr_bits,
        uint8_t number_of_bht_bits,
        PredictorState initial_state);

public: // General functions
    PredictorType get_type() const override { return PredictorType::SMITH_2_BIT_HYSTERESIS; };
    BranchResult predict(PredictionInput input) override;
    void update(PredictionFeedback feedback) override;
};

///////////////////////////
// BranchPredictor class //
///////////////////////////

class BranchPredictor : public QObject {
    Q_OBJECT

public: // Constructors & Destructor
    explicit BranchPredictor(
        bool enabled = false,
        PredictorType predictor_type = PredictorType::SMITH_1_BIT,
        PredictorState initial_state = PredictorState::NOT_TAKEN,
        uint8_t number_of_btb_bits = 2,
        uint8_t number_of_bhr_bits = 0,
        uint8_t number_of_bht_addr_bits = 2);
    ~BranchPredictor();

private: // Internal functions
    uint8_t init_number_of_btb_bits(const uint8_t b) const;
    uint8_t init_number_of_bhr_bits(const uint8_t b) const;
    uint8_t init_number_of_bht_addr_bits(const uint8_t b) const;
    uint8_t init_number_of_bht_bits(const uint8_t b_bhr, const uint8_t b_addr) const;
    

public: // General functions
    bool get_enabled() const;
    PredictorType get_predictor_type() const;
    QStringView get_predictor_name() const;
    PredictorState get_initial_state() const;
    uint8_t get_number_of_btb_bits() const;
    uint8_t get_number_of_bhr_bits() const;
    uint8_t get_number_of_bht_addr_bits() const;
    uint8_t get_number_of_bht_bits() const;
    void increment_jumps();
    void increment_mispredictions();
    Address predict_next_pc_address(const Instruction instruction, const Address instruction_address) const;
    void update(
        const Instruction instruction,
        const Address instruction_address,
        const Address target_address,
        const BranchType branch_type,
        const BranchResult result);
    void clear();
    void flush();

signals:
    void total_stats_updated(PredictionStatistics total_stats);
    void prediction_done(uint16_t btb_index, uint16_t bht_index, PredictionInput input, BranchResult result, BranchType branch_type) const;
    void update_done(uint16_t btb_index, uint16_t bht_index, PredictionFeedback feedback) const;
    void predictor_stats_updated(PredictionStatistics stats) const;
    void predictor_bht_row_updated(uint16_t index, BranchHistoryTableEntry entry) const;
    void bhr_updated(uint8_t number_of_bhr_bits, uint16_t register_value) const;
    void btb_row_updated(uint16_t index, BranchTargetBufferEntry btb_entry) const;
    void cleared() const; // All infomration was reset
    void flushed() const; // Only BHT state and BTB rows were reset

private: // Internal variables
    bool enabled{ false };
    PredictionStatistics total_stats;
    Predictor *predictor;
    BranchHistoryRegister *bhr;
    BranchTargetBuffer *btb;
    const PredictorState initial_state;
    const uint8_t number_of_btb_bits; // Number of bits for addressing Branch Target Buffer (all
                                      // taken from instruction address)
    const uint8_t number_of_bhr_bits; // Number of bits in Branch History Register
    const uint8_t number_of_bht_addr_bits; // Number of bits in Branch History Table which are taken
                                           // from instruction address
    const uint8_t number_of_bht_bits;      // = number_of_bhr_bits + number_of_bht_addr_bits
};

} // namespace machine

#endif // PREDICTOR_H
