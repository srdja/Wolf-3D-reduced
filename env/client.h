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
    IPM_INTRO,
    IPM_MAIN,
    IPM_SKILL,
    IPM_EPISODE,
    IPM_MAPS,
    IPM_MAPSELECTOR, //gsh
    IPM_CONTROLS,
    IPM_INTERMISSION,
    IPM_VICTORY,
    IPM_AUTOMAP,
    IPM_STATS,
    IPM_HUDEDIT,
    IPM_DOWNLOADPROGRESS, //gsh
    IPM_STOREKIT, //gsh
//  IPM_LOADING, //gsh
    IPM_TRIVIA, //gsh
    IPM_MORE, //gsh
    IPM_DOWNLOADINSTRUCTIONS, //gsh

} menuState_t;



// sound attenuation values
#define ATTN_NONE       0   // full volume the entire level
#define ATTN_NORM       1
#define ATTN_IDLE       2
#define ATTN_STATIC     3   // diminish very rapidly with distance


#define MAX_SOUNDS      256 // so they cannot be blindly increased




#define MAX_CONFIGSTRINGS   1024

#define MAX_ITEMS       256



#define CMD_BACKUP      64  // allow a lot of command backups for very fast systems

//
// the client_state_t structure is wiped completely at every
// server map change
//
typedef struct {
    int     timeoutcount;

    int     timedemo_frames;
    int     timedemo_start;

    _boolean    refresh_prepped;    // false if on new level or new ref dll
    _boolean    sound_prepped;      // ambient sounds can start
    _boolean    force_refdef;       // vid has changed, so we can't use a paused refdef

    int     parse_entities;     // index (not anded off) into cl_parse_entities[]

    usercmd_t   cmd;
    usercmd_t   cmds[ CMD_BACKUP ]; // each mesage will send several old cmds
    int     cmd_time[ CMD_BACKUP ]; // time sent, for calculating pings
    short       predicted_origins[ CMD_BACKUP ][ 3 ];   // for debug comparing against server

    float       predicted_step;     // for stair up smoothing
    unsigned    predicted_step_time;

    vec3_t      predicted_origin;   // generated by CL_PredictMovement
    vec3_t      predicted_angles;
    vec3_t      prediction_error;

//  frame_t     frame;          // received from server
//  int     surpressCount;      // number of messages rate supressed
//  frame_t     frames[UPDATE_BACKUP];

    // the client maintains its own idea of view angles, which are
    // sent to the server each frame.  It is cleared to 0 upon entering each level.
    // the server sends a delta each frame which is added to the locally
    // tracked view angles to account for standing on rotating objects,
    // and teleport direction changes
    vec3_t      viewangles;

    int     time;           // this is the time value that the client
    // is rendering at.  always <= cls.realtime
    float       lerpfrac;       // between oldframe and frame

//  refdef_t    refdef;

    vec3_t      v_forward, v_right, v_up;   // set when refdef.angles is set

    //
    // transient data from server
    //
    char        layout[ 1024 ];     // general 2D overlay
    int     inventory[ MAX_ITEMS ];

    //
    // non-gameserver infornamtion


    //
    // server state information
    //
    _boolean    attractloop;        // running the attract loop, any key will menu
    int     servercount;        // server identification for prespawns
    char        gamedir[ MAX_GAMEPATH ];
    int     playernum;

    char        configstrings[ MAX_CONFIGSTRINGS ][ MAX_GAMEPATH ];

    //
    // locally derived information from server state
    //
//  struct model_s  *model_draw[MAX_MODELS];
//  struct cmodel_s *model_clip[MAX_MODELS];

//  struct sfx_s    *sound_precache[MAX_SOUNDS];
//  struct image_s  *image_precache[MAX_IMAGES];

//  clientinfo_t    clientinfo[MAX_CLIENTS];
//  clientinfo_t    baseclientinfo;

} client_state_t;


extern  client_state_t  ClientState;



typedef enum { key_console, key_game, key_message, key_menu, KEY_AUTOMAP, KEY_LEVEL_EDITOR } keydest_t;


typedef enum {
    ca_uninitialized,
    ca_disconnected,    // not talking to a server
    ca_connecting,      // sending request packets to the server
    ca_connected,       // netchan_t established, waiting for svc_serverdata
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

extern  cvar_t *cl_name, *cl_color;
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
    float       frametime;          // seconds since last frame

// screen rendering information
    float       disable_screen;     // showing loading plaque between levels
    // or changing rendering dlls
    // if time gets > 30 seconds ahead, break it

    int     disable_servercount;    // when we receive a frame and cl.servercount
    // > cls.disable_servercount, clear disable_screen

// connection information
    char        servername[ MAX_OSPATH ];   // name of server from original connect
    float       connect_time;       // for connection retransmits

    int     quakePort;          // a 16 bit value that allows quake servers
    // to work around address translating routers
//  netchan_t   netchan;
    int     serverProtocol;         // in case we are doing some kind of version hack

    int     challenge;          // from the server to use for connecting

    FILE        *download;          // file transfer from server
    char        downloadtempname[MAX_OSPATH];
    char        downloadname[MAX_OSPATH];
    int     downloadnumber;
//  dltype_t    downloadtype;
    int     downloadpercent;

// demo recording info must be here, so it isn't cleared on level change
    _boolean    demorecording;
    _boolean    demowaiting;    // don't record until a non-delta message is received
    FILE        *demofile;

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
