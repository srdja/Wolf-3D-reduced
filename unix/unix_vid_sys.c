/*

    Copyright (C) 2004 Michael Liebscher <johnnycanuck@users.sourceforge.net>
    Copyright (C) 1997-2001 Id Software, Inc.

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

    See the GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

/*
 *  unix_vid.c: Main windowed and fullscreen graphics interface module.
 *
 *  Author: Michael Liebscher <johnnycanuck@users.sourceforge.net>
 *  Date:   2004
 *
 *  Acknowledgement:
 *  This code was derived from Quake II, and was originally
 *  written by Id Software, Inc.
 *
 */

#include <unistd.h>

#include "../env/client.h"
#include "../env/renderer.h"
#include "../env/video.h"

// Global variables used internally by this module
viddef_t    viddef; // global video state; used by other modules

#define VID_NUM_MODES ( sizeof( vid_modes ) / sizeof( vid_modes[ 0 ] ) )


typedef struct vidmode_s {
    const char *description;
    int         width, height;
    int         mode;

} vidmode_t;

vidmode_t vid_modes[] = {
    { "Mode 0: 640x480",   640,   480, 0 },
    { "Mode 1: 800x600",   800,   600, 1 },
    { "Mode 2: 960x720",   960,   720, 2 },
    { "Mode 3: 1024x768",  1024,  768, 3 },
    { "Mode 4: 1152x864",  1152,  864, 4 },
    { "Mode 5: 1280x960",  1280,  960, 5 },
    { "Mode 6: 1600x1200", 1600, 1200, 6 },
    { "Mode 7: 2048x1536", 2048, 1536, 7 }
};


/**
 * \brief Get window mode information.
 * \param[out] width File or directory must have these attributes.
 * \param[out] height File or directory can not have these attributes.
 * \param[in] mode Video mode (see vidmode_t for details).
 * \return true if mode was accepted, otherwise false.
 */
PUBLIC _boolean VID_GetModeInfo (int *width, int *height, int mode)
{
    if (mode < 0 || mode >= VID_NUM_MODES) {
        return false;
    }

    *width  = vid_modes[ mode ].width;
    *height = vid_modes[ mode ].height;

    return true;
}

/**
 * \brief Set new window width height.
 * \param[in] width Width of new window.
 * \param[in] height Height of new window.
 */
PUBLIC void VID_NewWindow (int width, int height)
{
    viddef.width  = width;
    viddef.height = height;
}

PUBLIC void Video_Init (void)
{
    if (R_Init (0, 0) == -1) {
        printf("Renderer initialization failed\n");
        exit(1);
    }
}