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

// Main windowed and fullscreen graphics interface module. This module
// is used for OpenGL rendering.

#include <assert.h>
#include <float.h>

#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>

#include "win_def.h"
#include "../env/client.h"
#include "../env/keys.h"
#include "../env/console.h"
#include "../env/renderer.h"
#include "../env/video.h"
#include "../env/common.h"
#include "../env/common_utils.h"
#include "../env/input.h"
#include "../env/sound.h"


extern cvar_t *r_ref;


extern cvar_t *win_noalttab;





// Console variables that we need to access from this module
cvar_t		*vid_gamma;
cvar_t		*vid_xpos;			// X coordinate of window position
cvar_t		*vid_ypos;			// Y coordinate of window position
cvar_t		*r_fullscreen;

// Global variables used internally by this module
viddef_t	viddef;	// global video state; used by other modules

extern HWND	cl_hwnd;	// Main window handle for life of program

#define VID_NUM_MODES ( sizeof( vid_modes ) / sizeof( vid_modes[ 0 ] ) )

extern LONG WINAPI MainWndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );



extern void WIN_DisableAltTab( void );
extern void WIN_EnableAltTab( void );





//==========================================================================


/*
-----------------------------------------------------------------------------
 Function: VID_Restart_f -Console command to re-start the video mode.
 
 Parameters: Nothing.
 
 Returns: Nothing.
 
 Notes: 
	We do this simply by setting the modified flag for the r_ref variable,
	which will cause the entire video mode to be reset on the next frame.

-----------------------------------------------------------------------------
*/
PRIVATE void VID_Restart_f( void )
{
	r_ref->modified = true;
}

/*
-----------------------------------------------------------------------------
 Function: VID_Front_f -Bring Window to the foreground.
 
 Parameters: Nothing.
 
 Returns: Nothing.
 
 Notes: 
-----------------------------------------------------------------------------
*/
PRIVATE void VID_Front_f( void )
{
	SetWindowLong( cl_hwnd, GWL_EXSTYLE, WS_EX_TOPMOST );
	SetForegroundWindow( cl_hwnd );
}

/*
** VID_GetModeInfo
*/
typedef struct vidmode_s
{
	const char *description;
	int         width, height;
	int         mode;

} vidmode_t;

vidmode_t vid_modes[] =
{
	{ "Mode 0: 640x480",   640,   480, 0 },
	{ "Mode 1: 800x600",   800,   600, 1 },
	{ "Mode 2: 960x720",   960,   720, 2 },
	{ "Mode 3: 1024x768",  1024,  768, 3 },
	{ "Mode 4: 1152x864",  1152,  864, 4 },
	{ "Mode 5: 1280x960",  1280,  960, 5 },
	{ "Mode 6: 1600x1200", 1600, 1200, 6 },
	{ "Mode 7: 2048x1536", 2048, 1536, 7 }
};

/*
-----------------------------------------------------------------------------
 Function: VID_GetModeInfo -Get window mode information.
 
 Parameters: width -[out] window width.
			 height -[out] window height.
			 mode -[in] Which mode window mode?
 
 Returns: True if mode was accepted, otherwise false.
 
 Notes: 
-----------------------------------------------------------------------------
*/
PUBLIC _boolean VID_GetModeInfo( int *width, int *height, int mode )
{
	if( mode < 0 || mode >= VID_NUM_MODES )
	{
		return false;
	}

	*width  = vid_modes[ mode ].width;
	*height = vid_modes[ mode ].height;

	return true;
}

/*
-----------------------------------------------------------------------------
 Function: VID_UpdateWindowPosAndSize -Set window position and size.
 
 Parameters: x -[in] Set window x postion on desktop.
			 y -[in] Set window y postion on desktop.
 
 Returns:	Nothing
 
 Notes: 
-----------------------------------------------------------------------------
*/
PRIVATE void VID_UpdateWindowPosAndSize( int x, int y )
{
	RECT r;
	int		style;
	int		w, h;

	r.left   = 0;
	r.top    = 0;
	r.right  = viddef.width;
	r.bottom = viddef.height;

	style = GetWindowLong( cl_hwnd, GWL_STYLE );
	AdjustWindowRect( &r, style, FALSE );

	w = r.right - r.left;
	h = r.bottom - r.top;

	MoveWindow( cl_hwnd, (int)vid_xpos->value, (int)vid_ypos->value, w, h, TRUE );
}

