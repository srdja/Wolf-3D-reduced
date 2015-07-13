/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.

This file is part of Quake III Arena source code.

Quake III Arena source code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

Quake III Arena source code is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Foobar; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/

/**
 * @file cmd.c
 * @brief Command text buffering and command execution.
 * @author Id Software, Inc.
 * @date 1999-2005
 * @note This code was derived from Quake III Arena, and was originally written by Id Software, Inc.
 */

#include <string.h>

#include "common.h"
#include "com_string.h"

#define MAX_CMD_BUFFER  16384
#define MAX_CMD_LINE    1024

typedef struct {
    byte    *data;
    int maxsize;
    int cursize;
} cmd_t;

int cmd_wait;
cmd_t   cmd_text;
byte    cmd_text_buf[MAX_CMD_BUFFER];

/**
 * Delay execution of the remainder of the command buffer until next frame.
 * @note This allows commands like:
 * bind g "cmd use rocket ; +attack ; wait ; -attack ; cmd use blaster"
 */
void Cmd_Wait_f (void)
{
    if (Cmd_Argc() == 2) {
        cmd_wait = atoi (Cmd_Argv (1));
    } else {
        cmd_wait = 1;
    }
}

/**
 * @defgroup COMMAND BUFFER
 */

/**
 * Initialize command buffer.
 * @ingroup COMMAND BUFFER
 */
void Cbuf_Init (void)
{
    cmd_text.data = cmd_text_buf;
    cmd_text.maxsize = MAX_CMD_BUFFER;
    cmd_text.cursize = 0;
}

/**
 * Add command text at the end of the command buffer.
 * @param text Pointer to the command.
 * @note Does NOT add a final \n.
 * @ingroup COMMAND BUFFER
 */
void Cbuf_AddText (const char *text)
{
    int     l;

    l = strlen (text);

    if (cmd_text.cursize + l >= cmd_text.maxsize) {
        return;
    }

    //Com_Memcpy(&cmd_text.data[cmd_text.cursize], text, l);
    memcpy (&cmd_text.data[cmd_text.cursize], text, l);
    cmd_text.cursize += l;
}

/**
 * Add command text immediately after the current command in the command buffer.
 * @param text Pointer to the command.
 * @note Adds a \n at the end of text.
 * @ingroup COMMAND BUFFER
 */
void Cbuf_InsertText (const char *text)
{
    int     len;
    int     i;

    len = strlen (text) + 1;

    if (len + cmd_text.cursize > cmd_text.maxsize) {
        return;
    }

    // move the existing command text
    for (i = cmd_text.cursize - 1 ; i >= 0 ; i--) {
        cmd_text.data[ i + len ] = cmd_text.data[ i ];
    }

    // copy the new text in
    //Com_Memcpy( cmd_text.data, text, len - 1 );
    memcpy (cmd_text.data, text, len - 1);

    // add a \n
    cmd_text.data[ len - 1 ] = '\n';

    cmd_text.cursize += len;
}

/**
 * Execute command text at exec_when.
 * @param exec_when Desired time of execution. If EXEC_NOW, execute right away.
 * If EXEC_INSERT, execute it after the current command. If EXEC_APPEND, append
 * at the end of command buffer. Puts out a fatal error if an invalid value is given.
 * @param text Pointer to the command.
 * @ingroup COMMAND BUFFER
 */
void Cbuf_ExecuteText (int exec_when, const char *text)
{
    switch (exec_when) {
    case EXEC_NOW:
        if (text && strlen (text) > 0) {
            Cmd_ExecuteString (text);
        } else {
            Cbuf_Execute();
        }

        break;

    case EXEC_INSERT:
        Cbuf_InsertText (text);
        break;

    case EXEC_APPEND:
        Cbuf_AddText (text);
        break;

    default:
        Com_Error (ERR_FATAL, "Cbuf_ExecuteText: bad exec_when");
    }
}

/**
 * Execute all commands in the command buffer.
 * @ingroup COMMAND BUFFER
 */
