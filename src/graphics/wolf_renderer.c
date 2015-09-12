/*

    Copyright (C) 2004-2012 Michael Liebscher <johnnycanuck@users.sourceforge.net>
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

/**
 * \file wolf_renderer.c
 * \brief Wolfenstein 3-D renderer.
 * \author Michael Liebscher
 * \date 2005-2012
 * \note Portion of this code was derived from Quake II, and was originally written by Id Software, Inc.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../graphics/texture_manager.h"
#include "../graphics/renderer.h"
#include "../graphics/video.h"
#include "../game/client.h"
#include "../graphics/color.h"
#include "../util/com_string.h"

#include "../game/wolf_local.h"
#include "../game/wolf_menu.h"
#include "../game/wolf_sprites.h"
#include "../game/wolf_player.h"

#include "../game/wolf_raycast.h"
#include "wolf_renderer.h"

extern viddef_t viddef;

LevelData_t *r_world;


static const char weaponnames[ 4 ][ 32 ] = {
    "pics/KNIFEPIC.tga",
    "pics/GUNPIC.tga",
    "pics/MACHINEGUNPIC.tga",
    "pics/GATLINGGUNPIC.tga"
};

static int32_t hud_x, hud_y;


/**
 * \brief Draws the heads-up-display
 */
void R_DrawHUD (void)
{
    int32_t w, h;
    uint32_t score = Player.score;

    TM_GetTextureSize (&w, &h, "pics/STATUSBARPIC.tga");
    hud_x = (viddef.width - w) >> 1;
    hud_y = viddef.height - h;
    R_Draw_Pic (hud_x, hud_y, "pics/STATUSBARPIC.tga");

    if (Player.items & ITEM_KEY_GOLD) {
        R_Draw_Pic (hud_x + 480, hud_y + 8, "pics/GOLDKEYPIC.tga");
    }

    if (Player.items & ITEM_KEY_SILVER) {
        R_Draw_Pic (hud_x + 480, hud_y + 40, "pics/SILVERKEYPIC.tga");
    }

    R_Draw_Pic (hud_x + 512, hud_y + 15, weaponnames[ Player.weapon ]);

    // Clamp score
    if (score > 999999) {
        score = 999999;
    }

    R_DrawNumber (hud_x + 48, hud_y + 32, levelstate.floornum + 1);
    R_DrawNumber (hud_x + 180, hud_y + 32, score);
    R_DrawNumber (hud_x + 224, hud_y + 32, Player.lives);
    R_DrawNumber (hud_x + 368, hud_y + 32, Player.health);
    R_DrawNumber (hud_x + 444, hud_y + 32, Player.ammo[AMMO_BULLETS]); // FIXME!
}

static const char mugshotnames[ 24 ][ 32 ] = {
    "pics/FACE1APIC.tga",
    "pics/FACE1BPIC.tga",
    "pics/FACE1CPIC.tga",

    "pics/FACE2APIC.tga",
    "pics/FACE2BPIC.tga",
    "pics/FACE2CPIC.tga",

    "pics/FACE3APIC.tga",
    "pics/FACE3BPIC.tga",
    "pics/FACE3CPIC.tga",

    "pics/FACE4APIC.tga",
    "pics/FACE4BPIC.tga",
    "pics/FACE4CPIC.tga",

    "pics/FACE5APIC.tga",
    "pics/FACE5BPIC.tga",
    "pics/FACE5CPIC.tga",

    "pics/FACE6APIC.tga",
    "pics/FACE6BPIC.tga",
    "pics/FACE6CPIC.tga",

    "pics/FACE7APIC.tga",
    "pics/FACE7BPIC.tga",
    "pics/FACE7CPIC.tga",

    "pics/FACE8APIC.tga"
};

static const char godmugshotnames[][ 32 ] = {
    "pics/GODMODEFACE0PIC.tga",
    "pics/GODMODEFACE1PIC.tga",
    "pics/GODMODEFACE2PIC.tga"
};


/**
 * \brief Draws BJ's face on the heads-up-display
 */
void R_DrawFace (void)
{
    Player.facecount += tics;

    if ((Player.face_gotgun || Player.face_ouch) && Player.facecount > 0) {
        // gotgun/ouch will set facecount to a negative number initially, go back
        // to normal face with random look after expired.
        Player.face_gotgun = false;
        Player.face_ouch = false;
    }

    if (Player.facecount > US_RndT()) {
        Player.face_gotgun = false;
        Player.face_ouch = false;
        Player.faceframe = US_RndT() >> 6;

        if (Player.faceframe == 3) {
            Player.faceframe = 0;
        }

        Player.facecount = 0;
    }


    if (Player.health) {
        if (g_version == SPEAROFDESTINY && Player.flags & FL_GODMODE) {
            R_Draw_Pic (hud_x + 272, hud_y + 8, godmugshotnames[ Player.faceframe ]);
        } else if (Player.face_gotgun) {
            R_Draw_Pic (hud_x + 272, hud_y + 8, "pics/GOTGATLINGPIC.tga");
        } else if (g_version == SPEAROFDESTINY && Player.face_ouch) {
            R_Draw_Pic (hud_x + 272, hud_y + 8, "pics/BJOUCHPIC.tga");
        } else {
            int health = Player.health;

            if (health > 100) {
                health = 100;
            }

            if (health < 0) {
                health = 0;
            }

            R_Draw_Pic (hud_x + 272, hud_y + 8, mugshotnames[ 3 * ((100 - health) / 16) + Player.faceframe ]);
        }
    } else {
        if (Player.LastAttacker && Player.LastAttacker->type == en_needle) {
            R_Draw_Pic (hud_x + 272, hud_y + 8, "pics/MUTANTBJPIC.tga");
        } else {
            R_Draw_Pic (hud_x + 272, hud_y + 8, "pics/FACE8APIC.tga");
        }
    }
}


