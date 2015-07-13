/*

    Copyright (C) 2004 Michael Liebscher
    Copyright (C) 1997-2001 Id Software, Inc.

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

    See the GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

/*
 *  openglw_imp.c: unix OpenGL implementation.
 *
 *  Author: Michael Liebscher <johnnycanuck@users.sourceforge.net>
 *  Date:   2004
 *
 *  Acknowledgement:
 *  This code was derived from Quake II, and was originally
 *  written by Id Software, Inc.
 *
 */

/*

Notes:

This file contains ALL X11 specific stuff having to do with the
OpenGL refresh. When a port is being made the following functions
must be implemented by the port:

GLimp_EndFrame
GLimp_Init
GLimp_Shutdown
GLimp_SwitchFullscreen

*/

#include <signal.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>


#include <GL/glx.h>

#include <X11/extensions/xf86vmode.h>

#include "../env/opengl_local.h"

#include "../env/video.h"
#include "../env/com_string.h"
#include "../env/common.h"


extern void uninstall_grabs (void);
extern void install_grabs (void);

Display *display = NULL;
PRIVATE int screen_num;
Window mainwin;
PRIVATE GLXContext ctx = NULL;


PRIVATE XF86VidModeModeInfo **vidmodes;

PRIVATE _boolean vidmode_ext = false;

PRIVATE int num_vidmodes;
_boolean vidmode_active = false;

PRIVATE cvar_t  *r_fakeFullscreen;


extern _boolean mouse_active;
extern _boolean dgamouse;



#define KEY_MASK (KeyPressMask | KeyReleaseMask)
#define MOUSE_MASK (ButtonPressMask | ButtonReleaseMask | PointerMotionMask | ButtonMotionMask )
#define X_MASK (KEY_MASK | MOUSE_MASK | VisibilityChangeMask | StructureNotifyMask | ExposureMask | EnterWindowMask | LeaveWindowMask )


PRIVATE char *signal_ErrorString (int sig)
{
    switch (sig) {
    case SIGHUP:
        return "Hangup (POSIX)";

    case SIGINT:
        return "Interrupt (ANSI)";

    case SIGQUIT:
        return "Quit (POSIX)";

    case SIGILL:
        return "Illegal instruction (ANSI)";

    case SIGTRAP:
        return "Trace trap (POSIX)";

    case SIGIOT:
        return "Abort (ANSI) | IOT trap (4.2 BSD)";

    case SIGBUS:
        return "BUS error (4.2 BSD)";

    case SIGFPE:
        return "Floating-point exception (ANSI)";

    case SIGKILL:
        return "Kill, unblockable (POSIX)";

    case SIGUSR1:
        return "User-defined signal 1 (POSIX)";

    case SIGSEGV:
        return "Segmentation violation (ANSI)";

    case SIGUSR2:
        return "User-defined signal 2 (POSIX)";

    case SIGPIPE:
        return "Broken pipe (POSIX)";

    case SIGALRM:
        return "Alarm clock (POSIX)";

    case SIGTERM:
        return "Termination (ANSI)";

    default:
        return "Unknown SIG";
    }

    return "";

}


PRIVATE void signal_handler (int sig)
{
    printf ("Received signal (%s), exiting...\n", signal_ErrorString (sig));
    GLimp_Shutdown();
    _exit (0);
}


PRIVATE void InitSig (void)
{
    signal (SIGHUP, signal_handler);
    signal (SIGQUIT, signal_handler);
    signal (SIGILL, signal_handler);
    signal (SIGTRAP, signal_handler);
    signal (SIGIOT, signal_handler);
    signal (SIGBUS, signal_handler);
    signal (SIGFPE, signal_handler);
    signal (SIGSEGV, signal_handler);
    signal (SIGTERM, signal_handler);
}



