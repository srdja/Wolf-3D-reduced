/*

    Copyright (C) 2004-2013 Michael Liebscher <johnnycanuck@users.sourceforge.net>
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
 * \file wolf_act_stat.c
 * \brief Wolfenstein 3-D entity management.
 * \author Michael Liebscher
 * \date 2004-2013
 * \author DarkOne the Hacker
 * \date 2000-2002
 * \note Portion of this code was derived from NewWolf, and was originally written by DarkOne the Hacker.
 *       Portion of this code was derived from Wolfenstein 3-D, and was originally written by Id Software, Inc.
 */

#include "wolf_act_stat.h"

/*
    1-if object can be rotated, 0 if one sprite for every direction
    base object's state texture if rotation is on facing player
    after how man frames change state to .next_state
    what to do every frame
    what to do once per state
    next state
*/

stateinfo objstate[ NUMENEMIES ][ NUMSTATES ] = {
    // en_guard,
    {
        { 1, SPR_GRD_S_1,    0, T_Stand, NULL, st_stand }, // st_stand,

        { 1, SPR_GRD_W1_1,  20, T_Path, NULL, st_path1s }, // st_path1,
        { 1, SPR_GRD_W1_1,  5,  NULL,   NULL, st_path2  }, // st_path1s,
        { 1, SPR_GRD_W2_1,  15, T_Path, NULL, st_path3  }, // st_path2,
        { 1, SPR_GRD_W3_1,  20, T_Path, NULL, st_path3s }, // st_path3,
        { 1, SPR_GRD_W3_1,  5,  NULL,   NULL, st_path4  }, // st_path3s,
        { 1, SPR_GRD_W4_1,  15, T_Path, NULL, st_path1  }, // st_path4,

        { 0, SPR_GRD_PAIN_1,    10, NULL,   NULL, st_chase1},// st_pain,
        { 0, SPR_GRD_PAIN_2,    10, NULL,   NULL, st_chase1},// st_pain1,

        { 0, SPR_GRD_SHOOT1,    20, NULL,   NULL,   st_shoot2},// st_shoot1,
        { 0, SPR_GRD_SHOOT2,    20, NULL,   T_Shoot, st_shoot3}, // st_shoot2,
        { 0, SPR_GRD_SHOOT3,    20, NULL,   NULL,   st_chase1},// st_shoot3,

        { 0, SPR_DEMO,  0, NULL,    NULL, st_chase1 }, // st_shoot4,
        { 0, SPR_DEMO,  0, NULL,    NULL, st_chase1 }, // st_shoot5,
        { 0, SPR_DEMO,  0, NULL,    NULL, st_chase1 }, // st_shoot6,
        { 0, SPR_DEMO,  0, NULL,    NULL, st_chase1 }, // st_shoot7,
        { 0, SPR_DEMO,  0, NULL,    NULL, st_chase1 }, // st_shoot8,
        { 0, SPR_DEMO,  0, NULL,    NULL, st_chase1 }, // st_shoot9,

        { 1, SPR_GRD_W1_1, 10, T_Chase, NULL, st_chase1s }, // st_chase1,
        { 1, SPR_GRD_W1_1,  3, NULL,    NULL, st_chase2  }, // st_chase1s,
        { 1, SPR_GRD_W2_1,  8, T_Chase, NULL, st_chase3  }, // st_chase2,
        { 1, SPR_GRD_W3_1, 10, T_Chase, NULL, st_chase3s }, // st_chase3,
        { 1, SPR_GRD_W3_1,  3, NULL,    NULL, st_chase4  }, // st_chase3s,
        { 1, SPR_GRD_W4_1,  8, T_Chase, NULL, st_chase1  }, // st_chase4,

        { 0, SPR_GRD_DIE_1, 15, NULL, A_DeathScream, st_die2 }, // st_die1,
        { 0, SPR_GRD_DIE_2, 15, NULL, NULL,         st_die3 }, // st_die2,
        { 0, SPR_GRD_DIE_3, 15, NULL, NULL,         st_dead }, // st_die3,

        { 0, SPR_DEMO,  0, NULL,     NULL, st_dead }, // st_die4,
        { 0, SPR_DEMO,  0, NULL,     NULL, st_dead }, // st_die5,
        { 0, SPR_DEMO,  0, NULL,     NULL, st_dead }, // st_die6,
        { 0, SPR_DEMO,  0, NULL,     NULL, st_dead }, // st_die7,
        { 0, SPR_DEMO,  0, NULL,     NULL, st_dead }, // st_die8,
        { 0, SPR_DEMO,  0, NULL,     NULL, st_dead }, // st_die9,

        { 0, SPR_GRD_DEAD,  0, NULL,     NULL, st_dead }, // st_dead
        ST_INFO_NULL // st_deathcam
    },
    // en_officer,
    {
        {1, SPR_OFC_S_1,     0, T_Stand, NULL, st_stand}, // st_stand,

        {1, SPR_OFC_W1_1,   20, T_Path,  NULL, st_path1s},// st_path1,
        {1, SPR_OFC_W1_1,  5, NULL  ,  NULL, st_path2}, // st_path1s,
        {1, SPR_OFC_W2_1, 15, T_Path,  NULL, st_path3}, // st_path2,
        {1, SPR_OFC_W3_1,   20, T_Path,  NULL, st_path3s},// st_path3,
        {1, SPR_OFC_W3_1,    5, NULL    ,    NULL, st_path4}, // st_path3s,
        {1, SPR_OFC_W4_1,   15, T_Path,  NULL, st_path1}, // st_path4,

        {0, SPR_OFC_PAIN_1, 10, NULL,    NULL, st_chase1},// st_pain,
        {0, SPR_OFC_PAIN_2, 10, NULL,    NULL, st_chase1},// st_pain1,

        {0, SPR_OFC_SHOOT1,  6, NULL,    NULL, st_shoot2},// st_shoot1,
        {0, SPR_OFC_SHOOT2, 20, NULL,    T_Shoot, st_shoot3},// st_shoot2,
        {0, SPR_OFC_SHOOT3, 10, NULL,    NULL, st_chase1},// st_shoot3,

        {0, SPR_DEMO,   0, NULL,     NULL, st_chase1},// st_shoot4,
        {0, SPR_DEMO,   0, NULL,     NULL, st_chase1},// st_shoot5,
        {0, SPR_DEMO,   0, NULL,     NULL, st_chase1},// st_shoot6,
        {0, SPR_DEMO,   0, NULL,     NULL, st_chase1},// st_shoot7,
        {0, SPR_DEMO,   0, NULL,     NULL, st_chase1},// st_shoot8,
        {0, SPR_DEMO,   0, NULL,     NULL, st_chase1},// st_shoot9,

        {1, SPR_OFC_W1_1, 10, T_Chase, NULL, st_chase1s},// st_chase1,
        {1, SPR_OFC_W1_1,  3, NULL   , NULL, st_chase2}, // st_chase1s,
        {1, SPR_OFC_W2_1,  8, T_Chase, NULL, st_chase3}, // st_chase2,
        {1, SPR_OFC_W3_1, 10, T_Chase, NULL, st_chase3s},// st_chase3,
        {1, SPR_OFC_W3_1,  3, NULL   , NULL, st_chase4}, // st_chase3s,
        {1, SPR_OFC_W4_1,  8, T_Chase, NULL, st_chase1}, // st_chase4,

        {0, SPR_OFC_DIE_1, 11, NULL, A_DeathScream, st_die2},// st_die1,
        {0, SPR_OFC_DIE_2, 11, NULL, NULL,          st_die3},// st_die2,
        {0, SPR_OFC_DIE_3, 11, NULL, NULL,          st_dead},// st_die3,

        {0, SPR_DEMO,   0, NULL,     NULL, st_dead},// st_die4,
        {0, SPR_DEMO,   0, NULL,     NULL, st_dead},// st_die5,
        {0, SPR_DEMO,   0, NULL,     NULL, st_dead},// st_die6,
        {0, SPR_DEMO,   0, NULL,     NULL, st_dead},// st_die7,
        {0, SPR_DEMO,   0, NULL,     NULL, st_dead},// st_die8,
        {0, SPR_DEMO,   0, NULL,     NULL, st_dead},// st_die9,

        {0, SPR_OFC_DEAD,   0, NULL,     NULL, st_dead}, // st_dead
        ST_INFO_NULL // st_deathcam
    },
    // en_ss,
    {
        {1, SPR_SS_S_1,  0, T_Stand, NULL, st_stand}, // st_stand,

        {1, SPR_SS_W1_1,    20, T_Path,  NULL, st_path1s},// st_path1,
        {1, SPR_SS_W1_1,  5, NULL   ,  NULL, st_path2}, // st_path1s,
        {1, SPR_SS_W2_1, 15, T_Path,  NULL, st_path3}, // st_path2,
        {1, SPR_SS_W3_1,    20, T_Path,  NULL, st_path3s},// st_path3,
        {1, SPR_SS_W3_1,     5, NULL    ,    NULL, st_path4}, // st_path3s,
        {1, SPR_SS_W4_1,    15, T_Path,  NULL, st_path1}, // st_path4,

        {0, SPR_SS_PAIN_1,  10, NULL,    NULL, st_chase1},// st_pain,
        {0, SPR_SS_PAIN_2,  10, NULL,    NULL, st_chase1},// st_pain1,

        {0, SPR_SS_SHOOT1, 20, NULL, NULL,      st_shoot2},// st_shoot1,
        {0, SPR_SS_SHOOT2, 20, NULL, T_Shoot, st_shoot3},// st_shoot2,
        {0, SPR_SS_SHOOT3, 10, NULL, NULL,      st_shoot4},// st_shoot3,
        {0, SPR_SS_SHOOT2, 10, NULL, T_Shoot, st_shoot5},// st_shoot4,
        {0, SPR_SS_SHOOT3, 10, NULL, NULL,      st_shoot6},// st_shoot5,
        {0, SPR_SS_SHOOT2, 10, NULL, T_Shoot, st_shoot7},// st_shoot6,
        {0, SPR_SS_SHOOT3, 10, NULL, NULL,      st_shoot8},// st_shoot7,
        {0, SPR_SS_SHOOT2, 10, NULL, T_Shoot, st_shoot9},// st_shoot8,
        {0, SPR_SS_SHOOT3, 10, NULL, NULL,      st_chase1},// st_shoot9,

        {1, SPR_SS_W1_1, 10, T_Chase, NULL, st_chase1s},// st_chase1,
        {1, SPR_SS_W1_1,  3, NULL    , NULL, st_chase2}, // st_chase1s,
        {1, SPR_SS_W2_1,  8, T_Chase, NULL, st_chase3}, // st_chase2,
        {1, SPR_SS_W3_1, 10, T_Chase, NULL, st_chase3s},// st_chase3,
        {1, SPR_SS_W3_1,  3, NULL    , NULL, st_chase4},     // st_chase3s,
        {1, SPR_SS_W4_1,  8, T_Chase, NULL, st_chase1}, // st_chase4,

        {0, SPR_SS_DIE_1, 15, NULL, A_DeathScream, st_die2},// st_die1,
        {0, SPR_SS_DIE_2, 15, NULL, NULL,                    st_die3},// st_die2,
        {0, SPR_SS_DIE_3, 15, NULL, NULL,                    st_dead},// st_die3,

        {0, SPR_DEMO,   0, NULL,     NULL, st_dead},// st_die4,
        {0, SPR_DEMO,   0, NULL,     NULL, st_dead},// st_die5,
        {0, SPR_DEMO,   0, NULL,     NULL, st_dead},// st_die6,
        {0, SPR_DEMO,   0, NULL,     NULL, st_dead},// st_die7,
        {0, SPR_DEMO,   0, NULL,     NULL, st_dead},// st_die8,
        {0, SPR_DEMO,   0, NULL,     NULL, st_dead},// st_die9,

        {0, SPR_SS_DEAD,    0, NULL,     NULL, st_dead}, // st_dead
        ST_INFO_NULL // st_deathcam
    },
    // en_dog,
    {
        {0, SPR_DEMO,   0, NULL, NULL, st_stand}, // st_stand,

        {1, SPR_DOG_W1_1,   20, T_Path,  NULL, st_path1s},// st_path1,
        {1, SPR_DOG_W1_1,  5, NULL  ,  NULL, st_path2}, // st_path1s,
        {1, SPR_DOG_W2_1, 15, T_Path,  NULL, st_path3}, // st_path2,
        {1, SPR_DOG_W3_1,   20, T_Path,  NULL, st_path3s},// st_path3,
        {1, SPR_DOG_W3_1,    5, NULL    ,    NULL, st_path4}, // st_path3s,
        {1, SPR_DOG_W4_1,   15, T_Path,  NULL, st_path1}, // st_path4,

        {0, SPR_DEMO,   0, NULL, NULL, st_chase1},// st_pain,
        {0, SPR_DEMO,   0, NULL, NULL, st_chase1},// st_pain1,

        {0, SPR_DOG_JUMP1, 10, NULL,     NULL,   st_shoot2},// st_shoot1,
        {0, SPR_DOG_JUMP2, 10, NULL,     T_Bite, st_shoot3},// st_shoot2,
        {0, SPR_DOG_JUMP3, 10, NULL,     NULL,   st_shoot4},// st_shoot3,
        {0, SPR_DOG_JUMP1, 10, NULL,     NULL,   st_shoot5},// st_shoot4,
        {0, SPR_DOG_W1_1,    10, NULL,   NULL,   st_chase1},// st_shoot5,

        {0, SPR_DEMO,   0, NULL,     NULL, st_chase1},// st_shoot6,
        {0, SPR_DEMO,   0, NULL,     NULL, st_chase1},// st_shoot7,
        {0, SPR_DEMO,   0, NULL,     NULL, st_chase1},// st_shoot8,
        {0, SPR_DEMO,   0, NULL,     NULL, st_chase1},// st_shoot9,

        {1, SPR_DOG_W1_1, 10, T_DogChase, NULL, st_chase1s},// st_chase1,
        {1, SPR_DOG_W1_1,  3, NULL          , NULL, st_chase2}, // st_chase1s,
        {1, SPR_DOG_W2_1,  8, T_DogChase, NULL, st_chase3}, // st_chase2,
        {1, SPR_DOG_W3_1, 10, T_DogChase, NULL, st_chase3s},// st_chase3,
        {1, SPR_DOG_W3_1,  3, NULL          , NULL, st_chase4},  // st_chase3s,
        {1, SPR_DOG_W4_1,  8, T_DogChase, NULL, st_chase1}, // st_chase4,

        {0, SPR_DOG_DIE_1, 15, NULL, A_DeathScream, st_die2},// st_die1,
        {0, SPR_DOG_DIE_2, 15, NULL, NULL,                  st_die3},// st_die2,
        {0, SPR_DOG_DIE_3, 15, NULL, NULL,                  st_dead},// st_die3,

        {0, SPR_DEMO,   0, NULL, NULL, st_dead},// st_die4,
        {0, SPR_DEMO,   0, NULL, NULL, st_dead},// st_die5,
        {0, SPR_DEMO,   0, NULL, NULL, st_dead},// st_die6,
        {0, SPR_DEMO,   0, NULL, NULL, st_dead},// st_die7,
        {0, SPR_DEMO,   0, NULL, NULL, st_dead},// st_die8,
        {0, SPR_DEMO,   0, NULL, NULL, st_dead},// st_die9,

        {0, SPR_DOG_DEAD,   0, NULL, NULL, st_dead}, // st_dead
        ST_INFO_NULL // st_deathcam
    },
    // en_boss,
    {
        {0, SPR_BOSS_W1,    0, T_Stand, NULL, st_stand}, // st_stand,

        {0, SPR_DEMO, 0, NULL, NULL, st_path1s},// st_path1,
        {0, SPR_DEMO, 0, NULL, NULL, st_path2}, // st_path1s,
        {0, SPR_DEMO, 0, NULL, NULL, st_path3}, // st_path2,
        {0, SPR_DEMO, 0, NULL, NULL, st_path3s},// st_path3,
        {0, SPR_DEMO, 0, NULL, NULL, st_path4}, // st_path3s,
        {0, SPR_DEMO, 0, NULL, NULL, st_path1}, // st_path4,

        {0, SPR_DEMO,   0, NULL, NULL, st_chase1},// st_pain,
        {0, SPR_DEMO,   0, NULL, NULL, st_chase1},// st_pain1,

        {0, SPR_BOSS_SHOOT1, 30, NULL, NULL,        st_shoot2},// st_shoot1,
        {0, SPR_BOSS_SHOOT2, 10, NULL, T_Shoot, st_shoot3},// st_shoot2,
        {0, SPR_BOSS_SHOOT3, 10, NULL, T_Shoot, st_shoot4},// st_shoot3,
        {0, SPR_BOSS_SHOOT2, 10, NULL, T_Shoot, st_shoot5},// st_shoot4,
        {0, SPR_BOSS_SHOOT3, 10, NULL, T_Shoot, st_shoot6},// st_shoot5,
        {0, SPR_BOSS_SHOOT2, 10, NULL, T_Shoot, st_shoot7},// st_shoot6,
        {0, SPR_BOSS_SHOOT3, 10, NULL, T_Shoot, st_shoot8},// st_shoot7,
        {0, SPR_BOSS_SHOOT1, 10, NULL, NULL,        st_chase1},// st_shoot8,

        {0, SPR_DEMO,   0, NULL,     NULL, st_chase1},// st_shoot9,

        {0, SPR_BOSS_W1, 10, T_Chase, NULL, st_chase1s},// st_chase1,
        {0, SPR_BOSS_W1,  3, NULL       , NULL, st_chase2}, // st_chase1s,
        {0, SPR_BOSS_W2,  8, T_Chase, NULL, st_chase3}, // st_chase2,
        {0, SPR_BOSS_W3, 10, T_Chase, NULL, st_chase3s},// st_chase3,
        {0, SPR_BOSS_W3,  3, NULL       , NULL, st_chase4},  // st_chase3s,
        {0, SPR_BOSS_W4,  8, T_Chase, NULL, st_chase1}, // st_chase4,

        {0, SPR_BOSS_DIE1, 15, NULL, A_DeathScream, st_die2},// st_die1,
        {0, SPR_BOSS_DIE2, 15, NULL, NULL,                  st_die3},// st_die2,
        {0, SPR_BOSS_DIE3, 15, NULL, NULL,                  st_dead},// st_die3,

        {0, SPR_DEMO,   0, NULL, NULL, st_dead},// st_die4,
        {0, SPR_DEMO,   0, NULL, NULL, st_dead},// st_die5,
        {0, SPR_DEMO,   0, NULL, NULL, st_dead},// st_die6,
        {0, SPR_DEMO,   0, NULL, NULL, st_dead},// st_die7,
        {0, SPR_DEMO,   0, NULL, NULL, st_dead},// st_die8,
        {0, SPR_DEMO,   0, NULL, NULL, st_dead},// st_die9,

        {0, SPR_BOSS_DEAD, 0, NULL, NULL, st_dead}, // st_dead
        ST_INFO_NULL // st_deathcam
    },
    // en_schabbs,
    {
        {0, SPR_SCHABB_W1,  0, T_Stand, NULL, st_stand}, // st_stand,

        {0, SPR_DEMO, 0, NULL, NULL, st_path1s},// st_path1,
        {0, SPR_DEMO, 0, NULL, NULL, st_path2}, // st_path1s,
        {0, SPR_DEMO, 0, NULL, NULL, st_path3}, // st_path2,
        {0, SPR_DEMO, 0, NULL, NULL, st_path3s},// st_path3,
        {0, SPR_DEMO, 0, NULL, NULL, st_path4}, // st_path3s,
        {0, SPR_DEMO, 0, NULL, NULL, st_path1}, // st_path4,

        {0, SPR_DEMO,   0, NULL, NULL, st_chase1},// st_pain,
        {0, SPR_DEMO,   0, NULL, NULL, st_chase1},// st_pain1,

        {0, SPR_SCHABB_SHOOT1, 30, NULL, NULL,     st_shoot2},// st_shoot1,
        {0, SPR_SCHABB_SHOOT2, 10, NULL, T_Launch, st_chase1},// st_shoot2,

        {0, SPR_DEMO,   0, NULL,     NULL, st_chase1},// st_shoot3,
        {0, SPR_DEMO,   0, NULL,     NULL, st_chase1},//  st_shoot4,
        {0, SPR_DEMO,   0, NULL,     NULL, st_chase1},// st_shoot5,
        {0, SPR_DEMO,   0, NULL,     NULL, st_chase1},// st_shoot6,
        {0, SPR_DEMO,   0, NULL,     NULL, st_chase1},// st_shoot7,
        {0, SPR_DEMO,   0, NULL,     NULL, st_chase1},// st_shoot8,
        {0, SPR_DEMO,   0, NULL,     NULL, st_chase1},// st_shoot9,

        {0, SPR_SCHABB_W1, 10, T_BossChase, NULL, st_chase1s},// st_chase1,
        {0, SPR_SCHABB_W1,  3, NULL         , NULL, st_chase2}, // st_chase1s,
        {0, SPR_SCHABB_W2,  8, T_BossChase, NULL, st_chase3}, // st_chase2,
        {0, SPR_SCHABB_W3, 10, T_BossChase, NULL, st_chase3s},// st_chase3,
        {0, SPR_SCHABB_W3,  3, NULL         , NULL, st_chase4},  // st_chase3s,
        {0, SPR_SCHABB_W4,  8, T_BossChase, NULL, st_chase1}, // st_chase4,

        {0, SPR_SCHABB_W1,      10, NULL, A_DeathScream, st_die2},// st_die1,
        {0, SPR_SCHABB_W1,      10, NULL, NULL,                  st_die3},// st_die2,
        {0, SPR_SCHABB_DIE1,    10, NULL, NULL,                  st_die4},// st_die3,
        {0, SPR_SCHABB_DIE2,    10, NULL, NULL,                  st_die5},// st_die4,
        {0, SPR_SCHABB_DIE3,    10, NULL, NULL,                  st_dead},// st_die5,

        {0, SPR_DEMO,   0, NULL, NULL, st_dead},// st_die6,
        {0, SPR_DEMO,   0, NULL, NULL, st_dead},// st_die7,
        {0, SPR_DEMO,   0, NULL, NULL, st_dead},// st_die8,
        {0, SPR_DEMO,   0, NULL, NULL, st_dead},// st_die9,

        {0, SPR_SCHABB_DEAD, 20, NULL, A_StartDeathCam, st_dead}, // st_dead
        {0, SPR_SCHABB_W1,  1, NULL, NULL, st_die1} // st_deathcam
    },
    // en_fake,
    {
        {0, SPR_FAKE_W1,    0, T_Stand, NULL, st_stand}, // st_stand,

        {0, SPR_DEMO, 0, NULL, NULL, st_path1s},// st_path1,
        {0, SPR_DEMO, 0, NULL, NULL, st_path2}, // st_path1s,
        {0, SPR_DEMO, 0, NULL, NULL, st_path3}, // st_path2,
        {0, SPR_DEMO, 0, NULL, NULL, st_path3s},// st_path3,
        {0, SPR_DEMO, 0, NULL, NULL, st_path4}, // st_path3s,
        {0, SPR_DEMO, 0, NULL, NULL, st_path1}, // st_path4,

        {0, SPR_DEMO,   0, NULL, NULL, st_chase1},// st_pain,
        {0, SPR_DEMO,   0, NULL, NULL, st_chase1},// st_pain1,

        {0, SPR_FAKE_SHOOT, 8, NULL, T_Launch, st_shoot2},// st_shoot1,
        {0, SPR_FAKE_SHOOT, 8, NULL, T_Launch, st_shoot3},// st_shoot2,
        {0, SPR_FAKE_SHOOT, 8, NULL, T_Launch, st_shoot4},// st_shoot3,
        {0, SPR_FAKE_SHOOT, 8, NULL, T_Launch, st_shoot5},// st_shoot4,
        {0, SPR_FAKE_SHOOT, 8, NULL, T_Launch, st_shoot6},// st_shoot4,
        {0, SPR_FAKE_SHOOT, 8, NULL, T_Launch, st_shoot7},// st_shoot4,
        {0, SPR_FAKE_SHOOT, 8, NULL, T_Launch, st_shoot8},// st_shoot4,
        {0, SPR_FAKE_SHOOT, 8, NULL, T_Launch, st_shoot9},// st_shoot4,
        {0, SPR_FAKE_SHOOT, 8, NULL, NULL,           st_chase1},// st_shoot4,

        {0, SPR_FAKE_W1, 10, T_Fake, NULL, st_chase1s},// st_chase1,
        {0, SPR_FAKE_W1,  3, NULL  , NULL, st_chase2}, // st_chase1s,
        {0, SPR_FAKE_W2,  8, T_Fake, NULL, st_chase3}, // st_chase2,
        {0, SPR_FAKE_W3, 10, T_Fake, NULL, st_chase3s},// st_chase3,
        {0, SPR_FAKE_W3,  3, NULL  , NULL, st_chase4}, // st_chase3s,
        {0, SPR_FAKE_W4,  8, T_Fake, NULL, st_chase1}, // st_chase4,

        {0, SPR_FAKE_DIE1, 10, NULL, A_DeathScream, st_die2},// st_die1,
        {0, SPR_FAKE_DIE2, 10, NULL, NULL,                  st_die3},// st_die2,
        {0, SPR_FAKE_DIE3, 10, NULL, NULL,                  st_die4},// st_die3,
        {0, SPR_FAKE_DIE4, 10, NULL, NULL,                  st_die5},// st_die4,
        {0, SPR_FAKE_DIE5, 10, NULL, NULL,                  st_dead},// st_die5,

        {0, SPR_DEMO,   0, NULL, NULL, st_dead},// st_die6,
        {0, SPR_DEMO,   0, NULL, NULL, st_dead},// st_die7,
        {0, SPR_DEMO,   0, NULL, NULL, st_dead},// st_die8,
        {0, SPR_DEMO,   0, NULL, NULL, st_dead},// st_die9,

        {0, SPR_FAKE_DEAD, 0, NULL, NULL, st_dead}, // st_dead
        ST_INFO_NULL // st_deathcam
    },
    // en_hitler, (mecha)
    {
        {0, SPR_MECHA_W1,   0, T_Stand, NULL, st_stand}, // st_stand,

        {0, SPR_DEMO, 0, NULL, NULL, st_path1s},// st_path1,
        {0, SPR_DEMO, 0, NULL, NULL, st_path2}, // st_path1s,
        {0, SPR_DEMO, 0, NULL, NULL, st_path3}, // st_path2,
        {0, SPR_DEMO, 0, NULL, NULL, st_path3s},// st_path3,
        {0, SPR_DEMO, 0, NULL, NULL, st_path4}, // st_path3s,
        {0, SPR_DEMO, 0, NULL, NULL, st_path1}, // st_path4,

        {0, SPR_DEMO,   0, NULL, NULL, st_chase1},// st_pain,
        {0, SPR_DEMO,   0, NULL, NULL, st_chase1},// st_pain1,

        {0, SPR_MECHA_SHOOT1, 30, NULL, NULL, st_shoot2},// st_shoot1,
        {0, SPR_MECHA_SHOOT2, 10, NULL, T_Shoot, st_shoot3},// st_shoot2,
        {0, SPR_MECHA_SHOOT3, 10, NULL, T_Shoot, st_shoot4},// st_shoot3,
        {0, SPR_MECHA_SHOOT2, 10, NULL, T_Shoot, st_shoot5},// st_shoot4,
        {0, SPR_MECHA_SHOOT3, 10, NULL, T_Shoot, st_shoot6},// st_shoot5,
        {0, SPR_MECHA_SHOOT2, 10, NULL, T_Shoot, st_chase1},// st_shoot6,

        {0, SPR_DEMO,   0, NULL, NULL, st_shoot8},// st_shoot7,
        {0, SPR_DEMO,   0, NULL, NULL, st_shoot9},// st_shoot8,
        {0, SPR_DEMO,   0, NULL, NULL, st_chase1},// st_shoot9,

        {0, SPR_MECHA_W1, 10, T_Chase, A_MechaSound, st_chase1s},// st_chase1,
        {0, SPR_MECHA_W1,  6, NULL   , NULL, st_chase2}, // st_chase1s,
        {0, SPR_MECHA_W2,  8, T_Chase, NULL, st_chase3}, // st_chase2,
        {0, SPR_MECHA_W3, 10, T_Chase, A_MechaSound, st_chase3s},// st_chase3,
        {0, SPR_MECHA_W3,  6, NULL   , NULL, st_chase4},     // st_chase3s,
        {0, SPR_MECHA_W4,  8, T_Chase, NULL, st_chase1}, // st_chase4,

        {0, SPR_MECHA_DIE1, 10, NULL, A_DeathScream, st_die2},// st_die1,
        {0, SPR_MECHA_DIE2, 10, NULL, NULL,                  st_die3},// st_die2,
        {0, SPR_MECHA_DIE3, 10, NULL, A_HitlerMorph, st_dead},// st_die3,

        {0, SPR_DEMO,   0, NULL, NULL, st_dead},// st_die4,
        {0, SPR_DEMO,   0, NULL, NULL, st_dead},// st_die5,
        {0, SPR_DEMO,   0, NULL, NULL, st_dead},// st_die6,
        {0, SPR_DEMO,   0, NULL, NULL, st_dead},// st_die7,
        {0, SPR_DEMO,   0, NULL, NULL, st_dead},// st_die8,
        {0, SPR_DEMO,   0, NULL, NULL, st_dead},// st_die9,

        {0, SPR_MECHA_DEAD, 0, NULL, NULL, st_dead}, // st_dead
        ST_INFO_NULL // st_deathcam
    },
    // en_hitler,
    {
        {0, SPR_DEMO,   0, NULL, NULL, st_stand}, // st_stand,

        {0, SPR_DEMO, 0, NULL, NULL, st_path1s},// st_path1,
        {0, SPR_DEMO, 0, NULL, NULL, st_path2}, // st_path1s,
        {0, SPR_DEMO, 0, NULL, NULL, st_path3}, // st_path2,
        {0, SPR_DEMO, 0, NULL, NULL, st_path3s},// st_path3,
        {0, SPR_DEMO, 0, NULL, NULL, st_path4}, // st_path3s,
        {0, SPR_DEMO, 0, NULL, NULL, st_path1}, // st_path4,

        {0, SPR_DEMO,   0, NULL, NULL, st_chase1},// st_pain,
        {0, SPR_DEMO,   0, NULL, NULL, st_chase1},// st_pain1,

        {0, SPR_HITLER_SHOOT1, 30, NULL, NULL, st_shoot2},// st_shoot1,
        {0, SPR_HITLER_SHOOT2, 10, NULL, T_Shoot, st_shoot3},// st_shoot2,
        {0, SPR_HITLER_SHOOT3, 10, NULL, T_Shoot, st_shoot4},// st_shoot3,
        {0, SPR_HITLER_SHOOT2, 10, NULL, T_Shoot, st_shoot5},// st_shoot4,
        {0, SPR_HITLER_SHOOT3, 10, NULL, T_Shoot, st_shoot6},// st_shoot5,
        {0, SPR_HITLER_SHOOT2, 10, NULL, T_Shoot, st_chase1},// st_shoot6,

        {0, SPR_DEMO,   0, NULL, NULL, st_shoot8},// st_shoot7,
        {0, SPR_DEMO,   0, NULL, NULL, st_shoot9},// st_shoot8,
        {0, SPR_DEMO,   0, NULL, NULL, st_chase1},// st_shoot9,

        {0, SPR_HITLER_W1, 6, T_Chase, NULL, st_chase1s}, // st_chase1,
        {0, SPR_HITLER_W1, 4, NULL   , NULL, st_chase2},  // st_chase1s,
        {0, SPR_HITLER_W2, 2, T_Chase, NULL, st_chase3},  // st_chase2,
        {0, SPR_HITLER_W3, 6, T_Chase, NULL, st_chase3s}, // st_chase3,
        {0, SPR_HITLER_W3, 4, NULL      , NULL, st_chase4}, // st_chase3s,
        {0, SPR_HITLER_W4, 2, T_Chase, NULL, st_chase1},  // st_chase4,

        {0, SPR_HITLER_W1,    1, NULL, A_DeathScream,  st_die2},// st_die1,
        {0, SPR_HITLER_W1,   10, NULL, NULL, st_die3},// st_die2,
        {0, SPR_HITLER_DIE1, 10, NULL, A_Slurpie, st_die4},// st_die3,
        {0, SPR_HITLER_DIE2, 10, NULL, NULL, st_die5},// st_die4,
        {0, SPR_HITLER_DIE3, 10, NULL, NULL, st_die6},// st_die5,
        {0, SPR_HITLER_DIE4, 10, NULL, NULL, st_die7},// st_die6,
        {0, SPR_HITLER_DIE5, 10, NULL, NULL, st_die8},// st_die7,
        {0, SPR_HITLER_DIE6, 10, NULL, NULL, st_die9},// st_die8,
        {0, SPR_HITLER_DIE7, 10, NULL, NULL, st_dead},// st_die9,

        {0, SPR_HITLER_DEAD, 20, NULL, A_StartDeathCam, st_dead}, // st_dead
        {0, SPR_HITLER_W1,  1, NULL, NULL, st_die1} // st_deathcam
    },
    // en_mutant,
    {
        {1, SPR_MUT_S_1,     0, T_Stand, NULL, st_stand}, // st_stand,

        {1, SPR_MUT_W1_1,   20, T_Path,  NULL, st_path1s},// st_path1,
        {1, SPR_MUT_W1_1,  5, NULL  ,  NULL, st_path2}, // st_path1s,
        {1, SPR_MUT_W2_1, 15, T_Path,  NULL, st_path3}, // st_path2,
        {1, SPR_MUT_W3_1,   20, T_Path,  NULL, st_path3s},// st_path3,
        {1, SPR_MUT_W3_1,    5, NULL    ,    NULL, st_path4}, // st_path3s,
        {1, SPR_MUT_W4_1,   15, T_Path,  NULL, st_path1}, // st_path4,

        {0, SPR_MUT_PAIN_1, 10, NULL,    NULL, st_chase1},// st_pain,
        {0, SPR_MUT_PAIN_2, 10, NULL,    NULL, st_chase1},// st_pain1,

        {0, SPR_MUT_SHOOT1,  6, NULL, T_Shoot, st_shoot2}, // st_shoot1,
        {0, SPR_MUT_SHOOT2, 20, NULL, NULL,      st_shoot3}, // st_shoot2,
        {0, SPR_MUT_SHOOT3, 10, NULL, T_Shoot, st_shoot4}, // st_shoot3,
        {0, SPR_MUT_SHOOT4, 20, NULL, NULL,      st_chase1}, // st_shoot4,

        {0, SPR_DEMO,   0, NULL,     NULL, st_chase1},// st_shoot5,
        {0, SPR_DEMO,   0, NULL,     NULL, st_chase1},// st_shoot6,
        {0, SPR_DEMO,   0, NULL,     NULL, st_chase1},// st_shoot7,
        {0, SPR_DEMO,   0, NULL,     NULL, st_chase1},// st_shoot8,
        {0, SPR_DEMO,   0, NULL,     NULL, st_chase1},// st_shoot9,

        {1, SPR_MUT_W1_1, 10, T_Chase, NULL, st_chase1s},// st_chase1,
        {1, SPR_MUT_W1_1,  3, NULL   , NULL, st_chase2}, // st_chase1s,
        {1, SPR_MUT_W2_1,  8, T_Chase, NULL, st_chase3}, // st_chase2,
        {1, SPR_MUT_W3_1, 10, T_Chase, NULL, st_chase3s},// st_chase3,
        {1, SPR_MUT_W3_1,  3, NULL   , NULL, st_chase4},     // st_chase3s,
        {1, SPR_MUT_W4_1,  8, T_Chase, NULL, st_chase1}, // st_chase4,

        {0, SPR_MUT_DIE_1, 7, NULL, A_DeathScream, st_die2},// st_die1,
        {0, SPR_MUT_DIE_2, 7, NULL, NULL,                    st_die3},// st_die2,
        {0, SPR_MUT_DIE_3, 7, NULL, NULL,                    st_die4},// st_die3,
        {0, SPR_MUT_DIE_4, 7, NULL, NULL,                    st_dead},// st_die4,

        {0, SPR_DEMO,   0, NULL,     NULL, st_dead},// st_die5,
        {0, SPR_DEMO,   0, NULL,     NULL, st_dead},// st_die6,
        {0, SPR_DEMO,   0, NULL,     NULL, st_dead},// st_die7,
        {0, SPR_DEMO,   0, NULL,     NULL, st_dead},// st_die8,
        {0, SPR_DEMO,   0, NULL,     NULL, st_dead},// st_die9,

        {0, SPR_MUT_DEAD,   0, NULL,     NULL, st_dead}, // st_dead
        ST_INFO_NULL // st_deathcam
    },
    // en_blinky,
    {
        {0, SPR_DEMO,   0, NULL, NULL, st_stand}, // st_stand,

        {0, SPR_DEMO, 0, NULL, NULL, st_path1s},// st_path1,
        {0, SPR_DEMO, 0, NULL, NULL, st_path2}, // st_path1s,
        {0, SPR_DEMO, 0, NULL, NULL, st_path3}, // st_path2,
        {0, SPR_DEMO, 0, NULL, NULL, st_path3s},// st_path3,
        {0, SPR_DEMO, 0, NULL, NULL, st_path4}, // st_path3s,
        {0, SPR_DEMO, 0, NULL, NULL, st_path1}, // st_path4,

        {0, SPR_DEMO,   0, NULL, NULL, st_chase1},// st_pain,
        {0, SPR_DEMO,   0, NULL, NULL, st_chase1},// st_pain1,

        {0, SPR_DEMO, 0, NULL, NULL, st_shoot2},// st_shoot1,
        {0, SPR_DEMO, 0, NULL, NULL, st_shoot3},// st_shoot2,
        {0, SPR_DEMO, 0, NULL, NULL, st_shoot4},// st_shoot3,
        {0, SPR_DEMO, 0, NULL, NULL, st_shoot5},// st_shoot4,
        {0, SPR_DEMO, 0, NULL, NULL, st_shoot6},// st_shoot5,
        {0, SPR_DEMO, 0, NULL, NULL, st_chase1},// st_shoot6,
        {0, SPR_DEMO,   0, NULL, NULL, st_shoot8},// st_shoot7,
        {0, SPR_DEMO,   0, NULL, NULL, st_shoot9},// st_shoot8,
        {0, SPR_DEMO,   0, NULL, NULL, st_chase1},// st_shoot9,

        {0, SPR_BLINKY_W1, 10, T_Ghosts, NULL, st_chase2},// st_chase1,
        {0, SPR_DEMO,  0, NULL, NULL, st_chase2},           // st_chase1s,
        {0, SPR_BLINKY_W2, 10, T_Ghosts, NULL, st_chase1},// st_chase2,

        {0, SPR_DEMO, 0, NULL, NULL, st_chase3s},// st_chase3,
        {0, SPR_DEMO, 0, NULL, NULL, st_chase4},     // st_chase3s,
        {0, SPR_DEMO, 0, NULL, NULL, st_chase1}, // st_chase4,

        {0, SPR_DEMO, 10, NULL, NULL, st_die2},// st_die1,
        {0, SPR_DEMO, 10, NULL, NULL, st_die3},// st_die2,
        {0, SPR_DEMO, 10, NULL, NULL, st_dead},// st_die3,
        {0, SPR_DEMO,   0, NULL, NULL, st_dead},// st_die4,
        {0, SPR_DEMO,   0, NULL, NULL, st_dead},// st_die5,
        {0, SPR_DEMO,   0, NULL, NULL, st_dead},// st_die6,
        {0, SPR_DEMO,   0, NULL, NULL, st_dead},// st_die7,
        {0, SPR_DEMO,   0, NULL, NULL, st_dead},// st_die8,
        {0, SPR_DEMO,   0, NULL, NULL, st_dead},// st_die9,

        {0, SPR_DEMO, 0, NULL, NULL, st_dead}, // st_dead
        ST_INFO_NULL // st_deathcam
    },
    // en_clyde,
    {
        {0, SPR_DEMO,   0, NULL, NULL, st_stand}, // st_stand,

        {0, SPR_DEMO, 0, NULL, NULL, st_path1s},// st_path1,
        {0, SPR_DEMO, 0, NULL, NULL, st_path2}, // st_path1s,
        {0, SPR_DEMO, 0, NULL, NULL, st_path3}, // st_path2,
        {0, SPR_DEMO, 0, NULL, NULL, st_path3s},// st_path3,
        {0, SPR_DEMO, 0, NULL, NULL, st_path4}, // st_path3s,
        {0, SPR_DEMO, 0, NULL, NULL, st_path1}, // st_path4,

        {0, SPR_DEMO,   0, NULL, NULL, st_chase1},// st_pain,
        {0, SPR_DEMO,   0, NULL, NULL, st_chase1},// st_pain1,

        {0, SPR_DEMO, 0, NULL, NULL, st_shoot2},// st_shoot1,
        {0, SPR_DEMO, 0, NULL, NULL, st_shoot3},// st_shoot2,
        {0, SPR_DEMO, 0, NULL, NULL, st_shoot4},// st_shoot3,
        {0, SPR_DEMO, 0, NULL, NULL, st_shoot5},// st_shoot4,
        {0, SPR_DEMO, 0, NULL, NULL, st_shoot6},// st_shoot5,
        {0, SPR_DEMO, 0, NULL, NULL, st_chase1},// st_shoot6,
        {0, SPR_DEMO,   0, NULL, NULL, st_shoot8},// st_shoot7,
        {0, SPR_DEMO,   0, NULL, NULL, st_shoot9},// st_shoot8,
        {0, SPR_DEMO,   0, NULL, NULL, st_chase1},// st_shoot9,

        {0, SPR_CLYDE_W1, 10, T_Ghosts, NULL, st_chase2},// st_chase1,
        {0, SPR_DEMO,  0, NULL, NULL, st_chase2},           // st_chase1s,
        {0, SPR_CLYDE_W2, 10, T_Ghosts, NULL, st_chase1},// st_chase2,

        {0, SPR_DEMO, 0, NULL, NULL, st_chase3s},// st_chase3,
        {0, SPR_DEMO, 0, NULL, NULL, st_chase4},     // st_chase3s,
        {0, SPR_DEMO, 0, NULL, NULL, st_chase1}, // st_chase4,

        {0, SPR_DEMO, 10, NULL, NULL, st_die2},// st_die1,
        {0, SPR_DEMO, 10, NULL, NULL, st_die3},// st_die2,
        {0, SPR_DEMO, 10, NULL, NULL, st_dead},// st_die3,
        {0, SPR_DEMO,   0, NULL, NULL, st_dead},// st_die4,
        {0, SPR_DEMO,   0, NULL, NULL, st_dead},// st_die5,
        {0, SPR_DEMO,   0, NULL, NULL, st_dead},// st_die6,
        {0, SPR_DEMO,   0, NULL, NULL, st_dead},// st_die7,
        {0, SPR_DEMO,   0, NULL, NULL, st_dead},// st_die8,
        {0, SPR_DEMO,   0, NULL, NULL, st_dead},// st_die9,

        {0, SPR_DEMO, 0, NULL, NULL, st_dead}, // st_dead
        ST_INFO_NULL // st_deathcam
    },
    // en_pinky,
    {
        {0, SPR_DEMO,   0, NULL, NULL, st_stand}, // st_stand,

        {0, SPR_DEMO, 0, NULL, NULL, st_path1s},// st_path1,
        {0, SPR_DEMO, 0, NULL, NULL, st_path2}, // st_path1s,
        {0, SPR_DEMO, 0, NULL, NULL, st_path3}, // st_path2,
        {0, SPR_DEMO, 0, NULL, NULL, st_path3s},// st_path3,
        {0, SPR_DEMO, 0, NULL, NULL, st_path4}, // st_path3s,
        {0, SPR_DEMO, 0, NULL, NULL, st_path1}, // st_path4,

        {0, SPR_DEMO,   0, NULL, NULL, st_chase1},// st_pain,
        {0, SPR_DEMO,   0, NULL, NULL, st_chase1},// st_pain1,

        {0, SPR_DEMO, 0, NULL, NULL, st_shoot2},// st_shoot1,
        {0, SPR_DEMO, 0, NULL, NULL, st_shoot3},// st_shoot2,
        {0, SPR_DEMO, 0, NULL, NULL, st_shoot4},// st_shoot3,
        {0, SPR_DEMO, 0, NULL, NULL, st_shoot5},// st_shoot4,
        {0, SPR_DEMO, 0, NULL, NULL, st_shoot6},// st_shoot5,
        {0, SPR_DEMO, 0, NULL, NULL, st_chase1},// st_shoot6,
        {0, SPR_DEMO,   0, NULL, NULL, st_shoot8},// st_shoot7,
        {0, SPR_DEMO,   0, NULL, NULL, st_shoot9},// st_shoot8,
        {0, SPR_DEMO,   0, NULL, NULL, st_chase1},// st_shoot9,

        {0, SPR_PINKY_W1, 10, T_Ghosts, NULL, st_chase2},// st_chase1,
        {0, SPR_DEMO,  0, NULL, NULL, st_chase2},           // st_chase1s,
        {0, SPR_PINKY_W2, 10, T_Ghosts, NULL, st_chase1},// st_chase2,

        {0, SPR_DEMO, 0, NULL, NULL, st_chase3s},// st_chase3,
        {0, SPR_DEMO, 0, NULL, NULL, st_chase4},     // st_chase3s,
        {0, SPR_DEMO, 0, NULL, NULL, st_chase1}, // st_chase4,

        {0, SPR_DEMO, 10, NULL, NULL, st_die2},// st_die1,
        {0, SPR_DEMO, 10, NULL, NULL, st_die3},// st_die2,
        {0, SPR_DEMO, 10, NULL, NULL, st_dead},// st_die3,
        {0, SPR_DEMO,   0, NULL, NULL, st_dead},// st_die4,
        {0, SPR_DEMO,   0, NULL, NULL, st_dead},// st_die5,
        {0, SPR_DEMO,   0, NULL, NULL, st_dead},// st_die6,
        {0, SPR_DEMO,   0, NULL, NULL, st_dead},// st_die7,
        {0, SPR_DEMO,   0, NULL, NULL, st_dead},// st_die8,
        {0, SPR_DEMO,   0, NULL, NULL, st_dead},// st_die9,

        {0, SPR_DEMO, 0, NULL, NULL, st_dead}, // st_dead
        ST_INFO_NULL // st_deathcam
    },
    // en_inky,
    {
        {0, SPR_DEMO,   0, NULL, NULL, st_stand}, // st_stand,

        {0, SPR_DEMO, 0, NULL, NULL, st_path1s},// st_path1,
        {0, SPR_DEMO, 0, NULL, NULL, st_path2}, // st_path1s,
        {0, SPR_DEMO, 0, NULL, NULL, st_path3}, // st_path2,
        {0, SPR_DEMO, 0, NULL, NULL, st_path3s},// st_path3,
        {0, SPR_DEMO, 0, NULL, NULL, st_path4}, // st_path3s,
        {0, SPR_DEMO, 0, NULL, NULL, st_path1}, // st_path4,

        {0, SPR_DEMO,   0, NULL, NULL, st_chase1},// st_pain,
        {0, SPR_DEMO,   0, NULL, NULL, st_chase1},// st_pain1,

        {0, SPR_DEMO, 0, NULL, NULL, st_shoot2},// st_shoot1,
        {0, SPR_DEMO, 0, NULL, NULL, st_shoot3},// st_shoot2,
        {0, SPR_DEMO, 0, NULL, NULL, st_shoot4},// st_shoot3,
        {0, SPR_DEMO, 0, NULL, NULL, st_shoot5},// st_shoot4,
        {0, SPR_DEMO, 0, NULL, NULL, st_shoot6},// st_shoot5,
        {0, SPR_DEMO, 0, NULL, NULL, st_chase1},// st_shoot6,
        {0, SPR_DEMO,   0, NULL, NULL, st_shoot8},// st_shoot7,
        {0, SPR_DEMO,   0, NULL, NULL, st_shoot9},// st_shoot8,
        {0, SPR_DEMO,   0, NULL, NULL, st_chase1},// st_shoot9,

        {0, SPR_INKY_W1, 10, T_Ghosts, NULL, st_chase2},// st_chase1,
        {0, SPR_DEMO,  0, NULL, NULL, st_chase2},           // st_chase1s,
        {0, SPR_INKY_W2, 10, T_Ghosts, NULL, st_chase1},// st_chase2,

        {0, SPR_DEMO, 0, NULL, NULL, st_chase3s},// st_chase3,
        {0, SPR_DEMO, 0, NULL, NULL, st_chase4},     // st_chase3s,
        {0, SPR_DEMO, 0, NULL, NULL, st_chase1}, // st_chase4,

        {0, SPR_DEMO, 10, NULL, NULL, st_die2},// st_die1,
        {0, SPR_DEMO, 10, NULL, NULL, st_die3},// st_die2,
        {0, SPR_DEMO, 10, NULL, NULL, st_dead},// st_die3,
        {0, SPR_DEMO,   0, NULL, NULL, st_dead},// st_die4,
        {0, SPR_DEMO,   0, NULL, NULL, st_dead},// st_die5,
        {0, SPR_DEMO,   0, NULL, NULL, st_dead},// st_die6,
        {0, SPR_DEMO,   0, NULL, NULL, st_dead},// st_die7,
        {0, SPR_DEMO,   0, NULL, NULL, st_dead},// st_die8,
        {0, SPR_DEMO,   0, NULL, NULL, st_dead},// st_die9,

        {0, SPR_DEMO, 0, NULL, NULL, st_dead}, // st_dead
        ST_INFO_NULL // st_deathcam
    },
    // en_gretel,
    {
        {0, SPR_GRETEL_W1,  0, T_Stand, NULL, st_stand}, // st_stand,

        {0, SPR_DEMO, 0, NULL, NULL, st_path1s},// st_path1,
        {0, SPR_DEMO, 0, NULL, NULL, st_path2}, // st_path1s,
        {0, SPR_DEMO, 0, NULL, NULL, st_path3}, // st_path2,
        {0, SPR_DEMO, 0, NULL, NULL, st_path3s},// st_path3,
        {0, SPR_DEMO, 0, NULL, NULL, st_path4}, // st_path3s,
        {0, SPR_DEMO, 0, NULL, NULL, st_path1}, // st_path4,

        {0, SPR_DEMO,   0, NULL, NULL, st_chase1},// st_pain,
        {0, SPR_DEMO,   0, NULL, NULL, st_chase1},// st_pain1,

        {0, SPR_GRETEL_SHOOT1, 30, NULL, NULL,      st_shoot2},// st_shoot1,
        {0, SPR_GRETEL_SHOOT2, 10, NULL, T_Shoot, st_shoot3},// st_shoot2,
        {0, SPR_GRETEL_SHOOT3, 10, NULL, T_Shoot, st_shoot4},// st_shoot3,
        {0, SPR_GRETEL_SHOOT2, 10, NULL, T_Shoot,   st_shoot5},// st_shoot4,
        {0, SPR_GRETEL_SHOOT3, 10, NULL, T_Shoot,   st_shoot6},// st_shoot5,
        {0, SPR_GRETEL_SHOOT2, 10, NULL, T_Shoot, st_shoot7},// st_shoot6,
        {0, SPR_GRETEL_SHOOT3, 10, NULL, T_Shoot, st_shoot8},// st_shoot7,
        {0, SPR_GRETEL_SHOOT1, 10, NULL, NULL,      st_chase1},// st_shoot8,

        {0, SPR_DEMO,   0, NULL,     NULL, st_chase1},// st_shoot9,

        {0, SPR_GRETEL_W1, 10, T_Chase, NULL, st_chase1s},// st_chase1,
        {0, SPR_GRETEL_W1,  3, NULL     , NULL, st_chase2}, // st_chase1s,
        {0, SPR_GRETEL_W2,  8, T_Chase, NULL, st_chase3}, // st_chase2,
        {0, SPR_GRETEL_W3, 10, T_Chase, NULL, st_chase3s},// st_chase3,
        {0, SPR_GRETEL_W3,  3, NULL     , NULL, st_chase4},  // st_chase3s,
        {0, SPR_GRETEL_W4,  8, T_Chase, NULL, st_chase1}, // st_chase4,

        {0, SPR_GRETEL_DIE1, 15, NULL, A_DeathScream, st_die2},// st_die1,
        {0, SPR_GRETEL_DIE2, 15, NULL, NULL,                    st_die3},// st_die2,
        {0, SPR_GRETEL_DIE3, 15, NULL, NULL,                    st_dead},// st_die3,

        {0, SPR_DEMO,   0, NULL, NULL, st_dead},// st_die4,
        {0, SPR_DEMO,   0, NULL, NULL, st_dead},// st_die5,
        {0, SPR_DEMO,   0, NULL, NULL, st_dead},// st_die6,
        {0, SPR_DEMO,   0, NULL, NULL, st_dead},// st_die7,
        {0, SPR_DEMO,   0, NULL, NULL, st_dead},// st_die8,
        {0, SPR_DEMO,   0, NULL, NULL, st_dead},// st_die9,

        {0, SPR_GRETEL_DEAD, 0, NULL, NULL, st_dead}, // st_dead
        ST_INFO_NULL // st_deathcam
    },
    // en_gift,
    {
        {0, SPR_GIFT_W1,    0, T_Stand, NULL, st_stand}, // st_stand,

        {0, SPR_DEMO, 0, NULL, NULL, st_path1s},// st_path1,
        {0, SPR_DEMO, 0, NULL, NULL, st_path2}, // st_path1s,
        {0, SPR_DEMO, 0, NULL, NULL, st_path3}, // st_path2,
        {0, SPR_DEMO, 0, NULL, NULL, st_path3s},// st_path3,
        {0, SPR_DEMO, 0, NULL, NULL, st_path4}, // st_path3s,
        {0, SPR_DEMO, 0, NULL, NULL, st_path1}, // st_path4,

        {0, SPR_DEMO,   0, NULL, NULL, st_chase1},// st_pain,
        {0, SPR_DEMO,   0, NULL, NULL, st_chase1},// st_pain1,

        {0, SPR_GIFT_SHOOT1, 30, NULL, NULL,     st_shoot2},// st_shoot1,
        {0, SPR_GIFT_SHOOT2, 10, NULL, T_Launch, st_chase1},// st_shoot2,

        {0, SPR_DEMO,   0, NULL,     NULL, st_chase1},// st_shoot3,
        {0, SPR_DEMO,   0, NULL,     NULL, st_chase1},//  st_shoot4,
        {0, SPR_DEMO,   0, NULL,     NULL, st_chase1},// st_shoot5,
        {0, SPR_DEMO,   0, NULL,     NULL, st_chase1},// st_shoot6,
        {0, SPR_DEMO,   0, NULL,     NULL, st_chase1},// st_shoot7,
        {0, SPR_DEMO,   0, NULL,     NULL, st_chase1},// st_shoot8,
        {0, SPR_DEMO,   0, NULL,     NULL, st_chase1},// st_shoot9,

        {0, SPR_GIFT_W1, 10, T_BossChase, NULL, st_chase1s},// st_chase1,
        {0, SPR_GIFT_W1,  3, NULL           , NULL, st_chase2}, // st_chase1s,
        {0, SPR_GIFT_W2,  8, T_BossChase, NULL, st_chase3}, // st_chase2,
        {0, SPR_GIFT_W3, 10, T_BossChase, NULL, st_chase3s},// st_chase3,
        {0, SPR_GIFT_W3,  3, NULL           , NULL, st_chase4},  // st_chase3s,
        {0, SPR_GIFT_W4,  8, T_BossChase, NULL, st_chase1}, // st_chase4,

        {0, SPR_GIFT_W1,     10, NULL, A_DeathScream, st_die2},// st_die1,
        {0, SPR_GIFT_W1,     10, NULL, NULL,                     st_die3},// st_die2,
        {0, SPR_GIFT_DIE1, 10, NULL, NULL,                   st_die4},// st_die3,
        {0, SPR_GIFT_DIE2, 10, NULL, NULL,                   st_die5},// st_die4,
        {0, SPR_GIFT_DIE3, 10, NULL, NULL,                   st_dead},// st_die5,

        {0, SPR_DEMO,   0, NULL, NULL, st_dead},// st_die6,
        {0, SPR_DEMO,   0, NULL, NULL, st_dead},// st_die7,
        {0, SPR_DEMO,   0, NULL, NULL, st_dead},// st_die8,
        {0, SPR_DEMO,   0, NULL, NULL, st_dead},// st_die9,

        {0, SPR_GIFT_DEAD, 20, NULL, A_StartDeathCam, st_dead}, // st_dead
        {0, SPR_GIFT_W1,    1, NULL, NULL, st_die1} // st_deathcam
    },
    // en_fat,
    {
        {0, SPR_FAT_W1, 0, T_Stand, NULL, st_stand}, // st_stand,

        {0, SPR_DEMO, 0, NULL, NULL, st_path1s},// st_path1,
        {0, SPR_DEMO, 0, NULL, NULL, st_path2}, // st_path1s,
        {0, SPR_DEMO, 0, NULL, NULL, st_path3}, // st_path2,
        {0, SPR_DEMO, 0, NULL, NULL, st_path3s},// st_path3,
        {0, SPR_DEMO, 0, NULL, NULL, st_path4}, // st_path3s,
        {0, SPR_DEMO, 0, NULL, NULL, st_path1}, // st_path4,

        {0, SPR_DEMO,   0, NULL, NULL, st_chase1},// st_pain,
        {0, SPR_DEMO,   0, NULL, NULL, st_chase1},// st_pain1,

        {0, SPR_FAT_SHOOT1, 30, NULL, NULL,              st_shoot2},// st_shoot1,
        {0, SPR_FAT_SHOOT2, 10, NULL, T_Launch, st_shoot3},// st_shoot2,
        {0, SPR_FAT_SHOOT3, 10, NULL,   T_Shoot, st_shoot4},// st_shoot3,
        {0, SPR_FAT_SHOOT4, 10, NULL,   T_Shoot, st_shoot5},//  st_shoot4,
        {0, SPR_FAT_SHOOT3, 10, NULL,   T_Shoot, st_shoot6},// st_shoot5,
        {0, SPR_FAT_SHOOT4, 10, NULL,   T_Shoot, st_chase1},// st_shoot6,

        {0, SPR_DEMO,   0, NULL,     NULL, st_chase1},// st_shoot7,
        {0, SPR_DEMO,   0, NULL,     NULL, st_chase1},// st_shoot8,
        {0, SPR_DEMO,   0, NULL,     NULL, st_chase1},// st_shoot9,

        {0, SPR_FAT_W1, 10, T_BossChase, NULL, st_chase1s},// st_chase1,
        {0, SPR_FAT_W1,  3, NULL       , NULL, st_chase2}, // st_chase1s,
        {0, SPR_FAT_W2,  8, T_BossChase, NULL, st_chase3}, // st_chase2,
        {0, SPR_FAT_W3, 10, T_BossChase, NULL, st_chase3s},// st_chase3,
        {0, SPR_FAT_W3,  3, NULL       , NULL, st_chase4},   // st_chase3s,
        {0, SPR_FAT_W4,  8, T_BossChase, NULL, st_chase1}, // st_chase4,

        {0, SPR_FAT_W1,     10, NULL, A_DeathScream, st_die2},// st_die1,
        {0, SPR_FAT_W1,     10, NULL, NULL,                  st_die3},// st_die2,
        {0, SPR_FAT_DIE1, 10, NULL, NULL,                    st_die4},// st_die3,
        {0, SPR_FAT_DIE2,   10, NULL, NULL,                  st_die5},// st_die4,
        {0, SPR_FAT_DIE3,   10, NULL, NULL,                  st_dead},// st_die5,

        {0, SPR_DEMO,   0, NULL, NULL, st_dead},// st_die6,
        {0, SPR_DEMO,   0, NULL, NULL, st_dead},// st_die7,
        {0, SPR_DEMO,   0, NULL, NULL, st_dead},// st_die8,
        {0, SPR_DEMO,   0, NULL, NULL, st_dead},// st_die9,

        {0, SPR_FAT_DEAD, 20, NULL, A_StartDeathCam, st_dead}, // st_dead
        {0, SPR_FAT_W1, 1, NULL, NULL, st_die1} // st_deathcam
    },
// --- Projectiles
    // en_needle,
    {
        ST_INFO_NULL, // st_stand,

        {0, SPR_HYPO1, 6, T_Projectile, NULL, st_path2}, // st_path1,
        ST_INFO_NULL, // st_path1s,
        {0, SPR_HYPO2, 6, T_Projectile, NULL, st_path3}, // st_path2,
        {0, SPR_HYPO3, 6, T_Projectile, NULL, st_path4}, // st_path3,
        ST_INFO_NULL, // st_path3s,
        {0, SPR_HYPO4, 6, T_Projectile, NULL, st_path1}, // st_path4,

        ST_INFO_NULL,// st_pain,
        ST_INFO_NULL,// st_pain1,

        ST_INFO_NULL,// st_shoot1,
        ST_INFO_NULL,// st_shoot2,
        ST_INFO_NULL,// st_shoot3,
        ST_INFO_NULL,//  st_shoot4,
        ST_INFO_NULL,// st_shoot5,
        ST_INFO_NULL,// st_shoot6,

        ST_INFO_NULL,// st_shoot7,
        ST_INFO_NULL,// st_shoot8,
        ST_INFO_NULL,// st_shoot9,

        ST_INFO_NULL,// st_chase1,
        ST_INFO_NULL, // st_chase1s,
        ST_INFO_NULL, // st_chase2,
        ST_INFO_NULL,// st_chase3,
        ST_INFO_NULL,    // st_chase3s,
        ST_INFO_NULL, // st_chase4,

        ST_INFO_NULL, // st_die1,
        ST_INFO_NULL, // st_die2,
        ST_INFO_NULL, // st_die3,
        ST_INFO_NULL,// st_die4,
        ST_INFO_NULL,// st_die5,

        ST_INFO_NULL,// st_die6,
        ST_INFO_NULL,// st_die7,
        ST_INFO_NULL,// st_die8,
        ST_INFO_NULL,// st_die9,

        ST_INFO_NULL, // st_dead
        ST_INFO_NULL // st_deathcam
    },
    // en_fire,
    {
        ST_INFO_NULL, // st_stand,

        {0, SPR_FIRE1, 6, NULL, T_Projectile, st_path2}, // st_path1,
        ST_INFO_NULL, // st_path1s,
        {0, SPR_FIRE2, 6, NULL, T_Projectile, st_path1}, // st_path2,
        ST_INFO_NULL, // st_path3,
        ST_INFO_NULL, // st_path3s,
        ST_INFO_NULL, // st_path4,

        ST_INFO_NULL,// st_pain,
        ST_INFO_NULL,// st_pain1,

        ST_INFO_NULL,// st_shoot1,
        ST_INFO_NULL,// st_shoot2,
        ST_INFO_NULL,// st_shoot3,
        ST_INFO_NULL,//  st_shoot4,
        ST_INFO_NULL,// st_shoot5,
        ST_INFO_NULL,// st_shoot6,

        ST_INFO_NULL,// st_shoot7,
        ST_INFO_NULL,// st_shoot8,
        ST_INFO_NULL,// st_shoot9,

        ST_INFO_NULL,// st_chase1,
        ST_INFO_NULL, // st_chase1s,
        ST_INFO_NULL, // st_chase2,
        ST_INFO_NULL,// st_chase3,
        ST_INFO_NULL,    // st_chase3s,
        ST_INFO_NULL, // st_chase4,

        ST_INFO_NULL, // st_die1,
        ST_INFO_NULL, // st_die2,
        ST_INFO_NULL, // st_die3,
        ST_INFO_NULL,// st_die4,
        ST_INFO_NULL,// st_die5,

        ST_INFO_NULL,// st_die6,
        ST_INFO_NULL,// st_die7,
        ST_INFO_NULL,// st_die8,
        ST_INFO_NULL,// st_die9,

        ST_INFO_NULL, // st_dead
        ST_INFO_NULL // st_deathcam
    },
    // en_rocket,
    {
        {1, SPR_ROCKET_1, 3, T_Projectile, A_Smoke, st_stand}, // st_stand,

        ST_INFO_NULL,// st_path1,
        ST_INFO_NULL, // st_path1s,
        ST_INFO_NULL, // st_path2,
        ST_INFO_NULL,// st_path3,
        ST_INFO_NULL, // st_path3s,
        ST_INFO_NULL, // st_path4,

        ST_INFO_NULL,// st_pain,
        ST_INFO_NULL,// st_pain1,

        ST_INFO_NULL,// st_shoot1,
        ST_INFO_NULL,// st_shoot2,
        ST_INFO_NULL,// st_shoot3,
        ST_INFO_NULL,//  st_shoot4,
        ST_INFO_NULL,// st_shoot5,
        ST_INFO_NULL,// st_shoot6,

        ST_INFO_NULL,// st_shoot7,
        ST_INFO_NULL,// st_shoot8,
        ST_INFO_NULL,// st_shoot9,

        ST_INFO_NULL,// st_chase1,
        ST_INFO_NULL, // st_chase1s,
        ST_INFO_NULL, // st_chase2,
        ST_INFO_NULL,// st_chase3,
        ST_INFO_NULL,    // st_chase3s,
        ST_INFO_NULL, // st_chase4,

        {0, SPR_BOOM_1, 6, NULL, NULL, st_die2}, // st_die1,
        {0, SPR_BOOM_2, 6, NULL, NULL, st_die3}, // st_die2,
        {0, SPR_BOOM_3, 6, NULL, NULL, st_remove}, // st_die3,
        ST_INFO_NULL,// st_die4,
        ST_INFO_NULL,// st_die5,

        ST_INFO_NULL,// st_die6,
        ST_INFO_NULL,// st_die7,
        ST_INFO_NULL,// st_die8,
        ST_INFO_NULL,// st_die9,

        ST_INFO_NULL, // st_dead
        ST_INFO_NULL // st_deathcam
    },
    // en_smoke,
    {
        ST_INFO_NULL, // st_stand,

        ST_INFO_NULL, // st_path1,
        ST_INFO_NULL, // st_path1s,
        ST_INFO_NULL, // st_path2,
        ST_INFO_NULL, // st_path3,
        ST_INFO_NULL, // st_path3s,
        ST_INFO_NULL, // st_path4,

        ST_INFO_NULL, // st_pain,
        ST_INFO_NULL, // st_pain1,

        ST_INFO_NULL, // st_shoot1,
        ST_INFO_NULL, // st_shoot2,
        ST_INFO_NULL, // st_shoot3,
        ST_INFO_NULL, // st_shoot4,
        ST_INFO_NULL, // st_shoot5,
        ST_INFO_NULL, // st_shoot6,

        ST_INFO_NULL, // st_shoot7,
        ST_INFO_NULL, // st_shoot8,
        ST_INFO_NULL, // st_shoot9,

        ST_INFO_NULL, // st_chase1,
        ST_INFO_NULL, // st_chase1s,
        ST_INFO_NULL, // st_chase2,
        ST_INFO_NULL, // st_chase3,
        ST_INFO_NULL,   // st_chase3s,
        ST_INFO_NULL, // st_chase4,

        {0, SPR_SMOKE_1, 3, NULL, NULL, st_die2}, // st_die1,
        {0, SPR_SMOKE_2, 3, NULL, NULL, st_die3}, // st_die2,
        {0, SPR_SMOKE_3, 3, NULL, NULL, st_die4}, // st_die3,
        {0, SPR_SMOKE_4, 3, NULL, NULL, st_remove}, // st_die4,
        ST_INFO_NULL, // st_die5,

        ST_INFO_NULL, // st_die6,
        ST_INFO_NULL, // st_die7,
        ST_INFO_NULL, // st_die8,
        ST_INFO_NULL, // st_die9,

        ST_INFO_NULL, // st_dead
        ST_INFO_NULL // st_deathcam
    },
    // en_bj,
    {
        ST_INFO_NULL, // st_stand,

        {0, SPR_BJ_W1, 12, T_BJRun, NULL, st_path1s}, // st_path1,
        {0, SPR_BJ_W1,  3, NULL,      NULL, st_path2}, // st_path1s,
        {0, SPR_BJ_W2,  8, T_BJRun, NULL, st_path3}, // st_path2,
        {0, SPR_BJ_W3, 12, T_BJRun, NULL, st_path3s}, // st_path3,
        {0, SPR_BJ_W3,  3, NULL,      NULL, st_path4}, // st_path3s,
        {0, SPR_BJ_W4,  8, T_BJRun, NULL, st_path1}, // st_path4,

        ST_INFO_NULL, // st_pain,
        ST_INFO_NULL, // st_pain1,

        {0, SPR_BJ_JUMP1, 14, T_BJJump, NULL, st_shoot2}, // st_shoot1,
        {0, SPR_BJ_JUMP2, 14, T_BJJump, T_BJYell, st_shoot3}, // st_shoot2,
        {0, SPR_BJ_JUMP3, 14, T_BJJump, NULL, st_shoot4}, // st_shoot3,
        {0, SPR_BJ_JUMP4, 100, NULL, T_BJDone, st_shoot4}, // st_shoot4,
        ST_INFO_NULL, // st_shoot5,
        ST_INFO_NULL, // st_shoot6,

        ST_INFO_NULL, // st_shoot7,
        ST_INFO_NULL, // st_shoot8,
        ST_INFO_NULL, // st_shoot9,

        ST_INFO_NULL, // st_chase1,
        ST_INFO_NULL, // st_chase1s,
        ST_INFO_NULL, // st_chase2,
        ST_INFO_NULL, // st_chase3,
        ST_INFO_NULL,   // st_chase3s,
        ST_INFO_NULL, // st_chase4,

        ST_INFO_NULL, // st_die1,
        ST_INFO_NULL, // st_die2,
        ST_INFO_NULL, // st_die3,
        ST_INFO_NULL, // st_die4,
        ST_INFO_NULL, // st_die5,

        ST_INFO_NULL, // st_die6,
        ST_INFO_NULL, // st_die7,
        ST_INFO_NULL, // st_die8,
        ST_INFO_NULL, // st_die9,

        ST_INFO_NULL, // st_dead
        ST_INFO_NULL // st_deathcam
    }
};

