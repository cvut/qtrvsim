/*
gelftrans.c - gelf_* translation functions.
Copyright (C) 2000 - 2001 Michael Riepe

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

#if __LIBELF64

    #ifndef lint
static const char rcsid[]
    = "@(#) $Id: gelftrans.c,v 1.10 2008/05/23 08:15:34 michael Exp $";
    #endif /* lint */

    #define check_and_copy(type, d, s, name, eret)                             \
        do {                                                                   \
            if (sizeof((d)->name) < sizeof((s)->name)                          \
                && (type)(s)->name != (s)->name) {                             \
                seterr(ERROR_BADVALUE);                                        \
                return (eret);                                                 \
            }                                                                  \
            (d)->name = (type)(s)->name;                                       \
        } while (0)

    /*
     * These macros are missing on some Linux systems
     */
    #if !defined(ELF32_R_SYM) || !defined(ELF32_R_TYPE)                        \
        || !defined(ELF32_R_INFO)
        #undef ELF32_R_SYM
        #undef ELF32_R_TYPE
        #undef ELF32_R_INFO
        #define ELF32_R_SYM(i) ((i) >> 8)
        #define ELF32_R_TYPE(i) ((unsigned char)(i))
        #define ELF32_R_INFO(s, t) (((s) << 8) + (unsigned char)(t))
    #endif /* !defined(...) */

    #if !defined(ELF64_R_SYM) || !defined(ELF64_R_TYPE)                        \
        || !defined(ELF64_R_INFO)
        #undef ELF64_R_SYM
        #undef ELF64_R_TYPE
        #undef ELF64_R_INFO
        #define ELF64_R_SYM(i) ((i) >> 32)
        #define ELF64_R_TYPE(i) ((i)&0xffffffffL)
        #define ELF64_R_INFO(s, t)                                             \
            (((Elf64_Xword)(s) << 32) + ((t)&0xffffffffL))
    #endif /* !defined(...) */

static char *
get_addr_and_class(const Elf_Data *data, int ndx, Elf_Type type, unsigned *cls) {
    Scn_Data *sd = (Scn_Data *)data;
    Elf_Scn *scn;
    Elf *elf;
    size_t n;

    if (!sd) {
        return NULL;
    }
    elf_assert(sd->sd_magic == DATA_MAGIC);
    scn = sd->sd_scn;
    elf_assert(scn);
    elf_assert(scn->s_magic == SCN_MAGIC);
    elf = scn->s_elf;
    elf_assert(elf);
    elf_assert(elf->e_magic == ELF_MAGIC);
    if (elf->e_kind != ELF_K_ELF) {
        seterr(ERROR_NOTELF);
        return NULL;
    }
    if (!valid_class(elf->e_class)) {
        seterr(ERROR_UNKNOWN_CLASS);
        return NULL;
    }
    if (data->d_type != type) {
        seterr(ERROR_BADTYPE);
        return NULL;
    }
    n = _msize(elf->e_class, data->d_version, type);
    if (n == 0) {
        seterr(ERROR_UNIMPLEMENTED);
        return NULL;
    }
    if (ndx < 0 || data->d_size < (ndx + 1) * n) {
        seterr(ERROR_BADINDEX);
        return NULL;
    }
    if (!data->d_buf) {
        seterr(ERROR_NULLBUF);
        return NULL;
    }
    if (cls) {
        *cls = elf->e_class;
    }
    return (char *)data->d_buf + n * ndx;
}

GElf_Sym *gelf_getsym(Elf_Data *src, int ndx, GElf_Sym *dst) {
    GElf_Sym buf;
    unsigned cls;
    char *tmp;

    if (!dst) {
        dst = &buf;
    }
    tmp = get_addr_and_class(src, ndx, ELF_T_SYM, &cls);
    if (!tmp) {
        return NULL;
    }
    if (cls == ELFCLASS64) {
        *dst = *(Elf64_Sym *)tmp;
    } else if (cls == ELFCLASS32) {
        Elf32_Sym *src = (Elf32_Sym *)tmp;

        check_and_copy(GElf_Word, dst, src, st_name, NULL);
        check_and_copy(unsigned char, dst, src, st_info, NULL);
        check_and_copy(unsigned char, dst, src, st_other, NULL);
        check_and_copy(GElf_Half, dst, src, st_shndx, NULL);
        check_and_copy(GElf_Addr, dst, src, st_value, NULL);
        check_and_copy(GElf_Xword, dst, src, st_size, NULL);
    } else {
        seterr(ERROR_UNIMPLEMENTED);
        return NULL;
    }
    if (dst == &buf) {
        dst = (GElf_Sym *)malloc(sizeof(GElf_Sym));
        if (!dst) {
            seterr(ERROR_MEM_SYM);
            return NULL;
        }
        *dst = buf;
    }
    return dst;
}

