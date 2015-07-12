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
 * \file wolf_areas.c
 * \brief Wolfenstein 3-D area management.
 * \author Michael Liebscher
 * \date 2004
 * \author DarkOne the Hacker
 * \date 2000-2002
 * \note Portion of this code was derived from NewWolf, and was originally written by DarkOne the Hacker.
 *       Portion of this code was derived from Wolfenstein 3-D, and was originally written by Id Software, Inc.
 */

/*!
    \note

    Open doors connect two areas, so sounds will travel between them and sight
    will be checked when the player is in a connected area.

    Areaconnect is incremented/decremented by each door. If >0 they connect.

    Every time a door opens or closes the areabyplayer matrix gets recalculated.
    An area is true if it connects with the player's current position.

*/

#include <memory.h>
#include <string.h>

#include "wolf_level.h"


PUBLIC W8           areaconnect[ NUMAREAS ][ NUMAREAS ];    /* Is this area mated with another? */
PUBLIC  _boolean    areabyplayer[ NUMAREAS ];               /* Which areas can I see into? */



/**
 * \brief Recursive routine to properly set the areabyplayer array by using the contents of the areaconnect array.
 * \param[in] area_number Area to connect
 */
PRIVATE void Areas_RecursiveConnect (int area_number)
{
    int i;

    for (i = 0 ; i < NUMAREAS ; ++i) {
        if (areaconnect[ area_number ][ i ] && ! areabyplayer[ i ]) {
            areabyplayer[ i ] = true;
            Areas_RecursiveConnect (i);  /* Link it in... */
        }
    }
}

/**
 * \brief Properly set the areabyplayer record
 * \param[in] area_number Area to connect
 */
PUBLIC void Areas_Connect (int area_number)
{
    memset (areabyplayer, 0, sizeof (areabyplayer));
    areabyplayer[ area_number ] = true;
    Areas_RecursiveConnect (area_number);
}

/**
 * \brief Zero areabyplayer array and areaconnect array
 * \param[in] area_number Area to connect
 * \note Called when a new level is loaded
 */
PUBLIC void Areas_Init (int area_number)
{
    memset (areaconnect, 0, sizeof (areaconnect));
    memset (areabyplayer, 0, sizeof (areabyplayer));
    areabyplayer[ area_number ] = true;
}

/**
 * \brief Insert a connection between two rooms
 * \param[in] area1 Area to connect
 * \param[in] area2 Area to connect
 */
PUBLIC void Areas_Join (int area1, int area2)
{
    // FIXME: check for overflow!
    areaconnect[ area1 ][ area2 ]++;
    areaconnect[ area2 ][ area1 ]++;
}

/**
 * \brief Remove a connection between two rooms
 * \param[in] area1 Area to disconnect
 * \param[in] area2 Area to disconnect
 */
PUBLIC void Areas_Disconnect (int area1, int area2)
{
    // FIXME: check for underflow!
    areaconnect[ area1 ][ area2 ]--;
    areaconnect[ area2 ][ area1 ]--;
}
