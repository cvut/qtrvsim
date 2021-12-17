#ifndef PREDICTOR_H
#define PREDICTOR_H

#include "instruction.h"
#include "memory/address.h"

namespace machine {

class Predictor {
public:
    virtual Address predict(Instruction inst, Address addr) = 0;
    virtual ~Predictor() = default;
};

// Always predicts not taking the branch, even on JAL(R) instructions
class FalsePredictor : public Predictor {
    Address predict(Instruction inst, Address addr) final {
        (void)inst; // explicitly unused argument
        return addr + 4;
    }
};

} // namespace machine

#endif // PREDICTOR_H
