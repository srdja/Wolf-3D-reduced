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

#include "../common.h"
#include <stdbool.h>

typedef enum menuState {
    IPM_GAME,
    IPM_MAIN,
    IPM_AUTOMAP,
} menuState_t;

typedef struct player_state_s {
    bool is_using;
    bool is_attacking;

    /* Amout of movement along x and y axis per update */
    short move_x;
    short move_y;

    /* view angle */
    short angles[3];
} PlayerState;

typedef struct menu_state_s {

} MenuState;

//
// the client_state_t structure is wiped completely at every
// server map change
//
typedef struct {
    usercmd_t   cmd;
    vec3_t  viewangles;
    int time;           // this is the time value that the client
} client_state_t;

extern  client_state_t ClientState;

typedef enum {key_console, key_game, key_menu, KEY_AUTOMAP} keydest_t;

///////////////////
//
//  Global cvars
//
///////////////////

extern float cl_forwardspeed;
extern float cl_sidespeed;
extern float cl_yawspeed;
extern float sensitivity;
extern float m_forward;
extern float m_yaw;

typedef struct {
    int         down[ 2 ];      // key nums holding it down
    unsigned    downtime;       // msec timestamp
    unsigned    msec;           // msec down this frame
    int         state;
} kbutton_t;

typedef struct {
    PlayerState player;
    MenuState   menu;

    bool is_menu_active;

    keydest_t   key_dest;

    menuState_t  menuState;

    int     framecount;
    int     realtime;           // always increasing, no clamping, etc
    float   frametime;          // seconds since last frame

    float   disable_screen;    // showing loading plaque between levels
} client_static_t;

extern client_static_t  ClientStatic;

void Client_Init (void);
void Client_Screen_UpdateScreen (void);

void Menu_Init (void);
void M_Draw (void);
void M_Menu_Main_f (void);
void M_ForceMenuOff (void);


#endif /* __CLIENT_H__ */
