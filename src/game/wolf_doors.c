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
 * \file wolf_doors.c
 * \brief Wolfenstein 3-D door management.
 * \author Michael Liebscher
 * \date 2004-2012
 * \author DarkOne the Hacker
 * \date 2000-2002
 * \note Portion of this code was derived from NewWolf, and was originally written by DarkOne the Hacker.
 *       Portion of this code was derived from Wolfenstein 3-D, and was originally written by Id Software, Inc.
 */

#include <stdio.h>
#include <string.h>

#include "wolf_level.h"
#include "wolf_player.h"
#include "wolf_local.h"

#define CLOSEWALL   MINDIST // Space between wall & player
#define MAXDOORS    64      // max number of sliding doors




/**
 * \brief Resets doors status
 * \param[in] lvldoors LevelDoors_t  structure
 */
PUBLIC void Door_Reset (LevelDoors_t *lvldoors)
{
    lvldoors->doornum = 0;

    memset (lvldoors->Doors, 0, sizeof (lvldoors->Doors));
    memset (lvldoors->DoorMap, 0, sizeof (lvldoors->DoorMap));
}

/**
 * \brief Spawn door
 * \param[in] lvldoors Level doors structure
 * \param[in] x X position in tile map
 * \param[in] y Y position in tile map
 * \param[in] type Type of door to spawn
 * \return 0 on error, otherwise 1
 */
PUBLIC int Door_Spawn (LevelDoors_t *lvldoors, int x, int y, int type)
{
    if (lvldoors->doornum >= MAXDOORS) {
        printf("[%s]: Too many Doors on level! (%d)\n", "wolf_doors.c", lvldoors->doornum);
        return 0;
    }

    switch (type) {
    case 0x5A:
        lvldoors->DoorMap[ x ][ y ].type    = DOOR_VERT;
        lvldoors->DoorMap[ x ][ y ].vertical = true;
        lvldoors->DoorMap[ x ][ y ].texture = TEX_DDOOR + 1;
        break;

    case 0x5B:
        lvldoors->DoorMap[ x ][ y ].type = DOOR_HORIZ;
        lvldoors->DoorMap[ x ][ y ].vertical = false;
        lvldoors->DoorMap[ x ][ y ].texture = TEX_DDOOR;
        break;

    case 0x5C:
        lvldoors->DoorMap[ x ][ y ].type = DOOR_G_VERT;
        lvldoors->DoorMap[ x ][ y ].vertical = true;
        lvldoors->DoorMap[ x ][ y ].texture = TEX_DLOCK;
        break;

    case 0x5D:
        lvldoors->DoorMap[ x ][ y ].type = DOOR_G_HORIZ;
        lvldoors->DoorMap[ x ][ y ].vertical = false;
        lvldoors->DoorMap[ x ][ y ].texture = TEX_DLOCK;
        break;

    case 0x5E:
        lvldoors->DoorMap[ x ][ y ].type = DOOR_S_VERT;
        lvldoors->DoorMap[ x ][ y ].vertical = true;
        lvldoors->DoorMap[ x ][ y ].texture = TEX_DLOCK + 1;
        break;

    case 0x5F:
        lvldoors->DoorMap[ x ][ y ].type = DOOR_S_HORIZ;
        lvldoors->DoorMap[ x ][ y ].vertical = false;
        lvldoors->DoorMap[ x ][ y ].texture = TEX_DLOCK + 1;
        break;

    case 0x64:
        lvldoors->DoorMap[ x ][ y ].type = DOOR_E_VERT;
        lvldoors->DoorMap[ x ][ y ].vertical = true;
        lvldoors->DoorMap[ x ][ y ].texture = TEX_DELEV + 1;
        break;

    case 0x65:
        lvldoors->DoorMap[ x ][ y ].type = DOOR_E_HORIZ;
        lvldoors->DoorMap[ x ][ y ].vertical = false;
        lvldoors->DoorMap[ x ][ y ].texture = TEX_DELEV;
        break;

    default:
        printf("Door_Spawn: Unknown door type: %d\n", type);
        return 0;
    }

    lvldoors->DoorMap[ x ][ y ].tilex = x;
    lvldoors->DoorMap[ x ][ y ].tiley = y;
    lvldoors->DoorMap[ x ][ y ].action = dr_closed;

    lvldoors->Doors[ lvldoors->doornum ] = &lvldoors->DoorMap[ x ][ y ];
    lvldoors->doornum++;

    return lvldoors->doornum - 1;
}

/**
 * \brief Spawn door
 * \param[in] lvldoors Level doors structure
 * \param[in] x X position in tile map
 */