int gelf_update_sym(Elf_Data *dst, int ndx, GElf_Sym *src) {
    unsigned cls;
    char *tmp;

    tmp = get_addr_and_class(dst, ndx, ELF_T_SYM, &cls);
    if (!tmp) {
        return 0;
    }
    if (cls == ELFCLASS64) {
        *(Elf64_Sym *)tmp = *src;
    } else if (cls == ELFCLASS32) {
        Elf32_Sym *dst = (Elf32_Sym *)tmp;

        check_and_copy(Elf32_Word, dst, src, st_name, 0);
        check_and_copy(Elf32_Addr, dst, src, st_value, 0);
        check_and_copy(Elf32_Word, dst, src, st_size, 0);
        check_and_copy(unsigned char, dst, src, st_info, 0);
        check_and_copy(unsigned char, dst, src, st_other, 0);
        check_and_copy(Elf32_Half, dst, src, st_shndx, 0);
    } else {
        seterr(ERROR_UNIMPLEMENTED);
        return 0;
    }
    return 1;
}

GElf_Dyn *gelf_getdyn(Elf_Data *src, int ndx, GElf_Dyn *dst) {
    GElf_Dyn buf;
    unsigned cls;
    char *tmp;

    if (!dst) {
        dst = &buf;
    }
    tmp = get_addr_and_class(src, ndx, ELF_T_DYN, &cls);
    if (!tmp) {
        return NULL;
    }
    if (cls == ELFCLASS64) {
        *dst = *(Elf64_Dyn *)tmp;
    } else if (cls == ELFCLASS32) {
        Elf32_Dyn *src = (Elf32_Dyn *)tmp;

        check_and_copy(GElf_Sxword, dst, src, d_tag, NULL);
        check_and_copy(GElf_Xword, dst, src, d_un.d_val, NULL);
    } else {
        seterr(ERROR_UNIMPLEMENTED);
        return NULL;
    }
    if (dst == &buf) {
        dst = (GElf_Dyn *)malloc(sizeof(GElf_Dyn));
        if (!dst) {
            seterr(ERROR_MEM_DYN);
            return NULL;
        }
        *dst = buf;
    }
    return dst;
}

int gelf_update_dyn(Elf_Data *dst, int ndx, GElf_Dyn *src) {
    unsigned cls;
    char *tmp;

    tmp = get_addr_and_class(dst, ndx, ELF_T_DYN, &cls);
    if (!tmp) {
        return 0;
    }
    if (cls == ELFCLASS64) {
        *(Elf64_Dyn *)tmp = *src;
    } else if (cls == ELFCLASS32) {
        Elf32_Dyn *dst = (Elf32_Dyn *)tmp;

        check_and_copy(Elf32_Sword, dst, src, d_tag, 0);
        check_and_copy(Elf32_Word, dst, src, d_un.d_val, 0);
    } else {
        seterr(ERROR_UNIMPLEMENTED);
        return 0;
    }
    return 1;
}

GElf_Rela *gelf_getrela(Elf_Data *src, int ndx, GElf_Rela *dst) {
    GElf_Rela buf;
    unsigned cls;
    char *tmp;

    if (!dst) {
        dst = &buf;
    }
    tmp = get_addr_and_class(src, ndx, ELF_T_RELA, &cls);
    if (!tmp) {
        return NULL;
    }
    if (cls == ELFCLASS64) {
        *dst = *(Elf64_Rela *)tmp;
    } else if (cls == ELFCLASS32) {
        Elf32_Rela *src = (Elf32_Rela *)tmp;

        check_and_copy(GElf_Addr, dst, src, r_offset, NULL);
        dst->r_info = ELF64_R_INFO(
            (Elf64_Xword)ELF32_R_SYM(src->r_info),
            (Elf64_Xword)ELF32_R_TYPE(src->r_info));
        check_and_copy(GElf_Sxword, dst, src, r_addend, NULL);
    } else {
        seterr(ERROR_UNIMPLEMENTED);
        return NULL;
    }
    if (dst == &buf) {
        dst = (GElf_Rela *)malloc(sizeof(GElf_Rela));
        if (!dst) {
            seterr(ERROR_MEM_RELA);
            return NULL;
        }
        *dst = buf;
    }
    return dst;
}

