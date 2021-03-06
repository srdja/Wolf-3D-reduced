/*

    Copyright (C) 2004-2005 Michael Liebscher <johnnycanuck@users.sourceforge.net>
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
 *  wolf_mnewgame.c:   New game menu.
 *
 *  Author: Michael Liebscher   <johnnycanuck@users.sourceforge.net>
 *
 *  Acknowledgement:
 *  This code was derived from Quake II, and was originally
 *  written by Id Software, Inc.
 *
 */

#include <string.h>

#include "wolf_menu.h"
#include "../graphics/opengl_local.h"
#include "wolf_local.h"
#include "wolf_player.h"

#include "../util/com_string.h"

extern void M_Menu_Skill_f (void);
extern void M_Menu_Mission_f (void);

static int tempEpisode;
static int tempSkill;

/////////////////////////////////////////////////////////////////////
//
//  NEW GAME MENU
//
/////////////////////////////////////////////////////////////////////

static int     m_game_cursor;

static menuframework_s s_game_menu;
static menuaction_s    s_episode_actions[ MAX_EPISODES ];



static void SetEpisodeGameFunc (void *data)
{
    menuaction_s *a = (menuaction_s *)data;
    char num[ 16 ];

    com_snprintf (num, sizeof (num), "%d", a->generic.parent->cursor);

    tempEpisode = a->generic.parent->cursor;

    M_Menu_Mission_f();
}


static void MenuCursorDrawFunc (menuframework_s *menu)
{
    M_DrawCursor (((viddef.width - 616) >> 1) + 5, 70 + menu->cursor * 60);
}


static void MenuDrawNewGameImages (void)
{
    char buffer[ 24 ];
    int i;

    for (i = 0; i < 6; ++i) {
        com_snprintf (buffer, sizeof (buffer),  "pics/C_EPISODE%dPIC.tga",  i + 1);

        glColor3f (2.0f, 2.0f, 2.0f);

        R_Draw_Pic (((viddef.width - 616) >> 1) + 69, 70 + i * 60, buffer);

        glColor3f (1.0f, 1.0f, 1.0f);
    }
}

static void Game_MenuInit (void)
{
    static char *wolf3d_episode_names[] = {
        "Episode 1\nEscape from Wolfenstein",
        "Episode 2\nOperation: Eisenfaust",
        "Episode 3\nDie, Fuhrer, Die!",
        "Episode 4\nA Dark Secret",
        "Episode 5\nTrail of the Madman",
        "Episode 6\nConfrontation",
        0
    };

    static char *spear_episode_names[] = {
        "Part 1\nThe Tunnels",
        "Part 2\nThe Dungeons",
        "Part 3\nThe Castle",
        0
    };

    char **episode_names;

    int i;

    s_game_menu.x = (viddef.width - 616) >> 1;
    s_game_menu.nitems = 0;
    s_game_menu.cursordraw = MenuCursorDrawFunc;


    episode_names = wolf3d_episode_names;


    for (i = 0; episode_names[ i ] != 0; ++i) {
        s_episode_actions[ i ].generic.type = MTYPE_ACTION;
        s_episode_actions[ i ].generic.flags  = MENUFONT_LEFT_JUSTIFY;
        s_episode_actions[ i ].generic.x        =  175;
        s_episode_actions[ i ].generic.y        =  70 + i * 60;
        s_episode_actions[ i ].generic.fs       = FONT1;
        s_episode_actions[ i ].generic.fontBaseColour = &textcolour;
        s_episode_actions[ i ].generic.fontHighColour = &highlight;
        s_episode_actions[ i ].generic.name = episode_names[ i ];
        s_episode_actions[ i ].generic.callback = SetEpisodeGameFunc;

        Menu_AddItem (&s_game_menu, &s_episode_actions[ i ]);
    }
}

