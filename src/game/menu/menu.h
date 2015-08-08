//
// Created by srdja on 7/24/15.
//

#ifndef WOLF3D_REDUX_MENU_H
#define WOLF3D_REDUX_MENU_H

#include <stdbool.h>

typedef struct menu_s Menu;

struct menu_s {
    int num_items;
    int cursor_position;

    bool is_active;

    void (*draw)       (Menu *m);
    void (*activate)   (Menu *m);
    void (*deactivate) (Menu *m);
};

#endif //WOLF3D_REDUX_MENU_H
