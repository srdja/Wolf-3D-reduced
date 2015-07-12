/*

    Copyright (C) 2004-2013 Michael Liebscher <johnnycanuck@users.sourceforge.net>

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
 * @file wolf_main.c
 * @author Michael Liebscher
 * @date 2004-2013
 */

#include <assert.h>
#include <string.h>

#include "wolf_local.h"
#include "wolf_powerups.h"
#include "wolf_sprites.h"
#include "wolf_player.h"
#include "wolf_act_stat.h"

#include "../env/com_string.h"
#include "../env/client.h"


level_locals_t  levelstate;

LRstruct LevelRatios;

cvar_t  *g_version; // Wolfenstein or Spear of Destiny
cvar_t  *g_fov;
cvar_t  *g_autoaim;
cvar_t  *skill;
cvar_t  *deathmatch;
cvar_t  *maxentities;
cvar_t  *mapScale;

currentMap_t currentMap;

char *spritelocation = WL6SPRITESDIRNAME;

PRIVATE _boolean ingame;


/**
 * Reset level state
 */
PUBLIC void Game_Reset (void)
{
    memset (&levelstate, 0, sizeof (levelstate));
}

extern void Map_f (void);
extern void Load_f (void);
extern void Save_f (void);
extern void Con_ToggleAutomap_f (void);

/**
 * Initialize game
 */
PUBLIC void Game_Init (void)
{
    Com_Printf ("\n------ Game Init ------\n");


    // noset vars
//  dedicated = Cvar_Get( "dedicated", "0", CVAR_ROM );

    // latched vars
//  sv_cheats = Cvar_Get( "cheats", "0", CVAR_SERVERINFO |CVAR_LATCH );

    skill = Cvar_Get ("skill", "1", CVAR_LATCH);
    g_version = Cvar_Get ("g_version", "0", CVAR_ARCHIVE | CVAR_LATCH);
    g_fov = Cvar_Get ("g_fov", "68", CVAR_ARCHIVE | CVAR_LATCH);
    g_autoaim = Cvar_Get ("g_autoaim", "1", CVAR_ARCHIVE | CVAR_LATCH);
//  maxentities = Cvar_Get( "maxentities", "1024", CVAR_LATCH );
    mapScale = Cvar_Get ("mapScale", "10", CVAR_ARCHIVE);


    // dm map list
    Cmd_AddCommand ("map", Map_f);
    Cmd_AddCommand ("load", Load_f);
    Cmd_AddCommand ("save", Save_f);

    Cmd_AddCommand ("toggleautomap", Con_ToggleAutomap_f);


    WM_BuildTables();
    CorrectBossDeathTimeouts();

    Powerup_Reset();
    Sprite_Reset();

    Game_Reset();
    PL_Init();

    Com_Printf ("\n-----------------------\n");

}

/**
 * Shutdown game
 */
PUBLIC void Game_Shutdown (void)
{
    Com_Printf ("==== Game Shutdown ====\n");

    Z_FreeTags (TAG_LEVEL);
    Z_FreeTags (TAG_GAME);
}

//FIXME: put this in the right place
#define SAVEGAME_VERSION 0

extern W8   areaconnect[ NUMAREAS ][ NUMAREAS ];
extern _boolean areabyplayer[ NUMAREAS ];

/**
 * Save current game state to file
 * @param[in] name Name of save game file.
 */
