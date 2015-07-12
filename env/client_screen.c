/*

    Copyright (C) 2004-2013 Michael Liebscher <johnnycanuck@users.sourceforge.net>
    Copyright (C) 1997-2001 Id Software, Inc.

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
 * @file client_screen.c
 * @brief Master for refresh, status bar, console, chat, notify, etc.
 * @author Michael Liebscher
 * @date 2004-2013
 * @author Id Software, Inc.
 * @date 1997-2001
 * @note This code was derived from Quake II, and was originally written by Id Software, Inc.
 */

#include "client.h"
#include "console.h"

#include "renderer.h"
#include "video.h"

#include "wolf_level.h"

void R_DrawWorld (void);

float       scr_con_current;    // aproaches scr_conlines at scr_conspeed
float       scr_conlines;       // 0.0 to 1.0 lines of console to display
cvar_t      *scr_conspeed;


/**
 * Figure out how the console should be drawn.
 */
PUBLIC void Client_Screen_RunConsole (void)
{
    // decide on the height of the console
    if (ClientStatic.key_dest == key_console) {
        scr_conlines = 0.5;     // half screen
    } else {
        scr_conlines = 0;       // none visible
    }

    if (scr_conlines < scr_con_current) {
        scr_con_current -= scr_conspeed->value * ClientStatic.frametime;

        if (scr_conlines > scr_con_current) {
            scr_con_current = scr_conlines;
        }

    } else if (scr_conlines > scr_con_current) {
        scr_con_current += scr_conspeed->value * ClientStatic.frametime;

        if (scr_conlines < scr_con_current) {
            scr_con_current = scr_conlines;
        }
    }
}

/**
 * Draw the console on the screen.
 */
PUBLIC void Client_Screen_DrawConsole (void)
{
    if (r_world != NULL) {
        R_DrawWorld();
    }

    Con_CheckResize();

    if (ClientStatic.state == ca_disconnected || ClientStatic.state == ca_connecting) {
        // forced full screen console
        Con_DrawConsole (1.0);
        return;
    }

    if (ClientStatic.state != ca_active) { // || !ClientState.refresh_prepped )
        // connected, but can't render
        Con_DrawConsole (0.5);
        R_Draw_Fill (0, viddef.height >> 1, viddef.width, viddef.height >> 1, colourBlack);
        return;
    }

    if (scr_con_current) {
        Con_DrawConsole (scr_con_current);
    } else {
        if (ClientStatic.key_dest == key_game || ClientStatic.key_dest == key_message) {
            Con_DrawNotify();   // only draw notify in game
        }
    }
}

/**
 * Initialize client screen variables.
 */
PUBLIC void Client_Screen_Init (void)
{
    scr_conspeed = Cvar_Get ("scr_conspeed", "3", CVAR_INIT);
}

