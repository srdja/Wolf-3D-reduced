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
#include <stdbool.h>

#define PATH_SEP '/'

typedef uint8_t  W8;
typedef uint8_t *PW8;
typedef uint16_t            W16,    *PW16;
typedef uint32_t            W32;
typedef int32_t             SW32;
typedef intptr_t            INT_PTR;

typedef bool _boolean;

#endif /* __PLATFORM_H__ */

