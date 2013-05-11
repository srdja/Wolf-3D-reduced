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
 * \file win_mouse.c
 * \brief Windows mouse input layer.
 * \author Id Software, Inc.
 * \date 1997-2001
 * \note This code was derived from Quake II, and was originally written by Id Software, Inc.
 */

#ifndef _WIN32

	#error "Do not compile this file on this platform."

#endif

#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>

#include "../env/client.h"
#include "../env/keys.h"
#include "../env/keycodes.h"


// mouse variables
extern	unsigned	sys_msg_time;

extern cvar_t	*in_mouse;
cvar_t	*m_filter;

extern _boolean	mlooking;

int			mouse_buttons;
extern int	mouse_oldbuttonstate;
POINT		current_pos;
int			mouse_x, mouse_y, old_mouse_x, old_mouse_y, mx_accum, com_accum;

int			old_x, old_y;

extern _boolean	mouseactive;	// false when not focus app

_boolean	restore_spi;
extern _boolean	mouseinitialized;
int		originalmouseparms[3], newmouseparms[3] = {0, 0, 1};
_boolean	mouseparmsvalid;

int			window_center_x, window_center_y;
RECT		window_rect;

extern HWND cl_hwnd;



void IN_MouseLookDown( void ) 
{ 
	mlooking = true; 
}

void IN_MouseLookUp( void ) 
{
	mlooking = false;
	if( ! freelook->value && lookspring->value )
	{
		IN_CenterView();
	}
}

/*
-----------------------------------------------------------------------------
 Function: IN_ActivateMouse
 
 Parameters: Nothing
 
 Returns:	Nothing 
 
 Notes: Called when the window gains focus or changes in some way
 
-----------------------------------------------------------------------------
*/
PUBLIC void IN_ActivateMouse (void)
{
	int	width, height;

	if( ! mouseinitialized )
		return;
	
	if( ! in_mouse->value )
	{
		mouseactive = false;
		return;
	}
	
	if( mouseactive )
		return;

	mouseactive = true;

	if( mouseparmsvalid )
		restore_spi = SystemParametersInfo( SPI_SETMOUSE, 0, newmouseparms, 0 );

	width = GetSystemMetrics( SM_CXSCREEN );
	height = GetSystemMetrics( SM_CYSCREEN );

	GetWindowRect( cl_hwnd, &window_rect );
	if( window_rect.left < 0 )
		window_rect.left = 0;
	
	if (window_rect.top < 0)
		window_rect.top = 0;
	
	if (window_rect.right >= width)
		window_rect.right = width-1;
	
	if (window_rect.bottom >= height-1)
		window_rect.bottom = height-1;

	window_center_x = (window_rect.right + window_rect.left) >> 1;
	window_center_y = (window_rect.top + window_rect.bottom) >> 1;

	SetCursorPos( window_center_x, window_center_y );

	old_x = window_center_x;
	old_y = window_center_y;

	SetCapture( cl_hwnd );
	ClipCursor( &window_rect );
	while( ShowCursor( false ) >= 0 )
		;
}


/*
-----------------------------------------------------------------------------
 Function: IN_DeactivateMouse
 
 Parameters: Nothing.
 
 Returns:	Nothing.
 
 Notes: Called when the window loses focus.
 
-----------------------------------------------------------------------------
*/
PUBLIC void IN_DeactivateMouse (void)
{
	if( ! mouseinitialized )
		return;
	
	if( ! mouseactive )
		return;

	if( restore_spi )
		SystemParametersInfo( SPI_SETMOUSE, 0, originalmouseparms, 0 );

	mouseactive = false;

	ClipCursor( NULL );
	ReleaseCapture();
	while( ShowCursor( true ) < 0 )
		;
}

//-----------------------------------------------------------------------------
// Function: IN_StartupMouse
// Parameters: Nothing
// Returns:	Nothing 
// Notes: 
//-----------------------------------------------------------------------------
PUBLIC void IN_StartupMouse (void)
{
	cvar_t	*cv;

	// mouse variables
	m_filter = Cvar_Get( "m_filter", "0", CVAR_INIT );
    in_mouse = Cvar_Get( "in_mouse", "1", CVAR_ARCHIVE|CVAR_LATCH );

	Cmd_AddCommand( "+mlook", IN_MouseLookDown );
	Cmd_AddCommand( "-mlook", IN_MouseLookUp );

	// abort startup if user requests no mouse
	cv = Cvar_Get( "in_initmouse", "1", CVAR_ROM );
	if ( ! cv->value ) 
	{
		return; 
	}

	mouseinitialized = true;
	mouseparmsvalid = SystemParametersInfo( SPI_GETMOUSE, 0, originalmouseparms, 0 );
	mouse_buttons = 5;
	
}

/*
===========
IN_MouseEvent
===========
*/
PUBLIC void IN_MouseEvent( int mstate )
{
	int		i;

	if( ! mouseinitialized )
		return;

// perform button actions
	for( i = 0 ; i < mouse_buttons ; ++i )
	{
		if ( (mstate & (1<<i)) && !(mouse_oldbuttonstate & (1<<i)) )
		{
			Key_Event( K_MOUSE1 + i, true, sys_msg_time );
		}

		if ( ! (mstate & (1<<i)) && (mouse_oldbuttonstate & (1<<i)) )
		{
				Key_Event (K_MOUSE1 + i, false, sys_msg_time);
		}
	}	
		
	mouse_oldbuttonstate = mstate;
}



/*
===========
IN_MouseMove
===========
*/
PUBLIC void IN_MouseMove( usercmd_t *cmd )
{
	int		mx, my;

	if( ! mouseactive )
		return;

	// find mouse movement
	if( ! GetCursorPos( &current_pos ) )
		return;

	mx = current_pos.x - window_center_x;
	my = current_pos.y - window_center_y;

#if 0

	if( ! mx && ! my )
		return;

#endif

	if( m_filter->value )
	{
		mouse_x = (mx + old_mouse_x) / 2;
		mouse_y = (my + old_mouse_y) / 2;
	}
	else
	{
		mouse_x = mx;
		mouse_y = my;
	}

	old_mouse_x = mx;
	old_mouse_y = my;

	mouse_x = (int)(mouse_x * sensitivity->value);
	mouse_y = (int)(mouse_y * sensitivity->value);

// add mouse X/Y movement to cmd
	if( (in_strafe.state & 1) || lookstrafe->value )
		cmd->sidemove += (short)(m_side->value * mouse_x);
	else
		ClientState.viewangles[ YAW ] -= m_yaw->value * mouse_x;

	if( (mlooking || freelook->value) && ! (in_strafe.state & 1) )
	{
		ClientState.viewangles[ PITCH ] += m_pitch->value * mouse_y;
	}
	else
	{
		cmd->forwardmove -= (short)(m_forward->value * mouse_y);
	}
	

	// force the mouse to the center, so there's room to move
	if( mx || my )
	{
		SetCursorPos( window_center_x, window_center_y );
	}
	
}

