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
 *	texture_manager.c:	Texture manager.
 *	
 *	Author:	Michael Liebscher <johnnycanuck@users.sourceforge.net>	 
 *	Date:	2004  
 *
 *	Acknowledgement:
 *	Portion of this code was derived from
 *	The GIMP (an image manipulation program) and was originally
 *	written by Spencer Kimball and Peter Mattis.
 *
 *	Portion of this code was derived from Quake II, and was originally
 *	written by Id Software, Inc.
 *
 */

#ifndef __TEXTURE_MANAGER_H__
#define __TEXTURE_MANAGER_H__

#include "platform.h"

#include "filesystem.h"


#define		MAX_TEXTURES	1024


typedef enum 
{
	TT_Sprite,
	TT_Wall,
	TT_Pic,

	TextureTypeCount

} texturetype_t;

typedef enum 
{
	Repeat = 0,
	Clamp,

	WrapModeCount	// Number of Wrap modes

} TWrapMode;

typedef enum 
{
	Auto = 0,
	RGBA8888,
	RGBA4444,
	RGBA5551,
	RGB888,
	RGB565,
	DXTC1,
	DXTC1Alpha,
	DXTC3,
	DXTC5,
	Luminance,
	Alpha,
	AlphaLuminance,
	DsDt,

	UpImageFormatCount	// Number of Upload formats

} TTexFormat;

typedef enum 
{
	Nearest = 0,
	Linear,

	MagFilterCount	// Number of Magnification filters

} TMagFilter;

typedef enum 
{
	NearestMipMapOff = 0,
	NearestMipMapNearest,
	NearestMipMapLinear,
	LinearMipMapOff,
	LinearMipMapNearest,
	LinearMipMapLinear,

	MinFilterCount	// Number of Min filters

} TMinFilter;


typedef struct texture_s
{
	_boolean		MipMap;
	_boolean		isTextureCube;
	
    TTexFormat		UploadFormat;
	
    TWrapMode		WrapS;
	TWrapMode		WrapT;
	TWrapMode		WrapR;
	
    TMinFilter		MinFilter;
	TMagFilter		MagFilter;

	W32		registration_sequence;		// 0 = free
	W16 width, height;
	W16 upload_width, upload_height;
	unsigned int texnum;
	W16 bytes;
	texturetype_t type;
	char	name[ MAX_GAMEPATH ];			// game path, including extension
} texture_t;



typedef enum 
{

	INTERPOLATION_NONE,   /* None (Fastest) */
	INTERPOLATION_LINEAR, /* Linear         */
	INTERPOLATION_CUBIC   /* Cubic (Best)   */

} InterpolationType;


extern W32 texture_registration_sequence;



void TM_Init( void );
void TM_Shutdown( void );


_boolean TM_MipMap( PW8 in, W16 *width, W16 *height, W16 bytes );

unsigned int TM_getWallTextureId( W32 imageId );
unsigned int TM_getSpriteTextureId( W32 imageId );

texture_t *TM_FindTexture_Wall( W32 imageId );
texture_t *TM_FindTexture_Sprite( W32 imageId );
texture_t *TM_FindTexture( const char *name, texturetype_t type );

void TM_GetTextureSize( SW32 *width, SW32 *height, const char *name );

void TM_ResampleTexture( PW8 in, int inwidth, int inheight, PW8 out,  int outwidth, int outheight, W16 bytes, InterpolationType interpolation );

void TM_FreeUnusedTextures( void );





#endif /* __TEXTURE_MANAGER_H__ */
