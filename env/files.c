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

/*!
    \file files.c
    \brief Interface to file i/o layer.
    \author Michael Liebscher
    \date 2004-2012
    \author Id Software, Inc.
    \date 1997-2001
    \note This code was derived from Quake II, and was originally written by Id Software, Inc.
    \note
    This  module accesses data  through a  hierarchal file system, but  the
    contents  of the file  system can be transparently  merged from several
    sources.

    The  "base  directory"  is  the  path  to  the  directory  holding  the
    executable  and  all  game  directories.  The  sys_* files pass this to
    host_init  in  quakeparms_t->basedir.  This  can be overridden with the
    "-basedir"  command  line  parm  to allow code debugging in a different
    directory.  The  base  directory  is  only   used  during  file  system
    initialization.

    The "game directory" is the first tree on the search path and directory
    that  all  generated  files  (save  games, screen  shots, demos, config
    files)  will  be  saved  to.  This  can  be overridden with the "-game"
    command line parameter.   The game directory can never be changed while
    the  application  is  executing.  This is a precaution against having a
    malicious  server  instruct  clients  to  write  files  over areas they
    shouldn't.

*/

#include <string.h>
#include <zlib.h>

#include "common.h"
#include "com_string.h"
#include "zmem.h"
#include "memory.h"
#include "filesystem.h"
#include "filelink.h"

#include "zippak.h"

PRIVATE char fs_gamedir[ MAX_OSPATH ];

PRIVATE cvar_t  *fs_basedir;
PRIVATE cvar_t  *fs_cddir;
PRIVATE cvar_t  *fs_gamedirvar;

/**
 * \brief Add directory to search path.
 * \param[in] dir Game directory path.
 * \note Sets fs_gamedir, adds the directory to the head of the path, then loads and adds *.pak then *.zip files.
 */
PRIVATE void FS_AddGameDirectory (const char *dir)
{
    searchpath_t    *search;
    char        path[ MAX_OSPATH ];
    pack_t      *pak;
    char        *pakfile;

    com_strlcpy (fs_gamedir, dir, sizeof (fs_gamedir));

    //
    // add the directory to the search path
    //
    search = (searchpath_t *) Z_Malloc (sizeof (searchpath_t));
    com_strlcpy (search->filename, dir, sizeof (search->filename));
    search->next = fs_searchpaths;
    fs_searchpaths = search;

    //
    // add any pak files
    //
    com_snprintf (path, sizeof (path), "%s%c*.pak", fs_gamedir, PATH_SEP);

    pakfile = FS_FindFirst (path, 0, 0);

    if (pakfile) {
        pak = FS_LoadZipFile (pakfile);

        if (pak) {
            search = (searchpath_t *) Z_Malloc (sizeof (searchpath_t));
            search->pack = pak;
            search->next = fs_searchpaths;
            fs_searchpaths = search;
        }

        while ((pakfile = FS_FindNext (0, 0)) != NULL) {
            pak = FS_LoadZipFile (pakfile);

            if (! pak) {
                continue;
            }

            search = Z_Malloc (sizeof (searchpath_t));
            search->pack = pak;
            search->next = fs_searchpaths;
            fs_searchpaths = search;
        }
    }
}

/**
 * \brief Get root directory.
 * \return String with the name of the root directory.
 */
PUBLIC char *FS_Gamedir (void)
{
    if (*fs_gamedir) {
        return fs_gamedir;
    } else {
        return BASE_DIRECTORY;
    }
}

/**
 * \brief Execute autoexec script
 */
PUBLIC void FS_ExecAutoexec (void)
{
    char *dir;
    char name[ MAX_GAMEPATH ];

    dir = Cvar_VariableString ("gamedir");

    if (*dir) {
        com_snprintf (name, sizeof (name), "%s%c%s%cautoexec.cfg", fs_basedir->string, PATH_SEP, dir, PATH_SEP);
    } else {
        com_snprintf (name, sizeof (name), "%s%c%s%cautoexec.cfg", fs_basedir->string, PATH_SEP, BASE_DIRECTORY, PATH_SEP);
    }

    if (FS_FindFirst (name, 0, FA_DIR | FA_HIDDEN | FA_SYSTEM)) {
        Cbuf_AddText ("exec autoexec.cfg\n");
    }

    FS_FindClose();
}

