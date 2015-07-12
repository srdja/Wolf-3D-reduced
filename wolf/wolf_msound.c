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
 *  wolf_msound.c:   Sound menu.
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

#include "../env/sound.h"
#include "../env/renderer.h"

/////////////////////////////////////////////////////////////////////
//
//  SOUND MENU
//
/////////////////////////////////////////////////////////////////////

PRIVATE menuframework_s s_sound_menu;
PRIVATE menuslider_s    s_sound_mastervolume_slider;
PRIVATE menuslider_s    s_sound_sfxvolume_slider;
PRIVATE menuslider_s    s_sound_musicvolume_slider;
PRIVATE menulist_s      s_sound_device_list;


#define SOUNDMSG    "Restarting the sound system.\nThis could take up to a\nminute, so please be patient."




PRIVATE void UpdateSoundDeviceFunc (void *unused)
{
    Cvar_Set ("s_device", s_sound_device_list.itemnames[ s_sound_device_list.curvalue ]);
}

PRIVATE void UpdateMasterVolumeFunc (void *unused)
{
    Cvar_SetValue ("s_masterVolume", s_sound_mastervolume_slider.curvalue / 10);
}

PRIVATE void UpdateSFXVolumeFunc (void *unused)
{
    Cvar_SetValue ("s_sfxVolume", s_sound_sfxvolume_slider.curvalue / 10);
}

PRIVATE void UpdateMusicVolumeFunc (void *unused)
{
    Cvar_SetValue ("s_musicVolume", s_sound_musicvolume_slider.curvalue / 10);
}

PRIVATE void Sound_MenuInit (void)
{
    // configure controls menu and menu items
    s_sound_menu.x = (viddef.width >> 1) + 70;
    s_sound_menu.y = (viddef.height >> 1) - 20;
    s_sound_menu.nitems = 0;

    s_sound_mastervolume_slider.generic.type    = MTYPE_SLIDER;
    s_sound_mastervolume_slider.generic.x   = 0;
    s_sound_mastervolume_slider.generic.y   = 0;
    s_sound_mastervolume_slider.generic.name    = "Master Volume:";
    s_sound_mastervolume_slider.generic.fs  = FONT1;
    s_sound_mastervolume_slider.generic.fontBaseColour = &textcolour;
    s_sound_mastervolume_slider.generic.callback    = UpdateMasterVolumeFunc;
    s_sound_mastervolume_slider.minvalue        = 0;
    s_sound_mastervolume_slider.maxvalue        = 10;
    s_sound_mastervolume_slider.curvalue        = Cvar_VariableValue ("s_masterVolume") * 10;


    s_sound_sfxvolume_slider.generic.type   = MTYPE_SLIDER;
    s_sound_sfxvolume_slider.generic.x  = 0;
    s_sound_sfxvolume_slider.generic.y  = 30;
    s_sound_sfxvolume_slider.generic.name   = "Effects Volume:";
    s_sound_sfxvolume_slider.generic.fs     = FONT1;
    s_sound_sfxvolume_slider.generic.fontBaseColour = &textcolour;
    s_sound_sfxvolume_slider.generic.callback   = UpdateSFXVolumeFunc;
    s_sound_sfxvolume_slider.minvalue       = 0;
    s_sound_sfxvolume_slider.maxvalue       = 10;
    s_sound_sfxvolume_slider.curvalue       = Cvar_VariableValue ("s_sfxVolume") * 10;


    s_sound_musicvolume_slider.generic.type = MTYPE_SLIDER;
    s_sound_musicvolume_slider.generic.x    = 0;
    s_sound_musicvolume_slider.generic.y    = 60;
    s_sound_musicvolume_slider.generic.name = "Music Volume:";
    s_sound_musicvolume_slider.generic.fs   = FONT1;
    s_sound_musicvolume_slider.generic.fontBaseColour = &textcolour;
    s_sound_musicvolume_slider.generic.callback = UpdateMusicVolumeFunc;
    s_sound_musicvolume_slider.minvalue     = 0;
    s_sound_musicvolume_slider.maxvalue     = 10;
    s_sound_musicvolume_slider.curvalue     = Cvar_VariableValue ("s_musicVolume") * 10;


    if (sound_initialized) {
        s_sound_device_list.generic.type    = MTYPE_SPINCONTROL;
        s_sound_device_list.generic.x       = 0;
        s_sound_device_list.generic.y       = 90;
        s_sound_device_list.generic.name    = "Sound Device:";
        s_sound_device_list.generic.fs      = FONT1;
        s_sound_device_list.generic.fontBaseColour = &textcolour;
        s_sound_device_list.generic.fontHighColour = &readcolour;
        s_sound_device_list.generic.callback = UpdateSoundDeviceFunc;
        s_sound_device_list.itemnames   = sound_devices;
        s_sound_device_list.curvalue    = numDefaultSoundDevice;
    }

    Menu_AddItem (&s_sound_menu, (void *) &s_sound_mastervolume_slider);
    Menu_AddItem (&s_sound_menu, (void *) &s_sound_sfxvolume_slider);
    Menu_AddItem (&s_sound_menu, (void *) &s_sound_musicvolume_slider);

    if (sound_initialized) {
        Menu_AddItem (&s_sound_menu, (void *) &s_sound_device_list);
    }

}



PRIVATE void Sound_MenuDraw (void)
{
    if (g_version->value == SPEAROFDESTINY) {
        R_Draw_Tile (0, 0, viddef.width, viddef.height, "pics/C_BACKDROPPIC.tga");

        M_BannerString ("Sound Setup", 15);
        M_DrawWindow (((viddef.width - 500) >> 1), ((viddef.height - 150) >> 1) + 35, 500, 150,
                      sodbkgdcolour, sodbord2colour, soddeactive);
    } else {
        R_Draw_Fill (0, 0, viddef.width, viddef.height, bgcolour);

        M_BannerString ("Sound Setup", 15);
        M_DrawWindow (((viddef.width - 500) >> 1), ((viddef.height - 150) >> 1) + 35, 500, 150,
                      bkgdcolour, bord2colour, deactive);
    }


    M_DrawInfoBar();

    Menu_AdjustCursor (&s_sound_menu, 1);
    Menu_Draw (&s_sound_menu);
}

PRIVATE const char *Sound_MenuKey (int key)
{
    return Default_MenuKey (&s_sound_menu, key);
}

PUBLIC void M_Menu_Sound_f (void)
{
    Sound_MenuInit();
    M_PushMenu (Sound_MenuDraw, Sound_MenuKey);
}

