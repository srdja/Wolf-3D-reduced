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
 * \file cvar.c
 * \brief Dynamic variable tracking.
 * \author Id Software, Inc.
 * \date 1999-2005
 * \note This code was derived from Quake III Arena, and was originally written by Id Software, Inc.
 */

#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "com_string.h"

cvar_t      *cvar_vars;
cvar_t      *cvar_cheats;
int         cvar_modifiedFlags;

#define MAX_CVARS   1024
cvar_t      cvar_indexes[MAX_CVARS];
int         cvar_numIndexes;

#define FILE_HASH_SIZE      256
static  cvar_t     *hashTable[FILE_HASH_SIZE];

cvar_t *Cvar_Set2 (const char *var_name, const char *value, _boolean force);

/*
================
return a hash value for the filename
================
*/
PRIVATE long generateHashValue (const char *fname)
{
    int     i;
    long    hash;
    char    letter;

    hash = 0;
    i = 0;

    while (fname[i] != '\0') {
        letter = TOLOWER (fname[i]);
        hash += (long) (letter) * (i + 119);
        i++;
    }

    hash &= (FILE_HASH_SIZE - 1);
    return hash;
}

/*
============
Cvar_ValidateString
============
*/
PRIVATE _boolean Cvar_ValidateString (const char *s)
{
    if (!s) {
        return false;
    }

    if (strchr (s, '\\')) {
        return false;
    }

    if (strchr (s, '\"')) {
        return false;
    }

    if (strchr (s, ';')) {
        return false;
    }

    return true;
}

/*
============
Cvar_FindVar
============
*/
PRIVATE cvar_t *Cvar_FindVar (const char *var_name)
{
    cvar_t  *var;
    long hash;

    hash = generateHashValue (var_name);

    for (var = hashTable[hash] ; var ; var = var->hashNext) {
        if (!com_stricmp (var_name, var->name)) {
            return var;
        }
    }

    return NULL;
}

/*
============
Cvar_VariableValue
============
*/
float Cvar_VariableValue (const char *var_name)
{
    cvar_t  *var;

    var = Cvar_FindVar (var_name);

    if (!var)
        return 0;

    return var->value;
}

/*
============
Cvar_VariableString
============
*/
char *Cvar_VariableString (const char *var_name)
{
    cvar_t *var;

    var = Cvar_FindVar (var_name);

    if (!var)
        return "";

    return var->string;
}

/*
============
Cvar_Get

If the variable already exists, the value will not be set unless CVAR_ROM
The flags will be or'ed in if the variable exists.
============
*/
cvar_t *Cvar_Get (const char *var_name, const char *var_value, int flags)
{
    cvar_t  *var;
    long    hash;

    if (!Cvar_ValidateString (var_name)) {
        var_name = "BADNAME";
    }

    var = Cvar_FindVar (var_name);

    if (var) {
        // if the C code is now specifying a variable that the user already
        // set a value for, take the new value as the reset value
        if ((var->flags & CVAR_USER_CREATED) && ! (flags & CVAR_USER_CREATED)
                && var_value[0]) {
            var->flags &= ~CVAR_USER_CREATED;
            free(var->resetString);
            var->resetString = com_strcopy (var_value);

            // ZOID--needs to be set so that cvars the game sets as
            // SERVERINFO get sent to clients
            cvar_modifiedFlags |= flags;
        }

        var->flags |= flags;

        // only allow one non-empty reset string without a warning
        if (!var->resetString[0]) {
            // we don't have a reset string yet
            free(var->resetString);
            var->resetString = com_strcopy (var_value);
        } else if (var_value[0] && strcmp (var->resetString, var_value)) {
            Com_DPrintf ("Warning: cvar \"%s\" given initial values: \"%s\" and \"%s\"\n",
                         var_name, var->resetString, var_value);
        }

        // if we have a latched string, take that value now
        if (var->latchedString) {
            char *s;

            s = var->latchedString;
            var->latchedString = NULL;  // otherwise cvar_set2 would free it
            Cvar_Set2 (var_name, s, true);
            free (s);
        }

        return var;
    }

    var = &cvar_indexes[cvar_numIndexes];
    cvar_numIndexes++;
    var->name = com_strcopy (var_name);
    var->string = com_strcopy (var_value);
    var->modified = true;
    var->modificationCount = 1;
    var->value = (float)atof (var->string);
    var->integer = atoi (var->string);
    var->resetString = com_strcopy (var_value);

    // link the variable in
    var->next = cvar_vars;
    cvar_vars = var;

    var->flags = flags;

    hash = generateHashValue (var_name);
    var->hashNext = hashTable[hash];
    hashTable[hash] = var;

    return var;
}

