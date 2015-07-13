/*

    Copyright (C) 2004-2013 Michael Liebscher
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
#include <stdlib.h>

#include "wolf_player.h"
#include "wolf_local.h"
#include "wolf_raycast.h"
#include "wolf_actor_ai.h"


/**
 * \brief User is using a weapon, find out who it hit and calculate damage
 * \param[in] self User attacking
 */
PUBLIC void weapon_attack (player_t *self)
{
    entity_t *closest;
    int damage;
    int dx, dy, dist;
    int d1, shot_dist, n;

    switch (self->weapon) {
    case WEAPON_KNIFE:
        //Sound_StartSound (NULL, 0, CHAN_WEAPON, Sound_RegisterSound ("lsfx/023.wav"), 1, ATTN_NORM, 0);
        break;

    case WEAPON_PISTOL:
        //Sound_StartSound (NULL, 0, CHAN_WEAPON, Sound_RegisterSound ("sfx/012.wav"), 1, ATTN_NORM, 0);
        break;

    case WEAPON_MACHINEGUN:
        //Sound_StartSound (NULL, 0, CHAN_WEAPON, Sound_RegisterSound ("sfx/011.wav"), 1, ATTN_NORM, 0);
        break;

    case WEAPON_GATLINGGUN:
        //Sound_StartSound (NULL, 0, CHAN_WEAPON, Sound_RegisterSound ("sfx/013.wav"), 1, ATTN_NORM, 0);
        break;
    }

    self->madenoise = true;

    dist = 0x7fffffffl;
    closest = NULL;

    for (n = 0 ; n < NumGuards; ++n) {
        if (Guards[ n ].flags & FL_SHOOTABLE) { // && Guards[n].flags&FL_VISABLE
            int guardwidth;

            if (!g_autoaim->value) {
                // Make thin enemies harder to hit
                switch (Guards[ n ].type) {
                case en_guard:
                case en_officer:
                case en_ss:
                case en_dog:
                case en_mutant:
                    guardwidth = (1 * TILE_GLOBAL / 4);
                    break;

                default:
                    guardwidth = (2 * TILE_GLOBAL / 3);
                }
            } else {
                guardwidth = (2 * TILE_GLOBAL / 3);
            }

            shot_dist = Point2LineDist (Guards[ n ].x - self->position.origin[ 0 ], Guards[ n ].y - self->position.origin[ 1 ], self->position.angle);

            if (shot_dist > guardwidth) {
                continue; // miss
            }

            d1 = LineLen2Point (Guards[ n ].x - self->position.origin[ 0 ], Guards[ n ].y - self->position.origin[ 1 ], self->position.angle);

            if (d1 < 0 || d1 > dist) {
                continue;
            }

            if (! Level_CheckLine (Guards[ n ].x, Guards[ n ].y, Player.position.origin[0], Player.position.origin[1], r_world)) {
                //if( ! CheckLine( &Guards[ n ] ) )
                continue; // obscured
            }

            dist = d1;
            closest = &Guards[ n ];
        }
    }

    if (! closest) { // missed
        r_trace_t trace;

        if (self->weapon == WEAPON_KNIFE) {
            return;
        }

        trace.angle = angle_normalize (self->position.angle - DEG2RAD (2) + (DEG2RAD (rand() % 4)));

        trace.x = self->position.origin[ 0 ];
        trace.y = self->position.origin[ 1 ];
        trace.flags = TRACE_BULLET;
        trace.tile_vis = NULL;

        R_Trace (&trace, r_world);

        if (trace.flags & TRACE_HIT_DOOR) {
            //Sound_StartSound (NULL, 0, CHAN_AUTO, Sound_RegisterSound ("lsfx/028.wav"), 1, ATTN_NORM, 0);
        }

        return;
    }

    // hit something

    if (self->weapon == WEAPON_KNIFE) {
        if (dist > TILE2POS (1)) {
            return;
        }

        damage = US_RndT() >> 4;
    } else {
        dx = ABS (closest->tilex - self->tilex);
        dy = ABS (closest->tiley - self->tiley);
        dist = max_of_2 (dx, dy);

        if (dist < 2) {
            damage = US_RndT() / 4;
        } else if (dist < 4) {
            damage = US_RndT() / 6;
        } else {
            if (US_RndT() / 12 < dist) {
                return; // missed
            }

            damage = US_RndT() / 6;
        }
    }

    A_DamageActor (closest, damage);
}
