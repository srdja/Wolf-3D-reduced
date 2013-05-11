/*

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

#ifndef __OPENGL_LOCAL_H__
#define __OPENGL_LOCAL_H__

#ifdef _WIN32

	#define WIN32_LEAN_AND_MEAN 1
	#include <windows.h>
	
#endif

#include <stdio.h>


#include <GL/gl.h>
#include <GL/glu.h>

#include <GL/glext.h>

#include "w3d_opengl.h"
#include "opengl_binding.h"
#include "myopengl_extension.h"


#include "com_math.h"
#include "filesystem.h"
#include "cvar.h"

#include "renderer.h"


int glMaxTexSize; // maximum texture size 


//===================================================================


void GL_SetDefaultState( void );
void GL_UpdateSwapInterval( void );

extern	float	gldepthmin, gldepthmax;



//====================================================



extern	int			gl_filter_min, gl_filter_max;

//
// view origin
//
extern	vec3_t	vup;
extern	vec3_t	vpn;
extern	vec3_t	vright;
extern	vec3_t	r_origin;

//
// screen size info
//
//extern	refdef_t	r_newrefdef;

extern	cvar_t	*r_norefresh;
extern	cvar_t	*r_lefthand;
extern	cvar_t	*r_speeds;
extern	cvar_t	*r_novis;
extern	cvar_t	*r_nocull;


extern cvar_t	*gl_vertex_arrays;

extern cvar_t	*gl_ext_swapinterval;
extern cvar_t	*gl_ext_palettedtexture;
extern cvar_t	*gl_ext_multitexture;
extern cvar_t	*gl_ext_pointparameters;
extern cvar_t	*gl_ext_compiled_vertex_array;


extern	cvar_t	*gl_bitdepth;
extern	cvar_t	*gl_mode;
extern	cvar_t	*gl_round_down;
extern	cvar_t	*gl_picmip;
extern	cvar_t	*gl_finish;
extern	cvar_t	*gl_ztrick;
extern	cvar_t	*gl_clear;
extern	cvar_t	*gl_drawbuffer;
extern  cvar_t  *gl_driver;
extern	cvar_t	*gl_swapinterval;
extern	cvar_t	*gl_texturemode;

extern	cvar_t	*r_fullscreen;
extern	cvar_t	*vid_gamma;

extern	cvar_t		*intensity;

extern	int		gl_solid_format;
extern	int		gl_alpha_format;
extern	int		gl_tex_alpha_format;




void R_Bind( int texnum );
void R_MBind( GLenum target, int texnum );
void R_TexEnv( GLenum value );
void R_EnableMultitexture( _boolean enable );
void R_SelectTexture( GLenum );


//====================================================================


int		registration_sequence;



typedef struct
{
	const char *renderer_string;
	const char *vendor_string;
	const char *version_string;
	const char *extensions_string;
	_boolean Version_1_2;

} glconfig_t;

typedef struct
{
	float inverse_intensity;
	_boolean fullscreen;

	int     prev_mode;

	int	currenttextures[4];
	int currenttmu;

	
} glstate_t;

extern glconfig_t  gl_config;
extern glstate_t   gl_state;


//////////////////////////////////////////////////////


//////////////////////////////////////////////////////


typedef struct	
{
	_boolean	ARBMultiTexture;
	int		nTextureStages;	// Number of texture stages supported
	_boolean	EXTTextureEnvCombine;

	_boolean EXTTextureFilterAnisotropic;
	float		nMaxAnisotropy;
	// NB: Fence extension is not here, because NVVertexArrayRange is false if GL_NV_fence is not here.
	_boolean	NVVertexArrayRange;
	int		NVVertexArrayRangeMaxVertex;
	_boolean	EXTTextureCompressionS3TC;
	_boolean	EXTVertexWeighting;
	_boolean	EXTSeparateSpecularColor;
	_boolean	NVTextureEnvCombine4;
	_boolean	ARBTextureCubeMap;
	_boolean	NVVertexProgram;
	_boolean	EXTVertexShader;
	_boolean	NVTextureShader;
	// true if NVVertexProgram and if we know that VP is emulated
	_boolean	NVVertexProgramEmulated;
	_boolean	EXTSecondaryColor;
	_boolean	EXTBlendColor;
	// NVVertexArrayRange2. 
	_boolean	NVVertexArrayRange2;
	// equal to GL_VERTEX_ARRAY_RANGE_WITHOUT_FLUSH_NV if possible, or GL_VERTEX_ARRAY_RANGE_NV
	int	NVStateVARWithoutFlush;

	/* WGL ARB Extensions */
	_boolean	WGLARBPBuffer;
	_boolean	WGLARBPixelFormat;
	_boolean	WGLEXTSwapControl;

	/* ATI Extensions */
	_boolean	ATIVertexArrayObject;
	_boolean    ATIMapObjectBuffer;
	_boolean	ATITextureEnvCombine3;
	_boolean    ATIEnvMapBumpMap;
	_boolean    ATIFragmentShader;
	_boolean	ATIXTextureEnvRoute;
	
	/* ARB Extensions */
	_boolean	ARBTextureCompression;
	_boolean	ARBFragmentProgram;
	_boolean	ARBVertexBufferObject;
	_boolean	ARBVertexProgram;

	/* Disable Hardware feature */
	_boolean	DisableHardwareVertexProgram;	
	_boolean	DisableHardwareTextureShader;

} GL_Extensions;

extern GL_Extensions gl_ext;

void MYgluPerspective( GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar );
void PrintGLError( W32 err, const char *from );




/*
====================================================================

IMPLEMENTATION SPECIFIC FUNCTIONS

====================================================================
*/

void		GLimp_BeginFrame();
void		GLimp_EndFrame( void );
_boolean	GLimp_Init( void *hinstance, void *hWnd );
void		GLimp_Shutdown( void );
int     	GLimp_SetMode( int *pwidth, int *pheight, int mode, _boolean fullscreen );
void		GLimp_AppActivate( _boolean active );










#endif /* __OPENGL_LOCAL_H__ */
