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

/**
 * \file in_view.c
 * \brief Interface to mouse / joystick
 * \author Id Software, Inc.
 * \date 1997-2001
 */

#include "client.h"

_boolean    mouseinitialized;
// false when not focus app

extern void IN_StartupMouse (void);
extern void IN_ActivateMouse (void);
extern void IN_DeactivateMouse (void);
extern void IN_MouseMove (usercmd_t *cmd);


/**
 * \brief Initialize mouse and joy-stick.
 */
void IN_Init (void)
{
    IN_StartupMouse();
}

/**
 * \brief Shutdown input layer.
 */
void IN_Shutdown (void)
{
    IN_DeactivateMouse();
}

/**
 * \brief Called every frame, even if not generating commands.
 */
void IN_Frame (void)
{
    if (! mouseinitialized) {
        return;
    }
    IN_ActivateMouse();
}

/**
 * \brief Apply mouse and joy-stick movements to movement command.
 * \param[in] cmd Valid pointer to usercmd_t structure.
 */
void IN_Move (usercmd_t *cmd)
{
    IN_MouseMove (cmd);
}