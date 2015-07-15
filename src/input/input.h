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

/*
 *  input.h:    External (non-keyboard) input devices.
 *
 *  Author:  Id Software, Inc.
 *  Date:    1997-2001
 *
 */


#ifndef __INPUT_H__
#define __INPUT_H__

#include "platform.h"
#include "../game/client.h"

void IN_Init (void);
void IN_Shutdown (void);
void IN_Frame (void);
void IN_Move (usercmd_t *cmd);


#endif /* __INPUT_H__ */

