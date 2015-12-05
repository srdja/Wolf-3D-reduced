/*

    Copyright (C) 2004-2012 Michael Liebscher <johnnycanuck@users.sourceforge.net>
    Copyright (C) 1997-2001 Id Software, Inc.
    Copyright (C) 1995 Spencer Kimball and Peter Mattis.
    Copyright (C) 2015 Srđan Panić

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#include <SDL_surface.h>
#include <SDL_image.h>
#include <collectc/hashtable.h>
#include <GL/gl.h>

#include "../common.h"
#include "texture_manager.h"
#include "../util/com_string.h"
#include "opengl_local.h"


#define MAX_TEXTURE_PATH 1024


/* A separate cache for walls an sprites since their IDs collide. */
static HashTable *sprites;
static HashTable *walls;
static HashTable *pictures;

static Texture   *no_texture;
static uint32_t  texture_cache_index;


static Texture  *texture_new_missing(void);
static void      set_filters(TextureType type, Texture *tex);


/**
 * Initializes the texture manager.
 */
void texture_tm_init(void)
{
    HashTableConf table_config;
    hashtable_conf_init(&table_config);

    table_config.hash        = POINTER_HASH;
    table_config.key_compare = CMP_POINTER;
    table_config.key_length  = KEY_LENGTH_POINTER;

    sprites  = hashtable_new_conf(&table_config);
    walls    = hashtable_new_conf(&table_config);

    /* pictures are mapped to string names */
    pictures = hashtable_new();

    if (!sprites || !walls || !pictures) {
        fprintf(stderr, "Failed to initialize texture manager.\n");
        return;
    }
    texture_cache_index = 1;

    no_texture = texture_new_missing();
}

/**
 * Uploads the texture pixel data to video memory.
 */
static void texture_upload(Texture *tex, uint8_t *data) {
    glGenTextures(1, &tex->id);
    glBindTexture(GL_TEXTURE_2D, tex->id);

    glTexImage2D(GL_TEXTURE_2D, 0, tex->bytes_per_pixel, tex->width, tex->height, 0, tex->bytes_per_pixel == 4 ? GL_BGRA : GL_BGR, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, tex->WrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, tex->WrapT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, tex->MinFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, tex->MagFilter);
}

/**
 * Advances the texture cache index.
 */
void texture_cache_advance_index()
{
    texture_cache_index++;
}

/**
 * Bind the texture of id [id]. The texture must already be uploaded to video memory.
 */
void texture_use(int id)
{
    if (gl_state.bound_texture_id == id)
        return;

    gl_state.bound_texture_id = id;
    glBindTexture(GL_TEXTURE_2D, id);
}

/**
 * Sets the texture filters.
 */
static void set_filters(TextureType type, Texture *tex)
{
    switch (type) {
        case TT_Pic:
            tex->WrapS = GL_CLAMP;
            tex->WrapT = GL_CLAMP;
            tex->MinFilter = GL_NEAREST;
            tex->MagFilter = GL_NEAREST;
            break;
        case TT_Wall:
            tex->WrapS = GL_REPEAT;
            tex->WrapT = GL_REPEAT;
            tex->MinFilter = GL_LINEAR;
            tex->MagFilter = GL_LINEAR;
            break;
        case TT_Sprite:
            tex->WrapS = GL_REPEAT;
            tex->WrapT = GL_REPEAT;
            tex->MinFilter = GL_LINEAR;
            tex->MagFilter = GL_NEAREST;
            break;
        default:
            tex->WrapS = GL_CLAMP;
            tex->WrapT = GL_CLAMP;
            tex->MinFilter = GL_NEAREST;
            tex->MagFilter = GL_NEAREST;
            break;
    }
}

/**
 * Loads an image from the disk. The name should contain the name and any
 * subdirectory of base if there is one.
 */
static SDL_Surface *load_from_disk(char *name)
{
    if (!name || !*name)
        return NULL;

    char *base       = SDL_GetBasePath();
    char  path[MAX_TEXTURE_PATH] = {0};

    strncpy(path, base, MAX_TEXTURE_PATH);
    strncat(path, "base/", MAX_TEXTURE_PATH);
    strncat(path, name, MAX_TEXTURE_PATH);

    SDL_Surface *img = IMG_Load(path);

    if (!img) {
        fprintf(stdout, "Unable to load texture: %s\n", path);
        return NULL;
    }
    return img;
}

