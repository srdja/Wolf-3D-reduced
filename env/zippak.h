/*

    Copyright (C) 2004 Michael Liebscher <johnnycanuck@users.sourceforge.net>

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
 * \file zippak.h
 * \brief Parse zip file
 * \author Michael Liebscher
 * \date 2004
 */


#ifndef __ZIPPAK_H__
#define __ZIPPAK_H__

#include "platform.h"
#include "filesystem.h"

/* Compression Method */
#define CM_NO_COMPRESSION       0
#define CM_SHRUNK               1
#define CM_REDUCED_F1           2
#define CM_REDUCED_F2           3
#define CM_REDUCED_F3           4
#define CM_REDUCED_F4           5
#define CM_IMPLODED             6
#define CM_RESERVED_7           7
#define CM_DEFLATED             8
#define CM_DEFLATE64            9
#define CM_IMPLODING            10
#define CM_RESERVED_11          11
#define CM_BZIP2                12


typedef struct packfile_s {
    char    name[ MAX_GAMEPATH ];   /* file name */
    W32     compression_method;     /* compression method */
    W32     uncompressed_length;
    W32     hashid;                 /* hash on file name */
    W32     filelength;             /* file length */
    W32     fileoffset;             /* data offset in pack file from start */

    struct packfile_s *next;        /* Pointer to next pack file node */

} packfile_t;

typedef struct pack_s {
    char    filename[ MAX_OSPATH ];     /* pack file name */
    FILE    *handle;                    /* handle to pack file */
    W32     numfiles;                   /* number of files in pack file */

    packfile_t  *files;                 /* Link list of files in pack */

} pack_t;


pack_t *FS_LoadZipFile (const char *packfile);

#endif /* __ZIPPAK_H__ */
