/*

    Copyright (C) 2004 Michael Liebscher <johnnycanuck@users.sourceforge.net>
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

/**
 * \file unix_timer.c
 * \brief Handles non-portable timer services.
 * \author Michael Liebscher <johnnycanuck@users.sourceforge.net>
 * \note This code was derived from Quake II, and was originally written by Id Software, Inc.
 * \date 2004
 */

#include "../common.h"
#include <sys/time.h>


/**
 * \brief  This function retrieves the system time, in milliseconds. The system time is the time elapsed since application was started.
 * \return Returns the system time, in milliseconds.
 */
uint32_t Sys_Milliseconds (void)
{
    struct timeval tp;
    struct timezone tzp;
    static int      secbase;
    uint32_t curtime;

    gettimeofday (&tp, &tzp);

    if (! secbase) {
        secbase = tp.tv_sec;
        return tp.tv_usec / 1000;
    }

    curtime = (tp.tv_sec - secbase) * 1000 + tp.tv_usec / 1000;

    return curtime;
}
