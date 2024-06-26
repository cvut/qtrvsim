#include <instruction.test.gendata.h>

uint32_t
random_arg_code_mask(const ArgumentDesc *arg_desc, const InstructionMap *im, QString *string_data) {
    int32_t field
        // = QRandomGenerator::global()->generate();
        = QRandomGenerator::global()->bounded(int32_t(arg_desc->min), int32_t(arg_desc->max));

    // set fields from value
    uint32_t code_mask = arg_desc->arg.encode(field);

    switch (arg_desc->kind) {
    case 'E': {
        field = field % CSR::Id::_COUNT;
        CSR::RegisterDesc reg = CSR::REGISTERS[field];
        field = reg.address.data;
        code_mask = arg_desc->arg.encode(reg.address.data);
    }
    }
    // set to zero if needed
    if (zero_mask_tb.count(im->name)) { code_mask &= zero_mask_tb[im->name].zero_mask; }

    uint32_t decoded_value = arg_desc->arg.decode(code_mask);
    *string_data = field_to_string(decoded_value, arg_desc, Address(0), false);
    return code_mask;
}

GeneratedInst random_inst_from_im(const InstructionMap *im, const InstructionMap *im_source) {
    if (im->flags & InstructionFlags::IMF_CSR) {}

    uint32_t code = im->code;
    QString string_data = im->name;

    code &= im->mask;

    QString next_delim = " ";
    for (const QString &arg_string : im->args) {
        string_data += next_delim;
        next_delim = ", ";
        for (int pos = 0; pos < arg_string.size(); pos += 1) {
            char arg_letter = arg_string[pos].toLatin1();
            const ArgumentDesc *arg_desc = arg_desc_by_code[(unsigned char)arg_letter];
            if (arg_desc == nullptr) {
                string_data += arg_letter;
                continue;
            }

            QString field = 0;
            const uint32_t code_mask = random_arg_code_mask(arg_desc, im, &field);
            string_data += field;

            code |= code_mask;
        }
    }
    return GeneratedInst { code, string_data, im, im_source };
}

void WalkInstructionMapHelper(
    const InstructionMap *im_iter,
    BitField subfield,
    const InstructionMap *im_source,
    std::function<void(const InstructionMap *, const InstructionMap *)> handler) {
    for (int i = 0;; i++) {
        if (im_source == nullptr) {
            // end of subclass table
            if (i >= (1 << subfield.count)) { break; }
        } else {
            // end of alias
            if (im_iter[i].name == nullptr) { break; }
        }

        const InstructionMap *im = &im_iter[i];

        if (im->subclass != nullptr) {
            // walk into subclass
            WalkInstructionMapHelper(im->subclass, im->subfield, nullptr, handler);
            continue;
        }

        // handle alias
        if (im->aliases != nullptr) {
            WalkInstructionMapHelper(im->aliases, im->subfield, im, handler);
        }

        Instruction::Type t = im->type;
        if (im_source != nullptr) { t = im_source->type; }
        if (t == IT_UNKNOWN) { continue; }

        // handle self
        handler(im, im_source);
    }
}

void WalkInstructionMap(
    std::function<void(const InstructionMap *, const InstructionMap *)> handler) {
    return WalkInstructionMapHelper(C_inst_map, instruction_map_opcode_field, nullptr, handler);
}

int main() {
    fill_argdesbycode();
    instruction_from_string_build_base();
    WalkInstructionMap([](const InstructionMap *im, const InstructionMap *im_source) {
        printf("\ninst: [%s] \n\n", im->name);
        for (int i = 0; i < 100; i++) {
            printf("  - [%2d]", i);
            auto generated = random_inst_from_im(im, im_source);
            if (generated.im != nullptr) {
                printf(
                    "    code: [%10x] str: [%30s]\n", generated.code,
                    qPrintable(generated.string_data));

                try {
                    uint32_t parsed_code;
                    // mv 0, 0 => nop, which buffer is shorter
                    Instruction::code_from_string(
                        &parsed_code, 20, generated.string_data, Address(0x0));

                    // check code_from_string
                    if (parsed_code != generated.code) {
                        throw Instruction::ParseError("code_from_string not match");
                    }

                    if (im->flags & IMF_CSR || im_source != nullptr) {
                        // alias inst || csr inst
                    } else {
                        // base inst
                        // check to_str()
                        QString parsed_string_data = Instruction(generated.code).to_str();
                        if (parsed_string_data != generated.string_data) {
                            throw Instruction::ParseError("to_string not match");
                        }
                    }
                } catch (const Instruction::ParseError &e) { 
                    // QFAIL
                    throw(e);
                }
            }
        }
    });
    return 0;
}