static void Game_MenuDraw (void)
{
    R_Draw_Fill (0, 0, viddef.width, viddef.height, bgcolour);

    M_BannerString ("Which episode to play?", 15);

    M_DrawWindow (((viddef.width - 616) >> 1), 60, 616, 370,
                  bkgdcolour, bord2colour, deactive);
    M_DrawInfoBar();
    MenuDrawNewGameImages();
    Menu_AdjustCursor (&s_game_menu, 1);
    Menu_Draw (&s_game_menu);
}

static const char *Game_MenuKey (int key)
{
    return Default_MenuKey (&s_game_menu, key);
}

void M_Menu_Game_f (void)
{
    fprintf(stdout, "Entered game menu function\n");
    Game_MenuInit();
    M_PushMenu (Game_MenuDraw, Game_MenuKey);
    m_game_cursor = 1;
}

/////////////////////////////////////////////////////////////////////
//
//  SKILL MENU
//
/////////////////////////////////////////////////////////////////////

static menuframework_s s_skill_menu;
static menuaction_s    s_skill_actions[ MAX_SKILLS ];

extern void Client_PrepRefresh (const char *r_mapname);

void StartGame (int episode, int mission, int g_skill)
{
    char szTextMsg[ 128 ];

    currentMap.skill = g_skill;
    currentMap.levelCompleted = 0;

    skill = g_skill;

    memset (&LevelRatios, 0, sizeof (LevelRatios));

    PL_NewGame (&Player);

    //Sound_StopBGTrack();

    // disable updates and start the cinematic going
    M_ForceMenuOff();

    com_snprintf (szTextMsg, sizeof (szTextMsg),
            //  "loading ; map w%d%d.map\n", episode, mission);
                  "w%d%d.map", episode, mission);

    fprintf(stdout, "Enter game command sent [%s] \n", szTextMsg);
    Client_PrepRefresh (szTextMsg);
}

static void ToughPic (int i)
{
    char string[ 32 ];

    com_snprintf (string, sizeof (string), "pics/C_SKILL%dPIC.tga", i + 1);

    R_Draw_Pic (((viddef.width - 450) >> 1) + 375, 214, string);
}


static void SkillMenuCursorDrawFunc (struct _tag_menuframework *data)
{
    menuframework_s *menu = (menuframework_s *)data;
    M_DrawCursor (((viddef.width - 450) >> 1) + 5, 187 + menu->cursor * 30);
    ToughPic (menu->cursor);
}


static void SkillGameFunc (void *data)
{
    menuaction_s *a = (menuaction_s *)data;
    tempSkill = a->generic.parent->cursor;

    M_Menu_Game_f();
}


static void Skill_MenuInit (void)
{
    static const char *skill_names[] = {
        "Can I play, Daddy?",
        "Don't hurt me.",
        "Bring 'em on!",
        "I am Death incarnate!",
        0
    };

    int i;



    s_skill_menu.x = ((viddef.width - 450) >> 1);
    s_skill_menu.y = 0;
    s_skill_menu.nitems = 0;
    s_skill_menu.cursordraw = SkillMenuCursorDrawFunc;


    for (i = 0; skill_names[ i ] != 0; ++i) {
        s_skill_actions[ i ].generic.type   = MTYPE_ACTION;
        s_skill_actions[ i ].generic.flags  = MENUFONT_LEFT_JUSTIFY;
        s_skill_actions[ i ].generic.x      = 65;
        s_skill_actions[ i ].generic.y      = 190 + i * 30;
        s_skill_actions[ i ].generic.fs     = FONT1;
        s_skill_actions[ i ].generic.fontBaseColour = &textcolour;
        s_skill_actions[ i ].generic.fontHighColour = &highlight;
        s_skill_actions[ i ].generic.name   = skill_names[ i ];
        s_skill_actions[ i ].generic.callback = SkillGameFunc;

        Menu_AddItem (&s_skill_menu, &s_skill_actions[ i ]);
    }
}

static void Skill_MenuDraw (void)
{
    R_Draw_Fill (0, 0, viddef.width, viddef.height, bgcolour);
    M_BannerString ("How tough are you?", 136);

    M_DrawWindow (((viddef.width - 450) >> 1), 180, 450, 134,
                  bkgdcolour, bord2colour, deactive);

    M_DrawInfoBar();

    Menu_AdjustCursor (&s_skill_menu, 1);
    Menu_Draw (&s_skill_menu);
}


