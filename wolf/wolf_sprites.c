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

/**
 * \file wolf_sprites.c
 * \brief Wolfenstein3-D sprite handling.
 * \author Michael Liebscher
 * \date 2004
 * \author DarkOne the Hacker
 * \date 2000-2002
 * \note This code was derived from NewWolf, and was originally written by DarkOne the Hacker.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "wolf_local.h"
#include "wolf_sprites.h"
#include "wolf_math.h"
#include "wolf_raycast.h"
#include "wolf_player.h"


/**
 * \brief Reset sprite status.
 * \note Called only when client must reconnect will not set remove flag!
 */
PUBLIC void Sprite_Reset (void)
{
    levelData.numSprites = 0;
    memset (levelData.sprites, 0, sizeof (levelData.sprites));
}

/**
 * \brief Remove sprite.
 * \param[in] sprite_id sprite id to remove.
 */
PUBLIC void Sprite_RemoveSprite (int sprite_id)
{
    if (sprite_id == -1) {
        return;
    }

    levelData.sprites[ sprite_id ].flags |= SPRT_REMOVE;
}

/**
 * \brief Get index of free location.
 * \return -1 if there is no location free, otherwise a value between 0 and MAX_SPRITES
 */
PUBLIC int Sprite_GetNewSprite (void)
{
    W32 n;
    sprite_t *sprt;

    for (n = 0, sprt = levelData.sprites ; n < levelData.numSprites ; ++n, ++sprt) {
        if (sprt->flags & SPRT_REMOVE) {
            // free spot: clear it first
            memset (sprt, 0, sizeof (sprite_t));
            return n;
        }
    }

    if (levelData.numSprites >= MAX_SPRITES) {
        Com_Printf ("Warning levelData.numSprites == MAX_SPRITES\n");
        return -1;
    }

    return levelData.numSprites++;
}

/**
 * \brief Set sprite position.
 * \param[in] sprite_id sprite id to change.
 * \param[in] index texture index.
 * \param[in] tex  texture to set as.
 */
PUBLIC void Sprite_SetPos (int sprite_id, int x, int y, int angle)
{
    if (sprite_id == -1) {
        return;
    }

    levelData.sprites[ sprite_id ].x = x;
    levelData.sprites[ sprite_id ].y = y;
    levelData.sprites[ sprite_id ].ang = angle;
    levelData.sprites[ sprite_id ].tilex = POS2TILE (x);
    levelData.sprites[ sprite_id ].tiley = POS2TILE (y);
    levelData.sprites[ sprite_id ].flags |= SPRT_CHG_POS;

    if (! (x & TILE_HALF)) { // (x%TILE_GLOBAL>=TILE_HALF)
        levelData.sprites[ sprite_id ].tilex--;
    }

    if (! (y & TILE_HALF)) {
        levelData.sprites[ sprite_id ].tiley--;
    }
}

/**
 * \brief Set sprite texture.
 * \param[in] sprite_id sprite id to change.
 * \param[in] index texture index.
 * \param[in] tex  texture to set as.
 */
PUBLIC void Sprite_SetTex (int sprite_id, int index, int tex)
{
    if (sprite_id == -1) {
        return;
    }

    //CacheTextures( tex, tex );

    if (index == -1) {   // one texture for each phase
        levelData.sprites[ sprite_id ].tex[ 0 ] = tex;
        levelData.sprites[ sprite_id ].flags |= SPRT_ONE_TEX;
    } else {
        levelData.sprites[ sprite_id ].tex[ index ] = tex;
    }

    levelData.sprites[ sprite_id ].flags |= SPRT_CHG_TEX;
}

visobj_t vislist[ MAX_SPRITES ];

/**
 * \brief Compare function for vislist sorting.
 * \param[in] elem1 value to compare.
 * \param[in] elem2 value to compare.
 * \return
 *          <0  elem1 further than elem2
 *          0   elem1 equal distance to elem2
 *          >0  elem1 closer than elem2
 */
PRIVATE int Sprite_cmpVis (const void *elem1, const void *elem2)
{
    // macro to get distance from a void pointer to visobj_t
#define vis_dist( vis ) ( ((visobj_t *)vis)->dist )

    if (vis_dist (elem1) == vis_dist (elem2)) {
        return 0; // process equal distance
    } else {
        // if dist > sprite must be first
        return vis_dist (elem1) < vis_dist (elem2) ? 1 : -1;
    }
}

/**
 * \brief Build and sort visibility list of sprites.
 * \return Number of visible sprites.
 * \note
 *      List is sorted from far to near.
 *      List is based on tile visibility array, made by raycaster.
 *      Called only by client.
 */
PUBLIC int Sprite_CreateVisList (void)
{
    W32 tx, ty, n, num_visible;
    visobj_t *visptr;
    sprite_t *sprt;

    visptr = vislist;
    num_visible = 0;

    for (n = 0, sprt = levelData.sprites; n < levelData.numSprites; ++n, ++sprt) {
        if (sprt->flags & SPRT_REMOVE) {
            continue;
        }

        tx = sprt->tilex;
        ty = sprt->tiley;

        if (tx > 63)
            tx = 63;

        if (ty > 63)
            ty = 63;

        // can be in any of 4 surrounding tiles; not 9 - see definition of tilex & tiley
        if (tile_visible[ tx ][ ty ] || tile_visible[ tx + 1 ][ ty ] ||
                tile_visible[ tx ][ ty + 1 ] || tile_visible[ tx + 1 ][ ty + 1 ]) {
            // player spoted it
            visptr->dist = LineLen2Point (sprt->x - Player.position.origin[ 0 ],
                                          sprt->y - Player.position.origin[ 1 ],
                                          Player.position.angle);  //FIXME viewport
            visptr->x = sprt->x;
            visptr->y = sprt->y;
            //visptr->ang = sprt->ang;
            visptr->tex = sprt->tex[ 0 ]; //FIXME!

            if (++num_visible > MAX_SPRITES) {
                break; // vislist full
            }

            visptr++;
        }
    }

// sorting list
    if (num_visible) { // do not sort if no entries
        qsort (vislist, num_visible, sizeof (visobj_t), Sprite_cmpVis);
    }

    return num_visible;
}