int32_t r_damageflash = 0;

/**
 * \brief Draws BJ's face on the heads-up-display
 */
void R_DamageFlash (int damage)
{
    r_damageflash += damage;

    if (r_damageflash > 256) {
        r_damageflash = 256;
    }
}

/**
 * \brief Renders damage flash
 */
static void R_DrawFlash (void)
{
    if (r_damageflash) {
        R_DrawBox (0, 0, viddef.width, viddef.height, (0xFF << 24) | 245);

        if ((r_damageflash -= 1) < 0) {
            r_damageflash = 0;
        }
    }
}

/**
 * \brief Renders the background floor / ceiling colours.
 * \param[in] pitch Viewing angle
 * \param[in] floor floor colour.
 * \param[in] ceiling ceiling colour.
 */
static void R_DrawBackGnd (float pitch, colour3_t floor, colour3_t ceiling)
{
    int height = (int) ((viddef.height >> 1) * tan (-pitch) / TanDgr (g_fov * 0.5) + (viddef.height >> 1));
    R_Draw_Fill (0, 0, viddef.width, height, ceiling);
    R_Draw_Fill (0, height, viddef.width, viddef.height - height, floor);
}


uint32_t intensity = 0;


/**
 * \brief Renders the game world.
 */
void R_DrawWorld (void)
{
    placeonplane_t viewport;

// initializing
    viewport = Player.position;

    R_DrawBackGnd (viewport.pitch, r_world->floorColour, r_world->ceilingColour);

    R_SetGL3D (viewport);

    R_RayCast (viewport, r_world);
    R_DrawSprites();

    R_SetGL2D();    // restore 2D back

    if (Player.playstate == ex_dead) {
        R_DrawBox (0, 0, viddef.width, viddef.height, (0xFF << 24) | (uint8_t)intensity);

        if (++intensity >= 240) {
            intensity = 0;

            if (! PL_Reborn (&Player)) {
                M_Menu_Main_f();
            } else {
                PL_Spawn (r_world->pSpawn, r_world);  // Spawn Player
            }

        }
    } else {
        R_DrawWeapon();
        R_DrawFlash();
    }
    R_DrawHUD();
    R_DrawFace();
}


colour3_t barsecondcolour   = { 252, 0, 0 };
colour3_t barthirdcolour    = { 252, 156, 156 };


/**
 * \brief Draws the "Get Psyched" plaque.
 * \param[in] percent Set progress bar to this percentage (0 to 100).
 */
void R_DrawPsyched (uint32_t percent)
{
    int32_t w, h;
    uint32_t bar_length;

    R_Draw_Fill (0, 0, viddef.width, viddef.height, interbkgnd);

    TM_GetTextureSize (&w, &h, "pics/GETPSYCHEDPIC.tga");
    R_Draw_Pic ((viddef.width - w) >> 1, ((viddef.height - h) >> 1) - 80, "pics/GETPSYCHEDPIC.tga");


    R_Draw_Fill ((viddef.width - w) >> 1, ((viddef.height - h) >> 1) + h - 80, w, 4, colourBlack);

    bar_length = (w * percent) / 100;

    if (bar_length) {
        R_Draw_Fill ((viddef.width - w) >> 1, ((viddef.height - h) >> 1) + h - 80, bar_length, 4, barsecondcolour);
        R_Draw_Fill ((viddef.width - w) >> 1, ((viddef.height - h) >> 1) + h - 80, bar_length - 2, 3, barthirdcolour);
    }

    R_DrawHUD();
}

/**
 * \brief Start the rendering registration sequence.
 * \param[in] map The name of the map to load.
 */
void R_BeginRegistration (const char *map)
{
    char    fullname[ MAX_GAMEPATH ];

    if (! map || ! *map) {
        return;
    }
    ++texture_registration_sequence;

    com_snprintf (fullname, sizeof (fullname), "maps/%s.map", map);

//  Door_Reset( &r_world->Doors );
    Powerup_Reset();
    Sprite_Reset();
    Areas_Init (Player.areanumber);
    PushWall_Reset();

    memset (&levelstate, 0, sizeof (levelstate));   // Reset gamestate
    ResetGuards();

    r_world = Level_LoadMap (fullname);

    if (r_world == NULL) {
        return;
    }

    if (g_version == SPEAROFDESTINY) {
        if (strlen (map) >= 2) {
            levelstate.floornum = atoi (map + 1);

            if (levelstate.floornum == 20) {
                levelstate.floornum = 17;
            }
        }

        levelstate.episode = 0;
    } else {
        levelstate.episode  = map[1] - '0';
        levelstate.floornum = map[2] - '0';
    }

}

/**
 * \brief End the rendering registration sequence.
 * \note Releases unused textures
 */
void R_EndRegistration (void)
{
    TM_FreeUnusedTextures();
}
