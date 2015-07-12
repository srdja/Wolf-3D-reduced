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
 * \file math.c
 * \brief Math routines.
 * \author Michael Liebscher
 * \date 2004-2013
 */

#include <math.h>
#include <string.h>


#include "com_math.h"
#include "common.h"


/**
 * \brief Calculate the field of view.
 * \param[in] fov_x Must be within 1 and 179 degrees.
 * \param[in] width Width of viewing area.
 * \param[in] height Height of viewing area.
 * \return The field of view in degrees.
 */
PUBLIC float CalcFov (float fov_x, float width, float height)
{
    if (fov_x < 1 || fov_x > 179) {
        Com_Error (ERR_DROP, "Bad fov: %f", fov_x);
    }

    return (float)RAD2DEG (atan (height / (width / tan (fov_x / 360 * M_PI)))) * 2;
}



/**
 * \brief Initialize optimized math routines.
 */
PUBLIC void MathLib_Init (void)
{

}





