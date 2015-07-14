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

#include "num_type.h"
#include "opengl_local.h"
#include "video.h"
#include "com_string.h"
#include "tga.h"
#include "common.h"


float       gldepthmin, gldepthmax;

glconfig_t gl_config;
glstate_t  gl_state;

cvar_t  *gl_ext_palettedtexture;

cvar_t  *gl_drawbuffer;
cvar_t  *gl_driver;
cvar_t  *gl_mode;
cvar_t  *gl_round_down;
cvar_t  *gl_picmip;
cvar_t  *gl_ztrick;
cvar_t  *gl_finish;
cvar_t  *gl_clear;
cvar_t  *gl_swapinterval;

cvar_t  *r_fullscreen;
cvar_t  *vid_gamma;
cvar_t  *r_ref;


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
 * \brief Console callback for taking a screen shot
 */
PRIVATE void R_ScreenShot_f (void)
{
    W8      *buffer;
    char    picname[ 80 ];
    char    checkname[ MAX_OSPATH ];
    int     i;
    FILE    *f;

    // create the scrnshots directory if it doesn't exist
    com_snprintf (checkname, sizeof (checkname), "%s/scrnshot", FS_Gamedir());
    FS_CreateDirectory (checkname);

//
// find a file name to save it to
//
    strncpy(picname, "scrn00.tga", sizeof(picname));

    for (i = 0 ; i <= 99 ; ++i) {
        picname[ 4 ] = i / 10 + '0';
        picname[ 5 ] = i % 10 + '0';
        com_snprintf (checkname, sizeof (checkname), "%s%cscrnshot%c%s", FS_Gamedir(), PATH_SEP, PATH_SEP, picname);
        f = fopen (checkname, "rb");

        if (! f) {
            break;  // file doesn't exist
        }

        fclose (f);
    }

    if (i == 100) {
        return;
    }

    buffer = (PW8)malloc (viddef.width * viddef.height * 3);
    glReadPixels (0, 0, viddef.width, viddef.height, GL_RGB, GL_UNSIGNED_BYTE, buffer);

    WriteTGA (checkname, 24, viddef.width, viddef.height, buffer, 1, 1);

    free (buffer);
}

/**
 * \brief Console callback for displaying GL information
 */
PRIVATE void R_Strings_f (void)
{

}

/**
 * \brief clear buffers to preset values
 */
PRIVATE void R_Clear (void)
{
    if (gl_ztrick->value) {
        static int trickframe;

        if (gl_clear->value) {
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
        if (gl_clear->value) {
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
    gl_mode = Cvar_Get ("gl_mode", "0", CVAR_ARCHIVE);
    gl_round_down = Cvar_Get ("gl_round_down", "1", CVAR_INIT);
    gl_picmip = Cvar_Get ("gl_picmip", "0", CVAR_INIT);
    gl_ztrick = Cvar_Get ("gl_ztrick", "0", CVAR_INIT);
    gl_finish = Cvar_Get ("gl_finish", "1", CVAR_ARCHIVE);
    gl_clear = Cvar_Get ("gl_clear", "0", CVAR_INIT);
    gl_driver = Cvar_Get ("gl_driver", OPENGL_DLL_NAME, CVAR_ARCHIVE);

    gl_ext_palettedtexture = Cvar_Get ("gl_ext_palettedtexture", "1", CVAR_ARCHIVE);

    gl_drawbuffer = Cvar_Get ("gl_drawbuffer", "GL_BACK", CVAR_INIT);
    gl_swapinterval = Cvar_Get ("gl_swapinterval", "1", CVAR_ARCHIVE);

    r_fullscreen = Cvar_Get ("r_fullscreen", "0", CVAR_ARCHIVE);
    vid_gamma = Cvar_Get ("vid_gamma", "1.0", CVAR_ARCHIVE);
    r_ref = Cvar_Get ("r_ref", "soft", CVAR_ARCHIVE);

    //Cmd_AddCommand ("screenshot", R_ScreenShot_f);
    //Cmd_AddCommand ("r_strings", R_Strings_f);
}

/**
 * \brief Initialize OS specific parts of OpenGL.
 * \return true on success, otherwise false.
 */
PRIVATE _boolean R_SetMode (void)
{
    rserr_t err;
    _boolean fullscreen;

    fullscreen = (_boolean)r_fullscreen->value;

    r_fullscreen->modified = false;
    gl_mode->modified = false;

    if ((err = GLimp_SetMode (&viddef.width, &viddef.height, FloatToInt (gl_mode->value), fullscreen)) == rserr_ok) {
        gl_state.prev_mode = FloatToInt (gl_mode->value);
    } else {
        if (err == rserr_invalid_fullscreen) {
            Cvar_SetValue ("r_fullscreen", 0);
            r_fullscreen->modified = false;

            if ((err = GLimp_SetMode (&viddef.width, &viddef.height, FloatToInt (gl_mode->value), false)) == rserr_ok) {
                return true;
            }
        } else if (err == rserr_invalid_mode) {
            Cvar_SetValue ("gl_mode", (float)gl_state.prev_mode);
            gl_mode->modified = false;
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
    char renderer_buffer[ 1000 ];
    char vendor_buffer[ 1000 ];
    int     a, b;

    R_Register();

    // Initialize OS-specific parts of OpenGL
    if (! GLimp_Init (hinstance, hWnd)) {
       // OpenGL_Shutdown();
        return -1;
    }
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


    //com_strlcpy (vendor_buffer, gl_config.vendor_string, sizeof (vendor_buffer));
    //(void)com_strlwr (vendor_buffer);

    sscanf (gl_config.version_string, "%d.%d", &a, &b);

    if (a >= 1 && b >= 2) {
        gl_config.Version_1_2 = true;
    }

    Cvar_SetValue ("gl_finish", 1);

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
//
// Change modes if necessary.
//
    if (gl_mode->modified || r_fullscreen->modified) {
        // FIXME: only restart if CDS is required
        cvar_t  *ref;

        ref = Cvar_Get ("r_ref", "gl", CVAR_INIT);
        ref->modified = true;
    }
    R_SetGL2D();

//
// Draw buffer stuff.
//
    if (gl_drawbuffer->modified) {
        gl_drawbuffer->modified = false;

        if (com_stricmp (gl_drawbuffer->string, "GL_FRONT") == 0) {
            glDrawBuffer (GL_FRONT);
        } else {
            glDrawBuffer (GL_BACK);
        }

    }
    GL_UpdateSwapInterval();

    R_Clear();
}

/**
 * \brief End frame
 */
PUBLIC void R_EndFrame (void)
{
    GLimp_EndFrame();
}

/**
 * \brief Update swap interval
 */
PUBLIC void GL_UpdateSwapInterval (void)
{
    if (gl_swapinterval->modified)
        gl_swapinterval->modified = false;
}