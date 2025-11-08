#include "elf++.hh"

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>

int main(int argc, char **argv)
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

        elf::elf f(elf::create_mmap_loader(fd));
        for (auto &sec : f.sections()) {
                if (sec.get_hdr().type != elf::sht::symtab && sec.get_hdr().type != elf::sht::dynsym)
                        continue;

                printf("Symbol table '%s':\n", sec.get_name().c_str());
                printf("%6s: %-16s %-5s %-7s %-7s %-5s %s\n",
                       "Num", "Value", "Size", "Type", "Binding", "Index",
                       "Name");
                int i = 0;
                for (auto sym : sec.as_symtab()) {
                        auto &d = sym.get_data();
                        printf("%6d: %016" PRIx64 " %5" PRId64 " %-7s %-7s %5s %s\n",
                               i++, d.value, d.size,
                               to_string(d.type()).c_str(),
                               to_string(d.binding()).c_str(),
                               to_string(d.shnxd).c_str(),
                               sym.get_name().c_str());
                }
        }

        return 0;
}
