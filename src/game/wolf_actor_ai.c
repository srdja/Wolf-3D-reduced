/*

    Copyright (C) 2004-2013 Michael Liebscher <johnnycanuck@users.sourceforge.net>
    Copyright (C) 2001 by DarkOne the Hacker

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
 * \brief Wolfenstein 3-D artificial intelligence.
 * \author Michael Liebscher
 * \date 2004-2013
 * \author DarkOne the Hacker
 * \date 2000-2002
 * \note Portion of this code was derived from NewWolf, and was originally written by DarkOne the Hacker.
 *       Portion of this code was derived from Wolfenstein 3-D, and was originally written by Id Software, Inc.
 */

#include "wolf_menu.h"

#include "wolf_actor_ai.h"
#include "wolf_player.h"
#include "wolf_local.h"
#include "../graphics/wolf_renderer.h"

#include "client.h"
#include "../graphics/opengl_local.h"

const char dsounds[ 7 ][ 32 ] = {
    "sfx/025.wav",
    "sfx/026.wav",
    "sfx/086.wav",
    "sfx/088.wav",
    "sfx/105.wav",
    "sfx/107.wav",
    "sfx/109.wav"
};

const char dsodsounds[ 7 ][ 32 ] = {
    "sfx/021.wav",
    "sfx/022.wav",
    "sfx/052.wav",
    "sfx/054.wav",
    "sfx/057.wav",
    "sfx/059.wav",
    "sfx/061.wav"
};


/**
 * \brief Do a death scream sound depending on actor type.
 * \param[in] self Valid Pointer to an entity_t structure.
 */
