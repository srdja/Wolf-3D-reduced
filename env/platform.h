/*

    Copyright (C) 2005-2007 Michael Liebscher <johnnycanuck@users.sourceforge.net>


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
 * \file platform.h
 * \brief System dependant #defines and macros.
 * \author Michael Liebscher
 * \date 2005-2007
 */



#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>

#define PATH_SEP '/'

typedef uint8_t  W8;
typedef uint8_t *PW8;
typedef int8_t              SW8,    *PSW8;
typedef uint16_t            W16,    *PW16;
typedef int16_t             SW16,   *PSW16;
typedef uint32_t            W32,    *PW32;
typedef int32_t             SW32,   *PSW32;
typedef uint64_t            W64,    *PW64;
typedef int64_t             SW64,   *PSW64;
typedef intptr_t            INT_PTR;


typedef enum {
    false = 0,
    true  = 1
} _boolean;

typedef unsigned char       byte;

#define LittleShort( x )        ( x )
#define LittleLong( x )         ( x )

#endif /* __PLATFORM_H__ */