static const char *Skill_MenuKey (int key)
{
    return Default_MenuKey (&s_skill_menu, key);
}

void M_Menu_Skill_f (void)
{
    Skill_MenuInit();
    M_PushMenu (Skill_MenuDraw, Skill_MenuKey);
}


/////////////////////////////////////////////////////////////////////
//
//  MISSION MENU
//
/////////////////////////////////////////////////////////////////////

static menuframework_s s_mission_menu;
static menuaction_s    s_mission_actions[ 10 ];



static void SetMissionGameFunc (void *data)
{
    menuaction_s *a = (menuaction_s *)data;
    int floor = a->generic.parent->cursor;

    StartGame (tempEpisode, floor, tempSkill);
}


static void MissionMenuCursorDrawFunc (menuframework_s *menu)
{
    M_DrawCursor (((viddef.width - 616) >> 1) + 5, 70 + menu->cursor * 36);
}

static void Mission_MenuInit (void)
{
    static char *wolf3d_mission_names[] = {
        "Level 1",
        "Level 2",
        "Level 3",
        "Level 4",
        "Level 5",
        "Level 6",
        "Level 7",
        "Level 8",
        "Boss Level",
        "Secret Level",
        0
    };

    static char *spear_tunnels_mission_names[] = {
        "Tunnels 1",
        "Tunnels 2",
        "Tunnels 3",
        "Tunnels 4",
        "Tunnels Boss",
        "Secret Level",
        0
    };

    static char *spear_dungeons_mission_names[] = {
        "Dungeons 1",
        "Dungeons 2",
        "Dungeons 3",
        "Dungeons 4",
        "Dungeons Boss",
        0
    };

    static char *spear_castle_mission_names[] = {
        "Castle 1",
        "Castle 2",
        "Castle 3",
        "Castle 4",
        "Castle 5",
        "Castle Boss",
        "Ramparts",
        "Death Knight",
        "Angel of Death",
        "Secret Level",
        0
    };

    char **mission_names;

    int i;

    s_mission_menu.x = (viddef.width - 616) >> 1;
    s_mission_menu.nitems = 0;
    s_mission_menu.cursordraw = MissionMenuCursorDrawFunc;

    mission_names = wolf3d_mission_names;

    for (i = 0; mission_names[ i ] != 0; ++i) {
        s_mission_actions[ i ].generic.type = MTYPE_ACTION;
        s_mission_actions[ i ].generic.flags  = MENUFONT_LEFT_JUSTIFY;
        s_mission_actions[ i ].generic.x        =  175;
        s_mission_actions[ i ].generic.y        =  70 + i * 36;
        s_mission_actions[ i ].generic.fs       = FONT1;
        s_mission_actions[ i ].generic.fontBaseColour = &textcolour;
        s_mission_actions[ i ].generic.fontHighColour = &highlight;
        s_mission_actions[ i ].generic.name = mission_names[ i ];
        s_mission_actions[ i ].generic.callback = SetMissionGameFunc;

        Menu_AddItem (&s_mission_menu, &s_mission_actions[ i ]);
    }

    //Menu_Center( &s_mission_menu );
}

static void Mission_MenuDraw (void)
{
    R_Draw_Fill (0, 0, viddef.width, viddef.height, bgcolour);

    M_BannerString ("Which mission to play?", 15);

    M_DrawWindow (((viddef.width - 616) >> 1), 60, 616, 370,
                  bkgdcolour, bord2colour, deactive);
    M_DrawInfoBar();
    Menu_AdjustCursor (&s_mission_menu, 1);
    Menu_Draw (&s_mission_menu);
}

static const char *Mission_MenuKey (int key)
{
    return Default_MenuKey (&s_mission_menu, key);
}

void M_Menu_Mission_f (void)
{
    Mission_MenuInit();
    M_PushMenu (Mission_MenuDraw, Mission_MenuKey);
    m_game_cursor = 1;
}
