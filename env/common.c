/*

	Copyright (C) 2004-2012 Michael Liebscher <johnnycanuck@users.sourceforge.net>
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
 * \file common.c
 * \brief Misc functions used in client and server.
 * \author Michael Liebscher
 * \date 2004-2012
 * \author Id Software, Inc.
 * \date 1997-2001
 * \note This code was derived from Quake II, and was originally written by Id Software, Inc.
 */

#include <stdio.h>
#include <stdarg.h>

#include <string.h>

#include "common.h"
#include <setjmp.h>
#include "console.h"
#include "client.h"
#include "com_string.h"
#include "filesystem.h"
#include "timer.h"
#include "system.h"
#include "renderer.h"
#include "zmem.h"
#include "keys.h"
#include "input.h"

#include "../wolf/wolf_local.h"

#include "../app_def.h"





#define	MAXPRINTMSG	4096

#define MAX_NUM_ARGVS	50


colour3_t	colourBlack = {   0,   0,	0 };
colour3_t	colourRed	= { 255,   0,   0 };
colour3_t	colourGreen	= {   0, 255,   0 };
colour3_t	colourBlue	= {   0,   0, 255 };
colour3_t	colourWhite	= { 255, 255, 255 };

#define BUILDSTRING "TEST"

int		com_argc;
char	*com_argv[ MAX_NUM_ARGVS + 1 ];


jmp_buf abortframe;		// an ERR_DROP occured, exit the entire frame


FILE	*log_stats_file;

cvar_t	*host_speeds;
cvar_t	*log_stats;
cvar_t	*developer;
cvar_t	*timescale;
cvar_t	*fixedtime;
cvar_t	*logfile_active;	// 1 = buffer log, 2 = flush after each print
cvar_t	*showtrace;
cvar_t	*dedicated;

FILE	*logfile;

int		server_state;

// host_speeds times
int		time_before_game;
int		time_after_game;
int		time_before_ref;
int		time_after_ref;



/*
============================================================================

CLIENT / SERVER interactions

============================================================================
*/

/**
 * \brief Print formatted output to the standard output stream.
 * \param[in] fmt Format control.
 * \param[in] ... Optional arguments.
 * \note Both client and server can use this, and it will output to the appropriate place.
 */
PUBLIC void Com_Printf( const char *fmt, ... )
{
	va_list		argptr;
	static char	msg[ MAXPRINTMSG ];

	va_start( argptr, fmt );
	(void)vsnprintf( msg, sizeof( msg ), fmt, argptr );
	va_end( argptr );

	msg[ sizeof( msg ) - 1 ] = '\0';

	/*if( rd_target )
	{
		if( (strlen( msg ) + strlen( rd_buffer ) ) > (rd_buffersize - 1) )
		{
			rd_flush( rd_target, rd_buffer );
			*rd_buffer = '\0';
		}
		com_strlcat( rd_buffer, msg, rd_buffersize );
		return;
	}*/

	Con_Print( msg );

	// also echo to debugging console
//	Sys_ConsoleOutput( msg );

	// logfile
	if( logfile_active && logfile_active->value )
	{
		char name[ MAX_GAMEPATH ];

		if( !logfile )
		{
			com_snprintf( name, sizeof( name ), "%s/console.log", FS_Gamedir() );
			if( logfile_active->value > 2 )
			{
				logfile = fopen( name, "a" );
			}
			else
			{
				logfile = fopen( name, "w" );
			}
		}
		if( logfile )
		{
			fprintf( logfile, "%s", msg );
		}

		if( logfile_active->value > 1 )
		{
			fflush( logfile );		// force it to save every time
		}
	}
}

/**
 * \brief Print formatted output to the development output stream.
 * \param[in] fmt Format control.
 * \param[in] ... Optional arguments.
 * \note A Com_Printf that only shows up if the "developer" cvar is set.
 */
PUBLIC void Com_DPrintf( const char *fmt, ... )
{
	va_list		argptr;
	static char	msg[ MAXPRINTMSG ];

	if( !developer || !developer->value )
	{
		return;			// don't confuse non-developers with techie stuff...
	}

	va_start( argptr, fmt );
	(void)vsnprintf( msg, sizeof( msg ), fmt, argptr );
	va_end( argptr );

	msg[ sizeof( msg ) - 1 ] = '\0';

	Com_Printf( "%s", msg );
}


