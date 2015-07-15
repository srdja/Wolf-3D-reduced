/*

    Copyright (C) 2004 Michael Liebscher
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
 *  wolf_mquit.c:   Quit menu.
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
#include "wolf_math.h"
#include "wolf_local.h"

#include "../env/keycodes.h"
#include "../env/color.h"

extern void M_Main_Draw (void);
extern void M_SMain_Draw (void);

/////////////////////////////////////////////////////////////////////
//
//  QUIT MENU
//
/////////////////////////////////////////////////////////////////////

PRIVATE const char *M_Quit_Key (int key)
{
    switch (key) {
    case K_ESCAPE:
    case 'n':
    case 'N':
        M_PopMenu();
        break;

    case 'Y':
    case 'y':
        exit(0);
    default:
        break;
    }
    return NULL;
}


PRIVATE const char log_out_messages[ 18 ][ 80 ] = {

    { "Dost thou wish to\nleave with such hasty\nabandon?" },
    { "Chickening out...\nalready?" },
    { "Press N for more carnage.\nPress Y to be a weenie." },
    { "So, you think you can\nquit this easily, huh?" },
    { "Press N to save the world.\nPress Y to abandon it in\nits hour of need." },
    { "Press N if you are brave.\nPress Y to cower in shame." },
    { "Heroes, press N.\nWimps, press Y." },
    { "You are at an intersection.\nA sign says, 'Press Y to quit.'\n>" },
    { "For guns and glory, press N.\nFor work and worry, press Y." },

// SOD
    { "Heroes don't quit, but\ngo ahead and press Y\nif you aren't one." },
    { "Press Y to quit,\nor press N to enjoy\nmore violent diversion." },
    { "Depressing the Y key means\nyou must return to the\nhumdrum workday world." },
    { "Hey, quit or play,\nY or N:\nit's your choice." },
    { "Sure you don't want to\nwaste a few more\nproductive hours?" },
    { "I think you had better\nplay some more. Please\npress N...please?" },
    { "If you are tough, press N.\nIf not, press Y daintily." },
    { "I'm thinkin' that\nyou might wanna press N\nto play more. You do it." },
    { "Sure. Fine. Quit.\nSee if we care.\nGet it over with.\nPress Y." }

};

PRIVATE int menu_random;

PRIVATE void M_Quit_Draw (void)
{
    int w, h;

    Font_GetMsgDimensions (1, log_out_messages[ menu_random ], &w, &h);

    if (g_version == SPEAROFDESTINY) {
        M_SMain_Draw();
    } else {
        M_Main_Draw();
    }

    M_DrawWindow ((viddef.width - (w + 10)) >> 1, (viddef.height - (h + 10)) >> 1, w + 10, h + 10, textcolour, colourBlack, highlight);

    Font_SetColour (FONT1, colourBlack);
    Font_put_line (FONT1, ((viddef.width - (w + 10)) >> 1) + 5, ((viddef.height - (h + 10)) >> 1) + 5, log_out_messages[ menu_random ]);
}


PUBLIC void M_Menu_Quit_f (void)
{
    menu_random = US_RndT() % 18;
    M_PushMenu (M_Quit_Draw, M_Quit_Key);
}
