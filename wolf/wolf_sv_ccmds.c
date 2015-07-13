/*

    Copyright (C) 2004 Michael Liebscher

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
 * \file wolf_sv_ccmds.c
 * \brief
 * \author Michael Liebscher
 * \date 2004-2012
 */

#include <string.h>

#include "wolf_level.h"

#include "../env/cmd.h"
#include "../env/common.h"
#include "../env/client.h"
#include "../env/com_string.h"


extern void Client_PrepRefresh (const char *r_mapname);



PRIVATE void SV_GameMap_f (void)
{
    char r_mapname[ 32 ];

    if (Cmd_Argc() != 2) {
        return;
    }
    // check for clearing the current savegame
    Cmd_Argv (1);

    // start up the next map
    com_strlcpy (r_mapname, Cmd_Argv (1), sizeof (r_mapname));

    Client_PrepRefresh (r_mapname);
}

/*
-----------------------------------------------------------------------------
 Function:

 Parameters:

 Returns:

 Notes:

-----------------------------------------------------------------------------
*/
PUBLIC void Map_f (void)
{
    char    *map;
    char    expanded[ MAX_GAMEPATH ];

    // Check to make sure the level exists.
    map = Cmd_Argv (1);

    if (! strstr (map, ".")) {
        com_snprintf (expanded, sizeof (expanded), "maps%c%s.map", PATH_SEP, map);
    } else {
        com_snprintf (expanded, sizeof (expanded), "maps%c%s", PATH_SEP, map);
    }


//sv.state = ss_dead;       // don't save current level when changing
//  SV_WipeSavegame( "current" );
    SV_GameMap_f();

    if (r_world) {
        ClientStatic.menuState = IPM_GAME;
        ClientStatic.key_dest = key_game;
        ClientStatic.state = ca_active;
    } else {
        ClientStatic.key_dest = key_console;
    }

}

extern int LoadTheGame (const char *name);
extern int SaveTheGame (const char *name);

PUBLIC void Load_f (void)
{
    LoadTheGame (Cmd_Argv (1));
}

PUBLIC void Save_f (void)
{
    SaveTheGame (Cmd_Argv (1));
}