/**
 * \brief Log error message and shutdown client/server
 * \param[in] code Error code.
 * \param[in] fmt Format control.
 * \param[in] ... Optional arguments.
 * \return Both client and server can use this, and it will do the apropriate things.
 */
PUBLIC void Com_Error( int code, const char *fmt, ... )
{
	va_list			argptr;
	static char		msg[ MAXPRINTMSG ];
	static	_boolean	recursive;

	if( recursive )
	{
		Sys_Error( "recursive error after: %s", msg );
	}

	recursive = true;

	va_start (argptr,fmt);
	(void)vsnprintf( msg, sizeof( msg ), fmt, argptr );
	va_end (argptr);

	msg[ sizeof( msg ) - 1 ] = '\0';

	if( code == ERR_DISCONNECT )
	{
		Client_Drop();
		recursive = false;
		longjmp (abortframe, -1);
	}
	else if( code == ERR_DROP )
	{
		Com_Printf( "********************\nERROR: %s\n********************\n", msg );
//		SV_Shutdown (va("Server crashed: %s\n", msg), false);
		Client_Drop ();
		recursive = false;
		longjmp( abortframe, -1 );
	}
	else
	{
//		SV_Shutdown (va("Server fatal crashed: %s\n", msg), false);
		Client_Shutdown();
	}

	if( logfile )
	{
		fclose( logfile );
		logfile = NULL;
	}

	Sys_Error( "%s", msg );
}


/**
 * \brief Shutdown application
 * \return Both client and server can use this, and it will do the apropriate things.
 */
PUBLIC void Com_Quit( void )
{
//	SV_Shutdown ("Server quit\n", false);

	Game_Shutdown();

	if( logfile )
	{
		fclose( logfile );
		logfile = NULL;
	}



	Sys_Quit();


}

/**
 * \brief Get server state
 * \return Returns server state.
 */
PUBLIC int Com_ServerState( void )
{
	return server_state;
}

/**
 * \brief Set server state
 * \param[in] state Server state.
 */
PUBLIC void Com_SetServerState( int state )
{
	server_state = state;
}


//===========================================================================


/**
 * \brief Initialize script handler
 * \param[in\out] buf Valid pointer to sizebuf_t structure.
 * \param[in] data Data.
 * \param[in] length Length of data in bytes
 */
PUBLIC void SZ_Init( sizebuf_t *buf, PW8 data, int length )
{
	memset( buf, 0, sizeof( *buf ) );
	buf->data = data;
	buf->maxsize = length;
}

/**
 * \brief Clear script handler flags
 * \param[in\out] buf Valid pointer to sizebuf_t structure.
 */
PUBLIC void SZ_Clear( sizebuf_t *buf )
{
	buf->cursize = 0;
	buf->overflowed = false;
}

/**
 * \brief Get space
 * \param[in\out] buf Valid pointer to sizebuf_t structure.
 * \param[in] length Length of data in bytes.
 * \return Pointer to data
 */
PUBLIC void *SZ_GetSpace( sizebuf_t *buf, int length )
{
	void	*data;

	if( buf->cursize + length > buf->maxsize )
	{
		if( !buf->allowoverflow )
		{
			Com_Error( ERR_FATAL, "SZ_GetSpace: overflow without allowoverflow set" );
		}

		if( length > buf->maxsize )
		{
			Com_Error( ERR_FATAL, "SZ_GetSpace: %i is > full buffer size", length );
		}

		Com_Printf( "SZ_GetSpace: overflow\n" );
		SZ_Clear( buf );
		buf->overflowed = true;
	}

	data = buf->data + buf->cursize;
	buf->cursize += length;

	return data;
}

/**
 * \brief Get space
 * \param[in\out] buf Valid pointer to sizebuf_t structure.
 * \param[in] data Data.
 * \param[in] length Length of data in bytes.
 */
PUBLIC void SZ_Write( sizebuf_t *buf, void *data, int length )
{
	memcpy( SZ_GetSpace( buf, length ), data, length );
}

