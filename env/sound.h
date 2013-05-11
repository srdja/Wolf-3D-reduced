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
 *	sound.h:
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

#ifndef __SOUND_H__
#define __SOUND_H__

#include "platform.h"
#include "com_math.h"

#include "sound_local.h"

#include "cvar.h"


// Sound channels
// Channel 0 never willingly overrides
// Other channels (1-7) always override a playing sound on that channel
#define	CHAN_AUTO               0
#define	CHAN_WEAPON             1
#define	CHAN_VOICE              2
#define	CHAN_ITEM               3
#define	CHAN_BODY               4
// Modifier flags
#define	CHAN_NO_PHS_ADD			8	// Send to all clients, not just ones in PHS (ATTN 0 will also do this)
#define	CHAN_RELIABLE			16	// Send by reliable message, not datagram

// Sound attenuation values
#define	ATTN_NONE               0	// Full volume the entire level
#define	ATTN_NORM               1
#define	ATTN_IDLE               2
#define	ATTN_STATIC             3	// Diminish very rapidly with distance


extern cvar_t *s_device;
extern cvar_t *s_masterVolume;
extern cvar_t *s_sfxVolume;
extern cvar_t *s_musicVolume;

extern const char *sound_devices[];
extern W16 numSoundDevices;
extern W16 numDefaultSoundDevice;

extern _boolean	sound_initialized;

void Sound_Init( void );
void Sound_Shutdown( void );

void Sound_Update( const vec3_t position, const vec3_t velocity, const vec3_t at, const vec3_t up);

void Sound_Activate( _boolean active );

channel_t *Sound_PickChannel( W32 entNum, W32 entChannel );

sfx_t *Sound_RegisterSound( const char *name );
void Sound_StartLocalSound( const char *filename );

void Sound_StreamBGTrack( void );
void Sound_StartStreaming( void );
void Sound_StopStreaming( void );
void Sound_StartBGTrack( const char *introTrack, const char *loopTrack );
void Sound_StopBGTrack( void );

void Sound_StartSound( const vec3_t position, int entNum, int entChannel, sfx_t *sfx, float volume, float attenuation, int timeOfs );

void Sound_StopAllSounds( void );


#endif /* __SOUND_H__ */
