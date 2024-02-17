#include <stdint.h>
/* config.h.  Generated from config.h.in by configure.  */
/* config.h.in.  Generated from configure.in by autoheader.  */
/*
 * acconfig.h - Special definitions for libelf, processed by autoheader.
 * Copyright (C) 1995 - 2001, 2004, 2006 Michael Riepe
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

/* @(#) $Id: acconfig.h,v 1.16 2008/05/23 08:17:56 michael Exp $ */

/* Define if you want to include extra debugging code */
/* #undef ENABLE_DEBUG */

/* Define if you want to support extended ELF formats */
/* #undef ENABLE_EXTENDED_FORMAT */

/* Define if you want ELF format sanity checks by default */
#define ENABLE_SANITY_CHECKS 1

/* Define if memmove() does not copy overlapping arrays correctly */
#define HAVE_BROKEN_MEMMOVE 1

/* Define if you have the catgets function. */
/* #undef HAVE_CATGETS */

/* Define if you have the dgettext function. */
/* #undef HAVE_DGETTEXT */

/* Define if you have the memset function.  */
#define HAVE_MEMSET 1

/* Define if struct nlist is declared in <elf.h> or <sys/elf.h> */
/* #undef HAVE_STRUCT_NLIST_DECLARATION */

/* Define if Elf32_Dyn is declared in <link.h> */
/* #undef __LIBELF_NEED_LINK_H */

/* Define if Elf32_Dyn is declared in <sys/link.h> */
/* #undef __LIBELF_NEED_SYS_LINK_H */

/* Define to `<elf.h>' or `<sys/elf.h>' if one of them is present */
/* #undef __LIBELF_HEADER_ELF_H */

/* Define if you want 64-bit support (and your system supports it) */
#define __LIBELF64 1

/* Define if you want 64-bit support, and are running IRIX */
/* #undef __LIBELF64_IRIX */

/* Define if you want 64-bit support, and are running Linux */
/* #undef __LIBELF64_LINUX */

/* Define if you want symbol versioning (and your system supports it) */
#define __LIBELF_SYMBOL_VERSIONS 1

/* Define if symbol versioning uses Sun section type (SHT_SUNW_*) */
#define __LIBELF_SUN_SYMBOL_VERSIONS 1

/* Define if symbol versioning uses GNU section types (SHT_GNU_*) */
/* #undef __LIBELF_GNU_SYMBOL_VERSIONS */

/* Define to a 64-bit signed integer type if one exists */
#define __libelf_i64_t int64_t

/* Define to a 64-bit unsigned integer type if one exists */
#define __libelf_u64_t uint64_t

/* Define to a 32-bit signed integer type if one exists */
#define __libelf_i32_t int32_t

/* Define to a 32-bit unsigned integer type if one exists */
#define __libelf_u32_t uint32_t

/* Define to a 16-bit signed integer type if one exists */
#define __libelf_i16_t int16_t

/* Define to a 16-bit unsigned integer type if one exists */
#define __libelf_u16_t uint16_t

/* Define to 1 if you have the <ar.h> header file. */
/* #undef HAVE_AR_H */

/* Define to 1 if you have the <elf.h> header file. */
/* #undef HAVE_ELF_H */

/* Define to 1 if you have the <fcntl.h> header file. */
#define HAVE_FCNTL_H 1

/* Define to 1 if you have the `ftruncate' function. */
#define HAVE_FTRUNCATE 1

/* Define to 1 if you have the <gelf.h> header file. */
/* #undef HAVE_GELF_H */

/* Define to 1 if you have the `getpagesize' function. */
#define HAVE_GETPAGESIZE 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the <libelf.h> header file. */
/* #undef HAVE_LIBELF_H */

/* Define to 1 if you have the <link.h> header file. */
/* #undef HAVE_LINK_H */

/* Define to 1 if you have the `memcmp' function. */
#define HAVE_MEMCMP 1

/* Define to 1 if you have the `memcpy' function. */
#define HAVE_MEMCPY 1

/* Define to 1 if you have the `memmove' function. */
#define HAVE_MEMMOVE 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have the `memset' function. */
#define HAVE_MEMSET 1

/* Define to 1 if you have a working `mmap' system call. */
/* #undef HAVE_MMAP */

/* Define to 1 if you have the <nlist.h> header file. */
/* #undef HAVE_NLIST_H */

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the <sys/elf.h> header file. */
/* #undef HAVE_SYS_ELF_H */

/* Define to 1 if you have the <sys/link.h> header file. */
/* #undef HAVE_SYS_LINK_H */

/* Define to 1 if you have the <sys/param.h> header file. */
#define HAVE_SYS_PARAM_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT ""

/* Define to the full name of this package. */
#define PACKAGE_NAME ""

/* Define to the full name and version of this package. */
#define PACKAGE_STRING ""

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME ""

/* Define to the home page for this package. */
#define PACKAGE_URL ""

/* Define to the version of this package. */
#define PACKAGE_VERSION ""

/* The size of `int', as computed by sizeof. */
#define SIZEOF_INT 4

/* The size of `long', as computed by sizeof. */
#define SIZEOF_LONG 4

/* The size of `long long', as computed by sizeof. */
#define SIZEOF_LONG_LONG 8

/* The size of `short', as computed by sizeof. */
#define SIZEOF_SHORT 2

/* The size of `__int64', as computed by sizeof. */
#define SIZEOF___INT64 8

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Define to empty if `const' does not conform to ANSI C. */
/* #undef const */

/* Define to `long int' if <sys/types.h> does not define. */
/* #undef off_t */

/* Define to `unsigned int' if <sys/types.h> does not define. */
/* #undef size_t */