/**
 * \brief Sets the gamedir and path to a different directory.
 * \param[in] dir New game directory.
 */
PUBLIC void FS_SetGamedir (char *dir)
{
    searchpath_t    *next;

    if (strstr (dir, "..") || strstr (dir, "/")
            || strstr (dir, "\\") || strstr (dir, ":")) {
        Com_Printf ("Gamedir should be a single filename, not a path\n");
        return;
    }

    //
    // free up any current game dir info
    //
    while (fs_searchpaths != fs_base_searchpaths) {
        if (fs_searchpaths->pack) {
            fclose (fs_searchpaths->pack->handle);
            Z_Free (fs_searchpaths->pack->files);
            Z_Free (fs_searchpaths->pack);
        }

        next = fs_searchpaths->next;
        Z_Free (fs_searchpaths);
        fs_searchpaths = next;
    }

    //
    // flush all data, so it will be forced to reload
    //
//  if (dedicated && !dedicated->value)
//      Cbuf_AddText ("vid_restart\nsnd_restart\n");

    com_snprintf (fs_gamedir, sizeof (fs_gamedir), "%s%c%s", fs_basedir->string, PATH_SEP, dir);

    if (! strcmp (dir, BASE_DIRECTORY) || (*dir == 0)) {
        Cvar_Get ("gamedir", "", CVAR_SERVERINFO | CVAR_ROM);
        Cvar_Get ("game", "", CVAR_LATCH | CVAR_SERVERINFO);
    } else {
        Cvar_Get ("gamedir", dir, CVAR_SERVERINFO | CVAR_ROM);

        if (fs_cddir->string[ 0 ]) {
            FS_AddGameDirectory (va ("%s%c%s", fs_cddir->string, PATH_SEP, dir));
        }

        FS_AddGameDirectory (va ("%s%c%s", fs_basedir->string, PATH_SEP, dir));
    }
}

/**
 * \brief Console callback method to create file links.
 */
PRIVATE void FS_Link_f (void)
{
    filelink_t  *flink, **prev;

    if (Cmd_Argc() != 3) {
        Com_Printf ("Usage: link <from> <to>\n");
        return;
    }

    // see if the link already exists
    prev = &fs_links;

    for (flink = fs_links ; flink ; flink = flink->next) {
        if (! strcmp (flink->from, Cmd_Argv (1))) {
            Z_Free (flink->to);

            if (! strlen (Cmd_Argv (2))) {
                // delete it
                *prev = flink->next;
                Z_Free (flink->from);
                Z_Free (flink);
                return;
            }

            flink->to = com_strcopy (Cmd_Argv (2));
            return;
        }

        prev = &flink->next;
    }

    // create a new link
    flink = Z_Malloc (sizeof (*flink));
    flink->next = fs_links;
    fs_links = flink;
    flink->from = com_strcopy (Cmd_Argv (1));
    flink->fromlength = strlen (flink->from);
    flink->to = com_strcopy (Cmd_Argv (2));
}

/**
 * \brief List files based on criteria.
 * \param[in] findname file name.
 * \param[out] numfiles Number of files found.
 * \param[in] musthave Must have these attributes.
 * \param[in] canthave Cannot have these attributes.
 * \return List of files found
 */
PRIVATE char **FS_ListFiles (char *findname, int *numfiles, unsigned musthave, unsigned canthave)
{
    char *s;
    int nfiles = 0;
    char **list = 0;

    s = FS_FindFirst (findname, musthave, canthave);

    while (s) {
        if (s[strlen (s) - 1] != '.')
            nfiles++;

        s = FS_FindNext (musthave, canthave);
    }

    FS_FindClose();

    if (!nfiles)
        return NULL;

    nfiles++; // add space for a guard
    *numfiles = nfiles;

    list = (char **)MM_MALLOC (sizeof (char *) * nfiles);

    if (list == NULL) {
        MM_OUTOFMEM ("list");
    }

    memset (list, 0, sizeof (char *) * nfiles);

    s = FS_FindFirst (findname, musthave, canthave);
    nfiles = 0;

    while (s) {
        if (s[ strlen (s) - 1 ] != '.') {
            list[ nfiles ] = strdup (s);

            (void)com_strlwr (list[ nfiles ]);

            nfiles++;
        }

        s = FS_FindNext (musthave, canthave);
    }

    FS_FindClose();

    return list;
}

