/*

    Copyright (C) 2004-2013 Michael Liebscher <johnnycanuck@users.sourceforge.net>

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
 * \file wolf_level.c
 * \brief Level handler.
 * \author Michael Liebscher
 * \date 2004-2013
 * \note Portion of this code was derived from NewWolf, and was originally written by DarkOne the Hacker.
 *       Portion of this code was derived from Wolfenstein 3-D, and was originally written by Id Software, Inc.
 */

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "wolf_level.h"
#include "wolf_local.h"
#include "../graphics/wolf_renderer.h"
#include "../util/compression.h"

#include "../util/com_string.h"
#include "../graphics/texture_manager.h"

#include "wolf_actors.h"


statinfo_t static_wl6[] = {
    {false, -1},            // puddle          spr1v
    { true, -1},            // Green Barrel    "
    { true, -1},            // Table/chairs    "
    { true, -1},            // Floor lamp      "
    {false, -1},            // Chandelier      "
    { true, -1},            // Hanged man      "
    {false, pow_alpo},      // Bad food        "
    { true, -1},            // Red pillar      "
    { true, -1},            // Tree            spr2v
    {false, -1},            // Skeleton flat   "
    { true, -1},            // Sink            " (SOD:gibs)
    { true, -1},            // Potted plant    "
    { true, -1},            // Urn             "
    { true, -1},            // Bare table      "
    {false, -1},            // Ceiling light   "
    {false, -1},            // Kitchen stuff   "
    { true, -1},            // suit of armor   spr3v
    { true, -1},            // Hanging cage    "
    { true, -1},            // SkeletoninCage  "
    {false, -1},            // Skeleton relax  "
    {false, pow_key1},      // Key 1           "
    {false, pow_key2},      // Key 2           "
    { true, -1},            // stuff                (SOD:gibs)
    {false, -1},            // stuff
    {false, pow_food},      // Good food       spr4v
    {false, pow_firstaid},      // First aid       "
    {false, pow_clip},      // Clip            "
    {false, pow_machinegun},    // Machine gun     "
    {false, pow_gatlinggun},    // Gatling gun     "
    {false, pow_cross},     // Cross           "
    {false, pow_chalice},       // Chalice         "
    {false, pow_bible},     // Bible           "
    {false, pow_crown},     // crown           spr5v
    {false, pow_fullheal},      // one up          "
    {false, pow_gibs},      // gibs            "
    { true, -1},            // barrel          "
    { true, -1},            // well            "
    { true, -1},            // Empty well      "
    {false, pow_gibs},      // Gibs 2          "
    { true, -1},            // flag             "
    { true, -1},            // Call Apogee      spr7v
    {false, -1},            // junk            "
    {false, -1},            // junk            "
    {false, -1},            // junk            "
    {false, -1},            // pots            "
    { true, -1},            // stove           " (SOD:gibs)
    { true, -1},            // spears          " (SOD:gibs)
    {false, -1},            // vines           "
};

statinfo_t *statinfo = static_wl6;
int num_statics = sizeof (static_wl6) / sizeof (static_wl6[ 0 ]);

static uint16_t cachedGuard = 0;
static uint16_t cachedOfficer = 0;
static uint16_t cachedSS = 0;
static uint16_t cachedDog = 0;
static uint16_t cachedMutant = 0;

static int progress_bar = 0;

extern void R_EndFrame (void);

LevelData_t levelData;

/**
 * \brief Cache sprite textures
 * \param[in] start Start of range
 * \param[in] end End of range.
 */
void CacheTextures (uint16_t start, uint16_t end)
{
    uint16_t i;

    if (end < start) {
        return;
    }

    for (i = start ; i <= end ; ++i) {
        TM_FindTexture_Sprite (i);
    }

    R_DrawPsyched (++progress_bar + 30);
    R_EndFrame();
}

/**
 * \brief Spawn all actors and mark down special places.
 * \param[in] lvl Level structure
 */