/*
============
Cvar_Set2
============
*/
cvar_t *Cvar_Set2 (const char *var_name, const char *value, _boolean force)
{
    cvar_t  *var;

    Com_DPrintf ("Cvar_Set2: %s %s\n", var_name, value);

    if (!Cvar_ValidateString (var_name)) {
        var_name = "BADNAME";
    }

    var = Cvar_FindVar (var_name);

    if (!var) {
        if (!value) {
            return NULL;
        }

        // create it
        if (!force) {
            return Cvar_Get (var_name, value, CVAR_USER_CREATED);
        } else {
            return Cvar_Get (var_name, value, 0);
        }
    }

    if (!value) {
        value = var->resetString;
    }

    if (!strcmp (value, var->string)) {
        return var;
    }

    // note what types of cvars have been modified (userinfo, archive, serverinfo, systeminfo)
    cvar_modifiedFlags |= var->flags;

    if (!force) {
        if (var->flags & CVAR_ROM) {
            return var;
        }

        if (var->flags & CVAR_INIT) {
            return var;
        }

        if (var->flags & CVAR_LATCH) {
            if (var->latchedString) {
                if (strcmp (value, var->latchedString) == 0)
                    return var;

                free (var->latchedString);
            } else {
                if (strcmp (value, var->string) == 0)
                    return var;
            }
            var->latchedString = com_strcopy (value);
            var->modified = true;
            var->modificationCount++;
            return var;
        }

        if ((var->flags & CVAR_CHEAT) && !cvar_cheats->integer) {
            return var;
        }

    } else {
        if (var->latchedString) {
            free (var->latchedString);
            var->latchedString = NULL;
        }
    }

    if (!strcmp (value, var->string))
        return var;     // not changed

    var->modified = true;
    var->modificationCount++;

    free (var->string);   // free the old value string

    var->string = com_strcopy (value);
    var->value = (float) atof (var->string);
    var->integer = atoi (var->string);

    return var;
}

/*
============
Cvar_Set
============
*/
void Cvar_Set (const char *var_name, const char *value)
{
    Cvar_Set2 (var_name, value, true);
}

/*
============
Cvar_SetValue
============
*/
void Cvar_SetValue (const char *var_name, float value)
{
    char    val[32];

    if (value == (int)value) {
        com_snprintf (val, sizeof (val), "%i", (int)value);
    } else {
        com_snprintf (val, sizeof (val), "%f", value);
    }

    Cvar_Set (var_name, val);
}


/*
============
Cvar_Reset
============
*/
void Cvar_Reset (const char *var_name)
{
    Cvar_Set2 (var_name, NULL, false);
}

/*
============
Cvar_Command

Handles variable inspection and changing from the console
============
*/
_boolean Cvar_Command (void)
{
    cvar_t          *v;

    // check variables
    v = Cvar_FindVar (Cmd_Argv (0));

    if (!v) {
        return false;
    }

    // set the value if forcing isn't required
    Cvar_Set2 (v->name, Cmd_Argv (1), false);
    return true;
}


/*
============
Cvar_Toggle_f

Toggles a cvar for easy single key binding
============
*/
void Cvar_Toggle_f (void)
{
    int     v;

    if (Cmd_Argc() != 2) {
        return;
    }

    v = (int)Cvar_VariableValue (Cmd_Argv (1));
    v = !v;

    Cvar_Set2 (Cmd_Argv (1), va ("%i", v), false);
}

/*
============
Cvar_Set_f

Allows setting and defining of arbitrary cvars from console, even if they
weren't declared in C code.
============
*/
void Cvar_Set_f (void)
{
    int     i, c, l, len;
    char    combined[MAX_STRING_TOKENS];

    c = Cmd_Argc();

    if (c < 3) {
        return;
    }

    combined[0] = 0;
    l = 0;

    for (i = 2 ; i < c ; i++) {
        len = strlen (Cmd_Argv (i) + 1);

        if (l + len >= MAX_STRING_TOKENS - 2) {
            break;
        }

        com_strlcat (combined, Cmd_Argv (i), sizeof (combined));

        if (i != c - 1) {
            com_strlcat (combined, " ", sizeof (combined));
        }

        l += len;
    }

    Cvar_Set2 (Cmd_Argv (1), combined, false);
}

/*
============
Cvar_SetU_f

As Cvar_Set, but also flags it as userinfo
============
*/
void Cvar_SetU_f (void)
{
    cvar_t  *v;

    if (Cmd_Argc() != 3) {
        return;
    }

    Cvar_Set_f();
    v = Cvar_FindVar (Cmd_Argv (1));

    if (!v) {
        return;
    }

    v->flags |= CVAR_USERINFO;
}

/*
============
Cvar_SetS_f

As Cvar_Set, but also flags it as userinfo
============
*/
void Cvar_SetS_f (void)
{
    cvar_t  *v;

    if (Cmd_Argc() != 3) {
        return;
    }

    Cvar_Set_f();
    v = Cvar_FindVar (Cmd_Argv (1));

    if (!v) {
        return;
    }

    v->flags |= CVAR_SERVERINFO;
}

/*
============
Cvar_SetA_f

As Cvar_Set, but also flags it as archived
============
*/
void Cvar_SetA_f (void)
{
    cvar_t  *v;

    if (Cmd_Argc() != 3) {
        return;
    }

    Cvar_Set_f();
    v = Cvar_FindVar (Cmd_Argv (1));

    if (!v) {
        return;
    }

    v->flags |= CVAR_ARCHIVE;
}