void A_DeathScream (entity_t *entity)
{
    switch (entity->type) {
    case en_mutant:
        if (g_version == SPEAROFDESTINY) {
            //Sound_StartSound (NULL, 1, CHAN_VOICE, Sound_RegisterSound ("sfx/033.wav"), 1, ATTN_NORM, 0);
        } else {
          //  Sound_StartSound (NULL, 1, CHAN_VOICE, Sound_RegisterSound ("sfx/037.wav"), 1, ATTN_NORM, 0);
        }

        break;

    case en_guard:
        if (g_version == SPEAROFDESTINY) {
         //   Sound_StartSound (NULL, 1, CHAN_VOICE, Sound_RegisterSound (dsodsounds[ US_RndT() % 6 ]), 1, ATTN_NORM, 0);
        } else {
         //   Sound_StartSound (NULL, 1, CHAN_VOICE, Sound_RegisterSound (dsounds[ US_RndT() % 6 ]), 1, ATTN_NORM, 0);
        }

        break;

    case en_officer:
        if (g_version == SPEAROFDESTINY) {
         //   Sound_StartSound (NULL, 1, CHAN_VOICE, Sound_RegisterSound ("sfx/046.wav"), 1, ATTN_NORM, 0);
        } else {
          //  Sound_StartSound (NULL, 1, CHAN_VOICE, Sound_RegisterSound ("sfx/074.wav"), 1, ATTN_NORM, 0);
        }

        break;

    case en_ss:
        if (g_version == SPEAROFDESTINY) {
          //  Sound_StartSound (NULL, 1, CHAN_VOICE, Sound_RegisterSound ("sfx/035.wav"), 1, ATTN_NORM, 0);
        } else {
          //  Sound_StartSound (NULL, 1, CHAN_VOICE, Sound_RegisterSound ("sfx/046.wav"), 1, ATTN_NORM, 0);
        }

        break;

    case en_dog:
        if (g_version == SPEAROFDESTINY) {
          //  Sound_StartSound (NULL, 1, CHAN_VOICE, Sound_RegisterSound ("sfx/031.wav"), 1, ATTN_NORM, 0);
        } else {
           // Sound_StartSound (NULL, 1, CHAN_VOICE, Sound_RegisterSound ("sfx/035.wav"), 1, ATTN_NORM, 0);
        }

        break;

    case en_boss:
       // Sound_StartSound (NULL, 1, CHAN_VOICE, Sound_RegisterSound ("sfx/019.wav"), 1, ATTN_NORM, 0);
        break;

    case en_schabbs:
        //Sound_StartSound (NULL, 1, CHAN_VOICE, Sound_RegisterSound ("sfx/061.wav"), 1, ATTN_NORM, 0);
        break;

    case en_fake:
       // Sound_StartSound (NULL, 1, CHAN_VOICE, Sound_RegisterSound ("sfx/069.wav"), 1, ATTN_NORM, 0);
        break;

    case en_mecha:
       // Sound_StartSound (NULL, 1, CHAN_VOICE, Sound_RegisterSound ("sfx/084.wav"), 1, ATTN_NORM, 0);
        break;

    case en_hitler:
       // Sound_StartSound (NULL, 1, CHAN_VOICE, Sound_RegisterSound ("sfx/044.wav"), 1, ATTN_NORM, 0);
        break;

    case en_gretel:
        //Sound_StartSound (NULL, 1, CHAN_VOICE, Sound_RegisterSound ("sfx/115.wav"), 1, ATTN_NORM, 0);
        break;

    case en_gift:
        //Sound_StartSound (NULL, 1, CHAN_VOICE, Sound_RegisterSound ("sfx/091.wav"), 1, ATTN_NORM, 0);
        break;

    case en_fat:
        //Sound_StartSound (NULL, 1, CHAN_VOICE, Sound_RegisterSound ("sfx/119.wav"), 1, ATTN_NORM, 0);
        break;

    case en_spectre:
        //Sound_StartSound (NULL, 1, CHAN_VOICE, Sound_RegisterSound ("lsfx/062.wav"), 1, ATTN_NORM, 0);
        break;

    case en_angel:
        //Sound_StartSound (NULL, 1, CHAN_VOICE, Sound_RegisterSound ("sfx/098.wav"), 1, ATTN_NORM, 0);
        break;

    case en_trans:
        //Sound_StartSound (NULL, 1, CHAN_VOICE, Sound_RegisterSound ("sfx/070.wav"), 1, ATTN_NORM, 0);
        break;

    case en_uber:
        //Sound_StartSound (NULL, 1, CHAN_VOICE, Sound_RegisterSound ("sfx/082.wav"), 1, ATTN_NORM, 0);
        break;

    case en_will:
        //Sound_StartSound (NULL, 1, CHAN_VOICE, Sound_RegisterSound ("sfx/072.wav"), 1, ATTN_NORM, 0);
        break;

    case en_death:
        //Sound_StartSound (NULL, 1, CHAN_VOICE, Sound_RegisterSound ("sfx/090.wav"), 1, ATTN_NORM, 0);
        break;
    }
}

/**
 * \brief Puts an actor into attack mode and possibly reverses the direction if the player is behind it.
 * \param[in] self Valid Pointer to an entity_t structure.
 */
