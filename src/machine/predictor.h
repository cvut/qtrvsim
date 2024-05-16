#ifndef PREDICTOR_H
#define PREDICTOR_H

#include "common/logging.h"
#include "instruction.h"
#include "memory/address.h"
#include "predictor_types.h"

#include <QObject>
#include <QtMath>

namespace machine {

const char *int_to_bit_string(const uint64_t value, const uint8_t size = 8);

const char *branch_result_to_string(const BranchResult result);

const char *predictor_state_to_string(const PredictorState state);

QString addr_to_hex_str(const machine::Address address);

/////////////////////////////////
// BranchHistoryRegister class //
/////////////////////////////////

class BranchHistoryRegister final : public QObject {
    Q_OBJECT

public: // Constructors & Destructor
    explicit BranchHistoryRegister(const uint8_t number_of_bits);
    ~BranchHistoryRegister();

private: // Internal functions
    uint8_t init_number_of_bits(const uint8_t number_of_bits) const;
    uint16_t init_register_mask(const uint8_t number_of_bits) const;

public: // General functions
    uint8_t get_number_of_bits() const;
    uint16_t get_register_mask() const;
    uint16_t get_value() const;
    void update(const BranchResult result);

signals:
    void updated(uint8_t number_of_bhr_bits, uint16_t register_value);

private: // Internal variables
    const uint8_t number_of_bits;
    const uint16_t register_mask;
    uint16_t value { 0 };
};

/////////////////////////////
// BranchTargetTable class //
/////////////////////////////

struct BranchTargetTableEntry {
    Address instruction_address { Address::null() };
    Address target_address { Address::null() };
};

class BranchTargetTable final : public QObject {
    Q_OBJECT

public: // Constructors & Destructor
    explicit BranchTargetTable(const uint8_t number_of_bits);
    ~BranchTargetTable();

private: // Internal functions
    uint8_t init_number_of_bits(const uint8_t number_of_bits) const;

public: // General functions
    uint8_t get_number_of_bits() const;
    void print() const;
    Address get_target_address(const Address instruction_address) const;
    bool is_address_in_table(const Address instruction_address) const;
    void
    update(const uint16_t index, const Address instruction_address, const Address target_address);

signals:
    void updated_row(uint16_t index, Address instruction_address, Address target_address) const;

private: // Internal variables
    const uint8_t number_of_bits;
    std::vector<BranchTargetTableEntry> data;
};

/////////////////////
// Predictor class //
/////////////////////

struct PredictionInput {
    Instruction instruction {};
    Address instruction_address { Address::null() };
    Address target_address { Address::null() };
    uint16_t bht_index { 0 };
};

struct PredictionFeedback {
    Instruction instruction {};
    Address instruction_address { Address::null() };
    Address target_address { Address::null() };
    uint16_t bht_index { 0 };
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
    virtual BranchResult make_prediction(PredictionInput input) const = 0; // Function which returns
                                                                           // prediction based on
                                                                           // internal state

public: // General functions
    virtual PredictorType get_type() const = 0;
    virtual const char *get_name() const = 0;            // Returns name of the predictor as string
    virtual BranchResult predict(PredictionInput input); // Function which handles all actions ties
                                                         // to making a branch prediction
    virtual void update(PredictionFeedback feedback);    // Update predictor based on jump / branch
                                                         // result

signals:
    void new_prediction(PredictionInput input, BranchResult result) const;
    void new_update(PredictionFeedback feedback) const;
    void updated_stats(PredictionStatistics stats) const;
    void updated_bht_row(uint16_t index, BranchHistoryTableEntry entry) const;

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
    const char *get_name() const override { return "Always not taken"; };
};

//  Static Predictor - Always predicts taking the branch
class PredictorAlwaysTaken final : public Predictor {
public: // Constructors & Destructor
    PredictorAlwaysTaken();

private: // Internal functions
    BranchResult make_prediction(PredictionInput input) const override;

public: // General functions
    PredictorType get_type() const override { return PredictorType::ALWAYS_TAKEN; };
    const char *get_name() const override { return "Always taken"; };
};

// Static Predictor - Backward Taken Forward Not Taken
class PredictorBTFNT final : public Predictor {
public: // Constructors & Destructor
    PredictorBTFNT();

private: // Internal functions
    BranchResult make_prediction(PredictionInput input) const override;

public: // General functions
    PredictorType get_type() const override { return PredictorType::BTFNT; };
    const char *get_name() const override { return "Backward Taken Forward Not Taken"; };
    virtual void update(PredictionFeedback feedback) override;
};

// Dynamic Predictor - Smith Generic
class PredictorSmith : public Predictor {
public: // Constructors & Destructor
    PredictorSmith(uint8_t number_of_bht_bits, PredictorState initial_state);

protected: // Internal functions
    uint8_t init_number_of_bht_bits(uint8_t number_of_bht_bits) const;
    void update_stats(PredictionFeedback feedback) override;
    BranchResult state_to_prediction(PredictorState state) const;
    virtual void update_bht(PredictionFeedback feedback) = 0;

public:                                                   // General functions
    BranchResult predict(PredictionInput input) override; // Function which handles all actions ties
                                                          // to making a branch prediction
    void update(PredictionFeedback feedback) override;    // Update predictor based on jump / branch
                                                          // result

protected:                                    // Internal variables
    const uint8_t number_of_bht_bits;         // Number of Branch History Table (BHT) bits
    std::vector<BranchHistoryTableEntry> bht; // Branch History Table (BHT)
};

// Dynamic Predictor - Smith 1 Bit
class PredictorSmith1Bit final : public PredictorSmith {
public: // Constructors & Destructor
    PredictorSmith1Bit(uint8_t number_of_bht_bits, PredictorState initial_state);

private: // Internal functions
    BranchResult make_prediction(PredictionInput input) const override;
    void update_bht(PredictionFeedback feedback);

public: // General functions
    PredictorType get_type() const override { return PredictorType::SMITH_1_BIT; };
    const char *get_name() const override { return "Smith 1 bit"; };
};

// Dynamic Predictor - Smith 2 Bit
class PredictorSmith2Bit final : public PredictorSmith {
public: // Constructors & Destructor
    PredictorSmith2Bit(uint8_t number_of_bht_bits, PredictorState initial_state);

private: // Internal functions
    BranchResult make_prediction(PredictionInput input) const override;
    void update_bht(PredictionFeedback feedback);

public: // General functions
    PredictorType get_type() const override { return PredictorType::SMITH_2_BIT; };
    const char *get_name() const override { return "Smith 2 bit"; };
};

// Dynamic Predictor - Smith 2 Bit with hysteresis
class PredictorSmith2BitHysteresis final : public PredictorSmith {
public: // Constructors & Destructor
    PredictorSmith2BitHysteresis(uint8_t number_of_bht_bits, PredictorState initial_state);

private: // Internal functions
    BranchResult make_prediction(PredictionInput input) const override;
    void update_bht(PredictionFeedback feedback);

public: // General functions
    PredictorType get_type() const override { return PredictorType::SMITH_2_BIT_HYSTERESIS; };
    const char *get_name() const override { return "Smith 2 bit with hysteresis"; };
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
        uint8_t number_of_bhr_bits = 0,
        uint8_t number_of_address_bits = 2);
    ~BranchPredictor();

private: // Internal functions
    uint8_t init_number_of_bhr_bits(const uint8_t number_of_bhr_bits) const;
    uint8_t init_number_of_address_bits(
        const uint8_t number_of_bhr_bits,
        const uint8_t number_of_address_bits) const;
    uint8_t init_number_of_table_bits(
        const uint8_t number_of_bhr_bits,
        const uint8_t number_of_address_bits) const;

public: // General functions
    bool get_enabled() const;
    PredictorType get_predictor_type() const;
    const char *get_predictor_name() const;
    PredictorState get_initial_state() const;
    uint8_t get_number_of_bhr_bits() const;
    uint8_t get_number_of_address_bits() const;
    uint8_t get_number_of_table_bits() const;

