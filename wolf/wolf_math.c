/*

    Copyright (C) 2004-2013 Michael Liebscher
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

/**
 * \file wolf_math.c
 * \brief Wolfenstein 3-D math routines.
 * \author Michael Liebscher
 * \date 2004-2013
 * \author DarkOne the Hacker
 * \date 2000
 * \note This code was derived from NewWolf, and was originally written by DarkOne the Hacker.
 */

#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>

#include "wolf_math.h"
#include "../env/com_math.h"
#include "../env/common_utils.h"


#define XRES            640
#define YRES            480



char dx4dir[5] = {1, 0, -1,  0, 0}; // dx & dy based on direction
char dy4dir[5] = {0, 1,  0, -1, 0};
char dx8dir[9] = {1, 1, 0, -1, -1, -1,  0,  1, 0}; // dx & dy based on direction
char dy8dir[9] = {0, 1, 1,  1,  0, -1, -1, -1, 0};
dir4type opposite4[5] = {2, 3, 0, 1, 4};
dir8type opposite8[9] = {4, 5, 6, 7, 0, 1, 2, 3, 8};
dir8type dir4to8[5] = {0, 2, 4, 6, 8};
dir8type diagonal[9][9] = {
    /* east */  {dir8_nodir, dir8_nodir, dir8_northeast, dir8_nodir, dir8_nodir, dir8_nodir, dir8_southeast, dir8_nodir, dir8_nodir},
    {dir8_nodir, dir8_nodir, dir8_nodir, dir8_nodir, dir8_nodir, dir8_nodir, dir8_nodir, dir8_nodir, dir8_nodir},
    /* north */ {dir8_northeast, dir8_nodir, dir8_nodir, dir8_nodir, dir8_northwest, dir8_nodir, dir8_nodir, dir8_nodir, dir8_nodir},
    {dir8_nodir, dir8_nodir, dir8_nodir, dir8_nodir, dir8_nodir, dir8_nodir, dir8_nodir, dir8_nodir, dir8_nodir},
    /* west */  {dir8_nodir, dir8_nodir, dir8_northwest, dir8_nodir, dir8_nodir, dir8_nodir, dir8_southwest, dir8_nodir, dir8_nodir},
    {dir8_nodir, dir8_nodir, dir8_nodir, dir8_nodir, dir8_nodir, dir8_nodir, dir8_nodir, dir8_nodir, dir8_nodir},
    /* south */ {dir8_southeast, dir8_nodir, dir8_nodir, dir8_nodir, dir8_southwest, dir8_nodir, dir8_nodir, dir8_nodir, dir8_nodir},
    {dir8_nodir, dir8_nodir, dir8_nodir, dir8_nodir, dir8_nodir, dir8_nodir, dir8_nodir, dir8_nodir, dir8_nodir},
    {dir8_nodir, dir8_nodir, dir8_nodir, dir8_nodir, dir8_nodir, dir8_nodir, dir8_nodir, dir8_nodir, dir8_nodir}
};
// dir of delta tooks dx{-1|0|1}+1 & dy{-1|0|1}+1 and give direction
dir4type dir4d[3][3] = {{dir4_nodir, dir4_west , dir4_nodir},
    {dir4_south, dir4_nodir, dir4_north},
    {dir4_nodir, dir4_east , dir4_nodir}
};
int dir8angle[9] = {ANG_0, ANG_45, ANG_90, ANG_135, ANG_180, ANG_225, ANG_270, ANG_315, ANG_0};
int dir4angle[5] = {ANG_0, ANG_90, ANG_180, ANG_270, ANG_0};


/**
 * \brief Intialize wolf math module.
 */
PUBLIC int WM_BuildTables (void)
{


    srand ((unsigned int)time (NULL));


    return 1;
}

/**
 * \brief Generates a random number between 1 and 255
 * \return A value between 1 and 255.
 */
PUBLIC int US_RndT (void)
{
    return (1 + (rand() % 255)) & 0xFF;
}

/**
 * \brief Get the quadrant the angle resides in
 * \param[in] angle Angle in radians.
 * \return quadrant the angle resides in
 */
PUBLIC quadrant GetQuadrant (float angle)
{
    angle = angle_normalize (angle);

    if (angle < M_PI / 2) {
        return q_first;
    } else if (angle < M_PI) {
        return q_second;
    } else if (angle < 3 * M_PI / 2) {
        return q_third;
    } else {
        return q_fourth;
    }
}

/**
 * \brief Get the cardinal direction of angle
 * \param[in] angle Angle in radians.
 * \return dir4type direction
 */
PUBLIC dir4type Get4dir (float angle)
{
    angle = angle_normalize (angle + M_PI / 4);

    if (angle < M_PI / 2) {
        return dir4_east;
    } else if (angle < M_PI) {
        return dir4_north;
    } else if (angle < 3 * M_PI / 2) {
        return dir4_west;
    } else {
        return dir4_south;
    }
}

/**
 * \brief Get ordinal direction of angle
 * \param[in] angle Angle in radians.
 * \return dir8type direction
 */
PUBLIC dir8type Get8dir (float angle)
{
    angle = angle_normalize (angle + M_PI / 12);

    if (angle <= (M_PI / 4)) {
        return dir8_east;
    } else if (angle < (M_PI / 2)) {
        return dir8_northeast;
    } else if (angle <= (3 * M_PI / 4)) {
        return dir8_north;
    } else if (angle < M_PI) {
        return dir8_northwest;
    } else if (angle <= (5 * M_PI / 4)) {
        return dir8_west;
    } else if (angle < (3 * M_PI / 2)) {
        return dir8_southwest;
    } else if (angle <= (7 * M_PI / 4)) {
        return dir8_south;
    } else {
        return dir8_southeast;
    }
}

/**
 * \brief Calculates distance between a point (x, y) and a line.
 * \param[in] x X-Coordinate.
 * \param[in] y Y-Coordinate.
 * \param[in] angle Angle in radians.
 * \return Returns distance between the point and line
 */
PUBLIC int Point2LineDist (const int x, const int y, const float angle)
{
    return ABS ((int) (x * sin (angle) - y * cos (angle)));
}


/**
 * \brief Calculates line length to the point nearest to (point)
 * \param[in] x X-Coordinate.
 * \param[in] y Y-Coordinate.
 * \param[in] angle Angle in radians.
 * \return Returns length of line segment
 */
PUBLIC int LineLen2Point (const int x, const int y, const float angle)
{
    return (int) (cos (angle) * x + sin (angle) * y);
}

/**
 * \brief Transform point
 * \param[in] Point1X X-Coordinate.
 * \param[in] Point1Y Y-Coordinate.
 * \param[in] Point2X X-Coordinate.
 * \param[in] Point2Y Y-Coordinate.
 * \return Returns angle in radians
 * \note
 *      point2 = {x,y}
 *            / |
 *          /   |
 *        /     |
 *      /a______|----------> x
 *  point1 = {x, y}
 */
PUBLIC float TransformPoint (const double Point1X, const double Point1Y, const double Point2X, const double Point2Y)
{

    return (float)angle_normalize ((float)atan2 (Point1Y - Point2Y, Point1X - Point2X));
}

