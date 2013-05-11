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
 * \file zmem.h
 * \brief Zone memory management.
 * \author Michael Liebscher
 * \date 2004
 * \note This code was derived from Quake II, and was originally written by Id Software, Inc. 
 */

#ifndef __ZMEM_H__
#define __ZMEM_H__

#include <stdlib.h>

// memory tags to allow dynamic memory to be cleaned up
#define	TAG_GAME		765		/* clear when unloading the dll */
#define	TAG_LEVEL		766		/* clear when loading a new level */
#define	TAG_LEVEL_SCP	767		/* clear when unloading level script */



typedef struct zhead_s
{
	struct zhead_s	*prev, *next;

	short	magic;
	short	tag;			// for group free
	int		size;

} zhead_t;

extern zhead_t	z_chain;


// Returns 0 filled memory block
void *Z_Malloc( size_t size );

void *Z_TagMalloc( size_t size, int tag );
void Z_Free( void *memblock );
void Z_FreeTags( int tag );

void Z_Stats_f( void );


#endif /* __ZMEM_H__ */
