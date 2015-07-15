/*

    Copyright (C) 2004 Michael Liebscher <johnnycanuck@users.sourceforge.net>
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

/**
 * \file wolf_sprites.h
 * \brief Wolfenstein 3-D sprite handling.
 * \author Michael Liebscher
 * \date 2004
 * \note Portion of this code was derived from NewWolf, and was originally written by DarkOne the Hacker.
 *       Portion of this code was derived from Wolfenstein 3-D, and was originally written by Id Software, Inc.
 */


#ifndef __WOLF_SPRITES_H__
#define __WOLF_SPRITES_H__



#include <stdio.h>

#include "../util/com_math.h"

//
// sprite constants
//

enum {
    SPR_DEMO,
    SPR_DEATHCAM,
//
// static sprites
//
    SPR_STAT_0, SPR_STAT_1, SPR_STAT_2, SPR_STAT_3,
    SPR_STAT_4, SPR_STAT_5, SPR_STAT_6, SPR_STAT_7,

    SPR_STAT_8, SPR_STAT_9, SPR_STAT_10, SPR_STAT_11,
    SPR_STAT_12, SPR_STAT_13, SPR_STAT_14, SPR_STAT_15,

    SPR_STAT_16, SPR_STAT_17, SPR_STAT_18, SPR_STAT_19,
    SPR_STAT_20, SPR_STAT_21, SPR_STAT_22, SPR_STAT_23,

    SPR_STAT_24, SPR_STAT_25, SPR_STAT_26, SPR_STAT_27,
    SPR_STAT_28, SPR_STAT_29, SPR_STAT_30, SPR_STAT_31,

    SPR_STAT_32, SPR_STAT_33, SPR_STAT_34, SPR_STAT_35,
    SPR_STAT_36, SPR_STAT_37, SPR_STAT_38, SPR_STAT_39,

    SPR_STAT_40, SPR_STAT_41, SPR_STAT_42, SPR_STAT_43,
    SPR_STAT_44, SPR_STAT_45, SPR_STAT_46, SPR_STAT_47,




    SPR_STAT_48, SPR_STAT_49, SPR_STAT_50, SPR_STAT_51,





//
// Guard
//
    SPR_GRD_S_1, SPR_GRD_S_2, SPR_GRD_S_3, SPR_GRD_S_4,
    SPR_GRD_S_5, SPR_GRD_S_6, SPR_GRD_S_7, SPR_GRD_S_8,

    SPR_GRD_W1_1, SPR_GRD_W1_2, SPR_GRD_W1_3, SPR_GRD_W1_4,
    SPR_GRD_W1_5, SPR_GRD_W1_6, SPR_GRD_W1_7, SPR_GRD_W1_8,

    SPR_GRD_W2_1, SPR_GRD_W2_2, SPR_GRD_W2_3, SPR_GRD_W2_4,
    SPR_GRD_W2_5, SPR_GRD_W2_6, SPR_GRD_W2_7, SPR_GRD_W2_8,

    SPR_GRD_W3_1, SPR_GRD_W3_2, SPR_GRD_W3_3, SPR_GRD_W3_4,
    SPR_GRD_W3_5, SPR_GRD_W3_6, SPR_GRD_W3_7, SPR_GRD_W3_8,

    SPR_GRD_W4_1, SPR_GRD_W4_2, SPR_GRD_W4_3, SPR_GRD_W4_4,
    SPR_GRD_W4_5, SPR_GRD_W4_6, SPR_GRD_W4_7, SPR_GRD_W4_8,

    SPR_GRD_PAIN_1, SPR_GRD_DIE_1, SPR_GRD_DIE_2, SPR_GRD_DIE_3,
    SPR_GRD_PAIN_2, SPR_GRD_DEAD,

    SPR_GRD_SHOOT1, SPR_GRD_SHOOT2, SPR_GRD_SHOOT3,

//
// Dog
//
    SPR_DOG_W1_1, SPR_DOG_W1_2, SPR_DOG_W1_3, SPR_DOG_W1_4,
    SPR_DOG_W1_5, SPR_DOG_W1_6, SPR_DOG_W1_7, SPR_DOG_W1_8,

