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
 *	wolf_mnewgame.c:   New game menu.
 *
 *	Author:	Michael Liebscher	<johnnycanuck@users.sourceforge.net>
 *
 *	Acknowledgement:
 *	This code was derived from Quake II, and was originally
 *	written by Id Software, Inc.
 *
 */

#include <string.h>

#include "wolf_menu.h"

#include "wolf_local.h"
#include "wolf_player.h"

#include "../env/client.h"
#include "../env/com_string.h"
#include "../env/renderer.h"
#include "../env/sound.h"

extern void M_Menu_Skill_f( void );
extern void M_Menu_Mission_f (void);

PRIVATE int tempEpisode;
PRIVATE int tempSkill;

/////////////////////////////////////////////////////////////////////
//
//	NEW GAME MENU
//
/////////////////////////////////////////////////////////////////////

PRIVATE int		m_game_cursor;

PRIVATE menuframework_s	s_game_menu;
PRIVATE menuaction_s	s_episode_actions[ MAX_EPISODES ];



PRIVATE void SetEpisodeGameFunc( void *data )
{
	menuaction_s *a = (menuaction_s *)data;
	char num[ 16 ];

	com_snprintf( num, sizeof( num ), "%d", a->generic.parent->cursor );

	tempEpisode = a->generic.parent->cursor;

	M_Menu_Mission_f();
}


PRIVATE void MenuCursorDrawFunc( menuframework_s *menu )
{
	M_DrawCursor( ((viddef.width - 616) >> 1) + 5, 70 + menu->cursor * 60 );
}

#include "w3d_opengl.h"

PRIVATE void MenuDrawNewGameImages( void )
{
	char buffer[ 24 ];
	int i;

	for( i = 0; i < 6; ++i )
	{
		com_snprintf( buffer, sizeof( buffer ),  "pics/C_EPISODE%dPIC.tga",  i+1 );

		pfglColor3f(2.0f, 2.0f, 2.0f);

		R_Draw_Pic( ((viddef.width - 616) >> 1) + 69, 70 + i * 60, buffer );

		pfglColor3f(1.0f, 1.0f, 1.0f);
	}
}

