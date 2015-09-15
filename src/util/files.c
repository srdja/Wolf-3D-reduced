
#include <string.h>
#include <SDL_filesystem.h>
#include "../common.h"


char resource_base_path[MAX_OSPATH] = {0};

const char *get_resource_base_path(void)
{
    if (resource_base_path[0] != 0)
        return (const char*) &resource_base_path;

    char *path = SDL_GetBasePath();

    size_t p_len = strlen(path);
    size_t b_len = strlen("base");

    strncpy(resource_base_path, path, p_len);
    strncpy(resource_base_path + p_len, "base", b_len);

    resource_base_path[p_len + b_len] = '\0';

    free(path);

    return (const char*) &resource_base_path;
}