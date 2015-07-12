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
 * \file cmd.h
 * \brief Command text buffering and command execution.
 * \author Id Software, Inc.
 * \date 1999-2005
 * \note This code was derived from Quake III Arena, and was originally written by Id Software, Inc.
 */

/**
    \note

    Any number of commands can be added in a frame, from several different sources.
    Most commands come from either keybindings or console line input, but remote
    servers can also send across commands and entire text files can be execed.

    The + command line options are also added to the command buffer.

    The game starts with a Cbuf_AddText( "exec DEFAULT.CFG\n" ); Cbuf_Execute();

*/

#ifndef __CMD_H__
#define __CMD_H__

#include "platform.h"


/*
==============================================================

CMD

Command text buffering and command execution

==============================================================
*/

/*

Any number of commands can be added in a frame, from several different sources.
Most commands come from either keybindings or console line input, but entire text
files can be execed.

*/

void Cbuf_Init (void);
// allocates an initial text buffer that will grow as needed

void Cbuf_AddText (const char *text);
// Adds command text at the end of the buffer, does NOT add a final \n

void Cbuf_ExecuteText (int exec_when, const char *text);
// this can be used in place of either Cbuf_AddText or Cbuf_InsertText

void Cbuf_Execute (void);
// Pulls off \n terminated lines of text from the command buffer and sends
// them through Cmd_ExecuteString.  Stops when the buffer is empty.
// Normally called once per frame, but may be explicitly invoked.
// Do not call inside a command function, or current args will be destroyed.

//===========================================================================

#define MAX_STRING_CHARS    1024    // max length of a string passed to Cmd_TokenizeString
#define MAX_STRING_TOKENS   1024    // max tokens resulting from Cmd_TokenizeString
#define MAX_TOKEN_CHARS     1024    // max length of an individual token


/*

Command execution takes a null terminated string, breaks it into tokens,
then searches for a command or variable that matches the first token.

*/

typedef void (*xcommand_t) (void);

void    Cmd_Init (void);

void    Cmd_AddCommand (const char *cmd_name, xcommand_t function);
// called by the init functions of other parts of the program to
// register commands and functions to call for them.
// The cmd_name is referenced later, so it should not be in temp memory
// if function is NULL, the command will be forwarded to the server
// as a clc_clientCommand instead of executed locally

void    Cmd_RemoveCommand (const char *cmd_name);

void    Cmd_CommandCompletion (void (*callback) (const char *s));
// callback with each valid string

int     Cmd_Argc (void);
char    *Cmd_Argv (int arg);
void    Cmd_ArgvBuffer (int arg, char *buffer, int bufferLength);
char    *Cmd_Args (void);
char    *Cmd_ArgsFrom (int arg);
void    Cmd_ArgsBuffer (char *buffer, int bufferLength);
char    *Cmd_Cmd (void);
// The functions that execute commands get their parameters with these
// functions. Cmd_Argv () will return an empty string, not a NULL
// if arg > argc, so string operations are allways safe.

void    Cmd_TokenizeString (const char *text);
// Takes a null terminated string.  Does not need to be /n terminated.
// breaks the string up into arg tokens.

void    Cmd_ExecuteString (const char *text);
// Parses a single line of text into arguments and tries to execute it
// as if it was typed at the console





#endif /* __CMD_H__ */
