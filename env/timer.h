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
 *	timer.h:   Timer.
 *	
 *	Author:	Id Software, Inc.
 *	Date:	1997-2001
 *
 */

/*
	Notes:	
	This module is implemented by win_timer.c or unix_timer.c.

*/

#ifndef __TIMER_H__
#define __TIMER_H__

#include "platform.h"


W32	Sys_Milliseconds( void );


#endif /* __TIMER_H__ */

