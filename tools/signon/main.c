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
 *	main.c:   Extract Wolfenstein 3-D sign on image from signon.obj
 *
 *	Author:  Michael Liebscher    <johnnycanuck@users.sourceforge.net>
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <memory.h>


#include "tga.h"



#define INPUT_FNAME     "signon.obj"
#define OUTPUT_FNAME    "signon.tga"

#define IMAGE_WIDTH   320
#define IMAGE_HEIGHT  200
#define BYTESPERPIXEL   3
#define IMAGE_SIZE  (IMAGE_WIDTH * IMAGE_HEIGHT * BYTESPERPIXEL)


extern char gamepal[];	// Wolf3D palette


/*
-----------------------------------------------------------------------------
 Function: main -Extract Wolf3D sign on image from signon.obj

 Parameters: Nothing.

 Returns: 0 on success, otherwise nonzero.

 Notes: This is the application entry point.

 			1.  Open input file, read in data, close file.
			2.  Write targa file.

-----------------------------------------------------------------------------
*/
int main( int argc, char *argv[] )
{
    unsigned int i, temp, index;
    unsigned long count;
    unsigned char *buffer; // tga file buffer.
    unsigned char signon[ 64512 ]; // Buffer to hold raw sign on screen data.

    FILE *filestream = fopen( INPUT_FNAME, "rb" );

    if( filestream == NULL )
	{
		printf( "Could not open file \"signon.obj\" for read!\n" );

		return 1;
	}

    // Seek past header info
    if( fseek( filestream, 0x79, SEEK_SET ) )
    {
        if( fclose( filestream ) )
        {
            printf( "[Error]: The filestream was not closed\n" );
        }

        printf( "[Error]: Could not seek past header data!\n" );

		return 1;
    }

    // On the last iteration, we read in the seven filler bytes,
    //  output will discard them.
    count = 0;
    for( i = 0; i < 63; ++i )
    {
        // Read in 1k of Data
        count += fread( signon + (i*1024), sizeof( char ), 1024, filestream );

        // Seek past seven bytes of filler
        fseek( filestream, 7, SEEK_CUR );
    }

    if( fclose( filestream ) )
    {
        printf( "[Error]: The filestream was not closed\n" );
    }

    if( count != 64006 )
    {
        printf( "[Error]: Could not Read from input file\n" );

		return 1;
    }

//
// Output Targa image
//

    buffer = malloc( IMAGE_SIZE );
    if( buffer == NULL )
    {
        printf( "malloc: insufficient memory available!\n" );

		return 1;
    }

    // Convert palette to rgb
	for( i = 0, index = 0; i < IMAGE_SIZE; i += BYTESPERPIXEL, ++index )
	{
        temp = signon[ index ] * 3;

        buffer[ i ]   = gamepal[ temp ] << 2;		// Red
        buffer[ i+1 ] = gamepal[ temp + 1 ] << 2;	// Green
        buffer[ i+2 ] = gamepal[ temp + 2 ] << 2;	// Blue
    }


    (void)WriteTGA( OUTPUT_FNAME, 24, IMAGE_WIDTH, IMAGE_HEIGHT, buffer, 0, 1 );

    if( buffer )
    {
        free( buffer );
        buffer = NULL;
    }

    return 0;
}