void Level_ScanInfoPlane (LevelData_t *lvl)
{
    int x, y;
    uint16_t tile;

    cachedGuard = 0;
    cachedOfficer = 0;
    cachedSS = 0;
    cachedDog = 0;
    cachedMutant = 0;
    progress_bar = 0;

    for (y = 0 ; y < 64; ++y) {
        for (x = 0 ; x < 64 ; ++x) {
            tile = lvl->Plane2[ (63 - y) * 64 + x ];

            if (! tile) {
                continue;
            }

            switch (tile) {
//
// guard
//
            case 180:
            case 181:
            case 182:
            case 183:
                if (skill < gd_hard)
                    break;

                tile -= 36;

            case 144:
            case 145:
            case 146:
            case 147:
                if (skill < gd_medium)
                    break;

                tile -= 36;

            case 108:
            case 109:
            case 110:
            case 111:
                if (! cachedGuard) {
                    CacheTextures (SPR_GRD_S_1, SPR_GRD_SHOOT3);
                    cachedGuard = 1;
                }

                SpawnStand (en_guard, x, y, tile - 108);
                break;

            case 184:
            case 185:
            case 186:
            case 187:
                if (skill < gd_hard)
                    break;

                tile -= 36;

            case 148:
            case 149:
            case 150:
            case 151:
                if (skill < gd_medium)
                    break;

                tile -= 36;

            case 112:
            case 113:
            case 114:
            case 115:
                if (! cachedGuard) {
                    CacheTextures (SPR_GRD_S_1, SPR_GRD_SHOOT3);
                    cachedGuard = 1;
                }

                SpawnPatrol (en_guard, x, y, tile - 112);
                break;

            case 124:
                SpawnDeadGuard (en_guard, x, y);
                break;

//
// officer
//
            case 188:
            case 189:
            case 190:
            case 191:
                if (skill < gd_hard)
                    break;

                tile -= 36;

            case 152:
            case 153:
            case 154:
            case 155:
                if (skill < gd_medium)
                    break;

                tile -= 36;

            case 116:
            case 117:
            case 118:
            case 119:
                if (! cachedOfficer) {
                    CacheTextures (SPR_OFC_S_1, SPR_OFC_SHOOT3);
                    cachedOfficer = 1;
                }

                SpawnStand (en_officer, x, y, tile - 116);
                break;


            case 192:
            case 193:
            case 194:
            case 195:
                if (skill < gd_hard)
                    break;

                tile -= 36;

            case 156:
            case 157:
            case 158:
            case 159:
                if (skill < gd_medium)
                    break;

                tile -= 36;

            case 120:
            case 121:
            case 122:
            case 123:
                if (! cachedOfficer) {
                    CacheTextures (SPR_OFC_S_1, SPR_OFC_SHOOT3);
                    cachedOfficer = 1;
                }

                SpawnPatrol (en_officer, x, y, tile - 120);
                break;

//
// SS
//
            case 198:
            case 199:
            case 200:
            case 201:
                if (skill < gd_hard)
                    break;

                tile -= 36;

            case 162:
            case 163:
            case 164:
            case 165:
                if (skill < gd_medium)
                    break;

                tile -= 36;

            case 126:
            case 127:
            case 128:
            case 129:
                if (! cachedSS) {
                    CacheTextures (SPR_SS_S_1, SPR_SS_SHOOT3);
                    cachedSS = 1;
                }

                SpawnStand (en_ss, x, y, tile - 126);
                break;

            case 202:
            case 203:
            case 204:
            case 205:
                if (skill < gd_hard)
                    break;

                tile -= 36;

            case 166:
            case 167:
            case 168:
            case 169:
                if (skill < gd_medium)
                    break;

                tile -= 36;

            case 130:
            case 131:
            case 132:
            case 133:
                if (! cachedSS) {
                    CacheTextures (SPR_SS_S_1, SPR_SS_SHOOT3);
                    cachedSS = 1;
                }

                SpawnPatrol (en_ss, x, y, tile - 130);
                break;

//
// dogs
//
            case 206:
            case 207:
            case 208:
            case 209:
                if (skill < gd_hard)
                    break;

                tile -= 36;

            case 170:
            case 171:
            case 172:
            case 173:
                if (skill < gd_medium)
                    break;

                tile -= 36;

            case 134:
            case 135:
            case 136:
            case 137:
                if (! cachedDog) {
                    CacheTextures (SPR_DOG_W1_1, SPR_DOG_JUMP3);
                    cachedDog = 1;
                }

                SpawnStand (en_dog, x, y, tile - 134);
                break;

            case 210:
            case 211:
            case 212:
            case 213:
                if (skill < gd_hard)
                    break;

                tile -= 36;

            case 174:
            case 175:
            case 176:
            case 177:
                if (skill < gd_medium)
                    break;

                tile -= 36;

            case 138:
            case 139:
            case 140:
            case 141:
                if (! cachedDog) {
                    CacheTextures (SPR_DOG_W1_1, SPR_DOG_JUMP3);
                    cachedDog = 1;
                }

                SpawnPatrol (en_dog, x, y, tile - 138);
                break;

// bosses
            case 214:
                CacheTextures (SPR_BOSS_W1, SPR_BOSS_DIE3);
                SpawnBoss (en_boss, x, y);
                break;

            case 197:
                CacheTextures (SPR_GRETEL_W1, SPR_GRETEL_DIE3);
                SpawnBoss (en_gretel, x, y);
                break;

            case 215:
                CacheTextures (SPR_GIFT_W1, SPR_GIFT_DEAD);
                SpawnBoss (en_gift, x, y);
                break;

            case 179:
                CacheTextures (SPR_FAT_W1, SPR_FAT_DEAD);
                SpawnBoss (en_fat, x, y);
                break;

            case 196:
                CacheTextures (SPR_SCHABB_W1, SPR_HYPO4);
                SpawnBoss (en_schabbs, x, y);
                break;

            case 160:
                CacheTextures (SPR_FAKE_W1, SPR_FAKE_DEAD);
                SpawnBoss (en_fake, x, y);
                break;

            case 178:
                CacheTextures (SPR_MECHA_W1, SPR_HITLER_DIE7);
                SpawnBoss (en_mecha, x, y);
                break;

//
// mutants
//
            case 252:
            case 253:
            case 254:
            case 255:
                if (skill < gd_hard)
                    break;

                tile -= 18;

            case 234:
            case 235:
            case 236:
            case 237:
                if (skill < gd_medium)
                    break;

                tile -= 18;

            case 216:
            case 217:
            case 218:
            case 219:
                if (! cachedMutant) {
                    CacheTextures (SPR_MUT_S_1, SPR_MUT_SHOOT4);
                    cachedMutant = 1;
                }

                SpawnStand (en_mutant, x, y, tile - 216);
                break;

            case 256:
            case 257:
            case 258:
            case 259:
                if (skill < gd_hard)
                    break;

                tile -= 18;

            case 238:
            case 239:
            case 240:
            case 241:
                if (skill < gd_medium)
                    break;

                tile -= 18;

            case 220:
            case 221:
            case 222:
            case 223:
                if (! cachedMutant) {
                    CacheTextures (SPR_MUT_S_1, SPR_MUT_SHOOT4);
                    cachedMutant = 1;
                }

                SpawnPatrol (en_mutant, x, y, tile - 220);
                break;

//
// ghosts
//
            case 224:
                CacheTextures (SPR_BLINKY_W1, SPR_BLINKY_W2);
                SpawnGhosts (en_blinky, x, y);
                break;

            case 225:
                CacheTextures (SPR_PINKY_W1, SPR_PINKY_W2);
                SpawnGhosts (en_clyde, x, y);
                break;

            case 226:
                CacheTextures (SPR_CLYDE_W1, SPR_CLYDE_W2);
                SpawnGhosts (en_pinky, x, y);
                break;

            case 227:
                CacheTextures (SPR_INKY_W1, SPR_INKY_W2);
                SpawnGhosts (en_inky, x, y);
                break;
            }
        }
    }
}

