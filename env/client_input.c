/*

    Copyright (C) 2004 Michael Liebscher <johnnycanuck@users.sourceforge.net>
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

/**
 * @file client_input.c
 * @brief Builds an intended movement command to send to the server.
 * @author Michael Liebscher
 * @date 2004-2012
 * @author Id Software, Inc.
 * @date 1997-2001
 * @note This code was derived from Quake II, and was originally written by Id Software, Inc.
 */

#include <stdlib.h>
#include <string.h>

#include "keys.h"
#include "input.h"

extern  W32 sys_frame_time;
PRIVATE W32 frame_msec;
PRIVATE W32 old_sys_frame_time;

/*!
    @note

    KEY BUTTONS

    Continuous button event tracking is complicated by the fact that two different
    input sources (say, mouse button 1 and the control key) can both press the
    same button, but the button should only be released when both of the
    pressing key have been released.

    When a key event issues a button command (+forward, +attack, etc), it appends
    its key number as a parameter to the command so it can be matched up with
    the release.

    state bit 0 is the current state of the key
    state bit 1 is edge triggered on the up to down transition
    state bit 2 is edge triggered on the down to up transition


    Key_Event (int key, qboolean down, unsigned time);

      +mlook src time

*/

kbutton_t   in_left, in_right, in_forward, in_back;
kbutton_t   in_moveleft, in_moveright;
kbutton_t   in_strafe, in_speed, in_use, in_attack;

int     in_impulse;

/**
 * Called whenever a key goes into the down state.
 * @param[in] b Valid pointer to a kbutton_t structure.
 */
PRIVATE void KeyDown (kbutton_t *b)
{
    int k;
  //  char    *c;

  //  c = Cmd_Argv (1);
/*
    if (c[ 0 ]) {
        k = atoi (c);
    } else {
        k = -1;     // typed manually at the console for continuous down
    }
*/
    k = -1;
    if (k == b->down[ 0 ] || k == b->down[ 1 ]) {
        return;     // repeating key
    }

    if (!b->down[ 0 ]) {
        b->down[ 0 ] = k;
    } else if (!b->down[ 1 ]) {
        b->down[ 1 ] = k;
    } else {
        return;
    }

    if (b->state & 1) {
        return;     // still down
    }
/*
    // save timestamp
    c = Cmd_Argv (2);
    b->downtime = atoi (c);*/

    if (!b->downtime) {
        b->downtime = sys_frame_time - 100;
    }

    b->state |= 1 + 2;  // down + impulse down

}

/**
 * Called whenever a key goes into the up state.
 * @param[in] b Valid pointer to a kbutton_t structure.
 */
PRIVATE void KeyUp (kbutton_t *b)
{
    int     k;
  //  char    *c;
    unsigned    uptime;

  //  c = Cmd_Argv (1);
/*
    if (c[ 0 ]) {
        k = atoi (c);
    } else {
        // typed manually at the console, assume for unsticking, so clear all
        b->down[ 0 ] = b->down[ 1 ] = 0;
        b->state = 4;   // impulse up
        return;
    }*/

    k = 1;

    if (b->down[ 0 ] == k) {
        b->down[ 0 ] = 0;
    } else if (b->down[ 1 ] == k) {
        b->down[ 1 ] = 0;
    } else {
        return;     // key up without coresponding down (menu pass through)
    }

    if (b->down[ 0 ] || b->down[ 1 ]) {
        return;     // some other key is still holding it down
    }

    if (! (b->state & 1)) {
        return;     // still up (this should not happen)
    }
/*
    // save timestamp
    c = Cmd_Argv (2);
    uptime = atoi (c);
*/
    if (uptime) {
        b->msec += uptime - b->downtime;
    } else {
        b->msec += 10;
    }

    b->state &= ~1;     // now up
    b->state |= 4;      // impulse up
}

void IN_LeftDown (void)
{
    KeyDown (&in_left);
}
void IN_LeftUp (void)
{
    KeyUp (&in_left);
}
void IN_RightDown (void)
{
    KeyDown (&in_right);
}
void IN_RightUp (void)
{
    KeyUp (&in_right);
}
void IN_ForwardDown (void)
{
    KeyDown (&in_forward);
}
void IN_ForwardUp (void)
{
    KeyUp (&in_forward);
}
void IN_BackDown (void)
{
    KeyDown (&in_back);
}
void IN_BackUp (void)
{
    KeyUp (&in_back);
}

void IN_MoveleftDown (void)
{
    KeyDown (&in_moveleft);
}
void IN_MoveleftUp (void)
{
    KeyUp (&in_moveleft);
}
void IN_MoverightDown (void)
{
    KeyDown (&in_moveright);
}
void IN_MoverightUp (void)
{
    KeyUp (&in_moveright);
}

