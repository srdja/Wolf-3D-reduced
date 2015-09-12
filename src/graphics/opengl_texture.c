/*

    Copyright (C) 2004-2012 Michael Liebscher <johnnycanuck@users.sourceforge.net>
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
 * \file opengl_texture.c
 * \brief OpenGL Texture Manager.
 * \author Michael Liebscher
 * \date 2004-2012
 * \author Id Software, Inc.
 * \date 1997-2001
 * \note Portion of this code was derived from Quake II, and was originally written by Id Software, Inc.
 */

#include "../common.h"
#include "opengl_local.h"

static GLenum WrapToGL (TWrapMode mode)
{
    if (mode == Repeat) {
        return GL_REPEAT;
    } else {
        return GL_CLAMP;
    }
}

static GLenum MagFilterToGL (TMagFilter MagFilter)
{
    switch (MagFilter) {
    case Nearest:
        return GL_NEAREST;

    case Linear:
        return GL_LINEAR;

    default:
        break;
    }

    return GL_LINEAR;
}

static GLenum MinFilterToGL (bool MipMap, TMinFilter MinFilter)
{
    if (MipMap) {
        switch (MinFilter) {
        case NearestMipMapOff:
            return GL_NEAREST;

        case NearestMipMapNearest:
            return GL_NEAREST_MIPMAP_NEAREST;

        case NearestMipMapLinear:
            return GL_NEAREST_MIPMAP_LINEAR;

        case LinearMipMapOff:
            return GL_LINEAR;

        case LinearMipMapNearest:
            return GL_LINEAR_MIPMAP_NEAREST;

        case LinearMipMapLinear:
            return GL_LINEAR_MIPMAP_LINEAR;

        default:
            break;
        }
    } else {
        switch (MinFilter) {
        case NearestMipMapOff:
        case NearestMipMapNearest:
        case NearestMipMapLinear:
            return GL_NEAREST;

        case LinearMipMapOff:
        case LinearMipMapNearest:
        case LinearMipMapLinear:
            return GL_LINEAR;

        default:
            break;
        }
    }

    return GL_LINEAR;
}


/**
 * \brief Finds the difference between two angles.
 * \param[in] tex Angle in radians.
 * \param[in] data Angle in radians.
 * \return true if texture was uploaded, otherwise false.
 */
bool R_UploadTexture (texture_t *tex, uint8_t data)
{
    uint8_t *scaled;
    uint16_t scaled_width, scaled_height;
    int comp = tex->bytes;
    int gl_round_down = 1;

    glGenTextures (1, &tex->texnum);
    glBindTexture (GL_TEXTURE_2D, tex->texnum);


    for (scaled_width = 1 ; scaled_width < tex->width ; scaled_width <<= 1) {
        ;
    }

    if (gl_round_down && scaled_width > tex->width && tex->MipMap) {
        scaled_width >>= 1;
    }

    for (scaled_height = 1 ; scaled_height < tex->height ; scaled_height <<= 1) {
        ;
    }

    if (gl_round_down && scaled_height > tex->height && tex->MipMap) {
        scaled_height >>= 1;
    }

    // let people sample down the world textures for speed
    if (tex->MipMap) {
        scaled_width >>= 0;
        scaled_height >>= 0;
    }

    // don't ever bother with > glMaxTexSize textures
    if (scaled_width > glMaxTexSize) {
        scaled_width = glMaxTexSize;
    }

    if (scaled_height > glMaxTexSize) {
        scaled_height = glMaxTexSize;
    }

    if (scaled_width < 1) {
        scaled_width = 1;
    }

    if (scaled_height < 1) {
        scaled_height = 1;
    }

    tex->upload_width = scaled_width;
    tex->upload_height = scaled_height;

    scaled = malloc (scaled_width * scaled_height * tex->bytes);

    if (scaled_width == tex->width && scaled_height == tex->height) {
        memcpy (scaled, data, tex->width * tex->height * tex->bytes);
    } else {
        TM_ResampleTexture (data, tex->width, tex->height, scaled, scaled_width, scaled_height, tex->bytes, INTERPOLATION_NONE);
    }

// upload base image
    glTexImage2D (GL_TEXTURE_2D, 0, comp, scaled_width, scaled_height, 0, tex->bytes == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, scaled);

// upload mipmaps if required
    if (tex->MipMap) {
        int miplevel = 1;

        while (TM_MipMap (scaled, &scaled_width, &scaled_height, tex->bytes)) {
            glTexImage2D (GL_TEXTURE_2D, miplevel++, tex->bytes, scaled_width, scaled_height, 0, tex->bytes == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, scaled);
        }
    }
    free (scaled);

    if (tex->isTextureCube) {
        glTexParameteri (GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_S, WrapToGL (tex->WrapS));
        glTexParameteri (GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_T, WrapToGL (tex->WrapT));
        glTexParameteri (GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_R, WrapToGL (tex->WrapR));
        glTexParameteri (GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MIN_FILTER, MinFilterToGL (tex->MipMap, tex->MinFilter));
        glTexParameteri (GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MAG_FILTER, MagFilterToGL (tex->MagFilter));
    } else {
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, WrapToGL (tex->WrapS));
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, WrapToGL (tex->WrapT));
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, MinFilterToGL (tex->MipMap, tex->MinFilter));
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, MagFilterToGL (tex->MagFilter));
    }
    return true;
}

void R_DeleteTexture (unsigned int texnum)
{
    glDeleteTextures (1, &texnum);
}

void R_Bind (int texnum)
{
    // Is this texture already bound
    if (gl_state.currenttextures[ gl_state.currenttmu ] == texnum) {
        return;
    }
    gl_state.currenttextures[ gl_state.currenttmu ] = texnum;

    glBindTexture (GL_TEXTURE_2D, texnum);
}