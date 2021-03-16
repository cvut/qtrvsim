/*
 * gelfehdr.c - gelf_* translation functions.
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
    = "@(#) $Id: gelfehdr.c,v 1.9 2008/05/23 08:15:34 michael Exp $";
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

GElf_Ehdr *gelf_getehdr(Elf *elf, GElf_Ehdr *dst) {
    GElf_Ehdr buf;
    char *tmp;

    if (!elf) {
        return NULL;
    }
    elf_assert(elf->e_magic == ELF_MAGIC);
    tmp = _elf_getehdr(elf, elf->e_class);
    if (!tmp) {
        return NULL;
    }
    if (!dst) {
        dst = &buf;
    }
    if (elf->e_class == ELFCLASS64) {
        *dst = *(Elf64_Ehdr *)tmp;
    } else if (elf->e_class == ELFCLASS32) {
        Elf32_Ehdr *src = (Elf32_Ehdr *)tmp;

        memcpy(dst->e_ident, src->e_ident, EI_NIDENT);
        check_and_copy(GElf_Half, dst, src, e_type, NULL);
        check_and_copy(GElf_Half, dst, src, e_machine, NULL);
        check_and_copy(GElf_Word, dst, src, e_version, NULL);
        check_and_copy(GElf_Addr, dst, src, e_entry, NULL);
        check_and_copy(GElf_Off, dst, src, e_phoff, NULL);
        check_and_copy(GElf_Off, dst, src, e_shoff, NULL);
        check_and_copy(GElf_Word, dst, src, e_flags, NULL);
        check_and_copy(GElf_Half, dst, src, e_ehsize, NULL);
        check_and_copy(GElf_Half, dst, src, e_phentsize, NULL);
        check_and_copy(GElf_Half, dst, src, e_phnum, NULL);
        check_and_copy(GElf_Half, dst, src, e_shentsize, NULL);
        check_and_copy(GElf_Half, dst, src, e_shnum, NULL);
        check_and_copy(GElf_Half, dst, src, e_shstrndx, NULL);
    } else {
        if (valid_class(elf->e_class)) {
            seterr(ERROR_UNIMPLEMENTED);
        } else {
            seterr(ERROR_UNKNOWN_CLASS);
        }
        return NULL;
    }
    if (dst == &buf) {
        dst = (GElf_Ehdr *)malloc(sizeof(GElf_Ehdr));
        if (!dst) {
            seterr(ERROR_MEM_EHDR);
            return NULL;
        }
        *dst = buf;
    }
    return dst;
}

int gelf_update_ehdr(Elf *elf, GElf_Ehdr *src) {
    char *tmp;

    if (!elf || !src) {
        return 0;
    }
    elf_assert(elf->e_magic == ELF_MAGIC);
    tmp = _elf_getehdr(elf, elf->e_class);
    if (!tmp) {
        return 0;
    }
    if (elf->e_class == ELFCLASS64) {
        *(Elf64_Ehdr *)tmp = *src;
    } else if (elf->e_class == ELFCLASS32) {
        Elf32_Ehdr *dst = (Elf32_Ehdr *)tmp;

        memcpy(dst->e_ident, src->e_ident, EI_NIDENT);
        check_and_copy(Elf32_Half, dst, src, e_type, 0);
        check_and_copy(Elf32_Half, dst, src, e_machine, 0);
        check_and_copy(Elf32_Word, dst, src, e_version, 0);
        check_and_copy(Elf32_Addr, dst, src, e_entry, 0);
        check_and_copy(Elf32_Off, dst, src, e_phoff, 0);
        check_and_copy(Elf32_Off, dst, src, e_shoff, 0);
        check_and_copy(Elf32_Word, dst, src, e_flags, 0);
        check_and_copy(Elf32_Half, dst, src, e_ehsize, 0);
        check_and_copy(Elf32_Half, dst, src, e_phentsize, 0);
        check_and_copy(Elf32_Half, dst, src, e_phnum, 0);
        check_and_copy(Elf32_Half, dst, src, e_shentsize, 0);
        check_and_copy(Elf32_Half, dst, src, e_shnum, 0);
        check_and_copy(Elf32_Half, dst, src, e_shstrndx, 0);
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
