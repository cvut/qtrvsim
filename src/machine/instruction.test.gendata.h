#ifndef INSTRUCTION_TEST_GENDATA_H
#define INSTRUCTION_TEST_GENDATA_H

#include "machine/instruction.h"
#include "machine/instruction.cpp"

#include <QFile>
#include <QMap>
#include <QRandomGenerator>

using namespace machine;

struct GeneratedInst {
    uint32_t code;
    QString string_data;

    const InstructionMap *im;
    const InstructionMap *im_source;
};

GeneratedInst random_inst_from_im(const InstructionMap *im, const InstructionMap *im_source);
uint32_t random_arg_code_mask(const ArgumentDesc *arg_desc, const InstructionMap *im, QString *string_data);
void WalkInstructionMap(std::function<void(const InstructionMap *, const InstructionMap *)> handler);
void WalkInstructionMapHelper(
    const InstructionMap *im_iter,
    BitField subfield,
    const InstructionMap *parent,
    std::function<void(const InstructionMap *, const InstructionMap *)> handler);

bool fill_argdesbycode();
void instruction_from_string_build_base();
QString field_to_string(int32_t field, const ArgumentDesc* arg_desc, Address inst_addr, bool symbolic_registers_enabled);

static const BitField MASK_R_RD = { 5, 7 };
static const BitField MASK_R_RS1 = { 5, 15 };
static const BitField MASK_R_RS2 = { 5, 20 };

static const BitField MASK_AMO_RD = { 5, 7 };
static const BitField MASK_AMO_RS1 = { 5, 15 };
static const BitField MASK_AMO_RS2 = { 5, 20 };

static const BitField MASK_I_RD = { 5, 7 };
static const BitField MASK_I_RS1 = { 5, 15 };
static const BitField MASK_I_IMM = { 12, 20 };

static const BitField MASK_ZICSR_RD = { 5, 7 };
static const BitField MASK_ZICSR_RS1 = { 5, 15 };
static const BitField MASK_ZICSR_CSR = { 12, 20 };

static const BitField MASK_S_IMM_0_4 = { 4, 7 };
static const BitField MASK_S_IMM_5_11 = { 7, 25 };
static const BitField MASK_S_IMM_12 = { 1, 31 };
static const BitField MASK_S_RS1 = { 5, 15 };
static const BitField MASK_S_RS2 = { 5, 20 };

static const BitField MASK_B_IMM_1_4 = { 4, 8 };
static const BitField MASK_B_IMM_11 = { 1, 7 };
static const BitField MASK_B_IMM_5_10 = { 6, 25 };
static const BitField MASK_B_IMM_12 = { 1, 31 };
static const BitField MASK_B_RS1 = { 5, 15 };
static const BitField MASK_B_RS2 = { 5, 20 };

static const BitField MASK_U_RD = { 5, 7 };
static const BitField MASK_U_IMM = { 20, 12 };

static const BitField MASK_J_RD = { 5, 7 };
static const BitField MASK_J_IMM_12_19 = { 8, 12 };
static const BitField MASK_J_IMM_11 = { 1, 20 };
static const BitField MASK_J_IMM_1_10 = { 31, 21 };
static const BitField MASK_J_IMM_20 = { 1, 31 };

#define AMO_MAP_4ITEMS(NAME, MASK)                                                                 \
    { NAME, { MASK } }, { NAME ".aq", { MASK } }, { NAME ".rl", { MASK } }, {                      \
        NAME ".aqrl", {                                                                            \
            MASK                                                                                   \
        }                                                                                          \
    }

struct ADDITIONAL_TB {
    uint64_t zero_mask;
    uint64_t value = 0;
    uint64_t alias = false;
};
static const QMap<QString, ADDITIONAL_TB> zero_mask_tb = {
    // lr.w/d rs2 = 0
    AMO_MAP_4ITEMS("lr.w", ~MASK_AMO_RS2.mask()),
    AMO_MAP_4ITEMS("lr.d", ~MASK_AMO_RS2.mask()),
    // csrw(alias of csrrs) rs1 = 0
    { "csrrs", { ~MASK_I_RS1.mask() } }
};

#endif