/**
 * \brief Print
 * \param[in\out] buf Valid pointer to sizebuf_t structure.
 * \param[in] data Data.
 */
PUBLIC void SZ_Print( sizebuf_t *buf, W8 *data )
{
	int		len;

	len = strlen( (char *)data ) + 1;

	if (buf->cursize)
	{
		if( buf->data[ buf->cursize - 1 ] )
		{
			memcpy( (PW8)SZ_GetSpace( buf, len ), data, len ); // no trailing 0
		}
		else
		{
			memcpy( (PW8)SZ_GetSpace( buf, len - 1 ) - 1, data, len ); // write over trailing 0
		}
	}
	else
	{
		memcpy( (PW8)SZ_GetSpace( buf, len ),data,len );
	}
}


//============================================================================

/**
 * \brief Check param string
 * \param[in] parm Param string
 * \return The position (1 to argc-1) in the program's argument list where the given parameter apears, or 0 if not present
 */
PUBLIC int COM_CheckParm( char *parm )
{
	int		i;

	for( i = 1 ; i < com_argc ; ++i )
	{
		if( !strcmp( parm, com_argv[ i ] ) )
		{
			return i;
		}
	}

	return 0;
}

/**
 * \brief Get common arguments
 * \return An integer that contains the count of arguments that are in argv
 */
PUBLIC int COM_Argc (void)
{
	return com_argc;
}

/**
 * \brief Get command-line arguments based on index.
 * \param[in] arg Index
 * \return A null-terminated string
 * \note Argv is an array of null-terminated strings representing command-line arguments entered by the user of the program.
 */
PUBLIC char *COM_Argv ( int arg )
{
	if (arg < 0 || arg >= com_argc || !com_argv[arg])
    {
		return "";
    }
	return com_argv[ arg ];
}

/**
 * \brief Clear argument at index
 * \param[in] arg Index to clear from argv
 */
PUBLIC void COM_ClearArgv( int arg )
{
	if (arg < 0 || arg >= com_argc || !com_argv[arg])
	{
		return;
	}

	com_argv[ arg ] = "";
}

/**
 * \brief Set global argv values with commadline argv values.
 * \param[in] argc An integer specifying how many arguments are in argv[].
 * \param[in] argv An array of null-terminated strings. The last pointer (argv[argc]) is NULL.
 */
PUBLIC void COM_InitArgv( int argc, char *argv[] )
{
	int i;

	if( argc > MAX_NUM_ARGVS )
	{
		argc = MAX_NUM_ARGVS;
		Com_DPrintf( "argc > MAX_NUM_ARGVS\n" );
	}

	com_argc = argc;
	for( i = 0; i < argc; ++i )
	{
		if( !argv[ i ] || strlen( argv[ i ] ) >= MAX_TOKEN_CHARS )
		{
			com_argv[ i ] = "";
		}
		else
		{
			com_argv[ i ] = argv[ i ];
		}
	}
}

/**
 * \brief Adds the given string at the end of the current argument list.
 * \param[in] parm Agrument to add to argv
 */
PUBLIC void COM_AddParm( char *parm )
{
	if( com_argc == MAX_NUM_ARGVS )
	{
		Com_Error( ERR_FATAL, "COM_AddParm: MAX_NUM_ARGS" );
	}

	com_argv[ com_argc++ ] = parm;
}

PUBLIC char *Com_StringContains(char *str1, char *str2, int casesensitive)
{
	int len, i, j;

	len = strlen(str1) - strlen(str2);
	for (i = 0; i <= len; i++, str1++) {
		for (j = 0; str2[j]; j++) {
			if (casesensitive) {
				if (str1[j] != str2[j]) {
					break;
				}
			}
			else {
				if (TOUPPER(str1[j]) != TOUPPER(str2[j])) {
					break;
				}
			}
		}
		if (!str2[j]) {
			return str1;
		}
	}
	return NULL;
}

