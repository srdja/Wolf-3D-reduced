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
 *  unix_mouse.c: unix mouse input layer.
 *
 *  Author: Michael Liebscher <johnnycanuck@users.sourceforge.net>
 *  Date:   2004
 *
 *  Acknowledgement:
 *  This code was derived from Quake II, and was originally
 *  written by Id Software, Inc.
 *
 */

#include <X11/Xlib.h>

#include <X11/extensions/Xxf86dga.h>

#include "../env/common_utils.h"
#include "../env/video.h"
#include "../env/client.h"

extern Display *display;
extern Window mainwin;

extern _boolean vidmode_active;

PRIVATE _boolean        mouse_avail;
int mx, my;
PRIVATE int old_mouse_x, old_mouse_y;

PRIVATE cvar_t  *m_filter;
PRIVATE cvar_t  *in_mouse;
PRIVATE int in_dgamouse;

_boolean mouse_active = false;
_boolean dgamouse = false;


PRIVATE Cursor CreateNullCursor (Display *display, Window root)
{
    Pixmap cursormask;
    XGCValues xgc;
    GC gc;
    XColor dummycolour;
    Cursor cursor;

    cursormask = XCreatePixmap (display, root, 1, 1, 1);
    xgc.function = GXclear;
    gc = XCreateGC (display, cursormask, GCFunction, &xgc);
    XFillRectangle (display, cursormask, gc, 0, 0, 1, 1);
    dummycolour.pixel = 0;
    dummycolour.red = 0;
    dummycolour.flags = 04;
    cursor = XCreatePixmapCursor (display, cursormask, cursormask,
                                  &dummycolour, &dummycolour, 0, 0);
    XFreePixmap (display, cursormask);
    XFreeGC (display, gc);

    return cursor;
}


PUBLIC void install_grabs (void)
{

// inviso cursor
    XDefineCursor (display, mainwin, CreateNullCursor (display, mainwin));

    XGrabPointer (display, mainwin,
                  True,
                  0,
                  GrabModeAsync, GrabModeAsync,
                  mainwin,
                  None,
                  CurrentTime);

    if (in_dgamouse) {
        int MajorVersion, MinorVersion;

        if (! XF86DGAQueryVersion (display, &MajorVersion, &MinorVersion)) {
            // unable to query, probalby not supported
            dgamouse = 0;
        } else {
            dgamouse = true;
            XF86DGADirectVideo (display, DefaultScreen (display), XF86DGADirectMouse);
            XWarpPointer (display, None, mainwin, 0, 0, 0, 0, 0, 0);
        }
    } else {
        XWarpPointer (display, None, mainwin,
                      0, 0, 0, 0,
                      viddef.width / 2, viddef.height / 2);
    }

    XGrabKeyboard (display, mainwin,
                   False,
                   GrabModeAsync, GrabModeAsync,
                   CurrentTime);

    mouse_active = true;
}


PUBLIC void uninstall_grabs (void)
{
    if (! display || ! mainwin) {
        return;
    }

    if (dgamouse) {
        dgamouse = false;
        XF86DGADirectVideo (display, DefaultScreen (display), 0);
    }

    XUngrabPointer (display, CurrentTime);
    XUngrabKeyboard (display, CurrentTime);

// inviso cursor
    XUndefineCursor (display, mainwin);

    mouse_active = false;
}

PUBLIC void IN_StartupMouse (void)
{
    // mouse variables
    m_filter = Cvar_Get ("m_filter", "0", 0);
    in_mouse = Cvar_Get ("in_mouse", "1", CVAR_ARCHIVE);
    in_dgamouse = 1;
    m_forward = 1;

    //Cmd_AddCommand ("+mlook", RW_IN_MLookDown);
    //Cmd_AddCommand ("-mlook", RW_IN_MLookUp);

    //Cmd_AddCommand ("force_centerview", Force_CenterView_f);

    mx = my = 0.0;
    mouse_avail = true;

    install_grabs();
}

PUBLIC void IN_MouseMove (usercmd_t *cmd)
{
    if (! mouse_avail) {
        return;
    }

    if (m_filter->value) {
        mx = (mx + old_mouse_x) * 0.5;
        my = (my + old_mouse_y) * 0.5;
    }

    old_mouse_x = mx;
    old_mouse_y = my;

    mx *= sensitivity;
    my *= sensitivity;

// add mouse X/Y movement to cmd

    ClientState.viewangles[ YAW ] -= m_yaw * mx;

    cmd->forwardmove -= m_forward * my;

    mx = my = 0;
}

PUBLIC void IN_DeactivateMouse (void)
{
    if (! mouse_avail || ! display || ! mainwin) {
        return;
    }

    if (mouse_active) {
        uninstall_grabs();
        mouse_active = false;
    }
}

PUBLIC void IN_ActivateMouse (void) {
    if (!mouse_avail || !display || !mainwin) {
        return;
    }

    if (!mouse_active) {
        mx = my = 0; // don't spazz
        install_grabs();
        mouse_active = true;
    }
}
