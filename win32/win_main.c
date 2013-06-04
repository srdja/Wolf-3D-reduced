/*

	Copyright (C) 2004-2005 Michael Liebscher <johnnycanuck@users.sourceforge.net>
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
 * \file win_main.c
 * \brief Windows interface to application.
 * \author Michael Liebscher
 * \date 2005-2012
 * \author Id Software, Inc.
 * \date 1997-2001
 * \note This code was derived from Quake II, and was originally written by Id Software, Inc.
 */

#ifndef _WIN32

	#error "Do not compile this file on this platform"

#endif

#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>
#include <mmsystem.h>

#include "../env/common.h"
#include "../env/memory.h"
#include "../env/timer.h"
#include "com_string.h"


HINSTANCE	global_hInstance;


_boolean	ActiveApp;
_boolean	Minimized;


static HANDLE hinput, houtput;

unsigned	sys_msg_time;
W32	sys_frame_time;

static HANDLE hMutex;


#define	MAX_NUM_ARGVS	128

int	argc;
char *argv[ MAX_NUM_ARGVS ];

PRIVATE char sys_cmdline[ MAX_STRING_CHARS ];


/**
 * \brief Terminate the application due to error.
 * \param[in] format Pointer to null-terminated string.
 * \param[in] ... Optional arguments
 * \return
 * \note Shutdown Application due to error. See format specification fields.
 *      1.  Terminate Client Sub-systems.
 *		2.  Display error message in MessageBox.
 *		3.  Exit application.
 */
void Sys_Error( const char *format, ... )
{
	va_list	argptr;
	char	text[ 1024 ];

	Client_Shutdown();

	va_start( argptr, format );
	(void)vsnprintf( text, sizeof( text ), format, argptr );
	va_end( argptr );

	text[ sizeof( text ) - 1 ] = '\0';

	MessageBox( NULL, text, "Error", MB_OK );

	if( hMutex )
	{
		CloseHandle( hMutex );
	}

	exit( 1 );
}

/**
 * \brief Terminate the application after cleanup.
 * \note Shutdown Application.
 *  1.  Terminate Client Sub-systems.
 *  2.  Exit application.
 */
void Sys_Quit( void )
{
	timeEndPeriod( 1 );

	Client_Shutdown();

	if( hMutex )
	{
		CloseHandle( hMutex );
	}


	if( dedicated && dedicated->value )
	{
		FreeConsole();
	}

	exit( 0 );
}



static char	console_text[ 256 ];
static int	console_textlen;

/**
 * \brief Console input for dedicated server.
 * \return NULL or pointer to string with console input.
 */
char *Sys_ConsoleInput( void )
{
	INPUT_RECORD	recs[1024];
	LPDWORD	dummy;
	int	ch;
	LPDWORD numevents, numread;

	if( ! dedicated || ! dedicated->value )
		return NULL;


	for( ;; )
	{
		if( ! GetNumberOfConsoleInputEvents( hinput, &numevents ) )
			Sys_Error( "Error getting # of console events" );

		if( numevents <= 0 )
			break;

		if( ! ReadConsoleInput( hinput, recs, 1, &numread ) )
			Sys_Error ("Error reading console input");

		if( numread != 1 )
			Sys_Error ("Couldn't read console input");

		if( recs[0].EventType == KEY_EVENT )
		{
			if( ! recs[0].Event.KeyEvent.bKeyDown )
			{
				ch = recs[0].Event.KeyEvent.uChar.AsciiChar;

				switch( ch )
				{
					case '\r':
						WriteFile( houtput, "\r\n", 2, &dummy, NULL );

						if( console_textlen )
						{
							console_text[ console_textlen ] = '\0';
							console_textlen = 0;
							return console_text;
						}
						break;

					case '\b':
						if( console_textlen )
						{
							console_textlen--;
							WriteFile( houtput, "\b \b", 3, &dummy, NULL );
						}
						break;

					default:
						if( ch >= ' ' )
						{
							if( console_textlen < sizeof( console_text ) - 2 )
							{
								WriteFile( houtput, &ch, 1, &dummy, NULL );
								console_text[ console_textlen ] = ch;
								console_textlen++;
							}
						}

						break;

				} // end switch ch

			} //  end if ! recs[0].Event.KeyEvent.bKeyDown

		} // end if recs[0].EventType == KEY_EVENT

	} // end for ;;

	return NULL;
}

/**
 * \brief Send Key_Event calls.
 */
