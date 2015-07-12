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





//
// view origin
//
vec3_t  vup;
vec3_t  vpn;
vec3_t  vright;
vec3_t  r_origin;

cvar_t  *r_norefresh;
cvar_t  *r_speeds;
cvar_t  *r_novis;
cvar_t  *r_nocull;
cvar_t  *r_lefthand;




cvar_t  *gl_vertex_arrays;



cvar_t  *gl_ext_swapinterval;
cvar_t  *gl_ext_palettedtexture;
cvar_t  *gl_ext_multitexture;
cvar_t  *gl_ext_pointparameters;
cvar_t  *gl_ext_compiled_vertex_array;

//cvar_t    *gl_ext_TextureCompressionS3TC;

cvar_t  *gl_bitdepth;
cvar_t  *gl_drawbuffer;
cvar_t  *gl_driver;
cvar_t  *gl_mode;
cvar_t  *gl_round_down;
cvar_t  *gl_picmip;
cvar_t  *gl_ztrick;
cvar_t  *gl_finish;
cvar_t  *gl_clear;
cvar_t  *gl_swapinterval;
cvar_t  *gl_texturemode;

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

    pfglFrustum (xmin, xmax, ymin, ymax, zNear, zFar);
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
    com_strlcpy (picname, "scrn00.tga", sizeof (picname));

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
        Com_Printf ("R_ScreenShot_f: Couldn't create a file\n");
        return;
    }


    buffer = (PW8)MM_MALLOC (viddef.width * viddef.height * 3);


    pfglReadPixels (0, 0, viddef.width, viddef.height, GL_RGB, GL_UNSIGNED_BYTE, buffer);


    WriteTGA (checkname, 24, viddef.width, viddef.height, buffer, 1, 1);


    MM_FREE (buffer);
    Com_Printf ("Wrote %s\n", picname);
}

/**
 * \brief Console callback for displaying GL information
 */
PRIVATE void R_Strings_f (void)
{
    Com_Printf ("GL_VENDOR: %s\n", gl_config.vendor_string);
    Com_Printf ("GL_RENDERER: %s\n", gl_config.renderer_string);
    Com_Printf ("GL_VERSION: %s\n", gl_config.version_string);
    Com_Printf ("GL_EXTENSIONS: %s\n", gl_config.extensions_string);
}

/**
 * \brief clear buffers to preset values
 */
PRIVATE void R_Clear (void)
{
    if (gl_ztrick->value) {
        static int trickframe;

        if (gl_clear->value) {
            pfglClear (GL_COLOR_BUFFER_BIT);
        }

        trickframe++;

        if (trickframe & 1) {
            gldepthmin = 0;
            gldepthmax = 0.49999f;
            pfglDepthFunc (GL_LEQUAL);
        } else {
            gldepthmin = 1;
            gldepthmax = 0.5;
            pfglDepthFunc (GL_GEQUAL);
        }
    } else {
        if (gl_clear->value) {
            pfglClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        } else {
            pfglClear (GL_DEPTH_BUFFER_BIT);
        }

        gldepthmin = 0;
        gldepthmax = 1;
        pfglDepthFunc (GL_LEQUAL);
    }

    pfglDepthRange (gldepthmin, gldepthmax);

}


/**
 * \brief Set 2D virtual screen mode
 */
PUBLIC void R_SetGL2D (void)
{
    // set 2D virtual screen size
    pfglViewport (0, 0, viddef.width, viddef.height);
    pfglMatrixMode (GL_PROJECTION);
    pfglLoadIdentity();
    pfglOrtho (0, viddef.width, viddef.height, 0, -99999, 99999);
    pfglMatrixMode (GL_MODELVIEW);
    pfglLoadIdentity();
    pfglDisable (GL_DEPTH_TEST);
    pfglDisable (GL_CULL_FACE);
    pfglDisable (GL_BLEND);
    pfglEnable (GL_ALPHA_TEST);
    pfglColor4f (1, 1, 1, 1);

}


/**
 * \brief Register GL data and methods
 */
