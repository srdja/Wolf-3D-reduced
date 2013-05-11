/*

	Copyright (C) 2004 Michael Liebscher
	Copyright (C) 2000-2002 by DarkOne the Hacker

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
 *	wolf_act_stat.h:  Wolfenstein3-D entity management.
 *	
 *	Author:	Michael Liebscher <johnnycanuck@users.sourceforge.net>
 *	Date:	2004 
 *
 *	Acknowledgement:
 *	Portion of this code was derived from NewWolf, and was originally
 *	written by DarkOne the Hacker. 
 *
 *	Portion of this code was derived from Wolfenstein3-D, and was originally
 *	written by Id Software, Inc.
 *
 */

/*
	Notes:	


*/

#ifndef __WOLF_ACT_STAT_H__
#define __WOLF_ACT_STAT_H__

#include "wolf_bj.h"
#include "wolf_actor_ai.h"
#include "wolf_ai_com.h"
#include "wolf_sprites.h"

#define ST_INFO_NULL { 0, SPR_DEMO, 0, NULL, NULL, st_dead }

extern stateinfo objstate[ NUMENEMIES ][ NUMSTATES ];
extern int	starthitpoints[ 4 ][ NUMENEMIES ];

void CorrectBossDeathTimeouts();

#endif /* __WOLF_ACT_STAT_H__ */

