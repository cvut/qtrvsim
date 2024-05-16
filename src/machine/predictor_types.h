#ifndef PREDICTOR_TYPES_H
#define PREDICTOR_TYPES_H

namespace machine {

// Should not exceed 16, because uint16_t is used for addressing
#define PREDICTOR_MAX_TABLE_BITS 16

enum class BranchResult { NOT_TAKEN, TAKEN, UNDEFINED };

enum class PredictorType {
    ALWAYS_NOT_TAKEN,
    ALWAYS_TAKEN,
    BTFNT, // Backward Taken, Forward Not Taken
    SMITH_1_BIT,
    SMITH_2_BIT,
    SMITH_2_BIT_HYSTERESIS,
    UNDEFINED
};

enum class PredictorState {
    NOT_TAKEN,          // Smith 1 bit
    TAKEN,              // Smith 1 bit
    STRONGLY_NOT_TAKEN, // Smith 2 bit
    WEAKLY_NOT_TAKEN,   // Smith 2 bit
    WEAKLY_TAKEN,       // Smith 2 bit
    STRONGLY_TAKEN,     // Smith 2 bit
    UNDEFINED
};

} // namespace machine

#endif // PREDICTOR_TYPES_H
