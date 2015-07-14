/*

    Copyright (C) 2006-2007 Michael Liebscher <johnnycanuck@users.sourceforge.net>

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
 * \file file_string.c
 * \brief Path and file name string manipulation.
 * \author Michael Liebscher
 * \date 2006-2007
 */


#include <string.h>

#include "filesystem.h"
#include "common.h"

#include "com_string.h"


/**
 * \brief Returns file name from path string.
 * \param[in] path Path to get file name from.
 * \return Pointer to start of file name, otherwise NULL.
 */
PUBLIC char *FS_getFileName (char *path)
{
    char *last;

    last = path;

    while (*path) {
        if (*path == PATH_SEP) {
            last = path + 1;
        }

        path++;
    }

    return last;
}

/**
 * \brief Removes file extension from path string.
 * \param[in] in Path to remove file extension.
 * \param[in] out Pointer to hold path string.
 * \return Nothing.
 */
PUBLIC void FS_RemoveExtension (const char *in, char *out)
{
    while (*in && *in != '.') {
        *out++ = *in++;
    }

    *out = '\0';    /* NUL-terminate string */
}


/**
 * \brief Returns path from full path.
 * \param[in] in Path with file name.
 * \param[in,out] out path.
 * \param[in] size_out Size of out buffer in bytes.
 * \return On success true, otherwise false.
 */
PUBLIC _boolean FS_getPath (const char *in, char *out, W32 size_out)
{
    const char *start;

    start = in + strlen (in) - 1;

    while (start != in && *start != PATH_SEP) {
        start--;
    }

    if (((start - in) + 1) >= (size_out - 1)) {
        return false;
    }

    strncpy(out, in, (start - in) + 1);
    return true;
}

/**
 * \brief Append file extension to path if necessary.
 * \param[in] in Path with file name.
 * \param[in] maxSize Max size of path buffer.
 * \param[in] extension Extension to append to path.
 */
PUBLIC void FS_DefaultExtension (char *path, const int maxSize, const char *extension)
{
    char    oldPath[MAX_OSPATH];
    char    *src;

    src = path + strlen (path) - 1;

    while (*src != '/' && src != path) {
        if (*src == '.') {
            return;                 // it has an extension
        }

        src--;
    }
    strncpy(oldPath, path, sizeof(oldPath));
    com_snprintf (path, maxSize, "%s%s", oldPath, extension);
}