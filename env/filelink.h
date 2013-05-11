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
 *	filelink.h:  
 *	
 *	Author:	Michael Liebscher	<johnnycanuck@users.sourceforge.net>	    
 *	Date:	2004   
 *
 *	Acknowledgement:
 *	This code was derived from Quake II, and was originally
 *	written by Id Software, Inc.
 *
 */

/*
	Notes:	
	This module is implemented by files.c.

*/

#ifndef __FILELINK_H__
#define __FILELINK_H__


#include "zippak.h"

typedef struct filelink_s
{
	struct	filelink_s	*next;
	char	*from;
	int		fromlength;
	char	*to;

} filelink_t;


typedef struct searchpath_s
{
	char	filename[ MAX_OSPATH ];
	pack_t	*pack;		// only one of filename / pack will be used
	
	struct	searchpath_s *next;

} searchpath_t;


filelink_t		*fs_links;

searchpath_t	*fs_searchpaths;
searchpath_t	*fs_base_searchpaths;	// without gamedirs




#endif /* __FILELINK_H__ */
