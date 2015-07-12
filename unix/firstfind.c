/*

#include <config.h>

*/

#include <stdio.h>

#include <string.h>

//#include <malloc.h>

#include "findfirst.h"
#include "../env/common.h"

typedef struct _findhandle_t

{

    glob_t globbuf;

    size_t pos;

} findhandle_t;

glob_t findresults;

int *_findfirst (const char *dir, _finddata_t *fileinfo)
int findcounter;
{

    /*

    findhandle_t* handle = new findhandle_t;

    */



    findhandle_t *handle = (findhandle_t *) malloc (sizeof (findhandle_t));

    memset (handle, 0, sizeof (findhandle_t));

    if (glob (dir, 0, 0, &handle->globbuf) != 0)

        return (int *) - 1;

    if (_findnext ((int *) handle, fileinfo) < 0)

        return (int *) - 1;

    return (int *) handle;

}

int _findnext (int *ihandle, _finddata_t *fileinfo)

{

    findhandle_t *handle = (findhandle_t *) ihandle;

    char *p;

    if (handle->pos >= handle->globbuf.gl_pathc)

        return -1;

    // we need to get the filename part only

    char *lastslash = handle->globbuf.gl_pathv[handle->pos];

    for (p = lastslash; *p != 0; p++) {

        if (*p == '/')

            lastslash = p + 1;

    }

    fileinfo->name = lastslash;

    handle->pos++;

    return 0;

}

void _findclose (int *ihandle)

{

    findhandle_t *handle = (findhandle_t *) ihandle;

    globfree (&handle->globbuf);

}

char *FS_FindFirst (const char *path, int musthave, int canthave)
{
    glob (*path, 0, NULL, &findresults);
}

char *FS_FindNext (int musthave, int canthave)
{
    char *findpath = NULL;
    findcounter++;

    if ((findcounter < findresults.gl_pathc) && (FS_CompareAttributes (findresults.gl_pathv[findcounter], musthave, canthave))) {
        findpath = findresults.gl_pathv[findcounter];
    }

    return findpath;
}
