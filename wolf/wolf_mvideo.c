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

#include "keycodes.h"
#include "wolf_menu.h"

#include "wolf_local.h"

#include "../env/renderer.h"


extern cvar_t *r_ref;
extern cvar_t *r_fullscreen;
extern cvar_t *vid_gamma;


PRIVATE cvar_t *gl_mode;
PRIVATE cvar_t *gl_driver;
PRIVATE cvar_t *gl_picmip;
PRIVATE cvar_t *gl_ext_palettedtexture;
PRIVATE cvar_t *gl_finish;



/////////////////////////////////////////////////////////////////////
//
//  MENU INTERACTION
//
/////////////////////////////////////////////////////////////////////

PRIVATE menuframework_s s_opengl_menu;

PRIVATE menulist_s      s_mode_list;
PRIVATE menuslider_s    s_tq_slider;
PRIVATE menuslider_s    s_brightness_slider;
PRIVATE menulist_s      s_fs_box;
PRIVATE menulist_s      s_paletted_texture_box;
PRIVATE menulist_s      s_finish_box;
PRIVATE menuaction_s    s_cancel_action;
PRIVATE menuaction_s    s_defaults_action;
PRIVATE menuaction_s    s_driver_action;




/*
-----------------------------------------------------------------------------
 Function:

 Parameters:

 Returns:

 Notes:
-----------------------------------------------------------------------------
*/
PRIVATE void BrightnessCallback (void *s)
{
    menuslider_s *slider = (menuslider_s *) s;


    s_brightness_slider.curvalue = s_brightness_slider.curvalue;


}

/*
-----------------------------------------------------------------------------
 Function:

 Parameters:

 Returns:

 Notes:
-----------------------------------------------------------------------------
*/
PRIVATE void ResetDefaults (void *unused)
{
    Video_MenuInit();
}

/*
-----------------------------------------------------------------------------
 Function:

 Parameters:

 Returns:

 Notes:
-----------------------------------------------------------------------------
*/
PRIVATE void ApplyChanges (void *unused)
{
    float gamma;



    // Scale to a range of -1.f to 1.f
    gamma = (s_brightness_slider.curvalue - 10.0f) / 10.0f;

    Cvar_SetValue ("vid_gamma", gamma);

    Cvar_SetValue ("gl_picmip", 3 - s_tq_slider.curvalue);
    Cvar_SetValue ("r_fullscreen", (float)s_fs_box.curvalue);
    Cvar_SetValue ("gl_ext_palettedtexture", (float)s_paletted_texture_box.curvalue);
    Cvar_SetValue ("gl_finish", (float)s_finish_box.curvalue);
    Cvar_SetValue ("gl_mode", (float)s_mode_list.curvalue);

    Cvar_Set ("r_ref", "gl");
    Cvar_Set ("gl_driver", OPENGL_DLL_NAME);


    /*
       update appropriate stuff if we're running OpenGL and gamma
       has been modified
    */

    if (vid_gamma->modified) {
        r_ref->modified = true;
    }

    if (gl_driver->modified) {
        r_ref->modified = true;
    }

    M_ForceMenuOff();
}

/*
-----------------------------------------------------------------------------
 Function:

 Parameters:

 Returns:

 Notes:
-----------------------------------------------------------------------------
*/
PRIVATE void CancelChanges (void *unused)
{
    extern void M_PopMenu (void);

    M_PopMenu();
}

/*
-----------------------------------------------------------------------------
 Function:

 Parameters:

 Returns:

 Notes:
-----------------------------------------------------------------------------
*/
PUBLIC void Video_MenuInit (void)
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

    if (! gl_driver)
        gl_driver = Cvar_Get ("gl_driver", OPENGL_DLL_NAME, CVAR_INIT);

    if (! gl_picmip)
        gl_picmip = Cvar_Get ("gl_picmip", "0", CVAR_INIT);

    if (! gl_mode)
        gl_mode = Cvar_Get ("gl_mode", "3", CVAR_INIT);

    if (! gl_ext_palettedtexture)
        gl_ext_palettedtexture = Cvar_Get ("gl_ext_palettedtexture", "1", CVAR_ARCHIVE);

    if (! gl_finish)
        gl_finish = Cvar_Get ("gl_finish", "1", CVAR_ARCHIVE);


    s_mode_list.curvalue = (int)gl_mode->value;


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

    s_brightness_slider.generic.type    = MTYPE_SLIDER;
    s_brightness_slider.generic.x   = 0;
    s_brightness_slider.generic.y   = y += nYOffset;
    s_brightness_slider.generic.fs      = FONT1;
    s_brightness_slider.generic.fontBaseColour = &textcolour;
    s_brightness_slider.generic.fontHighColour = &highlight;
    s_brightness_slider.generic.name    = "brightness";
    s_brightness_slider.generic.callback = BrightnessCallback;
    s_brightness_slider.minvalue = 0;
    s_brightness_slider.maxvalue = 20;
    s_brightness_slider.curvalue = (vid_gamma->value + 10) * 10;

    s_fs_box.generic.type = MTYPE_SPINCONTROL;
    s_fs_box.generic.x  = 0;
    s_fs_box.generic.y  = y += nYOffset;
    s_fs_box.generic.fs     = FONT1;
    s_fs_box.generic.fontBaseColour = &textcolour;
    s_fs_box.generic.fontHighColour = &readcolour;
    s_fs_box.generic.name   = "fullscreen";
    s_fs_box.itemnames = yesno_names;
    s_fs_box.curvalue = (int)r_fullscreen->value;



    s_tq_slider.generic.type    = MTYPE_SLIDER;
    s_tq_slider.generic.x       = 0;
    s_tq_slider.generic.y       = y += nYOffset;
    s_tq_slider.generic.fs      = FONT1;
    s_tq_slider.generic.fontBaseColour = &textcolour;
    s_tq_slider.generic.fontHighColour = &highlight;
    s_tq_slider.generic.name    = "texture quality";
    s_tq_slider.minvalue = 0;
    s_tq_slider.maxvalue = 3;
    s_tq_slider.curvalue = 3 - gl_picmip->value;

    s_finish_box.generic.type = MTYPE_SPINCONTROL;
    s_finish_box.generic.x  = 0;
    s_finish_box.generic.y  = y += nYOffset;
    s_finish_box.generic.fs     = FONT1;
    s_finish_box.generic.fontBaseColour = &textcolour;
    s_finish_box.generic.fontHighColour = &readcolour;
    s_finish_box.generic.name = "sync every frame";
    s_finish_box.curvalue = (int)gl_finish->value;
    s_finish_box.itemnames = yesno_names;

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

/*
-----------------------------------------------------------------------------
 Function:

 Parameters:

 Returns:

 Notes:
-----------------------------------------------------------------------------
*/
void Video_MenuDraw (void)
{

    if (g_version->value == SPEAROFDESTINY) {
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

/*
-----------------------------------------------------------------------------
 Function:

 Parameters:

 Returns:

 Notes:
-----------------------------------------------------------------------------
*/
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