void Cbuf_Execute (void)
{
    int i;
    char    *text;
    char    line[MAX_CMD_LINE];
    int quotes;

    while (cmd_text.cursize) {
        if (cmd_wait)  {
            // skip out while text still remains in buffer, leaving it
            // for next frame
            cmd_wait--;
            break;
        }

        // find a \n or ; line break
        text = (char *)cmd_text.data;

        quotes = 0;

        for (i = 0 ; i < cmd_text.cursize ; i++) {
            if (text[i] == '"')
                quotes++;

            if (! (quotes & 1) &&  text[i] == ';')
                break;  // don't break if inside a quoted string

            if (text[i] == '\n' || text[i] == '\r')
                break;
        }

        if (i >= (MAX_CMD_LINE - 1)) {
            i = MAX_CMD_LINE - 1;
        }
        memcpy (line, text, i);
        line[i] = 0;

// delete the text from the command buffer and move remaining commands down
// this is necessary because commands (exec) can insert data at the
// beginning of the text buffer

        if (i == cmd_text.cursize)
            cmd_text.cursize = 0;
        else {
            i++;
            cmd_text.cursize -= i;
            memmove (text, text + i, cmd_text.cursize);
        }
        Cmd_ExecuteString (line);
    }
}


/**
 * @defgroup SCRIPT COMMANDS
 */

/**
 * Execute script given as argument to exec command.
 * @ingroup SCRIPT COMMANDS
 */
void Cmd_Exec_f (void)
{
    char        *f;
    int     len;
    char        filename[MAX_OSPATH];
    filehandle_t    *scriptFileHandle;

    if (Cmd_Argc() != 2) {
        return;
    }


    com_strlcpy (filename, Cmd_Argv (1), sizeof (filename));
    FS_DefaultExtension (filename, sizeof (filename), ".cfg");

    scriptFileHandle = FS_OpenFile (filename, DA_GENERIC_READ);
    len = FS_GetFileSize (scriptFileHandle);

    f = (char *)Z_Malloc (len);

    FS_ReadFile (f, 1, len, scriptFileHandle);

    if (!f) {
        return;
    }

    Cbuf_InsertText (f);

    Z_Free (f);
    FS_CloseFile (scriptFileHandle);
}

/**
 * Insert the current value of a variable as next instruction in the command buffer.
 * @ingroup SCRIPT COMMANDS
 */
void Cmd_Vstr_f (void)
{
    char    *v;

    if (Cmd_Argc() != 2) {
        return;
    }

    v = Cvar_VariableString (Cmd_Argv (1));
    Cbuf_InsertText (va ("%s\n", v));
}

/**
 * Print the rest of the line to the console.
 * @ingroup SCRIPT COMMANDS
 */
void Cmd_Echo_f (void)
{
}

/**
 * @defgroup COMMAND EXECUTION
 */

typedef struct cmd_function_s {
    struct cmd_function_s   *next;
    char            *name;
    xcommand_t      function;
} cmd_function_t;


static int  cmd_argc;
static char *cmd_argv[MAX_STRING_TOKENS]; // points into cmd_tokenized
static char cmd_tokenized[BIG_INFO_STRING + MAX_STRING_TOKENS]; // will have 0 bytes inserted
static char cmd_cmd[BIG_INFO_STRING]; // the original command we received (no token processing)

static cmd_function_t *cmd_functions; // possible commands to execute

/**
 * Return argument count for the command.
 * @return Argument count.
 * @ingroup COMMAND EXECUTION
 */
int Cmd_Argc (void)
{
    return cmd_argc;
}

/**
 * Return a pointer to the argument with index arg for the command.
 * @param[in] arg Argument's index.
 * @return Pointer to argument with index arg.
 * @ingroup COMMAND EXECUTION
 */
char *Cmd_Argv (int arg)
{
    if ((unsigned)arg >= cmd_argc) {
        return "";
    }

    return cmd_argv[arg];
}

/*
============
Cmd_TokenizeString

Parses the given string into command line tokens.
The text is copied to a seperate buffer and 0 characters
are inserted in the apropriate place, The argv array
will point into this temporary buffer.
============
*/
/* NOTE TTimo define that to track tokenization issues */
//#define TKN_DBG
void Cmd_TokenizeString (const char *text_in)
{
    const char  *text;
    char        *textOut;

    // clear previous args
    cmd_argc = 0;

    if (!text_in) {
        return;
    }

    com_strlcpy (cmd_cmd, text_in, sizeof (cmd_cmd));

    text = text_in;
    textOut = cmd_tokenized;

    while (1) {
        if (cmd_argc == MAX_STRING_TOKENS) {
            return;         // this is usually something malicious
        }

        while (1) {
            // skip whitespace
            while (*text && *text <= ' ') {
                text++;
            }

            if (!*text) {
                return;         // all tokens parsed
            }

            // skip // comments
            if (text[0] == '/' && text[1] == '/') {
                return;         // all tokens parsed
            }

            // skip /* */ comments
            if (text[0] == '/' && text[1] == '*') {
                while (*text && (text[0] != '*' || text[1] != '/')) {
                    text++;
                }

                if (!*text) {
                    return;     // all tokens parsed
                }

                text += 2;
            } else {
                break;          // we are ready to parse a token
            }
        }

        // handle quoted strings
        // NOTE TTimo this doesn't handle \" escaping
        // NOTE Mario text[0] != '\' && (text[0] == '"' || text[1] == '"') or something similar may handle it.
        if (*text == '"') {
            cmd_argv[cmd_argc] = textOut;
            cmd_argc++;
            text++;

            while (*text && *text != '"') {
                *textOut++ = *text++;
            }

            *textOut++ = 0;

            if (!*text) {
                return;     // all tokens parsed
            }

            text++;
            continue;
        }

        // regular token
        cmd_argv[cmd_argc] = textOut;
        cmd_argc++;

        // skip until whitespace, quote, or command
        while (*text > ' ') {
            if (text[0] == '"') {
                break;
            }

            if (text[0] == '/' && text[1] == '/') {
                break;
            }

            // skip /* */ comments
            if (text[0] == '/' && text[1] == '*') {
                break;
            }

            *textOut++ = *text++;
        }

        *textOut++ = 0;

        if (!*text) {
            return;     // all tokens parsed
        }
    }

}

