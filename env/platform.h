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



// Windows
/////////////////////////////////////////////////

#if defined(WINDOWS) || defined(_WINDOWS) || defined(_WIN32) || defined(_WIN64)

	#define PLATFORM_NAME "Windows"

    #define	PATH_SEP '\\'

#endif


// Linux
/////////////////////////////////////////////////

#if defined(linux) || defined(__linux__)

	#define PLATFORM_NAME "Linux"

	#define	PATH_SEP '/'

#endif


// BSD
/////////////////////////////////////////////////

#if defined(__FreeBSD__)

	#define PLATFORM_NAME "FreeBSD"

	#define	PATH_SEP '/'

#endif

#if defined(__NetBSD__)

	#define PLATFORM_NAME "NetBSD"

	#define	PATH_SEP '/'

#endif

#if defined(__OpenBSD__)

	#define PLATFORM_NAME "OpenBSD"

	#define	PATH_SEP '/'

#endif


// Solaris
/////////////////////////////////////////////////

#if defined(sun) || defined(__sun__)

	#define PLATFORM_NAME "Solaris"

	#define	PATH_SEP '/'

#endif


// MAC
/////////////////////////////////////////////////

#if defined( macintosh )

    #define PLATFORM_NAME "MAC OS"

    #define	PATH_SEP ':'

#endif


#if defined(__APPLE__) && defined(__MACH__)

	#define MACOS_X		1

    #define PLATFORM_NAME "MAC OS X"

	#define	PATH_SEP ':'

#endif


// Other
/////////////////////////////////////////////////

#ifndef PLATFORM_NAME

	#define PLATFORM_NAME "Unknown OS"
    #define	PATH_SEP '/'

#endif



/* Define CPUSTRING based on platform */

#if defined(_M_IX86) || defined(__i386__) || defined(__i486__) || defined(__i586__) || defined(__i686__)

	#define CPUSTRING       "x86"
	#define __ARCH_X86__    1

#elif defined(__amd64__) || defined(__X86_64__) || defined(__amd64) || (__x86_64)

	#define CPUSTRING	"x86_64"
	#define __ARCH_X64__	1

#elif defined(_M_ALPHA) || defined(__alpha__)

	#define CPUSTRING       "Alpha"

#elif defined(__powerpc__)

	#define CPUSTRING	"PowerPC"

#elif defined(__sparc__)

	#define CPUSTRING	"Sparc"

#else

	#define CPUSTRING       "Unknown CPU"

#endif



#if defined(WINDOWS) || defined(_WINDOWS)  || defined(_WIN32) || defined(_WIN64)

	#define OS_WINDOWS  1

#else

	#define OS_UNIX     1

#endif


/*
	correct numeric types:  W8, SW8, W16, SW16, W32, SW32, W64, SW64
	correct char types:	char
	correct misc types:	void, float, wtwtBoolean

	s       -signed
	XX      -Number of bits
*/
#if defined(_MSC_VER)

	typedef unsigned	__int8		W8,	*PW8;
	typedef signed		__int8		SW8,    *PSW8;
	typedef unsigned	__int16		W16,    *PW16;
	typedef signed		__int16		SW16,   *PSW16;
	typedef unsigned	__int32		W32,    *PW32;
	typedef signed		__int32		SW32,   *PSW32;
	typedef unsigned	__int64		W64,	*PW64;
	typedef signed		__int64		SW64,   *PSW64;
	#ifdef __ARCH_X86__
	typedef signed		__int32		INT_PTR;
	#elif __ARCH_X64__
	typedef signed		__int64		INT_PTR;
	#endif

#else

	#include <stdint.h>
	#include <inttypes.h>

	typedef	uint8_t				W8,	*PW8;
	typedef	int8_t				SW8,    *PSW8;
	typedef	uint16_t			W16,    *PW16;
	typedef	int16_t				SW16,   *PSW16;
	typedef	uint32_t			W32,    *PW32;
	typedef	int32_t				SW32,   *PSW32;
	typedef	uint64_t			W64,	*PW64;
	typedef	int64_t				SW64,   *PSW64;
	typedef intptr_t			INT_PTR;

#endif


/* Define NULL pointer value */
#ifndef NULL

        #ifdef __cplusplus
                #define NULL    0
        #else
                #define NULL    ((void *)0)
        #endif

#endif /* NULL */



/* Define INLINECALL keyword */
#ifndef INLINECALL

	#if defined(__cplusplus) || defined(__GNUC__)

		#define INLINECALL inline

	#elif defined(_WIN32) && !defined(__WATCOMC__)

		#define INLINECALL __inline

	#else

		#define INLINECALL	/* Not supported */

	#endif

#endif /* INLINECALL */



typedef enum
{

   false = 0,
   true  = 1

} _boolean;


typedef unsigned char 		byte;



#define Swap16( x )		( ( (((W16) (x)) & 0x00FF) << 8 ) | ( (((W16) (x))& 0xFF00) >> 8) )
#define Swap32( x )		( ( ((W32) (x)) & 0xFF000000) >> 24 ) | ( ((( (W32) (x) ) & 0xFF0000) >> 8)  ) | ( ((( (W32) (x) ) & 0xFF00) << 8 ) ) | ( (( (W32) (x) ) & 0xFF) << 24 )
#define Swap64( x )		( ( Swap32( (x) & 0xFFFFFFFF ) << 32 ) | Swap32( (x) >> 32 ) )


#if defined(__ARCH_X86__) || defined(__ARCH_X64__)  // Little endian


	#define LITTLE_ENDIAN_SYSTEM	1


	#define BigShort( x )           Swap16( x )
	#define LittleShort( x )        ( x )

	#define BigLong( x )            Swap32( x )
	#define LittleLong( x )         ( x )

	#define BigFloat( x )           SwapFloat( x )
	#define LittleFloat( x )        ( x )

	#define Big64( x )              Swap64( x )
	#define Little64( x )           ( x )


#else // Big endian

	#define BIG_ENDIAN_SYSTEM	1


	#define BigShort( x )           ( x )
	#define LittleShort( x )        Swap16( x )

	#define BigLong( x )            ( x )
	#define LittleLong( x )         Swap32( x )

	#define BigFloat( x )           ( x )
	#define LittleFloat( x )        SwapFloat( x )

	#define Big64( x )              ( x )
	#define Little64( x )           Swap64( x )

#endif





#endif /* __PLATFORM_H__ */

