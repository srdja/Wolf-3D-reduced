/*

    Copyright (C) 2004-2012 Michael Liebscher <johnnycanuck@users.sourceforge.net>
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
 * \file console.c
 * \brief Console drawing and management.
 * \author Michael Liebscher
 * \date 2004-2012
 * \note This code was derived from Quake II, and was originally written by Id Software, Inc.
 */


#include <string.h>

#include "num_type.h"
#include "console.h"
#include "client.h"
#include "common.h"
#include "font_manager.h"
#include "com_string.h"
#include "video.h"
#include "renderer.h"


extern viddef_t viddef;



console_t   con;

cvar_t      *con_notifytime;


#define     MAXCMDLINE  256

extern  char    key_lines[ 32 ][ MAXCMDLINE ];
extern  int     edit_line;
extern  int     key_linepos;


colour3_t colourconLGray = { 220, 220, 220 };
colour3_t colourconLLGray = { 192, 192, 192 };

/**
 * \brief Put string of text on screen
 * \param[in] x X-Coordinate
 * \param[in] y Y-Coordinate
 * \param[in] s Text
 */
PRIVATE void DrawString (int x, int y, char *s)
{
    Font_put_line (FONT0, x, y, s);
}

/**
 * \brief Clear any keys that where typed.
 */
void Key_ClearTyping (void)
{
    key_lines[ edit_line ][ 1 ] = 0;    // clear any typing
    key_linepos = 1;
}

/**
 * \brief Get ready to enter console.
 */
PUBLIC void Con_ToggleConsole_f (void)
{
//  SCR_EndLoadingPlaque(); // get rid of loading plaque

//  if( cl.attractloop )
//  {
//      Cbuf_AddText( "killserver\n" );
//      return;
//  }

    Key_ClearTyping();
    Con_ClearNotify();

    if (ClientStatic.key_dest == key_console) {
        M_ForceMenuOff();
        Cvar_Set ("paused", "0");
    } else {
        M_ForceMenuOff();
        ClientStatic.key_dest = key_console;

//      if (Cvar_VariableValue ("maxclients") == 1
//          && Com_ServerState ())
//          Cvar_Set( "paused", "1" );
    }
}

/**
 * \brief Enter console for chat.
 */
PRIVATE void Con_ToggleChat_f (void)
{
    Key_ClearTyping();

    if (ClientStatic.key_dest == key_console) {
        if (ClientStatic.state == ca_active) {
            M_ForceMenuOff();
            ClientStatic.key_dest = key_game;
        }
    } else {
        ClientStatic.key_dest = key_console;
    }

    Con_ClearNotify();
}

/**
 * \brief Clear console text buffer.
 * \note Zero sets con.text memory block.
 */
PRIVATE void Con_Clear_f (void)
{
    memset (con.text, ' ', CON_TEXTSIZE);
}

/**
 * \brief Save the console contents out to a file.
 */
PRIVATE void Con_Dump_f (void)
{
    int     length, x;
    char    *line;
    FILE    *f;
    char    buffer[1024];
    char    name[MAX_OSPATH];

    if (Cmd_Argc() != 2) {
        Com_Printf ("usage: conDump <filename>\n");
        return;
    }

    com_snprintf (name, sizeof (name), "%s%c%s.txt", FS_Gamedir(), PATH_SEP, Cmd_Argv (1));

    Com_Printf ("Dumped console text to %s.\n", name);
    FS_CreateDirectory (name);
    f = fopen (name, "w");

    if (!f) {
        Com_Printf ("ERROR: couldn't open.\n");
        return;
    }

    // skip empty lines
    for (length = con.current - con.totallines + 1; length <= con.current; ++length) {
        line = con.text + (length % con.totallines) * con.linewidth;

        for (x = 0; x < con.linewidth; ++x) {
            if (line[ x ] != ' ') {
                break;
            }
        }

        if (x != con.linewidth) {
            break;
        }
    }

    // write the remaining lines
    buffer[ con.linewidth ] = '\0';

    for (; length <= con.current ; ++length) {
        line = con.text + (length % con.totallines) * con.linewidth;
        com_strlcpy (buffer, line, con.linewidth);

        for (x = con.linewidth - 1; x >= 0; --x) {
            if (buffer[ x ] == ' ') {
                buffer[ x ] = '\0'; // NUL-terminate string
            } else {
                break;
            }
        }

        for (x = 0; buffer[ x ]; ++x) {
            buffer[ x ] &= 0x7f;
        }

        fprintf (f, "%s\n", buffer);
    }

    fclose (f);
}

/**
 * \brief Clear console con.times.
 */
PUBLIC void Con_ClearNotify (void)
{
    int i;

    for (i = 0; i < NUM_CON_TIMES; ++i) {
        con.times[ i ] = 0;
    }
}

/**
 * \brief Set to message mode.
 */
PRIVATE void Con_MessageMode_f (void)
{
//  chat_team = false;
    ClientStatic.key_dest = key_message;
}

/**
 * \brief Set to team message mode.
 */
PRIVATE void Con_MessageMode2_f (void)
{
//  chat_team = true;
    ClientStatic.key_dest = key_message;
}

