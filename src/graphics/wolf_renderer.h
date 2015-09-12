/*

    Copyright (C) 2004 Michael Liebscher <johnnycanuck@users.sourceforge.net>
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
 *  wolf_renderer.h:  Wolfenstein3-D renderer.
 *
 *  Author: Michael Liebscher <johnnycanuck@users.sourceforge.net>
 *  Date:   2004
 *
 *  Acknowledgement:
 *  Portion of this code was derived from Quake II, and was originally
 *  written by Id Software, Inc.
 *
 */

/*
    Notes:
    This module is implemented by wolf_renderer.c and wolf_opengl.c

*/

#ifndef __WOLF_RENDERER_H__
#define __WOLF_RENDERER_H__



#include "../game/wolf_math.h"


void R_SetGL3D (placeonplane_t viewport);


void R_ResetFlash (void);
void R_DamageFlash (int damage);


void R_DrawBox (int x, int y, int w, int h, uint32_t color);

void R_Draw_Door (int x, int y, float z1, float z2, bool vertical, bool backside, int tex, int amount);
void R_Draw_Wall (float x, float y, float z1, float z2, int type, int tex);



void R_DrawSprites (void);

void R_DrawPsyched (uint32_t percent);

void R_DrawHUD (void);
void R_DrawNumber (int x, int y, int number);
void R_DrawWeapon (void);


void R_put_line (int x, int y, const char *string);


#endif /* __WOLF_RENDERER_H__ */
