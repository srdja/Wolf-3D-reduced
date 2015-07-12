/*

    Copyright (C) 2004-2013 Michael Liebscher <johnnycanuck@users.sourceforge.net>
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
 * \file wolf_mcredits.c
 * \brief Credits menu.
 * \author Michael Liebscher
 * \date 2004-2013
 * \note Portion of this code was derived from Quake II, and was originally written by Id Software, Inc.
 */

#include "wolf_menu.h"

#include "wolf_local.h"

#include "../env/renderer.h"

/////////////////////////////////////////////////////////////////////
//
//  Credits
//
/////////////////////////////////////////////////////////////////////


PRIVATE const char **credits;
PRIVATE char *creditsIndex[ 256 ];


PRIVATE const char *reduxcredits[] = {
    "Wolfenstein 3-D Redux",
    "",
    "+Code Conductor",
    "Michael Liebscher",
    "email: johnnycanuck@users.sourceforge.net",
    "",
    "Be sure to check out",
    "+http://wolf3dredux.sourceforge.net",
    "for the latest releases and news.",
    "",
    "",
    "+This project would not be possible without",
    "+the contributions made by the following:",
    "Ahmes, Pythagoras of Samos,",
    "Euclid of Alexandria, Archimedes of Syracuse,",
    "Pierre de Fermat, Sir Isaac Newton,",
    "Jean Baptiste Joseph Fourier,",
    "Carl Friedrich Gauss, Charles Babbage,",
    "Alan Mathison Turing, David A. Huffman,",
    "Id Software, NewWolf, MAME DEV Team,",
    "zlib (Jean-loup Gailly and Mark Adler),",
    "The GIMP (Spencer Kimball and Peter Mattis),",
    "Todd C. Miller, Steven Fuller, Peter A. Friend",
    "and players like you.",
    0
};

/**
 * \brief Callback to draw the credits menu
 */
PRIVATE void M_Credits_MenuDraw (void)
{
    int i, x, y;
    int w, h;

    if (g_version->value == SPEAROFDESTINY) {
        R_Draw_Tile (0, 0, viddef.width, viddef.height, "pics/C_BACKDROPPIC.tga");
    } else {
        R_Draw_Fill (0, 0, viddef.width, viddef.height, bgcolour);
    }

    Font_SetSize (FONT0, 1);
    Font_SetColour (FONT0, colourWhite);

//
//  Draw the credits
//
    for (i = 0, y = 50; credits[ i ] ; y += Font_GetSize (FONT0), i++) {
        int stringoffset = 0;
        int bold = false;

        if (credits[ i ][ 0 ] == '+') {
            bold = true;
            stringoffset = 1;
        } else {
            bold = false;
            stringoffset = 0;
        }

        Font_GetMsgDimensions (FONT0, credits[ i ] + stringoffset, &w, &h);

        x = (viddef.width - w) >> 1;

        if (bold) {
            Font_SetColour (FONT0, readhcolour);
            Font_put_line (FONT0, x, y, credits[ i ] + stringoffset);
            Font_SetColour (FONT0, colourWhite);
        } else {
            Font_put_line (FONT0, x, y, credits[ i ] + stringoffset);
        }
    }

}

/**
 * \brief Callback for key input, causes the credits menu to close
 */
PRIVATE const char *M_Credits_Key (int key)
{
    M_PopMenu();

    return menu_out_sound;
}

/**
 * \brief Trigger for credits menu
 */
PUBLIC void M_Menu_Credits_f (void)
{
    credits = reduxcredits;

    M_PushMenu (M_Credits_MenuDraw, M_Credits_Key);
}
