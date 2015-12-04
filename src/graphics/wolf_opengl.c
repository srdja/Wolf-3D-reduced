/*

    Copyright (C) 2004 Michael Liebscher
    Copyright (C) 2000-2002 by DarkOne the Hacker
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
 * \file wolf_opengl.c
 * \brief Wolfenstein 3-D OpenGL renderer.
 * \author Michael Liebscher
 * \date 2004-2012
 */

/*
 *  wolf_opengl.c:  Wolfenstein3-D OpenGL renderer.
 *
 *  Author: Michael Liebscher <johnnycanuck@users.sourceforge.net>
 *  Date:   2004
 *
 *  Acknowledgement:
 *  Portion of this code was derived from NewWolf, and was originally
 *  written by DarkOne the Hacker.
 *
 *  Portion of this code was derived from Quake II, and was originally
 *  written by Id Software, Inc.
 *
 */

#include <string.h>

#include "../game/wolf_math.h"
#include "../graphics/video.h"
#include "../graphics/opengl_local.h"
#include "../util/com_string.h"
#include "../game/client.h"

#include "../game/wolf_level.h"
#include "../game/../game/wolf_player.h"
#include "../game/wolf_local.h"
#include "../game/wolf_raycast.h"

extern viddef_t viddef;


float cur_y_fov; // x & y field of view (in degrees)
float ratio; // viewport width/height


/**
 * \brief Set openGL default state
 */
void GL_SetDefaultState (void)
{
    glClearColor(1, 0, 0.5 , 0.5);
    glCullFace(GL_FRONT);
    glEnable(GL_TEXTURE_2D);

    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.666f);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_BLEND);

    glColor4f(1, 1, 1, 1);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glShadeModel(GL_FLAT);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

/**
 * \brief Check field-of-view
 */
static void R_CheckFOV (void)
{
    ratio = (float) viddef.width / (float)viddef.height; // FIXME: move somewhere
    cur_y_fov = g_fov;
}


/**
 * \brief Set openGL 3D view
 * \param[in] viewport player's viewport
 */
void R_SetGL3D (placeonplane_t viewport)
{
    R_CheckFOV();

    glMatrixMode (GL_PROJECTION);
    glLoadIdentity();
    MYgluPerspective (cur_y_fov - 2.0f, ratio, 0.2f, 64.0f);
    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity();

    glRotatef ((GLfloat) (90 - RAD2DEG (viewport.angle)), 0, 1, 0);
    glTranslatef (-viewport.origin[ 0 ] / FLOATTILE, 0, viewport.origin[ 1 ] / FLOATTILE);

    glCullFace (GL_BACK);

    glEnable (GL_DEPTH_TEST);
    glEnable (GL_CULL_FACE);
    glEnable (GL_BLEND);
}

/**
 * \brief Draws a rectangle
 * \param[in] x Left
 * \param[in] y Top
 * \param[in] w Right
 * \param[in] h Bottom
 * \param[in] color Color
 */
void R_DrawBox (int x, int y, int w, int h, uint32_t color)
{
    glDisable (GL_TEXTURE_2D);

    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_COLOR, GL_DST_COLOR);

    glColor4ubv ((GLubyte *) & color);

    glBegin (GL_QUADS);
        glVertex2i (x, y);
        glVertex2i (x, y + h);
        glVertex2i (x + w, y + h);
        glVertex2i (x + w, y);
    glEnd();

    glColor3f (1, 1, 1);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable (GL_BLEND);
    glEnable (GL_TEXTURE_2D);
}

/**
 * \brief Load wall texture
 * \param[in] wallPicNum Wall texture to load
 * \param[out] pIsDark Is this texture going to be dark?
 * \note
 *  Returns with the texture bound and glColor set to the right intensity.
 *  Loads an image from the filesystem if necessary.
 *  Used both during gameplay and for preloading during level parse.
 *
 *  Wolfenstein was very wasteful with texture usage, making almost half of
 *  the textures just dim versions to provide "lighting" on the different
 *  wall sides.  With only a few exceptions for things like the elevator tiles
 *  and outdoor tiles that could only be used in particular orientations
 */
void LoadWallTexture (int wallPicNum , bool *pIsDark)
{
    texture_t *twall;
    bool isDark = false;

    if ((wallPicNum & 1) &&
            wallPicNum != 31 &&
            wallPicNum != 41 &&
            wallPicNum != 43 &&
            wallPicNum != 133) {
        isDark = true;
        wallPicNum--;
    }

    twall = TM_FindTexture_Wall (wallPicNum);
    R_Bind (twall->texnum);

    if (pIsDark) {
        *pIsDark = isDark;
    }
}

/**
 * \brief Draws a wall
 * \param[in] x
 * \param[in] y
 * \param[in] z1
 * \param[in] z2
 * \param[in] type facing wall
 * \param[in] tex Wall texture to use
 * \note
 *              north (y)
 *              __________
 *              |        |
 *    west (x)  |        | east (x)
 *              |________|
 *              south (y)
 */