    SPR_DOG_W2_1, SPR_DOG_W2_2, SPR_DOG_W2_3, SPR_DOG_W2_4,
    SPR_DOG_W2_5, SPR_DOG_W2_6, SPR_DOG_W2_7, SPR_DOG_W2_8,

    SPR_DOG_W3_1, SPR_DOG_W3_2, SPR_DOG_W3_3, SPR_DOG_W3_4,
    SPR_DOG_W3_5, SPR_DOG_W3_6, SPR_DOG_W3_7, SPR_DOG_W3_8,

    SPR_DOG_W4_1, SPR_DOG_W4_2, SPR_DOG_W4_3, SPR_DOG_W4_4,
    SPR_DOG_W4_5, SPR_DOG_W4_6, SPR_DOG_W4_7, SPR_DOG_W4_8,

    SPR_DOG_DIE_1, SPR_DOG_DIE_2, SPR_DOG_DIE_3, SPR_DOG_DEAD,
    SPR_DOG_JUMP1, SPR_DOG_JUMP2, SPR_DOG_JUMP3,



//
// SS
//
    SPR_SS_S_1, SPR_SS_S_2, SPR_SS_S_3, SPR_SS_S_4,
    SPR_SS_S_5, SPR_SS_S_6, SPR_SS_S_7, SPR_SS_S_8,

    SPR_SS_W1_1, SPR_SS_W1_2, SPR_SS_W1_3, SPR_SS_W1_4,
    SPR_SS_W1_5, SPR_SS_W1_6, SPR_SS_W1_7, SPR_SS_W1_8,

    SPR_SS_W2_1, SPR_SS_W2_2, SPR_SS_W2_3, SPR_SS_W2_4,
    SPR_SS_W2_5, SPR_SS_W2_6, SPR_SS_W2_7, SPR_SS_W2_8,

    SPR_SS_W3_1, SPR_SS_W3_2, SPR_SS_W3_3, SPR_SS_W3_4,
    SPR_SS_W3_5, SPR_SS_W3_6, SPR_SS_W3_7, SPR_SS_W3_8,

    SPR_SS_W4_1, SPR_SS_W4_2, SPR_SS_W4_3, SPR_SS_W4_4,
    SPR_SS_W4_5, SPR_SS_W4_6, SPR_SS_W4_7, SPR_SS_W4_8,

    SPR_SS_PAIN_1, SPR_SS_DIE_1, SPR_SS_DIE_2, SPR_SS_DIE_3,
    SPR_SS_PAIN_2, SPR_SS_DEAD,

    SPR_SS_SHOOT1, SPR_SS_SHOOT2, SPR_SS_SHOOT3,

//
// Mutant
//
    SPR_MUT_S_1, SPR_MUT_S_2, SPR_MUT_S_3, SPR_MUT_S_4,
    SPR_MUT_S_5, SPR_MUT_S_6, SPR_MUT_S_7, SPR_MUT_S_8,

    SPR_MUT_W1_1, SPR_MUT_W1_2, SPR_MUT_W1_3, SPR_MUT_W1_4,
    SPR_MUT_W1_5, SPR_MUT_W1_6, SPR_MUT_W1_7, SPR_MUT_W1_8,

    SPR_MUT_W2_1, SPR_MUT_W2_2, SPR_MUT_W2_3, SPR_MUT_W2_4,
    SPR_MUT_W2_5, SPR_MUT_W2_6, SPR_MUT_W2_7, SPR_MUT_W2_8,

    SPR_MUT_W3_1, SPR_MUT_W3_2, SPR_MUT_W3_3, SPR_MUT_W3_4,
    SPR_MUT_W3_5, SPR_MUT_W3_6, SPR_MUT_W3_7, SPR_MUT_W3_8,

    SPR_MUT_W4_1, SPR_MUT_W4_2, SPR_MUT_W4_3, SPR_MUT_W4_4,
    SPR_MUT_W4_5, SPR_MUT_W4_6, SPR_MUT_W4_7, SPR_MUT_W4_8,

    SPR_MUT_PAIN_1, SPR_MUT_DIE_1, SPR_MUT_DIE_2, SPR_MUT_DIE_3,
    SPR_MUT_PAIN_2, SPR_MUT_DIE_4, SPR_MUT_DEAD,

