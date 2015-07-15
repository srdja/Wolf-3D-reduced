/*

    Copyright (C) 2004 Michael Liebscher <johnnycanuck@users.sourceforge.net>
    Copyright (c) 1998 Todd C. Miller <Todd.Miller@courtesan.com>

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
 * \file com_string.h
 * \brief String functions done in a portable manner.
 * \author Michael Liebscher
 * \date 2004-2012
 * \author Todd C. Miller
 * \date 2004-2006
 * \note Portion of this code was derived from code that was originally written by Todd C. Miller.
 */

#ifndef __COM_STRING_H__
#define __COM_STRING_H__

#include <stdlib.h>
#include "platform.h"

#define ISUPPER( c )    ( ( c ) >= 'A' && ( c ) <= 'Z' )
#define ISLOWER( c )    ( ( c ) >= 'a' && ( c ) <= 'z' )
#define TOUPPER( c )    ( ISLOWER( c ) ? (c) - 'a' + 'A' : ( c ) )
#define TOLOWER( c )    ( ISUPPER( c ) ? (c) - 'A' + 'a' : ( c ) )

size_t com_strlcat (char *dest, const char *source, size_t nMaxLength);

int com_stricmp (const char *string1, const char *string2);
int com_strnicmp (const char *string1, const char *string2, size_t count);

void com_snprintf (char *dest, size_t size, const char *format, ...);
W32 com_strhash (const char *string);


char *va (char *format, ...);

#endif /* __COM_STRING_H__ */
