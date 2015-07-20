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

#include "../common.h"
#include "com_string.h"
#include "filelink.h"

static char fs_gamedir[ MAX_OSPATH ];

#define BASE_DIRECTORY "base"

static char  *fs_basedir;
static char  *fs_cddir;
static char  *fs_gamedirvar;

/**
 * \brief Add directory to search path.
 * \param[in] dir Game directory path.
 * \note Sets fs_gamedir, adds the directory to the head of the path, then loads and adds *.pak then *.zip files.
 */
static void FS_AddGameDirectory (const char *dir)
{
    searchpath_t    *search;
    char        path[ MAX_OSPATH ];
    //pack_t      *pak;
    char        *pakfile;

    strncpy(fs_gamedir, dir, sizeof(fs_gamedir));

    //
    // add the directory to the search path
    //
    search = (searchpath_t *) malloc (sizeof (searchpath_t));
    strncpy(search->filename, dir, sizeof(search->filename));
    search->next = fs_searchpaths;
    fs_searchpaths = search;

    //
    // add any pak files
    //
    com_snprintf (path, sizeof (path), "%s%c*.pak", fs_gamedir, PATH_SEP);
}

/**
 * \brief Get root directory.
 * \return String with the name of the root directory.
 */
char *FS_Gamedir (void)
{
    if (*fs_gamedir) {
        return fs_gamedir;
    } else {
        return BASE_DIRECTORY;
    }
}

/**
 * \brief Sets the gamedir and path to a different directory.
 * \param[in] dir New game directory.
 */
void FS_SetGamedir (char *dir)
{
    searchpath_t    *next;

    if (strstr (dir, "..") || strstr (dir, "/")
            || strstr (dir, "\\") || strstr (dir, ":")) {
        return;
    }
    //
    // free up any current game dir info
    //
    while (fs_searchpaths != fs_base_searchpaths) {

        com_snprintf(fs_gamedir, sizeof(fs_gamedir), "%s%c%s", fs_basedir, PATH_SEP, dir);

        if (fs_cddir[0]) {
            FS_AddGameDirectory(va("%s%c%s", fs_cddir, PATH_SEP, dir));
        }

        FS_AddGameDirectory(va("%s%c%s", fs_basedir, PATH_SEP, dir));
    }
}

/**
 * \brief Initialize file system.
 */
void FS_InitFilesystem (void)
{
    char path[1024];

    fs_basedir = ".";
    fs_cddir = "";

    if (fs_cddir[ 0 ]) {
        com_snprintf (path, sizeof (path), "%s%c%s", fs_cddir, PATH_SEP, BASE_DIRECTORY);
        FS_AddGameDirectory (path);
    }
    //
    // start up with BASEDIRNAME by default
    //
    com_snprintf (path, sizeof (path), "%s%c%s", fs_basedir, PATH_SEP, BASE_DIRECTORY);
    FS_AddGameDirectory (path);

    // any set gamedirs will be freed up to here
    fs_base_searchpaths = fs_searchpaths;

    // check for game override
    fs_gamedirvar = "";

    if (fs_gamedirvar[ 0 ]) {
        FS_SetGamedir (fs_gamedirvar);
    }
}