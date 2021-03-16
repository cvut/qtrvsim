/*
 * getarsym.c - implementation of the elf_getarsym(3) function.
 * Copyright (C) 1995 - 1998, 2004 Michael Riepe
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

#include "byteswap.h"
#include "private.h"

#ifndef lint
static const char rcsid[]
    = "@(#) $Id: getarsym.c,v 1.9 2008/05/23 08:15:34 michael Exp $";
#endif /* lint */

Elf_Arsym *elf_getarsym(Elf *elf, size_t *ptr) {
    Elf_Arsym *syms;
    size_t count;
    size_t tmp;
    size_t i;
    char *s;
    char *e;

    if (!ptr) {
        ptr = &tmp;
    }
    *ptr = 0;
    if (!elf) {
        return NULL;
    }
    elf_assert(elf->e_magic == ELF_MAGIC);
    if (elf->e_kind != ELF_K_AR) {
        seterr(ERROR_NOTARCHIVE);
        return NULL;
    }
    if (elf->e_symtab && !elf->e_free_syms) {
        if (elf->e_symlen < 4) {
            seterr(ERROR_SIZE_ARSYMTAB);
            return NULL;
        }
        count = __load_u32M(elf->e_symtab);
        if (elf->e_symlen < 4 * (count + 1)) {
            seterr(ERROR_SIZE_ARSYMTAB);
            return NULL;
        }
        if (!(syms = (Elf_Arsym *)malloc((count + 1) * sizeof(*syms)))) {
            seterr(ERROR_MEM_ARSYMTAB);
            return NULL;
        }
        s = elf->e_symtab + 4 * (count + 1);
        e = elf->e_symtab + elf->e_symlen;
        for (i = 0; i < count; i++, s++) {
            syms[i].as_name = s;
            while (s < e && *s) {
                s++;
            }
            if (s >= e) {
                seterr(ERROR_SIZE_ARSYMTAB);
                free(syms);
                return NULL;
            }
            elf_assert(!*s);
            syms[i].as_hash = elf_hash((unsigned char *)syms[i].as_name);
            syms[i].as_off = __load_u32M(elf->e_symtab + 4 * (i + 1));
        }
        syms[count].as_name = NULL;
        syms[count].as_hash = ~0UL;
        syms[count].as_off = 0;
        elf->e_symtab = (char *)syms;
        elf->e_symlen = count + 1;
        elf->e_free_syms = 1;
    }
    *ptr = elf->e_symlen;
    return (Elf_Arsym *)elf->e_symtab;
}
