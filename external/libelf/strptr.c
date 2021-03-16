/*
 * strptr.c - implementation of the elf_strptr(3) function.
 * Copyright (C) 1995 - 2007 Michael Riepe
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
    = "@(#) $Id: strptr.c,v 1.12 2008/05/23 08:15:35 michael Exp $";
#endif /* lint */

char *elf_strptr(Elf *elf, size_t section, size_t offset) {
    Elf_Data *data;
    Elf_Scn *scn;
    size_t n;
    char *s;

    if (!elf) {
        return NULL;
    }
    elf_assert(elf->e_magic == ELF_MAGIC);
    if (!(scn = elf_getscn(elf, section))) {
        return NULL;
    }
    if (scn->s_index == SHN_UNDEF) {
        seterr(ERROR_NOSTRTAB);
        return NULL;
    }
    /*
     * checking the section header is more appropriate
     */
    if (elf->e_class == ELFCLASS32) {
        if (scn->s_shdr32.sh_type != SHT_STRTAB) {
            seterr(ERROR_NOSTRTAB);
            return NULL;
        }
    }
#if __LIBELF64
    else if (elf->e_class == ELFCLASS64) {
        if (scn->s_shdr64.sh_type != SHT_STRTAB) {
            seterr(ERROR_NOSTRTAB);
            return NULL;
        }
    }
#endif /* __LIBELF64 */
    else if (valid_class(elf->e_class)) {
        seterr(ERROR_UNIMPLEMENTED);
        return NULL;
    } else {
        seterr(ERROR_UNKNOWN_CLASS);
        return NULL;
    }
    /*
     * Find matching buffer
     */
    n = 0;
    data = NULL;
    if (elf->e_elf_flags & ELF_F_LAYOUT) {
        /*
         * Programmer is responsible for d_off
         * Note: buffers may be in any order!
         */
        while ((data = elf_getdata(scn, data))) {
            n = data->d_off;
            if (offset >= n && offset - n < data->d_size) {
                /*
                 * Found it
                 */
                break;
            }
        }
    } else {
        /*
         * Calculate offsets myself
         */
        while ((data = elf_getdata(scn, data))) {
            if (data->d_align > 1) {
                n += data->d_align - 1;
                n -= n % data->d_align;
            }
            if (offset < n) {
                /*
                 * Invalid offset: points into a hole
                 */
                seterr(ERROR_BADSTROFF);
                return NULL;
            }
            if (offset - n < data->d_size) {
                /*
                 * Found it
                 */
                break;
            }
            n += data->d_size;
        }
    }
    if (data == NULL) {
        /*
         * Not found
         */
        seterr(ERROR_BADSTROFF);
        return NULL;
    }
    if (data->d_buf == NULL) {
        /*
         * Buffer is NULL (usually the programmers' fault)
         */
        seterr(ERROR_NULLBUF);
        return NULL;
    }
    offset -= n;
    s = (char *)data->d_buf;
    if (!(_elf_sanity_checks & SANITY_CHECK_STRPTR)) {
        return s + offset;
    }
    /*
     * Perform extra sanity check
     */
    for (n = offset; n < data->d_size; n++) {
        if (s[n] == '\0') {
            /*
             * Return properly NUL terminated string
             */
            return s + offset;
        }
    }
    /*
     * String is not NUL terminated
     * Return error to avoid SEGV in application
     */
    seterr(ERROR_UNTERM);
    return NULL;
}
