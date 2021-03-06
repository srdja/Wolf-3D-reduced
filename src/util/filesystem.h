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

#include "filestring.h"

#define MAX_GAMEPATH 2048  // max length of a game pathname
#define MAX_OSPATH   2048 // max length of a filesystem pathname

const char *get_resource_base_path(void);
char       *FS_Userdir (void);

/////////////////////////////////////////////////////////////////////
//
//  PORTABLE FILE SYSTEM SERVICES
//
/////////////////////////////////////////////////////////////////////

typedef struct {
    FILE *hFile;

    /* Following is used when the file is loaded into memory */
    bool bLoaded;           /* Was file loaded into memory? */
    uint32_t filesize;           /* Size of file data in bytes */

    uint8_t *ptrStart;          /* pointer to start of file data block */
    uint8_t *ptrCurrent;            /* pointer to current position in file data block */
    uint8_t *ptrEnd;                /* pointer to end of file data block */

    void *filedata;             /* file data loaded into memory */

} filehandle_t;

filehandle_t *FS_OpenFile (const char *filename);
void FS_CloseFile (filehandle_t *fhandle);
// note: this can't be called from another DLL, due to MS libc issues

int32_t FS_ReadFile (void *buffer, uint32_t size, uint32_t count, filehandle_t *fhandle);
uint32_t FS_FileSeek (filehandle_t *fhandle, int32_t offset, uint32_t origin);
int32_t FS_GetFileSize (filehandle_t *fhandle);
/////////////////////////////////////////////////////////////////////
//
//  NON-PORTABLE FILE SYSTEM SERVICES
//
/////////////////////////////////////////////////////////////////////

uint8_t FS_CreateDirectory (const char *dirname);
#define FA_DIR      0x08

// pass in an attribute mask of things you wish to REJECT
char *FS_FindFirst (const char *path, uint32_t musthave, uint32_t canthave);
void FS_FindClose (void);


#endif /* __FILESYSTEM_H__ */

