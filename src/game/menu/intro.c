/*

    Copyright (C) 2004-2005 Michael Liebscher
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
 *  wolf_mintro.c:   Intro menu.
 *
 *  Author: Michael Liebscher   <johnnycanuck@users.sourceforge.net>
 *  Date:   2004
 *
 *  Acknowledgement:
 *  This code was derived from Quake II, and was originally
 *  written by Id Software, Inc.
 *
 */

#include "../wolf_menu.h"

#include "../wolf_local.h"
#include "intro.h"
#include "../graphics/color.h"
#include "../client.h"
#include "../graphics/renderer.h"
#include "../util/com_string.h"
#include "../../input/input.h"

static uint8_t  intro_slide = 0;
static uint32_t intro_basetime;

colour3_t pc13intro = { 32, 168, 248 };

typedef struct {
    void (*draw) (void);
    uint32_t intro_time;
} Slide;

#define MAX_HIGHNAME    57
#define MAX_SCORES      7

typedef struct {
    char    name[ MAX_HIGHNAME + 1 ];
    uint32_t score;
    uint16_t completed, episode;

} HighScore_t;

static HighScore_t Scores[ MAX_SCORES ] = {
        { "id software-'92", 10000, 1 },
        { "Adrian Carmack", 10000, 1 },
        { "John Carmack",   10000, 1 },
        { "Kevin Cloud",    10000, 1 },
        { "Tom Hall",       10000, 1 },
        { "John Romero",    10000, 1 },
        { "Jay Wilbur",     10000, 1 }
};

static Slide slides[6];

static void draw_pc13(void)
{
    uint32_t w, h;

    R_Draw_Fill (0, 0, viddef.width, viddef.height, pc13intro);
    TM_GetTextureSize (&w, &h, "pics/PC13PIC.tga");
    R_Draw_Pic ((viddef.width - w) - 16, (viddef.height - h) - 26, "pics/PC13PIC.tga");
}


static void draw_title(void)
{
    uint32_t w, h;

    R_Draw_Fill (0, 0, viddef.width, viddef.height, colourBlack);

    TM_GetTextureSize (&w, &h, "pics/TITLEPIC.tga");
    R_Draw_Pic ((viddef.width - w) >> 1, (viddef.height - h) >> 1, "pics/TITLEPIC.tga");
}

static void draw_credits(void)
{
    uint32_t w, h;

    R_Draw_Fill (0, 0, viddef.width, viddef.height, bgcolour);
    R_Draw_Fill (0, 36, viddef.width, 48, colourBlack);
    R_Draw_Fill (0, 80, viddef.width, 2, bannerline);

    TM_GetTextureSize (&w, &h, "pics/CREDITSPIC.tga");
    R_Draw_Pic ((viddef.width - w) >> 1, 0, "pics/CREDITSPIC.tga");
}

static void draw_score(void)
{
    uint32_t w, h;
    uint16_t i;
    uint32_t y;

    HighScore_t *hs_temp;
    char string[ 32 ];

    R_Draw_Fill (0, 0, viddef.width, viddef.height, bgcolour);

    M_Banner ("pics/HIGHSCORESPIC.tga", 0);

    TM_GetTextureSize (&w, &h, "pics/C_NAMEPIC.tga");
    R_Draw_Pic ((8 * 8) + ((viddef.width - 640) >> 1), 136 + ((viddef.height - 480) >> 1), "pics/C_NAMEPIC.tga");

    TM_GetTextureSize (&w, &h, "pics/C_LEVELPIC.tga");
    R_Draw_Pic ((40 * 8) + ((viddef.width - 640) >> 1), 136 + ((viddef.height - 480) >> 1), "pics/C_LEVELPIC.tga");

    TM_GetTextureSize (&w, &h, "pics/C_SCOREPIC.tga");
    R_Draw_Pic ((56 * 8) + ((viddef.width - 640) >> 1), 136 + ((viddef.height - 480) >> 1), "pics/C_SCOREPIC.tga");

    y = 160 + ((viddef.height - 480) >> 1);

    //Font_SetColour (FONT0, colourWhite);
   // h = Font_GetSize (FONT0);

    for (i = 0, hs_temp = Scores ; i < MAX_SCORES ; ++i, ++hs_temp) {
        // Name
       // Font_put_line (FONT0, (8 * 8) + ((viddef.width - 640) >> 1), y, hs_temp->name);

        // Episode / Level
        com_snprintf (string, sizeof (string), "E%d/L%d", hs_temp->episode + 1, hs_temp->completed);
      //  Font_put_line (FONT0, (40 * 8) + ((viddef.width - 640) >> 1), y, string);

        // Score
        com_snprintf (string, sizeof (string), "%d", hs_temp->score);
      //  Font_put_line (FONT0, (56 * 8) + ((viddef.width - 640) >> 1), y, string);

      //  y += h * 2;
    }

}

extern void StartGame(int a, int b, int g_skill);

void intro_next_slide()
{
    intro_slide++;
}

void intro_init()
{
    // Sound_StartBGTrack ("music/INTROCW3.ogg", "music/INTROCW3.ogg");

    intro_slide = 0;

    ClientStatic.menuState = IPM_INTRO;

    slides[0].draw = draw_pc13;
    slides[0].intro_time = 7000;

    slides[1].draw = draw_title;
    slides[1].intro_time = 15000;

    slides[2].draw = draw_credits;
    slides[2].intro_time = 10000;

    slides[3].draw = draw_score;
    slides[3].intro_time = 10000;

    intro_basetime = ClientStatic.realtime;

    input_set_context("intro");
}

void intro_run()
{
    if (intro_slide > 3) {
        //Sound_StopBGTrack();
        intro_slide = 1;
        input_set_context("game");
        ClientStatic.menuState = IPM_GAME;
        Game_Init();
        StartGame( 1,  0,  1);
        return;
    }
    slides[intro_slide].draw();

    if (ClientStatic.realtime >= (int) (intro_basetime + slides[intro_slide].intro_time)) {
        intro_basetime = ClientStatic.realtime;
        ++intro_slide;
    }
}