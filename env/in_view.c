/*

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
 * \file in_view.c
 * \brief Interface to mouse / joystick
 * \author Id Software, Inc.
 * \date 1997-2001
 */

#include "client.h"



_boolean	mouseinitialized;
int			mouse_oldbuttonstate;
_boolean	mouseactive;	// false when not focus app
int mx_accum;
int my_accum;

_boolean	in_appactive;

_boolean	mlooking;

cvar_t	*in_mouse;



extern void IN_StartupMouse( void );
extern void IN_ActivateMouse( void );
extern void IN_DeactivateMouse( void );
extern void IN_MouseMove( usercmd_t *cmd );


extern void IN_StartupJoystick( void );
extern void IN_JoyMove( usercmd_t *cmd );

/*
=========================================================================

VIEW CENTERING

=========================================================================
*/

cvar_t	*v_centermove;
cvar_t	*v_centerspeed;

/**
 * \brief Initialize mouse and joy-stick.
 */
void IN_Init( void )
{
	// centering
	v_centermove  = Cvar_Get( "v_centermove",	"0.15",		0);
	v_centerspeed = Cvar_Get( "v_centerspeed",	"500",		0);		

	IN_StartupMouse();
	IN_StartupJoystick();
}

/**
 * \brief Shutdown input layer.
 */
void IN_Shutdown( void )
{
	IN_DeactivateMouse();
}

/**
 * \brief Is the application still active?
 * \pararm[in] active true if application is active, otherwise false.
 * \note Called when the main window gains or loses focus.
 *	The window may have been destroyed and recreated
 *	between a deactivate and an activate.
 */
void IN_Activate( _boolean active )
{
	in_appactive = active;
	mouseactive = ! active;		// force a new window check or turn off
}

/**
 * \brief Called every frame, even if not generating commands.
 */
void IN_Frame( void )
{
	if( ! mouseinitialized )
	{
		return;
	}

	if( ! in_mouse || ! in_appactive )
	{
		IN_DeactivateMouse();
		return;
	}

	if( ! ClientState.refresh_prepped
		|| ClientStatic.key_dest == key_console
		|| ClientStatic.key_dest == key_menu)
	{
		// temporarily deactivate if in fullscreen
		if( Cvar_VariableValue( "r_fullscreen" ) == 0 )
		{
			IN_DeactivateMouse ();
			return;
		}
	}

	IN_ActivateMouse ();
}

/**
 * \brief Apply mouse and joy-stick movements to movement command.
 * \param[in] cmd Valid pointer to usercmd_t structure.
 */
void IN_Move( usercmd_t *cmd )
{
	IN_MouseMove( cmd );

/*	if( ActiveApp )
		IN_JoyMove( cmd );*/
}

/**
 * \brief Clear input states.
 */
void IN_ClearStates( void )
{
	mx_accum = 0;
	my_accum = 0;
	mouse_oldbuttonstate = 0;
}