void R_Draw_Wall (float x, float y, float z1, float z2, int type, int tex)
{
    float x1, x2, y1, y2;
    bool isDark;

    switch (type) {
    // X wall
    case dir4_east:
        x1 = x2 = x + 1;
        y1 = -1 - y;
        y2 = -y;
        break;

    case dir4_west:
        x1 = x2 = x;
        y1 = -y;
        y2 = -1 - y;
        break;

    // Y wall
    case dir4_north:
        y1 = y2 = -y - 1;
        x1 = x;
        x2 = x + 1;
        break;

    case dir4_south:
        y1 = y2 = -y;
        x1 = x + 1;
        x2 = x;
        break;
    }

    LoadWallTexture (tex, &isDark);

    if (isDark) {
        glColor3f (0.7f, 0.7f, 0.7f);
    }

    glBegin (GL_QUADS);
        glTexCoord2f (1.0, 0.0);
        glVertex3f (x1, z2, y1);
        glTexCoord2f (0.0, 0.0);
        glVertex3f (x2, z2, y2);
        glTexCoord2f (0.0, 1.0);
        glVertex3f (x2, z1, y2);
        glTexCoord2f (1.0, 1.0);
        glVertex3f (x1, z1, y1);
    glEnd();

    if (isDark) {
        glColor3f (1.0f, 1.0f, 1.0f);
    }

}

/**
 * \brief Draws a door
 * \param[in] x
 * \param[in] y
 * \param[in] z1
 * \param[in] z2
 * \param[in] vertical Set to true if the door vertical, otherwise set to false if door is horizontal.
 * \param[in] backside Set to true if door is back facing, otherwise set to false for front facing door.
 * \param[in] amount Set to true if door is back facing, otherwise set to false for front facing door.
 */
void R_Draw_Door (int x, int y, float z1, float z2, bool vertical, bool backside, int tex, int amount)
{
    float x1, x2, y1, y2, amt;
    bool isDark;

    if (amount == DOOR_FULLOPEN) {
        return;
    }
    amt = (float)amount / DOOR_FULLOPEN;

    if (vertical) {
        x1 = x2 = (float)x + 0.5f;
        y1 = - ((float)y - amt);
        y2 = - ((float)y - amt); // -1

        if (backside) {
            y1 -= 1;
        } else {
            y2 -= 1;
        }
    } else {
        y1 = y2 = - (float)y - 0.5f;
        x1 = (float)x + amt; // +1
        x2 = (float)x + amt;

        if (backside) {
            x2 += 1;
        } else {
            x1 += 1;
        }
    }

    LoadWallTexture (tex, &isDark);

    if (isDark) {
        glColor3f (0.7f, 0.7f, 0.7f);
    }

    glBegin (GL_QUADS);
        glTexCoord2f (backside ? 0.0f : 1.0f, 0.0);
        glVertex3f (x1, z2, y1);
        glTexCoord2f (backside ? 1.0f : 0.0f, 0.0);
        glVertex3f (x2, z2, y2);
        glTexCoord2f (backside ? 1.0f : 0.0f, 1.0);
        glVertex3f (x2, z1, y2);
        glTexCoord2f (backside ? 0.0f : 1.0f, 1.0);
        glVertex3f (x1, z1, y1);
    glEnd();

    if (isDark) {
        glColor3f (1.0f, 1.0f, 1.0f);
    }
}

/**
 * \brief Draws all visible sprites.
 */
void R_DrawSprites (void)
{
    float sina, cosa;
    float Ex, Ey, Dx, Dy;
    int n_sprt, n;
    float ang;
    texture_t *twall;

// build visible sprites list

    n_sprt = Sprite_CreateVisList();

    if (! n_sprt) {
        return; // nothing to draw
    }

    // prepare values for billboarding
    ang = angle_normalize (Player.position.angle + M_PI / 2);   // FIXME: take viewport

    sina = (float) (0.5 * sin (ang));
    cosa = (float) (0.5 * cos (ang));

    for (n = 0; n < n_sprt; ++n) {
        if (vislist[ n ].dist < MINDIST / 2) {
            continue; // little hack to save speed & z-buffer
        }

        twall = TM_FindTexture_Sprite (vislist[ n ].tex);

        R_Bind (twall->texnum);

        glBegin (GL_QUADS);
            Ex = Dx = vislist[ n ].x / FLOATTILE;
            Ey = Dy = vislist[ n ].y / FLOATTILE;
            Ex += cosa;
            Ey += sina;
            Dx -= cosa;
            Dy -= sina;

            glTexCoord2f (0.0, 0.0);
            glVertex3f (Ex, UPPERZCOORD, -Ey);
            glTexCoord2f (0.0, 1.0);
            glVertex3f (Ex, LOWERZCOORD, -Ey);
            glTexCoord2f (1.0, 1.0);
            glVertex3f (Dx, LOWERZCOORD, -Dy);
            glTexCoord2f (1.0, 0.0);
            glVertex3f (Dx, UPPERZCOORD, -Dy);
        glEnd();
    }
}

