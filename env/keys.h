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

/*
 *	keys.h:   Portable key tracking module.
 *
 *	Author:  Id Software, Inc.
 *	Date:    1997-2001
 *
 */

/*

	Use this module to track of input from mouse, joystick and keyboard.
	Call Key_Init() to initialize module.

	This module is implemented by keys.c

*/

#ifndef __KEYS_H__
#define __KEYS_H__

#include <stdio.h>
#include "platform.h"


#define	MAX_KEYS		256

typedef struct
{
	_boolean	down;
	int			repeats;	// if > 1, it is autorepeating
	char		*binding;

} qkey_t;


extern	_boolean	key_overstrikeMode;
extern	qkey_t		keys[MAX_KEYS];

extern char		*keybindings[ 256 ];
extern int		key_repeats[ 256 ];

extern int	anykeydown;
extern char chat_buffer[];
extern int chat_bufferlen;
extern _boolean	chat_team;


void Key_Init( void );
void Key_Event( int key, _boolean down, unsigned time );
void Key_WriteBindings( FILE *f );
void Key_SetBinding( int keynum, char *binding );
char *Key_GetBinding( const int keynum );
int Key_GetKey( const char *binding );
void Key_ClearStates( void );
char *Key_KeynumToString( int keynum );



#endif /* __KEYS_H__ */

