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
        printf("  %-16s  %-16s   %-16s   %s\n",
                "Type", "Offset", "VirtAddr", "PhysAddr");
        printf("  %-16s  %-16s   %-16s  %6s %5s\n",
                " ", "FileSiz", "MemSiz", "Flags", "Align");
        for (auto &seg : f.segments()) {
                auto &hdr = seg.get_hdr();
                printf("   %-16s 0x%016" PRIx64 " 0x%016" PRIx64 " 0x%016" PRIx64 "\n",
                        to_string(hdr.type).c_str(), hdr.offset,
                        hdr.vaddr, hdr.paddr);
                printf("   %-16s 0x%016" PRIx64 " 0x%016" PRIx64 " %-5s %-5" PRIx64 "\n", "",
                        hdr.filesz, hdr.memsz,
                        to_string(hdr.flags).c_str(), hdr.align);
        }

        return 0;
}

