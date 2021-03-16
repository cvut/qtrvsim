/*
swap64.c - 64-bit byte swapping functions.
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

#include "byteswap.h"
#include "private.h"

#if __LIBELF64

    #ifndef lint
static const char rcsid[]
    = "@(#) $Id: swap64.c,v 1.6 2008/05/23 08:15:35 michael Exp $";
    #endif /* lint */

__libelf_u64_t _elf_load_u64L(const unsigned char *from) {
    return ((__libelf_u64_t)__load_u32L(from + 4) << 32)
           | (__libelf_u64_t)__load_u32L(from);
}

__libelf_u64_t _elf_load_u64M(const unsigned char *from) {
    return ((__libelf_u64_t)__load_u32M(from) << 32)
           | (__libelf_u64_t)__load_u32M(from + 4);
}

__libelf_i64_t _elf_load_i64L(const unsigned char *from) {
    return ((__libelf_i64_t)__load_i32L(from + 4) << 32)
           | (__libelf_u64_t)__load_u32L(from);
}

__libelf_i64_t _elf_load_i64M(const unsigned char *from) {
    return ((__libelf_i64_t)__load_i32M(from) << 32)
           | (__libelf_u64_t)__load_u32M(from + 4);
}

void _elf_store_u64L(unsigned char *to, __libelf_u64_t v) {
    __store_u32L(to, (__libelf_u32_t)v);
    v >>= 32;
    __store_u32L(to + 4, (__libelf_u32_t)v);
}

void _elf_store_u64M(unsigned char *to, __libelf_u64_t v) {
    __store_u32M(to + 4, (__libelf_u32_t)v);
    v >>= 32;
    __store_u32M(to, (__libelf_u32_t)v);
}

void _elf_store_i64L(unsigned char *to, __libelf_u64_t v) {
    __store_u32L(to, (__libelf_u32_t)v);
    v >>= 32;
    __store_i32L(to + 4, (__libelf_u32_t)v);
}

void _elf_store_i64M(unsigned char *to, __libelf_u64_t v) {
    __store_u32M(to + 4, (__libelf_u32_t)v);
    v >>= 32;
    __store_i32M(to, (__libelf_u32_t)v);
}

#endif /* __LIBELF64 */
