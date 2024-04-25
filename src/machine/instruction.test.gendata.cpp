#include "common/logging.h"
#include "machine/instruction.h"
#include "machine/instruction.cpp"
#include <QFile>
#include <unordered_set>

using namespace machine;

static uint32_t MASK_AMO_RS2 = 0b1111100000000000000000000;

#define AMO_MAP_4ITEMS(NAME_BASE, MASK) \
    { NAME_BASE, MASK}, \
    { NAME_BASE ".rl", MASK }, \
    { NAME_BASE ".aq", MASK }, \
    { NAME_BASE ".aqrl", MASK },

static std::unordered_map<QString, uint32_t> mask_map = {
    AMO_MAP_4ITEMS("lr.w", MASK_AMO_RS2)
    AMO_MAP_4ITEMS("lr.d", MASK_AMO_RS2)
};

void generate_code_and_string_data(QTextStream& out, const InstructionMap* im_iter, BitField subfield) {
    for (int i = 0; i < (1 << subfield.count); i++) {
        const InstructionMap* im = &im_iter[i];

        if (im->name == "unknown") {
            continue;
        }

        if (im->subclass != nullptr) {
            generate_code_and_string_data(out, im->subclass, im->subfield);
            continue;
        }

        uint32_t code = im->code;
        QString string_data = im->name;

        if (im->args.size()) {
            int val_mask;
            switch (im->type) {
            case Instruction::Type::R:
            case Instruction::Type::AMO:  {
                val_mask = 0b00000000000100001000000010000000;
                break;
            }
            case Instruction::Type::I: 
                val_mask = 0b00000000000100001000000010000000;
                break;
            case Instruction::Type::ZICSR: {
                val_mask = 0b00000000000100001000000010000000;
                break;
            }
            case Instruction::Type::S: {
                val_mask = 0b00000000000100001000000010000000;
                break;
            }
            case Instruction::Type::B: {
                val_mask = 0b00000000000100001000000100000000;
                break;
            }
            case Instruction::Type::U: {
                val_mask = 0b00000000000000000001000010000000;
                break;
            }
            case Instruction::Type::J: {
                val_mask = 0b00000000001000000000000010000000;
                break;
            }
            case Instruction::Type::UNKNOWN: {
                return;
            }
            }
            code |= val_mask & ~im->mask;
            if (mask_map.count(im->name)) {
                uint32_t old_code = code; 
                code &= ~mask_map[im->name];
            }

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
                    switch (arg_desc->kind) {
                    case 'g': {
                        string_data += "x1";
                        break;
                    }
                    case 'p':
                    case 'a':
                        string_data += QString::asprintf("0x%d", Instruction(code).immediate());
                        break;
                    case 'o':
                    case 'n': {
                        if (arg_desc->min < 0) {
                            string_data += "1";
                        } else {
                            string_data += "0x1";
                        }
                        break;
                    }
                    case 'E': {
                        string_data += "0x1";
                        break;
                    }
                    }
                }
            }
        }

        QString enum_str = QString::asprintf("{0x%x, \"%s\"},", code, qPrintable(string_data));
        if (Instruction(code).to_str() != string_data) {
            enum_str += " // failed";
        }
        enum_str += "\n";
        out << enum_str;
    }
}

int main(int argc, char *argv[]) {
    fill_argdesbycode();
    instruction_from_string_build_base();
    QFile outfile("instruction.test.data.h");
    if (outfile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&outfile);
        generate_code_and_string_data(out, C_inst_map, instruction_map_opcode_field);
        outfile.close();
    } else {
        printf("open output file failed\n");
        exit(1);
    }
}