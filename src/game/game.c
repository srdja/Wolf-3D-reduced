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

#include "../util/com_string.h"
#include "client.h"


level_locals_t  levelstate;

LRstruct LevelRatios;

float g_fov;
float  g_autoaim;
int skill;
float  mapScale;

currentMap_t currentMap;

char *spritelocation = WL6SPRITESDIRNAME;


/**
 * Reset level state
 */
void Game_Reset (void)
{
    memset (&levelstate, 0, sizeof (levelstate));
}

extern void Con_ToggleAutomap_f (void);

/**
 * Initialize game
 */
void Game_Init (void)
{
    g_fov     = 68;
    g_autoaim = 1;
    mapScale  = 0; // automap scale
    skill     = 1;

    WM_BuildTables();
    CorrectBossDeathTimeouts();

    Powerup_Reset();
    Sprite_Reset();

    Game_Reset();
    PL_Init();
}

//FIXME: put this in the right place
#define SAVEGAME_VERSION 0

extern uint8_t areaconnect[ NUMAREAS ][ NUMAREAS ];
extern bool areabyplayer[ NUMAREAS ];

/**
 * Save current game state to file
 * @param[in] name Name of save game file.
 */
void SaveTheGame (const char *name)
{
    FILE    *f;
    char    path[1024];
    int     version = SAVEGAME_VERSION;
    int     i;
    LevelData_t copiedLevelData;

    if (Player.playstate != ex_playing) {
        return;
    }

    com_snprintf (path, sizeof (path), "%s%c%s.bin", FS_Userdir(), PATH_SEP, name);
    f = fopen (path, "wb");

    if (! f) {
        return;
    }

    // turn the r_world->Doors.doors from pointers to indexes
    // Can't be destructive anymore, because the app can be suspended in the background now.

    // Quick fix for non-destructiveness: Just make a copy of the levelData, convert the
    // pointers to indices in the copy, and write the copy to the save data. Leave the actual
    // current levelData alone.
    copiedLevelData = levelData;

    for (i = 0 ; i < copiedLevelData.Doors.doornum ; i++) {
        intptr_t index = r_world->Doors.Doors[i] - &r_world->Doors.DoorMap[0][0];
        assert (index >= 0 && index < 4096);
        copiedLevelData.Doors.Doors[i] = (void *)index;
    }

    // this is only used for the mutant death face, so just
    // clear it instead of fixing it up
    Player.LastAttacker = NULL;

    currentMap.version = SAVEGAME_VERSION;
    fwrite (&currentMap, 1, sizeof (currentMap), f);

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

extern void StartGame (int episode, int mission, int g_skill);

/**
 * Load game state from file
 * @param[in] name Name of save game file to load.
 * @return 1 on success, otherwise 0
 */
int LoadTheGame (const char *name)
{
    FILE    *f;
    char    path[1024];
    int     version;
    int     i;
    int     oldCompleted;

    com_snprintf (path, sizeof (path), "%s%c%s.bin", FS_Userdir(), PATH_SEP, name);
    f = fopen (path, "rb");

    if (! f) {
        return 0;
    }

    fread (&currentMap, 1, sizeof (currentMap) , f);

    if (currentMap.version != SAVEGAME_VERSION) {
        fclose (f);
        return 0;
    }

    // do a normal map start
    skill = currentMap.skill;
    PL_NewGame (&Player);

    oldCompleted = currentMap.levelCompleted;
    StartGame (currentMap.episode, currentMap.map, currentMap.skill);

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
        return 0;
    }

    // turn the r_world->Doors.doors back to pointers
    for (i = 0 ; i < r_world->Doors.doornum ; i++) {
        int index = (intptr_t)r_world->Doors.Doors[i];
        assert (index >= 0 && index < 4096);
        r_world->Doors.Doors[i] = &r_world->Doors.DoorMap[0][0] + index;
    }

    return 1;
}
