/*
32.getphdr.c - implementation of the elf{32,64}_getphdr(3) functions.
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
    = "@(#) $Id: 32.getphdr.c,v 1.11 2008/05/23 08:15:34 michael Exp $";
#endif /* lint */

char *_elf_getphdr(Elf *elf, unsigned cls) {
    if (!elf) {
        return NULL;
    }
    elf_assert(elf->e_magic == ELF_MAGIC);
    if (elf->e_kind != ELF_K_ELF) {
        seterr(ERROR_NOTELF);
    } else if (elf->e_class != cls) {
        seterr(ERROR_CLASSMISMATCH);
    } else if (elf->e_ehdr || _elf_cook(elf)) {
        return elf->e_phdr;
    }
    return NULL;
}

Elf32_Phdr *elf32_getphdr(Elf *elf) {
    return (Elf32_Phdr *)_elf_getphdr(elf, ELFCLASS32);
}

#if __LIBELF64

Elf64_Phdr *elf64_getphdr(Elf *elf) {
    return (Elf64_Phdr *)_elf_getphdr(elf, ELFCLASS64);
}

#endif /* __LIBELF64 */
