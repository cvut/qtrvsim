#ifndef QTRVSIM_INSTRUCTIONFLAG_H
#define QTRVSIM_INSTRUCTIONFLAG_H

namespace machine {
enum InstructionFlags {
    IMF_SUPPORTED = 1L << 0, /**< Instruction is supported */
    IMF_MEMWRITE = 1L << 1,  /**< Write to the memory when memory stage is reached */
    IMF_MEMREAD = 1L << 2,   /**< Read from the memory when memory stage is reached */
    IMF_ALUSRC = 1L << 3,    /**< The second ALU source is immediate operand */
    IMF_REGWRITE = 1L << 5,  /**< Instruction result (ALU or memory) is written to register file */
    IMF_MEM = 1L << 11,      /**< Instruction is memory access instruction */
    IMF_ALU_REQ_RS = 1L << 12, /**< Execution phase/ALU requires RS value */
    IMF_ALU_REQ_RT = 1L << 13, /**< Execution phase/ALU/mem requires RT value */
    IMF_BRANCH = 1L << 17, /**< Operation is conditional or unconditional branch or branch and link
                              when PC_TO_R31 is set */
    IMF_JUMP = 1L << 18,   /**< Jump operation - JAL, JALR */
    IMF_BJ_NOT = 1L << 19, /**< Negate condition for branch instruction */
    IMF_BRANCH_JALR = 1L << 20, /**< Use ALU output as branch/jump target. Used by JALR. */
    IMF_EXCEPTION = 1L << 22,   /**< Instruction causes synchronous exception */
    IMF_ALU_MOD = 1L << 24,     /**< ADD and right-shift modifier */
    IMF_PC_TO_ALU = 1L << 25,   /**< PC is loaded instead of RS to ALU */
    IMF_ECALL = 1L << 26,       // seems easiest to encode ecall and ebreak as flags, but they might
    IMF_EBREAK = 1L << 27,      // be moved elsewhere in case we run out of InstructionFlag space.

    // Extensions:
    // =============================================================================================
    // RV64/32M
    IMF_MUL = 1L << 28, /**< Enables multiplication component of ALU. */
    // TODO do we want to add those signals to the visualization?
};
}

#endif // QTRVSIM_INSTRUCTIONFLAG_H
