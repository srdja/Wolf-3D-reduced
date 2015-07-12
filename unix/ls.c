** ** ** **
*ls.c
** ** ** **
/*
        Copyright (C) 2011 by Philippe.Vouters@laposte.net

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by the
    Free Software Foundation, either version 3 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>;.

*/
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#if defined (__VMS)
#include <unixio.h>
#include <unixlib.h>
#elif defined (__unix__) || (__unix)
#include <sys/types.h>
#include <sys/stat.h>
#endif
#include <unistd.h>
#include <string.h>
#include <glob.h>

#ifndef PATH_MAX
#ifdef FILENAME_MAX
#define PATH_MAX FILENAME_MAX
#else
#define PATH_MAX 256
#endif
#endif

#ifndef MAX_PATH
#define MAX_PATH PATH_MAX
#endif

#if defined (__VMS)
const char *path_add = "/*";
#elif defined (__unix__) || defined (__unix)
const char *path_add = "/*";
#endif

typedef struct filnam {
    struct filnam *next;
    char buffer[MAX_PATH];
} filenam_t;

int errfunc (const char *path, int eerrno)
{
    if (eerrno) {
        fprintf (stderr, "error accessing %s; eerrno = %d, errno=%d\n",
                 path, eerrno, errno);
        return eerrno;
    } else
        return 0;
}

filenam_t *store_files (filenam_t **head, filenam_t **first,
                        filenam_t **prev, glob_t *globbuf)
{
    filenam_t *file;
    size_t i;

    for (i = 0; i < globbuf->gl_pathc; i++) {
        file = calloc (1, sizeof (filenam_t));
        file->next = NULL;

        if (!i && first) *first = file;

        if (head && !*head) *head = file;

        strcpy (file->buffer, globbuf->gl_pathv[i]);

        if (prev && *prev) (*prev)->next = file;

        *prev = file;
    }

    return file;
}

#ifdef __VMS
typedef struct {
    char *name;
    int value;
} decc$features_t;

decc$features_t decc$features[] = {
    {"DECC$EFS_CHARSET", 1},
    {"DECC$DISABLE_TO_VMS_LOGNAME_TRANSLATION", 1},
    {"DECC$FILENAME_UNIX_NO_VERSION", 1},
    {"DECC$FILENAME_UNIX_REPORT", 1},
    {"DECC$READDIR_DROPDOTNOTYPE", 1},
    {"DECC$RENAME_NO_INHERIT", 1},
    {"DECC$FILENAME_UNIX_ONLY", 1}
};

void init_vms_crtl (void)
{
    int i, index;

    for (i = 0; i < sizeof (decc$features) / sizeof (decc$features_t); i++) {
        if ((index = decc$feature_get_index (decc$features[i].name)) == -1) {
            perror ("decc$feature_get_index");
            exit (EXIT_FAILURE);
        }

        if (decc$feature_set_value (index, 1, decc$features[i].value) == -1) {
            perror ("decc$feature_set_value");
            exit (EXIT_FAILURE);
        }
    }
}
#endif

int main (int argc, char **argv)
{
    filenam_t *file;
    filenam_t *head = NULL;
    filenam_t *prev = NULL;
    filenam_t *first;
    char pattern[MAX_PATH];
    glob_t globbuf;
    struct stat buf;
    int status;

#ifdef __VMS
    init_vms_crtl();
#endif
    strcpy (pattern, argv[1]);

    if (pattern[strlen (pattern) - 1] == '/') strcat (pattern, "*");

    memset (&globbuf, 0, sizeof (glob_t));
    status = glob (pattern, GLOB_ERR, errfunc, &globbuf);

    if (status) exit (EXIT_FAILURE);

    store_files (&head, &first, &prev, &globbuf);
    globfree (&globbuf);

    do {
        for (file = first; file; file = file->next) {

            if (lstat (file->buffer, &buf)) {
                char perhapsDir[MAX_PATH];

                strcpy (perhapsDir, file->buffer);
                strcat (perhapsDir, "/");

                if (lstat (perhapsDir, &buf)) {
                    perror ("lstat");
                    exit (EXIT_FAILURE);
                }
            }

            printf ("%s", file->buffer);

            if (S_ISLNK (buf.st_mode)) {
                char realfile[MAX_PATH];
                ssize_t size;
                size = readlink (file->buffer, realfile, sizeof (realfile));
                printf (" -> %.*s\n", size, realfile);
            }

            if (S_ISREG (buf.st_mode))
                printf ("\n");

            if (S_ISDIR (buf.st_mode)) {
                printf (":\n");

                if (strcmp (file->buffer, "..") &&
                        strcmp (file->buffer, ".")) {
                    char dirname[MAX_PATH];
                    filenam_t *f;

                    strcpy (dirname, file->buffer);
                    strcat (dirname, path_add);
                    memset (&globbuf, 0, sizeof (glob_t));
                    glob (dirname, 0, NULL, &globbuf);
                    store_files (NULL, NULL, &prev, &globbuf);
                    globfree (&globbuf);
                    break;
                }
            }
        }

        if (file && S_ISDIR (buf.st_mode))
            first = file->next;
    } while (file);

    file = head;

    while (file) {
        prev = file->next;
        free (file);
        file = prev;
    }

    exit (EXIT_SUCCESS);
}