int starthitpoints[ 4 ][ NUMENEMIES ] =
//
// BABY MODE
//
{
    {
        25,   // guards
        50, // officer
        100,    // SS
        1,  // dogs
        850,    // Hans
        850,    // Schabbs
        200,    // fake hitler
        800,    // mecha hitler
        500,  // hitler
        45, // mutants
        25, // ghosts
        25, // ghosts
        25, // ghosts
        25, // ghosts

        850,    // Gretel
        850,    // Gift
        850,    // Fat

// --- Projectiles
        0,      // en_needle,
        0,      // en_fire,
        0,      // en_rocket,
        0,      // en_smoke,
        100    // en_bj,
    },
    //
    // DON'T HURT ME MODE
    //
    {
        25,   // guards
        50, // officer
        100,    // SS
        1,  // dogs
        950,    // Hans
        950,    // Schabbs
        300,    // fake hitler
        950,    // mecha hitler
        700,    // hitler
        55, // mutants
        25, // ghosts
        25, // ghosts
        25, // ghosts
        25, // ghosts

        950,    // Gretel
        950,    // Gift
        950,    // Fat

// --- Projectiles
        0,      // en_needle,
        0,      // en_fire,
        0,      // en_rocket,
        0,      // en_smoke,
        100    // en_bj,
    },
    //
    // BRING 'EM ON MODE
    //
    {
            25,   // guards
            50, // officer
            100,    // SS
            1,  // dogs

            1050,   // Hans
            1550,   // Schabbs
            400,    // fake hitler
            1050,   // mecha hitler
            800,    // hitler

            55, // mutants
            25, // ghosts
            25, // ghosts
            25, // ghosts
            25, // ghosts

            1050,   // Gretel
            1050,   // Gift
            1050,   // Fat

// --- Projectiles
            0,      // en_needle,
            0,      // en_fire,
            0,      // en_rocket,
            0,      // en_smoke,
            100    // en_bj,
                 },
    //
    // DEATH INCARNATE MODE
    //
    {
        25,   // guards
        50, // officer
        100,    // SS
        1,  // dogs

        1200,   // Hans
        2400,   // Schabbs
        500,    // fake hitler
        1200,   // mecha hitler
        900,    // hitler

        65, // mutants
        25, // ghosts
        25, // ghosts
        25, // ghosts
        25, // ghosts

        1200,   // Gretel
        1200,   // Gift
        1200,   // Fat

// --- Projectiles
        0,      // en_needle,
        0,      // en_fire,
        0,      // en_rocket,
        0,      // en_smoke,
        100    // en_bj,
    }
};

// FIXME: unnecessary coupling
extern bool sound_initialized;

void CorrectBossDeathTimeouts()
{
    /*
    if (sound_initialized) {
        objstate[en_trans][st_die2].timeout = 105;
        objstate[en_uber][st_die2].timeout = 70;
        objstate[en_will][st_die2].timeout = 70;
        objstate[en_death][st_die2].timeout = 105;
        objstate[en_angel][st_die2].timeout = 105;
        objstate[en_schabbs][st_die2].timeout = 140;
        objstate[en_gift][st_die2].timeout = 140;
        objstate[en_fat][st_die2].timeout = 140;
        objstate[en_hitler][st_die2].timeout = 140;
    } else */{
        objstate[en_schabbs][st_die2].timeout = 5;
        objstate[en_gift][st_die2].timeout = 5;
        objstate[en_fat][st_die2].timeout = 5;
        objstate[en_hitler][st_die2].timeout = 5;
    }
}