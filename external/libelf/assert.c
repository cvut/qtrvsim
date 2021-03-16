/*
assert.c - assert function for libelf.
Copyright (C) 1999 Michael Riepe

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
    = "@(#) $Id: assert.c,v 1.5 2008/05/23 08:15:34 michael Exp $";
#endif /* lint */

#include <stdio.h>

void __elf_assert(const char *file, unsigned line, const char *cond) {
    fprintf(stderr, "%s:%u: libelf assertion failure: %s\n", file, line, cond);
    abort();
}
