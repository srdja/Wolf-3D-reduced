/*

	Copyright (C) 2004 Michael Liebscher

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
 *	openal_binding.h: Interface to OpenAL library.
 *	
 *	Author:	Michael Liebscher <johnnycanuck@users.sourceforge.net>	
 *	Date:	2004
 *
 */


#ifndef __OPENAL_BINDING_H__
#define __OPENAL_BINDING_H__



#include <AL/alc.h>
#include <AL/al.h>

#include "platform.h"

#ifndef _WIN32
#define ALC_API
#endif

_boolean OpenAL_Init( const char *dllname );
void OpenAL_Shutdown( void );



typedef ALC_API ALCvoid (ALC_APIENTRY * ALCCLOSEDEVICE)(ALCdevice *pDevice);
typedef ALC_API ALCcontext*(ALC_APIENTRY * ALCCREATECONTEXT)(ALCdevice *device, const ALCint *attrList);
typedef ALC_API ALCvoid (ALC_APIENTRY * ALCDESTROYCONTEXT)(ALCcontext *context);
typedef ALC_API ALCdevice* (ALC_APIENTRY * ALCGETCONTEXTSDEVICE)(ALCcontext *pContext);
typedef ALC_API ALCcontext * (ALC_APIENTRY * ALCGETCURRENTCONTEXT)(ALCvoid);
typedef ALC_API ALCenum (ALC_APIENTRY * ALCGETENUMVALUE)(ALCdevice *device, const ALCubyte *enumName);
typedef ALC_API ALCenum (ALC_APIENTRY * ALCGETERROR)(ALCdevice *device);
typedef ALC_API ALCvoid (ALC_APIENTRY * ALCGETINTEGERV)(ALCdevice *device,ALCenum param,ALsizei size,ALCint *data);
typedef ALC_API ALCvoid *  (ALC_APIENTRY * ALCGETPROCADDRESS)(ALCdevice *device, const ALCubyte *funcName);
typedef ALC_API const ALCubyte* (ALC_APIENTRY * ALCGETSTRING)(ALCdevice *device,ALCenum param);
typedef ALC_API ALCboolean (ALC_APIENTRY * ALCISEXTENSIONPRESENT)(ALCdevice *device, const ALCubyte *extName);
typedef ALC_API ALCboolean (ALC_APIENTRY * ALCMAKECONTEXTCURRENT)(ALCcontext *context);
typedef ALC_API ALCdevice* (ALC_APIENTRY * ALCOPENDEVICE)(const ALCubyte *deviceName);
typedef ALC_API ALCvoid (ALC_APIENTRY * ALCPROCESSCONTEXT)(ALCcontext *pContext);
typedef ALC_API ALCvoid (ALC_APIENTRY * ALCSUSPENDCONTEXT)(ALCcontext *pContext);