/**
 * \brief If the line width has changed, reformat the buffer.
 */
PUBLIC void Con_CheckResize (void)
{
    int i, j, width, oldwidth, oldtotallines, numlines, numchars;
    char    tbuf[ CON_TEXTSIZE ];

    width = (viddef.width >> 3) - 2;

    if (width == con.linewidth) {
        return;
    }

    if (width < 1) {         // video hasn't been initialized yet
        width = 38;
        con.linewidth = width;
        con.totallines = CON_TEXTSIZE / con.linewidth;
        memset (con.text, ' ', CON_TEXTSIZE);
    } else {
        oldwidth = con.linewidth;
        con.linewidth = width;
        oldtotallines = con.totallines;
        con.totallines = CON_TEXTSIZE / con.linewidth;
        numlines = oldtotallines;

        if (con.totallines < numlines) {
            numlines = con.totallines;
        }

        numchars = oldwidth;

        if (con.linewidth < numchars) {
            numchars = con.linewidth;
        }

        memcpy (tbuf, con.text, CON_TEXTSIZE);
        memset (con.text, ' ', CON_TEXTSIZE);

        for (i = 0; i < numlines; ++i) {
            for (j = 0; j < numchars; ++j) {
                con.text[ (con.totallines - 1 - i) * con.linewidth + j] =
                    tbuf[ ((con.current - i + oldtotallines) %
                           oldtotallines) * oldwidth + j];
            }
        }

        Con_ClearNotify();
    }

    con.current = con.totallines - 1;
    con.display = con.current;
}

/**
 * \brief Initialize console
 */
PUBLIC void Con_Init (void)
{
    con.linewidth = -1;

    Con_CheckResize();

    Com_Printf ("Console Initialized\n");

//
// register our commands
//
    con_notifytime = Cvar_Get ("con_notifytime", "3", CVAR_INIT);

    Cmd_AddCommand ("toggleconsole", Con_ToggleConsole_f);
    Cmd_AddCommand ("togglechat", Con_ToggleChat_f);
    Cmd_AddCommand ("messagemode", Con_MessageMode_f);
    Cmd_AddCommand ("messagemode2", Con_MessageMode2_f);
    Cmd_AddCommand ("clear", Con_Clear_f);
    Cmd_AddCommand ("conDump", Con_Dump_f);

    con.initialized = true;
}

/**
 * \brief Fill rest of line with spaces.
 */
PRIVATE void Con_Linefeed (void)
{
    con.x = 0;

    if (con.display == con.current) {
        con.display++;
    }

    con.current++;
    memset (&con.text[ (con.current % con.totallines) * con.linewidth ], ' ', con.linewidth);
}

/**
 * \brief Print formatted message to the console.
 * \param[in] txt Text message to print
 * \note
 *      Handles cursor positioning, line wrapping, etc
 *      All console printing must go through this in order to be logged to disk
 *      If no console is visible, the text will appear at the top of the game window
 */
PUBLIC void Con_Print (char *txt)
{
    int     y;
    int     c, wordlength;
    static int  cr;
    int     mask;

    if (!con.initialized) {
        return;
    }

    if (txt[ 0 ] == 1 || txt[ 0 ] == 2) {
        mask = 128;     // go to colored text
        txt++;
    } else {
        mask = 0;
    }


    while ((c = *txt)) {
        // count word length
        for (wordlength = 0 ; wordlength < con.linewidth ; ++wordlength) {
            if (txt[ wordlength ] <= ' ') {
                break;
            }
        }

        // word wrap
        if (wordlength != con.linewidth && (con.x + wordlength > con.linewidth)) {
            con.x = 0;
        }

        txt++;

        if (cr) {
            con.current--;
            cr = false;
        }


        if (!con.x) {
            Con_Linefeed();

            // mark time for transparent overlay
            if (con.current >= 0) {
                con.times[ con.current % NUM_CON_TIMES ] = (float)ClientStatic.realtime;
            }
        }

        switch (c) {
        case '\n':
            con.x = 0;
            break;

        case '\r':
            con.x = 0;
            cr = 1;
            break;

        default:    // display character and advance
            y = con.current % con.totallines;
            con.text[ y * con.linewidth + con.x] = c | mask | con.ormask;
            con.x++;

            if (con.x >= con.linewidth) {
                con.x = 0;
            }

            break;
        }

    }
}

/**
 * \brief Print message that is centered on screen.
 * \param[in] text Message string.
 */
PUBLIC void Con_CenteredPrint (const char *text)
{
    int     length;
    char    buffer[ 1024 ];

    length = strlen (text);
    length = (con.linewidth - length) >> 1;

    if (length < 0) {
        length = 0;
    }

    memset (buffer, ' ', length);
    com_strlcpy (buffer + length, text, sizeof (buffer) - length);
    com_strlcat (buffer, "\n", sizeof (buffer));
    Con_Print (buffer);
}


/*
==============================================================================

    DRAWING

==============================================================================
*/

/**
 * \brief The input line scrolls horizontally if typing goes beyond the right edge.
 */
