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

#include <stdbool.h>
#include "input_context.h"

bool input_init();
void input_poll();

void input_add_context(InputContext *context, char *name);
void input_set_context(char *name);

#endif /* __INPUT_H__ */

