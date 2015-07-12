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

#include <string.h>

#include "opengl_local.h"

#include "memory.h"
#include "zmem.h"
#include "platform.h"
#include "common_utils.h"


// ***************************************************************************

PRIVATE INLINECALL GLenum WrapToGL (TWrapMode mode)
{
    if (mode == Repeat) {
        return GL_REPEAT;
    } else {
        if (gl_config.Version_1_2) {
            return GL_CLAMP_TO_EDGE;
        } else {
            return GL_CLAMP;
        }
    }
}

PRIVATE INLINECALL GLenum MagFilterToGL (TMagFilter MagFilter)
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



PRIVATE INLINECALL GLenum MinFilterToGL (_boolean MipMap, TMinFilter MinFilter)
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

// ***************************************************************************



/**
 * \brief Finds the difference between two angles.
 * \param[in] tex Angle in radians.
 * \param[in] data Angle in radians.
 * \return true if texture was uploaded, otherwise false.
 */
PUBLIC _boolean R_UploadTexture (texture_t *tex, PW8 data)
{
    W8 *scaled;
    W16 scaled_width, scaled_height;
    int comp = tex->bytes;

    pfglGenTextures (1, &tex->texnum);
    pfglBindTexture (GL_TEXTURE_2D, tex->texnum);





    for (scaled_width = 1 ; scaled_width < tex->width ; scaled_width <<= 1) {
        ;
    }

    if (gl_round_down->value && scaled_width > tex->width && tex->MipMap) {
        scaled_width >>= 1;
    }



    for (scaled_height = 1 ; scaled_height < tex->height ; scaled_height <<= 1) {
        ;
    }

    if (gl_round_down->value && scaled_height > tex->height && tex->MipMap) {
        scaled_height >>= 1;
    }



    // let people sample down the world textures for speed
    if (tex->MipMap) {
        scaled_width >>= (int)gl_picmip->value;
        scaled_height >>= (int)gl_picmip->value;
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



    scaled = (PW8)Z_Malloc (scaled_width * scaled_height * tex->bytes);



    if (scaled_width == tex->width && scaled_height == tex->height) {
        memcpy (scaled, data, tex->width * tex->height * tex->bytes);
    } else {
        TM_ResampleTexture (data, tex->width, tex->height, scaled, scaled_width, scaled_height, tex->bytes, INTERPOLATION_NONE);
    }


// upload base image
    pfglTexImage2D (GL_TEXTURE_2D, 0, comp, scaled_width, scaled_height, 0, tex->bytes == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, scaled);


// upload mipmaps if required
    if (tex->MipMap) {
        int miplevel = 1;

        while (TM_MipMap (scaled, &scaled_width, &scaled_height, tex->bytes)) {
            pfglTexImage2D (GL_TEXTURE_2D, miplevel++, tex->bytes, scaled_width, scaled_height, 0, tex->bytes == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, scaled);
        }
    }


    Z_Free (scaled);




    if (tex->isTextureCube) {
        pfglTexParameteri (GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_S, WrapToGL (tex->WrapS));
        pfglTexParameteri (GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_T, WrapToGL (tex->WrapT));
        pfglTexParameteri (GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_R, WrapToGL (tex->WrapR));
        pfglTexParameteri (GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MIN_FILTER, MinFilterToGL (tex->MipMap, tex->MinFilter));
        pfglTexParameteri (GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MAG_FILTER, MagFilterToGL (tex->MagFilter));
    } else {
        pfglTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, WrapToGL (tex->WrapS));
        pfglTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, WrapToGL (tex->WrapT));
        pfglTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, MinFilterToGL (tex->MipMap, tex->MinFilter));
        pfglTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, MagFilterToGL (tex->MagFilter));
    }

    if (gl_ext.EXTTextureFilterAnisotropic) {
        pfglTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, gl_ext.nMaxAnisotropy);
    }


    return true;
}


/*
-----------------------------------------------------------------------------
 Function:

 Parameters:

 Returns:

 Notes:

-----------------------------------------------------------------------------
*/
PUBLIC void R_DeleteTexture (unsigned int texnum)
{
    pfglDeleteTextures (1, &texnum);
}

/*
-----------------------------------------------------------------------------
 Function:

 Parameters:

 Returns:

 Notes:

-----------------------------------------------------------------------------
*/
PUBLIC void R_TexEnv (GLenum mode)
{
    static int lastmodes[ 4 ] = { -1, -1, -1, -1 };

    if (mode != lastmodes[ gl_state.currenttmu ]) {
        pfglTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, mode);
        lastmodes[ gl_state.currenttmu ] = mode;
    }
}

/*
-----------------------------------------------------------------------------
 Function:

 Parameters:

 Returns:

 Notes:

-----------------------------------------------------------------------------
*/
PUBLIC void R_SelectTexture (GLenum texture)
{
    int tmu;

    if (! pfglActiveTextureARB) {
        return;
    }

    if (texture == GL_TEXTURE0) {
        tmu = 0;
    } else if (texture == GL_TEXTURE1) {
        tmu = 1;
    } else if (texture == GL_TEXTURE2) {
        tmu = 2;
    } else {
        tmu = 3;
    }

    if (tmu == gl_state.currenttmu) {
        return;
    }


    gl_state.currenttmu = tmu;

    pfglActiveTextureARB (texture);
    pfglClientActiveTextureARB (texture);

}


/*
-----------------------------------------------------------------------------
 Function:

 Parameters:

 Returns:

 Notes:

-----------------------------------------------------------------------------
*/
PUBLIC void R_Bind (int texnum)
{
    // Is this texture already bound
    if (gl_state.currenttextures[ gl_state.currenttmu ] == texnum) {
        return;
    }

    gl_state.currenttextures[ gl_state.currenttmu ] = texnum;

    pfglBindTexture (GL_TEXTURE_2D, texnum);
}

/*
-----------------------------------------------------------------------------
 Function:

 Parameters:

 Returns:

 Notes:

-----------------------------------------------------------------------------
*/
PUBLIC void R_MBind (GLenum target, int texnum)
{
    R_SelectTexture (target);

    if (target == GL_TEXTURE0) {
        if (gl_state.currenttextures[ 0 ] == texnum) {
            return;
        }
    } else if (target == GL_TEXTURE1) {
        if (gl_state.currenttextures[ 1 ] == texnum) {
            return;
        }
    } else if (target == GL_TEXTURE2) {
        if (gl_state.currenttextures[ 2 ] == texnum) {
            return;
        }
    } else {
        if (gl_state.currenttextures[ 3 ] == texnum) {
            return;
        }
    }

    R_Bind (texnum);
}

/*
-----------------------------------------------------------------------------
 Function:

 Parameters:

 Returns:

 Notes:

-----------------------------------------------------------------------------
*/
PUBLIC void R_EnableMultitexture (_boolean enable)
{
    if (! pfglActiveTextureARB) {
        return;
    }

    if (enable) {
        R_SelectTexture (GL_TEXTURE1);
        pfglEnable (GL_TEXTURE_2D);
        R_TexEnv (GL_REPLACE);
    } else {
        R_SelectTexture (GL_TEXTURE1);
        pfglDisable (GL_TEXTURE_2D);
        R_TexEnv (GL_REPLACE);
    }

    R_SelectTexture (GL_TEXTURE0);
    R_TexEnv (GL_REPLACE);

}

