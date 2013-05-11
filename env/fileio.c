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
#include <zlib.h>

#include "platform.h"
#include "common_utils.h"
#include "common.h"
#include "com_string.h"
#include "filesystem.h"
#include "filelink.h"
#include "zippak.h"




/**
 * \brief Get file pointer.
 * \param[in] fhandle Target file handle.
 * \param[in] origin Pointer position
 *            SEEK_SET -Beginning of file.
 *			  SEEK_CUR -Current position of file pointer.
 *			  SEEK_END -End of file.
 * \return File pointer on success, otherwise NULL.
 */
PUBLIC void *FS_GetLoadedFilePointer( filehandle_t *fhandle, W32 origin )
{
	if( !fhandle->bLoaded )
	{
		return NULL;
	}		

	switch( origin )
	{
		case SEEK_SET:
			return( (void *)fhandle->ptrStart );

		case SEEK_END:
			return( (void *)fhandle->ptrEnd );

		case SEEK_CUR:
			return( (void *)fhandle->ptrCurrent );
	}

	return NULL;
}

/**
 * \brief Get the length of a file.
 * \param[in] Target file handle.
 * \return The file length in bytes.
 */
PUBLIC SW32 FS_GetFileSize( filehandle_t *fhandle )
{
	SW32	pos;
	SW32	end;

	if( fhandle->bLoaded )
	{
		return fhandle->filesize;
	}		

	pos = ftell( fhandle->hFile );
	fseek( fhandle->hFile, 0, SEEK_END );
	end = ftell( fhandle->hFile );
	fseek( fhandle->hFile, pos, SEEK_SET );

	return end;
}

/**
 * \brief Moves the file pointer to a specified location.
 * \param[in] fhandle Pointer to filehandle_t structure.
 * \param[in] offset Number of bytes from origin
 * \param[in] origin Initial position
 *                          SEEK_SET -Beginning of file.
 *							SEEK_CUR -Current position of file pointer.
 *							SEEK_END -End of file.
 * \return If successful zero, otherwise a nonzero value.
 */