/*
============
Cvar_WriteVariables

Appends lines containing "set variable value" for all variables
with the archive flag set to true.
============
*/
void Cvar_WriteVariables (FILE *f)
{
    cvar_t  *var;
    char    buffer[1024];

    for (var = cvar_vars ; var ; var = var->next) {
        if (com_stricmp (var->name, "cl_cdkey") == 0) {
            continue;
        }

        if (var->flags & CVAR_ARCHIVE) {
            // write the latched value, even if it hasn't taken effect yet
            if (var->latchedString) {
                com_snprintf (buffer, sizeof (buffer), "seta %s \"%s\"\n", var->name, var->latchedString);
            } else {
                com_snprintf (buffer, sizeof (buffer), "seta %s \"%s\"\n", var->name, var->string);
            }

            //FS_Printf (f, "%s", buffer);
            fprintf (f, "%s", buffer);
        }
    }
}

/*
============
Cvar_List_f
============
*/
void Cvar_List_f (void)
{
}

/*
============
Cvar_Restart_f

Resets all cvars to their hardcoded values
============
*/
void Cvar_Restart_f (void)
{
    cvar_t  *var;
    cvar_t  **prev;

    prev = &cvar_vars;

    while (1) {
        var = *prev;

        if (!var) {
            break;
        }

        // don't mess with rom values, or some inter-module
        // communication will get broken (com_cl_running, etc)
        if (var->flags & (CVAR_ROM | CVAR_INIT | CVAR_NORESTART)) {
            prev = &var->next;
            continue;
        }

        // throw out any variables the user created
        if (var->flags & CVAR_USER_CREATED) {
            *prev = var->next;

            if (var->name) {
                free(var->name);
            }

            if (var->string) {
               free(var->string);
            }

            if (var->latchedString) {
                free (var->latchedString);
            }

            if (var->resetString) {
                free (var->resetString);
            }

            // clear the var completely, since we
            // can't remove the index from the list
            //Com_Memset( var, 0, sizeof( var ) );
            memset (var, 0, sizeof (var));
            continue;
        }

        Cvar_Set (var->name, var->resetString);

        prev = &var->next;
    }
}

extern cvar_t *r_ref;
extern cvar_t *vid_gamma;
extern cvar_t *gl_mode;
extern cvar_t *gl_finish;
extern cvar_t *gl_driver;
extern cvar_t *gl_ext_palettedtexture;
extern cvar_t *gl_swapinterval;
 cvar_t *r_fakeFullscreen;
extern cvar_t *in_mouse;
 cvar_t *in_dgamouse;
extern cvar_t *g_fov;
extern cvar_t *g_autoaim;
extern cvar_t *mapScale;;
extern cvar_t *g_version;
extern cvar_t *name;
extern cvar_t *r_fullscreen;
/*
============
Cvar_Init

Reads in all archived cvars
============
*/
PUBLIC void Cvar_Init (void)
{
    cvar_cheats = Cvar_Get ("sv_cheats", "1", CVAR_ROM | CVAR_SYSTEMINFO);

    r_ref                  = Cvar_Get("r_ref", "gl", CVAR_INIT);
    vid_gamma              = Cvar_Get("vid_gamma", "18.299999", CVAR_INIT);
    gl_mode                = Cvar_Get("gl_mode", "1", CVAR_INIT);
    gl_finish              = Cvar_Get("gl_finish", "1", CVAR_INIT);
    gl_driver              = Cvar_Get("gl_driver", "libGL.so.1", CVAR_INIT);
    gl_ext_palettedtexture = Cvar_Get("gl_ext_palettedtexture", "0", CVAR_INIT);
    gl_swapinterval        = Cvar_Get("gl_swapinterval", "1", CVAR_INIT);
    r_fakeFullscreen       = Cvar_Get("r_fakeFullscreen", "0", CVAR_INIT);
    in_mouse               = Cvar_Get("in_mouse", "1", CVAR_INIT);
    in_dgamouse            = Cvar_Get("in_dgamouse", "1", CVAR_INIT);
    g_fov                  = Cvar_Get("g_fov", "68", CVAR_INIT);
    g_autoaim              = Cvar_Get("g_autoaim", "1", CVAR_INIT);
    mapScale               = Cvar_Get("mapScale", "8", CVAR_INIT);
    g_version              = Cvar_Get("g_version", "0", CVAR_INIT);
    name                   = Cvar_Get("name", "wolf", CVAR_INIT);
    r_fullscreen           = Cvar_Get("r_fullscreen", "0", CVAR_INIT);

    //Cmd_AddCommand ("toggle", Cvar_Toggle_f);
    //Cmd_AddCommand ("set", Cvar_Set_f);
    //Cmd_AddCommand ("sets", Cvar_SetS_f);
   // Cmd_AddCommand ("setu", Cvar_SetU_f);
    //Cmd_AddCommand ("seta", Cvar_SetA_f);
    //Cmd_AddCommand ("reset", Cvar_Reset_f);
   // Cmd_AddCommand ("cvarlist", Cvar_List_f);
   // Cmd_AddCommand ("cvar_restart", Cvar_Restart_f);
}
