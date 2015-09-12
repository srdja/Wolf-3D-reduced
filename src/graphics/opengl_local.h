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

#include "../util/com_math.h"
#include "../util/filesystem.h"
#include "renderer.h"

int glMaxTexSize; // maximum texture size

void GL_SetDefaultState (void);

extern  float   gldepthmin, gldepthmax;

extern  int intensity;

void R_Bind (int texnum);

typedef struct {
    float inverse_intensity;
    bool fullscreen;

    int     prev_mode;

    int currenttextures[4];
    int currenttmu;
} glstate_t;

extern glstate_t   gl_state;

void MYgluPerspective (GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar);

void window_buffer_swap(void);

#endif /* __OPENGL_LOCAL_H__ */