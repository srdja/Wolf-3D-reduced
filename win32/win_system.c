/*

	Copyright (C) 2004-2012 Michael Liebscher <johnnycanuck@users.sourceforge.net>

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
 * \file win_system.c
 * \brief Non-portable system functions.
 * \author Michael Liebscher
 * \date 2004-2012
 */


#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>
#include <mmsystem.h>


#include "common.h"


/**
 * \brief Init OS specfic stuff.
 */
PUBLIC void Sys_OS_Init( void )
{
	// make sure the timer is high precision, otherwise
	// NT gets 18ms resolution
	timeBeginPeriod( 1 );
}


