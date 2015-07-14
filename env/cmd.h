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


#define MAX_STRING_TOKENS   1024    // max tokens resulting from Cmd_TokenizeString
#define MAX_TOKEN_CHARS     1024    // max length of an individual token


typedef void (*xcommand_t) (void);

int     Cmd_Argc (void);
char    *Cmd_Argv (int arg);

void    Cmd_TokenizeString (const char *text);
void    Cmd_ExecuteString (const char *text);

#endif /* __CMD_H__ */