void IN_AttackDown (void)
{
    KeyDown (&in_attack);
}
void IN_AttackUp (void)
{
    KeyUp (&in_attack);
}

void IN_UseDown (void)
{
    KeyDown (&in_use);
}
void IN_UseUp (void)
{
    KeyUp (&in_use);
}


/**
 * Key state tracking.
 * @param[in] b Valid pointer to a kbutton_t structure.
 * @return Returns the fraction of the frame that the key was down.
 */
PRIVATE float Client_KeyState (kbutton_t *key)
{
    float val = 0;
    _boolean impulsedown, impulseup, down;

    impulsedown = key->state & 2;
    impulseup = key->state & 4;
    down = key->state & 1;

    if (impulsedown && !impulseup) {
        if (down) {
            val = 0.5; // pressed and held this frame
        } else {
            val = 0;
        }
    }

    if (impulseup && !impulsedown) {
        if (down)
            val = 0;
        else
            val = 0;    // released this frame
    }

    if (!impulsedown && !impulseup) {
        if (down)
            val = 1.0;  // held the entire frame
        else
            val = 0;      // up the entire frame
    }

    if (impulsedown && impulseup) {
        if (down)
            val = 0.75; // released and re-pressed this frame
        else
            val = 0.25; // pressed and released this frame
    }

    key->state &= 1; // clear impulses

    return val;
}

float cl_forwardspeed;
float cl_sidespeed;
float cl_yawspeed;

/**
 * Moves the local angle positions.
 */
PRIVATE void Client_AdjustAngles (void)
{
    float speed = (float)ClientStatic.frametime * 100.0f;

    if (! (in_strafe.state & 1)) {
        ClientState.viewangles[ YAW ] += (int) (speed * cl_yawspeed * (Client_KeyState (&in_left) - Client_KeyState (&in_right)));
    }
}

/**
 * Send the intended movement message to the server.
 * @param[in\out] Pointer to valid usercmd_t structure.
 */
PRIVATE void Client_BaseMove (usercmd_t *cmd)
{
    Client_AdjustAngles();

    memset (cmd, 0, sizeof (usercmd_t));

    vectorCopy (ClientState.viewangles, cmd->angles);

    if (in_strafe.state & 1) {
        cmd->sidemove += (short) (cl_sidespeed * Client_KeyState (&in_right));
        cmd->sidemove -= (short) (cl_sidespeed * Client_KeyState (&in_left));
    }

    cmd->sidemove += (short) (cl_sidespeed * Client_KeyState (&in_moveright));
    cmd->sidemove -= (short) (cl_sidespeed * Client_KeyState (&in_moveleft));

    cmd->forwardmove += (short) (cl_forwardspeed * Client_KeyState (&in_forward));
    cmd->forwardmove -= (short) (cl_forwardspeed * Client_KeyState (&in_back));
}

/**
 * Final movement adjustments applied to move command.
 * @param[in] cmd Valid pointer to usercmd_t structure.
 */
PRIVATE void Client_FinishMove (usercmd_t *cmd)
{
    int     ms;
    int     i;

    if (in_attack.state & 3) {
        cmd->buttons |= BUTTON_ATTACK;
    }

    in_attack.state &= ~2;

    if (in_use.state & 3) {
        cmd->buttons |= BUTTON_USE;
    }

    in_use.state &= ~2;

    if (anykeydown && ClientStatic.key_dest == key_game) {
        cmd->buttons |= BUTTON_ANY;
    }

    // send milliseconds of time to apply the move
    ms = (int)ClientStatic.frametime * 1000;

    if (ms > 250) {
        ms = 100;       // time was unreasonable
    }

    cmd->msec = ms;

    for (i = 0 ; i < 3 ; ++i) {
        cmd->angles[ i ] = ANGLE2SHORT (ClientState.viewangles[ i ]);
    }

    cmd->impulse = in_impulse;
    in_impulse = 0;

}

/**
 * Create the movement command.
 * @return Filled out usercmd_t structure.
 */
PRIVATE usercmd_t Client_CreateCmd (void)
{
    usercmd_t   cmd;

    frame_msec = sys_frame_time - old_sys_frame_time;

    if (frame_msec < 1) {
        frame_msec = 1;
    }

    if (frame_msec > 200) {
        frame_msec = 200;
    }

    // get basic movement from keyboard
    Client_BaseMove (&cmd);

    // allow mice or other external controllers to add to the move
    IN_Move (&cmd);

    Client_FinishMove (&cmd);

    old_sys_frame_time = sys_frame_time;

    return cmd;
}

/**
 * Create a movement command to send to the server
 */
PUBLIC void Client_SendCommand (void) {
    ClientState.cmd = Client_CreateCmd();
}