    SPR_MUT_SHOOT1, SPR_MUT_SHOOT2, SPR_MUT_SHOOT3, SPR_MUT_SHOOT4,

//
// Officer
//
    SPR_OFC_S_1, SPR_OFC_S_2, SPR_OFC_S_3, SPR_OFC_S_4,
    SPR_OFC_S_5, SPR_OFC_S_6, SPR_OFC_S_7, SPR_OFC_S_8,

    SPR_OFC_W1_1, SPR_OFC_W1_2, SPR_OFC_W1_3, SPR_OFC_W1_4,
    SPR_OFC_W1_5, SPR_OFC_W1_6, SPR_OFC_W1_7, SPR_OFC_W1_8,

    SPR_OFC_W2_1, SPR_OFC_W2_2, SPR_OFC_W2_3, SPR_OFC_W2_4,
    SPR_OFC_W2_5, SPR_OFC_W2_6, SPR_OFC_W2_7, SPR_OFC_W2_8,

    SPR_OFC_W3_1, SPR_OFC_W3_2, SPR_OFC_W3_3, SPR_OFC_W3_4,
    SPR_OFC_W3_5, SPR_OFC_W3_6, SPR_OFC_W3_7, SPR_OFC_W3_8,

    SPR_OFC_W4_1, SPR_OFC_W4_2, SPR_OFC_W4_3, SPR_OFC_W4_4,
    SPR_OFC_W4_5, SPR_OFC_W4_6, SPR_OFC_W4_7, SPR_OFC_W4_8,

    SPR_OFC_PAIN_1, SPR_OFC_DIE_1, SPR_OFC_DIE_2, SPR_OFC_DIE_3,
    SPR_OFC_PAIN_2, SPR_OFC_DIE_4, SPR_OFC_DEAD,

    SPR_OFC_SHOOT1, SPR_OFC_SHOOT2, SPR_OFC_SHOOT3,



//
// Ghosts
//
    SPR_BLINKY_W1, SPR_BLINKY_W2, SPR_PINKY_W1, SPR_PINKY_W2,
    SPR_CLYDE_W1, SPR_CLYDE_W2, SPR_INKY_W1, SPR_INKY_W2,

//
// Hans
//
    SPR_BOSS_W1, SPR_BOSS_W2, SPR_BOSS_W3, SPR_BOSS_W4,
    SPR_BOSS_SHOOT1, SPR_BOSS_SHOOT2, SPR_BOSS_SHOOT3, SPR_BOSS_DEAD,

    SPR_BOSS_DIE1, SPR_BOSS_DIE2, SPR_BOSS_DIE3,

//
// Schabbs
//
    SPR_SCHABB_W1, SPR_SCHABB_W2, SPR_SCHABB_W3, SPR_SCHABB_W4,
    SPR_SCHABB_SHOOT1, SPR_SCHABB_SHOOT2,

    SPR_SCHABB_DIE1, SPR_SCHABB_DIE2, SPR_SCHABB_DIE3, SPR_SCHABB_DEAD,
    SPR_HYPO1, SPR_HYPO2, SPR_HYPO3, SPR_HYPO4,

//
// Fake
//
    SPR_FAKE_W1, SPR_FAKE_W2, SPR_FAKE_W3, SPR_FAKE_W4,
    SPR_FAKE_SHOOT, SPR_FIRE1, SPR_FIRE2,

    SPR_FAKE_DIE1, SPR_FAKE_DIE2, SPR_FAKE_DIE3, SPR_FAKE_DIE4,
    SPR_FAKE_DIE5, SPR_FAKE_DEAD,

//
// Hitler
//
    SPR_MECHA_W1, SPR_MECHA_W2, SPR_MECHA_W3, SPR_MECHA_W4,
    SPR_MECHA_SHOOT1, SPR_MECHA_SHOOT2, SPR_MECHA_SHOOT3, SPR_MECHA_DEAD,

    SPR_MECHA_DIE1, SPR_MECHA_DIE2, SPR_MECHA_DIE3,

    SPR_HITLER_W1, SPR_HITLER_W2, SPR_HITLER_W3, SPR_HITLER_W4,
    SPR_HITLER_SHOOT1, SPR_HITLER_SHOOT2, SPR_HITLER_SHOOT3, SPR_HITLER_DEAD,

