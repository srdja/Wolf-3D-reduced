/*
    Copyright (C) 2004 Michael Liebscher <johnnycanuck@users.sourceforge.net>
    Copyright (C) 1997-2001 Id Software, Inc.

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
 * \file unix_file.c
 * \brief Handles non-portable file services.
 * \author Michael Liebscher <johnnycanuck@users.sourceforge.net>
 * \author 1997-2001 Id Software, Inc.
 * \note Portion of this code was derived from Quake II, and was originally written by Id Software, Inc.
 * \date 2004
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>

#include "../env/common.h"


/**
 * \brief Creates a new directory.
 * \param[in] dirname Pointer to a NUL-terminated string that specifies the path of the directory to be created.
 * \return On success nonzero, otherwise zero.
 */
PUBLIC W8 FS_CreateDirectory (const char *dirname)
{
    int ret_val = mkdir (dirname, S_IRUSR | S_IWUSR | S_IXUSR);

    if (ret_val == -1 && errno == EEXIST)
        ret_val = 0;

    return (W8) (! ret_val);
}

/**
 * \brief Changes the current directory
 * \param[in] path Pointer to a NUL-terminated string that specifies the path to the new directory.
 * \return On success nonzero, otherwise zero.
 */
PUBLIC W8 FS_ChangeCurrentDirectory (const char *path)
{
    return ! chdir (path);
}

/**
 * \brief Deletes an existing file.
 * \param[in] filename Pointer to a NUL-terminated string that specifies the file to be deleted.
 * \return If successful the return value is nonzero, otherwise zero.
 */
PUBLIC _boolean FS_DeleteFile (const char *filename)
{
    return (! unlink (filename));
}

/**
 * \brief Deletes an existing empty directory.
 * \param[in] pathname Pointer to a NUL-terminated string that specifies the directory to be deleted.
 * \return If successful the return value is nonzero, otherwise zero.
 */
PUBLIC _boolean FS_RemoveDirectory (const char *pathname)
{
    return (! rmdir (pathname));
}

PUBLIC  char *FS_Userdir()
{

}