typedef ALC_API ALvoid (ALC_APIENTRY * ALGENBUFFERS)(ALsizei n, ALuint* bufferNames);
typedef ALC_API ALvoid (ALC_APIENTRY * ALDELETEBUFFERS)(ALsizei n, const ALuint* bufferNames);
typedef ALC_API ALboolean (ALC_APIENTRY * ALISBUFFER)(ALuint bufferName);
typedef ALC_API ALvoid (ALC_APIENTRY * ALBUFFERDATA)(ALuint bufferName, ALenum format, const ALvoid* data, ALsizei size, ALsizei freq);
typedef ALC_API ALvoid (ALC_APIENTRY * ALGETBUFFERF)(ALuint bufferName, ALenum param, ALfloat* value);
typedef ALC_API ALvoid (ALC_APIENTRY * ALGETBUFFERI)(ALuint bufferName, ALenum param, ALint* value);
typedef ALC_API ALvoid (ALC_APIENTRY * ALENABLE)(ALenum capability);
typedef ALC_API ALvoid (ALC_APIENTRY * ALDISABLE)(ALenum capability);
typedef ALC_API ALvoid (ALC_APIENTRY * ALDOPPLERFACTOR)(ALfloat value);
typedef ALC_API ALvoid (ALC_APIENTRY * ALDOPPLERVELOCITY)(ALfloat value);
typedef ALC_API ALvoid (ALC_APIENTRY * ALDISTANCEMODEL)(ALenum value);
typedef ALC_API ALboolean (ALC_APIENTRY * ALGETBOOLEAN)(ALenum param);
typedef ALC_API ALvoid (ALC_APIENTRY * ALGETBOOLEANV)(ALenum param, ALboolean* data);
typedef ALC_API ALdouble (ALC_APIENTRY * ALGETDOUBLE)(ALenum param);
typedef ALC_API ALvoid (ALC_APIENTRY * ALGETDOUBLEV)(ALenum param, ALdouble* data);
typedef ALC_API ALfloat (ALC_APIENTRY * ALGETFLOAT)(ALenum param);
typedef ALC_API ALvoid (ALC_APIENTRY * ALGETFLOATV)(ALenum param, ALfloat* data);
typedef ALC_API ALint (ALC_APIENTRY * ALGETINTEGER)(ALenum param);
typedef ALC_API ALvoid (ALC_APIENTRY * ALGETINTEGERV)(ALenum param, ALint* data);
typedef ALC_API ALenum (ALC_APIENTRY * ALGETENUMVALUE)(const ALubyte* ename);
typedef ALC_API ALenum (ALC_APIENTRY * ALGETERROR)(ALvoid);
typedef ALC_API ALvoid* (ALC_APIENTRY * ALGETPROCADDRESS)(const ALubyte* fname);
typedef ALC_API const ALubyte* (ALC_APIENTRY * ALGETSTRING)(ALenum param);
typedef ALC_API ALboolean (ALC_APIENTRY * ALISEXTENSIONPRESENT)(const ALubyte* ename);
typedef ALC_API ALboolean (ALC_APIENTRY * ALISENABLED)(ALenum capability);
typedef ALC_API ALvoid (ALC_APIENTRY * ALLISTENERF)(ALenum param, ALfloat value);
typedef ALC_API ALvoid (ALC_APIENTRY * ALLISTENER3F)(ALenum param, ALfloat v1, ALfloat v2, ALfloat v3);
typedef ALC_API ALvoid (ALC_APIENTRY * ALLISTENERFV)(ALenum param, const ALfloat* values);
typedef ALC_API ALvoid (ALC_APIENTRY * ALLISTENERI)(ALenum param, ALint value);
typedef ALC_API ALvoid (ALC_APIENTRY * ALGETLISTENERF)(ALenum param, ALfloat* value);
typedef ALC_API ALvoid (ALC_APIENTRY * ALGETLISTENER3F)(ALenum param, ALfloat* v1, ALfloat* v2, ALfloat* v3);
typedef ALC_API ALvoid (ALC_APIENTRY * ALGETLISTENERFV)(ALenum param, ALfloat* values);
typedef ALC_API ALvoid (ALC_APIENTRY * ALGETLISTENERI)(ALenum param, ALint* value);
typedef ALC_API ALvoid (ALC_APIENTRY * ALGENSOURCES)(ALsizei n, ALuint* sourceNames);
typedef ALC_API ALvoid (ALC_APIENTRY * ALDELETESOURCES)(ALsizei n, const ALuint* sourceNames);
typedef ALC_API ALboolean (ALC_APIENTRY * ALISSOURCE)(ALuint sourceName);
typedef ALC_API ALvoid (ALC_APIENTRY * ALSOURCEF)(ALuint sourceName, ALenum param, ALfloat value);
typedef ALC_API ALvoid (ALC_APIENTRY * ALSOURCEFV)(ALuint sourceName, ALenum param, const ALfloat* values);
typedef ALC_API ALvoid (ALC_APIENTRY * ALSOURCE3F)(ALuint sourceName, ALenum param, ALfloat v1, ALfloat v2, ALfloat v3);
typedef ALC_API ALvoid (ALC_APIENTRY * ALSOURCEI)(ALuint sourceName, ALenum param, ALint value);
typedef ALC_API ALvoid (ALC_APIENTRY * ALGETSOURCEF)(ALuint sourceName, ALenum param, ALfloat* value);
typedef ALC_API ALvoid (ALC_APIENTRY * ALGETSOURCE3F)(ALuint sourceName, ALenum param, ALfloat* v1, ALfloat* v2, ALfloat* v3);
typedef ALC_API ALvoid (ALC_APIENTRY * ALGETSOURCEFV)(ALuint sourceName, ALenum param, ALfloat* values);
typedef ALC_API ALvoid (ALC_APIENTRY * ALGETSOURCEI)(ALuint sourceName, ALenum param, ALint* value);
typedef ALC_API ALvoid (ALC_APIENTRY * ALSOURCEPLAY)(ALuint sourceName);
typedef ALC_API ALvoid (ALC_APIENTRY * ALSOURCEPLAYV)(ALsizei n, const ALuint* sourceNames);
typedef ALC_API ALvoid (ALC_APIENTRY * ALSOURCEPAUSE)(ALuint sourceName);
typedef ALC_API ALvoid (ALC_APIENTRY * ALSOURCEPAUSEV)(ALsizei n, const ALuint* sourceNames);
typedef ALC_API ALvoid (ALC_APIENTRY * ALSOURCESTOP)(ALuint sourceName);
typedef ALC_API ALvoid (ALC_APIENTRY * ALSOURCESTOPV)(ALsizei n, const ALuint* sourceNames);
typedef ALC_API ALvoid (ALC_APIENTRY * ALSOURCEREWIND)(ALuint sourceName);
typedef ALC_API ALvoid (ALC_APIENTRY * ALSOURCEREWINDV)(ALsizei n, const ALuint* sourceNames);
typedef ALC_API ALvoid (ALC_APIENTRY * ALSOURCEQUEUEBUFFERS)(ALuint sourceName, ALsizei n, const ALuint* buffers);
typedef ALC_API ALvoid (ALC_APIENTRY * ALSOURCEUNQUEUEBUFFERS)(ALuint sourceName, ALsizei n, ALuint* buffers);



