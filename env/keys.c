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
#include "client.h"

#include "com_string.h"
#include "console.h"


/*

key up events are sent even if in console mode

*/


#define         MAXCMDLINE      256

char    key_lines[ 32 ][ MAXCMDLINE ];
int     key_linepos;
int     shift_down = false;
int     anykeydown;

int     edit_line = 0;
int     history_line = 0;

int         key_waiting;
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

/**
 * \brief Complete console command or cvar.
 */
PRIVATE void CompleteCommand (void)
{
    char    *cmd, *s;

    s = key_lines[edit_line] + 1;

    if (*s == '\\' || *s == '/') {
        s++;
    }

    completionString = s;
    Cmd_CommandCompletion (FindMatches);
    cmd = shortestMatch;

    if (! cmd) {
        // TODO: For now
        Cvar_CommandCompletion (FindMatches);
        cmd = shortestMatch;
    }

    if (cmd) {
        key_lines[ edit_line ][ 1 ] = '/';
        com_strlcpy (key_lines[ edit_line ] + 2, cmd, sizeof (key_lines[ edit_line ]) - 2);
        key_linepos = strlen (cmd) + 2;
        key_lines[ edit_line ][ key_linepos ] = ' ';
        key_linepos++;
        key_lines[ edit_line ][ key_linepos ] = 0;
        return;
    }
}

PUBLIC _boolean Key_IsDown (int keynum)
{
    if (keynum == -1) {
        return false;
    }

    return keydown[ keynum ];
}

/**
 * \brief Interactive line editing and console scrollback.
 * \param[in] key Key that has been pressed.
 */
