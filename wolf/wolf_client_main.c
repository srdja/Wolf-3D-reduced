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

#include "../env/client.h"
#include "../env/video.h"
#include "../env/renderer.h"
#include "../env/timer.h"
#include "../env/com_string.h"
#include "../env/console.h"
#include "../env/input.h"
#include "../env/sound.h"

#include "wolf_local.h"
#include "wolf_level.h"
#include "wolf_player.h"
#include "wolf_renderer.h"
#include "wolf_menu.h"


#include "opengl_local.h"

extern void R_DrawWorld (void);
extern int elevatorSwitchTime;

vec3_t vnull = { 0, 0, 0 };


/**
 * \brief Render game world
 */
PRIVATE void V_RenderView (void)
{
    if (ClientStatic.key_dest != key_game) {
        return;
    }

    R_DrawWorld();
}

/**
 * \brief Process current frame
 */
PUBLIC void Client_Screen_UpdateScreen (void)
{
    R_BeginFrame();


    V_RenderView(); // Draw game world

    Client_Screen_DrawConsole(); // Draw console

    M_Draw(); // Draw menu


    R_EndFrame();

}

/**
 * \brief Refresh level
 * \param[in] r_mapname Name of name
 */
PUBLIC void Client_PrepRefresh (const char *r_mapname)
{
    char mapname[ 64 ]; //gsh, decided to allow longer map names

    if (! r_mapname || ! *r_mapname) {
        return;
    }

    R_DrawPsyched (0);
    R_EndFrame();

    if (g_version->value == SPEAROFDESTINY) {
        spritelocation = SODSPRITESDIRNAME;
    } else {
        spritelocation = WL6SPRITESDIRNAME;
    }

    com_strlcpy (mapname, r_mapname, sizeof (mapname));   // skip "maps/"

    // !@# fix crash bug if you type something short...
    if (strlen (mapname) > 4 && !strcmp (mapname + strlen (mapname) - 4, ".map")) {
        mapname[ strlen (mapname) - 4 ] = '\0';      // cut off ".map"
    }

    if (g_version->value == SPEAROFDESTINY) {
        currentMap.episode = 0;
        currentMap.map = atoi (mapname + 1);
    } else {
        currentMap.episode = mapname[1] - '0';
        currentMap.map =     mapname[2] - '0';
    }

    //this is a hack for "Floor 18, Part II: Death's Door"
    //there's no gold key to leave the first room
    //so we give it to the player here... gsh
    if (g_version->value == SPEAROFDESTINY && currentMap.map == 20) {
        Player.items |= ITEM_KEY_GOLD;
    }

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

    // clear any lines of console text
    Con_ClearNotify();


    if (r_world->musicName) {
        Sound_StartBGTrack (r_world->musicName, r_world->musicName);
    }


    R_EndFrame();

}


int tics;

extern void DrawMenus();


/**
 * \brief Process client frame
 * \param[in] msec Time
 */
PUBLIC void Client_Frame (int msec)
{
    static int extratime;
    extratime += msec;


    // let the mouse activate or deactivate
    IN_Frame();

    // decide the simulation time
    ClientStatic.frametime = extratime / 1000.0f;
    ClientStatic.realtime = Sys_Milliseconds();

    extratime = 0;

#if 0

    if (ClientStatic.frametime > (1.0f / cl_minfps->value)) {
        ClientStatic.frametime = (1.0f / cl_minfps->value);
    }

#else

    if (ClientStatic.frametime > (1.0f / 5)) {
        ClientStatic.frametime = (1.0f / 5);
    }

#endif

    ClientState.time = (int)ClientStatic.frametime * 100;


    tics = 1;



    // allow rendering change
    Video_CheckChanges();
//  if( ! ClientState.refresh_prepped && ClientStatic.state == ca_active )
//      Client_PrepRefresh( levelstate. );


    Sound_Update (vnull, vnull, vnull, vnull);



    if (ClientStatic.menuState == IPM_AUTOMAP) {
        pfglDepthMask (GL_TRUE);
        pfglClear (GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        pfglMatrixMode (GL_MODELVIEW);
        pfglLoadIdentity();
        pfglDisable (GL_DEPTH_TEST);


        pfglEnable (GL_BLEND);


        pfglColor4f (1, 1, 1, 1);

        pfglMatrixMode (GL_PROJECTION);
        pfglLoadIdentity();

        pfglOrtho (0, viddef.width, viddef.height, 0, -99999, 99999);

        DrawMenus();


        R_EndFrame();

        return;
    }




    if (ClientStatic.key_dest == key_game && ClientStatic.state == ca_active) {
        if (Player.playstate != ex_dead &&
                Player.playstate != ex_watchingdeathcam &&
                Player.playstate != ex_watchingbj) {
            Client_SendCommand();

            Player.position.angle = angle_normalize (FINE2RAD (ClientState.viewangles[ YAW ]));
            Player.position.pitch = angle_normalize (FINE2RAD (ClientState.viewangles[ PITCH ]));
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

    Client_Screen_RunConsole();
    Client_Screen_UpdateScreen();


    ClientStatic.framecount++;
}