    SPR_HITLER_DIE1, SPR_HITLER_DIE2, SPR_HITLER_DIE3, SPR_HITLER_DIE4,
    SPR_HITLER_DIE5, SPR_HITLER_DIE6, SPR_HITLER_DIE7,

//
// Giftmacher
//
    SPR_GIFT_W1, SPR_GIFT_W2, SPR_GIFT_W3, SPR_GIFT_W4,
    SPR_GIFT_SHOOT1, SPR_GIFT_SHOOT2,

    SPR_GIFT_DIE1, SPR_GIFT_DIE2, SPR_GIFT_DIE3, SPR_GIFT_DEAD,


//
// Rocket, smoke and small explosion
//
    SPR_ROCKET_1, SPR_ROCKET_2, SPR_ROCKET_3, SPR_ROCKET_4,
    SPR_ROCKET_5, SPR_ROCKET_6, SPR_ROCKET_7, SPR_ROCKET_8,

    SPR_SMOKE_1, SPR_SMOKE_2, SPR_SMOKE_3, SPR_SMOKE_4,
    SPR_BOOM_1, SPR_BOOM_2, SPR_BOOM_3,

//
// Angel of Death's DeathSparks(tm)
//
    SPR_HROCKET_1, SPR_HROCKET_2, SPR_HROCKET_3, SPR_HROCKET_4,
    SPR_HROCKET_5, SPR_HROCKET_6, SPR_HROCKET_7, SPR_HROCKET_8,

    SPR_HSMOKE_1, SPR_HSMOKE_2, SPR_HSMOKE_3, SPR_HSMOKE_4,
    SPR_HBOOM_1, SPR_HBOOM_2, SPR_HBOOM_3,

    SPR_SPARK1, SPR_SPARK2, SPR_SPARK3, SPR_SPARK4,



//
// Gretel
//
    SPR_GRETEL_W1, SPR_GRETEL_W2, SPR_GRETEL_W3, SPR_GRETEL_W4,
    SPR_GRETEL_SHOOT1, SPR_GRETEL_SHOOT2, SPR_GRETEL_SHOOT3, SPR_GRETEL_DEAD,

    SPR_GRETEL_DIE1, SPR_GRETEL_DIE2, SPR_GRETEL_DIE3,

//
// Fat Face
//
    SPR_FAT_W1, SPR_FAT_W2, SPR_FAT_W3, SPR_FAT_W4,
    SPR_FAT_SHOOT1, SPR_FAT_SHOOT2, SPR_FAT_SHOOT3, SPR_FAT_SHOOT4,

    SPR_FAT_DIE1, SPR_FAT_DIE2, SPR_FAT_DIE3, SPR_FAT_DEAD,

//
// bj
//
    SPR_BJ_W1, SPR_BJ_W2, SPR_BJ_W3, SPR_BJ_W4,
    SPR_BJ_JUMP1, SPR_BJ_JUMP2, SPR_BJ_JUMP3, SPR_BJ_JUMP4,


//
// SPEAR OF DESTINY
//

//
// Trans Grosse
//
    SPR_TRANS_W1, SPR_TRANS_W2, SPR_TRANS_W3, SPR_TRANS_W4,
    SPR_TRANS_SHOOT1, SPR_TRANS_SHOOT2, SPR_TRANS_SHOOT3, SPR_TRANS_DEAD,

    SPR_TRANS_DIE1, SPR_TRANS_DIE2, SPR_TRANS_DIE3,

//
// Wilhelm
//
    SPR_WILL_W1, SPR_WILL_W2, SPR_WILL_W3, SPR_WILL_W4,
    SPR_WILL_SHOOT1, SPR_WILL_SHOOT2, SPR_WILL_SHOOT3, SPR_WILL_SHOOT4,

    SPR_WILL_DIE1, SPR_WILL_DIE2, SPR_WILL_DIE3, SPR_WILL_DEAD,

//
// UberMutant
//
    SPR_UBER_W1, SPR_UBER_W2, SPR_UBER_W3, SPR_UBER_W4,
    SPR_UBER_SHOOT1, SPR_UBER_SHOOT2, SPR_UBER_SHOOT3, SPR_UBER_SHOOT4,

