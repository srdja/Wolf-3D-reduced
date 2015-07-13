/*

    Copyright (C) 2005 Michael Liebscher <johnnycanuck@users.sourceforge.net>
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
 *  vector.h: 2D and 3D vector math routines.
 *
 *  Author: Michael Liebscher   <johnnycanuck@users.sourceforge.net>
 *
 *  Acknowledgement:
 *  Portion of this code was derived from Quake II, and was originally
 *  written by Id Software, Inc.
 *
 */

#ifndef __VECTOR_H__
#define __VECTOR_H__

#include "com_math.h"

#include "common_utils.h"
#include "platform.h"

typedef float vec_t;
typedef vec_t vec3_t[ 3 ];

#define vectorCopy( a, b )  ( (b)[ 0 ] = (a)[ 0 ], (b)[ 1 ] = (a)[ 1 ], (b)[ 2 ] = (a)[ 2 ] )

#endif /* __VECTOR_H__ */
