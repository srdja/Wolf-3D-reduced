/*

    Copyright (C) 2013 Michael Liebscher <johnnycanuck@users.sourceforge.net>

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

/**
 * \file wolf_automap.c
 * \brief Wolfenstein 3-D automap.
 * \author Michael Liebscher
 * \date 2013
 */

#include "platform.h"
#include "keys.h"
#include "common_utils.h"
#include "texture_manager.h"
#include "client.h"
#include "video.h"
#include "wolf_local.h"
#include "wolf_level.h"
#include "wolf_player.h"

#include "opengl_local.h"

#include "../env/color.h"

extern cvar_t   *mapScale;

float   mapOrigin[2];

typedef struct {
    W8  x, y;
    short   texnum;

} mapTile_t;

#define MAPTILE_SPRITE_FLAG 1024
#define MAX_MAP_TILES 5000      // 4096 tiles + sprites

mapTile_t   mapTiles[ MAX_MAP_TILES ];
int         numMapTiles;

extern _boolean Key_IsDown (int keynum);

/**
 * \brief Callback for qsort to sort tile maps by texture number
 * \param[in] a Valid pointer to mapTile_t structure
 * \param[in] b Valid pointer to mapTile_t structure
 */
PRIVATE int MapTileSort (const void *a, const void *b)
{
    return ((mapTile_t *)a)->texnum - ((mapTile_t *)b)->texnum;
}

/**
 * \brief Initialize automap
 */
PUBLIC void OpenAutomap()
{
    mapTile_t *mt = mapTiles;
    int     x, y;
    int     xx, yy;
    int     tex;
    W32 tx, ty, n;
    sprite_t *sprt;
    numMapTiles = 0;

    mapOrigin[0] = Player.position.origin[0] / (float)TILE_GLOBAL;
    mapOrigin[1] = Player.position.origin[1] / (float)TILE_GLOBAL;
    ClientStatic.menuState = IPM_AUTOMAP;

    // identify all the tiles to fill in
    for (x = 0 ; x < 64; ++x) {
        for (y = 0 ; y < 64; ++y) {
            if (r_world->tilemap[ x ][ y ] & (WALL_TILE | PUSHWALL_TILE)) {
                int     visible = 0;

                // check all 8 surrounding tiles for visibility
                for (xx = -1 ; xx <= 1 ; xx++) {
                    if (x + xx < 0) {
                        continue;
                    }

                    if (x + xx > 63) {
                        continue;
                    }

                    for (yy = -1 ; yy <= 1 ; yy++) {
                        if (y + yy < 0) {
                            continue;
                        }

                        if (y + yy > 63) {
                            continue;
                        }

                        if (r_world->tileEverVisible[x + xx][y + yy]) {
                            visible = 1;
                            break;
                        }
                    }
                }

                if (!visible) {
                    continue;
                }

                tex = r_world->wall_tex_y[ x ][ y ];

                // special hack for the elevator switch tile, which is always
                // facing east west for the switch, and north south for the railing
                if (tex == 40 && ((x > 0 && r_world->tileEverVisible[x + 1][y])
                                  || (x < 63 && r_world->tileEverVisible[x - 1][y]))) {
                    tex = r_world->wall_tex_x[ x ][ y ];
                }

                if (tex < 0x6a) {    // skip pushwall destinations

                    mt->x = x;
                    mt->y = y;
                    mt->texnum = tex;

                    TM_FindTexture_Wall (mt->texnum);

                    mt++;
                    continue;
                }
            }

            if (!r_world->tileEverVisible[x][y]) {

                continue;
            }

            if (r_world->tilemap[ x ][ y ] & DOOR_TILE) {

                mt->x = x;
                mt->y = y;
                mt->texnum = r_world->Doors.DoorMap[ x ][ y ].texture;

                TM_FindTexture_Wall (mt->texnum);

                mt++;
                continue;
            }

            // solid floor
            mt->x = x;
            mt->y = y;
            mt->texnum = -1;
            mt++;
        }
    }

    // add solid sprite objects
    for (n = 0, sprt = levelData.sprites; n < levelData.numSprites; ++n, ++sprt) {
        if (sprt->flags & SPRT_REMOVE) {
            continue;
        }

        if (sprt->tex[0] >= SPR_GRD_S_1) {
            // don't draw enemies, only static sprites
            continue;
        }

        tx = sprt->tilex;
        ty = sprt->tiley;

        if (tx > 63) {
            continue;
        }

        if (ty > 63) {
            continue;
        }

        if (!r_world->tileEverVisible[tx][ty]) {
            continue;
        }

        mt->x = tx;
        mt->y = ty;
        mt->texnum = MAPTILE_SPRITE_FLAG | sprt->tex[0];
        mt++;

        if (mt == &mapTiles[ MAX_MAP_TILES ]) {
            break;  // list is full, some items won't show (shouldn't happen)
        }
    }

    // sort the tiles to be drawn by texture
    numMapTiles = mt - mapTiles;

    qsort (mapTiles, numMapTiles, sizeof (mapTiles[0]), MapTileSort);
}

/**
 * \brief Draw the automap
 */