PRIVATE void Key_Console (int key)
{
    switch (key) {
    case K_KP_SLASH:
        key = '/';
        break;

    case K_KP_MINUS:
        key = '-';
        break;

    case K_KP_PLUS:
        key = '+';
        break;

    case K_KP_HOME:
        key = '7';
        break;

    case K_KP_UPARROW:
        key = '8';
        break;

    case K_KP_PGUP:
        key = '9';
        break;

    case K_KP_LEFTARROW:
        key = '4';
        break;

    case K_KP_5:
        key = '5';
        break;

    case K_KP_RIGHTARROW:
        key = '6';
        break;

    case K_KP_END:
        key = '1';
        break;

    case K_KP_DOWNARROW:
        key = '2';
        break;

    case K_KP_PGDN:
        key = '3';
        break;

    case K_KP_INS:
        key = '0';
        break;

    case K_KP_DEL:
        key = '.';
        break;
    }

    if ((TOUPPER (key) == 'V' && keydown[K_CTRL]) ||
            (((key == K_INS) || (key == K_KP_INS)) && keydown[K_SHIFT])) {
        char *cbd;

        if ((cbd = Sys_GetClipboardData()) != 0) {
            int iStringLength;

            strtok (cbd, "\n\r\b");

            iStringLength = strlen (cbd);

            if (iStringLength + key_linepos >= MAXCMDLINE) {
                iStringLength = MAXCMDLINE - key_linepos;
            }

            if (iStringLength > 0) {
                cbd[ iStringLength ] = '\0';
                com_strlcat (key_lines[ edit_line ], cbd, sizeof (key_lines[ edit_line ]));
                key_linepos += iStringLength;
            }

            MM_FREE (cbd);
        }

        return;
    }

    // ctrl-L clears screen
    if (key == 'l' && keydown[ K_CTRL ]) {
        Cbuf_AddText ("clear\n");
        return;
    }

    // enter finishes the line
    if (key == K_ENTER || key == K_KP_ENTER) {
        // backslash text are commands, else chat
        if (key_lines[ edit_line ][ 1 ] == '\\' || key_lines[ edit_line ][ 1 ] == '/') {
            Cbuf_AddText (key_lines[ edit_line ] + 2);   // skip the >
        } else {
            Cbuf_AddText (key_lines[ edit_line ] + 1);   // valid command
        }

        Cbuf_AddText ("\n");
        edit_line = (edit_line + 1) & 31;
        history_line = edit_line;
        key_lines[ edit_line ][ 0 ] = ']';
        key_linepos = 1;

        if (ClientStatic.state == ca_disconnected) {
            Client_Screen_UpdateScreen();   // force an update, because the command
            // may take some time
        }

        return;
    }

    if (key == K_TAB) {
        // command completion
        CompleteCommand();
        return;
    }

    if ((key == K_BACKSPACE) || (key == K_LEFTARROW) || (key == K_KP_LEFTARROW) || ((key == 'h') && (keydown[ K_CTRL ]))) {
        if (key_linepos > 1) {
            key_linepos--;
        }

        return;
    }

    if ((key == K_UPARROW) || (key == K_KP_UPARROW) ||
            ((key == 'p') && keydown[K_CTRL])) {
        do {
            history_line = (history_line - 1) & 31;

        } while (history_line != edit_line
                 && ! key_lines[ history_line ][ 1 ]);

        if (history_line == edit_line) {
            history_line = (edit_line + 1) & 31;
        }

        com_strlcpy (key_lines[ edit_line ], key_lines[ history_line ], sizeof (key_lines[ edit_line ]));
        key_linepos = strlen (key_lines[edit_line]);
        return;
    }

    if ((key == K_DOWNARROW) || (key == K_KP_DOWNARROW) ||
            ((key == 'n') && keydown[K_CTRL])) {
        if (history_line == edit_line) {
            return;
        }

        do {
            history_line = (history_line + 1) & 31;
        } while (history_line != edit_line
                 && !key_lines[history_line][1]);

        if (history_line == edit_line) {
            key_lines[edit_line][0] = ']';
            key_linepos = 1;
        } else {
            com_strlcpy (key_lines[ edit_line ], key_lines[ history_line ], sizeof (key_lines[ edit_line ]));
            key_linepos = strlen (key_lines[edit_line]);
        }

        return;
    }


    // console scrolling
    if (key == K_PGUP || key == K_KP_PGUP) {
        Con_PageUp();
        return;
    }

    if (key == K_PGDN || key == K_KP_PGDN) {
        Con_PageDown();
        return;
    }

    if (key == K_MWHEELUP) {
        Con_PageUp();

        if (keys[K_CTRL].down) { // hold <ctrl> to accelerate scrolling
            Con_PageUp();
            Con_PageUp();
        }

        return;
    }

    if (key == K_MWHEELDOWN) {
        Con_PageDown();

        if (keys[K_CTRL].down) { // hold <ctrl> to accelerate scrolling
            Con_PageDown();
            Con_PageDown();
        }

        return;
    }

    // ctrl-home = top of console
    if (key == K_HOME || key == K_KP_HOME) {
        Con_Top();
        return;
    }

    // ctrl-end = bottom of console
    if (key == K_END || key == K_KP_END) {
        Con_Bottom();
        return;
    }

    if (key < 32 || key > 127) {
        return; // non printable
    }

    if (key_linepos < MAXCMDLINE - 1) {
        key_lines[ edit_line ][ key_linepos ] = key;
        key_linepos++;
        key_lines[ edit_line ][ key_linepos ] = 0;
    }

}

//============================================================================

PUBLIC _boolean chat_team;
PUBLIC char     chat_buffer[ MAXCMDLINE ];
PUBLIC int      chat_bufferlen = 0;


/**
 * \brief Key from message loop
 * \param[in] key Key that has been pressed.
 */
PRIVATE void Key_Message (int key)
{

    if (key == K_ENTER || key == K_KP_ENTER) {
        if (chat_team) {
            Cbuf_AddText ("say_team \"");
        } else {
            Cbuf_AddText ("say \"");
        }

        Cbuf_AddText (chat_buffer);
        Cbuf_AddText ("\"\n");

        ClientStatic.key_dest = key_game;
        chat_bufferlen = 0;
        chat_buffer[ 0 ] = '\0'; // NUL-terminate chat buffer.
        return;
    }

    if (key == K_ESCAPE) {
        ClientStatic.key_dest = key_game;
        chat_bufferlen = 0;
        chat_buffer[ 0 ] = '\0'; // NUL-terminate chat buffer.
        return;
    }

    if (key < 32 || key > 127) {
        return; // non printable
    }

    if (key == K_BACKSPACE) {
        if (chat_bufferlen) {
            chat_bufferlen--;
            chat_buffer[ chat_bufferlen ] = '\0'; // NUL-terminate chat buffer.
        }

        return;
    }

    if (chat_bufferlen == sizeof (chat_buffer) - 1) {
        return; // all full
    }

    chat_buffer[ chat_bufferlen++ ] = key;
    chat_buffer[ chat_bufferlen ] = '\0'; // NUL-terminate chat buffer.
}

