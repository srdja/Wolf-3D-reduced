#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <glob.h>
//#include "../env/platform.h"
//#include "../env/common_utils.h"
//#include "../env/filesystem.h"
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

char        **findresults = NULL;
int     findresults_amount = 0;
static int  findcounter = 0;

/**
 * \brief Compare directory and file attributes.
 * \param[in] found Specifies the file attributes of the file found.
 * \param[in] musthave File or directory must have these attributes.
 * \param[in] canthave File or directory can not have these attributes.
 * \return On success true, otherwise false.
 */
/*static _boolean CompareAttributes( const char *path, W32 musthave, W32 canthave )*/
/*{*/
/*  struct stat st;*/
/*  _boolean ret_val;*/

/*  if( stat( path, &st ) == -1 )*/
/*      ret_val = false;*/
/*  else if( ( st.st_mode & S_IFDIR ) && ( canthave & FA_DIR ) )*/
/*      ret_val = false;*/
/*  else if( ( musthave & FA_DIR ) && !( st.st_mode & S_IFDIR ) )*/
/*      ret_val = false;*/
/*  else ret_val = true;*/
/*  return ret_val;*/
/*}*/

/**
 * \brief Searches a directory for a file.
 * \param[in] path Pointer to a NUL-terminated string that specifies a valid directory or path and file name.
 * \param[in] musthave File or directory must have these attributes.
 * \param[in] canthave File or directory can not have these attributes.
 * \return On success string of file name or directory, otherwise NULL.
 */
char *FS_FindFirst (const char *path, int musthave, int canthave)
{
    struct glob_t *findhandle;

    if (glob (*path, 0, NULL, findhandle)) {
        findresults_amount = & (&findhandle->gl_pathc);
        findresults = & (&findhandle->gl_pathv);
    }
}

/**
 * \brief Continues a file search from a previous call to the FS_FindFirst function.
 * \param[in] musthave File or directory must have these attributes.
 * \param[in] canthave File or directory can not have these attributes.
 * \return On success string of file name or directory, otherwise NULL.
 */
char *FS_FindNext (int musthave, int canthave)
{
    char *findpath = NULL;
    findcounter++;
//  if( (findcounter < findresults->gl_pathc) && ( FS_CompareAttributes( findresults->gl_pathv[findcounter], musthave, canthave ) ) ){
    findpath = &findresults[findcounter];
//  }
    return findpath;
}

/**
 * \brief Closes the search handle.
 */
void FS_FindClose (void)
{
}
