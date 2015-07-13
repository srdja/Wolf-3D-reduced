/*

    Copyright (C) 2004-2012 Michael Liebscher <johnnycanuck@users.sourceforge.net>

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
 * \file openal_main.c
 * \brief Interface to Sound Device.
 * \author Michael Liebscher
 * \date 2004-2012
 */

#include <string.h>

#include "common.h"
#include "com_string.h"
#include "openal_binding.h"

#define OPENAL_DLL_NAME     "libopenal.so"


PRIVATE ALCcontext *Context;
PRIVATE ALCdevice *Device;


cvar_t  *s_driver;
cvar_t  *s_device;

char *deviceList;
const char *sound_devices[ 12 ];

W16 numSoundDevices, numDefaultSoundDevice;



/**
 * \brief Get OpenAL device list.
 * \return Returns list of devices
 */
PRIVATE char *Sound_Device_getDeviceList (void)
{
    char deviceName[ 256 ];

    com_strlcpy (deviceName, s_device->string, sizeof (deviceName));

    if (pfalcIsExtensionPresent (NULL, (ALubyte *)"ALC_ENUMERATION_EXT") == AL_TRUE) {
        // try out enumeration extension
        deviceList = (char *)pfalcGetString (NULL, ALC_DEVICE_SPECIFIER);

        for (numSoundDevices = 0 ; numSoundDevices < 12 ; ++numSoundDevices) {
            sound_devices[ numSoundDevices ] = NULL;
        }

        for (numSoundDevices = 0 ; numSoundDevices < 12 ; ++numSoundDevices) { // son las mismas condiciones que el anterior (?)
            sound_devices[ numSoundDevices ] = deviceList;

            if (strcmp (sound_devices[ numSoundDevices ], deviceName) == 0) {
                numDefaultSoundDevice = numSoundDevices;
                Com_DPrintf ("Default sound device = %d\n", numDefaultSoundDevice);
            }

            deviceList += strlen (deviceList);

            if (deviceList[ 0 ] == 0) {
                if (deviceList[ 1 ] == 0) {
                    break;
                } else {
                    deviceList += 1;
                }
            }

        } // End for numSoundDevices = 0 ; numSoundDevices < 12 ; ++numSoundDevices
    }

    return deviceList;
}

/**
 * \brief Register OpenAL cvars.
 */
PRIVATE void Sound_Device_Register (void)
{
    s_driver = Cvar_Get ("s_driver", OPENAL_DLL_NAME, CVAR_ARCHIVE);
    s_device = Cvar_Get ("s_device", "", CVAR_LATCH | CVAR_ARCHIVE);
}


/**
 * \brief Setup OpenAL sound device.
 * \return true on success, otherwise false.
 * \note Call Sound_Device_Shutdown() when you are done.
 */
PUBLIC _boolean Sound_Device_Setup (void)
{
    Sound_Device_Register();

    // Initialize our OpenAL dynamic bindings
    if (! OpenAL_Init (s_driver->string)) {
        goto failed;
    }


    Sound_Device_getDeviceList();


    Device = pfalcOpenDevice ((ALCubyte *) ((s_device->string[ 0 ]) ? s_device->string : NULL));

    if (Device == NULL) {
        goto failed;
    }

    // Create context(s)
    Context = pfalcCreateContext (Device, NULL);

    if (Context == NULL) {
        goto failed;
    }

    // Set active context
    pfalcGetError (Device);
    pfalcMakeContextCurrent (Context);

    if (pfalcGetError (Device) != ALC_NO_ERROR) {
        goto failed;
    }
    return true;

failed:

    if (Context) {
        pfalcDestroyContext (Context);
        Context = NULL;
    }

    if (Device) {
        pfalcCloseDevice (Device);
        Device = NULL;
    }

    OpenAL_Shutdown();

    return false;
}

/**
 * \brief Shutdown OpenAL sound device.
 */
PUBLIC void Sound_Device_Shutdown (void)
{
    if (Context) {
        pfalcMakeContextCurrent (NULL);
        pfalcDestroyContext (Context);

        Context = NULL;
    }

    if (Device) {
        pfalcCloseDevice (Device);

        Device = NULL;
    }

    OpenAL_Shutdown();
}

