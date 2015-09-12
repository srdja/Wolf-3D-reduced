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
 * \file math.h
 * \brief Math routines.
 * \author Michael Liebscher
 * \date 2004-2013
 */

#ifndef __COM_MATH_H__
#define __COM_MATH_H__

#include <stdint.h>

#define DEG2RAD( a ) ( (a) * 0.01745329251994329576f ) // a * M_PI / 180.0f
#define RAD2DEG( a ) ( (int)((a) * 57.2957795130823209071f) ) // a * 180.0f / M_PI

#define ANGLE2SHORT( x )    ( (int)((x) * 65536 / 360) & 65535 )

extern float angle_diff (float angle1, float angle2);
extern float angle_wise (float angle1, float angle2);
extern float angle_normalize (float angle);

typedef float vec_t;
typedef vec_t vec3_t[ 3 ];

#define vectorCopy( a, b )  ( (b)[ 0 ] = (a)[ 0 ], (b)[ 1 ] = (a)[ 1 ], (b)[ 2 ] = (a)[ 2 ] )

typedef uint8_t colour3_t[ 3 ]; // RGB
typedef uint8_t colour4_t[4];

#define BIT( x )    ( 1 << (x) )
#define FloatToInt( a ) (SW32)(a)


#define PITCH   0   /* up / down */
#define YAW     1   /* left / right */

#ifndef M_PI

#define M_PI    3.14159265358979323846f // matches value in gcc v2 math.h

#endif


#ifndef ABS

#define ABS( x )    ( (x) < 0 ? -(x) : (x) )

#endif


#define RINT( x ) rint( x )


float CalcFov (float fov_x, float width, float height);



#endif /* __COM_MATH_H__ */

