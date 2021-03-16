/*
next.c - implementation of the elf_next(3) function.
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
    = "@(#) $Id: next.c,v 1.7 2008/05/23 08:15:35 michael Exp $";
#endif /* lint */

Elf_Cmd elf_next(Elf *elf) {
    if (!elf) {
        return ELF_C_NULL;
    }
    elf_assert(elf->e_magic == ELF_MAGIC);
    if (!elf->e_parent) {
        return ELF_C_NULL;
    }
    elf_assert(elf->e_parent->e_magic == ELF_MAGIC);
    elf_assert(elf->e_parent->e_kind == ELF_K_AR);
    elf->e_parent->e_off = elf->e_next;
    if (elf->e_next == elf->e_parent->e_size) {
        return ELF_C_NULL;
    }
    return ELF_C_READ;
}
