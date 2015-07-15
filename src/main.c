/*
    Copyright (C) 2004-2005 Michael Liebscher <johnnycanuck@users.sourceforge.net>
    Copyright (C) 1997-2001 Id Software, Inc.

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

    See the GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
/*
 *  unix_main.c: UNIX interface to application.
 *
 *  Author: Michael Liebscher <johnnycanuck@users.sourceforge.net>
 *
 *  Acknowledgement:
 *  This code was derived from Quake II, and was originally
 *  written by Id Software, Inc.
 *
 */

#include "input/keys.h"
#include "game/wolf_local.h"
#include "util/timer.h"

W32 sys_frame_time;

extern void KBD_Update (void);
extern void StartGame(int a, int b, int g_skill);

int main(int argc, char *argv[])
{
    int time_delta;
    int time_start;
    int time_current;

    Key_Init();

    FS_InitFilesystem();
    Client_Init();
    Game_Init();    // game and player init

    // FIXME temporarily go directly to map one until menus are sorted out
    StartGame( 0,  0,  1);

    time_start = Sys_Milliseconds();

    while (1) {
        KBD_Update();

        // find time spent rendering last frame
        do {
            time_current = Sys_Milliseconds();
            time_delta = time_current - time_start;
        } while (time_delta < 1);

        frame_run(time_delta);
        time_start = time_current;
    }
    return 0;
}
