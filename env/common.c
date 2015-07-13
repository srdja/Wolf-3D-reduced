/*

    Copyright (C) 2004-2012 Michael Liebscher <johnnycanuck@users.sourceforge.net>
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

#include <stdio.h>
#include <stdarg.h>
#include "common.h"
#include <setjmp.h>
#include "keys.h"
#include "../wolf/wolf_local.h"

#define MAXPRINTMSG 4096

colour3_t   colourBlack = {   0,   0,   0 };
colour3_t   colourWhite = { 255, 255, 255 };

jmp_buf abortframe;     // an ERR_DROP occured, exit the entire frame

FILE    *logfile;


PUBLIC void Com_DPrintf (const char *fmt, ...)
{
    va_list     argptr;
    static char msg[ MAXPRINTMSG ];

    va_start (argptr, fmt);
    (void)vsnprintf (msg, sizeof (msg), fmt, argptr);
    va_end (argptr);

    msg[ sizeof (msg) - 1 ] = '\0';
}

PUBLIC void Com_Quit (void)
{
    Game_Shutdown();

    if (logfile) {
        fclose (logfile);
        logfile = NULL;
    }
    Sys_Quit();
}


PUBLIC void common_Init()
{
    if (setjmp (abortframe)) {
        Sys_Error ("Error during initialization");
    }

    Cvar_Init();
    // prepare enough of the subsystems to handle
    // cvar and command buffer management

    Cbuf_Init();
    Cmd_Init();

    Key_Init();

    FS_InitFilesystem();

    Cbuf_AddText ("exec DEFAULT.CFG\n");
    Cbuf_AddText ("exec config.cfg\n");
    Cbuf_Execute();

    Client_Init();
    Game_Init();    // game and player init

    // if the user didn't give any commands, run default action
    Cbuf_AddText ("intro PC13\n");
    Cbuf_Execute();
}