/*
 * getaroff.c - implementation of the elf_getaroff(3) function.
 * Copyright (C) 2009 Michael Riepe
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
    = "@(#) $Id: getaroff.c,v 1.1 2009/11/01 13:04:19 michael Exp $";
#endif /* lint */

off_t elf_getaroff(Elf *elf) {
    Elf *ref;

    if (!elf) {
        return (off_t)-1;
    }
    elf_assert(elf->e_magic == ELF_MAGIC);
    if (!(ref = elf->e_parent)) {
        return (off_t)-1;
    }
    elf_assert(ref->e_magic == ELF_MAGIC);
    elf_assert(elf->e_base >= ref->e_base + SARMAG + sizeof(struct ar_hdr));
    return (off_t)(elf->e_base - ref->e_base - sizeof(struct ar_hdr));
}
