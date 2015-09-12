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

/**
 * \file common.h
 * \brief Common definitions between client and server.
 * \author Michael Liebscher
 * \date 2004
 * \note This code was derived from Quake II, and was originally written by Id Software, Inc.
 */


#ifndef __COMMON_H__
#define __COMMON_H__

#include "../util/com_math.h"
#include "memory.h"
#include "../util/filesystem.h"
#include "platform.h"

void Client_Init (void);
void frame_run(int msec);


typedef enum { FONT0 = 0, FONT1, FONT2, FONT3 } FONTSELECT;

//
// button bits
//
#define BUTTON_ATTACK       1
#define BUTTON_USE          2

// usercmd_t is sent to the server each client frame
typedef struct usercmd_s {
    W8  msec;
    W8  buttons;
    short   angles[ 3 ];
    short   forwardmove, sidemove, upmove;
    W8  impulse;        // remove?
} usercmd_t;

#endif /* __COMMON_H__ */

