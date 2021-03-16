/*
 * gelfshdr.c - gelf_* translation functions.
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
    = "@(#) $Id: gelfshdr.c,v 1.10 2008/05/23 08:15:34 michael Exp $";
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

GElf_Shdr *gelf_getshdr(Elf_Scn *scn, GElf_Shdr *dst) {
    GElf_Shdr buf;

    if (!scn) {
        return NULL;
    }
    elf_assert(scn->s_magic == SCN_MAGIC);
    elf_assert(scn->s_elf);
    elf_assert(scn->s_elf->e_magic == ELF_MAGIC);
    if (!dst) {
        dst = &buf;
    }
    if (scn->s_elf->e_class == ELFCLASS64) {
        *dst = scn->s_shdr64;
    } else if (scn->s_elf->e_class == ELFCLASS32) {
        Elf32_Shdr *src = &scn->s_shdr32;

        check_and_copy(GElf_Word, dst, src, sh_name, NULL);
        check_and_copy(GElf_Word, dst, src, sh_type, NULL);
        check_and_copy(GElf_Xword, dst, src, sh_flags, NULL);
        check_and_copy(GElf_Addr, dst, src, sh_addr, NULL);
        check_and_copy(GElf_Off, dst, src, sh_offset, NULL);
        check_and_copy(GElf_Xword, dst, src, sh_size, NULL);
        check_and_copy(GElf_Word, dst, src, sh_link, NULL);
        check_and_copy(GElf_Word, dst, src, sh_info, NULL);
        check_and_copy(GElf_Xword, dst, src, sh_addralign, NULL);
        check_and_copy(GElf_Xword, dst, src, sh_entsize, NULL);
    } else {
        if (valid_class(scn->s_elf->e_class)) {
            seterr(ERROR_UNIMPLEMENTED);
        } else {
            seterr(ERROR_UNKNOWN_CLASS);
        }
        return NULL;
    }
    if (dst == &buf) {
        dst = (GElf_Shdr *)malloc(sizeof(GElf_Shdr));
        if (!dst) {
            seterr(ERROR_MEM_SHDR);
            return NULL;
        }
        *dst = buf;
    }
    return dst;
}

int gelf_update_shdr(Elf_Scn *scn, GElf_Shdr *src) {
    if (!scn || !src) {
        return 0;
    }
    elf_assert(scn->s_magic == SCN_MAGIC);
    elf_assert(scn->s_elf);
    elf_assert(scn->s_elf->e_magic == ELF_MAGIC);
    if (scn->s_elf->e_class == ELFCLASS64) {
        scn->s_shdr64 = *src;
    } else if (scn->s_elf->e_class == ELFCLASS32) {
        Elf32_Shdr *dst = &scn->s_shdr32;

        check_and_copy(Elf32_Word, dst, src, sh_name, 0);
        check_and_copy(Elf32_Word, dst, src, sh_type, 0);
        check_and_copy(Elf32_Word, dst, src, sh_flags, 0);
        check_and_copy(Elf32_Addr, dst, src, sh_addr, 0);
        check_and_copy(Elf32_Off, dst, src, sh_offset, 0);
        check_and_copy(Elf32_Word, dst, src, sh_size, 0);
        check_and_copy(Elf32_Word, dst, src, sh_link, 0);
        check_and_copy(Elf32_Word, dst, src, sh_info, 0);
        check_and_copy(Elf32_Word, dst, src, sh_addralign, 0);
        check_and_copy(Elf32_Word, dst, src, sh_entsize, 0);
    } else {
        if (valid_class(scn->s_elf->e_class)) {
            seterr(ERROR_UNIMPLEMENTED);
        } else {
            seterr(ERROR_UNKNOWN_CLASS);
        }
        return 0;
    }
    return 1;
}

#endif /* __LIBELF64 */