PRIVATE void Game_MenuInit( void )
{
	static char *wolf3d_episode_names[] =
	{
		"Episode 1\nEscape from Wolfenstein",
		"Episode 2\nOperation: Eisenfaust",
		"Episode 3\nDie, Fuhrer, Die!",
		"Episode 4\nA Dark Secret",
		"Episode 5\nTrail of the Madman",
		"Episode 6\nConfrontation",
		0
	};

	static char *spear_episode_names[] =
	{
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

	if( g_version->value == SPEAROFDESTINY )
	{
		episode_names = spear_episode_names;
	}
	else
	{
		episode_names = wolf3d_episode_names;
	}

	for( i = 0; episode_names[ i ] != 0; ++i )
	{
		s_episode_actions[ i ].generic.type	= MTYPE_ACTION;
		s_episode_actions[ i ].generic.flags  = MENUFONT_LEFT_JUSTIFY;
		s_episode_actions[ i ].generic.x		=  175;
		s_episode_actions[ i ].generic.y		=  70 + i * 60;
		s_episode_actions[ i ].generic.fs		= FONT1;
		s_episode_actions[ i ].generic.fontBaseColour = &textcolour;
		s_episode_actions[ i ].generic.fontHighColour = &highlight;
		s_episode_actions[ i ].generic.name	= episode_names[ i ];
		s_episode_actions[ i ].generic.callback = SetEpisodeGameFunc;

		Menu_AddItem( &s_game_menu, &s_episode_actions[ i ] );
	}

	//Menu_Center( &s_game_menu );
}

PRIVATE void Game_MenuDraw( void )
{
	R_Draw_Fill( 0, 0, viddef.width, viddef.height, bgcolour );

	M_BannerString( "Which episode to play?", 15 );

	M_DrawWindow(  ( (viddef.width - 616)>>1 ), 60, 616, 370,
		bkgdcolour, bord2colour, deactive );
	M_DrawInfoBar();
	MenuDrawNewGameImages();
	Menu_AdjustCursor( &s_game_menu, 1 );
	Menu_Draw( &s_game_menu );
}

PRIVATE const char *Game_MenuKey( int key )
{
	return Default_MenuKey( &s_game_menu, key );
}

PUBLIC void M_Menu_Game_f (void)
{
	Game_MenuInit();
	M_PushMenu( Game_MenuDraw, Game_MenuKey );
	m_game_cursor = 1;
}

/////////////////////////////////////////////////////////////////////
//
//	SKILL MENU
//
/////////////////////////////////////////////////////////////////////

PRIVATE menuframework_s	s_skill_menu;
PRIVATE menuaction_s	s_skill_actions[ MAX_SKILLS ];


PUBLIC void StartGame( int episode, int mission, int skill )
{
	char szTextMsg[ 128 ];
	char num[ 16 ];

	currentMap.skill = skill;
	currentMap.levelCompleted = 0;

	com_snprintf( num, sizeof( num ), "%d", skill );

	Cvar_Set( "skill", num );

	memset( &LevelRatios, 0, sizeof( LevelRatios ) );

	PL_NewGame( &Player );

	Sound_StopBGTrack();

	// disable updates and start the cinematic going
//	cl.servercount = -1;
	M_ForceMenuOff();
//	Cvar_SetValue( "deathmatch", 0 );
//	Cvar_SetValue( "coop", 0 );

//	Cvar_SetValue( "gamerules", 0 );		//PGM

	if( g_version->value == SPEAROFDESTINY )
	{
		com_snprintf( szTextMsg, sizeof( szTextMsg ),
			"loading ; map s%.2d.map\n", mission );
	}
	else
	{
		com_snprintf( szTextMsg, sizeof( szTextMsg ),
			"loading ; map w%d%d.map\n", episode, mission );
	}

	Cbuf_AddText( szTextMsg );
}

PRIVATE void ToughPic( int i )
{
	char string[ 32 ];

	if( g_version->value == SPEAROFDESTINY )
	{
		com_snprintf( string, sizeof( string ), "pics/SC_SKILL%dPIC.tga", i+1 );
	}
	else
	{
		com_snprintf( string, sizeof( string ), "pics/C_SKILL%dPIC.tga", i+1 );
	}


	R_Draw_Pic( ((viddef.width - 450) >> 1) + 375, 214, string );
}


PRIVATE void SkillMenuCursorDrawFunc( struct _tag_menuframework *data )
{
	menuframework_s *menu = (menuframework_s *)data;
	M_DrawCursor( ((viddef.width - 450) >> 1) + 5, 187 + menu->cursor * 30 );
	ToughPic( menu->cursor );
}


PRIVATE void SkillGameFunc( void *data )
{
	menuaction_s *a = (menuaction_s *)data;
	tempSkill = a->generic.parent->cursor;

	M_Menu_Game_f();
}


PRIVATE void Skill_MenuInit( void )
{
	static const char *skill_names[] =
	{
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


	for( i = 0; skill_names[ i ] != 0; ++i )
	{
		s_skill_actions[ i ].generic.type	= MTYPE_ACTION;
		s_skill_actions[ i ].generic.flags  = MENUFONT_LEFT_JUSTIFY;
		s_skill_actions[ i ].generic.x		= 65;
		s_skill_actions[ i ].generic.y		= 190 + i * 30;
		s_skill_actions[ i ].generic.fs		= FONT1;
		s_skill_actions[ i ].generic.fontBaseColour = &textcolour;
		s_skill_actions[ i ].generic.fontHighColour = &highlight;
		s_skill_actions[ i ].generic.name	= skill_names[ i ];
		s_skill_actions[ i ].generic.callback = SkillGameFunc;

		Menu_AddItem( &s_skill_menu, &s_skill_actions[ i ] );
	}
}

PRIVATE void Skill_MenuDraw( void )
{
	if( g_version->value == SPEAROFDESTINY )
	{
		SW32 w, h;

		R_Draw_Tile( 0, 0, viddef.width, viddef.height, "pics/C_BACKDROPPIC.tga" );

		TM_GetTextureSize( &w, &h, "pics/C_HOWTOUGHPIC.tga" );
		R_Draw_Pic( (viddef.width - w) >> 1, 136, "pics/C_HOWTOUGHPIC.tga" );

		M_DrawWindow(  ((viddef.width - 450) >> 1), 180, 450, 134,
			sodbkgdcolour, sodbord2colour, soddeactive );
	}
	else
	{
		R_Draw_Fill( 0, 0, viddef.width, viddef.height, bgcolour );

		M_BannerString( "How tough are you?", 136 );

		M_DrawWindow(  ((viddef.width - 450) >> 1), 180, 450, 134,
			bkgdcolour, bord2colour, deactive );
	}

	M_DrawInfoBar();

	Menu_AdjustCursor( &s_skill_menu, 1 );
	Menu_Draw( &s_skill_menu );
}




PRIVATE const char *Skill_MenuKey( int key )
{
	return Default_MenuKey( &s_skill_menu, key );
}

PUBLIC void M_Menu_Skill_f( void )
{
	Skill_MenuInit();
	M_PushMenu( Skill_MenuDraw, Skill_MenuKey );
}


/////////////////////////////////////////////////////////////////////
//
//	MISSION MENU
//
/////////////////////////////////////////////////////////////////////

PRIVATE menuframework_s	s_mission_menu;
PRIVATE menuaction_s	s_mission_actions[ 10 ];



PRIVATE void SetMissionGameFunc( void *data )
{
	menuaction_s *a = (menuaction_s *)data;
	int floor = a->generic.parent->cursor;

	if( g_version->value == SPEAROFDESTINY )
	{
		switch (tempEpisode)
		{
			case 0:
				if (floor == 5)
				{
					floor = 18;
				}
				break;
			case 1:
				floor += 5;
				break;
			case 2:
				if (floor == 8)
				{
					floor = 20;
				}
				else if (floor == 9)
				{
					floor = 19;
				}
				else
				{
					floor += 10;
				}
				break;
		}
		tempEpisode = 0;
	}

	StartGame(tempEpisode, floor, tempSkill);
}


PRIVATE void MissionMenuCursorDrawFunc( menuframework_s *menu )
{
	M_DrawCursor( ((viddef.width - 616) >> 1) + 5, 70 + menu->cursor * 36 );
}

PRIVATE void Mission_MenuInit( void )
{
	static char *wolf3d_mission_names[] =
	{
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

	static char *spear_tunnels_mission_names[] =
	{
		"Tunnels 1",
		"Tunnels 2",
		"Tunnels 3",
		"Tunnels 4",
		"Tunnels Boss",
		"Secret Level",
		0
	};

	static char *spear_dungeons_mission_names[] =
	{
		"Dungeons 1",
		"Dungeons 2",
		"Dungeons 3",
		"Dungeons 4",
		"Dungeons Boss",
		0
	};

	static char *spear_castle_mission_names[] =
	{
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

	if( g_version->value == SPEAROFDESTINY )
	{
		switch (tempEpisode)
		{
			case 0: mission_names = spear_tunnels_mission_names; break;
			case 1: mission_names = spear_dungeons_mission_names; break;
			case 2: mission_names = spear_castle_mission_names; break;
		}
	} else {
		mission_names = wolf3d_mission_names;
	}

	for( i = 0; mission_names[ i ] != 0; ++i )
	{
		s_mission_actions[ i ].generic.type	= MTYPE_ACTION;
		s_mission_actions[ i ].generic.flags  = MENUFONT_LEFT_JUSTIFY;
		s_mission_actions[ i ].generic.x		=  175;
		s_mission_actions[ i ].generic.y		=  70 + i * 36;
		s_mission_actions[ i ].generic.fs		= FONT1;
		s_mission_actions[ i ].generic.fontBaseColour = &textcolour;
		s_mission_actions[ i ].generic.fontHighColour = &highlight;
		s_mission_actions[ i ].generic.name	= mission_names[ i ];
		s_mission_actions[ i ].generic.callback = SetMissionGameFunc;

		Menu_AddItem( &s_mission_menu, &s_mission_actions[ i ] );
	}

	//Menu_Center( &s_mission_menu );
}

PRIVATE void Mission_MenuDraw( void )
{
	R_Draw_Fill( 0, 0, viddef.width, viddef.height, bgcolour );

	M_BannerString( "Which mission to play?", 15 );

	M_DrawWindow(  ( (viddef.width - 616)>>1 ), 60, 616, 370,
		bkgdcolour, bord2colour, deactive );
	M_DrawInfoBar();
	Menu_AdjustCursor( &s_mission_menu, 1 );
	Menu_Draw( &s_mission_menu );
}

PRIVATE const char *Mission_MenuKey( int key )
{
	return Default_MenuKey( &s_mission_menu, key );
}

PUBLIC void M_Menu_Mission_f (void)
{
	Mission_MenuInit();
	M_PushMenu( Mission_MenuDraw, Mission_MenuKey );
	m_game_cursor = 1;
}
