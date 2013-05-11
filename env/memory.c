/*

	Copyright (C) 2004-2012 Michael Liebscher <johnnycanuck@users.sourceforge.net>

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
 * \file memory.c
 * \brief Memory allocation module.
 * \author Michael Liebscher
 * \date 2004-2012
 */

#include <stdlib.h>

#include "memory.h"
#include "common.h"


#ifdef DEBUG_MEMORY

	#define DEBUG_MEMORY 1

#endif


/**
 * \brief Allocates memory blocks.
 * \param[in] size Bytes to allocate.
 * \return Void pointer to the allocated space on success, or NULL if there is insufficient memory available.
 */
PUBLIC void *Memory_malloc( size_t size )
{
    void *ptr;
    ptr = malloc( size );

    if( ptr != NULL )
    {

#if DEBUG_MEMORY

		Com_DPrintf( "[Memory_malloc]: %p size:%ld\n", ptr, size );

#endif

        return ptr;
    }


	Com_DPrintf( "[Memory_malloc]: Could not allocate %d bytes\n", size );

    return NULL;
}

/**
 * \brief Allocates an array in memory with elements initialized to 0.
 * \param[in] num Number of elements.
 * \param[in] size Bytes to allocate.
 * \return Void pointer to the allocated space on success, or NULL if there is insufficient memory available.
 */
PUBLIC void *Memory_calloc( size_t num, size_t size )
{
    void *ptr;
    ptr = calloc( num, size );

    if( ptr != NULL )
    {

#if DEBUG_MEMORY

		Com_DPrintf( "[Memory_calloc]: %p size:%ld num:%ld\n", ptr, size, num );

#endif

        return ptr;
    }

	Com_DPrintf( "[Memory_calloc]: Could not allocate %d objects of size %d\n", num, size );

    return NULL;
}

/**
 * \brief Reallocate memory blocks.
 * \param[in] memblock Pointer to previously allocated memory block.
 * \param[in] size Bytes to allocate.
 * \return A void pointer to the reallocated (and possibly moved) memory
 *		block. The return value is NULL if the size is zero and the
 *		buffer argument is not NULL, or if there is not enough
 *		available memory to expand the block to the given size.
 */
PUBLIC void *Memory_realloc( void *memblock, size_t size )
{
    void *ptr;

    ptr = realloc( memblock, size );

    if( ptr != NULL )
    {

#if DEBUG_MEMORY

		Com_DPrintf( "[Memory_realloc]: %p size:%ld\n", ptr, size );

#endif

        return ptr;
    }

	Com_DPrintf( "[Memory_realloc]: Could not reallocate %d bytes\n", size );

    return NULL;
}

/**
 * \brief Deallocates or frees a memory block.
 * \param[in] memblock Previously allocated memory block to be freed.
 */
PUBLIC void Memory_free( void *memblock )
{
    if( memblock )
    {

#if DEBUG_MEMORY

		Com_DPrintf( "[Memory_free]: %p\n", memblock );

#endif

	    free( memblock );
    }
}

/**
 * \brief Deallocates or frees a memory block.
 * \param[in] name function name this error occured in.
 * \param[in] file File this error occurred in.
 * \param[in] line Line number this error occurred on.
 */
PUBLIC void Memory_outofmem( const char *name, const char *file, W32 line )
{

    Com_Error( ERR_FATAL, "%s:%ld failed allocation for \"%s\"\n",
	    file, line, name );

}
