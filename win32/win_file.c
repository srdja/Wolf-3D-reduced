/*

	Copyright (C) 2004 Michael Liebscher

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
 * \file win_file.c
 * \brief Handles non-portable file services.
 * \author Michael Liebscher
 * \date 2004-2012
 */


#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>
#include <shlobj.h>
#include <winbase.h>

#include "../env/common.h"
#include "../env/com_string.h"
#include "../env/platform.h"
#include "../env/common_utils.h"

#include "../env/filesystem.h"



PRIVATE char    findbase[ MAX_PATH ];
PRIVATE char    findpath[ MAX_PATH ];
PRIVATE HANDLE  FindHandle;
    
    
/**
 * \brief Creates a new directory. 
 * \param[in] Pointer to a NUL-terminated string that specifies the path of the directory to be created. 
 * \return On success nonzero, otherwise zero. 
 */
PUBLIC W8 FS_CreateDirectory( const char *dirname )
{    
	BOOL ret_val = CreateDirectory( dirname, NULL );

	if( ret_val == 0 && GetLastError() != ERROR_ALREADY_EXISTS )	
	{		
		return 1;
	}

    return (W8)ret_val;
}

/**
 * \brief Changes the current directory  
 * \param[in] path Pointer to a NUL-terminated string that specifies the path to the new directory. 
 * \return On success nonzero, otherwise zero.
 */
PUBLIC W8 FS_ChangeCurrentDirectory( const char *path )
{
	return SetCurrentDirectory( path );
}

/**
 * \brief Compare directory and file attributes.  
 * \param[in] found Specifies the file attributes of the file found. 
 * \param[in] musthave File or directory must have these attributes.
 * \param[in] canthave File or directory can not have these attributes.
 * \return On success true, otherwise false.
 */
PRIVATE _boolean CompareAttributes( W32 found, W32 musthave, W32 canthave )
{
	if( ( found & FILE_ATTRIBUTE_READONLY ) && ( canthave & FA_RDONLY ) )
		return false;

	if( ( found & FILE_ATTRIBUTE_HIDDEN ) && ( canthave & FA_HIDDEN ) )
		return false;

	if( ( found & FILE_ATTRIBUTE_SYSTEM ) && ( canthave & FA_SYSTEM ) )
		return false;

	if( ( found & FILE_ATTRIBUTE_DIRECTORY ) && ( canthave & FA_DIR ) )
		return false;

	if( ( found & FILE_ATTRIBUTE_ARCHIVE ) && ( canthave & FA_ARCH ) )
		return false;



	if( ( musthave & FA_RDONLY ) && !( found & FILE_ATTRIBUTE_READONLY ) )
		return false;

	if( ( musthave & FA_HIDDEN ) && !( found & FILE_ATTRIBUTE_HIDDEN ) )
		return false;

	if( ( musthave & FA_SYSTEM ) && !( found & FILE_ATTRIBUTE_SYSTEM ) )
		return false;

	if( ( musthave & FA_DIR ) && !( found & FILE_ATTRIBUTE_DIRECTORY ) )
		return false;

	if( ( musthave & FA_ARCH ) && !( found & FILE_ATTRIBUTE_ARCHIVE ) )
		return false;

	return true;
}

/**
 * \brief Searches a directory for a file.   
 * \param[in] path Pointer to a NUL-terminated string that specifies a valid directory or path and file name. 
 * \param[in] musthave File or directory must have these attributes.
 * \param[in] canthave File or directory can not have these attributes.
 * \return On success string of file name or directory, otherwise NULL.
 */
PUBLIC char *FS_FindFirst( const char *path, W32 musthave, W32 canthave )
{
    WIN32_FIND_DATA FindFileData;

	if( FindHandle )
	{
		Com_DPrintf( "FS_FindFirst without close\n" );

		return NULL;
	}
  
	FS_getPath( path, findbase, 1024 );

    FindHandle = FindFirstFile( path, &FindFileData );
    
	if( FindHandle == INVALID_HANDLE_VALUE )
	{
        Com_DPrintf ( "FindFirstFile failed (%d)\n", GetLastError() );
        
		return NULL;
	}
    
	if( CompareAttributes( FindFileData.dwFileAttributes, musthave, canthave ) )
	{
		if( ! *findbase )
		{
			com_strlcpy( findpath, FindFileData.cFileName, sizeof( findpath ) );
		}
		else
		{
			com_snprintf( findpath, sizeof( findpath ), "%s%c%s", findbase, PATH_SEP, FindFileData.cFileName );
		}

		return findpath;
	}
		
   
    return FS_FindNext( musthave, canthave );
}

/**
 * \brief Continues a file search from a previous call to the FS_FindFirst function.
 * \param[in] musthave File or directory must have these attributes. 
 * \param[in] canthave File or directory can not have these attributes.
 * \return On success string of file name or directory, otherwise NULL.
 */
PUBLIC char *FS_FindNext( W32 musthave, W32 canthave )
{
	WIN32_FIND_DATA FindFileData;

	if( FindHandle == INVALID_HANDLE_VALUE )
	{
		return NULL;
	}

	while( 1 )
	{
		if( FindNextFile( FindHandle, &FindFileData ) == 0 )
		{
			return NULL;
		}

		if( CompareAttributes( FindFileData.dwFileAttributes, musthave, canthave ) )
		{
			break;
		}
	}

	if( ! *findbase )
	{
		com_snprintf( findpath, sizeof( findpath ), "%s", FindFileData.cFileName );
	}
	else
	{
		com_snprintf( findpath, sizeof( findpath ), "%s%c%s", findbase, PATH_SEP, FindFileData.cFileName );
	}
	
	return findpath;
}

/**
 * \brief Closes the search handle.
 */
PUBLIC void FS_FindClose( void )
{
	if( FindHandle != INVALID_HANDLE_VALUE )
	{
		FindClose( FindHandle );
    }
    
	FindHandle = 0;
}

/**
 * \brief Deletes an existing file.
 * \param[in] filename Pointer to a NUL-terminated string that specifies the file to be deleted.
 * \return If successful the return value is nonzero, otherwise zero.
 */
PUBLIC _boolean FS_DeleteFile( const char *filename )
{
	return DeleteFile( filename );	
}

/**
 * \brief Deletes an existing empty directory.
 * \param[in] pathname Pointer to a NUL-terminated string that specifies the directory to be deleted.
 * \return If successful the return value is nonzero, otherwise zero.
 */
PUBLIC _boolean FS_RemoveDirectory( const char *pathname )
{
	return RemoveDirectory( pathname );	
}

/**
 * \brief Retrieves current user directory
 * \return 
 */
PUBLIC char *FS_Userdir()
{
	static char fs_userdir[MAX_PATH];
	char homepath[MAX_PATH];
			
	com_snprintf( fs_userdir, MAX_PATH, "%s%c%s", FS_Gamedir(), PATH_SEP, "save" );
	
	fs_userdir[MAX_PATH - 1] = '\0';

	com_snprintf( homepath, MAX_PATH, "%s%s", fs_userdir, "\\" );
	FS_CreateDirectory( homepath );

	return fs_userdir;

}