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
 *  unix_keys.c: unix key input layer.
 *
 *  Author: Michael Liebscher <johnnycanuck@users.sourceforge.net>
 *
 *  Acknowledgement:
 *  This code was derived from Quake II, and was originally
 *  written by Id Software, Inc.
 *
 */

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "../env/timer.h"
#include "../env/video.h"
#include "../env/com_string.h"
#include "../env/keys.h"
#include "../env/keycodes.h"

extern Display *display;
extern Window mainwin;

extern _boolean mouse_active;
extern _boolean dgamouse;
extern int mx, my;
int win_x, win_y;


void Do_Key_Event (int key, _boolean down)
{
    Key_Event (key, down, Sys_Milliseconds());
}


static int XLateKey (XKeyEvent *ev)
{

    int key;
    char buf[ 64 ];
    KeySym keysym;

    XLookupString (ev, buf, sizeof buf, &keysym, 0);

    switch (keysym) {
    case XK_KP_Page_Up:
        key = K_KP_PGUP;
        break;

    case XK_Page_Up:
        key = K_PGUP;
        break;

    case XK_KP_Page_Down:
        key = K_KP_PGDN;
        break;

    case XK_Page_Down:
        key = K_PGDN;
        break;

    case XK_KP_Home:
        key = K_KP_HOME;
        break;

    case XK_Home:
        key = K_HOME;
        break;

    case XK_KP_End:
        key = K_KP_END;
        break;

    case XK_End:
        key = K_END;
        break;

    case XK_KP_Left:
        key = K_KP_LEFTARROW;
        break;

    case XK_Left:
        key = K_LEFTARROW;
        break;

    case XK_KP_Right:
        key = K_KP_RIGHTARROW;
        break;

    case XK_Right:
        key = K_RIGHTARROW;
        break;

    case XK_KP_Down:
        key = K_KP_DOWNARROW;
        break;

    case XK_Down:
        key = K_DOWNARROW;
        break;

    case XK_KP_Up:
        key = K_KP_UPARROW;
        break;

    case XK_Up:
        key = K_UPARROW;
        break;

    case XK_Escape:
        key = K_ESCAPE;
        break;

    case XK_KP_Enter:
        key = K_KP_ENTER;
        break;

    case XK_Return:
        key = K_ENTER;
        break;

    case XK_Tab:
        key = K_TAB;
        break;

    case XK_F1:
        key = K_F1;
        break;

    case XK_F2:
        key = K_F2;
        break;

    case XK_F3:
        key = K_F3;
        break;

    case XK_F4:
        key = K_F4;
        break;

    case XK_F5:
        key = K_F5;
        break;

    case XK_F6:
        key = K_F6;
        break;

    case XK_F7:
        key = K_F7;
        break;

    case XK_F8:
        key = K_F8;
        break;

    case XK_F9:
        key = K_F9;
        break;

    case XK_F10:
        key = K_F10;
        break;

    case XK_F11:
        key = K_F11;
        break;

    case XK_F12:
        key = K_F12;
        break;

    case XK_BackSpace:
        key = K_BACKSPACE;
        break;

    case XK_KP_Delete:
        key = K_KP_DEL;
        break;

    case XK_Delete:
        key = K_DEL;
        break;

    case XK_Pause:
        key = K_PAUSE;
        break;

    case XK_Shift_L:
    case XK_Shift_R:
        key = K_SHIFT;
        break;

    case XK_Execute:
    case XK_Control_L:
    case XK_Control_R:
        key = K_CTRL;
        break;

    case XK_Alt_L:
    case XK_Meta_L:
    case XK_Alt_R:
    case XK_Meta_R:
        key = K_ALT;
        break;

    case XK_KP_Begin:
        key = K_KP_5;
        break;

    case XK_Insert:
        key = K_INS;
        break;

    case XK_KP_Insert:
        key = K_KP_INS;
        break;

    case XK_KP_Multiply:
        key = '*';
        break;

    case XK_KP_Add:
        key = K_KP_PLUS;
        break;

    case XK_KP_Subtract:
        key = K_KP_MINUS;
        break;

    case XK_KP_Divide:
        key = K_KP_SLASH;
        break;

#if 0

    case 0x021:
        key = '1';
        break;/* [!] */

    case 0x040:
        key = '2';
        break;/* [@] */

    case 0x023:
        key = '3';
        break;/* [#] */

    case 0x024:
        key = '4';
        break;/* [$] */

    case 0x025:
        key = '5';
        break;/* [%] */

    case 0x05e:
        key = '6';
        break;/* [^] */

    case 0x026:
        key = '7';
        break;/* [&] */

    case 0x02a:
        key = '8';
        break;/* [*] */

    case 0x028:
        key = '9';;
        break;/* [(] */

    case 0x029:
        key = '0';
        break;/* [)] */

    case 0x05f:
        key = '-';
        break;/* [_] */

    case 0x02b:
        key = '=';
        break;/* [+] */

    case 0x07c:
        key = '\'';
        break;/* [|] */

    case 0x07d:
        key = '[';
        break;/* [}] */

    case 0x07b:
        key = ']';
        break;/* [{] */

    case 0x022:
        key = '\'';
        break;/* ["] */

    case 0x03a:
        key = ';';
        break;/* [:] */

    case 0x03f:
        key = '/';
        break;/* [?] */

    case 0x03e:
        key = '.';
        break;/* [>] */

    case 0x03c:
        key = ',';
        break;/* [<] */
#endif

    default:
        key = * (PW8)buf;
        key = TOLOWER (key);
        break;
    }

    return key;
}