void A_FirstSighting (entity_t *self)
{
    switch (self->type) {
    case en_guard:
        //Sound_StartSound (NULL, 1, CHAN_VOICE, Sound_RegisterSound ("sfx/001.wav"), 1, ATTN_NORM, 0);
        self->speed *= 3;   // go faster when chasing player
        break;

    case en_officer:
        if (g_version == SPEAROFDESTINY) {
         //   Sound_StartSound (NULL, 1, CHAN_VOICE, Sound_RegisterSound ("sfx/043.wav"), 1, ATTN_NORM, 0);
        } else {
          //  Sound_StartSound (NULL, 1, CHAN_VOICE, Sound_RegisterSound ("sfx/071.wav"), 1, ATTN_NORM, 0);
        }

        self->speed *= 5;   // go faster when chasing player
        break;

    case en_mutant:
        self->speed *= 3;   // go faster when chasing player
        break;

    case en_ss:
       // Sound_StartSound (NULL, 1, CHAN_VOICE, Sound_RegisterSound ("sfx/015.wav"), 1, ATTN_NORM, 0);
        self->speed *= 4;           // go faster when chasing player
        break;

    case en_dog:
       // Sound_StartSound (NULL, 1, CHAN_VOICE, Sound_RegisterSound ("sfx/002.wav"), 1, ATTN_NORM, 0);
        self->speed *= 2;           // go faster when chasing player
        break;

    case en_boss:
       // Sound_StartSound (NULL, 1, CHAN_VOICE, Sound_RegisterSound ("sfx/017.wav"), 1, ATTN_NORM, 0);
        self->speed = SPDPATROL * 3;    // go faster when chasing player
        break;

    case en_gretel:
       // Sound_StartSound (NULL, 1, CHAN_VOICE, Sound_RegisterSound ("sfx/112.wav"), 1, ATTN_NORM, 0);
        self->speed *= 3;           // go faster when chasing player
        break;

    case en_gift:
       // Sound_StartSound (NULL, 1, CHAN_VOICE, Sound_RegisterSound ("sfx/096.wav"), 1, ATTN_NORM, 0);
        self->speed *= 3;           // go faster when chasing player
        break;

    case en_fat:
        //Sound_StartSound (NULL, 1, CHAN_VOICE, Sound_RegisterSound ("sfx/102.wav"), 1, ATTN_NORM, 0);
        self->speed *= 3;           // go faster when chasing player
        break;

    case en_schabbs:
      //  Sound_StartSound (NULL, 1, CHAN_VOICE, Sound_RegisterSound ("sfx/065.wav"), 1, ATTN_NORM, 0);
        self->speed *= 3;           // go faster when chasing player
        break;

    case en_fake:
        //Sound_StartSound (NULL, 1, CHAN_VOICE, Sound_RegisterSound ("sfx/054.wav"), 1, ATTN_NORM, 0);
        self->speed *= 3;           // go faster when chasing player
        break;

    case en_mecha:
       // Sound_StartSound (NULL, 1, CHAN_VOICE, Sound_RegisterSound ("sfx/040.wav"), 1, ATTN_NORM, 0);
        self->speed *= 3;           // go faster when chasing player
        break;

    case en_hitler:
       // Sound_StartSound (NULL, 1, CHAN_VOICE, Sound_RegisterSound ("sfx/040.wav"), 1, ATTN_NORM, 0);
        self->speed *= 5;           // go faster when chasing player
        break;

    case en_blinky:
    case en_clyde:
    case en_pinky:
    case en_inky:
        self->speed *= 2;           // go faster when chasing player
        break;

//
// Spear of Destiny
//
    case en_spectre:
       // Sound_StartSound (NULL, 1, CHAN_VOICE, Sound_RegisterSound ("lsfx/003.wav"), 1, ATTN_NORM, 0);
        self->speed = 800;          // go faster when chasing player
        break;

    case en_angel:
        //Sound_StartSound (NULL, 1, CHAN_VOICE, Sound_RegisterSound ("sfx/095.wav"), 1, ATTN_NORM, 0);
        self->speed = 1536;         // go faster when chasing player
        break;

    case en_trans:
       // Sound_StartSound (NULL, 1, CHAN_VOICE, Sound_RegisterSound ("sfx/066.wav"), 1, ATTN_NORM, 0);
        self->speed = 1536;         // go faster when chasing player
        break;

    case en_uber:
        self->speed = 3000;         // go faster when chasing player
        break;

    case en_will:
       // Sound_StartSound (NULL, 1, CHAN_VOICE, Sound_RegisterSound ("sfx/073.wav"), 1, ATTN_NORM, 0);
        self->speed = 2048;         // go faster when chasing player
        break;

    case en_death:
        //Sound_StartSound (NULL, 1, CHAN_VOICE, Sound_RegisterSound ("sfx/085.wav"), 1, ATTN_NORM, 0);
        self->speed = 2048;         // go faster when chasing player
        break;

    default:
        return;
    }

    A_StateChange (self, st_chase1);

    if (self->waitfordoorx) {
        self->waitfordoorx = self->waitfordoory = 0;    // ignore the door opening command
    }

    self->dir = dir8_nodir;
    self->flags |= FL_ATTACKMODE | FL_FIRSTATTACK;
}

