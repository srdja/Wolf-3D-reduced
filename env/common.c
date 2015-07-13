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
#include "com_string.h"
#include "keys.h"

#include "../wolf/wolf_local.h"

#define MAXPRINTMSG 4096

colour3_t   colourBlack = {   0,   0,   0 };
colour3_t   colourWhite = { 255, 255, 255 };

jmp_buf abortframe;     // an ERR_DROP occured, exit the entire frame

cvar_t  *developer;
cvar_t  *dedicated;

FILE    *logfile;

/**
 * \brief Print formatted output to the development output stream.
 * \param[in] fmt Format control.
 * \param[in] ... Optional arguments.
 * \note A Com_Printf that only shows up if the "developer" cvar is set.
 */
PUBLIC void Com_DPrintf (const char *fmt, ...)
{
    va_list     argptr;
    static char msg[ MAXPRINTMSG ];

    if (!developer || !developer->value) {
        return;         // don't confuse non-developers with techie stuff...
    }

    va_start (argptr, fmt);
    (void)vsnprintf (msg, sizeof (msg), fmt, argptr);
    va_end (argptr);

    msg[ sizeof (msg) - 1 ] = '\0';
}


/**
 * \brief Log error message and shutdown client/server
 * \param[in] code Error code.
 * \param[in] fmt Format control.
 * \param[in] ... Optional arguments.
 * \return Both client and server can use this, and it will do the apropriate things.
 */
PUBLIC void Com_Error (int code, const char *fmt, ...)
{
    va_list         argptr;
    static char     msg[ MAXPRINTMSG ];
    static  _boolean    recursive;

    if (recursive) {
        Sys_Error ("recursive error after: %s", msg);
    }

    recursive = true;

    va_start (argptr, fmt);
    (void)vsnprintf (msg, sizeof (msg), fmt, argptr);
    va_end (argptr);

    msg[ sizeof (msg) - 1 ] = '\0';

    if (code == ERR_DISCONNECT) {
        Client_Drop();
        recursive = false;
        longjmp (abortframe, -1);
    } else if (code == ERR_DROP) {
        Client_Drop();
        recursive = false;
        longjmp (abortframe, -1);
    } else {
        Client_Shutdown();
    }

    if (logfile) {
        fclose (logfile);
        logfile = NULL;
    }
    Sys_Error ("%s", msg);
}


/**
 * \brief Shutdown application
 * \return Both client and server can use this, and it will do the apropriate things.
 */
PUBLIC void Com_Quit (void)
{
    Game_Shutdown();

    if (logfile) {
        fclose (logfile);
        logfile = NULL;
    }
    Sys_Quit();
}