/**
 * \brief List directories and files.
 */
PRIVATE void FS_Dir_f (void)
{
    char    *path = NULL;
    char    findname[1024];
    char    wildcard[1024] = "*.*";
    char    **dirnames;
    int     ndirs;

    if (Cmd_Argc() != 1) {
        com_strlcpy (wildcard, Cmd_Argv (1), sizeof (wildcard));
    }

    while ((path = FS_NextPath (path)) != NULL) {
        char *tmp = findname;

        com_snprintf (findname, sizeof (findname), "%s%c%s", path, PATH_SEP, wildcard);

        while (*tmp != 0) {
            if (*tmp == '\\')
                *tmp = '/';

            tmp++;
        }

        Com_Printf ("Directory of %s\n", findname);
        Com_Printf ("----\n");

        if ((dirnames = FS_ListFiles (findname, &ndirs, 0, 0)) != 0) {
            int i;

            for (i = 0; i < ndirs - 1; i++) {
                if (strrchr (dirnames[i], PATH_SEP))
                    Com_Printf ("%s\n", strrchr (dirnames[i], PATH_SEP) + 1);
                else
                    Com_Printf ("%s\n", dirnames[i]);

                MM_FREE (dirnames[i]);
            }

            MM_FREE (dirnames);
        }

        Com_Printf ("\n");
    };
}

/**
 * \brief List search path and file links.
 */
PRIVATE void FS_Path_f (void)
{
    searchpath_t    *s;
    filelink_t      *l;

    Com_Printf ("Current search path:\n");

    for (s = fs_searchpaths ; s ; s = s->next) {
        if (s == fs_base_searchpaths)
            Com_Printf ("----------\n");

        if (s->pack)
            Com_Printf ("%s (%i files)\n", s->pack->filename, s->pack->numfiles);
        else
            Com_Printf ("%s\n", s->filename);
    }

    Com_Printf ("\nLinks:\n");

    for (l = fs_links ; l ; l = l->next) {
        Com_Printf ("%s : %s\n", l->from, l->to);
    }
}


/**
 * \brief Allows enumerating all of the directories in the search path.
 * \param[in] prevpath Previous path searched
 * \return Next path to search on success, otherwise NULL
 */
PUBLIC char *FS_NextPath (char *prevpath)
{
    searchpath_t    *s;
    char            *prev;

    if (! prevpath) {
        return fs_gamedir;
    }

    prev = fs_gamedir;

    for (s = fs_searchpaths ; s ; s = s->next) {
        if (s->pack) {
            continue;
        }

        if (prevpath == prev) {
            return s->filename;
        }

        prev = s->filename;
    }

    return NULL;
}

/**
 * \brief Initialize file system.
 */
PUBLIC void FS_InitFilesystem (void)
{
    char path[1024];

    Cmd_AddCommand ("path", FS_Path_f);
    Cmd_AddCommand ("link", FS_Link_f);
    Cmd_AddCommand ("dir", FS_Dir_f);

    //
    // basedir <path>
    // allows the game to run from outside the data tree
    //
    fs_basedir = Cvar_Get ("basedir", ".", CVAR_ROM);

    //
    // cddir <path>
    // Logically concatenates the cddir after the basedir. This
    // allows the game to run from outside the data tree.
    //
    fs_cddir = Cvar_Get ("cddir", "", CVAR_ROM);

    if (fs_cddir->string[ 0 ]) {
        com_snprintf (path, sizeof (path), "%s%c%s", fs_cddir->string, PATH_SEP, BASE_DIRECTORY);
        FS_AddGameDirectory (path);
    }

    //
    // start up with BASEDIRNAME by default
    //
    com_snprintf (path, sizeof (path), "%s%c%s", fs_basedir->string, PATH_SEP, BASE_DIRECTORY);
    FS_AddGameDirectory (path);

    // any set gamedirs will be freed up to here
    fs_base_searchpaths = fs_searchpaths;

    // check for game override
    fs_gamedirvar = Cvar_Get ("game", "", CVAR_LATCH | CVAR_SERVERINFO);

    if (fs_gamedirvar->string[ 0 ]) {
        FS_SetGamedir (fs_gamedirvar->string);
    }
}
