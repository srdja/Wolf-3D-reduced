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
 * @file client_input.h
 * @brief Client System
 * @author Michael Liebscher
 * @date 2004-2013
 * @author Id Software, Inc.
 * @date 1997-2001
 * @note This code was derived from Quake II, and was originally written by Id Software, Inc.
 */

#ifndef __CLIENT_H__
#define __CLIENT_H__

#include "cvar.h"
#include "common.h"

typedef enum menuState {
    IPM_GAME,
    IPM_MAIN,
    IPM_AUTOMAP,
} menuState_t;

// sound attenuation values
#define ATTN_NONE       0   // full volume the entire level
#define ATTN_NORM       1
#define ATTN_STATIC     3   // diminish very rapidly with distance
#define CMD_BACKUP      64  // allow a lot of command backups for very fast systems

//
// the client_state_t structure is wiped completely at every
// server map change
//
typedef struct {
    int     timeoutcount;
    int     timedemo_frames;
    int     timedemo_start;

    _boolean  refresh_prepped;    // false if on new level or new ref dll

    usercmd_t   cmd;
    usercmd_t   cmds[ CMD_BACKUP ]; // each mesage will send several old cmds
    int         cmd_time[ CMD_BACKUP ]; // time sent, for calculating pings

    vec3_t  viewangles;

    int time;           // this is the time value that the client
} client_state_t;

extern  client_state_t  ClientState;

typedef enum { key_console, key_game, key_message, key_menu, KEY_AUTOMAP} keydest_t;


typedef enum {
    ca_uninitialized,
    ca_disconnected,    // not talking to a server
    ca_connecting,      // sending request packets to the server
    ca_active       // game views should be displayed
} connstate_t;

///////////////////
//
//  Global cvars
//
///////////////////

extern  cvar_t  *cl_forwardspeed;
extern  cvar_t  *cl_sidespeed;

extern  cvar_t  *cl_yawspeed;
extern  cvar_t  *cl_pitchspeed;

extern  cvar_t  *cl_run;
extern  cvar_t  *cl_anglespeedkey;
extern  cvar_t  *lookspring;
extern  cvar_t  *sensitivity;

extern  cvar_t  *m_pitch;
extern  cvar_t  *m_forward;

extern  cvar_t  *freelook;

extern  cvar_t *lookstrafe;
extern  cvar_t *m_yaw, *m_side;


///////////////////
//
//  client_input
//
///////////////////
typedef struct {
    int     down[ 2 ];      // key nums holding it down
    unsigned    downtime;       // msec timestamp
    unsigned    msec;           // msec down this frame
    int     state;
} kbutton_t;

extern kbutton_t in_left, in_right, in_forward, in_back;
extern kbutton_t in_moveleft, in_moveright;
extern kbutton_t in_strafe, in_speed, in_use, in_attack;

void Client_InitInput (void);
void Client_SendCommand (void);

void IN_CenterView (void);


///////////////////
//
//  client_main
//
///////////////////

typedef struct {
    connstate_t state;
    keydest_t   key_dest;

    menuState_t  menuState;

    int     framecount;
    int     realtime;           // always increasing, no clamping, etc
    float   frametime;          // seconds since last frame

    float   disable_screen;    // showing loading plaque between levels
} client_static_t;


extern client_static_t  ClientStatic;


void Client_Init (void);
void Client_Quit_f (void);

void automap_keydown (int key);

///////////////////
//
//  client_screen
//
///////////////////

void Client_Screen_Init (void);
void Client_Screen_RunConsole (void);
void Client_Screen_DrawConsole (void);
void Client_Screen_UpdateScreen (void);

///////////////////
//
//  Menus
//
///////////////////
void Menu_Init (void);
void M_Keydown (int key);
void M_Draw (void);
void M_Menu_Main_f (void);
void M_ForceMenuOff (void);


#endif /* __CLIENT_H__ */
