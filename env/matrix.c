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
 *	( a b c ) ( r u x )	  ( ar + bs + ct  au + bv + cw  ax + by + cz )
 *	( d e f ) ( s v y ) = ( dr + es + ft  du + ev + fw  dx + ey + fz )
 *	( h i j ) ( t w z )	  ( hr + hs + ht  iu + iv + iw  jx + jy + jz )
 */
PUBLIC void Matrix3x3Multiply( mat3_t in1, mat3_t in2, mat3_t out ) 
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

/**
 * \brief Set matrix to the identity matrix (unit matrix).
 * \param[in/out] in1 4x4 matrix.
 * \return 
 * \note 
 */
PUBLIC void MatrixIdentity( mat4_t matrix )
{
	matrix[ 0] = 1.0;  matrix[ 1] = 0.0;  matrix[ 2] = 0.0;  matrix[ 3] = 0.0;
	matrix[ 4] = 0.0;  matrix[ 5] = 1.0;  matrix[ 6] = 0.0;  matrix[ 7] = 0.0;
	matrix[ 8] = 0.0;  matrix[ 9] = 0.0;  matrix[10] = 1.0;  matrix[11] = 0.0;
	matrix[12] = 0.0;  matrix[13] = 0.0;  matrix[14] = 0.0;  matrix[15] = 1.0;
}

/**
 * \brief Invert a matrix.
 * \param[in] in 4x4 Input matrix
 * \param[out] out 4x4 Output matrix.
 * \return 
 * \note Matrix MUST be orthonormal
 */
PUBLIC void MatrixInvert( mat4_t in, mat4_t out )
{
	// Transpose rotation
	out[ 0] = in[ 0];  out[ 1] = in[ 4];  out[ 2] = in[ 8];
	out[ 4] = in[ 1];  out[ 5] = in[ 5];  out[ 6] = in[ 9];
	out[ 8] = in[ 2];  out[ 9] = in[ 6];  out[10] = in[10];

	// Clear shearing terms
	out[3] = 0.0f; out[7] = 0.0f; out[11] = 0.0f; out[15] = 1.0f;

	// Translation is minus the dot of translation and rotations
	out[12] = -(in[12] * in[ 0]) - (in[13] * in[ 1]) - (in[14] * in[ 2]);
	out[13] = -(in[12] * in[ 4]) - (in[13] * in[ 5]) - (in[14] * in[ 6]);
	out[14] = -(in[12] * in[ 8]) - (in[13] * in[ 9]) - (in[14] * in[10]);
}

/**
 * \brief Multiply a vector by a matrix.
 * \param[in] vectorIn Input vector.
 * \param[in] m Input matrix.
 * \param[out] vectorOut Output vector.
 * \return 
 * \note
 */
PUBLIC void VectorMatrixMultiply( vec3_t vectorIn, mat4_t m, vec3_t vectorOut )
{
	vectorOut[0] = (vectorIn[0] * m[ 0]) + (vectorIn[1] * m[ 4]) + (vectorIn[2] * m[ 8]) + m[12];
	vectorOut[1] = (vectorIn[0] * m[ 1]) + (vectorIn[1] * m[ 5]) + (vectorIn[2] * m[ 9]) + m[13];
	vectorOut[2] = (vectorIn[0] * m[ 2]) + (vectorIn[1] * m[ 6]) + (vectorIn[2] * m[10]) + m[14];
}

/**
 * \brief Multiply a vector by just the 3x3 portion of a matrix.
 * \param[in] in Input vector.
 * \param[in] m Input matrix.
 * \param[out] out Output vector.
 * \return 
 * \note
 */
PUBLIC void VectorMatrix3x3Multiply( vec3_t in, mat4_t m, vec3_t out )
{
	out[0] = (in[0] * m[ 0]) + (in[1] * m[ 4]) + (in[2] * m[ 8]);
	out[1] = (in[0] * m[ 1]) + (in[1] * m[ 5]) + (in[2] * m[ 9]);
	out[2] = (in[0] * m[ 2]) + (in[1] * m[ 6]) + (in[2] * m[10]);
}