void Sys_SendKeyEvents( void )
{
    MSG msg;

	while( PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) )
	{
		if( ! GetMessage( &msg, NULL, 0, 0 ) )
		{
			Sys_Quit();
		}
		sys_msg_time = msg.time;
      	TranslateMessage( &msg );
      	DispatchMessage( &msg );
	}

	// grab frame time
	sys_frame_time = timeGetTime();	// FIXME: should this be at start?
}

/**
 * \brief Retrieves data from the clipboard in a specified format. The clipboard must have been opened previously.
 * \return Returns a string Text format.
 * \note Caller is responsible for freeing data by calling MM_FREE
 */
char *Sys_GetClipboardData( void )
{
	char *data = NULL;
	char *cliptext;

	if( OpenClipboard( NULL ) != 0 )
	{
		HANDLE hClipboardData;

		if( ( hClipboardData = GetClipboardData( CF_TEXT ) ) != 0 )
		{
			if( ( cliptext = GlobalLock( hClipboardData ) ) != 0 )
			{
				data = (char *)MM_MALLOC( GlobalSize( hClipboardData ) + 1 );
				strncpy( data, cliptext, GlobalSize( hClipboardData ) );
				GlobalUnlock( hClipboardData );

				strtok( data, "\n\r\b" );
			}
		}
		CloseClipboard();
	}
	return data;
}

/**
 * \brief Check to stop multiple instances of application from executing.
 */
void CheckforInstance( void )
{
	// Create a mutex object on the client to stop
	//  multiple instances of application from running.
	hMutex = CreateMutex( NULL, 0, "wolf3dredux" );

	if( ! hMutex || (GetLastError() == ERROR_ALREADY_EXISTS) )
	{
		Sys_Error( "Application is already running on this system" );
	}
}

void ParseCommandLine( LPSTR lpCmdLine )
{
	argc = 1;
	argv[ 0 ] = "exe";

	while( *lpCmdLine && (argc < MAX_NUM_ARGVS) )
	{
		// Ignore ASCII characters outside the range of '!' and '}'
		while( *lpCmdLine && ( (*lpCmdLine < 33) || (*lpCmdLine > 126) ) )
		{
			++lpCmdLine;
		}

		if( *lpCmdLine )
		{
			argv[ argc ] = lpCmdLine;
			argc++;

			// Keep ASCII characters within the range of '!' and '}'
			while( *lpCmdLine && ( (*lpCmdLine > 32) && (*lpCmdLine < 127) ) )
			{
				++lpCmdLine;
			}

			if( *lpCmdLine )
			{
				*lpCmdLine = '\0';	// NUL-terminate string
				++lpCmdLine;
			}
		}
	}
}

/**
 * \brief The user-provided entry point for a graphical Windows-based application.
 * \param[in] hInstance A handle to the current instance of the application.
 * \param[in] hPrevInstance A handle to the previous instance of the application. This parameter is always NULL.
 * \param[in] lpCmdLine The command line for the application, excluding the program name.
 * \param[in] nCmdShow Controls how the window is to be shown.
 * \return If the function succeeds, terminating when it receives a WM_QUIT message, it should return the exit value contained in that message's wParam parameter. If the function terminates before entering the message loop, it should return zero.
 */
int WINAPI WinMain( HINSTANCE hInstance,
				    HINSTANCE hPrevInstance,
					LPSTR lpCmdLine,
					int nCmdShow )
{
	MSG	 msg;
	int	 time, oldtime, newtime;


	global_hInstance = hInstance;

	CheckforInstance();

//    com_strlcpy( sys_cmdline, lpCmdLine, sizeof( sys_cmdline ) );

	ParseCommandLine( lpCmdLine );

	common_Init( argc, argv );

	oldtime = Sys_Milliseconds();

	/* main window message loop */
	while( 1 )
	{
		// if at a full screen console, don't update unless needed
		if( Minimized || (dedicated && dedicated->value) )
		{
			Sleep( 1 );
		}

		while( PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) )
		{
			if( ! GetMessage( &msg, NULL, 0, 0 ) )
			{
				Com_Quit();
			}
			sys_msg_time = msg.time;

			TranslateMessage( &msg );
   			DispatchMessage( &msg );
		}

		do
		{
			newtime = Sys_Milliseconds();
			time = newtime - oldtime;

		} while( time < 1 );


		common_Frame( time );

		oldtime = newtime;

	}

	// Should never get here!
	return msg.wParam;

}

