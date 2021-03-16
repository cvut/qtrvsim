/*
rawdata.c - implementation of the elf_rawdata(3) function.
Copyright (C) 1995 - 2000 Michael Riepe

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
    = "@(#) $Id: rawdata.c,v 1.10 2008/05/23 08:15:35 michael Exp $";
#endif /* lint */

Elf_Data *elf_rawdata(Elf_Scn *scn, Elf_Data *data) {
    Scn_Data *sd;
    Elf *elf;

    if (!scn) {
        return NULL;
    }
    elf_assert(scn->s_magic == SCN_MAGIC);
    elf = scn->s_elf;
    elf_assert(elf);
    elf_assert(elf->e_magic == ELF_MAGIC);
    if (!elf->e_readable) {
        return NULL;
    } else if (scn->s_index == SHN_UNDEF || scn->s_type == SHT_NULL) {
        seterr(ERROR_NULLSCN);
    } else if (data) {
        return NULL;
    } else if ((sd = scn->s_rawdata)) {
        elf_assert(sd->sd_magic == DATA_MAGIC);
        elf_assert(sd->sd_scn == scn);
        return &sd->sd_data;
    } else if (scn->s_offset < 0 || scn->s_offset > elf->e_size) {
        seterr(ERROR_OUTSIDE);
    } else if (
        scn->s_type != SHT_NOBITS
        && scn->s_offset + scn->s_size > elf->e_size) {
        seterr(ERROR_TRUNC_SCN);
    } else if (!(sd = (Scn_Data *)malloc(sizeof(*sd)))) {
        seterr(ERROR_MEM_SCNDATA);
    } else {
        *sd = _elf_data_init;
        sd->sd_scn = scn;
        sd->sd_freeme = 1;
        sd->sd_data.d_size = scn->s_size;
        sd->sd_data.d_version = _elf_version;
        if (scn->s_type != SHT_NOBITS && scn->s_size) {
            if (!(sd->sd_memdata = (char *)malloc(scn->s_size))) {
                seterr(ERROR_IO_2BIG);
                free(sd);
                return NULL;
            } else if (elf->e_rawdata) {
                memcpy(
                    sd->sd_memdata, elf->e_rawdata + scn->s_offset,
                    scn->s_size);
            } else if (!_elf_read(
                           elf, sd->sd_memdata, scn->s_offset, scn->s_size)) {
                free(sd->sd_memdata);
                free(sd);
                return NULL;
            }
            sd->sd_data.d_buf = sd->sd_memdata;
            sd->sd_free_data = 1;
        }
        scn->s_rawdata = sd;
        return &sd->sd_data;
    }
    return NULL;
}
