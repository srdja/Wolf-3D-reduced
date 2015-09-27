//
// Created by srdja on 9/26/15.
//

#include "../wolf_actors.h"
#include "../wolf_ai_com.h"
#include "../wolf_actor_ai.h"
#include "../wolf_act_stat.h"

static int hitpoints[4] = {25, 25, 25, 25};

stateinfo guard_states[NUMSTATES] = {
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
};

entity_t guard_spawn()
{

}

void guard_death_scream(entity_t *self)
{

}