/*

	Copyright (C) 1997-2012 Id Software, Inc.

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
 * \file win_timer.c
 * \brief Handles non-portable timer services
 * \author Id Software, Inc.
 * \date 1997-2012
 * \note 
 */


#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>
#include <mmsystem.h>

#include "../env/platform.h"
#include "../env/common_utils.h"


/**
 * \brief  This function retrieves the system time, in milliseconds. The system time is the time elapsed since application was started.
 * \return Returns the system time, in milliseconds.
 */
PUBLIC int Sys_Milliseconds( void ) 
{
	static int sys_timeBase;
	static _boolean	initialized = false;

	if ( !initialized ) 
	{
		sys_timeBase = timeGetTime();
		initialized = true;
	}
	
	return timeGetTime() - sys_timeBase;
}
