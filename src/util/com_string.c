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
#include "../common.h"


/**
 * \brief Appends one string to another.
 * \param[in,out] dest Pointer to a NUL-terminated string. The buffer must be large enough to contain both strings or else truncation will occur.
 * \param[in] source Pointer to a NUL-terminated string from which the function copies characters.
 * \param[in] nMaxLength Full size of dest, not space left.
 * \return Returns strlen( source ) + MIN( nMaxLength, strlen( initial dest ) ). If retval >= nMaxLength, truncation occurred.
 * \note At most \c nMaxLength-1 characters will be copied. Always NUL-terminates (unless \c nMaxLength <= strlen( dest) ).
 */
PUBLIC size_t com_strlcat (char *dest, const char *source, size_t nMaxLength)
{
    char *d = dest;
    const char *s = source;
    size_t n = nMaxLength;
    size_t dlen;

    /* Find the end of dest and adjust bytes left but don't go past end */
    while (n-- != 0 && *d != '\0') {
        d++;
    }

    dlen = d - dest;
    n = nMaxLength - dlen;

    /* No room left to append string */
    if (n == 0) {
        return (dlen + strlen (s));
    }

    while (*s != '\0') {
        if (n != 1) {
            *d++ = *s;
            n--;
        }

        s++;
    }

    *d = '\0';  /* NUL-terminate string */

    return (dlen + (s - source));    /* count does not include NUL */

}

/**
 * \brief Compare characters of two strings without regard to case.
 * \param[in] string1 NUL-terminated strings to compare.
 * \param[in] string2 NUL-terminated strings to compare.
 * \param[in] count Number of characters to compare.
 * \return The return value indicates the relationship between the substrings as follows.
 * \arg \c <0  \c string1 substring less than \c string2 substring
 * \arg \c 0  \c string1 substring identical to \c string2 substring
 * \arg \c >0 \c string1 substring greater than \c string2 substring
 */
PUBLIC int com_strnicmp (const char *string1, const char *string2, size_t count)
{
    char c1, c2;

    if (!string1 || !*string1 ||
            !string2 || !*string2) {
        return -1;
    }

    do {
        c1 = *string1++;
        c2 = *string2++;

        if (!count--) {
            return 0;       /* strings are equal until end point */
        }

        if (c1 != c2) {
            if (TOUPPER (c1) != TOUPPER (c2)) {  /* Uppercase compare */
                return -1;  /* strings are not equal */
            }
        }

    } while (c1);

    return 0;       /* strings are equal */
}

/**
 * \brief Perform a lowercase comparison of strings.
 * \param[in] string1 NUL-terminated strings to compare.
 * \param[in] string2 NUL-terminated strings to compare.
 * \return The return value indicates the relation of \c string1 to \c string2 as follows.
 * \arg \c <0  \c string1 less than \c string2
 * \arg \c 0  \c string1 identical to \c string2
 * \arg \c >0 \c string1 greater than \c string2
 */
PUBLIC int com_stricmp (const char *string1, const char *string2)
{
    return com_strnicmp (string1, string2, 99999);
}

/**
 * \brief Write formatted data to a string.
 * \param[out] dest Storage location for output.
 * \param[in] size Maximum number of characters to store.
 * \param[in] format Format-control string.
 * \param[in]  ... Optional arguments.
 * \note If the format string is longer than 32768 truncation will occur. Also, if \c format is longer than \c dest truncation will occur.
 */
PUBLIC void com_snprintf (char *dest, size_t size, const char *format, ...)
{
    va_list argptr;
    char    bigbuffer[ 0x8000 ];

    va_start (argptr, format);
    vsnprintf (bigbuffer, sizeof (bigbuffer), format, argptr);
    va_end (argptr);

    bigbuffer[ sizeof (bigbuffer) - 1 ] = '\0';
// Debugging.
//  printf("Contenido de bigbuffer: '%s'\n", bigbuffer);

    strncpy(dest, bigbuffer, size);
}

/**
 * \brief Create another copy of string.
 * \param[in] source NUL-terminated string to copy.
 * \return Pointer to copy, otherwise NULL.
 * \note Caller is responsible for freeing allocated data by calling Z_Free
 */
PUBLIC char *com_strcopy (const char *source)
{
    char *dest;
    W32 length;

    length = strlen (source) + 1;
    dest = malloc (length);

    if (dest == NULL) {
        return NULL;
    }

    strncpy(dest, source, length);

    return dest;
}

/**
 * \brief Create a hash id from string.
 * \param[in] string NUL-terminated string to hash.
 * \return Hash id.
 */
PUBLIC W32 com_strhash (const char *string)
{
    W32 hash = *string;

    if (hash) {
        for (string += 1; *string != '\0'; ++string) {
            hash = (hash << 5) - hash + *string;
        }
    }

    return hash;
}

/**
 * \brief Does a varargs com_snprintf into a temp buffer.
 * \param[in] format  Format-control string.
 * \param[in] ... Optional arguments.
 * \return Formatted string.
 * \note If format string is longer than 1024 it will be truncated.
 */
PUBLIC char *va (char *format, ...)
{
    va_list argptr;
    static char string[ 1024 ];

    va_start (argptr, format);
    (void)com_snprintf (string, sizeof (string), format, argptr);
    va_end (argptr);

    string[ sizeof (string) - 1 ] = '\0';

    return string;
}