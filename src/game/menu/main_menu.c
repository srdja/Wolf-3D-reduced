//
// Created by srdja on 7/24/15.
//

#include <SDL_keycode.h>
#include "main_menu.h"
#include "../../input/input.h"

static Menu *menu;
static bool is_active;

static void cursor_up()
{
    menu->cursor_position = menu->cursor_position == 1 ?
                            menu->num_items : menu->cursor_position - 1;

    // TODO play sound
}

static void cursor_down()
{
    menu->cursor_position = menu->cursor_position == menu->num_items ?
                            1 : menu->cursor_position + 1;

    // TODO play sound
}

static void confirm()
{
    switch (menu->cursor_position) {
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        case 6:
            break;
        default:
            return;
    }
    is_active = false;
}

static void menu_input_init()
{
    ButtonMap *confirm     = button_map_new(SDLK_RETURN, false, NULL, NULL);
    ButtonMap *cursor_up   = button_map_new(SDLK_UP, false, cursor_up, NULL);
    ButtonMap *cursor_down = button_map_new(SDLK_DOWN, false, cursor_down, NULL);

    InputContext *main_menu = icontext_new(false);

    icontext_add_key_map(main_menu, cursor_down);
    icontext_add_key_map(main_menu, cursor_up);
    icontext_add_key_map(main_menu, confirm);

    input_add_context(main_menu, "main_menu");
}


Menu *main_menu_activate()
{
    input_set_context("main_menu");



    return menu;
}

void main_menu_init()
{

}