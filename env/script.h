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

/*
 *	script.h: Core script parser.
 *
 *	Author:	Michael Liebscher	<johnnycanuck@users.sourceforge.net>
 *
 */

#ifndef __SCRIPT_H__
#define __SCRIPT_H__


#include "common.h"


#define ENDOFSTREAM		'\0'


enum
{
	TT_INTEGER,
	TT_FLOAT

} NumberType;




typedef char *(*PropertyParser) ( W8 *in, W32 *linenumber );

typedef struct Tag_Property_s
{
	char                *name;
	PropertyParser       parsing_function;

} Tag_Property_t;


typedef struct
{
	double	Float;
	long	Integer;

} decimalType_t;

typedef struct datatype_s
{
	int		type;
	char	*string;
	decimalType_t value;

} dataType_t;



_boolean script_Parse( const char *filename, Tag_Property_t *tagProperties, W32 tagProperties_size, int tag );

W8 *script_ReadString( W8 *ptr, W8 **out );
W8 *script_ReadQuoteString( W8 *ptr, W8 **out );
W8 *script_ReadNumber( W8 *ptr, int numerictype, decimalType_t *nt );
W8 *script_lookforCharacter( W8 *ptr, char character, _boolean bNewline );
W8 *script_ignoreTag( W8 *ptr );
W8 *script_ignoreString( W8 *ptr );
W8 *script_ignoreWhiteSpace( W8 *ptr );


#endif /* __SCRIPT_H__ */
