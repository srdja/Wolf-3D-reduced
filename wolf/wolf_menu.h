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

/**
 * \file wolf_menu.h
 * \brief Menu management.
 * \author Michael Liebscher
 * \date 2004-2005
 * \note Portion of this code was derived from Quake II, and was originally written by Id Software, Inc.
 */

#ifndef __WOLF_MENU_H__
#define __WOLF_MENU_H__

#include "../env/platform.h"
#include "../env/common_utils.h"
#include "../env/video.h"

#include "../env/menu_conf.h"


extern colour3_t bgcolour;
extern colour3_t bord2colour;
extern colour3_t deactive;
extern colour3_t bkgdcolour;

extern colour3_t readcolour;
extern colour3_t readhcolour;
extern colour3_t textcolour;
extern colour3_t highlight;


extern colour3_t bannerline;

extern colour3_t interbkgnd;


extern colour3_t sodbgcolour;

extern colour3_t sodbord2colour;
extern colour3_t soddeactive;
extern colour3_t sodbkgdcolour;


extern const char *menu_in_sound;
extern const char *menu_move_sound;
extern const char *menu_out_sound;

void M_PopMenu( void );
void M_ForceMenuOff( void );
void M_PushMenu( void (*draw) (void), const char *(*key) (int k) );

void M_Intermission_f( void );



#define NUM_CURSOR_FRAMES 2
void M_DrawCursor( int x, int y );
void M_BannerString( const char *string, W16 nYOffset );
void M_Banner( const char *name, W16 nYOffest );
void M_DrawWindow( int x, int y, int w, int h, colour3_t bg, colour3_t act, colour3_t deact );
void M_DrawInfoBar( void );

const char *Default_MenuKey( menuframework_s *m, int key );


#endif /* __WOLF_MENU_H__ */
