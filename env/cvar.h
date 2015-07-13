/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.

This file is part of Quake III Arena source code.

Quake III Arena source code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

Quake III Arena source code is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Foobar; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/

/**
 * \file cvar.h
 * \brief Dynamic variable tracking.
 * \author Id Software, Inc.
 * \date 1999-2005
 * \note This code was derived from Quake III Arena, and was originally written by Id Software, Inc.
 */

/*

Notes:

    Dynamic variable tracking.

    cvar_t variables are used to hold scalar or string variables
    that can be changed or displayed at the console or prog code
    as well as accessed directly in C code.

    The user can access cvars from the console in three ways:
    r_draworder         -prints the current value
    r_draworder 0       -sets the current value to 0
    set r_draworder 0   -as above, but creates the cvar if not present

    Cvars are restricted from having the same names as commands to keep this
    module from being ambiguous.

    This module is implemented by cvar.c

*/

#ifndef __CVAR_H__
#define __CVAR_H__


#include <stdio.h>

#include "platform.h"


/*
==========================================================

CVARS (console variables)

Many variables can be used for cheating purposes, so when
cheats is zero, force all unspecified variables to their
default values.
==========================================================
*/

#define CVAR_ARCHIVE        1   // set to cause it to be saved to vars.rc
// used for system variables, not for player
// specific configurations

#define CVAR_USERINFO       2   // sent to server on connect or change
#define CVAR_SERVERINFO     4   // sent in response to front end requests
#define CVAR_SYSTEMINFO     8   // these cvars will be duplicated on all clients
#define CVAR_INIT       16  // don't allow change from console at all,
// but can be set from the command line

#define CVAR_LATCH      32  // will only change when C code next does
// a Cvar_Get(), so it can't be changed
// without proper initialization.  modified
// will be set, even though the value hasn't
// changed yet

#define CVAR_ROM        64  // display only, cannot be set by user at all
#define CVAR_USER_CREATED   128 // created by a set command
#define CVAR_TEMP       256 // can be set even when cheats are disabled, but is not archived
#define CVAR_CHEAT      512 // can not be changed if cheats are disabled
#define CVAR_NORESTART      1024    // do not clear when a cvar_restart is issued

// nothing outside the Cvar_*() functions should modify these fields!
typedef struct cvar_s {
    char        *name;
    char        *string;
    char        *resetString;       // cvar_restart will reset to this value
    char        *latchedString;     // for CVAR_LATCH vars
    int     flags;
    _boolean    modified;       // set each time the cvar is changed
    int     modificationCount;  // incremented each time the cvar is changed
    float       value;          // atof( string )
    int     integer;        // atoi( string )
    struct cvar_s *next;
    struct cvar_s *hashNext;
} cvar_t;

#define MAX_CVAR_VALUE_STRING   256

typedef int cvarHandle_t;

// the modules that run in the virtual machine can't access the cvar_t directly,
// so they must ask for structured updates
typedef struct {
    cvarHandle_t    handle;
    int         modificationCount;
    float       value;
    int         integer;
    char        string[MAX_CVAR_VALUE_STRING];
} vmCvar_t;

cvar_t *Cvar_Get (const char *var_name, const char *value, int flags);
// creates the variable if it doesn't exist, or returns the existing one
// if it exists, the value will not be changed, but flags will be ORed in
// that allows variables to be unarchived without needing bitflags
// if value is "", the value will not override a previously set value.

void    Cvar_Update (vmCvar_t *vmCvar);
// updates an interpreted modules' version of a cvar

void    Cvar_Set (const char *var_name, const char *value);
// will create the variable with no flags if it doesn't exist

void    Cvar_SetLatched (const char *var_name, const char *value);
// don't set the cvar immediately

void    Cvar_SetValue (const char *var_name, float value);
// expands value to a string and calls Cvar_Set

float   Cvar_VariableValue (const char *var_name);
int Cvar_VariableIntegerValue (const char *var_name);
// returns 0 if not defined or non numeric

char    *Cvar_VariableString (const char *var_name);
void    Cvar_VariableStringBuffer (const char *var_name, char *buffer, int bufsize);
// returns an empty string if not defined

void    Cvar_CommandCompletion (void (*callback) (const char *s));
// callback with each valid string

void    Cvar_Reset (const char *var_name);

_boolean Cvar_Command (void);
// called by Cmd_ExecuteString when Cmd_Argv(0) doesn't match a known
// command.  Returns true if the command was a variable reference that
// was handled. (print or change)

void    Cvar_WriteVariables (FILE *f);
// writes lines containing "set variable value" for all variables
// with the archive flag set to true.

void    Cvar_Init (void);
void    Cvar_Restart_f (void);

extern  int         cvar_modifiedFlags;
// whenever a cvar is modifed, its flags will be OR'd into this, so
// a single check can determine if any CVAR_USERINFO, CVAR_SERVERINFO,
// etc, variables have been modified since the last check.  The bit
// can then be cleared to allow another change detection.


#endif /* __CVAR_H__ */
