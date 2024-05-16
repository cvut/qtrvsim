#ifndef PREDICTOR_H
#define PREDICTOR_H

#include "instruction.h"
#include "memory/address.h"

namespace machine {

class BranchPredictor {
public:
    virtual ~BranchPredictor() = default;
    Address predict_next_pc_address(
        const Instruction instruction,
        const Address instruction_address) const {
        (void)instruction; // explicitly unused argument
        // Always predicts not taking the branch, even on JAL(R) instructions
        return instruction_address + 4;
    };
};

} // namespace machine

#endif // PREDICTOR_H
