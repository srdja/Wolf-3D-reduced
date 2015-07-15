/*

    Copyright (C) 2004 Michael Liebscher
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


/**
 * \file wolf_pushwalls.c
 * \brief Wolfenstein 3-D push-wall handler.
 * \author Michael Liebscher
 * \date 2004
 * \note This code was derived from NewWolf, and was originally written by DarkOne the Hacker.
 */

#include <stdio.h>
#include <string.h>


#include "wolf_local.h"
#include "wolf_level.h"

Pwall_t PWall;

/**
 * \brief Reset pushwall status.
 */
void PushWall_Reset (void)
{
    memset (&PWall, 0, sizeof (Pwall_t));
}

/**
 * \brief Try to move push-wall.
 * \param[in] x coordinates in tile map.
 * \param[in] y coordinates in tile map.
 * \param[in] dir Direction in which push-wall is intended to move.
 * \return Returns true if push successful, otherwise false.
 * \note Called whenever someone tries to push a secret wall.
 */
_boolean PushWall_Push (int x, int y, dir4type dir)
{
    int dx, dy;


    if (PWall.active) {
        return false; // another PWall is moving [only one at a time!]
    }

    dx = dx4dir[ dir ];
    dy = dy4dir[ dir ];


    if (r_world->tilemap[ x + dx ][ y + dy ] & (SOLID_TILE | DOOR_TILE)) {
        // noway (smth is blocking)
        return true;
    }

// remove secret flag & make everything needed when pushwall used!
    r_world->tilemap[ x ][ y ] &= (~SECRET_TILE);
    r_world->tilemap[ x ][ y ] &= (~WALL_TILE);
    r_world->tilemap[ x ][ y ] |= PUSHWALL_TILE;

    levelstate.found_secrets++;

    if (g_version == SPEAROFDESTINY) {
        //Sound_StartSound (NULL, 1, CHAN_AUTO, Sound_RegisterSound ("sfx/030.wav"), 1, ATTN_STATIC, 0);
    } else {
        //Sound_StartSound (NULL, 1, CHAN_AUTO, Sound_RegisterSound ("sfx/034.wav"), 1, ATTN_STATIC, 0);
    }

// good way to avoid stuckness; [un]comment one more down!
// it makes a tile behind pushwall unpassable

    r_world->tilemap[ x + dx ][ y + dy ] |= PUSHWALL_TILE;
    r_world->wall_tex_x[ x + dx ][ y + dy ] = r_world->wall_tex_x[ x ][ y ];
    r_world->wall_tex_y[ x + dx ][ y + dy ] = r_world->wall_tex_y[ x ][ y ];

// write down PWall info
    PWall.active = true;
    PWall.PWtilesmoved = PWall.PWpointsmoved = 0;
    PWall.dir = dir;
    PWall.x = x;
    PWall.y = y;
    PWall.dx = dx;
    PWall.dy = dy;

    PWall.tex_x = r_world->wall_tex_x[ x ][ y ];
    PWall.tex_y = r_world->wall_tex_y[ x ][ y ];


    return true;
}

/**
 * \brief Process push-walls
 */
void PushWall_Process (void)
{
    if (! PWall.active) {
        return; // no active PWall to work with
    }

    PWall.PWpointsmoved += tics;

    if (PWall.PWpointsmoved < 128) {
        return;
    }

    PWall.PWpointsmoved -= 128;
    PWall.PWtilesmoved++;
// Free tile
    r_world->tilemap[ PWall.x ][ PWall.y ] &= (~PUSHWALL_TILE);
// Occupy new tile
    PWall.x += PWall.dx;
    PWall.y += PWall.dy;

// Shall we move further?
    if (r_world->tilemap[ PWall.x + PWall.dx ][ PWall.y + PWall.dy ] & (SOLID_TILE | DOOR_TILE | ACTOR_TILE | POWERUP_TILE) ||
            PWall.PWtilesmoved == 3) {
        r_world->tilemap[ PWall.x ][ PWall.y ] &= (~PUSHWALL_TILE); // wall now
        r_world->tilemap[ PWall.x ][ PWall.y ] |= WALL_TILE; // wall now
        r_world->wall_tex_x[ PWall.x ][ PWall.y ] = PWall.tex_x;
        r_world->wall_tex_y[ PWall.x ][ PWall.y ] = PWall.tex_y;

        PWall.active = false; // Free Push Wall
    } else {
        r_world->tilemap[ PWall.x + PWall.dx ][ PWall.y + PWall.dy ] |= PUSHWALL_TILE;
    }

}
