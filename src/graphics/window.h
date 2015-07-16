//
// Created by srdja on 7/16/15.
//

#ifndef WOLF3D_REDUX_WINDOW_H
#define WOLF3D_REDUX_WINDOW_H

#include <stdbool.h>

bool window_init(int w, int h, bool fs);
bool window_set_fullscreen();
bool window_set_fullscreen_windowed();
bool window_set_windowed(int w, int h);

void window_buffer_swap();

#endif //WOLF3D_REDUX_WINDOW_H
