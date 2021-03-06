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
 * \file wolf_client_main.c
 * \brief Process client frame.
 * \author Michael Liebscher
 * \date 2004-2012
 */

#include <string.h>

#include "client.h"
#include "../graphics/video.h"
#include "../graphics/renderer.h"
#include "../util/timer.h"

#include "wolf_local.h"
#include "wolf_level.h"
#include "wolf_player.h"
#include "../graphics/wolf_renderer.h"
#include "wolf_menu.h"


#include "../graphics/opengl_local.h"
#include "menu/intro.h"

float sensitivity;
float m_yaw;
float m_forward;

float cl_forwardspeed;
float cl_sidespeed;
float cl_yawspeed;

client_state_t  ClientState;
client_static_t ClientStatic;

/**
 * Initialize client variables and commands.
 */
static void Client_InitLocal (void)
{
    ClientStatic.realtime = Sys_Milliseconds();

    cl_forwardspeed = 4000;
    cl_sidespeed    = 4000;
    cl_yawspeed     = 100;
    sensitivity     = 50;
    m_yaw           = 1;
    m_forward       = 1;
}

/**
 * Initialize Client sub-systems.
 * @return Initialize video and sound sub-systems.
 */
void Client_Init (void)
{
    Menu_Init();
    Client_InitLocal();
}

extern void R_DrawWorld (void);
extern int elevatorSwitchTime;

/**
 * \brief Render game world
 */
static void V_RenderView (void)
{
    if (ClientStatic.key_dest != key_game) {
        return;
    }
    R_DrawWorld();
}

/**
 * \brief Process current frame
 */
void Client_Screen_UpdateScreen (void)
{
    R_BeginFrame();
    V_RenderView(); // Draw game world
    M_Draw(); // Draw menu
    R_EndFrame();
}

/**
 * \brief Refresh level
 * \param[in] r_mapname Name of name
 */
void Client_PrepRefresh (const char *r_mapname)
{
    char mapname[ 64 ]; //gsh, decided to allow longer map names

    if (! r_mapname || ! *r_mapname) {
        return;
    }

    R_DrawPsyched (0);
    R_EndFrame();

    spritelocation = WL6SPRITESDIRNAME;

    strncpy(mapname, r_mapname, sizeof(mapname));   // skip "maps/"

    // !@# fix crash bug if you type something short...
    if (strlen (mapname) > 4 && !strcmp (mapname + strlen (mapname) - 4, ".map")) {
        mapname[ strlen (mapname) - 4 ] = '\0';      // cut off ".map"
    }

    currentMap.episode = mapname[1] - '0';
    currentMap.map =     mapname[2] - '0';

    // register models, pics, and skins
    R_BeginRegistration (mapname);

    if (r_world == NULL) {
        return;
    }

    R_DrawPsyched (30);
    R_EndFrame();

    Level_ScanInfoPlane (r_world);  // Spawn items/guards

    PL_Spawn (r_world->pSpawn, r_world);  // Spawn Player

    Level_PrecacheTextures_Sound (r_world);

    R_DrawPsyched (80);
    R_EndFrame();

    // the renderer can now free unneeded stuff
    R_EndRegistration();

    R_DrawPsyched (100);
    R_EndFrame();

    if (r_world->musicName) {
        //Sound_StartBGTrack (r_world->musicName, r_world->musicName);
    }

    R_EndFrame();

    // FIXME moved from wolf_sv_ccmds.c - Map_f()
    if (r_world) {
        ClientStatic.menuState = IPM_GAME;
        ClientStatic.key_dest = key_game;
    } else {
        ClientStatic.key_dest = key_console;
    }
}

int tics;

extern void DrawMenus();

static void frame_run_game()
{
    if (Player.playstate != ex_dead &&
        Player.playstate != ex_watchingdeathcam &&
        Player.playstate != ex_watchingbj)
    {
        player_update_movement();

        Player.position.angle = angle_normalize (FINE2RAD (ClientState.viewangles[ YAW ]));
    } else {
        memset (&ClientState.cmd, 0, sizeof (ClientState.cmd));
    }

    if ((Player.playstate == ex_complete || Player.playstate == ex_secretlevel)) {
        if (ClientStatic.realtime >= elevatorSwitchTime + 1429) {
            M_Intermission_f();
        }
    } else {
        PL_Process (&Player, r_world);   // Player processing
        ProcessGuards();                // if single
        PushWall_Process();
        Door_Process (&r_world->Doors, tics);

        levelstate.time += tics;
    }
}

static void frame_run_menu()
{

}

static void frame_run_dead()
{
    memset (&ClientState.cmd, 0, sizeof (ClientState.cmd));
}

static void frame_run_watching_deathcam()
{
    memset (&ClientState.cmd, 0, sizeof (ClientState.cmd));
}

static void frame_run_watching_bj()
{
    memset (&ClientState.cmd, 0, sizeof (ClientState.cmd));
}

static void frame_run_intermission()
{
    if (ClientStatic.realtime >= elevatorSwitchTime + 1429) {
        M_Intermission_f();
    }
}

/**
 * \brief Process client frame
 * \param[in] msec Time
 */
void frame_run(int msec)
{
    static int extratime;
    extratime += msec;

    // decide the simulation time
    ClientStatic.frametime = extratime / 1000.0f;
    ClientStatic.realtime  = Sys_Milliseconds();

    extratime = 0;

    if (ClientStatic.frametime > (1.0f / 5)) {
        ClientStatic.frametime = (1.0f / 5);
    }

    ClientState.time = (int)ClientStatic.frametime * 100;

    tics = 1;

    switch (ClientStatic.menuState) {
        case IPM_GAME:
            frame_run_game();
            break;
        case IPM_INTRO:
            intro_run();
            break;
    }

    Client_Screen_UpdateScreen();
    ClientStatic.framecount++;
}