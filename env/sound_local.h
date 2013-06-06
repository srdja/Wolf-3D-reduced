/*

	Copyright (C) 2004-2005 Michael Liebscher
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
 *	sound_local.h:
 *	
 *	Author:	Michael Liebscher <johnnycanuck@users.sourceforge.net>	
 *	Date:	2004
 *
 *	Acknowledgement:
 *	This code was derived from Quake II, and was originally
 *	written by Id Software, Inc.
 *
 *	Acknowledgement:
 *	Portion of this code was derived from Quake II Evolved.
 *
 */


#ifndef __SOUND_LOCAL_H__
#define __SOUND_LOCAL_H__

#include "openal_binding.h"

#include "com_math.h"



typedef struct sfx_s 
{
	char				name[ MAX_GAMEPATH ];
	_boolean			defaulted;
	_boolean			loaded;

	int				samples;
	int				rate;
	unsigned			format;	/* Sound samples: format specifier */
	unsigned			bufferNum;

	struct sfx_s			*nextHash;

} sfx_t;

// A playSound will be generated by each call to S_StartSound.
// When the mixer reaches playSound->beginTime, the playSound will be
// assigned to a channel.
typedef struct playSound_s 
{
	struct playSound_s		*prev, *next;
	sfx_t				*sfx;
	int				entNum;
	int				entChannel;
	_boolean			fixedPosition;	// Use position instead of fetching entity's origin
	vec3_t				position;	// Only use if fixedPosition is set
	float				volume;
	float				attenuation;
	int				beginTime;	// Begin at this time

} playSound_t;

typedef struct 
{
	_boolean			streaming;
	sfx_t				*sfx;			// NULL if unused
	int				entNum;			// To allow overriding a specific sound
	int				entChannel;
	int				startTime;		// For overriding oldest sounds
	_boolean			loopSound;		// Looping sound
	int				loopNum;		// Looping entity number
	int				loopFrame;		// For stopping looping sounds
	_boolean			fixedPosition;		// Use position instead of fetching entity's origin
	vec3_t				position;		// Only use if fixedPosition is set
	float				volume;
	float				distanceMult;
	unsigned			sourceName;		// OpenAL sourceName

} channel_t;

typedef struct 
{
	vec3_t				position;
	vec3_t				velocity;
	float				orientation[ 6 ];

} listener_t;



void		Sound_SoundList_f( void );
_boolean	Sound_LoadSound( sfx_t *sfx );
sfx_t		*Sound_FindSound( const char *name );
void 		Sound_FreeSounds( void );

_boolean	Sound_Device_Setup( void );
void		Sound_Device_Shutdown( void );


#endif /* __SOUND_LOCAL_H__ */

