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

QString addr_to_hex_str(const machine::Address address);

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

signals:
    void update_done(uint8_t number_of_bhr_bits, uint16_t register_value);

private: // Internal variables
    const uint8_t number_of_bits;
    const uint16_t register_mask;
    uint16_t value { 0 };
};

/////////////////////////////
// BranchTargetBuffer class //
/////////////////////////////

struct BranchTargetBufferEntry {
    Address instruction_address { Address::null() };
    Address target_address { Address::null() };
};

class BranchTargetBuffer final : public QObject {
    Q_OBJECT

public: // Constructors & Destructor
    explicit BranchTargetBuffer(const uint8_t number_of_bits);

private: // Internal functions
    uint8_t init_number_of_bits(const uint8_t b) const;
    uint16_t calculate_index(const Address instruction_address) const;

public: // General functions
    uint8_t get_number_of_bits() const;
    Address get_instruction_address(const Address instruction_address) const;
    Address get_target_address(const Address instruction_address) const;
    void update(const Address instruction_address, const Address target_address);

signals:
    void requested_target_address(uint16_t index) const;
    void update_row_done(uint16_t index, Address instruction_address, Address target_address) const;

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
};

struct PredictionStatistics {
    float accuracy { 0 }; // 0 - 100 %
    BranchResult last_prediction { BranchResult::UNDEFINED };
    BranchResult last_result { BranchResult::UNDEFINED };
    uint32_t number_of_correct_predictions { 0 };
    uint32_t number_of_wrong_predictions { 0 };
};

struct BranchHistoryTableEntry {
    PredictorState state;
    PredictionStatistics stats; // Per-entry statistics
};

class Predictor : public QObject {
    Q_OBJECT

public: // Constructors & Destructor
    virtual ~Predictor() = default;

protected: // Internal functions
    virtual void update_stats(PredictionFeedback feedback);
    virtual BranchResult make_prediction(PredictionInput input) const = 0; // Returns
                                                                           // prediction based on
                                                                           // internal state

public: // General functions
    virtual PredictorType get_type() const { return PredictorType::UNDEFINED; };
    virtual BranchResult predict(PredictionInput input); // Function which handles all actions ties
                                                         // to making a branch prediction
    virtual void update(PredictionFeedback feedback);    // Update predictor based on jump / branch
                                                         // result

signals:
    void prediction_done(uint16_t index, PredictionInput input, BranchResult result) const;
    void update_done(uint16_t index, PredictionFeedback feedback) const;
    void update_stats_done(PredictionStatistics stats) const;
    void update_bht_row_done(uint16_t index, BranchHistoryTableEntry entry) const;

protected:                      // Internal variables
    PredictionStatistics stats; // Total predictor statistics
};

//  Static Predictor - Always predicts not taking the branch
class PredictorAlwaysNotTaken final : public Predictor {
public: // Constructors & Destructor
    PredictorAlwaysNotTaken();

private: // Internal functions
    BranchResult make_prediction(PredictionInput input) const override;

public: // General functions
    PredictorType get_type() const override { return PredictorType::ALWAYS_NOT_TAKEN; };
};

//  Static Predictor - Always predicts taking the branch
class PredictorAlwaysTaken final : public Predictor {
public: // Constructors & Destructor
    PredictorAlwaysTaken();

private: // Internal functions
    BranchResult make_prediction(PredictionInput input) const override;

public: // General functions
    PredictorType get_type() const override { return PredictorType::ALWAYS_TAKEN; };
};

// Static Predictor - Backward Taken Forward Not Taken
class PredictorBTFNT final : public Predictor {
public: // Constructors & Destructor
    PredictorBTFNT();

private: // Internal functions
    BranchResult make_prediction(PredictionInput input) const override;

public: // General functions
    PredictorType get_type() const override { return PredictorType::BTFNT; };
    virtual void update(PredictionFeedback feedback) override;
};

// Dynamic Predictor - Smith Generic
class PredictorSmith : public Predictor {
public: // Constructors & Destructor
    PredictorSmith(
        uint8_t number_of_bht_addr_bits,
        uint8_t number_of_bht_bits,
        PredictorState initial_state);

protected: // Internal functions
    uint8_t init_number_of_bht_addr_bits(uint8_t b) const;
    uint8_t init_number_of_bht_bits(uint8_t b) const;
    uint16_t calculate_bht_index(const uint16_t bhr_value, const Address instruction_address) const;
    void update_stats(PredictionFeedback feedback) override;
    BranchResult convert_state_to_prediction(PredictorState state) const;
    virtual void update_bht(PredictionFeedback feedback) = 0;

public:                                                   // General functions
    BranchResult predict(PredictionInput input) override; // Function which handles all actions ties
                                                          // to making a branch prediction
    void update(PredictionFeedback feedback) override;    // Update predictor based on jump / branch
                                                          // result

protected:                                 // Internal variables
    const uint8_t number_of_bht_addr_bits; // Number of Branch History Table (BHT) bits taken from
                                           // instruction address
    const uint8_t number_of_bht_bits;      // Number of Branch History Table (BHT) bits
    std::vector<BranchHistoryTableEntry> bht; // Branch History Table (BHT)
};

// Dynamic Predictor - Smith 1 Bit
class PredictorSmith1Bit final : public PredictorSmith {
public: // Constructors & Destructor
    PredictorSmith1Bit(
        uint8_t number_of_bht_addr_bits,
        uint8_t number_of_bht_bits,
        PredictorState initial_state);

private: // Internal functions
    BranchResult make_prediction(PredictionInput input) const override;
    void update_bht(PredictionFeedback feedback);

public: // General functions
    PredictorType get_type() const override { return PredictorType::SMITH_1_BIT; };
};

// Dynamic Predictor - Smith 2 Bit
class PredictorSmith2Bit final : public PredictorSmith {
public: // Constructors & Destructor
    PredictorSmith2Bit(
        uint8_t number_of_bht_addr_bits,
        uint8_t number_of_bht_bits,
        PredictorState initial_state);

private: // Internal functions
    BranchResult make_prediction(PredictionInput input) const override;
    void update_bht(PredictionFeedback feedback);

public: // General functions
    PredictorType get_type() const override { return PredictorType::SMITH_2_BIT; };
};

// Dynamic Predictor - Smith 2 Bit with hysteresis
class PredictorSmith2BitHysteresis final : public PredictorSmith {
public: // Constructors & Destructor
    PredictorSmith2BitHysteresis(
        uint8_t number_of_bht_addr_bits,
        uint8_t number_of_bht_bits,
        PredictorState initial_state);

private: // Internal functions
    BranchResult make_prediction(PredictionInput input) const override;
    void update_bht(PredictionFeedback feedback);

public: // General functions
    PredictorType get_type() const override { return PredictorType::SMITH_2_BIT_HYSTERESIS; };
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

    Address
    predict_next_pc_address(const Instruction instruction, const Address instruction_address) const;
    void update(
        const Instruction instruction,
        const Address instruction_address,
        const Address target_address,
        const BranchResult result);

signals:
    void requested_bht_target_address(uint16_t index) const;
    void
    update_btb_row_done(uint16_t index, Address instruction_address, Address target_address) const;
    void update_bhr_done(uint8_t number_of_bhr_bits, uint16_t register_value) const;
    void prediction_done(uint16_t index, PredictionInput input, BranchResult result) const;
    void update_predictor_done(uint16_t index, PredictionFeedback feedback) const;
    void update_predictor_stats_done(PredictionStatistics stats) const;
    void update_predictor_bht_row_done(uint16_t index, BranchHistoryTableEntry entry) const;

private: // Internal variables
    bool enabled;
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
