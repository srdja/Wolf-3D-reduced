/*

    Copyright (C) 2004-2005 Michael Liebscher <johnnycanuck@users.sourceforge.net>
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

/*
 *  filesystem.h:  Interface to filesystem layer.
 *
 *  Author: Michael Liebscher   <johnnycanuck@users.sourceforge.net>
 *
 *  Acknowledgement:
 *  This code was derived from Quake II, and was originally
 *  written by Id Software, Inc.
 *
 */

/*
    Notes:
    This module is implemented by files.c.

*/

#ifndef __FILESYSTEM_H__
#define __FILESYSTEM_H__

#include <stdio.h>

#include "platform.h"
#include "filestring.h"

#define MAX_GAMEPATH    64  // max length of a game pathname
#define MAX_OSPATH  128 // max length of a filesystem pathname


void    FS_InitFilesystem (void);
void    FS_SetGamedir (char *dir);
char    *FS_Gamedir (void);
char    *FS_Userdir (void);
char    *FS_NextPath (char *prevpath);

/////////////////////////////////////////////////////////////////////
//
//  PORTABLE FILE SYSTEM SERVICES
//
/////////////////////////////////////////////////////////////////////

typedef struct {
    FILE *hFile;

    /* Following is used when the file is loaded into memory */
    _boolean bLoaded;           /* Was file loaded into memory? */
    W32 filesize;           /* Size of file data in bytes */

    W8  *ptrStart;          /* pointer to start of file data block */
    W8  *ptrCurrent;            /* pointer to current position in file data block */
    W8  *ptrEnd;                /* pointer to end of file data block */

    void *filedata;             /* file data loaded into memory */

} filehandle_t;


/* Desired Access Flags */
#define DA_GENERIC_READ         0x01

/* Flags and Attributes */
#define FA_FILE_FLAG_LOAD       0x01

filehandle_t *FS_OpenFile (const char *filename, W32 FlagsAndAttributes);
void FS_CloseFile (filehandle_t *fhandle);
// note: this can't be called from another DLL, due to MS libc issues

SW32 FS_ReadFile (void *buffer, W32 size, W32 count, filehandle_t *fhandle);

W32 FS_FileSeek (filehandle_t *fhandle, SW32 offset, W32 origin);

SW32 FS_GetFileSize (filehandle_t *fhandle);
/////////////////////////////////////////////////////////////////////
//
//  NON-PORTABLE FILE SYSTEM SERVICES
//
/////////////////////////////////////////////////////////////////////

W8 FS_CreateDirectory (const char *dirname);

#define FA_HIDDEN   0x02
#define FA_DIR      0x08
#define FA_SYSTEM   0x10

// pass in an attribute mask of things you wish to REJECT
char *FS_FindFirst (const char *path, W32 musthave, W32 canthave);
char *FS_FindNext (W32 musthave, W32 canthave);
void FS_FindClose (void);


#endif /* __FILESYSTEM_H__ */