/**
 * \brief Spawn static object
 * \param[in] lvl Level structure
 * \param[in] type Object type
 * \param[in] x X position in tile map
 * \param[in] y Y position in tile map
 */
static void Lvl_SpawnStatic (LevelData_t *lvl, int type, int x, int y)
{
    int spr_id;

    if (statinfo[ type ].powerup == -1) {
        if (statinfo[ type ].block) { // blocking static
            lvl->tilemap[ x ][ y ] |= BLOCK_TILE;
        } else {                    // dressing static
            lvl->tilemap[ x ][ y ] |= DRESS_TILE;
        }

        spr_id = Sprite_GetNewSprite();

        if (spr_id == -1) {
            return;
        }

        Sprite_SetPos (spr_id, TILE2POS (x), TILE2POS (y), 0);
        Sprite_SetTex (spr_id, 0, SPR_STAT_0 + type);
    } else {
        Powerup_Spawn (x, y, statinfo[ type ].powerup);

        if (statinfo[ type ].powerup == pow_cross ||
                statinfo[ type ].powerup == pow_chalice ||
                statinfo[ type ].powerup == pow_bible ||
                statinfo[ type ].powerup == pow_crown ||
                statinfo[ type ].powerup == pow_fullheal) {
            levelstate.total_treasure++; // FIXME: move this to Powerup_Spawn Function!
        }
    }
}