PUBLIC void Door_SetAreas (LevelDoors_t *lvldoors, int (*areas)[64])
{
    int n, x, y;

    for (n = 0 ; n < lvldoors->doornum ; ++n) {
        x = lvldoors->Doors[ n ]->tilex;
        y = lvldoors->Doors[ n ]->tiley;

        if (lvldoors->Doors[ n ]->vertical) {
            lvldoors->Doors[ n ]->area1 = areas[ x + 1 ][ y ] >= 0 ? areas[ x + 1 ][ y ] : 0;
            lvldoors->Doors[ n ]->area2 = areas[ x - 1 ][ y ] >= 0 ? areas[ x - 1 ][ y ] : 0;
        } else {
            lvldoors->Doors[ n ]->area1 = areas[ x ][ y + 1 ] >= 0 ? areas[ x ][ y + 1 ] : 0;
            lvldoors->Doors[ n ]->area2 = areas[ x ][ y - 1 ] >= 0 ? areas[ x ][ y - 1 ] : 0;
        }
    }
}

/**
 * \brief Can this door be closed
 * \param[in] x X position in tile map
 * \param[in] y Y position in tile map
 * \param[in] vertical Set to true if door is vertical, false if horizontal.
 * \return 0 if door can not be closed, otherwise 1.
 */
PRIVATE W8 CanCloseDoor (int x, int y, _boolean vertical)
{
    int n;

    if (POS2TILE (Player.position.origin[ 0 ]) == x &&
            POS2TILE (Player.position.origin[ 1 ]) == y) {
        return 0;
    }

    if (vertical) {
        if (POS2TILE (Player.position.origin[ 1 ]) == y) {
            if (POS2TILE (Player.position.origin[ 0 ] + CLOSEWALL) == x) {
                return 0;
            }

            if (POS2TILE (Player.position.origin[ 0 ] - CLOSEWALL) == x) {
                return 0;
            }
        }

        for (n = 0 ; n < NumGuards ; ++n) {
            if (Guards[ n ].tilex == x && Guards[ n ].tiley == y) {
                return 0; // guard in door
            }

            if (Guards[ n ].tilex == x - 1 &&
                    Guards[ n ].tiley == y &&
                    POS2TILE (Guards[ n ].x + CLOSEWALL) == x) {
                return 0; // guard in door
            }

            if (Guards[ n ].tilex == x + 1 &&
                    Guards[ n ].tiley == y &&
                    POS2TILE (Guards[ n ].x - CLOSEWALL) == x) {
                return 0; // guard in door
            }
        }
    } else {
        if (POS2TILE (Player.position.origin[ 0 ]) == x) {
            if (POS2TILE (Player.position.origin[ 1 ] + CLOSEWALL) == y) {
                return 0;
            }

            if (POS2TILE (Player.position.origin[ 1 ] - CLOSEWALL) == y) {
                return 0;
            }
        }

        for (n = 0 ; n < NumGuards ; ++n) {
            if (Guards[ n ].tilex == x && Guards[ n ].tiley == y) {
                return 0; // guard in door
            }

            if (Guards[ n ].tilex == x &&
                    Guards[ n ].tiley == y - 1 &&
                    POS2TILE (Guards[ n ].y + CLOSEWALL) == y) {
                return 0; // guard in door
            }

            if (Guards[ n ].tilex == x &&
                    Guards[ n ].tiley == y + 1 &&
                    POS2TILE (Guards[ n ].y - CLOSEWALL) == y) {
                return 0; // guard in door
            }
        }
    }

    return 1;
}

/**
 * \brief Open door
 * \param[in] door Door to open
 */
PUBLIC void Door_Open (doors_t *door)
{
    if (door->action == dr_open) {
        door->ticcount = 0;     // reset opened time
    } else {
        door->action = dr_opening;  // start opening it
    }
}

/**
 * \brief Change door state
 * \param[in] Door Door state to change
 */
PRIVATE void Door_ChangeDoorState (doors_t *Door)
{
    if (Door->action < dr_opening) {
        Door_Open (Door);
    } else if (Door->action == dr_open && CanCloseDoor (Door->tilex, Door->tiley, Door->vertical)) {
        Door->action = dr_closing;
        Door->ticcount = DOOR_FULLOPEN;
    }
}

/**
 * \brief Doors to process
 * \param[in] lvldoors Doors to process
 * \param[in] t_tk Clock tics
 */
