/*

    Copyright (C) 2005 Michael Liebscher <johnnycanuck@users.sourceforge.net>

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
 * \file num_type.h
 * \brief System dependant float/double to integer conversions.
 * \author Michael Liebscher
 * \date 2005
 */


#ifndef __NUM_TYPE_H__
#define __NUM_TYPE_H__

#include "platform.h"

typedef W8  colour3_t[ 3 ]; // RGB
typedef W8  colour4_t[ 4 ]; // RGBA

#define BIT( x )    ( 1 << (x) )
#define FloatToInt( a ) (SW32)(a)

#endif /* __NUM_TYPE_H__ */
