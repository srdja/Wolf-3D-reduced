/*

	Copyright (C) 2005-2009 Michael Liebscher <johnnycanuck@users.sourceforge.net>

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
 * \file script.c
 * \brief Core script parser.
 * \author  Michael Liebscher
 * \date 2006-2009
 */

#include <string.h>
#include <math.h>

#include "script.h"
#include "com_string.h"



PRIVATE W32 LineNumber;

PRIVATE int zmem_tag;


PUBLIC W8 *script_ReadString( W8 *ptr, W8 **out )
{
	W8 *start;
	W8 *buffer;
	W32 size;

	start = ptr;


	while( *ptr != ENDOFSTREAM && ISALPHANUMERIC( *ptr ) )
	{
		ptr++;
	}

	size = ptr - start;

	if( ! size )
	{
		*out = NULL;

		return (ptr);
	}

	buffer = (PW8) Z_TagMalloc( size + 1, zmem_tag );

	memcpy( buffer, start, size );
	buffer[ size ] = '\0';

	*out = buffer;

	return (ptr);
}


PUBLIC W8 *script_ReadQuoteString( W8 *ptr, W8 **out )
{
	W8 *start;
	W8 *buffer;
	W32 size;

	if( *ptr != '"' )
	{
		*out = NULL;

		return (ptr);
	}

	ptr++;

	start = ptr;

	while( *ptr != ENDOFSTREAM && *ptr != '"' )
	{
		ptr++;
	}

	size = ptr - start;

	if( ! size )
	{
		*out = NULL;

		return (ptr);
	}

	buffer = (PW8) Z_TagMalloc( size + 1, zmem_tag );

	memcpy( buffer, start, size );
	buffer[ size ] = '\0';

	*out = buffer;

	if( *ptr == '"' )
	{
		ptr++;
	}

	return (ptr);
}


PRIVATE W8 *readInteger( W8 *in, SW32 *number, W32 *error )
{
	W8 *ptr = in;
	SW32 num = 0;
	_boolean bError = false;
	_boolean bNegative = false;


	if( *ptr == '-' )
	{
		bNegative = true;

		ptr++;
	}

	while( *ptr != ENDOFSTREAM && ( ISNUMERIC( *ptr ) || *ptr == '.' ) )
	{
		if( *ptr == '.' )
		{
			break;
		}

		num = (num * 10) + *ptr - '0';

		ptr++;
	}

	if( bNegative )
	{
		num = -num;
	}

	if( bError )
	{
		*number = 0;
		*error = 0;

		return (ptr);
	}

	*number = num;
	*error = 0;

	return (ptr);
}


PRIVATE W8 *readFloat( W8 *in, double *number, W32 *error )
{
	W8 *ptr = in;
	double num = 0;
	SW32 exp = 0;
	_boolean bError = false;
	_boolean bNegative = false;


	if( *ptr == '-' )
	{
		bNegative = true;

		ptr++;
	}
	else if( *ptr == '+' )
	{
		ptr++;
	}

	while( *ptr != ENDOFSTREAM && ISNUMERIC( *ptr ) )
	{
		num = (num * 10) + (double)(*ptr - '0');

		ptr++;
	}


	if( *ptr == '.' )
	{
		ptr++;

		while( *ptr != ENDOFSTREAM && ISNUMERIC( *ptr ) )
		{
			num = (num * 10) + (double)(*ptr - '0');
			exp--;

			ptr++;
		}
	}

	if( *ptr == 'e' || *ptr == 'E' )
	{
		SW32 expnumber;
		W32 errornum;
		ptr++;

		ptr = readInteger( ptr, &expnumber, &errornum );

		exp += expnumber;
	}

	if( bNegative )
	{
		num = -num;
	}

	if( bError )
	{
		*number = 0;
		*error = 0;

		return (ptr);
	}

	*number = num * pow( 10, exp );
	*error = 0;

	return (ptr);
}