/*
-----------------------------------------------------------------------------
 Function: VID_NewWindow -Set new window width height.
 
 Parameters: width -[in] width of new window.
			 height -[in] height of new window.
 
 Returns:	Nothing
 
 Notes: Sets global variables [viddef.width] [viddef.height]
-----------------------------------------------------------------------------
*/
PUBLIC void VID_NewWindow( int width, int height )
{
	viddef.width  = width;
	viddef.height = height;

//	ClientState.force_refdef = true;		// can't use a paused refdef
}


/*
-----------------------------------------------------------------------------
 Function: VID_LoadRefresh -Restart the rendering layer.
 
 Parameters: Nothing
 
 Returns: false on error, true on success.
 
 Notes: 
 	This function refreshes the rendering layer.
-----------------------------------------------------------------------------
*/
PRIVATE _boolean VID_LoadRefresh( void )
{
	extern	HINSTANCE	global_hInstance;

	R_Shutdown();

	if( R_Init( global_hInstance, MainWndProc ) == -1 )
	{
		R_Shutdown();
		return false;
	}

	Com_Printf( "------------------------------------\n" );


	return true;
}


/*
-----------------------------------------------------------------------------
 Function: Video_CheckChanges -Check for video mode changes.
 
 Parameters: Nothing
 
 Returns: Nothing
 
 Notes: 
 	This function gets called once just before drawing each frame, and 
	it's sole purpose is to check to see if any of the video mode 
	parameters have changed, and if they have to update the video 
	mode to match.
-----------------------------------------------------------------------------
*/
PUBLIC void Video_CheckChanges( void )
{
	if( win_noalttab->modified )
	{
		if ( win_noalttab->value )
		{
			WIN_DisableAltTab();
		}
		else
		{
			WIN_EnableAltTab();
		}
		win_noalttab->modified = false;
	}

	if ( r_ref->modified )
	{
		ClientState.force_refdef = true;		// can't use a paused refdef
		Sound_StopAllSounds();
	}

	while( r_ref->modified )
	{
		// refresh has changed
		r_ref->modified = false;
		r_fullscreen->modified = true;
		ClientState.refresh_prepped = false;
//		ClientStatic.disable_screen = true;

		
		if( ! VID_LoadRefresh() )
		{
			Com_Error( ERR_FATAL, "Could not start OpenGL module!");
			
			// drop the console if we fail to load a refresh
			if ( ClientStatic.key_dest != key_console )
			{
				Con_ToggleConsole_f();
			}
		}
//		ClientStatic.disable_screen = false;
	}

	// update our window position
	if( vid_xpos->modified || vid_ypos->modified )
	{
		if( ! r_fullscreen->value )
		{
			VID_UpdateWindowPosAndSize( (int)vid_xpos->value, (int)vid_ypos->value );
		}

		vid_xpos->modified = false;
		vid_ypos->modified = false;
	}
}

/*
-----------------------------------------------------------------------------
 Function: Video_Init -Initialize video sub-system.
 
 Parameters: Nothing
 
 Returns:	Nothing
 
 Notes: 
-----------------------------------------------------------------------------
*/
PUBLIC void Video_Init( void )
{
	/* Create the video variables so we know how to start the graphics drivers */
	r_ref = Cvar_Get( "r_ref", "gl", CVAR_ARCHIVE );
	vid_xpos = Cvar_Get( "win_xpos", "3", CVAR_ARCHIVE );
	vid_ypos = Cvar_Get( "win_ypos", "22", CVAR_ARCHIVE );
	r_fullscreen = Cvar_Get( "r_fullscreen", "0", CVAR_ARCHIVE );
	vid_gamma = Cvar_Get( "vid_gamma", "1", CVAR_ARCHIVE );
	win_noalttab = Cvar_Get( "win_noalttab", "0", CVAR_ARCHIVE );

	/* Add some console commands that we want to handle */
	Cmd_AddCommand( "vid_restart", VID_Restart_f );
	Cmd_AddCommand( "vid_front", VID_Front_f );

		
	/* Start the graphics mode */
	Video_CheckChanges();
}

/*
-----------------------------------------------------------------------------
 Function: Video_Shutdown -Shutdown video sub-system.
 
 Parameters: Nothing
 
 Returns:	Nothing
 
 Notes: 
-----------------------------------------------------------------------------
*/
PUBLIC void Video_Shutdown( void )
{
	R_Shutdown();
}


