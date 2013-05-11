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
 * \file zmem.c
 * \brief Heap memory management.
 * \author Michael Liebscher 
 * \author Id Software, Inc.
 * \date 1997-2001
 * \note This code was derived from Quake II, and was originally written by Id Software, Inc.
 * \note Add the following line in your initization function: z_chain.next = z_chain.prev = &z_chain;
 */


#include <string.h>

#include "common.h"
#include "platform.h"
#include "common_utils.h"
#include "memory.h"

#include "zmem.h"



//  just cleared malloc with counters now...


#define	Z_MAGIC		0x1d1d


PRIVATE int		z_count, z_bytes;

zhead_t	z_chain;


/**
 * \brief Deallocates or frees a zone memory block.
 * \param[in] memblock Previously allocated zone memory block to be freed.  
 */
PUBLIC void Z_Free( void *memblock )
{
	zhead_t	*z;

	z = ( (zhead_t *)memblock ) - 1;

	if( z->magic != Z_MAGIC )
	{
		Com_Error( ERR_FATAL, "Z_Free: bad magic" );
	}

	z->prev->next = z->next;
	z->next->prev = z->prev;

	z_count--;
	z_bytes -= z->size;

	MM_FREE( z );
}

/**
 * \brief Console function to list zone memory usage.
 * \note Lists number of bytes and blocks of zone memory allocated.
 */
PUBLIC void Z_Stats_f( void )
{
	Com_Printf( "%i bytes in %i blocks\n", z_bytes, z_count );
}

/**
 * \brief Free allocated zone memory blocks based on tag.
 * \param[in] tag Tag of zone memory blocks to free (see header for tag).
 */
PUBLIC void Z_FreeTags( int tag )
{
	zhead_t	*z, *next;

	for( z = z_chain.next; z != &z_chain; z = next )
	{
		next = z->next;
		if( z->tag == tag )
		{
			Z_Free( (void *)(z+1) );
		}
	}
}

/**
 * \brief Allocates zone memory blocks.
 * \param[in] size Bytes to allocate. 
 * \param[in] tag Tag to associate with memory (see header for tag).
 * \return A void pointer to the allocated space, or application will shutdown if there is insufficient memory available.
 */
PUBLIC void *Z_TagMalloc( size_t size, int tag )
{
	zhead_t	*z;

	// Allocate memory
	size += sizeof( zhead_t );
	z = (zhead_t *)MM_MALLOC( size );
	
	
	if( ! z )
	{
		Com_Error( ERR_FATAL, "Z_Malloc: failed on allocation of %i bytes", size );

		return NULL;
	}

	// Set memory block to zero and fill in header.
	memset( z, 0, size );
	z_count++;
	z_bytes += size;
	z->magic = Z_MAGIC;
	z->tag = tag;
	z->size = size;

	// Add new memory block to chain.
	z->next = z_chain.next;
	z->prev = &z_chain;
	z_chain.next->prev = z;
	z_chain.next = z;

	return (void *)(z+1);
}

/**
 * \brief Allocates zone memory blocks.
 * \param[in] size Bytes to allocate.  
 * \return A void pointer to the allocated space, or will shutdown application if there is insufficient memory available.
 * \note Calls Z_TagMalloc() with tag set to zero.
 */
PUBLIC void *Z_Malloc( size_t size )
{
	return Z_TagMalloc( size, 0 );
}
