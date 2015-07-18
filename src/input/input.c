
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdint.h>
#include <collectc/hashtable.h>

#include "input.h"

static void process_key_event(InputContext *context, SDL_Event *e);

static HashTable    *contexts   = NULL;
static InputContext *active_cnt = NULL;

bool input_init()
{
    contexts = hashtable_new();
    if (contexts == NULL)
        return false;
    return true;
}

void input_set_context(char *name)
{
    InputContext *ic = hashtable_get(contexts, name);
    if (ic)
        active_cnt = ic;
}

void input_add_context(InputContext *c, char *name)
{
    hashtable_add(contexts, name, c);
}

void input_poll()
{
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_WINDOWEVENT:
            break;
        case SDL_QUIT:
            SDL_Quit();
            exit(0);
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
            break;
        case SDL_KEYDOWN:
        case SDL_KEYUP:
            process_key_event(active_cnt, &event);
            break;
        case SDL_MOUSEMOTION:
            break;
        case SDL_MOUSEWHEEL:
            break;
        default:
            break;
        }
    }
}

static void process_key_event(InputContext *context, SDL_Event *event)
{
    if (context == NULL)
        return;

    uint8_t  repeat = event->key.repeat;
    uint32_t key;

    if (context->use_scancodes)
        key = event->key.keysym.scancode;
    else
        key = event->key.keysym.sym;

    ButtonMap *map = icontext_get_key_map(context, key);

    if (!map)
        return;

    if (event->key.state == SDL_RELEASED && map->on_key_up != NULL)
        map->on_key_up();
    else if (!repeat || (repeat && map->repeat))
        map->on_key_down();
}