/**
 * \brief Spawn object
 * \param[in] lvl Level structure
 * \param[in] type Object type
 * \param[in] x X position in tile map
 * \param[in] y Y position in tile map
 */
static void Lvl_SpawnObj (LevelData_t *lvl, int type, int x, int y) {
    if (type >= 23 && type < 23 + num_statics) {
        // static object
        Lvl_SpawnStatic(lvl, type - 23, x, y);
        return;
    }

    switch (type) {
        case 0x13: // start N
            lvl->pSpawn.origin[0] = TILE2POS (x);
            lvl->pSpawn.origin[1] = TILE2POS (y);
            lvl->pSpawn.angle = DEG2RAD (90);
            break;

        case 0x14: // start E
            lvl->pSpawn.origin[0] = TILE2POS (x);
            lvl->pSpawn.origin[1] = TILE2POS (y);
            lvl->pSpawn.angle = DEG2RAD (0);
            break;

        case 0x15: // start S
            lvl->pSpawn.origin[0] = TILE2POS (x);
            lvl->pSpawn.origin[1] = TILE2POS (y);
            lvl->pSpawn.angle = DEG2RAD (270);
            break;

        case 0x16: // start W
            lvl->pSpawn.origin[0] = TILE2POS (x);
            lvl->pSpawn.origin[1] = TILE2POS (y);
            lvl->pSpawn.angle = DEG2RAD (180);
            break;

        case 0x5a: // turn E
            lvl->tilemap[x][y] |= TILE_IS_E_TURN;//FIXME!
            break;

        case 0x5b: // turn NE
            lvl->tilemap[x][y] |= TILE_IS_NE_TURN;//FIXME!
            break;

        case 0x5c: // turn N
            lvl->tilemap[x][y] |= TILE_IS_N_TURN;//FIXME!
            break;

        case 0x5d: // turn NW
            lvl->tilemap[x][y] |= TILE_IS_NW_TURN;//FIXME!
            break;

        case 0x5e: // turn W
            lvl->tilemap[x][y] |= TILE_IS_W_TURN;//FIXME!
            break;

        case 0x5f: // turn SW
            lvl->tilemap[x][y] |= TILE_IS_SW_TURN;//FIXME!
            break;

        case 0x60: // turn S
            lvl->tilemap[x][y] |= TILE_IS_S_TURN;//FIXME!
            break;

        case 0x61: // turn SE
            lvl->tilemap[x][y] |= TILE_IS_SE_TURN;//FIXME!
            break;

        case 0x62: // pushwall modifier
            lvl->tilemap[x][y] |= SECRET_TILE;
            levelstate.total_secrets++;
            break;

        case 0x63: // Victory trigger
            lvl->tilemap[x][y] |= EXIT_TILE;
            break;
            // spawn guards

    } // end of switch( type )

}

