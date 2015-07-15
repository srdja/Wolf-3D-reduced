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

#include "wolf_menu.h"

#include "wolf_local.h"

#include "../env/color.h"
#include "../env/client.h"
#include "../env/renderer.h"
#include "../env/com_string.h"


/////////////////////////////////////////////////////////////////////
//
//  Intro Slide Show
//
/////////////////////////////////////////////////////////////////////


PRIVATE W8 intro_slide = 0;

PRIVATE W32 intro_basetime;

colour3_t pc13intro = { 32, 168, 248 };

typedef struct {
    void (*draw) (void);
    W32 intro_time;

} IntroLayer_t;

PRIVATE IntroLayer_t sIntroLayer[ 6 ];


PRIVATE void Intro_PC13_Draw (void)
{
    W32 w, h;

    R_Draw_Fill (0, 0, viddef.width, viddef.height, pc13intro);

    TM_GetTextureSize (&w, &h, "pics/PC13PIC.tga");
    R_Draw_Pic ((viddef.width - w) - 16, (viddef.height - h) - 26, "pics/PC13PIC.tga");
}



PRIVATE void Intro_TitlePage_Draw (void)
{
    W32 w, h;

    R_Draw_Fill (0, 0, viddef.width, viddef.height, colourBlack);

    if (g_version == SPEAROFDESTINY) {
        TM_GetTextureSize (&w, &h, "pics/STITLEPIC.tga");
        R_Draw_Pic ((viddef.width - w) >> 1, (viddef.height - h) >> 1, "pics/STITLEPIC.tga");

        return;
    }

    TM_GetTextureSize (&w, &h, "pics/TITLEPIC.tga");
    R_Draw_Pic ((viddef.width - w) >> 1, (viddef.height - h) >> 1, "pics/TITLEPIC.tga");

}

PRIVATE void Intro_CreditsPage_Draw (void)
{
    W32 w, h;

    if (g_version == SPEAROFDESTINY) {

        R_Draw_Tile (0, 0, viddef.width, viddef.height, "pics/C_BACKDROPPIC.tga");

        R_Draw_Fill (0, 36, viddef.width, 48, colourBlack);
        R_Draw_Fill (0, 80, viddef.width, 2, sodbgcolour);

        TM_GetTextureSize (&w, &h, "pics/SCREDITSPIC.tga");
        R_Draw_Pic ((viddef.width - w) >> 1, 0, "pics/SCREDITSPIC.tga");

        return;
    }


    R_Draw_Fill (0, 0, viddef.width, viddef.height, bgcolour);

    R_Draw_Fill (0, 36, viddef.width, 48, colourBlack);
    R_Draw_Fill (0, 80, viddef.width, 2, bannerline);


    TM_GetTextureSize (&w, &h, "pics/CREDITSPIC.tga");
    R_Draw_Pic ((viddef.width - w) >> 1, 0, "pics/CREDITSPIC.tga");

}



#define MAX_HIGHNAME    57
#define MAX_SCORES      7

typedef struct {
    char    name[ MAX_HIGHNAME + 1 ];
    W32     score;
    W16     completed, episode;

} HighScore_t;

PRIVATE HighScore_t Scores[ MAX_SCORES ] = {
    { "id software-'92", 10000, 1 },
    { "Adrian Carmack", 10000, 1 },
    { "John Carmack",   10000, 1 },
    { "Kevin Cloud",    10000, 1 },
    { "Tom Hall",       10000, 1 },
    { "John Romero",    10000, 1 },
    { "Jay Wilbur",     10000, 1 }
};

PRIVATE void Intro_HighScore_Draw (void)
{
    W32 w, h;
    W16 i;
    W32 y;
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

    Font_SetColour (FONT0, colourWhite);
    h = Font_GetSize (FONT0);

    for (i = 0, hs_temp = Scores ; i < MAX_SCORES ; ++i, ++hs_temp) {
        // Name
        Font_put_line (FONT0, (8 * 8) + ((viddef.width - 640) >> 1), y, hs_temp->name);

        // Episode / Level
        com_snprintf (string, sizeof (string), "E%d/L%d", hs_temp->episode + 1, hs_temp->completed);
        Font_put_line (FONT0, (40 * 8) + ((viddef.width - 640) >> 1), y, string);

        // Score
        com_snprintf (string, sizeof (string), "%d", hs_temp->score);
        Font_put_line (FONT0, (56 * 8) + ((viddef.width - 640) >> 1), y, string);

        y += h * 2;
    }

}