ALCCLOSEDEVICE			pfalcCloseDevice;
ALCCREATECONTEXT		pfalcCreateContext;
ALCDESTROYCONTEXT		pfalcDestroyContext;
ALCGETCONTEXTSDEVICE		pfalcGetContextsDevice;
ALCGETCURRENTCONTEXT		pfalcGetCurrentContext;
ALCGETENUMVALUE			pfalcGetEnumValue;
ALCGETERROR			pfalcGetError;
ALCGETINTEGERV			pfalcGetIntegerv;
ALCGETPROCADDRESS		pfalcGetProcAddress;
ALCGETSTRING			pfalcGetString;
ALCISEXTENSIONPRESENT		pfalcIsExtensionPresent;
ALCMAKECONTEXTCURRENT		pfalcMakeContextCurrent;
ALCOPENDEVICE			pfalcOpenDevice;
ALCPROCESSCONTEXT		pfalcProcessContext;
ALCSUSPENDCONTEXT		pfalcSuspendContext;

ALGENBUFFERS			pfalGenBuffers;
ALDELETEBUFFERS			pfalDeleteBuffers;
ALISBUFFER			pfalIsBuffer;
ALBUFFERDATA			pfalBufferData;
ALGETBUFFERF			pfalGetBufferf;
ALGETBUFFERI			pfalGetBufferi;
ALENABLE			pfalEnable;
ALDISABLE			pfalDisable;
ALDOPPLERFACTOR			pfalDopplerFactor;
ALDOPPLERVELOCITY		pfalDopplerVelocity;
ALDISTANCEMODEL			pfalDistanceModel;
ALGETBOOLEAN			pfalGetBoolean;
ALGETBOOLEANV			pfalGetBooleanv;
ALGETDOUBLE			pfalGetDouble;
ALGETDOUBLEV			pfalGetDoublev;
ALGETFLOAT			pfalGetFloat;
ALGETFLOATV			pfalGetFloatv;
ALGETINTEGER			pfalGetInteger;
ALGETINTEGERV			pfalGetIntegerv;
ALGETENUMVALUE			pfalGetEnumValue;
ALGETERROR			pfalGetError;
ALGETPROCADDRESS		pfalGetProcAddress;
ALGETSTRING			pfalGetString;
ALISEXTENSIONPRESENT		pfalIsExtensionPresent;
ALISENABLED			pfalIsEnabled;
ALLISTENERF			pfalListenerf;
ALLISTENER3F			pfalListener3f;
ALLISTENERFV			pfalListenerfv;
ALLISTENERI			pfalListeneri;
ALGETLISTENERF			pfalGetListenerf;
ALGETLISTENER3F			pfalGetListener3f;
ALGETLISTENERFV			pfalGetListenerfv;
ALGETLISTENERI			pfalGetListeneri;
ALGENSOURCES			pfalGenSources;
ALDELETESOURCES			pfalDeleteSources;
ALISSOURCE			pfalIsSource;
ALSOURCEF			pfalSourcef;
ALSOURCEFV			pfalSourcefv;
ALSOURCE3F			pfalSource3f;
ALSOURCEI			pfalSourcei;
ALGETSOURCEF			pfalGetSourcef;
ALGETSOURCE3F			pfalGetSource3f;
ALGETSOURCEFV			pfalGetSourcefv;
ALGETSOURCEI			pfalGetSourcei;
ALSOURCEPLAY			pfalSourcePlay;
ALSOURCEPLAYV			pfalSourcePlayv;
ALSOURCEPAUSE			pfalSourcePause;
ALSOURCEPAUSEV			pfalSourcePausev;
ALSOURCESTOP			pfalSourceStop;
ALSOURCESTOPV			pfalSourceStopv;
ALSOURCEREWIND			pfalSourceRewind;
ALSOURCEREWINDV			pfalSourceRewindv;
ALSOURCEQUEUEBUFFERS		pfalSourceQueueBuffers;
ALSOURCEUNQUEUEBUFFERS		pfalSourceUnqueueBuffers;



#endif /* __OPENAL_BINDING_H__ */
