/*
 * x.elfext.c -- handle ELF format extensions
 * Copyright (C) 2002 - 2006 Michael Riepe
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
    = "@(#) $Id: x.elfext.c,v 1.5 2009/07/07 17:57:43 michael Exp $";
#endif /* lint */

int elf_getphdrnum(Elf *elf, size_t *resultp) {
    if (!elf) {
        return -1;
    }
    elf_assert(elf->e_magic == ELF_MAGIC);
    if (elf->e_kind != ELF_K_ELF) {
        seterr(ERROR_NOTELF);
        return -1;
    }
    if (!elf->e_ehdr && !_elf_cook(elf)) {
        return -1;
    }
    if (resultp) {
        *resultp = elf->e_phnum;
    }
    return 0;
}

int elf_getshdrnum(Elf *elf, size_t *resultp) {
    size_t num = 0;
    Elf_Scn *scn;

    if (!elf) {
        return -1;
    }
    elf_assert(elf->e_magic == ELF_MAGIC);
    if (elf->e_kind != ELF_K_ELF) {
        seterr(ERROR_NOTELF);
        return -1;
    }
    if (!elf->e_ehdr && !_elf_cook(elf)) {
        return -1;
    }
    if ((scn = elf->e_scn_n)) {
        num = scn->s_index + 1;
    }
    if (resultp) {
        *resultp = num;
    }
    return 0;
}

int elf_getshdrstrndx(Elf *elf, size_t *resultp) {
    size_t num = 0;
    size_t dummy;
    Elf_Scn *scn;

    if (!elf) {
        return -1;
    }
    elf_assert(elf->e_magic == ELF_MAGIC);
    if (resultp == NULL) {
        resultp = &dummy; /* handle NULL pointer gracefully */
    }
    if (elf->e_kind != ELF_K_ELF) {
        seterr(ERROR_NOTELF);
        return -1;
    }
    if (!elf->e_ehdr && !_elf_cook(elf)) {
        return -1;
    }
    if (elf->e_class == ELFCLASS32) {
        num = ((Elf32_Ehdr *)elf->e_ehdr)->e_shstrndx;
    }
#if __LIBELF64
    else if (elf->e_class == ELFCLASS64) {
        num = ((Elf64_Ehdr *)elf->e_ehdr)->e_shstrndx;
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
    if (num != SHN_XINDEX) {
        *resultp = num;
        return 0;
    }
    /*
     * look at first section header
     */
    if (!(scn = elf->e_scn_1)) {
        seterr(ERROR_NOSUCHSCN);
        return -1;
    }
    elf_assert(scn->s_magic == SCN_MAGIC);
#if __LIBELF64
    if (elf->e_class == ELFCLASS64) {
        *resultp = scn->s_shdr64.sh_link;
        return 0;
    }
#endif /* __LIBELF64 */
    *resultp = scn->s_shdr32.sh_link;
    return 0;
}

int elf_getphnum(Elf *elf, size_t *resultp) {
    return elf_getphdrnum(elf, resultp) ? LIBELF_FAILURE : LIBELF_SUCCESS;
}

int elf_getshnum(Elf *elf, size_t *resultp) {
    return elf_getshdrnum(elf, resultp) ? LIBELF_FAILURE : LIBELF_SUCCESS;
}

int elf_getshstrndx(Elf *elf, size_t *resultp) {
    return elf_getshdrstrndx(elf, resultp) ? LIBELF_FAILURE : LIBELF_SUCCESS;
}

int elfx_update_shstrndx(Elf *elf, size_t value) {
    size_t extvalue = 0;
    Elf_Scn *scn;

    if (!elf) {
        return LIBELF_FAILURE;
    }
    elf_assert(elf->e_magic == ELF_MAGIC);
    if (value >= SHN_LORESERVE) {
        extvalue = value;
        value = SHN_XINDEX;
    }
    if (elf->e_kind != ELF_K_ELF) {
        seterr(ERROR_NOTELF);
        return LIBELF_FAILURE;
    }
    if (!elf->e_ehdr && !_elf_cook(elf)) {
        return LIBELF_FAILURE;
    }
    if (!(scn = _elf_first_scn(elf))) {
        return LIBELF_FAILURE;
    }
    elf_assert(scn->s_magic == SCN_MAGIC);
    if (elf->e_class == ELFCLASS32) {
        ((Elf32_Ehdr *)elf->e_ehdr)->e_shstrndx = value;
        scn->s_shdr32.sh_link = extvalue;
    }
#if __LIBELF64
    else if (elf->e_class == ELFCLASS64) {
        ((Elf64_Ehdr *)elf->e_ehdr)->e_shstrndx = value;
        scn->s_shdr64.sh_link = extvalue;
    }
#endif /* __LIBELF64 */
    else {
        if (valid_class(elf->e_class)) {
            seterr(ERROR_UNIMPLEMENTED);
        } else {
            seterr(ERROR_UNKNOWN_CLASS);
        }
        return LIBELF_FAILURE;
    }
    elf->e_ehdr_flags |= ELF_F_DIRTY;
    scn->s_shdr_flags |= ELF_F_DIRTY;
    return LIBELF_SUCCESS;
}
