/*
 * errmsg.c - implementation of the elf_errmsg(3) function.
 * Copyright (C) 1995 - 1999, 2004 Michael Riepe
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
    = "@(#) $Id: errmsg.c,v 1.11 2008/05/23 08:15:34 michael Exp $";
#endif /* lint */

#if HAVE_DGETTEXT
    #undef HAVE_CATGETS
    #include <libintl.h>
#else /* HAVE_DGETTEXT */
    #define dgettext(dom, str) str
#endif /* HAVE_DGETTEXT */

#if HAVE_CATGETS
    #include <nl_types.h>
static nl_catd _libelf_cat = (nl_catd)0;
#endif /* HAVE_CATGETS */

#if HAVE_DGETTEXT || HAVE_CATGETS
static const char domain[] = "libelf";
#endif /* HAVE_DGETTEXT || HAVE_CATGETS */

#if PIC
static const char *_messages[] = {
#else /* PIC */
static const char *const _messages[] = {
#endif /* PIC */
#define __err__(a, b) b,
#include "errors.h" /* include string tables from errors.h */
#undef __err__
};

const char *elf_errmsg(int err) {
    if (err == 0) {
        err = _elf_errno;
        if (err == 0) {
            return NULL;
        }
    } else if (err == -1) {
        err = _elf_errno;
    }

    if (err < 0 || err >= ERROR_NUM || _messages[err] == NULL) {
        err = ERROR_UNKNOWN;
    }

#if HAVE_CATGETS
    if (_libelf_cat == (nl_catd)0) {
        _libelf_cat = catopen(domain, 0);
    }
    if (_libelf_cat != (nl_catd)-1) {
        return catgets(_libelf_cat, 1, err + 1, _messages[err]);
    }
#endif /* HAVE_CATGETS */
    return dgettext(domain, _messages[err]);
}
