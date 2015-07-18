#ifndef __INPUT_CONTEXT_H__
#define __INPUT_CONTEXT_H__

#include <collectc/hashtable.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct input_context_s {
    HashTable *key_maps;
    bool use_scancodes;
} InputContext;

typedef struct button_map_s {
    uint32_t key;
    bool repeat;
    void (*on_key_down) (void);
    void (*on_key_up) (void);
} ButtonMap;


InputContext *icontext_new         (bool use_scancodes);

ButtonMap    *icontext_get_key_map (InputContext *c, uint32_t key);
void          icontext_add_key_map (InputContext *c, ButtonMap *map);

ButtonMap    *button_map_new        (uint32_t key, bool repeat, void (*on_key_down) (void), void (*on_key_up) (void));


#endif
