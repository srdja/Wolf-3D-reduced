/*

    Copyright (C) 2004-2005 Michael Liebscher <johnnycanuck@users.sourceforge.net>
    Copyright (C) 1997-2001 Id Software, Inc.

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

    See the GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/



/*
 *  unix_main.c: UNIX interface to application.
 *
 *  Author: Michael Liebscher <johnnycanuck@users.sourceforge.net>
 *
 *  Acknowledgement:
 *  This code was derived from Quake II, and was originally
 *  written by Id Software, Inc.
 *
 */

#include <unistd.h>
#include <stdarg.h>
#include <stdio.h>
#include <fcntl.h>

#include "../env/platform.h"
#include "../env/common.h"
#include "../env/timer.h"

W32 sys_frame_time;

cvar_t *nostdout;


extern void KBD_Update (void);


/**
 * \brief Appends one string to another.
 * \param[in,out] dest Pointer to a NUL-terminated string. The buffer must be large enough to contain both strings or else truncation will occur.
 * \param[in] source Pointer to a NUL-terminated string from which the function copies characters.
 * \param[in] nMaxLength Full size of dest, not space left.
 * \return Returns strlen( source ) + MIN( nMaxLength, strlen( initial dest ) ). If retval >= nMaxLength, truncation occurred.
 * \note At most \c nMaxLength-1 characters will be copied. Always NUL-terminates (unless \c nMaxLength <= strlen( dest) ).
 */
void Sys_Error (const char *format, ...)
{
    va_list     argptr;
    char        string[ 1024 ];

// change stdin to non blocking
    fcntl (0, F_SETFL, fcntl (0, F_GETFL, 0) & ~FNDELAY);

    Client_Shutdown();

    va_start (argptr, format);
    (void)vsnprintf (string, sizeof (string), format, argptr);
    va_end (argptr);

    fprintf (stderr, "Error: %s\n", string);

    _exit (1);
}

void Sys_Quit (void)
{
    Client_Shutdown();

    fcntl (0, F_SETFL, fcntl (0, F_GETFL, 0) & ~FNDELAY);
    _exit (0);
}

/*
-----------------------------------------------------------------------------
 Function: Sys_GetClipboardData

 Parameters: Nothing.

 Returns: Pointer to a string on success, NULL otherwise.

 Notes: Grabs text from clipboard.
        Caller is responsible for freeing data.
-----------------------------------------------------------------------------
*/
char *Sys_GetClipboardData (void)
{
    return NULL;
}


/*
-----------------------------------------------------------------------------
 Function: main -Application entry point.

 Parameters:

 Returns: Nothing.

 Notes: This is the application entry point.
            1.  Check for mulitple instances.
            2.  Init Sub-systems.
            3.  Enter application loop.
-----------------------------------------------------------------------------
*/
int main (int argc, char *argv[])
{
    int     time, oldtime, newtime;

    // go back to real user for config loads
    seteuid (getuid());

    common_Init();

    fcntl (0, F_SETFL, fcntl (0, F_GETFL, 0) | FNDELAY);

    nostdout = Cvar_Get ("nostdout", "0", CVAR_INIT);

    if (! nostdout->value) {
        fcntl (0, F_SETFL, fcntl (0, F_GETFL, 0) | FNDELAY);
    }

    oldtime = Sys_Milliseconds();

    while (1) {
        KBD_Update();

        // find time spent rendering last frame
        do {
            newtime = Sys_Milliseconds();
            time = newtime - oldtime;
        } while (time < 1);

        //Cbuf_Execute();
        Client_Frame (time);

        oldtime = newtime;
    }

// Should never get here!
    return 0;
}

