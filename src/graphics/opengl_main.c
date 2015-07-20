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

/**
 * \file opengl_main.c
 * \brief Angle math routines.
 * \author Id Software, Inc.
 * \date 1997-2001
 */

#include <math.h>

#include "opengl_local.h"
#include "video.h"

float       gldepthmin, gldepthmax;

glstate_t  gl_state;


/**
 * \brief Set up a perspective projection matrix
 * \param[in] fovy Specifies the field of view angle, in degrees, in the y direction.
 * \param[in] aspect Specifies the aspect ratio that determines the field of view in the x direction. The aspect ratio is the ratio of x (width) to y (height).
 * \param[in] zNear Specifies the distance from the viewer to the near clipping plane (always positive).
 * \param[in] zFar Specifies the distance from the viewer to the far clipping plane (always positive).
 */
void MYgluPerspective (GLdouble fovy,  GLdouble aspect,
                       GLdouble zNear, GLdouble zFar)
{
    GLdouble xmin, xmax, ymin, ymax;

    ymax = zNear * tan (fovy * M_PI / 360.0);
    ymin = -ymax;

    xmin = ymin * aspect;
    xmax = ymax * aspect;

    xmin += - (2 * 0) / zNear;
    xmax += - (2 * 0) / zNear;

    glFrustum (xmin, xmax, ymin, ymax, zNear, zFar);
}


/**
 * \brief clear buffers to preset values
 */
static void R_Clear (void)
{
    glClear (GL_DEPTH_BUFFER_BIT);
    gldepthmin = 0;
    gldepthmax = 1;
    glDepthFunc (GL_LEQUAL);
    glDepthRange (gldepthmin, gldepthmax);
}


/**
 * \brief Set 2D virtual screen mode
 */
void R_SetGL2D (void)
{
    // set 2D virtual screen size
    glViewport (0, 0, viddef.width, viddef.height);
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity();
    glOrtho (0, viddef.width, viddef.height, 0, -99999, 99999);
    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity();
    glDisable (GL_DEPTH_TEST);
    glDisable (GL_CULL_FACE);
    glDisable (GL_BLEND);
    glEnable (GL_ALPHA_TEST);
    glColor4f(1, 1, 1, 1);
}


/**
 * \brief Initialize GL subsystem.
 * \param[in] hinstance A handle to a window instance.
 * \param[in] hWnd A handle to a window
 * \return 1 on success, otherwise -1.
 */
int opengl_init()
{
    Video_MenuInit();

    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &glMaxTexSize);

    GL_SetDefaultState();

    TM_Init();
    Font_Init();

    glGetError();
    return 1;
}


/**
 * \brief Begin frame
 */
void R_BeginFrame (void)
{
    R_SetGL2D();
    glDrawBuffer(GL_BACK);
    R_Clear();
}

/**
 * \brief End frame
 */
void R_EndFrame (void)
{
    window_buffer_swap();
}