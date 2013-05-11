/*

	Copyright (C) 2004-2013 Michael Liebscher <johnnycanuck@users.sourceforge.net>

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
 * \file wolf_bj.c
 * \brief BJ controller
 * \author Michael Liebscher
 * \date 2004-2013
 */

#include "wolf_actors.h"
#include "wolf_actor_ai.h"
#include "wolf_player.h"
#include "wolf_local.h"
#include "wolf_menu.h"

#include "../env/sound.h"



#define BJRUNSPEED	2048
#define BJJUMPSPEED	680



/**
 * \brief Spawn BJ and orient camera
 */
void SpawnBJVictory( void )
{
	entity_t *bj;


	bj = SpawnActor( en_bj, POS2TILE(Player.position.origin[0]), POS2TILE(Player.position.origin[1]), dir4_north, r_world );
	if( !bj )
	{
		return;
	}

	bj->x = Player.position.origin[ 0 ];
	bj->y = Player.position.origin[ 1 ];
	bj->speed = BJRUNSPEED;
	bj->flags = (W8)FL_NONMARK; // FL_NEVERMARK;
	bj->reacttime = 6;

    A_StateChange( bj, st_path1 );

    
    Player.position.origin[1] += TILE_GLOBAL * 6;
    Player.position.angle = 3 * M_PI / 2; // Face south
    
    PL_TryMove(&Player, r_world);    

    Player.playstate = ex_watchingbj;
}

/**
 * \brief Process BJ and after a set distance have BJ jump
 * \param[in] bj Valid pointer to BJ's entity_t structure
 */
void T_BJRun( entity_t *bj )
{
    T_Advance( bj, AI_Path );
	     
    if( (Player.position.origin[ 1 ] - bj->y) < TILE_GLOBAL * 2 )
	{
		A_StateChange( bj, st_shoot1 );
		bj->speed = BJJUMPSPEED;
	}
}

/**
 * \brief Process BJ jump
 * \param[in] bj Valid pointer to BJ's entity_t structure
 */
void T_BJJump( entity_t *bj )
{
    T_Advance( bj, AI_Path );
}

/**
 * \brief Play BJ's yell
 * \param[in] bj Valid pointer to BJ's entity_t structure
 */
void T_BJYell( entity_t *bj )
{
	Sound_StartSound( NULL, 0, CHAN_VOICE, Sound_RegisterSound( "sfx/082.wav" ), 1, ATTN_NORM, 0 );
}

/**
 * \brief Finished with BJ, so show the intermission screen
 * \param[in] bj Valid pointer to BJ's entity_t structure
 */
void T_BJDone( entity_t *bj )
{
	M_ForceMenuOff();
	Player.playstate = ex_victory;
	M_Intermission_f();			
}