#define MAPHEADER_SIZE  49
#define MAP_SIGNATURE   0x21444921


/**
 * \brief Load level
 * \param[in] levelname Name of level to load
 * \return Returns NULL on error, otherwise filled in LevelData_t structure.
 */
LevelData_t *Level_LoadMap (const char *levelname)
{
    uint16_t rle;
    uint32_t offset[ 3 ];
    uint16_t length[ 3 ];
    uint16_t w, h;
    uint32_t signature;
    uint16_t *buffer, expanded;
    uint8_t *data;
    uint32_t ceiling, floor;
    LevelData_t *newMap;
    filehandle_t *fhandle;
    uint16_t mapNameLength;
    char *mapName;
    uint16_t musicNameLength;
    char *musicName;
    int32_t filesize;

    int x, y0, y, layer1, layer2;

    statinfo = static_wl6;
    num_statics = sizeof (static_wl6) / sizeof (static_wl6[ 0 ]);

    newMap = &levelData;
    memset (newMap, 0, sizeof (LevelData_t));

    fhandle = FS_OpenFile (levelname);

    if (! fhandle) {
        return NULL;
    }

    filesize = FS_GetFileSize (fhandle);

    if (filesize < MAPHEADER_SIZE) {
        return NULL;
    }

//
// Process map header
//
    FS_ReadFile (&signature, 1, 4, fhandle);

    if (signature != MAP_SIGNATURE) {
        return NULL;
    }

    FS_ReadFile (&rle, 2, 1, fhandle);

    FS_ReadFile (&w, 2, 1, fhandle);
    FS_ReadFile (&h, 2, 1, fhandle);

    FS_ReadFile (&ceiling, 4, 1, fhandle);
    FS_ReadFile (&floor, 4, 1, fhandle);

    FS_ReadFile (&length, 2, 3, fhandle);
    FS_ReadFile (&offset, 4, 3, fhandle);


    FS_ReadFile (&mapNameLength, 1, 2, fhandle);
    FS_ReadFile (&musicNameLength, 1, 2, fhandle);

    FS_ReadFile (&levelstate.fpartime, sizeof (float), 1, fhandle);

    FS_ReadFile (levelstate.spartime, sizeof (uint8_t), 5, fhandle);
    levelstate.spartime[ 5 ] = '\0';


    if (filesize < (MAPHEADER_SIZE + mapNameLength + musicNameLength +
                    length[ 0 ] + length[ 1 ] + length[ 2 ])) {
        return NULL;
    }

    mapName = (char *)malloc (mapNameLength + 1);
    musicName = (char *)malloc (musicNameLength + 1);


    FS_ReadFile (mapName, 1, mapNameLength, fhandle);
    mapName[ mapNameLength ] = '\0';


    FS_ReadFile (musicName, 1, musicNameLength, fhandle);
    musicName[ musicNameLength ] = '\0';


    if (filesize < (MAPHEADER_SIZE + mapNameLength + musicNameLength)) {
        return NULL;
    }

//
// Plane1  -Walls
//
    data = (uint8_t*)malloc (length[ 0 ]);

    FS_FileSeek (fhandle, offset[ 0 ], SEEK_SET);
    FS_ReadFile (data, 1, length[ 0 ], fhandle);


    expanded = * ((unsigned short *)data);
    buffer = (uint16_t*)malloc (expanded);

    Lvl_CarmackExpand ((unsigned short *)data + 1, buffer, expanded);
    Lvl_RLEWexpand (buffer + 1, newMap->Plane1, 64 * 64 * 2, rle);

    free (buffer);
    free (data);

//
// Plane2 -Objects
//
    data = (uint8_t*)malloc (length[ 1 ]);

    FS_FileSeek (fhandle, offset[ 1 ], SEEK_SET);
    FS_ReadFile (data, 1, length[ 1 ], fhandle);


    expanded = * ((uint16_t*)data);
    buffer = (uint16_t*)malloc (expanded);

    Lvl_CarmackExpand ((uint16_t*)data + 1, buffer, expanded);
    Lvl_RLEWexpand (buffer + 1, newMap->Plane2, 64 * 64 * 2, rle);

    free (buffer);
    free (data);

//
// Plane3 -Other
//
    data = (uint8_t*)malloc (length[ 2 ]);

    FS_FileSeek (fhandle, offset[ 2 ], SEEK_SET);
    FS_ReadFile (data, 1, length[ 2 ], fhandle);


    expanded = * ((uint16_t*)data);
    buffer = (uint16_t*)malloc (expanded);

    Lvl_CarmackExpand ((uint16_t*)data + 1, buffer, expanded);
    Lvl_RLEWexpand (buffer + 1, newMap->Plane3, 64 * 64 * 2, rle);

    free (buffer);
    free (data);


    FS_CloseFile (fhandle);

// FIXME: Check if this is necessary with non-iphone controls, remove if so

    for (y0 = 0 ; y0 < 64 ; ++y0)
        for (x = 0 ; x < 64 ; ++x) {
            y = 63 - y0;
            layer1 = newMap->Plane1[ y0 * 64 + x ];
            layer2 = newMap->Plane2[ y0 * 64 + x ];

// if server, process obj layer!
            if (layer2) {
                Lvl_SpawnObj (newMap, layer2, x, y);
            }

// Map data layer
            if (layer1 == 0) {
                newMap->areas[ x ][ y ] = -3; // unknown area
            } else if (layer1 < 0x6a) { // solid map object
                if ((layer1 >= 0x5A && layer1 <= 0x5F) ||
                        layer1 == 0x64 || layer1 == 0x65) { // door
                    newMap->tilemap[ x ][ y ] |= DOOR_TILE;
                    Door_Spawn (&newMap->Doors, x, y, layer1);
                    newMap->areas[ x ][ y ] = -2; // door area
                } else {
                    newMap->tilemap[ x ][ y ] |= WALL_TILE;

                    newMap->wall_tex_x[ x ][ y ] = (layer1 - 1) * 2 + 1;
                    newMap->wall_tex_y[ x ][ y ] = (layer1 - 1) * 2;
                    newMap->areas[ x ][ y ] = -1; // wall area

                    if (layer1 == 0x15) { // elevator
                        newMap->tilemap[ x ][ y ] |= ELEVATOR_TILE;
                    }
                }
            } else if (layer1 == 0x6a) { // Ambush floor tile
                newMap->tilemap[ x ][ y ] |= AMBUSH_TILE;
                newMap->areas[ x ][ y ] = -3; // unknown area
            } else if (layer1 >= FIRSTAREA &&
                       layer1 < (FIRSTAREA + NUMAREAS)) { // area
                if (layer1 == FIRSTAREA) { // secret level
                    newMap->tilemap[ x ][ y ] |= SECRETLEVEL_TILE;
                }

                newMap->areas[ x ][ y ] = layer1 - FIRSTAREA;// spawn area
            } else {
                newMap->areas[ x ][ y ] = -3; // unknown area
            }

// End of the map data layer
        }

    // JDC: try to replace all the unknown areas with an adjacent area, to
    // avoid the silent attack / no damage problem when you get an ambush
    // guard stuck on their original tile
    for (x = 1 ; x < 63 ; x++) {
        for (y = 1 ; y < 63 ; y++) {
            if (newMap->areas[x][y] != -3) {
                continue;
            }

            if (newMap->areas[x - 1][y] >= 0) {
                newMap->areas[x][y] = newMap->areas[x - 1][y];
            } else if (newMap->areas[x + 1][y] >= 0) {
                newMap->areas[x][y] = newMap->areas[x + 1][y];
            } else if (newMap->areas[x][y - 1] >= 0) {
                newMap->areas[x][y] = newMap->areas[x][y - 1];
            } else if (newMap->areas[x + 1][y + 1] >= 0) {
                newMap->areas[x][y] = newMap->areas[x][y + 1];
            }
        }
    }

    Door_SetAreas (&newMap->Doors, newMap->areas);

    strncpy(levelstate.level_name, mapName, sizeof(levelstate.level_name));

    strncpy(newMap->mapName, mapName, 127);
    newMap->mapName[127] = '\0';
    strncpy(newMap->musicName, musicName, 127);
    newMap->musicName[127] = '\0';


    newMap->ceilingColour[ 0 ] = (uint8_t) ((ceiling >> 16) & 0xFF);
    newMap->ceilingColour[ 1 ] = (uint8_t) ((ceiling >> 8) & 0xFF);
    newMap->ceilingColour[ 2 ] = (uint8_t) ((ceiling) & 0xFF);
    newMap->floorColour[ 0 ] = (uint8_t) ((floor >> 16) & 0xFF);
    newMap->floorColour[ 1 ] = (uint8_t) ((floor >> 8) & 0xFF);
    newMap->floorColour[ 2 ] = (uint8_t) ((floor) & 0xFF);

    return newMap;
}

