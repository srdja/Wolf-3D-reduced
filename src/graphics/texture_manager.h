/*

    Copyright (C) 2004 Michael Liebscher
    Copyright (C) 1997-2001 Id Software, Inc.
    Copyright (C) 1995 Spencer Kimball and Peter Mattis.
    Copyright (C) 2015 Srđan Panić

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

#ifndef __TEXTURE_MANAGER_H__
#define __TEXTURE_MANAGER_H__

#include "../util/filesystem.h"
#include <GL/gl.h>

typedef enum {
    TT_Sprite,
    TT_Wall,
    TT_Pic,
} TextureType;

typedef struct texture_s {
    GLfloat WrapS;
    GLfloat WrapT;
    GLfloat MinFilter;
    GLfloat MagFilter;
    GLint   id;

    uint32_t cache_index;

    uint16_t width;
    uint16_t height;
    uint16_t bytes_per_pixel;

    TextureType type;
    char name[MAX_GAMEPATH];
} Texture;


void texture_tm_init(void);
void texture_cache_advance_index(void);
void texture_cache_remove_unused(void);

Texture *texture_get_wall(uint32_t id);
Texture *texture_get_sprite(uint32_t id);
Texture *texture_get_picture(char *name);


#endif /* __TEXTURE_MANAGER_H__ */