PUBLIC void Automap()
{
    int i;
    int halfWidth = viddef.width / 2;
    int halfHeight = viddef.height / 2;

    mapTile_t   *mt;
    float   px, py;
    float   angle, c, s;
    int     texnum;
    float   scale;

    // gsh - double tap for center on player
    static int tapX = 0;
    static int tapY = 0;
    static int zoom = 0;
    static float TargetX = 0;
    static float TargetY = 0;
    static float TargetZoom = 0;
    static int prevTapX = 0;
    static int prevTapY = 0;

    // Background colour
    R_Draw_Fill (0, 0, viddef.width, viddef.height, colourBlack);

    scale = mapScale->value;

    if (scale < 4.0) {
        // ensure we don't div by 0 from a console cvar change
        scale = 4.0;
    }


    if (Key_IsDown (Key_GetKey ("+back"))) {  // Scroll Up
        mapOrigin[1] -= (float) (4.0 /  mapScale->value);
    }

    if (Key_IsDown (Key_GetKey ("+forward"))) {  // Scroll Down
        mapOrigin[1] += (float) (4.0 / mapScale->value);
    }

    if (Key_IsDown (Key_GetKey ("+moveleft"))) {  // Scroll Left
        mapOrigin[0] -= (float) (4.0 / mapScale->value);
    }

    if (Key_IsDown (Key_GetKey ("+moveright"))) {  // Scroll Right
        mapOrigin[0] += (float) (4.0 / mapScale->value);
    }

    if (Key_IsDown (Key_GetKey ("+left"))) {  // Scale Up
        float scale = mapScale->value;
        scale += 1.0;

        if (scale > 64.0) {
            scale = 64.0;
        }

        Cvar_SetValue (mapScale->name, scale);
    }

    if (Key_IsDown (Key_GetKey ("+right"))) {  // Scale Down
        float scale = mapScale->value;
        scale -= 1.0;

        if (scale < 4.0) {
            scale = 4.0;
        }

        Cvar_SetValue (mapScale->name, scale);
    }

    if (Key_IsDown (Key_GetKey ("+use"))) {   // Centre on player
        zoom = 1;
        TargetX = Player.position.origin[0] / (float)TILE_GLOBAL;
        TargetY = Player.position.origin[1] / (float)TILE_GLOBAL;
        TargetZoom = scale;
    }

    if (zoom) {
        float tolerance = 0.5f;//3;
        int invSpeed = 2;//3;
        float dist = TargetX - mapOrigin[0];
        mapOrigin[0] += dist / invSpeed;
        dist = TargetY - mapOrigin[1];
        mapOrigin[1] += dist / invSpeed;

        dist = TargetZoom - scale;
        scale += dist / (2 * invSpeed); //let's zoom a litle slower than we pan

        if (abs (mapOrigin[0] - TargetX) < tolerance && abs (mapOrigin[1] - TargetY) < tolerance && abs (scale - TargetZoom) < tolerance / 2) {
            mapOrigin[0] = TargetX;
            mapOrigin[1] = TargetY;
            scale = TargetZoom;
            zoom = 0;
        }
        Cvar_SetValue (mapScale->name, scale);
    }

    // set up matrix for drawing in tile units
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity();

    glOrtho (mapOrigin[0] - halfWidth / scale, mapOrigin[0] + halfWidth / scale,
               mapOrigin[1] - halfHeight / scale, mapOrigin[1] + halfHeight / scale, -99999, 99999);

    mt = mapTiles;
    texnum = 99999;

    for (i = 0 ; i < numMapTiles ; i++, mt++) {
        if (texnum != mt->texnum) {
            texnum = mt->texnum;
            if (i != 0) {
                glEnd();
            }
            if (mt->texnum == -1) {
                glDisable (GL_TEXTURE_2D);
                glColor3f (r_world->floorColour[0] / 255.0, r_world->floorColour[1] / 255.0, r_world->floorColour[2] / 255.0);
            } else if (mt->texnum & MAPTILE_SPRITE_FLAG) {
                glEnable (GL_TEXTURE_2D);
                glColor3f (1, 1, 1);
                R_Bind (TM_getSpriteTextureId (mt->texnum & ~MAPTILE_SPRITE_FLAG));
            } else {
                glEnable (GL_TEXTURE_2D);
                glColor3f (1, 1, 1);
                R_Bind (TM_getWallTextureId (mt->texnum));
            }
            glBegin (GL_QUADS);
        }
        glTexCoord2f (0, 1);
        glVertex2i (mt->x, mt->y);
        glTexCoord2f (1, 1);
        glVertex2i (mt->x + 1, mt->y);
        glTexCoord2f (1, 0);
        glVertex2i (mt->x + 1, mt->y + 1);
        glTexCoord2f (0, 0);
        glVertex2i (mt->x, mt->y + 1);
    }
    glEnd();

    // draw the yellow player triangle
    glDisable (GL_TEXTURE_2D);

    if ((ClientStatic.realtime / 500) % 2) { // blink it
        glColor3f (1, 1, 0);
    } else {
        glColor3f (0.5, 0.5, 0);
    }

    angle = Player.position.angle;
    c = cos (angle);
    s = sin (angle);
    px = Player.position.origin[0] / (float)TILE_GLOBAL;
    py = Player.position.origin[1] / (float)TILE_GLOBAL;

    glBegin (GL_TRIANGLES);
        glVertex3f (px + c * 0.5, py + s * 0.5, 0);
        glVertex3f (px - c * 0.5 - s * 0.3, py - s * 0.5 + c * 0.3, 0);
        glVertex3f (px - c * 0.5 + s * 0.3, py - s * 0.5 - c * 0.3, 0);
    glEnd();

}

/**
 * \brief Automap trigger
 */
PUBLIC void Con_ToggleAutomap_f (void)
{
    if (ClientStatic.key_dest == KEY_AUTOMAP) {
        ClientStatic.key_dest = key_game;
    } else {
        ClientStatic.key_dest = KEY_AUTOMAP;
        OpenAutomap();
    }
}

/**
 * \brief Automap input handler
 */
PUBLIC void automap_keydown (int key)
{
    if (Key_IsDown (key)) {
        fprintf(stdout, "Tab pressed\n");
        Con_ToggleAutomap_f();
        ClientStatic.key_dest = key_game;
        ClientStatic.menuState = IPM_GAME;
    }
}

