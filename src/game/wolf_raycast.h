/*

    Copyright (C) 2004 Michael Liebscher <johnnycanuck@users.sourceforge.net>
    Copyright (C) 2000-2002 by DarkOne the Hacker

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
 *  wolf_raycast.h:  Wolfenstein3-D ray-casting.
 *
 *  Author: Michael Liebscher <johnnycanuck@users.sourceforge.net>
 *  Date:   2004
 *
 *  Acknowledgement:
 *  Portion of this code was derived from NewWolf, and was originally
 *  written by DarkOne the Hacker.
 *
 */

/*
    Notes:
    This module is implemented by wolf_raycast.c

*/

#ifndef __WOLF_RAYCAST_H__
#define __WOLF_RAYCAST_H__


#include "wolf_level.h"


// marks
#define TRACE_MARK_MAP      BIT( 1 )    // marks traced area in 'AM_AutoMap.vis' array
// obstacle levels
#define TRACE_SIGHT         BIT( 2 )    // player sight
#define TRACE_BULLET        BIT( 4 )    // bullet

#define TRACE_HIT_VERT      BIT( 6 )    // vertical wall was hit
#define TRACE_HIT_DOOR      BIT( 7 )    // door was hit

typedef struct r_trace_s {
    int x, y; // origin
    float angle;    // Trace angle in radians
    int flags;
    uint8_t (*tile_vis)[ 64 ]; // should point to [ 64 ][ 64 ] array

} r_trace_t;

#define UPPERZCOORD  0.6f
#define LOWERZCOORD -0.6f

extern uint8_t tile_visible[ 64 ][ 64 ]; // can player see this tile?


void R_RayCast (placeonplane_t viewport, LevelData_t *lvl);
void R_Trace (r_trace_t *trace, LevelData_t *lvl);


#endif /* __WOLF_RAYCAST_H__ */
