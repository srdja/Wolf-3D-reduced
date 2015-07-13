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

#include <stdio.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>

#include "w3d_opengl.h"
#include "opengl_binding.h"
#include "com_math.h"
#include "filesystem.h"
#include "cvar.h"
#include "renderer.h"

int glMaxTexSize; // maximum texture size

void GL_SetDefaultState (void);
void GL_UpdateSwapInterval (void);

extern  float   gldepthmin, gldepthmax;

extern  cvar_t  *gl_ext_palettedtexture;
extern  cvar_t  *gl_mode;
extern  cvar_t  *gl_round_down;
extern  cvar_t  *gl_picmip;
extern  cvar_t  *gl_finish;
extern  cvar_t  *gl_ztrick;
extern  cvar_t  *gl_clear;
extern  cvar_t  *gl_drawbuffer;
extern  cvar_t  *gl_driver;
extern  cvar_t  *gl_swapinterval;
extern  cvar_t  *r_fullscreen;
extern  cvar_t  *vid_gamma;
extern  cvar_t  *intensity;


void R_Bind (int texnum);


typedef struct {
    const char *renderer_string;
    const char *vendor_string;
    const char *version_string;
    const char *extensions_string;
    _boolean Version_1_2;
} glconfig_t;

typedef struct {
    float inverse_intensity;
    _boolean fullscreen;

    int     prev_mode;

    int currenttextures[4];
    int currenttmu;
} glstate_t;

extern glconfig_t  gl_config;
extern glstate_t   gl_state;


void MYgluPerspective (GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar);

void        GLimp_EndFrame (void);
_boolean    GLimp_Init (void *hinstance, void *hWnd);
void        GLimp_Shutdown (void);
int         GLimp_SetMode (int *pwidth, int *pheight, int mode, _boolean fullscreen);


#endif /* __OPENGL_LOCAL_H__ */
