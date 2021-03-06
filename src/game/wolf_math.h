/*

    Copyright (C) 2004 Michael Liebscher
    Copyright (C) 2000 by DarkOne the Hacker

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

/*
 *  wolf_math.h:  Wolfenstein 3-D math routines.
 *
 *  Author: Michael Liebscher <johnnycanuck@users.sourceforge.net>
 *
 *  Acknowledgement:
 *  This code was derived from NewWolf, and was originally
 *  written by DarkOne the Hacker.
 *
 *
 */

/*
    Notes:
    This module is implemented by wolf_math.c

*/
#ifndef __WOLF_MATH_H__
#define __WOLF_MATH_H__

#include <math.h>

#include "../util/com_math.h"


#define FLOATTILE  65536.0f


// Angle Direction Types & LUTs (Hard Coded! Please do not mess them)
typedef enum {q_first, q_second, q_third, q_fourth} quadrant;
typedef enum {dir4_east, dir4_north, dir4_west, dir4_south, dir4_nodir} dir4type;
typedef enum { dir8_east, dir8_northeast, dir8_north, dir8_northwest, dir8_west,
               dir8_southwest, dir8_south, dir8_southeast, dir8_nodir
             } dir8type;

extern char dx4dir[5], dy4dir[5], dx8dir[9], dy8dir[9];
extern dir4type opposite4[5], dir4d[3][3];
extern dir8type opposite8[9], dir4to8[5], diagonal[9][9];
extern int dir8angle[9], dir4angle[5];
// ------------------------- * Vectors * -------------------------
// Vectors & angles for 3D-Space


typedef struct {
    long origin[2];
    float angle; //in radians
    float pitch;

} placeonplane_t;

#define max_of_2(a, b) ((a)>(b)?(a):(b))


#define TILE2POS(a) (((a)<<TILE_SHIFT)+TILE_HALF)
#define POS2TILE(a)  ((a)>>TILE_SHIFT)
// ------------------------- * vvv FINE angles vvv * -------------------------
#define ANG_0           0       //(int)((float)0/ASTEP)
#define ANG_45      5760    //(int)((float)45/ASTEP)
#define ANG_90      11520   //(int)((float)90/ASTEP)
#define ANG_135     17280   //(int)((float)135/ASTEP)
#define ANG_180     23040   //(int)((float)180/ASTEP)
#define ANG_225     28800   //(int)((float)225/ASTEP)
#define ANG_270     34560     //(int)((float)270/ASTEP)
#define ANG_315     40320     //(int)((float)225/ASTEP)
// ------------------------- * ^^^ FINE angles ^^^ * -------------------------

#define FINE2RAD( a ) (((a) * M_PI ) / ANG_180)
#define RAD2FINE( a ) (((a) * ANG_180) / M_PI)

int WM_BuildTables (void);

#define TanDgr( x )     (tan( DEG2RAD( x ) ))

int US_RndT (void);

int Point2LineDist (const int x, const int y, const float angle);
int LineLen2Point (const int x, const int y, const float angle);

quadrant GetQuadrant (float angle);
dir4type Get4dir (float angle);
dir8type Get8dir (float angle);

float TransformPoint (const double Point1X, const double Point1Y, const double Point2X, const double Point2Y);


#endif /* __WOLF_MATH_H__ */