/*
============
Cmd_AddCommand
============
*/
void Cmd_AddCommand (const char *cmd_name, xcommand_t function)
{
    cmd_function_t  *cmd;

    // fail if the command already exists
    for (cmd = cmd_functions ; cmd ; cmd = cmd->next) {
        if (!strcmp (cmd_name, cmd->name)) {
            return;
        }
    }

    // use a small malloc to avoid zone fragmentation
    //cmd = S_Malloc (sizeof(cmd_function_t));
    cmd = Z_Malloc (sizeof (cmd_function_t));
    cmd->name = com_strcopy (cmd_name);
    cmd->function = function;
    cmd->next = cmd_functions;
    cmd_functions = cmd;
}

/*
============
Cmd_RemoveCommand
============
*/
void    Cmd_RemoveCommand (const char *cmd_name)
{
    cmd_function_t  *cmd, **back;

    back = &cmd_functions;

    while (1) {
        cmd = *back;

        if (!cmd) {
            // command wasn't active
            return;
        }

        if (!strcmp (cmd_name, cmd->name)) {
            *back = cmd->next;

            if (cmd->name) {
                Z_Free (cmd->name);
            }

            Z_Free (cmd);
            return;
        }

        back = &cmd->next;
    }
}

/*
============
Cmd_CommandCompletion
============
*/
void Cmd_CommandCompletion (void (*callback) (const char *s))
{
    cmd_function_t  *cmd;

    for (cmd = cmd_functions ; cmd ; cmd = cmd->next) {
        callback (cmd->name);
    }
}

/*
============
Cmd_ExecuteString

A complete command line has been parsed, so try to execute it
============
*/
void Cmd_ExecuteString (const char *text)
{
    cmd_function_t  *cmd, **prev;

    // execute the command line
    Cmd_TokenizeString (text);

    if (!Cmd_Argc()) {
        return;     // no tokens
    }

    // check registered command functions
    for (prev = &cmd_functions ; *prev ; prev = &cmd->next) {
        cmd = *prev;

        if (!com_stricmp (cmd_argv[0], cmd->name)) {
            // rearrange the links so that the command will be
            // near the head of the list next time it is used
            *prev = cmd->next;
            cmd->next = cmd_functions;
            cmd_functions = cmd;

            // perform the action
            if (!cmd->function) {
                // let the cgame or game handle it
                break;
            } else {
                cmd->function();
            }

            return;
        }
    }

    // check cvars
    if (Cvar_Command()) {
        return;
    }

}

/*
============
Cmd_List_f
============
*/
void Cmd_List_f (void)
{
    cmd_function_t  *cmd;
    int     i;
    char        *match;

    if (Cmd_Argc() > 1) {
        match = Cmd_Argv (1);
    } else {
        match = NULL;
    }

    i = 0;

    for (cmd = cmd_functions ; cmd ; cmd = cmd->next) {
        if (match && !Com_Filter (match, cmd->name, false))
            continue;
        i++;
    }
}

/*
============
Cmd_Init
============
*/
void Cmd_Init (void)
{
    Cmd_AddCommand ("cmdlist", Cmd_List_f);
    Cmd_AddCommand ("exec", Cmd_Exec_f);
    Cmd_AddCommand ("vstr", Cmd_Vstr_f);
    Cmd_AddCommand ("echo", Cmd_Echo_f);
    Cmd_AddCommand ("wait", Cmd_Wait_f);
}

