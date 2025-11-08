#include "elf++.hh"
#include "dwarf++.hh"

#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>

using namespace std;

void
dump_line_table(const dwarf::line_table &lt)
{
        for (auto &line : lt) {
                if (line.end_sequence)
                        printf("\n");
                else
                        printf("%-40s%8d%#20" PRIx64 "\n", line.file->path.c_str(),
                               line.line, line.address);
        }
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
                printf("--- <%x>\n", (unsigned int)cu.get_section_offset());
                dump_line_table(cu.get_line_table());
                printf("\n");
        }

        return 0;
}
