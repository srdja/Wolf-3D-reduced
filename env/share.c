/*

    Copyright (C) 1997-2001 Id Software, Inc.

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

#include <stdio.h>
#include <stdarg.h>


#include "platform.h"
#include "common_utils.h"
#include "cmd.h"


/*
-----------------------------------------------------------------------------
 Function: Com_PageInMemory()

 Parameters:

 Returns:

 Notes:

-----------------------------------------------------------------------------
*/
int paged_total;

PUBLIC void Com_PageInMemory (PW8 buffer, int size)
{
    int     i;

    for (i = size - 1 ; i > 0 ; i -= 4096) {
        paged_total += buffer[ i ];
    }
}