PUBLIC W32 FS_FileSeek( filehandle_t *fhandle, SW32 offset, W32 origin )
{
	if( fhandle->bLoaded )
	{
		switch( origin )
		{
			case SEEK_SET:
				if( offset < 0 ||
					offset > fhandle->filesize )
				{
					return 1;
				}
				
				fhandle->ptrCurrent = fhandle->ptrStart + offset;
				break;

			case SEEK_END:
				if( offset > 0 )
				{
					return 1;
				}

				// offset is negative 
				if( (fhandle->filesize + offset) < 0  )
				{
					return 1;
				}

				// offset is negative 
				fhandle->ptrCurrent = fhandle->ptrEnd + offset;
				break;

			case SEEK_CUR:
				if( offset < 0 )
				{
					// offset is negative
					if( ((fhandle->ptrCurrent - fhandle->ptrStart) + offset) < 0 )
					{
						return 1;
					}
				}

				if( offset > 0 )
				{
					if( offset > (fhandle->ptrEnd - fhandle->ptrCurrent) )
					{
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

	return fseek( fhandle->hFile, offset, origin );
}

/**
 * \brief Gets the current position of a file pointer.
 * \param[in] fhandle Pointer to filehandle_t structure.
 * \return If successful current file position, otherwise -1.
 */
PUBLIC SW32 FS_FileTell( filehandle_t *fhandle )
{
	if( fhandle->bLoaded )
	{
		return( fhandle->ptrCurrent - fhandle->ptrStart );
	}

	return ftell( fhandle->hFile );
}

/**
 * \brief Close file handle.
 * \param[in] filestream Pointer to valid FILE structure.	
 * \note Closes a file stream that was returned by FS_FOpenFile.
 */
PUBLIC void FS_CloseFile( filehandle_t *fhandle )
{
	if(	fhandle->hFile )
	{
		fclose( fhandle->hFile );
	}

	if( fhandle->filedata )
	{
		Z_Free( fhandle->filedata );
	}

	Z_Free( fhandle );
}

/**
 * \brief Loads a file from a pak/zip file into memory.
 * \param[in] hFile Pointer to a valid filehandle_t structure.
 * \param[in] pakfiles Pointer to a valid packfile_t structure.
 * \return true on success, otherwise false.
 */
PRIVATE _boolean LoadCompressedFile( filehandle_t *hFile, packfile_t *pakfiles )
{
	int err;
	W32 read;
	W8 *uncompr;
	W8 *buf;
	z_stream d_stream; /* decompression stream */

	buf = Z_Malloc( pakfiles->filelength + 2 );						

	// Zlib expects the 2 byte head that the inflate method adds.
	buf[ 0 ] = 120;
	buf[ 1 ] = 156;
	read = fread( buf+2, 1, pakfiles->filelength, hFile->hFile );
	if( read != pakfiles->filelength )
	{
		fclose( hFile->hFile );
		Z_Free( buf );

		return false;
	}

	fclose( hFile->hFile );
	hFile->hFile = NULL;

	uncompr = Z_Malloc( pakfiles->uncompressed_length );

	d_stream.zalloc = (alloc_func)0;
	d_stream.zfree = (free_func)0;
	d_stream.opaque = (voidpf)0;

	d_stream.next_in  = buf;
	d_stream.avail_in = (uInt)pakfiles->filelength+2;

	d_stream.next_out = uncompr; 
	d_stream.avail_out = (uInt)pakfiles->uncompressed_length;

	err = inflateInit( &d_stream );
	if( err != Z_OK )
	{
		MM_FREE( uncompr );
		Z_Free( buf );							
		FS_CloseFile( hFile );

		return false;
	}

	err = inflate( &d_stream, Z_NO_FLUSH );
	if( err != Z_OK )
	{
		Z_Free( uncompr );
		Z_Free( buf );							
		FS_CloseFile( hFile );
		
		return false;
	}
			
	err = inflateEnd( &d_stream );
	if( err != Z_OK )
	{
		Z_Free( uncompr );
		Z_Free( buf );							
		FS_CloseFile( hFile );
		
		return false;
	}
						
	Z_Free( buf );

	hFile->filedata = uncompr;
	hFile->filesize = d_stream.total_out;

	// align our file data pointers
	hFile->ptrStart =  hFile->ptrCurrent = (PW8)hFile->filedata;
	hFile->ptrEnd =  (PW8)hFile->filedata + hFile->filesize;

	hFile->bLoaded = true;

	return true;
}

/**
 * \brief Load the file into memory.
 * \param[out] Pointer to a valid filehandle_t structure.
 * \return true on success, otherwise false.
 */
PRIVATE _boolean LoadFile( filehandle_t *hFile )
{
	W32 read;

	hFile->filesize = FS_GetFileSize( hFile );
	hFile->filedata = Z_Malloc( hFile->filesize );

	read = fread( hFile->filedata, 1, hFile->filesize, hFile->hFile );
	if( read != hFile->filesize )
	{
		fclose( hFile->hFile );

		return false;
	}

	fclose( hFile->hFile );

	hFile->hFile = NULL;

	// align our file data pointers
	hFile->ptrStart =  hFile->ptrCurrent = (PW8)hFile->filedata;
	hFile->ptrEnd =  (PW8)hFile->filedata + hFile->filesize;

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
 *		Use the FS_CloseFile function to close an object handle returned
 *		by FS_OpenFile. 
 */
PUBLIC filehandle_t *FS_OpenFile( const char *filename, W32 FlagsAndAttributes )
{
	searchpath_t	*search;
	char			netpath[ MAX_OSPATH ];
	pack_t			*pak;
	packfile_t		*pakfiles;
	filelink_t		*link;
	W32				hashid;
	filehandle_t	*hFile;

	
	hFile = (filehandle_t *)Z_Malloc( sizeof( filehandle_t ) );
	memset( hFile, 0, sizeof( filehandle_t ) );

	// check for links first
	for( link = fs_links ; link ; link = link->next )
	{
		if( ! strncmp( filename, link->from, link->fromlength ) )
		{
			com_snprintf( netpath, sizeof( netpath ), "%s%s", link->to, filename + link->fromlength );
			hFile->hFile = fopen( netpath, "rb" );
			if( ! hFile->hFile )
			{		
				FS_CloseFile( hFile );

				return NULL;	
			}

			Com_DPrintf( "link file: %s\n", netpath );

			if( FlagsAndAttributes & FA_FILE_FLAG_LOAD )
			{
				if( ! LoadFile( hFile ) )
				{
					FS_CloseFile( hFile );

					return NULL;
				}
			}

			return hFile;
		}
	}


//
//	Check for the file in the directory tree
//
	com_snprintf( netpath, sizeof( netpath ), "%s/%s", FS_Gamedir(), filename );
			
	hFile->hFile = fopen( netpath, "rb" );
	if( hFile->hFile )
	{
		Com_DPrintf( "[FS_OpenFile]: %s\n", netpath );

		if( FlagsAndAttributes & FA_FILE_FLAG_LOAD )
		{
			if( ! LoadFile( hFile ) )
			{
				FS_CloseFile( hFile );

				return NULL;
			}
		}
	
		return hFile;
	}
	

//
// search through the path, one element at a time
//
	hashid = com_strhash( filename );
	for( search = fs_searchpaths ; search ; search = search->next )
	{
		// is the element a pak file?
		if( search->pack )
		{
			// look through all the pak file elements
			pak = search->pack;
			for( pakfiles = pak->files ; pakfiles ; pakfiles = pakfiles->next )
			{
				if( pakfiles->hashid == hashid )
				{	// found it!
					hFile->filesize = pakfiles->uncompressed_length;

					Com_DPrintf( "PackFile: %s : %s\n", pak->filename, filename );
				
					// open a new file handle on the pakfile
					hFile->hFile = fopen( pak->filename, "rb" );
					if( hFile->hFile == NULL )
					{
						FS_CloseFile( hFile );

						Com_Error( ERR_FATAL, "Could not reopen (%s)\n", pak->filename );	
					}

					fseek( hFile->hFile, pakfiles->fileoffset, SEEK_SET );

					if( pakfiles->compression_method )
					{
						if( ! LoadCompressedFile( hFile, pakfiles ) )
						{
							FS_CloseFile( hFile );

							return NULL;
						}	
					}
					else if( FlagsAndAttributes & FA_FILE_FLAG_LOAD )
					{
						if( ! LoadFile( hFile ) )
						{
							FS_CloseFile( hFile );

							return NULL;
						}
					}
					
					return hFile;
				}
			}
		}
		else
		{	
		// check a file in the directory tree
			
			com_snprintf( netpath, sizeof( netpath ), "%s%c%s", search->filename, PATH_SEP, filename );
			
			hFile->hFile = fopen( netpath, "rb" );
			if( ! hFile->hFile )
			{
				continue;
			}
			
			Com_DPrintf( "[FS_OpenFile]: %s\n", netpath );

			if( FlagsAndAttributes & FA_FILE_FLAG_LOAD )
			{
				if( ! LoadFile( hFile ) )
				{
					FS_CloseFile( hFile );

					return NULL;
				}
			}

			return hFile;
		}
		
	}
	
	Com_DPrintf( "[FS_OpenFile]: Could not find (%s)\n", filename );

	FS_CloseFile( hFile );
	
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
PUBLIC SW32 FS_ReadFile( void *buffer, W32 size, W32 count, filehandle_t *fhandle )
{		
	W8	*buf = (PW8)buffer;

	if( fhandle->bLoaded )
	{
		W32 i;

		if( (size * count) > (fhandle->ptrEnd - fhandle->ptrCurrent) )
		{
			SW32 read;

			read = (fhandle->ptrEnd - fhandle->ptrCurrent);

			for( i = 0 ; i < (fhandle->ptrEnd - fhandle->ptrCurrent) ; ++i )
			{
				buf[ i ] = fhandle->ptrCurrent[ i ];
			}

			fhandle->ptrCurrent = fhandle->ptrEnd;
			
			return( read );
		}
		else
		{
			for( i = 0 ; i < (size * count) ; ++i, fhandle->ptrCurrent++ )
			{
				buf[ i ] = *fhandle->ptrCurrent;
			}
		
			return( (size * count) / size );
		}
	}
	else
	{
		return fread( buf, size, count, fhandle->hFile );
	}

	/* should never get here */
	return -1;
}
