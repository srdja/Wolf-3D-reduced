/*

    Copyright (C) 2005-2013 Michael Liebscher <johnnycanuck@users.sourceforge.net>

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
 * @file angle.c
 * @brief Angle math routines.
 * @author Michael Liebscher
 * @date 2005-2013
 */

#include "com_math.h"
#include "common.h"

/**
 * Finds the difference between two angles.
 * @param[in] angle1 Angle in radians.
 * @param[in] angle2 Angle in radians.
 * @return Returns the absolute difference between two angles in radians, this will always be between 0 and PI.
 */
float angle_diff (float angle1, float angle2)
{
    float d;

    if (angle1 > angle2) {
        d = angle1 - angle2;
    } else {
        d = angle2 - angle1;
    }

    if (d > M_PI) {
        return 2 * M_PI - d;
    } else {
        return d;
    }
}

/**
 * Clockwise distance between two angles.
 * @param[in] angle1 Angle in radians.
 * @param[in] angle2 Angle in radians.
 * @return Returns the clockwise distance from angle2 to angle1, this may be greater than 180 degrees.
 */
float angle_wise (float angle1, float angle2)
{
    if (angle1 > angle2) {
        return angle1 - angle2;
    } else {
        return angle1 + 2 * M_PI - angle2;
    }
}

/**
 * Angles normalized to the range [-180 < angle <= 180]
 * @param[in] angle Angle in radians.
 * @return Angle normalized to the range [-180 < angle <= 180] in radians
 */
float angle_normalize (float angle)
{
    while (angle < 0) {
        angle += (2 * M_PI);
    }

    while (angle >= (2 * M_PI)) {
        angle -= (2 * M_PI);
    }

    return angle;
}
