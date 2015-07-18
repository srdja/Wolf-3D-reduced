#include <SDL2/SDL.h>
#include "input_context.h"

InputContext *icontext_new(bool use_scancodes)
{
    InputContext *c = malloc(sizeof(InputContext));

    if (!c)
        return NULL;

    HashTableConf conf;
    hashtable_conf_init(&conf);

    conf.key_compare = CMP_POINTER;
    conf.key_length  = KEY_LENGTH_POINTER;
    conf.hash        = POINTER_HASH;

    HashTable *keys  = hashtable_new_conf(&conf);

    if (!keys)
        return NULL;

    c->key_maps      = keys;
    c->use_scancodes = use_scancodes;

    return c;
}

void icontext_add_key_map(InputContext *c, ButtonMap *map)
{
    hashtable_add(c->key_maps, (void*) map->key, map);
}

ButtonMap *icontext_get_key_map (InputContext *c, uint32_t key)
{
    return hashtable_get(c->key_maps, key);
}

ButtonMap *button_map_new(uint32_t key,
                          bool on_repeat,
                          void (*on_key_down)(void),
                          void (*on_key_up) (void))
{
    ButtonMap *map = calloc(1, sizeof(ButtonMap));
  
    map->key         = key;
    map->repeat      = on_repeat;
    map->on_key_down = on_key_down;
    map->on_key_up   = on_key_up;

    return map;
}