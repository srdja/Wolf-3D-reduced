/*

    Copyright (C) 2004-2012 Michael Liebscher <johnnycanuck@users.sourceforge.net>

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
 * \file wolf_local.h
 * \brief Wolfenstein 3-D init.
 * \author Michael Liebscher
 * \date 2004-2012
 * \note Portion of this code was derived from Wolfenstein 3-D, and was originally written by Id Software, Inc.
 */


#ifndef __WOLF_LOCAL_H__
#define __WOLF_LOCAL_H__

#include "../common.h"


// Game Version
#define WOLFENSTEINWL6  0x00
#define SPEAROFDESTINY  0x01

#define MAX_EPISODES    6
#define MAX_SKILLS      4
#define MAX_MAPS        81 //60    changed by gsh to allow for SOD levels

typedef struct {
    int     episode;
    int     map;
    int     skill;
    int     levelCompleted;     // already at intermission when saved
    int     version;
    int     mapFlags[MAX_SKILLS][MAX_MAPS];
} currentMap_t;

extern currentMap_t currentMap;


#define TILE_GLOBAL 0x10000
#define TILE_HALF   0x8000
#define TILE_SHIFT  16

#define MINDIST     (0x5800)


typedef enum difficulty_e {
    gd_baby,
    gd_easy,
    gd_medium,
    gd_hard

} difficulty_t;

//
// this structure is cleared as each map is entered
//
typedef struct {
    int         framenum;
    float       time;

    char        level_name[ MAX_OSPATH ];   // the descriptive name (Outer Base, etc)
    char        mapname[ MAX_OSPATH ];      // the server name (base1, etc)
    char        nextmap[ MAX_OSPATH ];      // go here when fraglimit is hit

    // intermission state
    _boolean    inGame;

    W32         episode;
    W32         floornum;
    float       fpartime;
    char        spartime[6];

    W32         total_secrets;
    W32         found_secrets;

    W32         total_treasure;
    W32         found_treasure;

    W32         total_monsters;
    W32         killed_monsters;


} level_locals_t;



extern level_locals_t   levelstate;



typedef struct {
    W32 total_secrets;
    W32 found_secrets;

    W32 total_treasure;
    W32 found_treasure;

    W32 total_monsters;
    W32 killed_monsters;
    W32 time;

} LRstruct;

extern LRstruct LevelRatios;


extern int g_version;
extern float   g_fov;
extern float g_autoaim;
extern  int skill;

extern int tics;

void Game_Init (void);
void Game_Reset (void);

void ProcessGuards (void);


#define WL6SPRITESDIRNAME   "sprites"
#define SODSPRITESDIRNAME   "sodsprites"

extern char *spritelocation;

void LoadWallTexture (int wallPicNum , _boolean *pIsDark);

#endif /* __WOLF_LOCAL_H__ */
