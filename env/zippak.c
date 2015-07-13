/*

    Copyright (C) 2004 Michael Liebscher  <johnnycanuck@users.sourceforge.net>

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
 * \file zippak.c
 * \brief Parse zip file
 * \author Michael Liebscher
 * \date 2004
 * \note This zip parser is done for speed and so it makes a couple of assumptions.
 *  1. CRC of the uncompressed data is not checked.
 *  2. Assumes length of uncompressed data value is accurate (for now).
 *  3. Only parses the local file headers (central directory headers are
 *      redundant for what this module supports and end of central directory
 *      record is not processed).
 *
 *  Reference document:
 *  http://www.pkware.com/support/zip-app-note
 */


#include <stdio.h>
#include <string.h>

#include "zippak.h"
#include "common.h"
#include "com_string.h"


/* Signatures for zip file information headers */
#define SIG_LOCAL           0x04034b50L


/* Length of header (not counting the signature) */
#define LOCALHEAD_SIZE      26


typedef struct localzipheader_s {
    W16 version_needed;
    W16 flag;
    W16 compression_method;
    W32 timedate;
    W32 crc32;
    W32 compressed_size;
    W32 uncompressed_size;
    W16 filename_length;
    W16 extrafield_length;

} localzipheader_t;

/**
 * \brief Check zip file name string.
 * \param[in] string String to check.
 * \return false if not acceptable, otherwise true.
 * \note Changes '\' to the more acceptable '/'
 */
PRIVATE _boolean checkstring (char *string)
{
    char *s = string;

    while (*s) {

        if (*s == ':') {
            // absolute path given
            return false;
        } else if (*s == '\\') {
            *s = '/';
        }

        ++s;
    }

    return true;
}

/**
 * \brief Parse a zip pack file.
 * \param[in] packfile Name of packfile to parse.
 * \return NULL on error, otherwise a link list of pack files.
 */