PUBLIC W8 *script_ReadNumber( W8 *ptr, int numerictype, decimalType_t *nt )
{
	W8 *start;
	W8 *end;
	W8 *buffer;
	W32 size;
	W32 error;
	_boolean bDecimal = false;
	_boolean bNegative = false;


	ptr = script_ignoreWhiteSpace( ptr );



	start = ptr;

	while( *ptr != ENDOFSTREAM && ( ISNUMERIC( *ptr ) || *ptr == '.' ) )
	{
		if( *ptr == '.' )
		{
			bDecimal = true;
			end = ptr;
		}
		ptr++;
	}

	if( ptr == start )
	{
		memset( nt, 0, sizeof( decimalType_t ) );

		return (ptr);
	}


	if( bDecimal && ! numerictype )
	{
		size = end - start;
	}
	else
	{
		size = ptr - start;
	}

	if( size == 0 )
	{
		memset( nt, 0, sizeof( decimalType_t ) );

		return ptr;
	}

	buffer = (PW8) Z_TagMalloc( size + 1, zmem_tag );
	memcpy( buffer, start, size );
	buffer[ size ] = '\0';

	if( numerictype == TT_FLOAT )
	{
		nt->Float = StringToFloat( buffer, &error );

		if( bNegative )
		{
			nt->Float = -nt->Float;
		}
	}
	else if( numerictype == TT_INTEGER )
	{
		nt->Integer = StringToInteger( buffer, &error );

		if( bNegative )
		{
			nt->Integer = -nt->Integer;
		}
	}
	else
	{
		Com_Printf( "Unknown numeric type passed into [script_ReadNumber]\n" );

		memset( nt, 0, sizeof( decimalType_t ) );

		return (ptr);
	}



	return (ptr);
}

PUBLIC W8 *script_lookforCharacter( W8 *ptr, char character, _boolean bNewline )
{
	while( *ptr != ENDOFSTREAM && *ptr != character )
	{
		if( *ptr == '\n' )
		{
			if( bNewline )
			{
				return (ptr);
			}

			LineNumber++;
		}
		ptr++;
	}

	return (ptr);
}

PUBLIC W8 *script_ignoreTag( W8 *ptr )
{
	while( *ptr != ENDOFSTREAM && *ptr != '}' )
	{
		if( *ptr == '\n' )
		{
			LineNumber++;
		}
		ptr++;
	}

	if( *ptr == '}' )
	{
		ptr++;
	}

	return (ptr);
}

PUBLIC W8 *script_ignoreString( W8 *ptr )
{
	while( *ptr != ENDOFSTREAM &&
			(*ptr >= '!' && *ptr <= '~') &&
			*ptr != '}' )
	{
		ptr++;
	}

	return (ptr);
}



PUBLIC W8 *script_ignoreWhiteSpace( W8 *ptr )
{

	while( 1 )
	{
		while( ISSPACE( *ptr ) )
		{
			if( *ptr == '\n' )
			{
				LineNumber++;
			}
			ptr++;
		}

		// ignore comments
		if( ptr[ 0 ] == '/' && ptr[ 1 ] == '*' )
		{
			// look for the end of the comment block
			ptr = &ptr[ 2 ];
			while( ptr[ 0 ] != ENDOFSTREAM && (ptr[ 0 ] != '*' || ptr[ 1 ] != '/') )
			{
				ptr++;
			}

			if( ptr[ 0 ] != ENDOFSTREAM )
			{
				ptr = &ptr[ 2 ];
			}

			continue;
		}

		if( ptr[ 0 ] == '/' && ptr[ 1 ] == '/' )
		{
			ptr = script_lookforCharacter( ptr, '\n', 0 );

			if( *ptr == '\n' )
			{
				LineNumber++;
				ptr++;
			}

			continue;
		}

		break;
	}

	return (ptr);
}


PUBLIC _boolean script_Parse( const char *filename, Tag_Property_t *tagProperties, W32 tagProperties_size, int tag )
{
	W8 *buffer;
	W8 *ptr;
	SW32 filesize;
	SW32 read;
	W32 i, lg;
	filehandle_t *fp;

	fp = FS_OpenFile( filename, 0 );
	if( ! fp )
	{
		return false;
	}

	filesize = FS_GetFileSize( fp );

	buffer = (PW8) MM_MALLOC( filesize + 1 );

	read = FS_ReadFile( buffer, 1, filesize, fp );
	if( read != filesize )
	{
		MM_FREE( buffer );
		FS_CloseFile( fp );

		return false;
	}

	FS_CloseFile( fp );



	zmem_tag = tag;

	LineNumber = 1;

	buffer[ filesize ] = ENDOFSTREAM;
	ptr = buffer;

	while( *ptr != ENDOFSTREAM )
	{
		ptr = script_ignoreWhiteSpace( ptr );

		for( i = 0 ; i < tagProperties_size ; ++i )
		{
			lg = strlen( tagProperties[ i ].name );
			if( ! strncmp( ptr, tagProperties[ i ].name, lg ) )
			{
				ptr += lg;

				ptr = tagProperties[ i ].parsing_function( ptr, &LineNumber );

				break;
			}
		}


		if( i == tagProperties_size )
		{
			ptr = script_ignoreTag( ptr );
		}

	}

	MM_FREE( buffer );

	return true;
}
