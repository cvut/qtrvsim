#include "elf++.hh"
#include "dwarf++.hh"

#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>

using namespace std;

void
dump_tree(const dwarf::die &node, int depth = 0)
{
        printf("%*.s<%" PRIx64 "> %s\n", depth, "",
               node.get_section_offset(),
               to_string(node.tag).c_str());
        for (auto &attr : node.attributes())
                printf("%*.s      %s %s\n", depth, "",
                       to_string(attr.first).c_str(),
                       to_string(attr.second).c_str());
        for (auto &child : node)
                dump_tree(child, depth + 1);
}

int
main(int argc, char **argv)
{
        if (argc != 2) {
                fprintf(stderr, "usage: %s elf-file\n", argv[0]);
                return 2;
        }

        int fd = open(argv[1], O_RDONLY);
        if (fd < 0) {
                fprintf(stderr, "%s: %s\n", argv[1], strerror(errno));
                return 1;
        }

        elf::elf ef(elf::create_mmap_loader(fd));
        dwarf::dwarf dw(dwarf::elf::create_loader(ef));

        for (auto cu : dw.compilation_units()) {
                printf("--- <%" PRIx64 ">\n", cu.get_section_offset());
                dump_tree(cu.root());
        }

        return 0;
}
