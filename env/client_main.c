/*

    Copyright (C) 2004 Michael Liebscher <johnnycanuck@users.sourceforge.net>
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
 * @file client_main.c
 * @brief Client main loop.
 * @author Michael Liebscher
 * @date 2004
 * @author Id Software, Inc.
 * @date 1997-2001
 */

#include "client.h"
#include "video.h"
#include "timer.h"
#include "input.h"

float sensitivity;
float m_yaw;
float m_forward;

client_state_t  ClientState;
client_static_t ClientStatic;


#include "../wolf/wolf_local.h"


PUBLIC void Client_Quit_f (void)
{
    Game_Shutdown();
    Sys_Quit();
}

/**
 * Initialize client variables and commands.
 */
PRIVATE void Client_InitLocal (void)
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
PUBLIC void Client_Init (void)
{
    Video_Init();
    //Sound_Init();

    Menu_Init();

    Client_InitLocal();
    IN_Init();
}


/**
 * Shutdown client sub-systems.
 */
PUBLIC void Client_Shutdown (void)
{
    static _boolean isdown = false;

    if (isdown) {
        return;
    }
    isdown = true;

    IN_Shutdown();
    Video_Shutdown();
}