int gelf_update_rela(Elf_Data *dst, int ndx, GElf_Rela *src) {
    unsigned cls;
    char *tmp;

    tmp = get_addr_and_class(dst, ndx, ELF_T_RELA, &cls);
    if (!tmp) {
        return 0;
    }
    if (cls == ELFCLASS64) {
        *(Elf64_Rela *)tmp = *src;
    } else if (cls == ELFCLASS32) {
        Elf32_Rela *dst = (Elf32_Rela *)tmp;

        check_and_copy(Elf32_Addr, dst, src, r_offset, 0);
        if (ELF64_R_SYM(src->r_info) > 0xffffffUL
            || ELF64_R_TYPE(src->r_info) > 0xffUL) {
            seterr(ERROR_BADVALUE);
            return 0;
        }
        dst->r_info = ELF32_R_INFO(
            (Elf32_Word)ELF64_R_SYM(src->r_info),
            (Elf32_Word)ELF64_R_TYPE(src->r_info));
        check_and_copy(Elf32_Sword, dst, src, r_addend, 0);
    } else {
        seterr(ERROR_UNIMPLEMENTED);
        return 0;
    }
    return 1;
}

GElf_Rel *gelf_getrel(Elf_Data *src, int ndx, GElf_Rel *dst) {
    GElf_Rel buf;
    unsigned cls;
    char *tmp;

    if (!dst) {
        dst = &buf;
    }
    tmp = get_addr_and_class(src, ndx, ELF_T_REL, &cls);
    if (!tmp) {
        return NULL;
    }
    if (cls == ELFCLASS64) {
        *dst = *(Elf64_Rel *)tmp;
    } else if (cls == ELFCLASS32) {
        Elf32_Rel *src = (Elf32_Rel *)tmp;

        check_and_copy(GElf_Addr, dst, src, r_offset, NULL);
        dst->r_info = ELF64_R_INFO(
            (Elf64_Xword)ELF32_R_SYM(src->r_info),
            (Elf64_Xword)ELF32_R_TYPE(src->r_info));
    } else {
        seterr(ERROR_UNIMPLEMENTED);
        return NULL;
    }
    if (dst == &buf) {
        dst = (GElf_Rel *)malloc(sizeof(GElf_Rel));
        if (!dst) {
            seterr(ERROR_MEM_REL);
            return NULL;
        }
        *dst = buf;
    }
    return dst;
}

int gelf_update_rel(Elf_Data *dst, int ndx, GElf_Rel *src) {
    unsigned cls;
    char *tmp;

    tmp = get_addr_and_class(dst, ndx, ELF_T_REL, &cls);
    if (!tmp) {
        return 0;
    }
    if (cls == ELFCLASS64) {
        *(Elf64_Rel *)tmp = *src;
    } else if (cls == ELFCLASS32) {
        Elf32_Rel *dst = (Elf32_Rel *)tmp;

        check_and_copy(Elf32_Addr, dst, src, r_offset, 0);
        if (ELF64_R_SYM(src->r_info) > 0xffffffUL
            || ELF64_R_TYPE(src->r_info) > 0xffUL) {
            seterr(ERROR_BADVALUE);
            return 0;
        }
        dst->r_info = ELF32_R_INFO(
            (Elf32_Word)ELF64_R_SYM(src->r_info),
            (Elf32_Word)ELF64_R_TYPE(src->r_info));
    } else {
        seterr(ERROR_UNIMPLEMENTED);
        return 0;
    }
    return 1;
}

    #if 0

GElf_Syminfo*
gelf_getsyminfo(Elf_Data *src, int ndx, GElf_Syminfo *dst) {
    seterr(ERROR_UNIMPLEMENTED);
    return NULL;
}

int
gelf_update_syminfo(Elf_Data *dst, int ndx, GElf_Syminfo *src) {
    seterr(ERROR_UNIMPLEMENTED);
    return 0;
}

GElf_Move*
gelf_getmove(Elf_Data *src, int ndx, GElf_Move *src) {
    seterr(ERROR_UNIMPLEMENTED);
    return NULL;
}

int
gelf_update_move(Elf_Data *dst, int ndx, GElf_Move *src) {
    seterr(ERROR_UNIMPLEMENTED);
    return 0;
}

    #endif

#endif /* __LIBELF64 */
