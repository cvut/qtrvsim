/*
32.getshdr.c - implementation of the elf{32,64}_getshdr(3) functions.
Copyright (C) 1995 - 1998 Michael Riepe

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
    = "@(#) $Id: 32.getshdr.c,v 1.10 2008/05/23 08:15:34 michael Exp $";
#endif /* lint */

Elf32_Shdr *elf32_getshdr(Elf_Scn *scn) {
    if (!scn) {
        return NULL;
    }
    elf_assert(scn->s_magic == SCN_MAGIC);
    elf_assert(scn->s_elf);
    elf_assert(scn->s_elf->e_magic == ELF_MAGIC);
    if (scn->s_elf->e_class == ELFCLASS32) {
        return &scn->s_shdr32;
    }
    seterr(ERROR_CLASSMISMATCH);
    return NULL;
}

#if __LIBELF64

Elf64_Shdr *elf64_getshdr(Elf_Scn *scn) {
    if (!scn) {
        return NULL;
    }
    elf_assert(scn->s_magic == SCN_MAGIC);
    elf_assert(scn->s_elf);
    elf_assert(scn->s_elf->e_magic == ELF_MAGIC);
    if (scn->s_elf->e_class == ELFCLASS64) {
        return &scn->s_shdr64;
    }
    seterr(ERROR_CLASSMISMATCH);
    return NULL;
}

#endif /* __LIBELF64 */
