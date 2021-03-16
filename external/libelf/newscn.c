/*
 * newscn.c - implementation of the elf_newscn(3) function.
 * Copyright (C) 1995 - 2006 Michael Riepe
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

#ifndef lint
static const char rcsid[]
    = "@(#) $Id: newscn.c,v 1.13 2008/05/23 08:15:35 michael Exp $";
#endif /* lint */

int _elf_update_shnum(Elf *elf, size_t shnum) {
    size_t extshnum = 0;
    Elf_Scn *scn;

    elf_assert(elf);
    elf_assert(elf->e_ehdr);
    scn = elf->e_scn_1;
    elf_assert(scn);
    elf_assert(scn->s_index == 0);
    if (shnum >= SHN_LORESERVE) {
        extshnum = shnum;
        shnum = 0;
    }
    if (elf->e_class == ELFCLASS32) {
        ((Elf32_Ehdr *)elf->e_ehdr)->e_shnum = shnum;
        scn->s_shdr32.sh_size = extshnum;
    }
#if __LIBELF64
    else if (elf->e_class == ELFCLASS64) {
        ((Elf64_Ehdr *)elf->e_ehdr)->e_shnum = shnum;
        scn->s_shdr64.sh_size = extshnum;
    }
#endif /* __LIBELF64 */
    else {
        if (valid_class(elf->e_class)) {
            seterr(ERROR_UNIMPLEMENTED);
        } else {
            seterr(ERROR_UNKNOWN_CLASS);
        }
        return -1;
    }
    elf->e_ehdr_flags |= ELF_F_DIRTY;
    scn->s_shdr_flags |= ELF_F_DIRTY;
    return 0;
}

static Elf_Scn *_makescn(Elf *elf, size_t index) {
    Elf_Scn *scn;

    elf_assert(elf);
    elf_assert(elf->e_magic == ELF_MAGIC);
    elf_assert(elf->e_ehdr);
    elf_assert(_elf_scn_init.s_magic == SCN_MAGIC);
    if (!(scn = (Elf_Scn *)malloc(sizeof(*scn)))) {
        seterr(ERROR_MEM_SCN);
        return NULL;
    }
    *scn = _elf_scn_init;
    scn->s_elf = elf;
    scn->s_scn_flags = ELF_F_DIRTY;
    scn->s_shdr_flags = ELF_F_DIRTY;
    scn->s_freeme = 1;
    scn->s_index = index;
    return scn;
}

Elf_Scn *_elf_first_scn(Elf *elf) {
    Elf_Scn *scn;

    elf_assert(elf);
    elf_assert(elf->e_magic == ELF_MAGIC);
    if ((scn = elf->e_scn_1)) {
        return scn;
    }
    if ((scn = _makescn(elf, 0))) {
        elf->e_scn_1 = elf->e_scn_n = scn;
        if (_elf_update_shnum(elf, 1)) {
            free(scn);
            elf->e_scn_1 = elf->e_scn_n = scn = NULL;
        }
    }
    return scn;
}

static Elf_Scn *_buildscn(Elf *elf) {
    Elf_Scn *scn;

    if (!_elf_first_scn(elf)) {
        return NULL;
    }
    scn = elf->e_scn_n;
    elf_assert(scn);
    if (!(scn = _makescn(elf, scn->s_index + 1))) {
        return NULL;
    }
    if (_elf_update_shnum(elf, scn->s_index + 1)) {
        free(scn);
        return NULL;
    }
    elf->e_scn_n = elf->e_scn_n->s_link = scn;
    return scn;
}

Elf_Scn *elf_newscn(Elf *elf) {
    Elf_Scn *scn;

    if (!elf) {
        return NULL;
    }
    elf_assert(elf->e_magic == ELF_MAGIC);
    if (!elf->e_readable && !elf->e_ehdr) {
        seterr(ERROR_NOEHDR);
    } else if (elf->e_kind != ELF_K_ELF) {
        seterr(ERROR_NOTELF);
    } else if (!elf->e_ehdr && !_elf_cook(elf)) {
        return NULL;
    } else if ((scn = _buildscn(elf))) {
        return scn;
    }
    return NULL;
}