//============================================================================

/**
 * \brief Convert string to key number
 * \param[in] str String character to convert to key number
 * \return A key number to be used to index keybindings[] by looking at
    the given string. Single ASCII characters return themselves, while
    the K_* names are matched up. Otherwise -1 is returned.
 */
PRIVATE int Key_StringToKeynum (char *str)
{
    keyname_t *kn;

    if (! str || ! str[ 0 ]) {
        return -1;
    }

    if (! str[ 1 ]) {
        return str[ 0 ];
    }

    for (kn = keynames ; kn->name ; ++kn) {
        if (! com_stricmp (str, kn->name)) {
            return kn->keynum;
        }
    }

    return -1;
}

/**
 * \brief Convert key number to string
 * \param[in] keynum key number to convert to string
 * \return A string (either a single ASCII char, or a K_* name) for the given keynum.
 */
PUBLIC char *Key_KeynumToString (int keynum)
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
PUBLIC void Key_SetBinding (int keynum, char *binding)
{
    char *newbinding;
    int length;

    if (keynum == -1) {
        return;
    }

// free old bindings
    if (keybindings[ keynum ]) {
        Z_Free (keybindings[ keynum ]);
        keybindings[ keynum ] = NULL;
    }

// allocate memory for new binding
    length = strlen (binding);
    newbinding = (char *)Z_Malloc (length + 1);
    com_strlcpy (newbinding, binding, length + 1);
    newbinding[ length ] = 0;
    keybindings[ keynum ] = newbinding;
}


PUBLIC char *Key_GetBinding (const int keynum)
{
    if (keynum == -1) {
        return "";
    }

    return keybindings[ keynum ];
}

PUBLIC int Key_GetKey (const char *binding)
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
 * \brief Console callback method to unbind key mapping
 */
PRIVATE void Key_Unbind_f (void)
{
    int b;

    if (Cmd_Argc() != 2) {
        return;
    }

    b = Key_StringToKeynum (Cmd_Argv (1));

    if (b == -1) {
        return;
    }

    Key_SetBinding (b, "");
}

/**
 * \brief Console callback method to unbind all key mapping
 */
PRIVATE void Key_Unbindall_f (void)
{
    int i;

    for (i = 0; i < 256; ++i) {
        if (keybindings[ i ]) {
            Key_SetBinding (i, "");
        }
    }
}


/**
 * \brief Console callback method to bind key mapping
 */
PRIVATE void Key_Bind_f (void)
{
    int                     i, c, b;
    char            cmd[ 1024 ];

    c = Cmd_Argc();

    if (c < 2) {
        return;
    }

    b = Key_StringToKeynum (Cmd_Argv (1));

    if (b == -1) {
        return;
    }

    if (c == 2) {
        return;
    }

// copy the rest of the command line
    cmd[ 0 ] = '\0';                // start out with a null string

    for (i = 2; i < c; ++i) {
        com_strlcat (cmd, Cmd_Argv (i), sizeof (cmd));

        if (i != (c - 1)) {
            com_strlcat (cmd, " ", sizeof (cmd));
        }
    }

    Key_SetBinding (b, cmd);
}


/**
 * \brief Writes lines containing "bind key value".
 * \param[in] f File handle to write out key bindings
 */
PUBLIC void Key_WriteBindings (FILE *f)
{
    int     i;

    for (i = 0; i < 256; ++i) {
        if (keybindings[ i ] && keybindings[ i ][ 0 ]) {
            fprintf (f, "bind %s \"%s\"\n", Key_KeynumToString (i), keybindings[ i ]);
        }
    }
}

/**
 * \brief Console callback method to print bindlist
 */
PRIVATE void Key_Bindlist_f (void)
{
}

/**
 * \brief Initialize key mapping module.
 */