PRIVATE void Intro_SHighScore_Draw (void)
{
    W32 h;
    W16 i;
    W32 y;
    HighScore_t *hs_temp;
    char string[ 32 ];




    R_Draw_Tile (0, 0, viddef.width, viddef.height, "pics/C_BACKDROPPIC.tga");

    M_Banner ("pics/SHIGHSCORESPIC.tga", 0);


    y = 160 + ((viddef.height - 480) >> 1);

    Font_SetColour (FONT0, colourWhite);
    h = Font_GetSize (FONT0);

    for (i = 0, hs_temp = Scores ; i < MAX_SCORES ; ++i, ++hs_temp) {
        // Name
        Font_put_line (FONT0, (8 * 4) + ((viddef.width - 640) >> 1), y, hs_temp->name);

        // Episode / Level
        com_snprintf (string, sizeof (string), "%d", hs_temp->episode + 1, hs_temp->completed);
        Font_put_line (FONT0, (40 * 8) + ((viddef.width - 640) >> 1), y, string);

        // Score
        com_snprintf (string, sizeof (string), "%d", hs_temp->score);
        Font_put_line (FONT0, (60 * 8) + ((viddef.width - 640) >> 1), y, string);

        y += h * 2;
    }
}

PRIVATE void M_Intro_Draw (void)
{
    if (intro_slide > 3) {
        //Sound_StopBGTrack();
        intro_slide = 1;
        M_Menu_Main_f();
        return;
    }

    sIntroLayer[ intro_slide ].draw();


    if (ClientStatic.realtime >= (int) (intro_basetime + sIntroLayer[ intro_slide ].intro_time)) {
        intro_basetime = ClientStatic.realtime;

        ++intro_slide;
    }
}
/*
PRIVATE const char *M_Intro_Key (int key)
{
    intro_basetime = ClientStatic.realtime;
    ++intro_slide;

    return NULL;
}
*/
/*
PUBLIC void M_Intro_f (void)
{
    char *s;


    if (Cmd_Argc() < 2) {
        return;
    }

    s = Cmd_Argv (1);

    //Sound_StopBGTrack();

    if (g_version->value == SPEAROFDESTINY) {
       // Sound_StartBGTrack ("music/XTOWER2.ogg", "music/XTOWER2.ogg");
    } else {
       // Sound_StartBGTrack ("music/INTROCW3.ogg", "music/INTROCW3.ogg");
    }

    sIntroLayer[ 0 ].draw = Intro_PC13_Draw;
    sIntroLayer[ 0 ].intro_time = 7000;

    sIntroLayer[ 1 ].draw = Intro_TitlePage_Draw;
    sIntroLayer[ 1 ].intro_time = 15000;

    sIntroLayer[ 2 ].draw = Intro_CreditsPage_Draw;
    sIntroLayer[ 2 ].intro_time = 10000;

    sIntroLayer[ 3 ].draw = Intro_HighScore_Draw;

    if (g_version->value == SPEAROFDESTINY) {
        sIntroLayer[ 3 ].draw = Intro_SHighScore_Draw;
    }

    sIntroLayer[ 3 ].intro_time = 10000;

    if (!com_stricmp (s, "PC13")) {
        intro_slide = 0;
    } else if (!com_stricmp (s, "TITLEPAGE")) {
        intro_slide = 1;
    } else if (!com_stricmp (s, "CREDITSPAGE")) {
        intro_slide = 2;
    } else if (!com_stricmp (s, "HIGHSCORE")) {
        intro_slide = 3;
    } else {
        intro_slide = 0;
        return;
    }


    ClientStatic.disable_screen = false;

    intro_basetime = ClientStatic.realtime;

    M_PushMenu (M_Intro_Draw, M_Intro_Key);
}
*/