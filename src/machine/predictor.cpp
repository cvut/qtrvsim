#include "predictor.h"

LOG_CATEGORY("machine.BranchPredictor");

using namespace machine;

Address BranchPredictor::predict_next_pc_address(
    const Instruction instruction,
    const Address instruction_address) const {
    (void)instruction; // explicitly unused argument
    // Always predicts not taking the branch, even on JAL(R) instructions
    return instruction_address + 4;
}
