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
 *  wolf_mvideo.c:   Video menu.
 *
 *  Author: Michael Liebscher   <johnnycanuck@users.sourceforge.net>
 *  Date:   2004
 *
 *  Acknowledgement:
 *  This code was derived from Quake II, and was originally
 *  written by Id Software, Inc.
 *
 */

#include "../input/keycodes.h"
#include "wolf_menu.h"

#include "wolf_local.h"

#include "../graphics/renderer.h"

static int gl_mode;


/////////////////////////////////////////////////////////////////////
//
//  MENU INTERACTION
//
/////////////////////////////////////////////////////////////////////

static menuframework_s s_opengl_menu;

static menulist_s      s_mode_list;
static menuslider_s    s_tq_slider;
static menuslider_s    s_brightness_slider;
static menulist_s      s_fs_box;
static menulist_s      s_paletted_texture_box;
static menulist_s      s_finish_box;
static menuaction_s    s_cancel_action;
static menuaction_s    s_defaults_action;


static void ResetDefaults()
{
    Video_MenuInit();
}


static void ApplyChanges (void *unused)
{
    M_ForceMenuOff();
}

static void CancelChanges (void *unused)
{
    extern void M_PopMenu (void);

    M_PopMenu();
}


void Video_MenuInit (void)
{
    static const char *resolutions[] = {
        "[640 480]",
        "[800 600]",
        "[960 720]",
        "[1024 768]",
        "[1152 864]",
        "[1280 960]",
        "[1600 1200]",
        "[2048 1536]",
        0
    };

    static const char *yesno_names[] = {
        "no",
        "yes",
        0
    };

    int y = 0;
    int nYOffset = 27;

    s_mode_list.curvalue = gl_mode;

    s_opengl_menu.x = (viddef.width >> 1) + 80;
    s_opengl_menu.nitems = 0;

    s_mode_list.generic.type = MTYPE_SPINCONTROL;
    s_mode_list.generic.name = "Video Mode";
    s_mode_list.generic.x = 0;
    s_mode_list.generic.y = y;
    s_mode_list.generic.fs      = FONT1;
    s_mode_list.generic.fontBaseColour = &textcolour;
    s_mode_list.generic.fontHighColour = &readcolour;
    s_mode_list.itemnames = resolutions;

    s_defaults_action.generic.type = MTYPE_ACTION;
    s_defaults_action.generic.name = "reset to defaults";
    s_defaults_action.generic.x    = 0;
    s_defaults_action.generic.y    = y += nYOffset;
    s_defaults_action.generic.fs        = FONT1;
    s_defaults_action.generic.fontBaseColour = &textcolour;
    s_defaults_action.generic.fontHighColour = &highlight;
    s_defaults_action.generic.callback = ResetDefaults;

    s_cancel_action.generic.type = MTYPE_ACTION;
    s_cancel_action.generic.name = "cancel";
    s_cancel_action.generic.x    = 0;
    s_cancel_action.generic.y    = y += nYOffset;
    s_cancel_action.generic.fs      = FONT1;
    s_cancel_action.generic.fontBaseColour = &textcolour;
    s_cancel_action.generic.fontHighColour = &highlight;
    s_cancel_action.generic.callback = CancelChanges;

//  Menu_AddItem( &s_opengl_menu, ( void * ) &s_ref_list );
    Menu_AddItem (&s_opengl_menu, (void *) &s_mode_list);
    Menu_AddItem (&s_opengl_menu, (void *) &s_brightness_slider);
    Menu_AddItem (&s_opengl_menu, (void *) &s_fs_box);
    Menu_AddItem (&s_opengl_menu, (void *) &s_tq_slider);
//  Menu_AddItem( &s_opengl_menu, ( void * ) &s_paletted_texture_box );
    Menu_AddItem (&s_opengl_menu, (void *) &s_finish_box);

    Menu_AddItem (&s_opengl_menu, (void *) &s_defaults_action);

    Menu_AddItem (&s_opengl_menu, (void *) &s_cancel_action);

    Menu_Center (&s_opengl_menu);
    s_opengl_menu.x -= 8;
}


void Video_MenuDraw (void)
{

    if (g_version == SPEAROFDESTINY) {
        R_Draw_Tile (0, 0, viddef.width, viddef.height, "pics/C_BACKDROPPIC.tga");

        M_BannerString ("Video Setup", 15);
        M_DrawWindow (((viddef.width - 550) >> 1), ((viddef.height - 335) >> 1), 550, 335,
                      sodbkgdcolour, sodbord2colour, soddeactive);
    } else {
        R_Draw_Fill (0, 0, viddef.width, viddef.height, bgcolour);

        M_BannerString ("Video Setup", 15);
        M_DrawWindow (((viddef.width - 550) >> 1), ((viddef.height - 335) >> 1), 550, 335,
                      bkgdcolour, bord2colour, deactive);
    }


    // move cursor to a reasonable starting position
    Menu_AdjustCursor (&s_opengl_menu, 1);

    // draw the menu
    Menu_Draw (&s_opengl_menu);
}


const char *Video_MenuKey (int key)
{
    menuframework_s *m = &s_opengl_menu;

    switch (key) {
    case K_ESCAPE:
        ApplyChanges (0);
        return NULL;

    case K_KP_UPARROW:
    case K_UPARROW:
        m->cursor--;
        Menu_AdjustCursor (m, -1);
        break;

    case K_KP_DOWNARROW:
    case K_DOWNARROW:
        m->cursor++;
        Menu_AdjustCursor (m, 1);
        break;

    case K_KP_LEFTARROW:
    case K_LEFTARROW:
        Menu_SlideItem (m, -1);
        break;

    case K_KP_RIGHTARROW:
    case K_RIGHTARROW:
        Menu_SlideItem (m, 1);
        break;

    case K_KP_ENTER:
    case K_ENTER:
        if (! Menu_SelectItem (m))
            ApplyChanges (NULL);

        break;
    }

    return NULL;
}