/**
 * \brief Draws weapon
 */
void R_DrawWeapon (void)
{
    texture_t *tex;
    static int w = 128;
    static int h = 154;  // image was drawn for 320x200 res, so stretch to fit 320x240
    float scale = (float) (viddef.height / 240);
    int x = (int) (viddef.width - (w * scale)) >> 1;
    int y;

    if (Player.playstate == ex_watchingbj) {
        return;
    } else if (Player.playstate == ex_watchingdeathcam) {
        if (ClientStatic.realtime & 512) {
            return;
        }
        y = 7 * viddef.height / 200;
        tex = TM_FindTexture_Sprite (SPR_DEATHCAM);
    } else {
        y = (int) (viddef.height - (h * scale) - 79);
        //tex = TM_FindTexture_Sprite (Player.weapon * 5 + Player.weaponframe + SPR_KNIFEREADY);
        tex = TM_FindTexture_Sprite (422);
    }
    R_Bind (tex->texnum);

    glAlphaFunc (GL_GREATER, 0.3f);

    glEnable (GL_BLEND);

    glBegin (GL_QUADS);
        glTexCoord2f (0.0f, 0.0f);
        glVertex2i (x, y);
        glTexCoord2f (1.0f, 0.0f);
        glVertex2i (x + w * scale, y);
        glTexCoord2f (1.0f, 1.0f);
        glVertex2i (x + w * scale, y + h * scale);
        glTexCoord2f (0.0f, 1.0f);
        glVertex2i (x, y + h * scale);
    glEnd();

    glDisable (GL_BLEND);

    glAlphaFunc (GL_GREATER, 0.666f);
}

/**
 * \brief Draws number
 * \param[in] x X-Coordinent
 * \param[in] y Y-Coordinent
 * \param[in] number Number value to draw
 */
void R_DrawNumber (int x, int y, int number)
{
    texture_t *tex;
    int col;
    float fcol;
    static float w = 0.1f;
    int i;
    char string[ 20 ];
    uint32_t length;

    com_snprintf (string, sizeof (string), "%d", number);
    length = strlen (string);

    tex = TM_FindTexture ("pics/N_NUMPIC.tga", TT_Pic);

    glEnable (GL_TEXTURE_2D);

    R_Bind (tex->texnum);

    glBegin (GL_QUADS);
        for (i = length - 1 ; i >= 0 ; --i) {
            col = string[ i ] - 48;

            fcol = col * w;

            glTexCoord2f (fcol,    0);
            glVertex2i (x, y);
            glTexCoord2f (fcol + w, 0);
            glVertex2i (x + 18, y);
            glTexCoord2f (fcol + w, 1);
            glVertex2i (x + 18, y + 32);
            glTexCoord2f (fcol,    1);
            glVertex2i (x, y + 32);

            x -= 18;
        }
    glEnd();
}

uint8_t wfont[ ] = {
    32, 15, 32, 32, 32, 32, 32, 12, 32, 32, 32, 32, 32, 32, 32, 32,
    32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 16, 32, 32, 32, 32, 32,
    32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
    32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32
};

/**
 * \brief Draws a line of text
 * \param[in] x X-Coordinent
 * \param[in] y Y-Coordinent
 * \param[in] string Text
 */
void R_put_line (int x, int y, const char *string)
{
    texture_t *tex;
    int mx = x;
    int num;
    float frow, fcol;
    static float    h = 0.25f;  // (32 / 128.0f);
    static float    w = 0.0625f; // (32 / 512.0f);

    tex = TM_FindTexture ("pics/L_FONTPIC.tga", TT_Pic);

    R_Bind (tex->texnum);

    glBegin (GL_QUADS);
        while (*string) {
            if (*string == '\n') {
                mx = x;
                y += 32;
                ++string;
                continue;
            }
            num = *string;
            num &= 255;

            if ((num & 127) == 32) {
                mx += 32;
                ++string;
                continue;       // space
            }

            frow = ((num >> 4) - 2) * h;
            fcol = (num & 15) * w;

            glTexCoord2f (fcol, frow);
            glVertex2i (mx, y);
            glTexCoord2f (fcol + w, frow);
            glVertex2i (mx + 32, y);
            glTexCoord2f (fcol + w, frow + h);
            glVertex2i (mx + 32, y + 32);
            glTexCoord2f (fcol, frow + h);
            glVertex2i (mx, y + 32);

            mx += wfont[ (num & 127) - 32 ];
            ++string;
        }
    glEnd();
}