static void HandleEvents (void)
{
    XEvent event;
    int b;
    _boolean dowarp = false;
    int mwx = viddef.width >> 1;
    int mwy = viddef.height >> 1;

    if (! display) {
        return;
    }

    while (XPending (display)) {
        XNextEvent (display, &event);

        switch (event.type) {
        case KeyPress:
        case KeyRelease:
            Do_Key_Event (XLateKey (&event.xkey), event.type == KeyPress);
            break;

        case MotionNotify:
            if (mouse_active) {
                if (dgamouse) {
                    mx += (event.xmotion.x + win_x) * 2;
                    my += (event.xmotion.y + win_y) * 2;
                } else {
                    mx += ((int)event.xmotion.x - mwx) * 2;
                    my += ((int)event.xmotion.y - mwy) * 2;
                    mwx = event.xmotion.x;
                    mwy = event.xmotion.y;

                    if (mx || my) {
                        dowarp = true;
                    }
                }
            }

            break;


        case ButtonPress:
            b = -1;

            if (event.xbutton.button == 1) {
                b = 0;
            } else if (event.xbutton.button == 2) {
                b = 2;
            } else if (event.xbutton.button == 3) {
                b = 1;
            }

            if (b >= 0) {
                Do_Key_Event (K_MOUSE1 + b, true);
            }

            break;

        case ButtonRelease:
            b = -1;

            if (event.xbutton.button == 1) {
                b = 0;
            } else if (event.xbutton.button == 2) {
                b = 2;
            } else if (event.xbutton.button == 3) {
                b = 1;
            }

            if (b >= 0) {
                Do_Key_Event (K_MOUSE1 + b, false);
            }

            break;

        case CreateNotify :
            win_x = event.xcreatewindow.x;
            win_y = event.xcreatewindow.y;
            break;

        case ConfigureNotify :
            win_x = event.xconfigure.x;
            win_y = event.xconfigure.y;
            break;
        }
    }

    if (dowarp) {
        /* move the mouse to the window center again */
        XWarpPointer (display, None, mainwin, 0, 0, 0, 0, viddef.width >> 1, viddef.height >> 1);
    }
}


void KBD_Update (void)
{
// get events from x server
    HandleEvents();
}

