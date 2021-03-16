/*
checksum.c - implementation of the elf{32,64}_checksum(3) functions.
Copyright (C) 1995 - 2001 Michael Riepe

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
    = "@(#) $Id: checksum.c,v 1.7 2008/05/23 08:15:34 michael Exp $";
#endif /* lint */

/*
 * Compatibility note:
 *
 * The algorithm used in {elf32,elf64,gelf}_checksum() does not seem to
 * be documented.  I hope I got it right.  My implementation does the
 * following:
 *
 *   - skip sections that do not have the SHF_ALLOC flag set
 *   - skip sections of type SHT_NULL, SHT_NOBITS, SHT_DYNSYM and
 *     SHT_DYNAMIC
 *   - add all data bytes from the remaining sections, modulo 2**32
 *   - add upper and lower half of the result
 *   - subtract 0xffff if the result is > 0xffff
 *   - if any error occurs, return 0L
 */

static int skip_section(Elf_Scn *scn, unsigned cls) {
    if (cls == ELFCLASS32) {
        Elf32_Shdr *shdr = &scn->s_shdr32;

        if (!(shdr->sh_flags & SHF_ALLOC)) {
            return 1;
        }
        switch (shdr->sh_type) {
        case SHT_NULL:
        case SHT_NOBITS:
        /* Solaris seems to ignore these, too */
        case SHT_DYNSYM:
        case SHT_DYNAMIC: return 1;
        }
    }
#if __LIBELF64
    else if (cls == ELFCLASS64) {
        Elf64_Shdr *shdr = &scn->s_shdr64;

        if (!(shdr->sh_flags & SHF_ALLOC)) {
            return 1;
        }
        switch (shdr->sh_type) {
        case SHT_NULL:
        case SHT_NOBITS:
        /* Solaris seems to ignore these, too */
        case SHT_DYNSYM:
        case SHT_DYNAMIC: return 1;
        }
    }
#endif /* __LIBELF64 */
    else {
        seterr(ERROR_UNIMPLEMENTED);
    }
    return 0;
}

static long add_bytes(unsigned char *ptr, size_t len) {
    long csum = 0;

    while (len--) {
        csum += *ptr++;
    }
    return csum;
}

static long _elf_csum(Elf *elf) {
    long csum = 0;
    Elf_Data *data;
    Elf_Scn *scn;

    if (!elf->e_ehdr && !_elf_cook(elf)) {
        /* propagate errors from _elf_cook */
        return 0L;
    }
    seterr(0);
    for (scn = elf->e_scn_1; scn; scn = scn->s_link) {
        if (scn->s_index == SHN_UNDEF || skip_section(scn, elf->e_class)) {
            continue;
        }
        data = NULL;
        while ((data = elf_getdata(scn, data))) {
            if (data->d_size) {
                if (data->d_buf == NULL) {
                    seterr(ERROR_NULLBUF);
                    return 0L;
                }
                csum += add_bytes(data->d_buf, data->d_size);
            }
        }
    }
    if (_elf_errno) {
        return 0L;
    }
    csum = (csum & 0xffff) + ((csum >> 16) & 0xffff);
    if (csum > 0xffff) {
        csum -= 0xffff;
    }
    return csum;
}

long elf32_checksum(Elf *elf) {
    if (elf) {
        if (elf->e_kind != ELF_K_ELF) {
            seterr(ERROR_NOTELF);
        } else if (elf->e_class != ELFCLASS32) {
            seterr(ERROR_CLASSMISMATCH);
        } else {
            return _elf_csum(elf);
        }
    }
    return 0L;
}

#if __LIBELF64

long elf64_checksum(Elf *elf) {
    if (elf) {
        if (elf->e_kind != ELF_K_ELF) {
            seterr(ERROR_NOTELF);
        } else if (elf->e_class != ELFCLASS64) {
            seterr(ERROR_CLASSMISMATCH);
        } else {
            return _elf_csum(elf);
        }
    }
    return 0L;
}

long gelf_checksum(Elf *elf) {
    if (elf) {
        if (elf->e_kind != ELF_K_ELF) {
            seterr(ERROR_NOTELF);
        } else if (!valid_class(elf->e_class)) {
            seterr(ERROR_UNKNOWN_CLASS);
        } else {
            return _elf_csum(elf);
        }
    }
    return 0L;
}

#endif /* __LIBELF64 */
