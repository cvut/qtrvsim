/*
 * end.c - implementation of the elf_end(3) function.
 * Copyright (C) 1995 - 2004 Michael Riepe
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
    = "@(#) $Id: end.c,v 1.12 2008/05/23 08:15:34 michael Exp $";
#endif /* lint */

#if HAVE_MMAP
    #include <sys/mman.h>
#endif /* HAVE_MMAP */

static void _elf_free(void *ptr) {
    if (ptr) {
        free(ptr);
    }
}

static void _elf_free_scns(Elf *elf, Elf_Scn *scn) {
    Scn_Data *sd, *tmp;
    Elf_Scn *freescn;

    for (freescn = NULL; scn; scn = scn->s_link) {
        elf_assert(scn->s_magic == SCN_MAGIC);
        elf_assert(scn->s_elf == elf);
        for (sd = scn->s_data_1; sd; sd = tmp) {
            elf_assert(sd->sd_magic == DATA_MAGIC);
            elf_assert(sd->sd_scn == scn);
            tmp = sd->sd_link;
            if (sd->sd_free_data) {
                _elf_free(sd->sd_memdata);
            }
            if (sd->sd_freeme) {
                free(sd);
            }
        }
        if ((sd = scn->s_rawdata)) {
            elf_assert(sd->sd_magic == DATA_MAGIC);
            elf_assert(sd->sd_scn == scn);
            if (sd->sd_free_data) {
                _elf_free(sd->sd_memdata);
            }
            if (sd->sd_freeme) {
                free(sd);
            }
        }
        if (scn->s_freeme) {
            _elf_free(freescn);
            freescn = scn;
        }
    }
    _elf_free(freescn);
}

int elf_end(Elf *elf) {
    Elf **siblings;

    if (!elf) {
        return 0;
    }
    elf_assert(elf->e_magic == ELF_MAGIC);
    if (--elf->e_count) {
        return elf->e_count;
    }
    if (elf->e_parent) {
        elf_assert(elf->e_parent->e_magic == ELF_MAGIC);
        elf_assert(elf->e_parent->e_kind == ELF_K_AR);
        siblings = &elf->e_parent->e_members;
        while (*siblings) {
            if (*siblings == elf) {
                *siblings = elf->e_link;
                break;
            }
            siblings = &(*siblings)->e_link;
        }
        elf_end(elf->e_parent);
        _elf_free(elf->e_arhdr);
    }
#if HAVE_MMAP
    else if (elf->e_unmap_data) {
        munmap(elf->e_data, elf->e_size);
    }
#endif /* HAVE_MMAP */
    else if (!elf->e_memory) {
        _elf_free(elf->e_data);
    }
    _elf_free_scns(elf, elf->e_scn_1);
    if (elf->e_rawdata != elf->e_data) {
        _elf_free(elf->e_rawdata);
    }
    if (elf->e_free_syms) {
        _elf_free(elf->e_symtab);
    }
    _elf_free(elf->e_ehdr);
    _elf_free(elf->e_phdr);
    free(elf);
    return 0;
}
