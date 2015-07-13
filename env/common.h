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
 * \file common.h
 * \brief Common definitions between client and server.
 * \author Michael Liebscher
 * \date 2004
 * \note This code was derived from Quake II, and was originally written by Id Software, Inc.
 */


#ifndef __COMMON_H__
#define __COMMON_H__


#include "platform.h"
#include "com_math.h"
#include "cvar.h"
#include "cmd.h"
#include "num_type.h"

#include "memory.h"
#include "filesystem.h"

#include "../app_def.h"

#define BIG_INFO_STRING  8192  // used for system info key only


char *Com_StringContains (char *str1, char *str2, int casesensitive);
int Com_Filter (char *filter, char *name, int casesensitive);

extern colour3_t    colourBlack;
extern colour3_t    colourGreen;
extern colour3_t    colourWhite;

#define ERR_FATAL       0       // exit the entire game with a popup window
#define ERR_DROP        1       // print to console and disconnect from game
#define ERR_DISCONNECT  4       // don't kill server

#define EXEC_NOW    0       // don't return until completed
#define EXEC_INSERT 1       // insert at current position, but don't run yet
#define EXEC_APPEND 2       // add to end of the command buffer

void Com_DPrintf (const char *fmt, ...);
void Com_Error (int code, const char *fmt, ...);
void Com_Quit (void);

extern  cvar_t  *developer;
extern  cvar_t  *dedicated;

void common_Init (int argc, char *argv[]);
void common_Frame (int msec);

void    Sys_Error (const char *format, ...);
void    Sys_Quit (void);
char    *Sys_GetClipboardData (void);

void Client_Init (void);
void Client_Drop (void);
void Client_Shutdown (void);
void Client_Frame (int msec);

//
// button bits
//
#define BUTTON_ATTACK       1
#define BUTTON_USE          2
#define BUTTON_ANY          128         // any key whatsoever


// usercmd_t is sent to the server each client frame
typedef struct usercmd_s {
    W8  msec;
    W8  buttons;
    short   angles[ 3 ];
    short   forwardmove, sidemove, upmove;
    W8  impulse;        // remove?
    W8  lightlevel;     // light level the player is standing on

} usercmd_t;


extern const char *completionString;
extern char shortestMatch[MAX_TOKEN_CHARS];
void FindMatches (const char *s);


#endif /* __COMMON_H__ */

