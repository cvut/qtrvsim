#include "elf++.hh"
#include "dwarf++.hh"

#include <errno.h>
#include <fcntl.h>
#include <string>
#include <inttypes.h>

using namespace std;

void
usage(const char *cmd) 
{
        fprintf(stderr, "usage: %s elf-file pc\n", cmd);
        exit(2);
}

bool
find_pc(const dwarf::die &d, dwarf::taddr pc, vector<dwarf::die> *stack)
{
        using namespace dwarf;

        // Scan children first to find most specific DIE
        bool found = false;
        for (auto &child : d) {
                if ((found = find_pc(child, pc, stack)))
                        break;
        }
        switch (d.tag) {
        case DW_TAG::subprogram:
        case DW_TAG::inlined_subroutine:
                try {
                        if (found || die_pc_range(d).contains(pc)) {
                                found = true;
                                stack->push_back(d);
                        }
                } catch (out_of_range &e) {
                } catch (value_type_mismatch &e) {
                }
                break;
        default:
                break;
        }
        return found;
}

void
dump_die(const dwarf::die &node)
{
        printf("<%" PRIx64 "> %s\n",
               node.get_section_offset(),
               to_string(node.tag).c_str());
        for (auto &attr : node.attributes())
                printf("      %s %s\n",
                       to_string(attr.first).c_str(),
                       to_string(attr.second).c_str());
}

int
main(int argc, char **argv)
{
        if (argc != 3)
                usage(argv[0]);

        dwarf::taddr pc;
        try {
                pc = stoll(argv[2], nullptr, 0);
        } catch (invalid_argument &e) {
                usage(argv[0]);
        } catch (out_of_range &e) {
                usage(argv[0]);
        }

        int fd = open(argv[1], O_RDONLY);
        if (fd < 0) {
                fprintf(stderr, "%s: %s\n", argv[1], strerror(errno));
                return 1;
        }

        elf::elf ef(elf::create_mmap_loader(fd));
        dwarf::dwarf dw(dwarf::elf::create_loader(ef));

        // Find the CU containing pc
        // XXX Use .debug_aranges
        for (auto &cu : dw.compilation_units()) {
                if (die_pc_range(cu.root()).contains(pc)) {
                        // Map PC to a line
                        auto &lt = cu.get_line_table();
                        auto it = lt.find_address(pc);
                        if (it == lt.end())
                                printf("UNKNOWN\n");
                        else
                                printf("%s\n",
                                       it->get_description().c_str());

                        // Map PC to an object
                        // XXX Index/helper/something for looking up PCs
                        // XXX DW_AT_specification and DW_AT_abstract_origin
                        vector<dwarf::die> stack;
                        if (find_pc(cu.root(), pc, &stack)) {
                                bool first = true;
                                for (auto &d : stack) {
                                        if (!first)
                                                printf("\nInlined in:\n");
                                        first = false;
                                        dump_die(d);
                                }
                        }
                        break;
                }
        }

        return 0;
}
