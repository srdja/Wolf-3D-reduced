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
 * \file keys.c
 * \brief Portable key tracking module.
 * \author Id Software, Inc.
 * \date 1997-2001
 */

#include <stdlib.h>
#include <string.h>

#include "keys.h"
#include "keycodes.h"
#include "../game/client.h"

#include "../util/com_string.h"

/*

key up events are sent even if in console mode

*/

int     anykeydown;

char        *keybindings[ 256 ];
_boolean    consolekeys[ 256 ];     // if true, can't be rebound while in console
_boolean    menubound[ 256 ];       // if true, can't be rebound while in menu
int         keyshift[ 256 ];        // key to map to if shift held down in console
int         key_repeats[ 256 ];     // if > 1, it is autorepeating
_boolean    keydown[ 256 ];


qkey_t      keys[MAX_KEYS];

typedef struct {
    char    *name;
    int     keynum;

} keyname_t;


// names not in this list can either be lowercase ascii, or '0xnn' hex sequences
keyname_t keynames[] = {
    {"TAB", K_TAB},
    {"ENTER", K_ENTER},
    {"ESCAPE", K_ESCAPE},
    {"SPACE", K_SPACE},
    {"BACKSPACE", K_BACKSPACE},
    {"UPARROW", K_UPARROW},
    {"DOWNARROW", K_DOWNARROW},
    {"LEFTARROW", K_LEFTARROW},
    {"RIGHTARROW", K_RIGHTARROW},

    {"ALT", K_ALT},
    {"CTRL", K_CTRL},
    {"SHIFT", K_SHIFT},

    {"COMMAND", K_COMMAND},

    {"CAPSLOCK", K_CAPSLOCK},


    {"F1", K_F1},
    {"F2", K_F2},
    {"F3", K_F3},
    {"F4", K_F4},
    {"F5", K_F5},
    {"F6", K_F6},
    {"F7", K_F7},
    {"F8", K_F8},
    {"F9", K_F9},
    {"F10", K_F10},
    {"F11", K_F11},
    {"F12", K_F12},

    {"INS", K_INS},
    {"DEL", K_DEL},
    {"PGDN", K_PGDN},
    {"PGUP", K_PGUP},
    {"HOME", K_HOME},
    {"END", K_END},

    {"MOUSE1", K_MOUSE1},
    {"MOUSE2", K_MOUSE2},
    {"MOUSE3", K_MOUSE3},
    {"MOUSE4", K_MOUSE4},
    {"MOUSE5", K_MOUSE5},

    {"MWHEELUP",    K_MWHEELUP },
    {"MWHEELDOWN",  K_MWHEELDOWN },

    {"JOY1", K_JOY1},
    {"JOY2", K_JOY2},
    {"JOY3", K_JOY3},
    {"JOY4", K_JOY4},
    {"JOY5", K_JOY5},
    {"JOY6", K_JOY6},
    {"JOY7", K_JOY7},
    {"JOY8", K_JOY8},
    {"JOY9", K_JOY9},
    {"JOY10", K_JOY10},
    {"JOY11", K_JOY11},
    {"JOY12", K_JOY12},
    {"JOY13", K_JOY13},
    {"JOY14", K_JOY14},
    {"JOY15", K_JOY15},
    {"JOY16", K_JOY16},
    {"JOY17", K_JOY17},
    {"JOY18", K_JOY18},
    {"JOY19", K_JOY19},
    {"JOY20", K_JOY20},
    {"JOY21", K_JOY21},
    {"JOY22", K_JOY22},
    {"JOY23", K_JOY23},
    {"JOY24", K_JOY24},
    {"JOY25", K_JOY25},
    {"JOY26", K_JOY26},
    {"JOY27", K_JOY27},
    {"JOY28", K_JOY28},
    {"JOY29", K_JOY29},
    {"JOY30", K_JOY30},
    {"JOY31", K_JOY31},
    {"JOY32", K_JOY32},

    {"AUX1", K_AUX1},
    {"AUX2", K_AUX2},
    {"AUX3", K_AUX3},
    {"AUX4", K_AUX4},
    {"AUX5", K_AUX5},
    {"AUX6", K_AUX6},
    {"AUX7", K_AUX7},
    {"AUX8", K_AUX8},
    {"AUX9", K_AUX9},
    {"AUX10", K_AUX10},
    {"AUX11", K_AUX11},
    {"AUX12", K_AUX12},
    {"AUX13", K_AUX13},
    {"AUX14", K_AUX14},
    {"AUX15", K_AUX15},
    {"AUX16", K_AUX16},

    {"KP_HOME", K_KP_HOME },
    {"KP_UPARROW",  K_KP_UPARROW },
    {"KP_PGUP", K_KP_PGUP },
    {"KP_LEFTARROW",    K_KP_LEFTARROW },
    {"KP_5",    K_KP_5 },
    {"KP_RIGHTARROW",   K_KP_RIGHTARROW },
    {"KP_END",  K_KP_END },
    {"KP_DOWNARROW",    K_KP_DOWNARROW },
    {"KP_PGDN", K_KP_PGDN },
    {"KP_ENTER",    K_KP_ENTER },
    {"KP_INS",  K_KP_INS },
    {"KP_DEL",  K_KP_DEL },
    {"KP_SLASH",    K_KP_SLASH },
    {"KP_MINUS",    K_KP_MINUS },
    {"KP_PLUS", K_KP_PLUS },
    {"KP_NUMLOCK",  K_KP_NUMLOCK },
    {"KP_STAR", K_KP_STAR },
    {"KP_EQUALS",   K_KP_EQUALS },

    {"PAUSE", K_PAUSE},

    {"SEMICOLON", ';'}, // because a raw semicolon seperates commands

    {NULL, 0}
};