int GLimp_SetMode (int *pwidth, int *pheight, int mode, _boolean fullscreen)
{
    int width, height;

    int attrib[] = {
        GLX_RGBA,
        GLX_RED_SIZE, 1,
        GLX_GREEN_SIZE, 1,
        GLX_BLUE_SIZE, 1,
        GLX_DOUBLEBUFFER,
        GLX_DEPTH_SIZE, 1,
        None
    };

    Window root;
    XVisualInfo *visinfo;
    XSetWindowAttributes attr;
    W32 attribmask;
    int MajorVersion, MinorVersion;
    int i;
    XEvent report;
    XSizeHints size_hints;
    char display_name[ 512 ];

    r_fakeFullscreen = Cvar_Get ("r_fakeFullscreen", "0", CVAR_ARCHIVE);

    if (! VID_GetModeInfo (&width, &height, mode)) {
        return rserr_invalid_mode;
    }
    // destroy the existing window
    GLimp_Shutdown();

    // get the default display name from the environment variable DISPLAY
    com_strlcpy (display_name, getenv ("DISPLAY"), sizeof (display_name));

    if (display_name[ 0 ] == 0) {
        com_strlcpy (display_name, "unix:0.0", sizeof (display_name));
    }

    if (! (display = XOpenDisplay (display_name))) {
        fprintf (stderr, "Error could not open the X display\n");

        return rserr_invalid_mode;
    }

    screen_num = DefaultScreen (display);
    root = RootWindow (display, screen_num);

    // Get video mode list
    MajorVersion = MinorVersion = 0;

    if (! XF86VidModeQueryVersion (display, &MajorVersion, &MinorVersion)) {
        vidmode_ext = false;
    } else {
        vidmode_ext = true;
    }

    visinfo = pfglXChooseVisual (display, screen_num, attrib);

    if (! visinfo) {
        fprintf (stderr, "Error could not get an RGB, Double-buffered, Depth visual\n");
        return rserr_invalid_mode;
    }

    if (vidmode_ext) {
        int best_fit, best_dist, dist, x, y;

        XF86VidModeGetAllModeLines (display, screen_num, &num_vidmodes, &vidmodes);

        // Are we going fullscreen?  If so, let's change video mode
        if (fullscreen && ! r_fakeFullscreen->value) {
            best_dist = 9999999;
            best_fit = -1;

            for (i = 0 ; i < num_vidmodes ; i++) {
                if (width > vidmodes[ i ]->hdisplay ||
                        height > vidmodes[ i ]->vdisplay) {
                    continue;
                }

                x = width - vidmodes[ i ]->hdisplay;
                y = height - vidmodes[ i ]->vdisplay;
                dist = (x * x) + (y * y);

                if (dist < best_dist) {
                    best_dist = dist;
                    best_fit = i;
                }
            }

            if (best_fit != -1) {

                // change to the mode
                XF86VidModeSwitchToMode (display, screen_num, vidmodes[ best_fit ]);
                vidmode_active = true;

                // Move the viewport to top left
                XF86VidModeSetViewPort (display, screen_num, 0, 0);
            } else {
            }
        }
    }

    /* window attributes */
    attr.background_pixel = 0;
    attr.border_pixel = 0;
    attr.colormap = XCreateColormap (display, root, visinfo->visual, AllocNone);

    if (vidmode_active) {
        attribmask = CWBackPixel | CWColormap | CWSaveUnder | CWBackingStore | CWOverrideRedirect;
        attr.override_redirect = True;
        attr.backing_store = NotUseful;
        attr.save_under = False;
    } else {
        attribmask = CWBackPixel | CWBorderPixel | CWColormap;
    }

    mainwin = XCreateWindow (display, root, 0, 0, width, height,
                             0, visinfo->depth, InputOutput,
                             visinfo->visual, attribmask, &attr);



    size_hints.flags = PSize | PMinSize | PMaxSize;
    size_hints.min_width = width;
    size_hints.max_width = width;
    size_hints.min_height = height;
    size_hints.max_height = height;

    XSetStandardProperties (display, mainwin, GAME_NAME, NULL, None, 0, 0, &size_hints);

    XSelectInput (display, mainwin, X_MASK);

    XMapWindow (display, mainwin);

    do {
        XNextEvent (display, &report);

    } while (report.type != Expose);      // wait for our window to pop up

    if (vidmode_active) {
        XMoveWindow (display, mainwin, 0, 0);
        XRaiseWindow (display, mainwin);
        XWarpPointer (display, None, mainwin, 0, 0, 0, 0, 0, 0);
        XFlush (display);
        // Move the viewport to top left
        XF86VidModeSetViewPort (display, screen_num, 0, 0);
    }

    XFlush (display);

    ctx = pfglXCreateContext (display, visinfo, NULL, True);

    pfglXMakeCurrent (display, mainwin, ctx);

    *pwidth = width;
    *pheight = height;

    // let the sound and input subsystems know about the new window
    VID_NewWindow (width, height);

    pfglXMakeCurrent (display, mainwin, ctx);

    return rserr_ok;
}

void GLimp_Shutdown (void)
{
    uninstall_grabs();
    mouse_active = false;
    dgamouse = false;

    if (display) {
        if (ctx) {
            pfglXDestroyContext (display, ctx);
        }

        if (mainwin) {
            XDestroyWindow (display, mainwin);
        }

        if (vidmode_active) {
            XF86VidModeSwitchToMode (display, screen_num, vidmodes[ 0 ]);
        }

        XCloseDisplay (display);
    }

    ctx = NULL;
    display = NULL;
    mainwin = 0;
}

_boolean GLimp_Init (void *hinstance, void *wndproc)
{
    InitSig();

    return true;
}

void GLimp_BeginFrame (void)
{
    ;
}

void GLimp_EndFrame (void)
{
    pfglFlush();
    pfglXSwapBuffers (display, mainwin);
}
