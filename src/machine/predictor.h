#ifndef PREDICTOR_H
#define PREDICTOR_H

#include "predictor_types.h"
#include "instruction.h"
#include "memory/address.h"
#include "common/logging.h"

namespace machine {

class BranchPredictor {
public:
    virtual ~BranchPredictor() = default;
    Address predict_next_pc_address(const Instruction instruction, const Address instruction_address) const;
};

} // namespace machine

#endif // PREDICTOR_H
