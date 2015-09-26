/*

    Copyright (C) 2004-2012 Michael Liebscher <johnnycanuck@users.sourceforge.net>
    Copyright (C) 1998 Todd C. Miller <Todd.Miller@courtesan.com>

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

/**
 * \file com_string.c
 * \brief String functions done in a portable manner.
 * \author Michael Liebscher
 * \date 2004-2012
 * \author Todd C. Miller
 * \date 2004-2006
 * \note Portion of this code was derived from code that was originally written by Todd C. Miller.
 */

#include <stdlib.h>
#include <string.h>

#include <stdio.h>
#include <stdarg.h>

#include "com_string.h"

/**
 * \brief Write formatted data to a string.
 * \param[out] dest Storage location for output.
 * \param[in] size Maximum number of characters to store.
 * \param[in] format Format-control string.
 * \param[in]  ... Optional arguments.
 * \note If the format string is longer than 32768 truncation will occur. Also, if \c format is longer than \c dest truncation will occur.
 */
void com_snprintf (char *dest, size_t size, const char *format, ...)
{
    va_list argptr;
    char    bigbuffer[ 0x8000 ];

    va_start (argptr, format);
    vsnprintf (bigbuffer, sizeof (bigbuffer), format, argptr);
    va_end (argptr);

    bigbuffer[ sizeof (bigbuffer) - 1 ] = '\0';

    strncpy(dest, bigbuffer, size);
}
