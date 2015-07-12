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


#include "zmem.h"
#include "platform.h"
#include "com_math.h"
#include "cvar.h"
#include "cmd.h"
#include "num_type.h"

#include "memory.h"
#include "filesystem.h"

#include "../app_def.h"



#define MAX_INFO_STRING     1024
#define MAX_INFO_KEY        1024
#define MAX_INFO_VALUE      1024

#define BIG_INFO_STRING     8192  // used for system info key only
#define BIG_INFO_KEY        8192
#define BIG_INFO_VALUE      8192

//============================================================================

typedef struct sizebuf_s {
    _boolean    allowoverflow;  // if false, do a Com_Error
    _boolean    overflowed;     // set to true if the buffer size failed
    W8  *data;
    int     maxsize;
    int     cursize;
    int     readcount;

} sizebuf_t;

void SZ_Init (sizebuf_t *buf, PW8 data, int length);
void SZ_Clear (sizebuf_t *buf);
void *SZ_GetSpace (sizebuf_t *buf, int length);
void SZ_Write (sizebuf_t *buf, void *data, int length);
void SZ_Print (sizebuf_t *buf, W8 *data);    // memcpy onto the sizebuf

//============================================================================

char *Com_StringContains (char *str1, char *str2, int casesensitive);
int Com_Filter (char *filter, char *name, int casesensitive);


int COM_Argc (void);
char *COM_Argv (int arg);    // range and null checked
void COM_ClearArgv (int arg);
int COM_CheckParm (char *parm);
void COM_AddParm (char *parm);

void COM_Init (void);
void COM_InitArgv (int argc, char *argv[]);


//============================================================================




/////////////////////////////////////////////////////////////////////
//
//  Colour
//
/////////////////////////////////////////////////////////////////////

extern colour3_t    colourBlack;
extern colour3_t    colourRed;
extern colour3_t    colourBlue;
extern colour3_t    colourGreen;
extern colour3_t    colourWhite;


/////////////////////////////////////////////////////////////////////
//  End Colour
/////////////////////////////////////////////////////////////////////




/*
==============================================================

    MISC

==============================================================
*/


#define ERR_FATAL       0       // exit the entire game with a popup window
#define ERR_DROP        1       // print to console and disconnect from game
#define ERR_QUIT        2       // not an error, just a normal exit
#define ERR_DISCONNECT  4       // don't kill server

#define EXEC_NOW    0       // don't return until completed
#define EXEC_INSERT 1       // insert at current position, but don't run yet
#define EXEC_APPEND 2       // add to end of the command buffer

#define PRINT_ALL       0
#define PRINT_DEVELOPER 1   // only print when "developer 1"

void Com_Printf (const char *fmt, ...);
void Com_DPrintf (const char *fmt, ...);
void Com_Error (int code, const char *fmt, ...);
void Com_Quit (void);

extern  cvar_t  *developer;
extern  cvar_t  *dedicated;
extern  cvar_t  *host_speeds;
extern  cvar_t  *log_stats;

extern  FILE *log_stats_file;

// host_speeds times
extern  int     time_before_game;
extern  int     time_after_game;
extern  int     time_before_ref;
extern  int     time_after_ref;



void common_Init (int argc, char *argv[]);
void common_Frame (int msec);




/*
==============================================================

    NON-PORTABLE SYSTEM SERVICES

==============================================================
*/
char    *Sys_ConsoleInput (void);
void    Sys_ConsoleOutput (const char *string);
void    Sys_SendKeyEvents (void);
void    Sys_Error (const char *format, ...);
void    Sys_Quit (void);
char    *Sys_GetClipboardData (void);
void    Sys_CopyProtect (void);


/*
==============================================================

    CLIENT / SERVER SYSTEMS

==============================================================
*/
void Client_Init (void);
void Client_Drop (void);
void Client_Shutdown (void);
void Client_Frame (int msec);
void SCR_BeginLoadingPlaque (void);

void Server_Init (void);

void Server_Frame (int msec);





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

