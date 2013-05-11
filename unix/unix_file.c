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
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <pwd.h>
#include <string.h>


#include "glob.h"
#include "../env/common.h"
#include "../env/com_string.h"
#include "../env/platform.h"
#include "../env/common_utils.h"

#include "../env/filesystem.h"


PRIVATE	char	findbase[ 128 ];
PRIVATE	char	findpath[ 128 ];
PRIVATE	char	findpattern[ 128 ];
PRIVATE	DIR     *fdir;


/**
 * \brief Creates a new directory.
 * \param[in] dirname Pointer to a NUL-terminated string that specifies the path of the directory to be created. 
 * \return On success nonzero, otherwise zero.
 */
PUBLIC W8 FS_CreateDirectory( const char *dirname )
{
	int ret_val = mkdir( dirname, S_IRUSR | S_IWUSR | S_IXUSR );

	if( ret_val == -1 && errno == EEXIST )
	{
		return 1;
	}
	
	return  (W8)(! ret_val);
}

/**
 * \brief Changes the current directory 
 * \param[in] path Pointer to a NUL-terminated string that specifies the path to the new directory. 
 * \return On success nonzero, otherwise zero.
 */
PUBLIC W8 FS_ChangeCurrentDirectory( const char *path )
{
	return ! chdir( path );
}

/**
 * \brief Compare directory and file attributes.  
 * \param[in] found Specifies the file attributes of the file found. 
 * \param[in] musthave File or directory must have these attributes.
 * \param[in] canthave File or directory can not have these attributes.
 * \return On success true, otherwise false.
 */
PRIVATE _boolean CompareAttributes( const char *path, W32 musthave, W32 canthave )
{
	struct stat st;

	if( stat( path, &st ) == -1 )
	{
		return false; 
	}

	if( ( st.st_mode & S_IFDIR ) && ( canthave & FA_DIR ) )
	{
		return false;
	}

	if( ( musthave & FA_DIR ) && !( st.st_mode & S_IFDIR ) )
	{
		return false;
	}

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
	struct dirent *d;

	if( fdir )
	{
		Com_DPrintf( "FS_FindFirst without close\n" );

		return NULL;
	}

	com_strlcpy( findpattern, FS_getFileName( path ), sizeof( findpattern ) );

	FS_getPath( path, findbase, 1024 );

	if( ! *findbase )
	{
		if( (fdir = opendir( "." )) == NULL )
		{
			return NULL;
		}
	}
	else
	{
		if( (fdir = opendir( findbase )) == NULL )
		{
			return NULL;
		}	
	}

	while( (d = readdir( fdir )) != NULL )
	{
		if( ! *findpattern || glob_match( findpattern, d->d_name ) ) 
		{
			if( ! *findbase )
			{
				com_strlcpy( findpath, d->d_name, sizeof( findpath ) );
			}
			else
			{
				com_snprintf( findpath, sizeof( findpath ), "%s/%s", findbase, d->d_name );
			}

			if( CompareAttributes( findpath, musthave, canthave ) )
			{				
				return findpath;
			}			
		}
	}

	return NULL;

}

/**
 * \brief Continues a file search from a previous call to the FS_FindFirst function.
 * \param[in] musthave File or directory must have these attributes. 
 * \param[in] canthave File or directory can not have these attributes.
 * \return On success string of file name or directory, otherwise NULL.
 */
PUBLIC char *FS_FindNext( W32 musthave, W32 canthave )
{
	struct dirent *d;

	if( fdir == NULL )
	{
		return NULL;
	}

	while( (d = readdir( fdir ) ) != NULL)
	{
		if( ! *findpattern || glob_match( findpattern, d->d_name ) )
		{
			if( ! *findbase )
			{
				com_strlcpy( findpath, d->d_name, sizeof( findpath ) );
			}
			else
			{
				com_snprintf( findpath, sizeof( findpath ), "%s/%s", findbase, d->d_name );
			}

			if( CompareAttributes( findpath, musthave, canthave ) )
			{
				return findpath;
			}
		}
	}

	return NULL;
}

/**
 * \brief Closes the search handle.
 */
PUBLIC void FS_FindClose( void )
{
	if( fdir )
	{
		closedir( fdir );
	}

	fdir = NULL;	
}

/**
 * \brief Deletes an existing file.
 * \param[in] filename Pointer to a NUL-terminated string that specifies the file to be deleted.
 * \return If successful the return value is nonzero, otherwise zero.
 */
PUBLIC _boolean FS_DeleteFile( const char *filename )
{
	return( ! unlink( filename ) );	
}

/**
 * \brief Deletes an existing empty directory.
 * \param[in] pathname Pointer to a NUL-terminated string that specifies the directory to be deleted.
 * \return If successful the return value is nonzero, otherwise zero.
 */
PUBLIC _boolean FS_RemoveDirectory( const char *pathname )
{
	return( ! rmdir( pathname ) );	
}

/**
 * \brief Retrieves current user directory.
 */
PUBLIC  char *FS_Userdir()
{
	static char *home = NULL;
	static char W3Dlocaldir[128] = {0};

	if ( !W3Dlocaldir[0] ){
		home = getenv("HOME");
	
		/* Mac OSX doesn't always have the HOME variable defined, so if
		   looking for $HOME fails, try with getpwuid. */
		if (!home){
			struct passwd* pw;
			pw = getpwuid( getuid() );
			home = pw->pw_dir;
		}

		com_snprintf( W3Dlocaldir, 128, "%s%c%s%c%s%c", home, PATH_SEP,
				".local", PATH_SEP, "wolf3dredux", PATH_SEP );
		W3Dlocaldir[127] = '\0';

		FS_CreateDirectory(W3Dlocaldir);
		printf("FS_UserDir = '%s'\n", W3Dlocaldir);
	}
	return W3Dlocaldir;
}