/**
 * \brief Actor has been killed, so give points and spawn powerups
 * \param[in] self Valid pointer to an entity_t structure.
 */
static void A_KillActor (entity_t *self)
{
    int tilex, tiley;

    tilex = self->tilex = self->x >> TILE_SHIFT; // drop item on center
    tiley = self->tiley = self->y >> TILE_SHIFT;

    switch (self->type) {
    case en_guard:
        PL_GivePoints (&Player, 100);
        Powerup_Spawn (tilex, tiley, pow_clip2);
        break;

    case en_officer:
        PL_GivePoints (&Player, 400);
        Powerup_Spawn (tilex, tiley, pow_clip2);
        break;

    case en_mutant:
        PL_GivePoints (&Player, 700);
        Powerup_Spawn (tilex, tiley, pow_clip2);
        break;

    case en_ss:
        PL_GivePoints (&Player, 500);

        if (Player.items & ITEM_WEAPON_3) { // have a schmeiser?
            Powerup_Spawn (tilex, tiley, pow_clip2);
        } else {
            Powerup_Spawn (tilex, tiley, pow_machinegun);
        }

        break;

    case en_dog:
        PL_GivePoints (&Player, 200);
        break;

    case en_boss:
        PL_GivePoints (&Player, 5000);
        Powerup_Spawn (tilex, tiley, pow_key1);
        break;

    case en_gretel:
        PL_GivePoints (&Player, 5000);
        Powerup_Spawn (tilex, tiley, pow_key1);
        break;

    case en_gift:
        PL_GivePoints (&Player, 5000);
        break;

    case en_fat:
        PL_GivePoints (&Player, 5000);
        break;

    case en_schabbs:
        PL_GivePoints (&Player, 5000);
        break;

    case en_fake:
        PL_GivePoints (&Player, 2000);
        break;

    case en_mecha:
        PL_GivePoints (&Player, 5000);
        break;

    case en_hitler:
        PL_GivePoints (&Player, 5000);
        break;

    case en_spectre:
        PL_GivePoints (&Player, 200);
        break;

    case en_angel:
        PL_GivePoints (&Player, 5000);
        break;

    case en_trans:
        PL_GivePoints (&Player, 5000);
        Powerup_Spawn (tilex, tiley, pow_key1);
        break;

    case en_uber:
        PL_GivePoints (&Player, 5000);
        Powerup_Spawn (tilex, tiley, pow_key1);
        break;

    case en_will:
        PL_GivePoints (&Player, 5000);
        Powerup_Spawn (tilex, tiley, pow_key1);
        break;

    case en_death:
        PL_GivePoints (&Player, 5000);
        Powerup_Spawn (tilex, tiley, pow_key1);
        break;
    }

    A_StateChange (self, st_die1);
    levelstate.killed_monsters++;
    self->flags &= ~FL_SHOOTABLE;
    self->flags |= FL_NONMARK;

}

/**
 * \brief Called when the player succesfully hits an enemy.
 * \param[in] self Valid pointer to an entity_t structure.
 * \param[in] damage Damage being dealt
 * \note Does damage points to enemy ob, either putting it into a stun frame or killing it.
 */
void A_DamageActor (entity_t *self, int damage)
{
    Player.madenoise = 1;

// do double damage if shooting a non attack mode actor
    if (! (self->flags & FL_ATTACKMODE)) {
        damage <<= 1;
    }

    self->health -= damage;

    if (self->health <= 0) {
        A_KillActor (self);
    } else {
        if (! (self->flags & FL_ATTACKMODE)) {
            A_FirstSighting (self);      // put into combat mode
        }

        switch (self->type) {    // dogs only have one hit point
        case en_guard:
        case en_officer:
        case en_mutant:
        case en_ss:
            if (self->health & 1) {
                A_StateChange (self, st_pain);
            } else {
                A_StateChange (self, st_pain1);
            }

            break;
        }
    }
}

