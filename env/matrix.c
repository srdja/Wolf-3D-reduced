/*

    Copyright (C) 2005-2012 Michael Liebscher <johnnycanuck@users.sourceforge.net>

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
 * \file matrix.c
 * \brief Matrix math routines.
 * \author Michael Liebscher
 * \date 2005-2012
 */

#include "matrix.h"

/**
 * \brief Computes the product of two 3x3 matrices.
 * \param[in] in1 3x3 matrices.
 * \param[in] in2 3x3 matrices.
 * \param[out] out 3x3 matrices.
 * \return
 * \note
 *  Product of two 3x3 matrices
 *  ( a b c ) ( r u x )   ( ar + bs + ct  au + bv + cw  ax + by + cz )
 *  ( d e f ) ( s v y ) = ( dr + es + ft  du + ev + fw  dx + ey + fz )
 *  ( h i j ) ( t w z )   ( hr + hs + ht  iu + iv + iw  jx + jy + jz )
 */
PUBLIC void Matrix3x3Multiply (mat3_t in1, mat3_t in2, mat3_t out)
{
    out[0] = in1[0] * in2[0] + in1[1] * in2[3] + in1[2] * in2[6];
    out[1] = in1[0] * in2[1] + in1[1] * in2[4] + in1[2] * in2[7];
    out[2] = in1[0] * in2[2] + in1[1] * in2[5] + in1[2] * in2[8];

    out[3] = in1[3] * in2[0] + in1[4] * in2[3] + in1[5] * in2[6];
    out[4] = in1[3] * in2[1] + in1[4] * in2[4] + in1[5] * in2[7];
    out[5] = in1[3] * in2[2] + in1[4] * in2[5] + in1[5] * in2[8];

    out[6] = in1[6] * in2[0] + in1[7] * in2[3] + in1[8] * in2[6];
    out[7] = in1[6] * in2[1] + in1[7] * in2[4] + in1[8] * in2[7];
    out[8] = in1[6] * in2[2] + in1[7] * in2[5] + in1[8] * in2[8];
}
