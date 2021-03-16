/*
x.remscn.c - implementation of the elfx_remscn(3) function.
Copyright (C) 1995 - 2001, 2003 Michael Riepe

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
*/

#include "private.h"

#ifndef lint
static const char rcsid[]
    = "@(#) $Id: x.remscn.c,v 1.15 2008/05/23 08:15:35 michael Exp $";
#endif /* lint */

size_t elfx_remscn(Elf *elf, Elf_Scn *scn) {
    Elf_Scn *pscn;
    Scn_Data *sd;
    Scn_Data *tmp;
    size_t index;

    if (!elf || !scn) {
        return SHN_UNDEF;
    }
    elf_assert(elf->e_magic == ELF_MAGIC);
    if (elf->e_kind != ELF_K_ELF) {
        seterr(ERROR_NOTELF);
        return SHN_UNDEF;
    }
    elf_assert(scn->s_magic == SCN_MAGIC);
    elf_assert(elf->e_ehdr);
    if (scn->s_elf != elf) {
        seterr(ERROR_ELFSCNMISMATCH);
        return SHN_UNDEF;
    }
    elf_assert(elf->e_scn_1);
    if (scn == elf->e_scn_1) {
        seterr(ERROR_NULLSCN);
        return SHN_UNDEF;
    }

    /*
     * Find previous section.
     */
    for (pscn = elf->e_scn_1; pscn->s_link; pscn = pscn->s_link) {
        if (pscn->s_link == scn) {
            break;
        }
    }
    if (pscn->s_link != scn) {
        seterr(ERROR_ELFSCNMISMATCH);
        return SHN_UNDEF;
    }

    /*
     * Unlink section.
     */
    if (elf->e_scn_n == scn) {
        elf->e_scn_n = pscn;
    }
    pscn->s_link = scn->s_link;
    index = scn->s_index;

    /*
     * Free section descriptor and data.
     */
    for (sd = scn->s_data_1; sd; sd = tmp) {
        elf_assert(sd->sd_magic == DATA_MAGIC);
        elf_assert(sd->sd_scn == scn);
        tmp = sd->sd_link;
        if (sd->sd_free_data && sd->sd_memdata) {
            free(sd->sd_memdata);
        }
        if (sd->sd_freeme) {
            free(sd);
        }
    }
    if ((sd = scn->s_rawdata)) {
        elf_assert(sd->sd_magic == DATA_MAGIC);
        elf_assert(sd->sd_scn == scn);
        if (sd->sd_free_data && sd->sd_memdata) {
            free(sd->sd_memdata);
        }
        if (sd->sd_freeme) {
            free(sd);
        }
    }
    if (scn->s_freeme) {
        elf_assert(scn->s_index > 0);
        free(scn);
    }

    /*
     * Adjust section indices.
     */
    for (scn = pscn->s_link; scn; scn = scn->s_link) {
        elf_assert(scn->s_index > index);
        scn->s_index--;
    }

    /*
     * Adjust section count in ELF header
     */
    if (_elf_update_shnum(elf, elf->e_scn_n->s_index + 1)) {
        return SHN_UNDEF;
    }
    return index;
}