PUBLIC void Key_Init (void)
{
    int i;

    for (i = 0; i < 32; ++i) {
        key_lines[ i ][ 0 ] = ']';
        key_lines[ i ][ 1 ] = 0;
    }

    key_linepos = 1;

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
    Cmd_AddCommand ("bind",          Key_Bind_f);
    Cmd_AddCommand ("unbind",       Key_Unbind_f);
    Cmd_AddCommand ("unbindall", Key_Unbindall_f);
    Cmd_AddCommand ("listBinds",     Key_Bindlist_f);
}

/**
 * \brief Called by the system between frames for both key up and key down events.
 * \param[in] key Key mapping
 * \param[in] down Is key being pressed?
 * \param[in] time
 * \note Should NOT be called during an interrupt!
 */
PUBLIC void Key_Event (int key, _boolean down, unsigned time)
{
    char    *kb;
    char    cmd[ 1024 ];

    // hack for modal presses
    if (key_waiting == -1) {
        if (down) {
            key_waiting = key;
        }

        return;
    }

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

    if (key == K_SHIFT) {
        shift_down = down;
    }

    // console key is hardcoded, so the user can never unbind it
    if (key == '`' || key == '~') {
        if (! down) {
            return;
        }

        Con_ToggleConsole_f();
        return;
    }

    // menu key is hardcoded, so the user can never unbind it
    if (key == K_ESCAPE) {
        if (! down) {
            return;
        }

        switch (ClientStatic.key_dest) {
        case key_message:
            Key_Message (key);
            break;

        case KEY_AUTOMAP:
            automap_keydown (key);
            break;

        case key_menu:
            M_Keydown (key);
            break;

        case key_game:
        case key_console:
            M_Menu_Main_f();
            break;

        default:
            Com_DPrintf ("Bad ClientStatic.key_dest\n");
        }

        return;
    }

    // track if any key is down for BUTTON_ANY
    keydown[key] = down;

    if (down) {
        if (key_repeats[key] == 1) {
            anykeydown++;
        }
    } else {
        anykeydown--;

        if (anykeydown < 0) {
            anykeydown = 0;
        }
    }

//
// key up events only generate commands if the game key binding is
// a button command (leading + sign).  These will occur even in console mode,
// to keep the character from continuing an action started before a console
// switch.  Button commands include the kenum as a parameter, so multiple
// downs can be matched with ups
//
    if (! down) {
        kb = keybindings[key];

        if (kb && kb[0] == '+') {
            com_snprintf (cmd, sizeof (cmd), "-%s %i %i\n", kb + 1, key, time);
            Cbuf_AddText (cmd);
        }

        if (keyshift[key] != key) {
            kb = keybindings[keyshift[key]];

            if (kb && kb[0] == '+') {
                com_snprintf (cmd, sizeof (cmd), "-%s %i %i\n", kb + 1, key, time);
                Cbuf_AddText (cmd);
            }
        }

        return;
    }

//
// if not a consolekey, send to the interpreter no matter what mode is
//
    if ((ClientStatic.key_dest == key_menu && menubound[key])
            || (ClientStatic.key_dest == key_console && !consolekeys[key])
            || (ClientStatic.key_dest == key_game)) {
        kb = keybindings[key];

        if (kb) {
            if (kb[0] == '+') {
                // button commands add keynum and time as a parm
                com_snprintf (cmd, sizeof (cmd), "%s %i %i\n", kb, key, time);
                Cbuf_AddText (cmd);
            } else {
                Cbuf_AddText (kb);
                Cbuf_AddText ("\n");
            }
        }

        return;
    }

    if (! down) {
        return;         // other systems only care about key down events
    }

    if (shift_down) {
        key = keyshift[ key ];
    }

    switch (ClientStatic.key_dest) {
    case KEY_AUTOMAP:
        automap_keydown (key);
        break;

    case key_message:
        Key_Message (key);
        break;

    case key_menu:
        M_Keydown (key);
        break;

    case key_game:
    case key_console:
        Key_Console (key);
        break;

    default:
        Com_DPrintf ("Bad ClientStatic.key_dest\n");
    }
}

/**
 * \brief Clear key states
 */
PUBLIC void Key_ClearStates (void)
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