PUBLIC void SaveTheGame (const char *name)
{
    FILE    *f;
    char    path[1024];
    int     version = SAVEGAME_VERSION;
    int     i;
    LevelData_t copiedLevelData;

    if (Player.playstate != ex_playing) {
        Com_Printf ("Cannot save while not playing!");
        return;
    }

    com_snprintf (path, sizeof (path), "%s%c%s.bin", FS_Userdir(), PATH_SEP, name);
    f = fopen (path, "wb");

    if (! f) {
        Com_Printf ("Could not open %s.\n", path);
        return;
    }

    // turn the r_world->Doors.doors from pointers to indexes
    // Can't be destructive anymore, because the app can be suspended in the background now.

    // Quick fix for non-destructiveness: Just make a copy of the levelData, convert the
    // pointers to indices in the copy, and write the copy to the save data. Leave the actual
    // current levelData alone.
    copiedLevelData = levelData;

    for (i = 0 ; i < copiedLevelData.Doors.doornum ; i++) {
        INT_PTR index = r_world->Doors.Doors[i] - &r_world->Doors.DoorMap[0][0];
        assert (index >= 0 && index < 4096);
        copiedLevelData.Doors.Doors[i] = (void *)index;
    }

    // this is only used for the mutant death face, so just
    // clear it instead of fixing it up
    Player.LastAttacker = NULL;

    currentMap.version = SAVEGAME_VERSION;
    fwrite (&currentMap, 1, sizeof (currentMap), f);

    //fwrite( &huds, 1,sizeof(huds), f);

    fwrite (&copiedLevelData, 1, sizeof (levelData), f);
    fwrite (&LevelRatios, 1, sizeof (LevelRatios), f);
    fwrite (&levelstate, 1, sizeof (levelstate), f);
    fwrite (Guards, 1, sizeof (Guards), f);
    fwrite (areaconnect, 1, sizeof (areaconnect), f);
    fwrite (areabyplayer, 1, sizeof (areabyplayer), f);
    fwrite (&PWall, 1, sizeof (PWall), f);
    fwrite (&Player, 1, sizeof (Player), f);
    fwrite (&version, 1, sizeof (version), f);

    fclose (f);
}

extern PUBLIC void StartGame (int episode, int mission, int skill);

/**
 * Load game state from file
 * @param[in] name Name of save game file to load.
 * @return 1 on success, otherwise 0
 */
PUBLIC int LoadTheGame (const char *name)
{
    FILE    *f;
    char    path[1024];
    int     version;
    int     i;
    int     oldCompleted;

    com_snprintf (path, sizeof (path), "%s%c%s.bin", FS_Userdir(), PATH_SEP, name);
    f = fopen (path, "rb");

    if (! f) {
        Com_Printf ("Could not open %s.\n", path);
        return 0;
    }

    fread (&currentMap, 1, sizeof (currentMap) , f);

    if (currentMap.version != SAVEGAME_VERSION) {
        Com_Printf ("Savegame header version mismatch: %i != %i\n", currentMap.version, SAVEGAME_VERSION);
        fclose (f);
        return 0;
    }


    Com_Printf ("episode: %i\nmap: %i\n", currentMap.episode, currentMap.map);

    // load the huds
    //fread( &huds, 1,sizeof(huds), f);

    // do a normal map start
    Cvar_SetValue (skill->name, (float)currentMap.skill);
    PL_NewGame (&Player);

    oldCompleted = currentMap.levelCompleted;
    StartGame (currentMap.episode, currentMap.map, currentMap.skill);
    Cbuf_Execute();

    currentMap.levelCompleted = oldCompleted;

    // load modifications on top
    fread (&levelData, 1, sizeof (levelData), f);
    fread (&LevelRatios, 1, sizeof (LRstruct), f);
    fread (&levelstate, 1, sizeof (levelstate), f);
    fread (Guards, 1, sizeof (Guards), f);
    fread (areaconnect, 1, sizeof (areaconnect), f);
    fread (areabyplayer, 1, sizeof (areabyplayer), f);
    fread (&PWall, 1, sizeof (PWall), f);
    fread (&Player, 1, sizeof (Player), f);
    fread (&version, 1, sizeof (version), f);

    fclose (f);

    ClientState.viewangles[ YAW ] = RAD2FINE (Player.position.angle);
    ClientState.viewangles[ PITCH ] = RAD2FINE (Player.position.pitch);

    if (version != SAVEGAME_VERSION) {
        Com_Printf ("Savegame trailer version mismatch: %i != %i\n", version, SAVEGAME_VERSION);
        return 0;
    }

    // turn the r_world->Doors.doors back to pointers
    for (i = 0 ; i < r_world->Doors.doornum ; i++) {
        int index = (INT_PTR)r_world->Doors.Doors[i];
        assert (index >= 0 && index < 4096);
        r_world->Doors.Doors[i] = &r_world->Doors.DoorMap[0][0] + index;
    }

    return 1;
}