/**
 * \brief Play MechaHitler sound.
 * \param[in] self Valid pointer to an entity_t structure.
 */
void A_MechaSound (entity_t *self)
{
    if (areabyplayer[ self->areanumber ]) {
        //Sound_StartSound (NULL, 1, CHAN_VOICE, Sound_RegisterSound ("sfx/080.wav"), 1, ATTN_NORM, 0);
    }
}

/**
 * \brief Play slurpie sound.
 * \param[in] self Valid pointer to an entity_t structure.
 */
void A_Slurpie (entity_t *self)
{
   // Sound_StartSound (NULL, 1, CHAN_VOICE, Sound_RegisterSound ("lsfx/061.wav"), 1, ATTN_NORM, 0);
}

/**
 * \brief Spawn Hitler when MechaHitler is dead.
 * \param[in] self Valid pointer to an entity_t structure.
 */
void A_HitlerMorph (entity_t *self)
{
    int hitpoints[ 4 ] = { 500, 700, 800, 900 };
    entity_t *hitler;

    hitler = GetNewActor();

    if (! hitler) {
        return;
    }

    hitler->x = self->x;//
    hitler->y = self->y;//
    hitler->distance = self->distance;
    hitler->tilex = self->tilex;//
    hitler->tiley = self->tiley;//
    hitler->angle = self->angle;//
    hitler->dir = self->dir;//
    hitler->health = hitpoints[skill ];
    hitler->areanumber = self->areanumber;
    hitler->state = st_chase1;//
    hitler->type = en_hitler; //
    hitler->speed = SPDPATROL * 5;//
    hitler->ticcount = 0;//
    hitler->flags = self->flags | FL_SHOOTABLE; //
    hitler->sprite = Sprite_GetNewSprite();

    levelstate.killed_monsters++;
}

///////////////////////////
//
//  Angel of Death
//
///////////////////////////

/*
    Angel can't shoot more then 3 sparks in a row.
    It will get tired!
*/

static int angel_temp = 0;

/**
 * \brief Play Angel of Death breathing sound.
 * \param[in] self Valid pointer to an entity_t structure.
 */
void A_Breathing (entity_t *self)
{
   // Sound_StartSound (NULL, 0, CHAN_VOICE, Sound_RegisterSound ("lsfx/080.wav"), 1, ATTN_NORM, 0);
}

/**
 * \brief Angel of Death is starting his attack
 * \param[in] self Valid pointer to an entity_t structure.
 */
void A_StartAttack (entity_t *self)
{
    angel_temp = 0;
}

/**
 * \brief Relaunch Angel of Death state either pain or chase player
 * \param[in] self Valid pointer to an entity_t structure.
 */
void A_Relaunch (entity_t *self)
{
    if (++angel_temp == 3) {
        A_StateChange (self, st_pain);
        return;
    }

    if (US_RndT() & 1) {
        A_StateChange (self, st_chase1);
        return;
    }
}


/**
 * \brief Player is victorious
 * \param[in] self Valid pointer to an entity_t structure.
 */
void A_Victory (entity_t *self)
{
    M_Intermission_f();
}

/**
 * \brief Entity is dormant state.
 * \param[in] self Valid pointer to an entity_t structure.
 */