/**
 * \brief Precache textures and sounds
 * \param[in] lvl Level structure
 */
void Level_PrecacheTextures_Sound (LevelData_t *lvl)
{
    int x, y;
    char texname[ 32 ];

    for (x = 0 ; x < 64 ; ++x)
        for (y = 0 ; y < 64 ; ++y) {
            if (lvl->tilemap[ x ][ y ] & WALL_TILE) {
                LoadWallTexture (lvl->wall_tex_x[ x ][ y ], NULL);
                LoadWallTexture (lvl->wall_tex_y[ x ][ y ], NULL);
            }

            if (lvl->tilemap[ x ][ y ] & POWERUP_TILE) {
                int tex = lvl->tilemap[ x ][ y ] & POWERUP_TILE;
                TM_FindTexture_Sprite (tex);
            }
        }

    // Doors
    for (x = TEX_DOOR; x < TEX_DLOCK + 2 ; ++x) {
        LoadWallTexture (x, NULL);
    }

    // Items
    CacheTextures (26, 36);

    // Weapon frames
    CacheTextures (SPR_KNIFEREADY, SPR_CHAINATK4);

    for (x = 1; x < 8 ; ++x) {
        com_snprintf (texname, sizeof (texname), "pics/FACE%dAPIC.tga", x);
        (void)TM_FindTexture (texname, TT_Pic);

        com_snprintf (texname, sizeof (texname), "pics/FACE%dBPIC.tga", x);
        (void)TM_FindTexture (texname, TT_Pic);

        com_snprintf (texname, sizeof (texname), "pics/FACE%dCPIC.tga", x);
        (void)TM_FindTexture (texname, TT_Pic);
    }

    com_snprintf (texname, sizeof (texname), "pics/FACE8APIC.tga");
    (void)TM_FindTexture (texname, TT_Pic);
}