PUBLIC int Com_Filter(char *filter, char *name, int casesensitive)
{
	char buf[MAX_TOKEN_CHARS];
	char *ptr;
	int i, found;

	while(*filter) {
		if (*filter == '*') {
			filter++;
			for (i = 0; *filter; i++) {
				if (*filter == '*' || *filter == '?') break;
				buf[i] = *filter;
				filter++;
			}
			buf[i] = '\0';
			if (strlen(buf)) {
				ptr = Com_StringContains(name, buf, casesensitive);
				if (!ptr) return false;
				name = ptr + strlen(buf);
			}
		}
		else if (*filter == '?') {
			filter++;
			name++;
		}
		else if (*filter == '[' && *(filter+1) == '[') {
			filter++;
		}
		else if (*filter == '[') {
			filter++;
			found = false;
			while(*filter && !found) {
				if (*filter == ']' && *(filter+1) != ']') break;
				if (*(filter+1) == '-' && *(filter+2) && (*(filter+2) != ']' || *(filter+3) == ']')) {
					if (casesensitive) {
						if (*name >= *filter && *name <= *(filter+2)) found = true;
					}
					else {
						if (TOUPPER(*name) >= TOUPPER(*filter) &&
							TOUPPER(*name) <= TOUPPER(*(filter+2))) found = true;
					}
					filter += 3;
				}
				else {
					if (casesensitive) {
						if (*filter == *name) found = true;
					}
					else {
						if (TOUPPER(*filter) == TOUPPER(*name)) found = true;
					}
					filter++;
				}
			}
			if (!found) return false;
			while(*filter) {
				if (*filter == ']' && *(filter+1) != ']') break;
				filter++;
			}
			filter++;
			name++;
		}
		else {
			if (casesensitive) {
				if (*filter != *name) return false;
			}
			else {
				if (TOUPPER(*filter) != TOUPPER(*name)) return false;
			}
			filter++;
			name++;
		}
	}
	return true;
}


/*
============================================================================

COMMAND LINE FUNCTIONS

+ characters seperate the commandLine string into multiple console
command lines.

All of these are valid:

quake3 +set test blah +map test
quake3 set test blah+map test
quake3 set test blah + map test

============================================================================
*/

#define	MAX_CONSOLE_LINES	32
int		com_numConsoleLines;
/* Experiment */
/* char	*com_consoleLines[MAX_CONSOLE_LINES]; */
char **com_consoleLines;

/*
===================
Com_SafeMode

Check for "safe" on the command line, which will
skip loading of q3config.cfg
===================
*/
_boolean Com_SafeMode( void ) {
	int		i;

	for ( i = 0 ; i < com_numConsoleLines ; i++ ) {
		Cmd_TokenizeString( com_consoleLines[i] );
		if ( !com_stricmp( Cmd_Argv(0), "safe" )
			|| !com_stricmp( Cmd_Argv(0), "cvar_restart" ) ) {
			com_consoleLines[i][0] = 0;
			return true;
		}
	}
	return false;
}


/*
===============
Com_StartupVariable

Searches for command line parameters that are set commands.
If match is not NULL, only that cvar will be looked for.
That is necessary because cddir and basedir need to be set
before the filesystem is started, but all other sets shouls
be after execing the config and default.
===============
*/
void Com_StartupVariable( const char *match ) {
	int		i;
	char	*s;
	cvar_t	*cv;

	for (i=0 ; i < com_numConsoleLines ; i++) {
		Cmd_TokenizeString( com_consoleLines[i] );
		if ( strcmp( Cmd_Argv(0), "set" ) ) {
			continue;
		}

		s = Cmd_Argv(1);
		if ( !match || !strcmp( s, match ) ) {
			Cvar_Set( s, Cmd_Argv(2) );
			cv = Cvar_Get( s, "", 0 );
			cv->flags |= CVAR_USER_CREATED;
//			com_consoleLines[i] = 0;
		}
	}
}

/*
=================
Com_AddStartupCommands

Adds command line parameters as script statements
Commands are seperated by + signs

Returns qtrue if any late commands were added, which
will keep the demoloop from immediately starting
=================
*/
_boolean Com_AddStartupCommands( void ) {
	int		i;
	_boolean	added;

	added = false;
	// quote every token, so args with semicolons can work
	for (i=0 ; i < com_numConsoleLines ; i++) {
		if ( !com_consoleLines[i] || !com_consoleLines[i][0] ) {
			continue;
		}

		// set commands won't override menu startup
        if ( com_strnicmp( com_consoleLines[i], "set", 3 ) ) {
			added = true;
		}
		Cbuf_AddText( com_consoleLines[i] );
		Cbuf_AddText( "\n" );
	}

	return added;
}



