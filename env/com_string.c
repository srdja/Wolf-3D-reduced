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

#include <math.h>

#include "cmd.h"
#include "com_string.h"
#include "zmem.h"
#include "common_utils.h"




/**
 * \brief Copies a specified number of characters from a source string into a buffer.
 * \param[in,out] dest Pointer to a buffer into which the function copies characters.
 * \param[in] source Pointer to a NUL-terminated string from which the function copies characters.
 * \param[in] nMaxLength Specifies the number of bytes to be copied from the string pointed to by source into the buffer pointed to by dest.
 * \return Returns the string length of \c source. If \c retval >= \c nMaxLength, truncation occurred.
 * \note At most \c nMaxLength-1 characters will be copied. Always NUL-terminates (unless \c nMaxLength == 0).
 */
PUBLIC size_t com_strlcpy (char *dest, const char *source, size_t nMaxLength)
{
    char *d = dest;
    const char *s = source;
    size_t n = nMaxLength;

    /* Copy as many bytes as will fit */
    if (n != 0 && --n != 0) {
        do {
            if ((*d++ = *s++) == 0) {
                break;
            }

        } while (--n != 0);
    }

    /* Not enough room in dest, add NUL and traverse rest of source */
    if (n == 0) {
        if (nMaxLength != 0) {
            *d = '\0';      /* NUL-terminate dest */
        }

        while (*s++) {
            ;
        }
    }

    return (s - source - 1);     /* count does not include NUL */
}


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
    int writtenLength;

    va_start (argptr, format);
    vsnprintf (bigbuffer, sizeof (bigbuffer), format, argptr);
    va_end (argptr);

    bigbuffer[ sizeof (bigbuffer) - 1 ] = '\0';
// Debugging.
//  printf("Contenido de bigbuffer: '%s'\n", bigbuffer);

    com_strlcpy (dest, bigbuffer, size);
}


/**
 * \brief Convert a string to uppercase.
 * \param[in,out] string NUL-terminated string to capitalize.
 * \return This functions returns a pointer to the converted string. Because the modification is done in place,
 *  the pointer returned is the same as the pointer passed as the input argument. No return value is reserved to indicate an error.
 */
PUBLIC char *com_strupr (char *string)
{
    char *ptr;

    if (!string || !*string) {
        return string;
    }

    ptr = string;

    do {
        *ptr = TOUPPER (*ptr);

    } while (*ptr++);


    return string;
}


/**
 * \brief Convert a string to lowercase.
 * \param[in,out] string NUL-terminated string to convert to lowercase.
 * \return This functions returns a pointer to the converted string. Because the modification is done in place,
 *  the pointer returned is the same as the pointer passed as the input argument. No return value is reserved to indicate an error.
 */
PUBLIC char *com_strlwr (char *string)
{
    char *ptr;

    if (!string || !*string) {
        return string;
    }

    ptr = string;

    do {
        *ptr = TOLOWER (*ptr);

    } while (*ptr++);


    return string;
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
    dest = Z_Malloc (length);

    if (dest == NULL) {
        return NULL;
    }

    com_strlcpy (dest, source, length);

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
 * \brief Convert string to integer.
 * \param[in] string A NUL-terminated string that contains a number to convert.
 * \param[in,out] error Error code. See header.
 * \return A 32-bit signed integer equivalent to the number contained in \c string.
 */
PUBLIC SW32 StringToInteger (const char *string, W32 *error)
{
    const char *ptr = string;
    SW32 temp;
    SW32 number = 0;
    W32 errortag = 0;
    _boolean bNegative = false;

    if (!string || !*string) {
        *error = SCE_NULL_VALUE;

        return 0;
    }

    if (*ptr == '-') {
        bNegative = true;

        ptr++;
    }

    while (*ptr && ISNUMERIC (*ptr)) {
        temp = number;
        number = (number * 10) + *ptr - '0';

        if (number < temp) {
            errortag &= SCE_BUFFER_OVERFLOW;
        }

        ptr++;
    }

    if (*ptr) {
        errortag &= SCE_NON_NUMERIC;
    }


    if (bNegative) {
        number = -number;
    }


    *error = errortag;

    return number;
}

/**
 * \brief Converts the string representation of a number to its floating-point number equivalent.
 * \param[in] string A NUL-terminated string that contains a number to convert.
 * \param[in,out] error Error code. See header.
 * \return A double-precision floating-point number that is equivalent to the numeric value or symbol specified in \c string
 */
PUBLIC double StringToFloat (const char *string, W32 *error)
{
    const char *ptr = string;
    double number = 0;
    SW32 exponent = 0;
    W32 expError = 0;
    _boolean bNegative = false;

    *error = 0;

    if (!string || !*string) {
        *error &= SCE_NULL_VALUE;

        return 0;
    }

    if (*ptr == '-') {
        bNegative = true;

        ptr++;
    } else if (*ptr == '+') {
        ptr++;
    }

    while (*ptr && ISNUMERIC (*ptr)) {
        number = (number * 10) + (double) (*ptr - '0');

        ptr++;
    }

    if (*ptr == '.') {
        ptr++;

        while (*ptr && ISNUMERIC (*ptr)) {
            number = (number * 10) + (double) (*ptr - '0');
            exponent--;

            ptr++;
        }
    }

    if (TOLOWER (*ptr) == 'e') {
        ptr++;

        exponent += StringToInteger (ptr, &expError);
    }

    if (bNegative) {
        number = -number;
    }

    if (expError) {
        *error |= expError;
    }

    return (number * pow (10, exponent));
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

/**
 * \brief Parse a token out of a string.
 * \param[in/out] data_p  String to parse.
 * \return On success it will return the token string, otherwise it will return an empty string.
 */
PUBLIC char *com_parse (char **data_p)
{
    int     c;
    int     len;
    char        *data;
    static char com_token[ MAX_TOKEN_CHARS ];

    data = *data_p;
    len = 0;
    com_token[ 0 ] = 0;

    if (!data) {
        *data_p = NULL;
        return "";
    }

// skip whitespace
skipwhite:

    while ((c = *data) <= ' ') {
        if (c == 0) {
            *data_p = NULL;
            return "";
        }

        data++;
    }

// skip // comments
    if (c == '/' && data[ 1 ] == '/') {
        while (*data && *data != '\n') {
            data++;
        }

        goto skipwhite;
    }

// handle quoted strings specially
    if (c == '\"') {
        data++;

        while (1) {
            c = *data++;

            if (c == '\"' || !c) {
                com_token[ len ] = 0;
                *data_p = data;
                return com_token;
            }

            if (len < MAX_TOKEN_CHARS) {
                com_token[ len ] = c;
                len++;
            }
        }
    }

// parse a regular word
    do {
        if (len < MAX_TOKEN_CHARS) {
            com_token[ len ] = c;
            len++;
        }

        data++;
        c = *data;

    } while (c > 32);

    if (len == MAX_TOKEN_CHARS) {
//      Com_Printf ("Token exceeded %i chars, discarded.\n", MAX_TOKEN_CHARS);
        len = 0;
    }

    com_token[ len ] = 0;

    *data_p = data;

    return com_token;
}