    SPR_UBER_DIE1, SPR_UBER_DIE2, SPR_UBER_DIE3, SPR_UBER_DIE4,
    SPR_UBER_DEAD,

//
// Death Knight
//
    SPR_DEATH_W1, SPR_DEATH_W2, SPR_DEATH_W3, SPR_DEATH_W4,
    SPR_DEATH_SHOOT1, SPR_DEATH_SHOOT2, SPR_DEATH_SHOOT3, SPR_DEATH_SHOOT4,

    SPR_DEATH_DIE1, SPR_DEATH_DIE2, SPR_DEATH_DIE3, SPR_DEATH_DIE4,
    SPR_DEATH_DIE5, SPR_DEATH_DIE6, SPR_DEATH_DEAD,

//
// Ghost
//
    SPR_SPECTRE_W1, SPR_SPECTRE_W2, SPR_SPECTRE_W3, SPR_SPECTRE_W4,
    SPR_SPECTRE_F1, SPR_SPECTRE_F2, SPR_SPECTRE_F3, SPR_SPECTRE_F4,

//
// Angel of Death
//
    SPR_ANGEL_W1, SPR_ANGEL_W2, SPR_ANGEL_W3, SPR_ANGEL_W4,
    SPR_ANGEL_SHOOT1, SPR_ANGEL_SHOOT2, SPR_ANGEL_TIRED1, SPR_ANGEL_TIRED2,

    SPR_ANGEL_DIE1, SPR_ANGEL_DIE2, SPR_ANGEL_DIE3, SPR_ANGEL_DIE4,
    SPR_ANGEL_DIE5, SPR_ANGEL_DIE6, SPR_ANGEL_DIE7, SPR_ANGEL_DEAD,



//
// player attack frames
//
    SPR_KNIFEREADY, SPR_KNIFEATK1, SPR_KNIFEATK2, SPR_KNIFEATK3,
    SPR_KNIFEATK4,

    SPR_PISTOLREADY, SPR_PISTOLATK1, SPR_PISTOLATK2, SPR_PISTOLATK3,
    SPR_PISTOLATK4,

    SPR_MACHINEGUNREADY, SPR_MACHINEGUNATK1, SPR_MACHINEGUNATK2, MACHINEGUNATK3,
    SPR_MACHINEGUNATK4,

    SPR_CHAINREADY, SPR_CHAINATK1, SPR_CHAINATK2, SPR_CHAINATK3,
    SPR_CHAINATK4,

};




#define SPRT_ONE_TEX    BIT( 1 )
#define SPRT_NO_ROT     BIT( 2 )
#define SPRT_CHG_POS    BIT( 3 )
#define SPRT_CHG_TEX    BIT( 4 )
#define SPRT_REMOVE     BIT( 5 )

typedef struct sprite_s {
    vec3_t position;
    int x, y, ang;
// very clever to make it not just (x>>TILE_SHIFT)
// but also (x>>TILE_SHIFT)-1 if (x%TILEWIDTH)<TILE_HALF
// so we will check only 4 files instead of 9 as Carmack did!
    int tilex, tiley;
// controls appearence of this sprite:
// SPRT_ONE_TEX: use one texture for each rotation
// SPRT_NO_ROT: do not rotate sprite (fence)
// SPRT_CHG_POS
// SPRT_CHG_TEX
// SPRT_REMOVE
    int flags;
// 8 textures: one for each rotation phase!
// if SPRT_ONE_TEX flag use tex with index 0!

    // these are Wolf sprite texture numbers, not OpenGL sprites
    // or indexes in the textureManager list
    int tex[ 8 ];

} sprite_t;

// total sprites on level in a moment
#define MAX_SPRITES 1024

typedef struct visobj_s {
    int dist; // how we will sort them
    int x, y; // position in game space! (for OpenGL)
    int tex;  // texture to draw; renderer must know what to draw by this number!

} visobj_t;

extern visobj_t vislist[];
void Sprite_Reset (void);

void Sprite_RemoveSprite (int sprite_id);
int Sprite_GetNewSprite (void);
void Sprite_SetPos (int sprite_id, int x, int y, int angle);
void Sprite_SetTex (int sprite_id, int index, int tex);
int Sprite_CreateVisList (void);


#endif /* __WOLF_SPRITES_H__ */