/**
 * Creates a new "missing" texture.
 */
static Texture *texture_new_missing(void)
{
    uint32_t  color = 0xFD5F00FF;
    size_t    size  = (16 * 16);
    uint32_t *data  = malloc(size * 4);

    int x;
    for (x = 0; x < size; x++)
        data[x] = color;

    Texture *tex = calloc(1, sizeof(Texture));

    if (!tex)
        return no_texture;

    tex->cache_index = texture_cache_index;
    tex->type        = TT_Pic;
    tex->width       = 16;
    tex->height      = 16;
    tex->bytes_per_pixel = 4;

    strncpy(tex->name, "missing", MAX_GAMEPATH);
    set_filters(TT_Pic, tex);
    texture_upload(tex, data);
    free (data);

    return tex;
}

/**
 * Creates and returns a new Texture from the image file [name]. This function also uploads the pixel
 * buffer to video memory. It does not however add it to the texture cache.
 */
static Texture *texture_new(const char *name, TextureType type, uint32_t cache_index)
{
    Texture *tex = calloc(1, sizeof(Texture));

    if (!tex)
        return no_texture;

    SDL_Surface *img = load_from_disk(name);

    if (!img)
        return no_texture;

    tex->cache_index = cache_index;
    tex->type        = type;
    tex->width       = img->w;
    tex->height      = img->h;
    tex->bytes_per_pixel = img->format->BytesPerPixel;

    strncpy(tex->name, name, MAX_GAMEPATH);
    set_filters(type, tex);
    texture_upload(tex, img->pixels);

    SDL_free(img);

    return tex;
}

/**
 * Clears all unused textures. Any texture whose cache index does not match the current
 * cache index is considered stale.
 */
void texture_cache_remove_unused(void)
{
    HashTableIter iter;
    hashtable_iter_init(&iter, sprites);

    while (hashtable_iter_has_next(&iter)) {
        TableEntry *e = hashtable_iter_next(&iter);
        Texture    *t = e->value;
        if (t->cache_index != texture_cache_index) {
            hashtable_iter_remove(&iter);
            glDeleteTextures(1, t->id);
            free(t);
        }
    }
    hashtable_iter_init(&iter, walls);

    while (hashtable_iter_has_next(&iter)) {
        TableEntry *e = hashtable_iter_next(&iter);
        Texture    *t = e->value;
        if (t->cache_index < texture_cache_index) {
            hashtable_iter_remove(&iter);
            glDeleteTextures(1, t->id);
            free(t);
        }
    }
}

/**
 * Adds the texture to cache and returns it.
 */
Texture *add_to_cache_by_id(uint32_t id, HashTable *cache, TextureType type)
{
    Texture *tex = hashtable_get(cache, (void*) id);

    /* Texture is already in the cache */
    if (tex) {
        tex->cache_index = texture_cache_index;
        return tex;
    }
    char name[64];

    switch (type) {
    case TT_Wall:   com_snprintf(name, sizeof(name), "walls/%.3d.tga", id); break;
    case TT_Sprite: com_snprintf(name, sizeof(name), "sprites/%.3d.tga", id); break;
    case TT_Pic:    return no_texture;
    }

    if (!(tex = texture_new(&name, type, texture_cache_index)))
        return NULL;

    if (!hashtable_add(cache, (void*) id, tex))
        return NULL;

    return tex;
}

/**
 * Returns a wall from the texture cache if it exists. If doesn't exist it is loaded
 * into the cache and returned.
 */
Texture *texture_get_wall(uint32_t id)
{
    return add_to_cache_by_id(id, walls, TT_Wall);
}

/**
 * Returns a sprite from the texture cache if it exists. If doesn't exist it is loaded
 * into the cache and returned.
 */
Texture *texture_get_sprite(uint32_t id)
{
    return add_to_cache_by_id(id, sprites, TT_Sprite);
}

/**
 * Returns a picture from the texture cache if it exists. If doesn't exist it is loaded
 * into the cache and returned.
 */
Texture *texture_get_picture(char *name)
{
    Texture *tex = hashtable_get(pictures, name);

    if (tex) {
        tex->cache_index = texture_cache_index;
        return tex;
    }
    if (!(tex = texture_new(name, TT_Pic, texture_cache_index)))
        return NULL;

    if (!hashtable_add(pictures, name, tex))
        return NULL;

    return tex;
}