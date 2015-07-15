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

#include "com_math.h"
#include "opengl_local.h"
#include "video.h"
#include "common.h"

float       gldepthmin, gldepthmax;

glconfig_t gl_config;
glstate_t  gl_state;

int  gl_mode;
int  gl_round_down;
int  gl_ztrick;
int  gl_clear;

/**
 * \brief Set up a perspective projection matrix
 * \param[in] fovy Specifies the field of view angle, in degrees, in the y direction.
 * \param[in] aspect Specifies the aspect ratio that determines the field of view in the x direction. The aspect ratio is the ratio of x (width) to y (height).
 * \param[in] zNear Specifies the distance from the viewer to the near clipping plane (always positive).
 * \param[in] zFar Specifies the distance from the viewer to the far clipping plane (always positive).
 */
PUBLIC void MYgluPerspective (GLdouble fovy, GLdouble aspect,
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
PRIVATE void R_Clear (void)
{
    if (gl_ztrick) {
        static int trickframe;

        if (gl_clear) {
            glClear (GL_COLOR_BUFFER_BIT);
        }
        trickframe++;

        if (trickframe & 1) {
            gldepthmin = 0;
            gldepthmax = 0.49999f;
            glDepthFunc (GL_LEQUAL);
        } else {
            gldepthmin = 1;
            gldepthmax = 0.5;
            glDepthFunc (GL_GEQUAL);
        }
    } else {
        if (gl_clear) {
            glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        } else {
            glClear (GL_DEPTH_BUFFER_BIT);
        }

        gldepthmin = 0;
        gldepthmax = 1;
        glDepthFunc (GL_LEQUAL);
    }
    glDepthRange (gldepthmin, gldepthmax);
}


/**
 * \brief Set 2D virtual screen mode
 */
PUBLIC void R_SetGL2D (void)
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
    glColor4f (1, 1, 1, 1);
}


/**
 * \brief Register GL data and methods
 */
PRIVATE void R_Register (void)
{
    gl_mode = 0;
    gl_round_down = 1;
    gl_ztrick = 0;
    gl_clear = 0;
}

/**
 * \brief Initialize OS specific parts of OpenGL.
 * \return true on success, otherwise false.
 */
PRIVATE _boolean R_SetMode (void)
{
    rserr_t err;
    _boolean fullscreen;

    fullscreen = (_boolean)0;

    if ((err = GLimp_SetMode (&viddef.width, &viddef.height, FloatToInt (gl_mode), fullscreen)) == rserr_ok) {
        gl_state.prev_mode = FloatToInt (gl_mode);
    } else {
        if (err == rserr_invalid_fullscreen) {

            if ((err = GLimp_SetMode (&viddef.width, &viddef.height, FloatToInt (gl_mode), false)) == rserr_ok) {
                return true;
            }
        } else if (err == rserr_invalid_mode) {
            gl_mode = gl_state.prev_mode;
        }

        // try setting it back to something safe
        if ((err = GLimp_SetMode (&viddef.width, &viddef.height, gl_state.prev_mode, false)) != rserr_ok) {
            return false;
        }
    }
    return true;
}


/**
 * \brief Initialize GL subsystem.
 * \param[in] hinstance A handle to a window instance.
 * \param[in] hWnd A handle to a window
 * \return 1 on success, otherwise -1.
 */
PUBLIC int R_Init (void *hinstance, void *hWnd)
{
    gl_mode = 1;

    char renderer_buffer[ 1000 ];
    char vendor_buffer[ 1000 ];
    int     a, b;

    R_Register();

    // set our "safe" modes
    gl_state.prev_mode = 0;

    // create the window and set up the context
    if (! R_SetMode()) {
       // OpenGL_Shutdown();
        return -1;
    }

    Video_MenuInit();

    //  get various GL strings
    gl_config.vendor_string = glGetString (GL_VENDOR);
    gl_config.renderer_string = glGetString (GL_RENDERER);
    gl_config.version_string = glGetString (GL_VERSION);
    gl_config.extensions_string = glGetString (GL_EXTENSIONS);

    strncpy(renderer_buffer, gl_config.renderer_string, sizeof(renderer_buffer));
    strncpy(vendor_buffer, gl_config.vendor_string, sizeof(vendor_buffer));

    sscanf (gl_config.version_string, "%d.%d", &a, &b);

    if (a >= 1 && b >= 2) {
        gl_config.Version_1_2 = true;
    }

    //FIXME: A lot of these aren't required

    glGetIntegerv (GL_MAX_TEXTURE_SIZE, &glMaxTexSize);

    GL_SetDefaultState();

    TM_Init();
    Font_Init();

    glGetError();
    return 1;
}

/**
 * \brief Shutdown GL subsystem
 */
PUBLIC void R_Shutdown (void)
{
    TM_Shutdown();
    GLimp_Shutdown();
}


/**
 * \brief Begin frame
 */
PUBLIC void R_BeginFrame (void)
{
    R_SetGL2D();
    glDrawBuffer (GL_BACK);
    R_Clear();
}

/**
 * \brief End frame
 */
PUBLIC void R_EndFrame (void)
{
    GLimp_EndFrame();
}