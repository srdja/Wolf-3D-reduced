#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL_mixer.h>

#include "sound.h"

bool sound_init()
{
    if (SDL_Init(SDL_INIT_AUDIO) != 0) {
        fprintf(stderr, "Unable to initialize SDL: %s\n", SDL_GetError());
        return false;
    }
    if (Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT, 2, 4096 ) == -1 ) {
        fprintf(stdout, "Error while initializing sound!\n");
        return false;
    }
    return true;
}