/**
 * \brief Check level line
 * \param[in] x1 X-Coordinate of first point
 * \param[in] y1 Y-Coordinate of first point
 * \param[in] x2 X-Coordinate of second point
 * \param[in] y2 Y-Coordinate of second point
 * \param[in] lvl Level structure
 * return true if a straight line between 2 points is unobstructed, otherwise false.
 */
bool Level_CheckLine (int32_t x1, int32_t y1, int32_t x2, int32_t y2, LevelData_t *lvl)
{
    int32_t xt1, yt1, xt2, yt2; /* tile positions */
    int32_t x, y;              /* current point in !tiles! */
    int32_t xdist, ydist;
    int32_t xstep, ystep; /* Step value for each whole xy */

    int32_t deltafrac; /* current point in !1/256 of tile! */

    int32_t Frac;      /* Fractional xy stepper */

    int32_t partial;   /* how much to move in our direction to border */
    int32_t intercept; /* Temp for door code */

#define FRACBITS    8       /* Number of bits of fraction */

// get start & end tiles
    xt1 = x1 >> TILE_SHIFT;
    yt1 = y1 >> TILE_SHIFT;

    xt2 = x2 >> TILE_SHIFT;
    yt2 = y2 >> TILE_SHIFT;

    xdist = ABS (xt2 - xt1);  // X distance in tiles
    ydist = ABS (yt2 - yt1);  // Y distance in tiles

// 1/256 tile precision (TILE_SHIFT is 16)
    x1 >>= FRACBITS;
    y1 >>= FRACBITS;
    x2 >>= FRACBITS;
    y2 >>= FRACBITS;

    if (xdist) { // always positive check only for 0
        if (xt2 > xt1) {
            partial = 256 - (x1 & 0xff);
            xstep = 1;
        } else {
            partial = x1 & 0xff;
            xstep = -1;
        }

        deltafrac = ABS (x2 - x1);
        ystep = ((y2 - y1) << FRACBITS) / deltafrac;
        Frac = y1 + ((ystep * partial) >> FRACBITS);

        x = xt1 + xstep;
        xt2 += xstep;

        do {
            y = Frac >> FRACBITS;
            Frac += ystep;


            assert (x >= 0 && x < 64 && y >= 0 && y < 64);

            if (lvl->tilemap[ x ][ y ] & WALL_TILE) {
                return false; // Wall is in path quitting!
            }

            if (lvl->tilemap[ x ][ y ] & DOOR_TILE) {
                // door, see if the door is open enough
                if (lvl->Doors.DoorMap[ x ][ y ].action != dr_open) {
                    if (lvl->Doors.DoorMap[ x ][ y ].action == dr_closed) {
                        return false;
                    }

                    // checking vertical doors in action: ->_I_
                    intercept = ((Frac - ystep / 2) & 0xFF) >> 4; // 1/64 of tile

                    if (intercept < (63 - lvl->Doors.DoorMap[ x ][ y ].ticcount)) {
                        return false;
                    }
                }
            }

            x += xstep;

        } while (x != xt2);
    }

    if (ydist) { // always positive check only for 0
        if (yt2 > yt1) {
            partial = 256 - (y1 & 0xff);
            ystep = 1;
        } else {
            partial = y1 & 0xff;
            ystep = -1;
        }

        deltafrac = ABS (y2 - y1);
        xstep = ((x2 - x1) << FRACBITS) / deltafrac;
        Frac = x1 + ((xstep * partial) >> FRACBITS);

        y = yt1 + ystep;
        yt2 += ystep;

        do {
            x = Frac >> FRACBITS;
            Frac += xstep;

            assert (x >= 0 && x < 64 && y >= 0 && y < 64);

            if (lvl->tilemap[ x ][ y ] & WALL_TILE) {
                return false; // Wall is in path quitting!
            }

            if (lvl->tilemap[ x ][ y ] & DOOR_TILE) {
                // door, see if the door is open enough
                if (lvl->Doors.DoorMap[ x ][ y ].action != dr_open) {
                    if (lvl->Doors.DoorMap[ x ][ y ].action == dr_closed) {
                        return false;
                    }

                    // checking vertical doors in action: ->_I_
                    intercept = ((Frac - xstep / 2) & 0xFF) >> 4; // 1/64 of tile

                    if (intercept < lvl->Doors.DoorMap[ x ][ y ].ticcount) {
                        return false;
                    }
                }
            }

            y += ystep;

        } while (y != yt2);
    }

    return true;
}
