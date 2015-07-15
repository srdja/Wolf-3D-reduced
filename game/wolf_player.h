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
 *  wolf_player.h:  Wolfenstein3-D player management.
 *
 *  Author: Michael Liebscher <johnnycanuck@users.sourceforge.net>
 *  Date:   2004
 *
 *  Acknowledgement:
 *  Portion of this code was derived from NewWolf, and was originally
 *  written by DarkOne the Hacker.
 *
 */

/*
    Notes:
    This module is implemented by wolf_player.c

*/

#ifndef __WOLF_PLAYER_H__
#define __WOLF_PLAYER_H__

#include "../env/platform.h"
#include "../env/common.h"
#include "wolf_math.h"
#include "wolf_actors.h"



#define ITEM_KEY_GOLD       BIT( 1 )
#define ITEM_KEY_SILVER     BIT( 2 )
#define ITEM_WEAPON_1       BIT( 3 )
#define ITEM_WEAPON_2       BIT( 4 )
#define ITEM_WEAPON_3       BIT( 5 )
#define ITEM_BACKPACK       BIT( 11 ) // doubles carrying capacity
#define ITEM_AUGMENT        BIT( 12 ) // adds 50 to maximum health



enum weapon_e {
    WEAPON_KNIFE,
    WEAPON_PISTOL,
    WEAPON_MACHINEGUN,
    WEAPON_GATLINGGUN,

    WEAPON_TYPES
};

enum key_e {
    KEY_GOLD,
    KEY_SILVER,
    KEY_FREE1,
    KEY_FREE2,

    KEY_TYPES
};

enum ammo_e {
    AMMO_BULLETS,

    AMMO_TYPES
};



// flags
#define PL_FLAG_REUSE       BIT( 1 ) // use button pressed
#define PL_FLAG_ATTCK       BIT( 2 ) // attacking
// debug (cheat codes) flags
#define FL_GODMODE          BIT( 3 )
#define FL_NOTARGET         BIT( 4 )


typedef enum state_e {
    ex_notingame,
    ex_playing,
    ex_dead,
    ex_secretlevel,
    ex_watchingdeathcam,
    ex_watchingbj,
    ex_victory,
    ex_complete
    /*
        ex_stillplaying,
        ex_completed,
        ex_died,
        ex_warped,
        ex_resetgame,
        ex_loadedgame,
        ex_victorious,
        ex_abort,
        ex_demodone,
        ex_secretlevel
    */
} state_t;

// ------------------------- * types * -------------------------
// Player structure: Holds all info about player
typedef struct player_s {
    placeonplane_t position; // player position
    int movx, movy, speed;
    int tilex, tiley;

// stats
    int health, lives, frags;
    int armor;  // there are 2 types. The better one is indicated by high bit set
    int ammo[AMMO_TYPES];
    int old_score, score, next_extra;
    unsigned items; // (keys, weapon)
    int weapon, pendingweapon;
// additional info
    int attackframe, attackcount, weaponframe; // attack info
    unsigned flags;
    int areanumber;

    _boolean madenoise; // FIXME: move to flags?
    entity_t *LastAttacker;
    int faceframe, facecount;   // bj's face in the HUD // FIXME decide something!
    _boolean face_gotgun, face_ouch;
    state_t playstate; // fixme: move to gamestate

} player_t;

extern player_t Player;




void PL_Spawn (placeonplane_t location, LevelData_t *lvl);

void PL_Process (player_t *self, LevelData_t *lvl);

void PL_Damage (player_t *self, entity_t *attacker, int points);
_boolean PL_GiveHealth (player_t *self, int points, int max);
_boolean PL_GiveAmmo (player_t *self, int type, int ammo);
void PL_GiveWeapon (player_t *self, int weapon);
void PL_GiveLife (player_t *self);
void PL_GivePoints (player_t *self, W32 points);
void PL_GiveKey (player_t *self, int key);

void PL_NewGame (player_t *self);
void PL_NextLevel (player_t *self);
_boolean PL_Reborn (player_t *self);
_boolean PL_TryMove (player_t *self, LevelData_t *lvl);

void PL_Init (void);
void PL_Reset (void);


void weapon_attack (player_t *self);


#endif /* __WOLF_PLAYER_H__ */
