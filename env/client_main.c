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
 * \file client_main.c
 * \brief Client main loop.
 * \author Michael Liebscher
 * \date 2004
 * \author Id Software, Inc.
 * \date 1997-2001
 */

#include <stdio.h>
#include <stdlib.h>

#include "client.h"
#include "console.h"
#include "keys.h"
#include "video.h"
#include "timer.h"
#include "input.h"
#include "system.h"
#include "com_string.h"
#include "sound.h"
#include "../app_def.h"

cvar_t	*freelook;

cvar_t	*adr0;
cvar_t	*adr1;
cvar_t	*adr2;
cvar_t	*adr3;
cvar_t	*adr4;
cvar_t	*adr5;
cvar_t	*adr6;
cvar_t	*adr7;
cvar_t	*adr8;


cvar_t	*rcon_client_password;
cvar_t	*rcon_address;

cvar_t	*cl_noskins;
cvar_t	*cl_autoskins;
cvar_t	*cl_footsteps;
cvar_t	*cl_timeout;
cvar_t	*cl_predict;
//cvar_t	*cl_minfps;
cvar_t	*cl_maxfps;
cvar_t	*cl_gun;

cvar_t	*cl_add_particles;
cvar_t	*cl_add_lights;
cvar_t	*cl_add_entities;
cvar_t	*cl_add_blend;

cvar_t	*cl_shownet;
cvar_t	*cl_showmiss;
cvar_t	*cl_showclamp;

cvar_t	*cl_paused;
cvar_t	*cl_timedemo;

cvar_t	*lookspring;
cvar_t	*lookstrafe;
cvar_t	*sensitivity;

cvar_t	*m_pitch;
cvar_t	*m_yaw;
cvar_t	*m_forward;
cvar_t	*m_side;

cvar_t	*cl_lightlevel;

//
// userinfo
//
cvar_t	*info_password;
cvar_t	*info_spectator;
cvar_t	*name;
cvar_t	*skin;
cvar_t	*rate;
cvar_t	*fov;
cvar_t	*msg;
cvar_t	*hand;
cvar_t	*gender;
cvar_t	*gender_auto;

cvar_t	*cl_vwep;

cvar_t	*cl_drawfps;


client_state_t	ClientState;



cvar_t *cl_name, *cl_color;




client_static_t	ClientStatic;


/**
 * \brief Disconnect the client from the server.
 * \note Called after an ERR_DROP was thrown.
 */
PUBLIC void Client_Drop( void )
{
/*
	if( ClientStatic.state == ca_uninitialized )
		return;

	if( ClientStatic.state == ca_disconnected )
		return;

	CL_Disconnect();

	// drop loading plaque unless this is the initial game start
	if( ClientStatic.disable_servercount != -1 )
		SCR_EndLoadingPlaque();	// get rid of loading plaque
*/
}

/**
 * \brief Console command to disconnect client from server.
 */
PUBLIC void Client_Quit_f( void )
{
//	CL_Disconnect();
	Com_Quit();
}

/**
 * \brief Initialize client variables and commands.
 */
PRIVATE void Client_InitLocal( void )
{
	ClientStatic.state = ca_disconnected;
	ClientStatic.realtime = Sys_Milliseconds();


	Client_InitInput();


//
// register our variables
//
//	adr0 = Cvar_Get( "adr0", "", CVAR_ARCHIVE );
//	adr1 = Cvar_Get( "adr1", "", CVAR_ARCHIVE );
//	adr2 = Cvar_Get( "adr2", "", CVAR_ARCHIVE );
//	adr3 = Cvar_Get( "adr3", "", CVAR_ARCHIVE );
//	adr4 = Cvar_Get( "adr4", "", CVAR_ARCHIVE );
//	adr5 = Cvar_Get( "adr5", "", CVAR_ARCHIVE );
//	adr6 = Cvar_Get( "adr6", "", CVAR_ARCHIVE );
//	adr7 = Cvar_Get( "adr7", "", CVAR_ARCHIVE );
//	adr8 = Cvar_Get( "adr8", "", CVAR_ARCHIVE );

	cl_upspeed		= Cvar_Get( "cl_upspeed", "200", CVAR_INIT );
	cl_forwardspeed = Cvar_Get( "cl_forwardspeed", "4000", CVAR_INIT );
	cl_sidespeed	= Cvar_Get( "cl_sidespeed", "4000", CVAR_INIT );
	cl_yawspeed		= Cvar_Get( "cl_yawspeed", "100", CVAR_INIT );
	cl_pitchspeed	= Cvar_Get( "cl_pitchspeed", "100", CVAR_INIT );
	cl_anglespeedkey= Cvar_Get( "cl_anglespeedkey", "2", CVAR_INIT );


//	cl_drawfps = Cvar_Get( "cl_drawfps", "0", CVAR_ARCHIVE );


	cl_run		= Cvar_Get( "cl_run", "0", CVAR_ARCHIVE );
	freelook	= Cvar_Get( "freelook", "0", CVAR_ARCHIVE );
	lookspring	= Cvar_Get( "lookspring", "0", CVAR_ARCHIVE );
	lookstrafe	= Cvar_Get( "lookstrafe", "0", CVAR_ARCHIVE );
	sensitivity = Cvar_Get( "m_sensitivity", "300", CVAR_ARCHIVE );

	m_pitch = Cvar_Get( "m_pitch", "0.022", CVAR_ARCHIVE );
	m_yaw	= Cvar_Get( "m_yaw", "1", CVAR_INIT );
	m_side	= Cvar_Get( "m_side", "1", CVAR_INIT );
	m_forward = Cvar_Get( "m_forward", "1", CVAR_INIT );

	cl_name = Cvar_Get( "name", "player", CVAR_ARCHIVE );


//
// register our commands
//
	Cmd_AddCommand( "quit", Client_Quit_f );


}

/**
 * \brief Initialize Client sub-systems.
 * \return Initialize video and sound sub-systems.
 */
PUBLIC void Client_Init( void )
{

	if( dedicated->value )
		return;		// nothing running on the client

	// all archived variables will now be loaded

	Con_Init();

	Video_Init();
	Sound_Init();	// sound must be initialized after window is created

	Menu_Init();

	Client_Screen_Init();

	Client_InitLocal();
	IN_Init();

}

/**
 * \brief Writes key bindings and archived cvars to config.cfg.
 */
PRIVATE void Client_WriteConfiguration( void )
{
	FILE	*fp;
	char	path[ MAX_OSPATH];

	if( ClientStatic.state == ca_uninitialized )
	{
		return;
	}

	com_snprintf( path, sizeof( path ), "%s/config.cfg", FS_Gamedir() );
	fp = fopen( path, "w" );
	if( !fp )
	{
		Com_Printf( "Could not write config.cfg.\n" );
		return;
	}

	fprintf( fp, "// Generated by %s, do not modify!\n", GAME_NAME );
	Key_WriteBindings( fp );

	Cvar_WriteVariables( fp );

	fclose( fp );
}

/**
 * \brief Shutdown client sub-systems.
 */
PUBLIC void Client_Shutdown( void )
{
	static _boolean isdown = false;

	if( isdown )
	{
		Com_DPrintf( "recursive shutdown\n" );
		return;
	}
	isdown = true;

	Client_WriteConfiguration();


	Sound_Shutdown();
	IN_Shutdown();
	Video_Shutdown();
}