/*
==============================================================================

                        LINE TYPING INTO THE CONSOLE

==============================================================================
*/

_boolean Key_IsDown (int keynum)
{
    if (keynum == -1) {
        return false;
    }

    return keydown[ keynum ];
}

/**
 * \brief Convert key number to string
 * \param[in] keynum key number to convert to string
 * \return A string (either a single ASCII char, or a K_* name) for the given keynum.
 */
char *Key_KeynumToString (int keynum)
{
    // FIXME: handle quote special (general escape sequence?)
    keyname_t       *kn;
    static  char    tinystr[2];

    if (keynum == -1) {
        return "<KEY NOT FOUND>";
    }

    if (keynum > 32 && keynum < 127) {
        // printable ASCII
        tinystr[ 0 ] = keynum;
        tinystr[ 1 ] = '\0';
        return tinystr;
    }

    for (kn = keynames ; kn->name ; ++kn) {
        if (keynum == kn->keynum) {
            return kn->name;
        }
    }

    return "<UNKNOWN KEYNUM>";
}

/**
 * \brief Set new key binding.
 */
void Key_SetBinding (int keynum, char *binding)
{
    char *newbinding;
    int length;

    if (keynum == -1) {
        return;
    }

// free old bindings
    if (keybindings[ keynum ]) {
        free (keybindings[ keynum ]);
        keybindings[ keynum ] = NULL;
    }

// allocate memory for new binding
    length = strlen (binding);
    newbinding = (char *)malloc (length + 1);
    strncpy(newbinding, binding, length + 1);
    newbinding[ length ] = 0;
    keybindings[ keynum ] = newbinding;
}

int Key_GetKey (const char *binding)
{
    int i;

    if (binding) {
        for (i = 0 ; i < 256 ; i++) {
            if (keybindings[ i ] && com_stricmp (binding, keybindings[ i]) == 0) {
                return i;
            }
        }
    }

    return -1;
}

/**
 * \brief Writes lines containing "bind key value".
 * \param[in] f File handle to write out key bindings
 */
void Key_WriteBindings (FILE *f)
{
    int     i;

    for (i = 0; i < 256; ++i) {
        if (keybindings[ i ] && keybindings[ i ][ 0 ]) {
            fprintf (f, "bind %s \"%s\"\n", Key_KeynumToString (i), keybindings[ i ]);
        }
    }
}

/**
 * \brief Initialize key mapping module.
 */