const char *completionString;
char shortestMatch[MAX_TOKEN_CHARS];
static int	matchCount;

void FindMatches( const char *s )
{
	int		i;

	if ( com_strnicmp( s, completionString, strlen( completionString ) ) )
    {
		return;
	}
	matchCount++;
	if ( matchCount == 1 )
    {
		com_strlcpy( shortestMatch, s, sizeof( shortestMatch ) );
		return;
	}

	// cut shortestMatch to the amount common with s
	for ( i = 0 ; s[i] ; i++ )
    {
		if ( TOUPPER(shortestMatch[i]) != TOUPPER(s[i]) )
        {
			shortestMatch[i] = 0;
		}
	}
}


/**
 * \brief Just throw a fatal error to test error shutdown procedures.
 */
PRIVATE void Com_Error_f (void)
{
	Com_Error( ERR_FATAL, "%s", Cmd_Argv( 1 ) );
}

/**
 * \brief Initialize sub-systems.
 * \param[in] argc An integer specifying how many arguments are in argv array.
 * \param[in] An array of null-terminated strings. The last pointer (argv[argc]) is NULL.
 */
/* PUBLIC void common_Init( char *commandLine ) */
PUBLIC void common_Init( int argc, char *argv[] )
{
	if( setjmp( abortframe ) )
	{
		Sys_Error( "Error during initialization" );
	}

	z_chain.next = z_chain.prev = &z_chain;

    Cvar_Init();

	// prepare enough of the subsystems to handle
	// cvar and command buffer management

	com_numConsoleLines = argc;
	com_consoleLines = argv;
    Cbuf_Init ();
	Cmd_Init();


	Key_Init();

	// override anything from the config files with command line args
	Com_StartupVariable( NULL );

	FS_InitFilesystem();


	Cbuf_AddText( "exec DEFAULT.CFG\n" );

	Cbuf_AddText( "exec config.cfg\n" );

    Cbuf_Execute ();

	// override anything from the config files with command line args
	Com_StartupVariable( NULL );

	//
	// init commands and vars
	//
	Cmd_AddCommand( "z_stats", Z_Stats_f );
	Cmd_AddCommand( "error", Com_Error_f );

//	host_speeds = Cvar_Get( "host_speeds", "0", CVAR_INIT );
//	log_stats = Cvar_Get( "log_stats", "1", CVAR_INIT );
	developer = Cvar_Get( "developer", "1", CVAR_INIT );
//	timescale = Cvar_Get( "timescale", "1", CVAR_INIT );
//	fixedtime = Cvar_Get( "fixedtime", "0", CVAR_INIT );
	logfile_active = Cvar_Get( "logfile", "2", CVAR_INIT );
//	showtrace = Cvar_Get( "showtrace", "1", CVAR_INIT );

#ifdef DEDICATED_ONLY

	dedicated = Cvar_Get ( "dedicated", "1", CVAR_ROM );

#else

	dedicated = Cvar_Get ( "dedicated", "0", CVAR_ROM );

#endif

	//s = va( "%s %s %s %s %s %s", APP_VERSION, RELEASE_NAME, CPUSTRING, __DATE__, __TIME__, BUILDSTRING );
	//Cvar_Get( "version", s, CVAR_SERVERINFO | CVAR_ROM );

	if( dedicated->value )
	{
		Cmd_AddCommand( "quit", Com_Quit );
	}


	Sys_OS_Init();



	Client_Init();


	Game_Init();	// game and player init


    // add + commands from command line
	if ( !Com_AddStartupCommands() )
    {
		// if the user didn't give any commands, run default action
		if ( !dedicated->integer )
        {
			Cbuf_AddText ("intro PC13\n");
            Cbuf_Execute();
		}
	}

	Com_Printf( "\n====== Application Initialized ======\n\n" );

}
