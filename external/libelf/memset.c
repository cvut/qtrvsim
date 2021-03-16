/*
 * memset.c - replacement for memset(3), using duff's device.
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

#if HAVE_CONFIG_H
    #include <config.h>
#endif /* HAVE_CONFIG_H */

#ifndef lint
static const char rcsid[]
    = "@(#) $Id: memset.c,v 1.11 2008/05/23 08:15:35 michael Exp $";
#endif /* lint */

#include <stddef.h> /* for size_t */
#include <sys/types.h>

void *_elf_memset(void *s, int c, size_t n) {
    char *t = (char *)s;

    if (n) {
        switch (n % 8u) {
            do {
                n -= 8;
            default:
            case 0: *t++ = (char)c;
            case 7: *t++ = (char)c;
            case 6: *t++ = (char)c;
            case 5: *t++ = (char)c;
            case 4: *t++ = (char)c;
            case 3: *t++ = (char)c;
            case 2: *t++ = (char)c;
            case 1: *t++ = (char)c;
            } while (n > 8);
        }
    }
    return s;
}