PRIVATE void R_Register (void)
{
    r_lefthand = Cvar_Get ("hand", "0", CVAR_USERINFO | CVAR_ARCHIVE);
    r_norefresh = Cvar_Get ("r_norefresh", "0", CVAR_INIT);
    r_novis = Cvar_Get ("r_novis", "0", CVAR_INIT);
    r_nocull = Cvar_Get ("r_nocull", "0", CVAR_INIT);
    r_speeds = Cvar_Get ("r_speeds", "0", CVAR_INIT);

    gl_bitdepth = Cvar_Get ("gl_bitdepth", "0", CVAR_INIT);
    gl_mode = Cvar_Get ("gl_mode", "0", CVAR_ARCHIVE);
    gl_round_down = Cvar_Get ("gl_round_down", "1", CVAR_INIT);
    gl_picmip = Cvar_Get ("gl_picmip", "0", CVAR_INIT);
    gl_ztrick = Cvar_Get ("gl_ztrick", "0", CVAR_INIT);
    gl_finish = Cvar_Get ("gl_finish", "1", CVAR_ARCHIVE);
    gl_clear = Cvar_Get ("gl_clear", "0", CVAR_INIT);
    gl_driver = Cvar_Get ("gl_driver", OPENGL_DLL_NAME, CVAR_ARCHIVE);
//  gl_texturemode = Cvar_Get( "gl_texturemode", "GL_LINEAR_MIPMAP_NEAREST", CVAR_ARCHIVE );

    gl_vertex_arrays = Cvar_Get ("gl_vertex_arrays", "0", CVAR_ARCHIVE);

    gl_ext_swapinterval = Cvar_Get ("gl_ext_swapinterval", "1", CVAR_ARCHIVE);
    gl_ext_palettedtexture = Cvar_Get ("gl_ext_palettedtexture", "1", CVAR_ARCHIVE);
    gl_ext_multitexture = Cvar_Get ("gl_ext_multitexture", "1", CVAR_ARCHIVE);
    gl_ext_pointparameters = Cvar_Get ("gl_ext_pointparameters", "1", CVAR_ARCHIVE);
    gl_ext_compiled_vertex_array = Cvar_Get ("gl_ext_compiled_vertex_array", "1", CVAR_ARCHIVE);

    gl_drawbuffer = Cvar_Get ("gl_drawbuffer", "GL_BACK", CVAR_INIT);
    gl_swapinterval = Cvar_Get ("gl_swapinterval", "1", CVAR_ARCHIVE);


    r_fullscreen = Cvar_Get ("r_fullscreen", "0", CVAR_ARCHIVE);
    vid_gamma = Cvar_Get ("vid_gamma", "1.0", CVAR_ARCHIVE);
    r_ref = Cvar_Get ("r_ref", "soft", CVAR_ARCHIVE);


    Cmd_AddCommand ("screenshot", R_ScreenShot_f);
    Cmd_AddCommand ("r_strings", R_Strings_f);

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
            Com_Printf ("[R_SetMode()] -fullscreen unavailable in this mode\n");

            if ((err = GLimp_SetMode (&viddef.width, &viddef.height, FloatToInt (gl_mode->value), false)) == rserr_ok) {
                return true;
            }
        } else if (err == rserr_invalid_mode) {
            Cvar_SetValue ("gl_mode", (float)gl_state.prev_mode);
            gl_mode->modified = false;
            Com_Printf ("ref_gl::R_SetMode() - invalid mode\n");
        }

        // try setting it back to something safe
        if ((err = GLimp_SetMode (&viddef.width, &viddef.height, gl_state.prev_mode, false)) != rserr_ok) {
            Com_Printf ("ref_gl::R_SetMode() - could not revert to safe mode\n");
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
    int     err;
    int     a, b;


    Com_Printf ("\n------ Display Initialization ------\n");

    Com_Printf ("Initializing OpenGL Subsystem\n");

    R_Register();

    // Initialize our OpenGL dynamic bindings
    if (! OpenGL_Init (gl_driver->string)) {
        OpenGL_Shutdown();
        Com_Printf ("Dynamic binding of (%s) failed\n", gl_driver->string);
        return -1;
    }

    // Initialize OS-specific parts of OpenGL
    if (! GLimp_Init (hinstance, hWnd)) {
        OpenGL_Shutdown();
        return -1;
    }

    // set our "safe" modes
    gl_state.prev_mode = 0;

    // create the window and set up the context
    if (! R_SetMode()) {
        OpenGL_Shutdown();
        Com_Printf ("R_Init() - could not R_SetMode()\n");
        return -1;
    }

    Video_MenuInit();

    //  get various GL strings
    gl_config.vendor_string = pfglGetString (GL_VENDOR);
    Com_Printf ("GL_VENDOR: %s\n", gl_config.vendor_string);

    gl_config.renderer_string = pfglGetString (GL_RENDERER);
    Com_Printf ("GL_RENDERER: %s\n", gl_config.renderer_string);

    gl_config.version_string = pfglGetString (GL_VERSION);
    Com_Printf ("GL_VERSION: %s\n", gl_config.version_string);

    gl_config.extensions_string = pfglGetString (GL_EXTENSIONS);
    Com_Printf ("GL_EXTENSIONS: %s\n", gl_config.extensions_string);

    com_strlcpy (renderer_buffer, gl_config.renderer_string, sizeof (renderer_buffer));
    (void)com_strlwr (renderer_buffer);

    com_strlcpy (vendor_buffer, gl_config.vendor_string, sizeof (vendor_buffer));
    (void)com_strlwr (vendor_buffer);


    sscanf (gl_config.version_string, "%d.%d", &a, &b);

    if (a >= 1 && b >= 2) {
        gl_config.Version_1_2 = true;
    }


#ifdef __unix__

    Cvar_SetValue ("gl_finish", 1);

#endif

    //FIXME: A lot of these aren't required

    Com_Printf ("Initializing OpenGL extensions\n");
    GL_ConfigExtensions (gl_config.extensions_string);

//  if( ! gl_ext.ARBMultiTexture )
//  {
//      Com_Printf( "Missing Required GL extension: GL_ARB_multitexture. Update your driver.\n" );
//      return -1;
//  }
//
//  if( ! GLEW_EXT_texture_env_combine )
//  {
//      Com_Printf( "Missing Important GL extension: GL_EXT_texture_env_combine => All envcombine are setup to GL_MODULATE!");
//  }
//
    if (gl_ext.nMaxAnisotropy) {
        Com_Printf ("GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT: %4.2f\n", GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT);
    }


    Com_Printf ("GL_MAX_TEXTURE_UNITS_ARB: %d\n", GL_MAX_TEXTURE_UNITS_ARB);


    pfglGetIntegerv (GL_MAX_TEXTURE_SIZE, &glMaxTexSize);
    Com_Printf ("GL_MAX_TEXTURE_SIZE: %d\n", glMaxTexSize);

    GL_SetDefaultState();

    TM_Init();
    Font_Init();

    err = pfglGetError();

    if (err != GL_NO_ERROR) {
        Com_Printf ("glGetError() = 0x%x\n", err);
    }

    return 1;
}

