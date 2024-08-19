#ifndef PREDICTOR_TYPES_H
#define PREDICTOR_TYPES_H

namespace machine {
Q_NAMESPACE

// Should not exceed 16, because uint16_t is used for addressing
#define BP_MAX_BTB_BITS 8
#define BP_MAX_BHR_BITS 8
#define BP_MAX_BHT_ADDR_BITS 8
#define BP_MAX_BHT_BITS (BP_MAX_BHT_ADDR_BITS + BP_MAX_BHT_ADDR_BITS)

enum class BranchType {
    JUMP, // JAL, JALR - Unconditional
    BRANCH,   // BXX - Conditional
    UNDEFINED
};
Q_ENUM_NS(machine::BranchType)

enum class BranchResult { 
    NOT_TAKEN,
    TAKEN,
    UNDEFINED
};
Q_ENUM_NS(machine::BranchResult)

enum class PredictorType {
    ALWAYS_NOT_TAKEN,
    ALWAYS_TAKEN,
    BTFNT, // Backward Taken, Forward Not Taken
    SMITH_1_BIT,
    SMITH_2_BIT,
    SMITH_2_BIT_HYSTERESIS,
    UNDEFINED
};
Q_ENUM_NS(machine::PredictorType)

enum class PredictorState {
    NOT_TAKEN,          // Smith 1 bit
    TAKEN,              // Smith 1 bit
    STRONGLY_NOT_TAKEN, // Smith 2 bit
    WEAKLY_NOT_TAKEN,   // Smith 2 bit
    WEAKLY_TAKEN,       // Smith 2 bit
    STRONGLY_TAKEN,     // Smith 2 bit
    UNDEFINED
};
Q_ENUM_NS(machine::PredictorState)

} // namespace machine

#endif // PREDICTOR_TYPES_H