    uint16_t calculate_index(const Address instruction_address) const;
    Address
    predict_next_pc_address(const Instruction instruction, const Address instruction_address) const;
    void update_target(const Address instruction_address, const Address target_address);
    void update_result(
        const Instruction instruction,
        const Address instruction_address,
        const BranchResult result);

signals:
    void new_prediction(PredictionInput input, BranchResult result) const;
    void new_update(PredictionFeedback feedback) const;
    void updated_bhr(uint8_t number_of_bhr_bits, uint16_t register_value) const;
    void updated_btt_row(uint16_t index, Address instruction_address, Address target_address) const;
    void updated_predictor_stats(PredictionStatistics stats) const;
    void updated_bht_row(uint16_t index, BranchHistoryTableEntry entry) const;

private: // Internal variables
    bool enabled;
    Predictor *predictor;
    BranchHistoryRegister *bhr;
    BranchTargetTable *btt;
    const PredictorState initial_state;
    const uint8_t number_of_bhr_bits;     // Number of Branch History Register bits (used for BTT
                                          // addressing)
    const uint8_t number_of_address_bits; // Number of Program Counter address bits (used for BTT
                                          // addressing)
    const uint8_t number_of_table_bits;   // Sum of BHR and address bits (specifies size of Branch
                                          // Target and Branch History tables)
};

} // namespace machine

#endif // PREDICTOR_H