PUBLIC void Door_Process (LevelDoors_t *lvldoors, int t_tk)
{
    int n;

    for (n = 0 ; n < lvldoors->doornum ; ++n) {
        switch (lvldoors->Doors[ n ]->action) {
        case dr_closed: // this door is closed!
            continue;

        case dr_opening:
            if (lvldoors->Doors[ n ]->ticcount >= DOOR_FULLOPEN) { // door fully opened!
                lvldoors->Doors[ n ]->action = dr_open;
                lvldoors->Doors[ n ]->ticcount = 0;
            } else { // opening!
                if (lvldoors->Doors[ n ]->ticcount == 0) {
                    // door is just starting to open, so connect the areas
                    Areas_Join (lvldoors->Doors[ n ]->area1, lvldoors->Doors[ n ]->area2);
                    Areas_Connect (Player.areanumber);

                    if (areabyplayer[ lvldoors->Doors[ n ]->area1 ]) { // Door Opening sound!
                        //Sound_StartSound (NULL, 1, CHAN_AUTO, Sound_RegisterSound ("sfx/010.wav"), 1, ATTN_STATIC, 0);
                    }
                }

                lvldoors->Doors[n]->ticcount += t_tk;

                if (lvldoors->Doors[ n ]->ticcount > DOOR_FULLOPEN) {
                    lvldoors->Doors[ n ]->ticcount = DOOR_FULLOPEN;
                }
            }

            break;

        case dr_closing:
            if (lvldoors->Doors[ n ]->ticcount <= 0) { // door fully closed! disconnect areas!
                Areas_Disconnect (lvldoors->Doors[ n ]->area1, lvldoors->Doors[ n ]->area2);
                Areas_Connect (Player.areanumber);
                lvldoors->Doors[ n ]->ticcount = 0;
                lvldoors->Doors[ n ]->action = dr_closed;
            } else { // closing!
                if (lvldoors->Doors[ n ]->ticcount == DOOR_FULLOPEN) {
                    if (areabyplayer[ lvldoors->Doors[ n ]->area1 ]) { // Door Closing sound!
                        //Sound_StartSound (NULL, 1, CHAN_AUTO, Sound_RegisterSound ("sfx/007.wav"), 1, ATTN_STATIC, 0);
                    }
                }

                lvldoors->Doors[ n ]->ticcount -= t_tk;

                if (lvldoors->Doors[ n ]->ticcount < 0) {
                    lvldoors->Doors[ n ]->ticcount = 0;
                }
            }

            break;

        case dr_open:
            if (lvldoors->Doors[ n ]->ticcount > DOOR_MINOPEN) {
                // If player or something is in door do not close it!
                if (! CanCloseDoor (lvldoors->Doors[ n ]->tilex, lvldoors->Doors[ n ]->tiley, lvldoors->Doors[ n ]->vertical)) {
                    lvldoors->Doors[ n ]->ticcount = DOOR_MINOPEN; // do not close door immediately!
                }
            }

            if (lvldoors->Doors[ n ]->ticcount >= DOOR_TIMEOUT) {
                // Door timeout, time to close it!
                lvldoors->Doors[ n ]->action = dr_closing;
                lvldoors->Doors[ n ]->ticcount = DOOR_FULLOPEN;
            } else {
                // Increase timeout!
                lvldoors->Doors[ n ]->ticcount += t_tk;
            }

            break;

        } // End switch lvldoors->Doors[ n ].action

    } // End for n = 0 ; n < lvldoors->doornum ; ++n

}

/**
 * \brief Check to see if a door is open.
 * \param[in] lvldoors Doors
 * \param[in] x X position in door map
 * \param[in] x Y position in door map
 * \return DOOR_FULLOPEN        Door is opened
 *         0                    Door is closed
 *         >0 <DOOR_FULLOPEN    Door is partially opened.
 * \note If there are no doors in tile assume a closed door!
 */
PUBLIC int Door_Opened (LevelDoors_t *lvldoors, int x, int y)
{
    return lvldoors->DoorMap[ x ][ y ].action == dr_open ? DOOR_FULLOPEN : lvldoors->DoorMap[ x ][ y ].ticcount;
}

/**
 * \brief Try to use a door with keys that the player has.
 * \param[in] Door Door to use
 * \param[in] keys Player key flag
 * \return Always true
 */
PUBLIC void Door_Use (doors_t *Door, int keys)
{
    switch (Door->type) {
    case DOOR_VERT:
    case DOOR_HORIZ:
    case DOOR_E_VERT:
    case DOOR_E_HORIZ:
        Door_ChangeDoorState (Door);  // does not require key!
        break;

    case DOOR_G_VERT:
    case DOOR_G_HORIZ:
        if (keys & ITEM_KEY_GOLD) {
            Door_ChangeDoorState (Door);
        }
        break;

    case DOOR_S_VERT:
    case DOOR_S_HORIZ:
        if (keys & ITEM_KEY_SILVER)
            Door_ChangeDoorState (Door);
        break;
    }

}
