/*

    Copyright (C) 2004 Michael Liebscher <johnnycanuck@users.sourceforge.net>
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

/*
 *  console.h: Console drawing and management..
 *
 *  Author: Michael Liebscher   <johnnycanuck@users.sourceforge.net>
 *
 *  Acknowledgement:
 *  This code was derived from Quake II, and was originally
 *  written by Id Software, Inc.
 *
 */

/*
    Notes:
    This module is implemented by console.c.

*/

#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include "platform.h"


#define S_COLOR_BLACK   "^0"
#define S_COLOR_RED     "^1"
#define S_COLOR_GREEN   "^2"
#define S_COLOR_YELLOW  "^3"
#define S_COLOR_BLUE    "^4"
#define S_COLOR_CYAN    "^5"
#define S_COLOR_MAGENTA "^6"
#define S_COLOR_WHITE   "^7"

#define NUM_CON_TIMES 4

#define     CON_TEXTSIZE    32768

typedef struct {
    _boolean    initialized;

    char    text[CON_TEXTSIZE];
    int     current;        // line where next message will be printed
    int     x;              // offset in current line for next print
    int     display;        // bottom of console displays this line

    int     ormask;         // high bit mask for colored characters

    int     linewidth;      // characters across screen
    int     totallines;     // total lines in console scrollback

    float   cursorspeed;

    int     vislines;

    float   times[NUM_CON_TIMES];   // cls.realtime time the line was generated
    // for transparent notify lines
} console_t;

extern  console_t   con;

void Con_DrawCharacter (int cx, int line, int num);

void Con_CheckResize (void);
void Con_Init (void);
void Con_DrawConsole (float frac);
void Con_Print (char *txt);
void Con_CenteredPrint (const char *text);
void Con_DrawNotify (void);
void Con_ClearNotify (void);
void Con_ToggleConsole_f (void);

void Con_PageUp (void) ;
void Con_PageDown (void) ;
void Con_Top (void);
void Con_Bottom (void);


#endif /* __CONSOLE_H__ */