PUBLIC char *Com_StringContains (char *str1, char *str2, int casesensitive)
{
    int len, i, j;

    len = strlen (str1) - strlen (str2);

    for (i = 0; i <= len; i++, str1++) {
        for (j = 0; str2[j]; j++) {
            if (casesensitive) {
                if (str1[j] != str2[j]) {
                    break;
                }
            } else {
                if (TOUPPER (str1[j]) != TOUPPER (str2[j])) {
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

PUBLIC int Com_Filter (char *filter, char *name, int casesensitive)
{
    char buf[MAX_TOKEN_CHARS];
    char *ptr;
    int i, found;

    while (*filter) {
        if (*filter == '*') {
            filter++;

            for (i = 0; *filter; i++) {
                if (*filter == '*' || *filter == '?') break;

                buf[i] = *filter;
                filter++;
            }

            buf[i] = '\0';

            if (strlen (buf)) {
                ptr = Com_StringContains (name, buf, casesensitive);

                if (!ptr) return false;

                name = ptr + strlen (buf);
            }
        } else if (*filter == '?') {
            filter++;
            name++;
        } else if (*filter == '[' && * (filter + 1) == '[') {
            filter++;
        } else if (*filter == '[') {
            filter++;
            found = false;

            while (*filter && !found) {
                if (*filter == ']' && * (filter + 1) != ']') break;

                if (* (filter + 1) == '-' && * (filter + 2) && (* (filter + 2) != ']' || * (filter + 3) == ']')) {
                    if (casesensitive) {
                        if (*name >= *filter && *name <= * (filter + 2)) found = true;
                    } else {
                        if (TOUPPER (*name) >= TOUPPER (*filter) &&
                                TOUPPER (*name) <= TOUPPER (* (filter + 2))) found = true;
                    }

                    filter += 3;
                } else {
                    if (casesensitive) {
                        if (*filter == *name) found = true;
                    } else {
                        if (TOUPPER (*filter) == TOUPPER (*name)) found = true;
                    }

                    filter++;
                }
            }

            if (!found) return false;

            while (*filter) {
                if (*filter == ']' && * (filter + 1) != ']') break;

                filter++;
            }

            filter++;
            name++;
        } else {
            if (casesensitive) {
                if (*filter != *name) return false;
            } else {
                if (TOUPPER (*filter) != TOUPPER (*name)) return false;
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

int     com_numConsoleLines;
char **com_consoleLines;

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
void Com_StartupVariable (const char *match)
{
    int     i;
    char    *s;
    cvar_t  *cv;

    for (i = 0 ; i < com_numConsoleLines ; i++) {
        Cmd_TokenizeString (com_consoleLines[i]);

        if (strcmp (Cmd_Argv (0), "set")) {
            continue;
        }

        s = Cmd_Argv (1);

        if (!match || !strcmp (s, match)) {
            Cvar_Set (s, Cmd_Argv (2));
            cv = Cvar_Get (s, "", 0);
            cv->flags |= CVAR_USER_CREATED;
//          com_consoleLines[i] = 0;
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
_boolean Com_AddStartupCommands (void)
{
    int     i;
    _boolean    added;

    added = false;

    // quote every token, so args with semicolons can work
    for (i = 0 ; i < com_numConsoleLines ; i++) {
        if (!com_consoleLines[i] || !com_consoleLines[i][0]) {
            continue;
        }

        // set commands won't override menu startup
        if (com_strnicmp (com_consoleLines[i], "set", 3)) {
            added = true;
        }

        Cbuf_AddText (com_consoleLines[i]);
        Cbuf_AddText ("\n");
    }

    return added;
}

const char *completionString;
char shortestMatch[MAX_TOKEN_CHARS];
static int  matchCount;

/**
 * \brief Just throw a fatal error to test error shutdown procedures.
 */
PRIVATE void Com_Error_f (void)
{
    Com_Error (ERR_FATAL, "%s", Cmd_Argv (1));
}

/**
 * \brief Initialize sub-systems.
 * \param[in] argc An integer specifying how many arguments are in argv array.
 * \param[in] An array of null-terminated strings. The last pointer (argv[argc]) is NULL.
 */
/* PUBLIC void common_Init( char *commandLine ) */
PUBLIC void common_Init (int argc, char *argv[])
{
    if (setjmp (abortframe)) {
        Sys_Error ("Error during initialization");
    }

    z_chain.next = z_chain.prev = &z_chain;

    Cvar_Init();

    // prepare enough of the subsystems to handle
    // cvar and command buffer management

    com_numConsoleLines = argc;
    com_consoleLines = argv;
    Cbuf_Init();
    Cmd_Init();


    Key_Init();

    // override anything from the config files with command line args
    Com_StartupVariable (NULL);

    FS_InitFilesystem();

    Cbuf_AddText ("exec DEFAULT.CFG\n");
    Cbuf_AddText ("exec config.cfg\n");
    Cbuf_Execute();

    // override anything from the config files with command line args
    Com_StartupVariable (NULL);

    //
    // init commands and vars
    //
    Cmd_AddCommand ("z_stats", Z_Stats_f);
    Cmd_AddCommand ("error", Com_Error_f);

    developer = Cvar_Get ("developer", "1", CVAR_INIT);

    dedicated = Cvar_Get ("dedicated", "0", CVAR_ROM);

    if (dedicated->value) {
        Cmd_AddCommand ("quit", Com_Quit);
    }

    Client_Init();
    Game_Init();    // game and player init

    // add + commands from command line
    if (!Com_AddStartupCommands()) {
        // if the user didn't give any commands, run default action
        if (!dedicated->integer) {
            Cbuf_AddText ("intro PC13\n");
            Cbuf_Execute();
        }
    }
}
