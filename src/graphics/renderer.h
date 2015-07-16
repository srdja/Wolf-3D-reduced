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
 *  renderer.h: Interface to graphics API.
 *
 *  Author: Michael Liebscher   <johnnycanuck@users.sourceforge.net>
 *  Date:   2004
 *
 *  Acknowledgement:
 *  This code was derived from Quake II, and was originally
 *  written by Id Software, Inc.
 *
 */

/*

    Notes:

    This module communicates with the graphics API. The API can be any graphics
    API, e.g OpenGL, DirectX, SDL, GDI, etc; as long as the functions listed in
    this header are implemented.

*/

#ifndef __RENDERER_H__
#define __RENDERER_H__

#include "../util/filesystem.h"

#include "platform.h"
#include "texture_manager.h"
#include "font_manager.h"

int opengl_init();

void R_BeginRegistration (const char *model);
void R_EndRegistration (void);
void R_BeginFrame (void);
void R_EndFrame (void);

void R_DeleteTexture (unsigned int texnum);
_boolean R_UploadTexture (texture_t *tex, PW8 data);

void R_SetGL2D (void);

void R_Draw_Pic (int x, int y, const char *name);
void R_Draw_Character (int x, int y, int num, font_t *myfont);
void R_Draw_Tile (int x, int y, int w, int h, const char *name);
void R_Draw_Fill (int x, int y, int w, int h, colour3_t c);
void R_Draw_Line (int nXStart, int nYStart, int nXEnd, int nYEnd, int width, colour3_t c);


#endif /* __RENDERER_H__ */