void Key_Init (void)
{
    int i;
//
// init ASCII characters in console mode
//
    for (i = 32; i < 128; ++i) {
        consolekeys[ i ] = true;
    }

    consolekeys[ K_ENTER ] = true;
    consolekeys[ K_KP_ENTER ] = true;
    consolekeys[ K_TAB ] = true;
    consolekeys[ K_LEFTARROW ] = true;
    consolekeys[ K_KP_LEFTARROW ] = true;
    consolekeys[ K_RIGHTARROW ] = true;
    consolekeys[ K_KP_RIGHTARROW ] = true;
    consolekeys[ K_UPARROW ] = true;
    consolekeys[ K_KP_UPARROW ] = true;
    consolekeys[ K_DOWNARROW ] = true;
    consolekeys[ K_KP_DOWNARROW ] = true;
    consolekeys[ K_BACKSPACE ] = true;
    consolekeys[ K_HOME ] = true;
    consolekeys[ K_KP_HOME ] = true;
    consolekeys[ K_END ] = true;
    consolekeys[ K_KP_END ] = true;
    consolekeys[ K_PGUP ] = true;
    consolekeys[ K_KP_PGUP ] = true;
    consolekeys[ K_PGDN ] = true;
    consolekeys[ K_KP_PGDN ] = true;
    consolekeys[ K_SHIFT ] = true;
    consolekeys[ K_INS ] = true;
    consolekeys[ K_KP_INS ] = true;
    consolekeys[ K_KP_DEL ] = true;
    consolekeys[ K_KP_SLASH ] = true;
    consolekeys[ K_KP_PLUS ] = true;
    consolekeys[ K_KP_MINUS ] = true;
    consolekeys[ K_KP_5 ] = true;

    consolekeys[ '`' ] = false;
    consolekeys[ '~' ] = false;

    for (i = 0; i < 256; ++i) {
        keyshift[ i ] = i;
    }

    for (i = 'a'; i <= 'z'; ++i) {
        keyshift[ i ] = i - 'a' + 'A';
    }

    keyshift[ '1' ] = '!';
    keyshift[ '2' ] = '@';
    keyshift[ '3' ] = '#';
    keyshift[ '4' ] = '$';
    keyshift[ '5' ] = '%';
    keyshift[ '6' ] = '^';
    keyshift[ '7' ] = '&';
    keyshift[ '8' ] = '*';
    keyshift[ '9' ] = '(';
    keyshift[ '0' ] = ')';
    keyshift[ '-' ] = '_';
    keyshift[ '=' ] = '+';
    keyshift[ ',' ] = '<';
    keyshift[ '.' ] = '>';
    keyshift[ '/' ] = '?';
    keyshift[ ';' ] = ':';
    keyshift[ '\''] = '"';
    keyshift[ '[' ] = '{';
    keyshift[ ']' ] = '}';
    keyshift[ '`' ] = '~';
    keyshift[ '\\' ] = '|';

    menubound[ K_ESCAPE ] = true;

    for (i = 0; i < 12; ++i) {
        menubound[ K_F1 + i ] = true;
    }

//
// register our functions
//
    //Cmd_AddCommand ("bind",          Key_Bind_f);
   // Cmd_AddCommand ("unbind",       Key_Unbind_f);
    //Cmd_AddCommand ("unbindall", Key_Unbindall_f);
}

/**
 * \brief Called by the system between frames for both key up and key down events.
 * \param[in] key Key mapping
 * \param[in] down Is key being pressed?
 * \param[in] time
 * \note Should NOT be called during an interrupt!
 */

extern void IN_LeftDown (void);
extern void IN_LeftUp (void);
extern void IN_RightDown (void);
extern void IN_RightUp (void);
extern void IN_ForwardDown (void);
extern void IN_ForwardUp (void);
extern void IN_BackDown (void);
extern void IN_BackUp (void);
extern void IN_MoveleftDown (void);
extern void IN_MoveleftUp (void);
extern void IN_MoverightDown (void);
extern void IN_MoverightUp (void);
extern void IN_AttackDown (void);
extern void IN_AttackUp (void);
extern void IN_UseDown (void);
extern void IN_UseUp (void);

// TODO THIS FUNCTION GENERATES KEYPRESS COMMANDS
void Key_Event (int key, _boolean down, unsigned time)
{

    // update auto-repeat status
    if (down) {
        key_repeats[key]++;

        if (key != K_BACKSPACE
                && key != K_PAUSE
                && key != K_PGUP
                && key != K_KP_PGUP
                && key != K_PGDN
                && key != K_KP_PGDN
                && key_repeats[key] > 1) {
            return; // ignore most autorepeats
        }
    } else {
        key_repeats[ key ] = 0;
    }

    if (key == K_ESCAPE)
        exit(0);

    // FIXME transitional hack - only game play events
    if (!down) {
        switch (key) {
            case 'a':
                IN_MoveleftDown();
                break;
            case 'w':
                IN_ForwardDown();
                break;
            case 's':
                IN_BackDown();
                break;
            case 'd':
                IN_MoverightDown();
                break;
            case K_LEFTARROW:
                IN_LeftDown();
                break;
            case K_RIGHTARROW:
                IN_RightDown();
                break;
            case K_SPACE:
                IN_UseDown();
                break;
            case K_CTRL:
                IN_AttackDown();
                break;
        }
        return;
    }
    switch (key) {
        case 'a':
            IN_MoveleftUp();
            break;
        case 'w':
            IN_ForwardUp();
            break;
        case 's':
            IN_BackUp();
            break;
        case 'd':
            IN_MoverightUp();
            break;
        case K_LEFTARROW:
            IN_LeftUp();
            break;
        case K_RIGHTARROW:
            IN_RightUp();
            break;
        case K_SPACE:
            IN_UseUp();
            break;
        case K_CTRL:
            IN_AttackUp();
            break;
    }

}

/**
 * \brief Clear key states
 */
void Key_ClearStates (void)
{
    int i;

    anykeydown = false;

    for (i = 0 ; i < 256 ; ++i) {
        if (keydown[ i ] || key_repeats[ i ]) {
            Key_Event (i, false, 0);
        }

        keydown[ i ] = 0;
        key_repeats[ i ] = 0;
    }
}