PUBLIC pack_t *FS_LoadZipFile (const char *packfile)
{
    W32 ziphead;
    char tempfilename[ MAX_GAMEPATH ];
    localzipheader_t zlocalhead;
    FILE    *packhandle;
    pack_t *pack;
    W32     numberofpackedfiles = 0;
    packfile_t *pknewnode = NULL;
    packfile_t *pkrootnode = NULL;
    W16 temp16;
    W32 temp32;
    W32 ret;


    packhandle = fopen (packfile, "rb");

    if (! packhandle) {
        return NULL;
    }

    //  Scan local header information
    for (; ;) {
        ret = fread (&ziphead, 1, 4, packhandle);

        if (ret != 4) {
            Com_DPrintf ("[FS_LoadZipFile]: Read error on file (%s)\n", packfile);
            break;
        }

        // Only parse local file headers
        if (ziphead != SIG_LOCAL) {
            break;
        }

        memset (&zlocalhead, 0, sizeof (zlocalhead));

        // Read in local header
        ret = fread (&temp16, 1, 2, packhandle);
        zlocalhead.version_needed = LittleShort (temp16);

        ret += fread (&temp16, 1, 2, packhandle);
        zlocalhead.flag = LittleShort (temp16);

        ret += fread (&temp16, 1, 2, packhandle);
        zlocalhead.compression_method = LittleShort (temp16);

        ret += fread (&temp32, 1, 4, packhandle);
        zlocalhead.timedate = LittleLong (temp32);

        ret += fread (&temp32, 1, 4, packhandle);
        zlocalhead.crc32 = LittleLong (temp32);

        ret += fread (&temp32, 1, 4, packhandle);
        zlocalhead.compressed_size = LittleLong (temp32);

        ret += fread (&temp32, 1, 4, packhandle);
        zlocalhead.uncompressed_size = LittleLong (temp32);

        ret += fread (&temp16, 1, 2, packhandle);
        zlocalhead.filename_length = LittleShort (temp16);

        ret += fread (&temp16, 1, 2, packhandle);
        zlocalhead.extrafield_length = LittleShort (temp16);

        if (ret != LOCALHEAD_SIZE) {
            Com_DPrintf ("[FS_LoadZipFile]: Read error on local header in file (%s)\n", packfile);
            break;
        }

        // This maybe a directory listing
        if (zlocalhead.compressed_size == 0) {
            // seek past filename + extra field
            ret = fseek (packhandle,
                         zlocalhead.filename_length + zlocalhead.extrafield_length,
                         SEEK_CUR);

            if (ret != 0) {
                Com_DPrintf ("[FS_LoadZipFile]: Seek error in file (%s)\n", packfile);
                break;
            }

            continue;
        }

        if (zlocalhead.compression_method != CM_NO_COMPRESSION &&
                zlocalhead.compression_method != CM_DEFLATED) {
            Com_DPrintf ("[FS_LoadZipFile] Compression Level not supported\n");

            // seek past filename + extra field + file data
            ret = fseek (packhandle,
                         zlocalhead.filename_length + zlocalhead.extrafield_length + zlocalhead.compressed_size,
                         SEEK_CUR);

            if (ret != 0) {
                Com_DPrintf ("[FS_LoadZipFile]: Seek error in file (%s)\n", packfile);
                break;
            }

            continue;
        }

        if (zlocalhead.filename_length >= sizeof (tempfilename) || zlocalhead.filename_length == 0) {
            Com_DPrintf ("[FS_LoadZipFile]: Invalid file name length\n");

            // seek past filename + extra field + file data
            ret = fseek (packhandle,
                         zlocalhead.filename_length + zlocalhead.extrafield_length + zlocalhead.compressed_size,
                         SEEK_CUR);

            if (ret != 0) {
                Com_DPrintf ("[FS_LoadZipFile]: Seek error in file (%s)\n", packfile);
                break;
            }

            continue;
        }

        // Allocate new packfile node
        pknewnode = (packfile_t *)malloc (sizeof (packfile_t));

        // Read in file name
        ret = fread (&tempfilename, 1, zlocalhead.filename_length, packhandle);

        if (ret != zlocalhead.filename_length) {
            Com_DPrintf ("[FS_LoadZipFile]: Read error on file name in file (%s)\n", packfile);
            break;
        }

        tempfilename[ zlocalhead.filename_length ] = '\0'; // NUL-terminate string

        com_strlcpy (pknewnode->name, tempfilename, sizeof (pknewnode->name));

        pknewnode->hashid = com_strhash (pknewnode->name);

        pknewnode->compression_method = zlocalhead.compression_method;
        pknewnode->uncompressed_length = zlocalhead.uncompressed_size;
        pknewnode->filelength = zlocalhead.compressed_size;

        // seek past extra field
        ret = fseek (packhandle, zlocalhead.extrafield_length, SEEK_CUR);

        if (ret != 0) {
            Com_DPrintf ("[FS_LoadZipFile]: Seek error in file (%s)\n", packfile);
            break;
        }

        // Get file data offset
        pknewnode->fileoffset = ftell (packhandle);


        // Link into file list
        pknewnode->next = pkrootnode;
        pkrootnode = pknewnode;


        // seek past file data to next local file header
        ret = fseek (packhandle, zlocalhead.extrafield_length + zlocalhead.compressed_size, SEEK_CUR);

        if (ret != 0) {
            Com_DPrintf ("[FS_LoadZipFile]: Seek error in file (%s)\n", packfile);
            break;
        }


        ++numberofpackedfiles;
    }

    if (numberofpackedfiles == 0) {
        return NULL;
    }


    pack = (pack_t *)malloc (sizeof (pack_t));
    com_strlcpy (pack->filename, packfile, sizeof (pack->filename));
    pack->handle = packhandle;
    pack->numfiles = numberofpackedfiles;
    pack->files = pkrootnode;

    return pack;
}
