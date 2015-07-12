/*

    Copyright (C) 2006-2009 Michael Liebscher <johnnycanuck@users.sourceforge.net>

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
 * \file platform.c
 * \brief Portable byte swapping.
 * \author  Michael Liebscher
 * \date 2006-2009
 */


#include "platform.h"


/**
 * \brief Endian byte swapping on Float value.
 * \param[in] f Float value to byte swap.
 * \return Byte swapped float value.
 */
float SwapFloat (const float *f)
{

    union {
        float f;
        W32 b;

    } u1, u2;

    u1.f = *f;
    u2.b = Swap32 (u1.b);

    return u2.f;

}
