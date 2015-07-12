/*

    Copyright (C) 2004 Michael Liebscher <johnnycanuck@users.sourceforge.net>
    Copyright (C) 1997-2001 Id Software, Inc.

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

    See the GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#include "../env/keys.h"
#include "../env/keycodes.h"
#include "../env/client.h"

cvar_t  *in_joystick;


// none of these cvars are saved over a session
// this means that advanced controller configuration needs to be executed
// each time.  this avoids any problems with getting back to a default usage
// or when changing from one controller to another.  this way at least something
// works.
cvar_t  *joy_name;
cvar_t  *joy_advanced;
cvar_t  *joy_advaxisx;
cvar_t  *joy_advaxisy;
cvar_t  *joy_advaxisz;
cvar_t  *joy_advaxisr;
cvar_t  *joy_advaxisu;
cvar_t  *joy_advaxisv;
cvar_t  *joy_forwardthreshold;
cvar_t  *joy_sidethreshold;
cvar_t  *joy_pitchthreshold;
cvar_t  *joy_yawthreshold;
cvar_t  *joy_forwardsensitivity;
cvar_t  *joy_sidesensitivity;
cvar_t  *joy_pitchsensitivity;
cvar_t  *joy_yawsensitivity;
cvar_t  *joy_upthreshold;
cvar_t  *joy_upsensitivity;

void IN_StartupJoystick (void)
{
    // joystick variables
    in_joystick             = Cvar_Get ("in_joystick",               "0",            CVAR_ARCHIVE);
    joy_name                = Cvar_Get ("joy_name",                  "joystick",     0);
    joy_advanced            = Cvar_Get ("joy_advanced",              "0",            0);
    joy_advaxisx            = Cvar_Get ("joy_advaxisx",              "0",            0);
    joy_advaxisy            = Cvar_Get ("joy_advaxisy",              "0",            0);
    joy_advaxisz            = Cvar_Get ("joy_advaxisz",              "0",            0);
    joy_advaxisr            = Cvar_Get ("joy_advaxisr",              "0",            0);
    joy_advaxisu            = Cvar_Get ("joy_advaxisu",              "0",            0);
    joy_advaxisv            = Cvar_Get ("joy_advaxisv",              "0",            0);
    joy_forwardthreshold    = Cvar_Get ("joy_forwardthreshold",      "0.15",         0);
    joy_sidethreshold       = Cvar_Get ("joy_sidethreshold",     "0.15",         0);
    joy_upthreshold         = Cvar_Get ("joy_upthreshold",           "0.15",         0);
    joy_pitchthreshold      = Cvar_Get ("joy_pitchthreshold",        "0.15",         0);
    joy_yawthreshold        = Cvar_Get ("joy_yawthreshold",          "0.15",         0);
    joy_forwardsensitivity  = Cvar_Get ("joy_forwardsensitivity",   "-1",           0);
    joy_sidesensitivity     = Cvar_Get ("joy_sidesensitivity",       "-1",           0);
    joy_upsensitivity       = Cvar_Get ("joy_upsensitivity",     "-1",           0);
    joy_pitchsensitivity    = Cvar_Get ("joy_pitchsensitivity",      "1",            0);
    joy_yawsensitivity      = Cvar_Get ("joy_yawsensitivity",        "-1",           0);


}