void A_Dormant (entity_t *self)
{
    int deltax, deltay;
    int xl, xh, yl, yh, x, y, n;

    deltax = self->x - Player.position.origin[ 0 ];

    if (deltax < -MINACTORDIST || deltax > MINACTORDIST) {
        goto moveok;
    }

    deltay = self->y - Player.position.origin[ 1 ];

    if (deltay < -MINACTORDIST || deltay > MINACTORDIST) {
        goto moveok;
    }

    return;

moveok:
    xl = (self->x - MINDIST) >> TILE_SHIFT;
    xh = (self->x + MINDIST) >> TILE_SHIFT;
    yl = (self->y - MINDIST) >> TILE_SHIFT;
    yh = (self->y + MINDIST) >> TILE_SHIFT;

    for (y = yl ; y <= yh ; ++y)
        for (x = xl ; x <= xh ; ++x) {
            if (r_world->tilemap[ x ][ y ] & SOLID_TILE) {
                return;
            }

            for (n = 0 ; n < NumGuards ; ++n) {
                if (Guards[ n ].state >= st_die1) {
                    continue;
                }

                if (Guards[ n ].tilex == x && Guards[ n ].tiley == y) {
                    return; // another guard in path
                }
            }
        }

    self->flags |= FL_AMBUSH | FL_SHOOTABLE;
    self->flags &= ~FL_ATTACKMODE;
    self->dir = dir8_nodir;
    A_StateChange (self, st_path1);
}

static entity_t *deathcamEnt;
static int deathcamTime;
static int deathcamPhase;

/**
 * \brief Deathcam sequence controller
 */
static void M_LetsSeeThatAgain_Draw (void)
{
    float fangle;
    int dist;

    switch (deathcamPhase) {
    case 0:

        // do nothing for 100 VBLs, or 1429 ms
        if (ClientStatic.realtime >= deathcamTime + 1429) {
            deathcamPhase++;
            Player.playstate = ex_watchingdeathcam;
        }

        break;

    case 1:
        // orient camera and draw message
        // switch away after 300 VBLs, or 4286 ms
        // FIXME: this should be the original position where you killed the boss
        fangle = TransformPoint (deathcamEnt->x, deathcamEnt->y, Player.position.origin[0], Player.position.origin[1]);

        Player.position.angle = fangle;
        Player.position.pitch = 0;

        ClientState.viewangles[ YAW ] = RAD2FINE (Player.position.angle);
        ClientState.viewangles[ PITCH ] = RAD2FINE (Player.position.pitch);

        dist = 0x14000l;

        do {
            Player.position.origin[0] = (long) (deathcamEnt->x - dist * cos (fangle));
            Player.position.origin[1] = (long) (deathcamEnt->y - dist * sin (fangle));
            dist += 0x1000;

        } while (!PL_TryMove (&Player, r_world));

        R_Draw_Fill (0, 0, viddef.width, viddef.height, interbkgnd);

        R_put_line (0, 56, "LET'S SEE THAT AGAIN!");

        if (ClientStatic.realtime >= deathcamTime + 4286) {
            M_ForceMenuOff();
            A_StateChange (deathcamEnt, st_deathcam);
        }

        break;

    case 2:

        // do nothing for 100 VBLs, or 1429 ms
        if (ClientStatic.realtime >= deathcamTime + 1429) {
            M_ForceMenuOff();
            Player.playstate = ex_victory;
            M_Intermission_f();
        }

        break;
    }

    R_DrawHUD();
}

static const char *M_LetsSeeThatAgain_Key (int key)
{
    return NULL;
}

/**
 * \brief Enter the Deathcam sequence
 * \param[in] self Valid pointer to an entity_t structure.
 */
void A_StartDeathCam (entity_t *self)
{
    deathcamEnt = self;
    deathcamTime = ClientStatic.realtime;

    if (Player.playstate == ex_watchingdeathcam) {
        deathcamPhase = 2;
    } else {
        deathcamPhase = 0;
    }

    M_PushMenu (M_LetsSeeThatAgain_Draw, M_LetsSeeThatAgain_Key);
}

/**
 * \brief Emmit rocket smoke.
 * \param[in] self Valid pointer to an entity_t structure.
 */
void A_Smoke (entity_t *self)
{
    entity_t *smoke;

    smoke = GetNewActor();

    if (! smoke) {
        return;
    }

    smoke->x = self->x;
    smoke->y = self->y;
    smoke->tilex = self->tilex;
    smoke->tiley = self->tiley;
    smoke->state = st_die1;
    smoke->type = (self->type == en_hrocket) ? en_hsmoke : en_smoke;
    smoke->ticcount = 6;
    smoke->flags = FL_NEVERMARK;
    smoke->sprite = Sprite_GetNewSprite();
}

