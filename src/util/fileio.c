/*

    Copyright (C) 2005-2012 Michael Liebscher <johnnycanuck@users.sourceforge.net>

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
 * \file files.c
 * \brief Interface to portable file i/o layer.
 * \author Michael Liebscher
 * \date 2005-2012
 */

#include <string.h>
#include <stdio.h>
#include <zlib.h>

#include "platform.h"
#include "../common.h"
#include "com_string.h"

/**
 * \brief Get the length of a file.
 * \param[in] Target file handle.
 * \return The file length in bytes.
 */
SW32 FS_GetFileSize (filehandle_t *fhandle)
{
    SW32    pos;
    SW32    end;

    if (fhandle->bLoaded) {
        return fhandle->filesize;
    }

    pos = ftell (fhandle->hFile);
    fseek (fhandle->hFile, 0, SEEK_END);
    end = ftell (fhandle->hFile);
    fseek (fhandle->hFile, pos, SEEK_SET);

    return end;
}

/**
 * \brief Moves the file pointer to a specified location.
 * \param[in] fhandle Pointer to filehandle_t structure.
 * \param[in] offset Number of bytes from origin
 * \param[in] origin Initial position
 *                          SEEK_SET -Beginning of file.
 *                          SEEK_CUR -Current position of file pointer.
 *                          SEEK_END -End of file.
 * \return If successful zero, otherwise a nonzero value.
 */
W32 FS_FileSeek (filehandle_t *fhandle, SW32 offset, W32 origin)
{
    if (fhandle->bLoaded) {
        switch (origin) {
        case SEEK_SET:
            if (offset < 0 ||
                    offset > fhandle->filesize) {
                return 1;
            }

            fhandle->ptrCurrent = fhandle->ptrStart + offset;
            break;

        case SEEK_END:
            if (offset > 0) {
                return 1;
            }

            // offset is negative
            if ((fhandle->filesize + offset) < 0) {
                return 1;
            }

            // offset is negative
            fhandle->ptrCurrent = fhandle->ptrEnd + offset;
            break;

        case SEEK_CUR:
            if (offset < 0) {
                // offset is negative
                if (((fhandle->ptrCurrent - fhandle->ptrStart) + offset) < 0) {
                    return 1;
                }
            }

            if (offset > 0) {
                if (offset > (fhandle->ptrEnd - fhandle->ptrCurrent)) {
                    return 1;
                }
            }

            fhandle->ptrCurrent += offset;
            break;

        default:
            return 1;
        }

        return 0;
    }

    return fseek (fhandle->hFile, offset, origin);
}

/**
 * \brief Close file handle.
 * \param[in] filestream Pointer to valid FILE structure.
 * \note Closes a file stream that was returned by FS_FOpenFile.
 */
void FS_CloseFile (filehandle_t *fhandle)
{
    if (fhandle->hFile) {
        fclose (fhandle->hFile);
    }

    if (fhandle->filedata) {
        free (fhandle->filedata);
    }

    free (fhandle);
}

/**
 * \brief Load the file into memory.
 * \param[out] Pointer to a valid filehandle_t structure.
 * \return true on success, otherwise false.
 */
static _boolean LoadFile (filehandle_t *hFile)
{
    W32 read;

    hFile->filesize = FS_GetFileSize (hFile);
    hFile->filedata = malloc (hFile->filesize);

    read = fread (hFile->filedata, 1, hFile->filesize, hFile->hFile);

    if (read != hFile->filesize) {
        fclose (hFile->hFile);

        return false;
    }

    fclose (hFile->hFile);

    hFile->hFile = NULL;

    // align our file data pointers
    hFile->ptrStart =  hFile->ptrCurrent = (PW8)hFile->filedata;
    hFile->ptrEnd = (PW8)hFile->filedata + hFile->filesize;

    hFile->bLoaded = true;

    return true;
}

/**
 * \brief Open file from the file system.
 * \param[in] filename Pointer to a NUL-terminated string with the
 * \param[in] FlagsAndAttributes Flags and attributes when opening file
 * \return NULL on error, otherwise pointer to valid filehandle_t structure.
 * \note Finds the file in the search path.
 *      Used for streaming data out of either a pak file or a seperate file.
 *      Use the FS_CloseFile function to close an object handle returned
 *      by FS_OpenFile.
 */
filehandle_t *FS_OpenFile (const char *filename, W32 FlagsAndAttributes)
{
    char            netpath[ MAX_OSPATH ];
    filehandle_t    *hFile;

    hFile = (filehandle_t *)malloc (sizeof (filehandle_t));
    memset (hFile, 0, sizeof (filehandle_t));

//
//  Check for the file in the directory tree
//
    com_snprintf (netpath, sizeof (netpath), "%s/%s", get_resource_base_path(), filename);

    printf("Loading file: %s\n", netpath);

    hFile->hFile = fopen (netpath, "rb");

    if (hFile->hFile) {
        printf("[FS_OpenFile]: %s\n", netpath);

        if (FlagsAndAttributes & FA_FILE_FLAG_LOAD) {
            if (! LoadFile (hFile)) {
                FS_CloseFile (hFile);

                return NULL;
            }
        }
        return hFile;
    }

    printf("[FS_OpenFile]: Could not find (%s)\n", filename);

    FS_CloseFile (hFile);

    return NULL;
}

/**
 * \brief Reads data from a stream.
 * \param[in\out] buffer Storage location for data.
 * \param[in] size Item size in bytes.
 * \param[in] count Maximum number of items to be read.
 * \param[in] fhandle Pointer to valid filehandle_t structure.
 * \return On success number of full items actually read, otherwise -1.
 */
SW32 FS_ReadFile (void *buffer, W32 size, W32 count, filehandle_t *fhandle)
{
    W8  *buf = (PW8)buffer;

    if (fhandle->bLoaded) {
        W32 i;

        if ((size * count) > (fhandle->ptrEnd - fhandle->ptrCurrent)) {
            SW32 read;

            read = (fhandle->ptrEnd - fhandle->ptrCurrent);

            for (i = 0 ; i < (fhandle->ptrEnd - fhandle->ptrCurrent) ; ++i) {
                buf[ i ] = fhandle->ptrCurrent[ i ];
            }

            fhandle->ptrCurrent = fhandle->ptrEnd;

            return (read);
        } else {
            for (i = 0 ; i < (size * count) ; ++i, fhandle->ptrCurrent++) {
                buf[ i ] = *fhandle->ptrCurrent;
            }

            return ((size * count) / size);
        }
    } else {
        return fread (buf, size, count, fhandle->hFile);
    }

    /* should never get here */
    return -1;
}
