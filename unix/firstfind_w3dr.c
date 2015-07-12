#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>


#include "glob.h"
#include "../env/common.h"
#include "../env/com_string.h"
#include "../env/platform.h"
#include "../env/common_utils.h"

#include "../env/filesystem.h"


PRIVATE char    findbase[ 128 ];
PRIVATE char    findpath[ 128 ];
PRIVATE char    findpattern[ 128 ];
PRIVATE DIR     *fdir;

PUBLIC char *FS_FindFirst (const char *path, W32 musthave, W32 canthave)
{
    struct dirent *d;
    char *p;

    if (fdir) {
        Com_DPrintf ("FS_FindFirst without close\n");

        return NULL;
    }

    com_strlcpy (findpattern, FS_getFileName (path), sizeof (findpattern));

    FS_getPath (path, findbase, 1024);

    if (! *findbase) {
        if ((fdir = opendir (".")) == NULL) {
            return NULL;
        }
    } else {
        if ((fdir = opendir (findbase)) == NULL) {
            return NULL;
        }
    }

    while ((d = readdir (fdir)) != NULL) {
        if (! *findpattern || glob_match (findpattern, d->d_name)) {
            if (! *findbase) {
                com_strlcpy (findpath, d->d_name, sizeof (findpath));
            } else {
                com_snprintf (findpath, sizeof (findpath), "%s/%s", findbase, d->d_name);
            }

            if (CompareAttributes (findpath, musthave, canthave)) {
                return findpath;
            }
        }
    }

    return NULL;

}

/**
 * \brief Continues a file search from a previous call to the FS_FindFirst function.
 * \param[in] musthave File or directory must have these attributes.
 * \param[in] canthave File or directory can not have these attributes.
 * \return On success string of file name or directory, otherwise NULL.
 */
PUBLIC char *FS_FindNext (W32 musthave, W32 canthave)
{
    struct dirent *d;

    if (fdir == NULL) {
        return NULL;
    }

    while ((d = readdir (fdir)) != NULL) {
        if (! *findpattern || glob_match (findpattern, d->d_name)) {
            if (! *findbase) {
                com_strlcpy (findpath, d->d_name, sizeof (findpath));
            } else {
                com_snprintf (findpath, sizeof (findpath), "%s/%s", findbase, d->d_name);
            }

            if (CompareAttributes (findpath, musthave, canthave)) {
                return findpath;
            }
        }
    }

    return NULL;
}

/**
 * \brief Closes the search handle.
 */
PUBLIC void FS_FindClose (void)
{
    if (fdir) {
        closedir (fdir);
    }

    fdir = NULL;
}