/**
 * \brief Shutdown GL subsystem
 */
PUBLIC void R_Shutdown (void)
{
//  Cmd_RemoveCommand ("modellist");
//  Cmd_RemoveCommand ("screenshot");
//  Cmd_RemoveCommand ("imagelist");
//  Cmd_RemoveCommand ("gl_strings");

//  Mod_FreeAll ();

    TM_Shutdown();

    /*
    ** shut down OS specific OpenGL stuff like contexts, etc.
    */
    GLimp_Shutdown();

    /*
    ** shutdown our OpenGL subsystem
    */
    OpenGL_Shutdown();

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


    GLimp_BeginFrame();

//
// Go into 2D mode.
//

    R_SetGL2D();

//
// Draw buffer stuff.
//
    if (gl_drawbuffer->modified) {
        gl_drawbuffer->modified = false;

        if (com_stricmp (gl_drawbuffer->string, "GL_FRONT") == 0) {
            pfglDrawBuffer (GL_FRONT);
        } else {
            pfglDrawBuffer (GL_BACK);
        }

    }

//
// texturemode stuff
//
//  if ( gl_texturemode->modified )
//  {
//      R_TextureMode( gl_texturemode->string );
//      gl_texturemode->modified = false;
//  }



//
// swapinterval stuff
//
    GL_UpdateSwapInterval();

//
// clear screen if desired
//
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
 * \brief Application active
 * \param[in] active Is the current application active?
 */
PUBLIC void R_AppActivate (_boolean active)
{
    GLimp_AppActivate (active);
}


/**
 * \brief Update swap interval
 */
PUBLIC void GL_UpdateSwapInterval (void)
{
    if (gl_swapinterval->modified) {
        gl_swapinterval->modified = false;

#ifdef _WIN32

        if (pfwglSwapIntervalEXT) {
            pfwglSwapIntervalEXT (FloatToInt (gl_swapinterval->value));
        }

#endif

    }
}

/**
 * \brief Print GL error message.
 * \param[in] err Error code
 * \param[in] from Function name that produced the error.
 */
PUBLIC void PrintGLError (W32 err, const char *from)
{
    if (err == GL_NO_ERROR) {
        return;
    }

    if (from != "") {
        Com_Printf ("\n\n\nGL Error: %s\n", from);
    }

    switch (err) {
    case GL_NO_ERROR:
        Com_Printf ("GL_NO_ERROR:\nNo error has been recorded. The value of this symbolic constant is guaranteed to be zero.\n");
        break;

    case GL_INVALID_ENUM:
        Com_Printf ("GL_INVALID_ENUM:\nAn unacceptable value is specified for an enumerated argument. The offending function is ignored, having no side effect other than to set the error flag.\n");
        break;

    case GL_INVALID_VALUE:
        Com_Printf ("GL_INVALID_VALUE:\nA numeric argument is out of range. The offending function is ignored, having no side effect other than to set the error flag.\n");
        break;

    case GL_INVALID_OPERATION:
        Com_Printf ("GL_INVALID_OPERATION:\nThe specified operation is not allowed in the current state. The offending function is ignored, having no side effect other than to set the error flag.\n");
        break;

    case GL_STACK_OVERFLOW:
        Com_Printf ("GL_STACK_OVERFLOW:\nThis function would cause a stack overflow. The offending function is ignored, having no side effect other than to set the error flag.\n");
        break;

    case GL_STACK_UNDERFLOW:
        Com_Printf ("GL_STACK_UNDERFLOW:\nThis function would cause a stack underflow. The offending function is ignored, having no side effect other than to set the error flag.\n");
        break;

    case GL_OUT_OF_MEMORY:
        Com_Printf ("GL_OUT_OF_MEMORY:\nThere is not enough memory left to execute the function. The state of OpenGL is undefined, except for the state of the error flags, after this error is recorded.\n");
        break;

    default:
        Com_Printf ("Unknown GL error flag 0x%x\n", err);
    }
}
