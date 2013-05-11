/*

	Copyright (C) 2004-2005 Michael Liebscher <johnnycanuck@users.sourceforge.net>

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
 *	main.c:   Extract Wolfenstein 3-D palette from gamepal.obj
 *
 *	Author:  Michael Liebscher <johnnycanuck@users.sourceforge.net>
 *
 */

/*
	To convert into Spear of Destiny palette change the following:

	45, 8, 63	becomes		0, 14, 0
	42, 0, 63	becomes		0, 10, 0

*/

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>


#define INPUT_FNAME     "gamepal.obj"
#define OUTPUT_FNAME    "wolf_pal.c"

// Output format
#define VAR_DEC_S   "\nchar gamepal[ ] = {\n"
#define VAR_DEC_B   "\t%d, %d, %d,\n"
#define VAR_DEC_E   "\n};\n"

// 768 = 3 * 256 (rgb multiplied by the number of entries)
#define PAL_SIZE    768


#ifdef _WIN32

#define vsnprintf	_vsnprintf

#define snprintf	_vsprintf

#endif

/*
-----------------------------------------------------------------------------
 Function: LogErrorDestroyFile -Display error message then delete file.

 Parameters: filename -[in] Pointer to a null-terminated string with the
                        name of the file to delete.
            format -[in] Pointer to a null-terminated string with the error
                        message.
            [argument] -[in] Optional arguments.

 Returns: Nothing.

 Notes:
 			1.  Format message then display it to user.
			2.  Delete file.

-----------------------------------------------------------------------------
*/
void PrintErrorDestroyFile( const char *filename, const char *format, ... )
{
    va_list	argptr;
	char	msg[ 128 ];

	if( ! filename || ! *filename || ! format || ! *format )
	{
	   printf( "NULL arguments passed into [LogErrorDestroyFile]\n" );
	   return;
    }

    va_start( argptr, format );
	vsnprintf( msg, sizeof( msg ), format, argptr );
	va_end( argptr );

	msg[ sizeof( msg ) - 1 ] = '\0';

    printf( msg );

    // Delete file
    if( unlink( filename ) == -1 )
    {
        snprintf( msg, sizeof( msg ),
                      "[Error]: Could not remove file '%s'\n", filename );
        printf( msg );
    }
}


/*
-----------------------------------------------------------------------------
 Function: main -Extract Wolf3D Palette from gamepal.obj

 Parameters: Nothing.

 Returns: 0 on success, otherwise nonzero.

 Notes: This is the application entry point.

 			1.  Open input file, read in palette, close file.
			2.  Open output file, write c formatted palette, close file.

-----------------------------------------------------------------------------
*/
int main( int argc, char *argv[] )
{
    int i;
    long count; // Counts number of bytes read and written.
    char buffer[ 128 ];
    unsigned char gamepal[ PAL_SIZE ]; // Buffer to hold palette data.
    FILE *filestream = fopen( INPUT_FNAME, "rb" );

    if( filestream == NULL )
	{
	    printf( "[Error]: Could not open file '%s' for read!\n", INPUT_FNAME );

        return 1;
	}

    // Seek past header info
    if( fseek( filestream, 0x77, SEEK_SET ) )
    {
        fclose( filestream );

        printf( "[Error]: Could not seek past header data!\n" );

        return 2;
    }

    // Read in palette data
    count = fread( gamepal, sizeof( char ), PAL_SIZE, filestream );
    if( count != PAL_SIZE )
    {
		fclose( filestream );

        printf( "[Error]: Read error on file '%s'\n", INPUT_FNAME );

        return 3;
    }

    if( fclose( filestream ) )
    {
        printf( "[Error]: The filestream was not closed\n" );
    }

//
// Output Wolf3D Palette into file OUTPUT_FNAME
//

    filestream = fopen( OUTPUT_FNAME, "w" );
    if( filestream == NULL )
    {
        printf( "[Error]: Could not create output file '%s'\n", OUTPUT_FNAME );

        return 4;
    }

    // Write variable declaration
    count = fwrite( VAR_DEC_S, sizeof( char ),
                    strlen( VAR_DEC_S ), filestream );

    if( count != strlen( VAR_DEC_S ) )
    {
        fclose( filestream );

        PrintErrorDestroyFile( OUTPUT_FNAME,
                             "[Error]: Could not write to output file: '%s'",
                             OUTPUT_FNAME );

        return 5;
    }

    // Write out rgb values
    for( i = 0; i < PAL_SIZE; i += 3 )
    {
        snprintf( buffer, sizeof( buffer ), VAR_DEC_B,
                    gamepal[ i ], gamepal[ i+1 ], gamepal[ i+2 ]  );
        count = fwrite( buffer, sizeof( char ), strlen( buffer ), filestream );

        if( count != strlen( buffer ) )
        {
            fclose( filestream );

            PrintErrorDestroyFile( OUTPUT_FNAME,
                             "[Error]: Could not write to output file: '%s'",
                             OUTPUT_FNAME );

            return 5;
        }
    }

    // Seek back over the last lines: ",\n"
    if( fseek( filestream, -3, SEEK_CUR ) )
    {
        fclose( filestream );

        PrintErrorDestroyFile( OUTPUT_FNAME,
                         "[Error]: Could not seek back on output file '%s'\n",
                         OUTPUT_FNAME );

        return 6;
    }

    count = fwrite( VAR_DEC_E, sizeof( char ),
                    strlen( VAR_DEC_E ), filestream );

    if( count != strlen( VAR_DEC_E ) )
    {
        fclose( filestream );

        PrintErrorDestroyFile( OUTPUT_FNAME,
                             "[Error]: Could not write to output file: '%s'",
                             OUTPUT_FNAME );

        return 5;
    }

    if( fclose( filestream ) )
    {
        printf( "[Error]: The filestream was not closed\n" );
    }


    return 0;
}
