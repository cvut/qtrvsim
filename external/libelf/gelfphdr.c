/*
 * gelfphdr.c - gelf_* translation functions.
 * Copyright (C) 2000 - 2006 Michael Riepe
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 */

#include "private.h"

#if __LIBELF64

    #ifndef lint
static const char rcsid[]
    = "@(#) $Id: gelfphdr.c,v 1.9 2008/05/23 08:15:34 michael Exp $";
    #endif /* lint */

    #define check_and_copy(type, d, s, name, eret)                             \
        do {                                                                   \
            if (sizeof((d)->name) < sizeof((s)->name)                          \
                && (type)(s)->name != (s)->name) {                             \
                seterr(ERROR_BADVALUE);                                        \
                return (eret);                                                 \
            }                                                                  \
            (d)->name = (type)(s)->name;                                       \
        } while (0)

GElf_Phdr *gelf_getphdr(Elf *elf, int ndx, GElf_Phdr *dst) {
    GElf_Phdr buf;
    char *tmp;
    size_t n;

    if (!elf) {
        return NULL;
    }
    elf_assert(elf->e_magic == ELF_MAGIC);
    tmp = _elf_getphdr(elf, elf->e_class);
    if (!tmp) {
        return NULL;
    }
    if (ndx < 0 || ndx >= elf->e_phnum) {
        seterr(ERROR_BADINDEX);
        return NULL;
    }
    n = _msize(elf->e_class, _elf_version, ELF_T_PHDR);
    if (n == 0) {
        seterr(ERROR_UNIMPLEMENTED);
        return NULL;
    }
    if (!dst) {
        dst = &buf;
    }
    if (elf->e_class == ELFCLASS64) {
        *dst = *(Elf64_Phdr *)(tmp + ndx * n);
    } else if (elf->e_class == ELFCLASS32) {
        Elf32_Phdr *src = (Elf32_Phdr *)(tmp + ndx * n);

        check_and_copy(GElf_Word, dst, src, p_type, NULL);
        check_and_copy(GElf_Word, dst, src, p_flags, NULL);
        check_and_copy(GElf_Off, dst, src, p_offset, NULL);
        check_and_copy(GElf_Addr, dst, src, p_vaddr, NULL);
        check_and_copy(GElf_Addr, dst, src, p_paddr, NULL);
        check_and_copy(GElf_Xword, dst, src, p_filesz, NULL);
        check_and_copy(GElf_Xword, dst, src, p_memsz, NULL);
        check_and_copy(GElf_Xword, dst, src, p_align, NULL);
    } else {
        if (valid_class(elf->e_class)) {
            seterr(ERROR_UNIMPLEMENTED);
        } else {
            seterr(ERROR_UNKNOWN_CLASS);
        }
        return NULL;
    }
    if (dst == &buf) {
        dst = (GElf_Phdr *)malloc(sizeof(GElf_Phdr));
        if (!dst) {
            seterr(ERROR_MEM_PHDR);
            return NULL;
        }
        *dst = buf;
    }
    return dst;
}

int gelf_update_phdr(Elf *elf, int ndx, GElf_Phdr *src) {
    char *tmp;
    size_t n;

    if (!elf || !src) {
        return 0;
    }
    elf_assert(elf->e_magic == ELF_MAGIC);
    tmp = _elf_getphdr(elf, elf->e_class);
    if (!tmp) {
        return 0;
    }
    if (ndx < 0 || ndx >= elf->e_phnum) {
        seterr(ERROR_BADINDEX);
        return 0;
    }
    n = _msize(elf->e_class, _elf_version, ELF_T_PHDR);
    if (n == 0) {
        seterr(ERROR_UNIMPLEMENTED);
        return 0;
    }
    if (elf->e_class == ELFCLASS64) {
        *(Elf64_Phdr *)(tmp + ndx * n) = *src;
    } else if (elf->e_class == ELFCLASS32) {
        Elf32_Phdr *dst = (Elf32_Phdr *)(tmp + ndx * n);

        check_and_copy(Elf32_Word, dst, src, p_type, 0);
        check_and_copy(Elf32_Off, dst, src, p_offset, 0);
        check_and_copy(Elf32_Addr, dst, src, p_vaddr, 0);
        check_and_copy(Elf32_Addr, dst, src, p_paddr, 0);
        check_and_copy(Elf32_Word, dst, src, p_filesz, 0);
        check_and_copy(Elf32_Word, dst, src, p_memsz, 0);
        check_and_copy(Elf32_Word, dst, src, p_flags, 0);
        check_and_copy(Elf32_Word, dst, src, p_align, 0);
    } else {
        if (valid_class(elf->e_class)) {
            seterr(ERROR_UNIMPLEMENTED);
        } else {
            seterr(ERROR_UNKNOWN_CLASS);
        }
        return 0;
    }
    return 1;
}

#endif /* __LIBELF64 */