PRIVATE void Con_DrawInput (void)
{
    int     y;
    int     i;
    char    *text;
    W16 heightfont, charwidth;
    static char ccursor[ 2 ] = { ' ', '_' };


    if (ClientStatic.key_dest != key_console) {
        return;     // don't draw anything (always draw if not active)
    }


    heightfont = Font_GetSize (FONT0) + 4;


    text = key_lines[ edit_line ];

// add the cursor frame
    text[ key_linepos ] = ccursor[ 0 + ((int) (ClientStatic.realtime >> 8) & 1) ];

// fill out remainder with spaces
    for (i = key_linepos + 1 ; i < con.linewidth ; ++i) {
        text[ i ] = ' ';
    }

//  prestep if horizontally scrolling
    if (key_linepos >= con.linewidth) {
        text += 1 + key_linepos - con.linewidth;
    }

// draw it
    y = con.vislines - heightfont;

    charwidth = 8;

    for (i = 0 ; i < con.linewidth ; ++i) {
        charwidth += Font_put_character (FONT0, charwidth, con.vislines - 22, text[ i ]);
    }

// remove cursor
    key_lines[ edit_line ][ key_linepos ] = 0;
}

/**
 * \brief Draws the last few lines of output transparently over the game top.
 */
PUBLIC void Con_DrawNotify (void)
{
    int     x, v;
    int     charwidth;
    char    *text;
    int     i;
    int     time;
//  char    *s;
//  int     skip;
    W16     size;


    Font_SetSize (FONT1, 1);
    size = Font_GetSize (FONT1);

    Font_SetColour (FONT1, colourWhite);

    v = 0;

    for (i = con.current - NUM_CON_TIMES + 1 ; i <= con.current ; ++i) {
        if (i < 0) {
            continue;
        }

        time = FloatToInt (con.times[ i % NUM_CON_TIMES ]);

        if (time == 0) {
            continue;
        }

        time = ClientStatic.realtime - time;

        if (time > con_notifytime->value * 1000) {
            continue;
        }

        text = con.text + (i % con.totallines) * con.linewidth;

        charwidth = 0;

        for (x = 0 ; x < con.linewidth ; ++x) {
            charwidth += Font_put_character (FONT1, charwidth, v, text[ x ]);
        }

        v += size;
    }

    Font_SetSize (FONT1, 2);
}

/**
 * \brief Draws the console.
 * \param[in] frac Fraction of the screen the console will take up. Range is 0.0 to 1.0
 */
PUBLIC void Con_DrawConsole (float frac)
{
    int             i, x, y;
    int             rows;
    char            *text;
    int             row;
    W32             lines;
//  int w, h;
    int heightfont, charwidth;

    lines = FloatToInt (viddef.height * frac);

    if (lines < 1) {
        return;
    }


    Font_SetSize (FONT0, 1);
    Font_SetColour (FONT0, colourconLLGray);
    heightfont = Font_GetSize (FONT0);



    if (lines > viddef.height) {
        lines = viddef.height;
    }

//
// Draw the background
//
    R_Draw_Fill (0, -viddef.height + lines, viddef.width, viddef.height, colourBlack);
    R_Draw_Fill (0, lines - 2, viddef.width, 2, colourconLGray);
    Font_SetColour (FONT0, colourGreen);
    Font_put_lineR2L (FONT0, viddef.width - 20, lines - 2 - heightfont, APP_VERSION);
    Font_SetColour (FONT0, colourconLLGray);

//
// Draw the text
//
    con.vislines = lines;

#if 0

    rows = (lines - 8) >> 3;        // rows of text to draw

    y = lines - 24;

#else

    rows = (lines - 22) >> 3;       // rows of text to draw

    y = lines - 30;

#endif

// draw from the bottom up
    if (con.display != con.current) {
        // draw arrows to show the buffer is backscrolled
        for (x = 0; x < con.linewidth; x += 4) {
            Font_put_character (FONT0, (x + 1) << 3, y, '^');
        }

        y -= heightfont;
        rows--;
    }

    row = con.display;

    for (i = 0 ; i < rows ; ++i, y -= heightfont, --row) {
        if (row < 0) {
            break;
        }

        if (con.current - row >= con.totallines) {
            break;      // past scrollback wrap point
        }

        text = con.text + (row % con.totallines) * con.linewidth;

        charwidth = 0;

        for (x = 0; x < con.linewidth; ++x) {
            charwidth += Font_put_character (FONT0, charwidth, y, text[ x ]);
        }
    }

// draw the input prompt, user text, and cursor if desired
    Con_DrawInput();

    Font_SetSize (FONT0, 2);
}





void Con_PageUp (void)
{
    con.display -= 2;

    if (con.current - con.display >= con.totallines) {
        con.display = con.current - con.totallines + 1;
    }
}

void Con_PageDown (void)
{
    con.display += 2;

    if (con.display > con.current) {
        con.display = con.current;
    }
}

void Con_Top (void)
{
    con.display = con.totallines;

    if (con.current - con.display >= con.totallines) {
        con.display = con.current - con.totallines + 1;
    }
}

void Con_Bottom (void)
{
    con.display = con.current;
}




