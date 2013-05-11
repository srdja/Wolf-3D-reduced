/*

	Copyright (C) 2004-2005 Michael Liebscher <johnnycanuck@users.sourceforge.net>
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
 * \file opengl_draw.c
 * \brief OpenGL drawing routines.
 * \author Michael Liebscher
 * \date 2004-2005
 * \author Id Software, Inc.
 * \date 1997-2001
 */


#include "opengl_local.h"

//#include "vid.h"

#include "texture_manager.h"
#include "font_manager.h"
#include "common.h"

texture_t       *draw_chars;


/**
 * \brief Draw ASCII character to the screen.
 * \param[in] x x-coordinate.
 * \param[in] y y-coordinate.
 * \param[in] num ASCII character value.
 * \param[in] myfont Valid pointer to font_t structure.
 * \return 
 * \note It can be clipped to the top of the screen to allow the console to be smoothly scrolled off.
 */
PUBLIC void R_Draw_Character( int x, int y, int num, font_t *myfont )
{
	int	row, col; 
	int scale, sh; // scaled width, height
	float frow, fcol;

	num &= 255;
	
	if( (num & 127) == 32 )
	{
		return;		// space
	}

	if( y <= -myfont->nMaxHeight )
	{
		return;			// totally off screen
	}

	scale = myfont->nSize;
	sh = myfont->nMaxHeight;

	row = (num >> 4) - 2;
	col = num & 15;

	frow = row * myfont->hFrac;
	fcol = col * myfont->wFrac;


	pfglColor4ubv( myfont->colour );

	pfglEnable( GL_BLEND );
	

	R_Bind( myfont->texfont->texnum );
	

	pfglBegin( GL_QUADS );

	
	pfglTexCoord2f( fcol, frow );
	pfglVertex2i( x, y );
	
	pfglTexCoord2f( fcol+myfont->wFrac, frow );
	pfglVertex2i( x+myfont->nMaxWidth*scale, y );
	
	pfglTexCoord2f( fcol+myfont->wFrac, frow+myfont->hFrac );	
	pfglVertex2i( x+myfont->nMaxWidth*scale, (y+sh*scale) );
	
	pfglTexCoord2f( fcol, frow+myfont->hFrac );
	pfglVertex2i( x, (y+sh*scale) );


	
	pfglEnd();

	pfglDisable( GL_BLEND );
	pfglColor3f( 1, 1, 1 );
}


/**
 * \brief Draw stretched image to the screen.
 * \param[in] x x-coordinate.
 * \param[in] y y-coordinate.
 * \param[in] w Width of region.
 * \param[in] h Height of region.
 * \return 
 * \note
 */
PUBLIC void R_Draw_StretchPic( int x, int y, int w, int h, const char *pic )
{
	texture_t *gl;

	gl = TM_FindTexture( pic, TT_Pic );
	if( ! gl )
	{
		Com_Printf( "Can't find pic: %s\n", pic );
		return;
	}

	R_Bind( gl->texnum );
	
	pfglBegin( GL_QUADS );
	
	pfglTexCoord2f( 0.0f, 0.0f );	pfglVertex2i( x, y );
	pfglTexCoord2f( 1.0f, 0.0f );	pfglVertex2i( x+w, y );
	pfglTexCoord2f( 1.0f, 1.0f );	pfglVertex2i( x+w, y+h );
	pfglTexCoord2f( 0.0f, 1.0f );	pfglVertex2i( x, y+h );
	
	pfglEnd();
}

/**
 * \brief Draw image to the screen. 
 * \param[in] x x-coordinate.
 * \param[in] y y-coordinate.
 * \param[in] pic Image filename to draw.
 * \return 
 * \note
 */
PUBLIC void R_Draw_Pic( int x, int y, const char *pic )
{
	texture_t *tex;

	tex = TM_FindTexture( pic, TT_Pic );
	if( ! tex )
	{
		Com_Printf( "Can't find pic: %s\n", pic );
		return;
	}
	
	R_Bind( tex->texnum );

	pfglBegin( GL_QUADS );

	pfglTexCoord2f( 0.0, 0.0 );		pfglVertex2i( x, y );
	pfglTexCoord2f( 1.0, 0.0 );		pfglVertex2i( x + tex->width, y );
	pfglTexCoord2f( 1.0, 1.0 );		pfglVertex2i( x + tex->width, y + tex->height );
	pfglTexCoord2f( 0.0, 1.0 );		pfglVertex2i( x, y + tex->height );
	
	pfglEnd();

}

/**
 * \brief Tile image on the screen. 
 * \param[in] x x-coordinate.
 * \param[in] y y-coordinate.
 * \param[in] w Width of region.
 * \param[in] h Height of region.
 * \param[in] pic File name of texture to draw.
 * \return 
 * \note This repeats a tile graphic to fill a region on the screen.
 */
PUBLIC void R_Draw_Tile( int x, int y, int w, int h, const char *pic )
{
	texture_t	*image;

	image = TM_FindTexture( pic, TT_Wall );
	if( ! image )
	{
		Com_Printf( "Can't find pic: %s\n", pic );
		return;
	}


	R_Bind( image->texnum );
	
	pfglBegin( GL_QUADS );

	
	pfglTexCoord2i( x / image->upload_width, y / image->upload_height);
	pfglVertex2i( x, y );

	pfglTexCoord2i( (x + w) / image->upload_width, y / image->upload_height);
	pfglVertex2i( x + w, y );

	pfglTexCoord2i( (x + w) / image->upload_width, (y + h) / image->upload_height);
	pfglVertex2i( x + w, y + h );

	pfglTexCoord2i( x / image->upload_width, (y + h) / image->upload_height );
	pfglVertex2i( x, y + h );


	pfglEnd ();

}

/**
 * \brief Fills a box of pixels with a single color.
 * \param[in] x x-coordinate.
 * \param[in] y y-coordinate.
 * \param[in] w Width of region.
 * \param[in] h Height of region.
 * \param[in] c Colour to fill region.
 * \return
 * \note
 */
PUBLIC void R_Draw_Fill( int x, int y, int w, int h, colour3_t c )
{
	pfglDisable( GL_TEXTURE_2D );

	pfglColor3ubv( c );


	pfglBegin( GL_QUADS );

	pfglVertex2i( x, y );
	pfglVertex2i( x+w, y );
	pfglVertex2i( x+w, y+h );
	pfglVertex2i( x, y+h );

	pfglEnd();


	pfglColor3f( 1, 1, 1 );
	pfglEnable( GL_TEXTURE_2D );
	
}

/**
 * \brief Draw a line on the screen.
 * \param[in] nXStart x-coordinate of starting point. 
 * \param[in] nYStart y-coordinate of starting point. 
 * \param[in] nXEnd x-coordinate of ending point.
 * \param[in] nYEnd y-coordinate of ending point.
 * \param[in] c Colour value.
 * \return
 * \note
 */
PUBLIC void R_Draw_Line( int nXStart, int nYStart, int nXEnd, int nYEnd, int width, colour3_t c )
{
	pfglDisable( GL_TEXTURE_2D );


	pfglColor3ubv( c );


	pfglLineWidth( (float)width );

	pfglBegin( GL_LINES );
		
	pfglVertex2i( nXStart, nYStart );
	pfglVertex2i( nXEnd, nYEnd );

	pfglEnd();

	pfglColor3f( 1, 1, 1 );
	pfglEnable( GL_TEXTURE_2D );
}


