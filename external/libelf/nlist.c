/*
 * nlist.c - implementation of the nlist(3) function.
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

#include "nlist.h"

#include "private.h"

#ifndef lint
static const char rcsid[]
    = "@(#) $Id: nlist.c,v 1.15 2008/05/23 08:15:35 michael Exp $";
#endif /* lint */

#if !defined(_WIN32)
    #if HAVE_FCNTL_H
        #include <fcntl.h>
    #else
extern int open();
    #endif /* HAVE_FCNTL_H */
#endif /* defined(_WIN32) */

#ifndef O_RDONLY
    #define O_RDONLY 0
#endif /* O_RDONLY */

#ifndef O_BINARY
    #define O_BINARY 0
#endif /* O_BINARY */

#define FILE_OPEN_MODE (O_RDONLY | O_BINARY)

#define PRIME 217

struct hash {
    const char *name;
    unsigned long hash;
    unsigned next;
};

static const char *symbol_name(
    Elf *elf,
    const void *syms,
    const char *names,
    size_t nlimit,
    size_t index) {
    size_t off;

    if (elf->e_class == ELFCLASS32) {
        off = ((Elf32_Sym *)syms)[index].st_name;
    }
#if __LIBELF64
    else if (elf->e_class == ELFCLASS64) {
        off = ((Elf64_Sym *)syms)[index].st_name;
    }
#endif /* __LIBELF64 */
    else {
        return NULL;
    }
    if (off >= 0 && off < nlimit) {
        return &names[off];
    }
    return NULL;
}

static void
copy_symbol(Elf *elf, struct nlist *np, const void *syms, size_t index) {
    if (elf->e_class == ELFCLASS32) {
        np->n_value = ((Elf32_Sym *)syms)[index].st_value;
        np->n_scnum = ((Elf32_Sym *)syms)[index].st_shndx;
    }
#if __LIBELF64
    else if (elf->e_class == ELFCLASS64) {
        np->n_value = ((Elf64_Sym *)syms)[index].st_value;
        np->n_scnum = ((Elf64_Sym *)syms)[index].st_shndx;
    }
#endif /* __LIBELF64 */
    /*
     * this needs more work
     */
    np->n_type = 0;
    np->n_sclass = 0;
    np->n_numaux = 0;
}

static int _elf_nlist(Elf *elf, struct nlist *nl) {
    unsigned first[PRIME];
    Elf_Scn *symtab = NULL;
    Elf_Scn *strtab = NULL;
    Elf_Data *symdata;
    Elf_Data *strdata;
    size_t symsize;
    size_t nsymbols;
    const char *name;
    struct hash *table;
    unsigned long hash;
    unsigned i;
    struct nlist *np;

    /*
     * Get and translate ELF header, section table and so on.
     * Must be class independent, so don't use elf32_get*().
     */
    if (elf->e_kind != ELF_K_ELF) {
        return -1;
    }
    if (!elf->e_ehdr && !_elf_cook(elf)) {
        return -1;
    }

    /*
     * Find symbol table. If there is none, try dynamic symbols.
     */
    for (symtab = elf->e_scn_1; symtab; symtab = symtab->s_link) {
        if (symtab->s_type == SHT_SYMTAB) {
            break;
        }
        if (symtab->s_type == SHT_DYNSYM) {
            strtab = symtab;
        }
    }
    if (!symtab && !(symtab = strtab)) {
        return -1;
    }

    /*
     * Get associated string table.
     */
    i = 0;
    if (elf->e_class == ELFCLASS32) {
        i = symtab->s_shdr32.sh_link;
    }
#if __LIBELF64
    else if (elf->e_class == ELFCLASS64) {
        i = symtab->s_shdr64.sh_link;
    }
#endif /* __LIBELF64 */
    if (i == 0) {
        return -1;
    }
    for (strtab = elf->e_scn_1; strtab; strtab = strtab->s_link) {
        if (strtab->s_index == i) {
            break;
        }
    }
    if (!strtab || strtab->s_type != SHT_STRTAB) {
        return -1;
    }

    /*
     * Get and translate section data.
     */
    symdata = elf_getdata(symtab, NULL);
    strdata = elf_getdata(strtab, NULL);
    if (!symdata || !strdata) {
        return -1;
    }
    symsize = _msize(elf->e_class, _elf_version, ELF_T_SYM);
    elf_assert(symsize);
    nsymbols = symdata->d_size / symsize;
    if (!symdata->d_buf || !strdata->d_buf || !nsymbols || !strdata->d_size) {
        return -1;
    }

    /*
     * Build a simple hash table.
     */
    if (!(table = (struct hash *)malloc(nsymbols * sizeof(*table)))) {
        return -1;
    }
    for (i = 0; i < PRIME; i++) {
        first[i] = 0;
    }
    for (i = 0; i < nsymbols; i++) {
        table[i].name = NULL;
        table[i].hash = 0;
        table[i].next = 0;
    }
    for (i = 1; i < nsymbols; i++) {
        name = symbol_name(
            elf, symdata->d_buf, strdata->d_buf, strdata->d_size, i);
        if (name == NULL) {
            free(table);
            return -1;
        }
        if (*name != '\0') {
            table[i].name = name;
            table[i].hash = elf_hash((unsigned char *)name);
            hash = table[i].hash % PRIME;
            table[i].next = first[hash];
            first[hash] = i;
        }
    }

    /*
     * Lookup symbols, one by one.
     */
    for (np = nl; (name = np->n_name) && *name; np++) {
        hash = elf_hash((unsigned char *)name);
        for (i = first[hash % PRIME]; i; i = table[i].next) {
            if (table[i].hash == hash && !strcmp(table[i].name, name)) {
                break;
            }
        }
        if (i) {
            copy_symbol(elf, np, symdata->d_buf, i);
        } else {
            np->n_value = 0;
            np->n_scnum = 0;
            np->n_type = 0;
            np->n_sclass = 0;
            np->n_numaux = 0;
        }
    }
    free(table);
    return 0;
}

int nlist(const char *filename, struct nlist *nl) {
    int result = -1;
    unsigned oldver;
    Elf *elf;
    int fd;

    if ((oldver = elf_version(EV_CURRENT)) != EV_NONE) {
        if ((fd = open(filename, FILE_OPEN_MODE)) != -1) {
            if ((elf = elf_begin(fd, ELF_C_READ, NULL))) {
                result = _elf_nlist(elf, nl);
                elf_end(elf);
            }
            close(fd);
        }
        elf_version(oldver);
    }
    if (result) {
        while (nl->n_name && *nl->n_name) {
            nl->n_value = 0;
            nl++;
        }
    }
    return result;
}
