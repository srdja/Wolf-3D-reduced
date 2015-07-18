

#include <stdlib.h>
#include <string.h>

#include "input.h"
/*
extern  W32 sys_frame_time;
static W32 frame_msec;
static W32 old_sys_frame_time;*/
/*

kbutton_t   in_left, in_right, in_forward, in_back;
kbutton_t   in_moveleft, in_moveright;
kbutton_t   in_strafe, in_use, in_attack;
*/
int     in_impulse;

/**
 * Key state tracking.
 * @param[in] b Valid pointer to a kbutton_t structure.
 * @return Returns the fraction of the frame that the key was down.
 *//*
static float Client_KeyState (kbutton_t *key)
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
}*/

float cl_forwardspeed;
float cl_sidespeed;
float cl_yawspeed;

/**
 * Moves the local angle positions.
 *//*
static void Client_AdjustAngles (void)
{
    float speed = (float)ClientStatic.frametime * 100.0f;

    if (! (in_strafe.state & 1)) {
        ClientState.viewangles[ YAW ] += (int) (speed * cl_yawspeed * (Client_KeyState (&in_left) - Client_KeyState (&in_right)));
    }
}*/

/**
 * Send the intended movement message to the server.
 * @param[in\out] Pointer to valid usercmd_t structure.
 */
/*
static void Client_BaseMove (usercmd_t *cmd)
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
}*/
/*

static void Client_FinishMove (usercmd_t *cmd)
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

}*/
/*
static usercmd_t Client_CreateCmd (void)
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
    //IN_MouseMove(&cmd);

    Client_FinishMove (&cmd);

    old_sys_frame_time = sys_frame_time;

    return cmd;
}
*/
/**
 * Create a movement command to send to the server
 */
void Client_SendCommand (void) {
  //  ClientState.cmd = Client_CreateCmd();
}