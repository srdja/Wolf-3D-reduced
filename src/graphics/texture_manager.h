/*

    Copyright (C) 2004 Michael Liebscher
    Copyright (C) 1997-2001 Id Software, Inc.
    Copyright (C) 1995 Spencer Kimball and Peter Mattis.

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
 *  texture_manager.c:  Texture manager.
 *
 *  Author: Michael Liebscher <johnnycanuck@users.sourceforge.net>
 *  Date:   2004
 *
 *  Acknowledgement:
 *  Portion of this code was derived from
 *  The GIMP (an image manipulation program) and was originally
 *  written by Spencer Kimball and Peter Mattis.
 *
 *  Portion of this code was derived from Quake II, and was originally
 *  written by Id Software, Inc.
 *
 */

#ifndef __TEXTURE_MANAGER_H__
#define __TEXTURE_MANAGER_H__


#include "../util/filesystem.h"

#define     MAX_TEXTURES    1024

typedef enum {
    TT_Sprite,
    TT_Wall,
    TT_Pic,
} texturetype_t;

typedef enum {
    Repeat = 0,
    Clamp,
} TWrapMode;

typedef enum {
    Nearest = 0,
    Linear,

} TMagFilter;

typedef enum {
    NearestMipMapOff = 0,
    NearestMipMapNearest,
    NearestMipMapLinear,
    LinearMipMapOff,
    LinearMipMapNearest,
    LinearMipMapLinear,
} TMinFilter;


typedef struct texture_s {
    bool        isTextureCube;

    TWrapMode       WrapS;
    TWrapMode       WrapT;
    TWrapMode       WrapR;

    TMinFilter      MinFilter;
    TMagFilter      MagFilter;

    uint32_t registration_sequence;      // 0 = free
    uint16_t width, height;
    uint16_t upload_width, upload_height;
    unsigned int texnum;
    uint16_t bytes;
    texturetype_t type;
    char    name[ MAX_GAMEPATH ];           // game path, including extension
} texture_t;


typedef enum {
    INTERPOLATION_NONE,   /* None (Fastest) */
} InterpolationType;


extern uint32_t texture_registration_sequence;

void TM_Init (void);

unsigned int TM_getWallTextureId (uint32_t imageId);
unsigned int TM_getSpriteTextureId (uint32_t imageId);

texture_t *TM_FindTexture_Wall (uint32_t imageId);
texture_t *TM_FindTexture_Sprite (uint32_t imageId);
texture_t *TM_FindTexture (const char *name, texturetype_t type);

void TM_GetTextureSize (int32_t *width, int32_t *height, const char *name);
void TM_ResampleTexture (uint8_t *in, int inwidth, int inheight, uint8_t *out,  int outwidth, int outheight, uint16_t bytes, InterpolationType interpolation);
void TM_FreeUnusedTextures (void);


#endif /* __TEXTURE_MANAGER_H__ */
