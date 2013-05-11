#ifndef __FINDFIRST_H__

#define __FINDFIRST_H__

typedef struct _finddata_t

{

    char* name;

}_finddata_t;

int* _findfirst(const char* dir, _finddata_t* fileinfo);

int _findnext(int* ihandle, _finddata_t* fileinfo);

void _findclose(int* ihandle);

/* --> other things needed from M$ */

#ifndef _MAX_PATH

#define _MAX_PATH 255

#endif

/* --> other things needed from M$ */

#endif
