//
// Created by srdja on 7/16/15.
//

#include <stdbool.h>
#include <SDL_mixer.h>
#include <stdio.h>

#include "music.h"

Mix_Music *music = NULL;

bool music_play(char *name)
{
    char *res = "./base/music/HITLWLTZ.ogg";

    //Load the music
    music = Mix_LoadMUS(res);

    //If there was a problem loading the music
    if(music == NULL) {
        printf("Unable to load %s!\n", res);
        return false;
    }
    Mix_PlayMusic(music, -1);
    return true;
}

bool music_stop()
{
    music = NULL;
    return true;
}