/**
 * \brief Called when projectile is in flight.
 * \param[in] self Valid pointer to an entity_t structure.
 * \param[in] lvl Valid Pointer to LevelData_t structure.
 * \return true if move ok, otherwise false.
 */
static bool ProjectileTryMove (entity_t *self, LevelData_t *lvl)
{
#define PROJSIZE    0x2000


    int xl, yl, xh, yh, x, y;

    xl = (self->x - PROJSIZE) >> TILE_SHIFT;
    yl = (self->y - PROJSIZE) >> TILE_SHIFT;

    xh = (self->x + PROJSIZE) >> TILE_SHIFT;
    yh = (self->y + PROJSIZE) >> TILE_SHIFT;

    // Checking for solid walls:
    for (y = yl ; y <= yh ; ++y) {
        for (x = xl ; x <= xh ; ++x) {
// FIXME: decide what to do with statics & Doors!
            if (lvl->tilemap[ x ][ y ] & (WALL_TILE | BLOCK_TILE)) {
                return false;
            }

            if (lvl->tilemap[ x ][ y ] & DOOR_TILE) {
                if (Door_Opened (&lvl->Doors, x, y) != DOOR_FULLOPEN) {
                    return false;
                }
            }
        }
    }

// FIXME: Projectile will fly through objects (even guards & columns) - must fix to create rocket launcher!
    return true;
}

/**
 * \brief Called when projectile is in flight.
 * \param[in] self Valid pointer to an entity_t structure.
 */
void T_Projectile (entity_t *self)
{
#define PROJECTILESIZE  0xC000


    int deltax, deltay, speed, damage;

    speed = self->speed * tics;

    deltax = (int) (speed * cos (FINE2RAD (self->angle)));
    deltay = (int) (speed * sin (FINE2RAD (self->angle)));

    if (deltax > TILE_GLOBAL) {
        deltax = TILE_GLOBAL;
    }

    if (deltax < -TILE_GLOBAL) {
        deltax = -TILE_GLOBAL; // my
    }

    if (deltay > TILE_GLOBAL) {
        deltay = TILE_GLOBAL;
    }

    if (deltay < -TILE_GLOBAL) {
        deltay = -TILE_GLOBAL; // my
    }

    self->x += deltax;
    self->y += deltay;

    deltax = ABS (self->x - Player.position.origin[ 0 ]);
    deltay = ABS (self->y - Player.position.origin[ 1 ]);

    if (! ProjectileTryMove (self, r_world)) {
        if (self->type == en_rocket || self->type == en_hrocket) {
            // rocket ran into obstacle, draw explosion!
            if (g_version == SPEAROFDESTINY) {
                //Sound_StartSound (NULL, 1, CHAN_WEAPON, Sound_RegisterSound ("lsfx/001.wav"), 1, ATTN_NORM, 0);
            } else {
               // Sound_StartSound (NULL, 1, CHAN_WEAPON, Sound_RegisterSound ("lsfx/086.wav"), 1, ATTN_NORM, 0);
            }

            A_StateChange (self, st_die1);
        } else {
            A_StateChange (self, st_remove);  // mark for removal
        }

        return;
    }

    if (deltax < PROJECTILESIZE && deltay < PROJECTILESIZE) {
        // hit the player
        switch (self->type) {
        case en_needle:
            damage = (US_RndT() >> 3) + 20;
            break;

        case en_rocket:
        case en_hrocket:
        case en_spark:
            damage = (US_RndT() >> 3) + 30;
            break;

        case en_fire:
            damage = (US_RndT() >> 3);
            break;

        default:
            damage = 0;
            break;
        }

        player_take_damage(&Player, self, damage);
        A_StateChange (self, st_remove);  // mark for removal
        return;
    }

    self->tilex = self->x >> TILE_SHIFT;
    self->tiley = self->y >> TILE_SHIFT;
}
