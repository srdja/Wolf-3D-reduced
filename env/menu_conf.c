/*

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

#include <string.h>
#include <ctype.h>

#include "client.h"
#include "menu_conf.h"
#include "keycodes.h"
#include "com_string.h"
#include "timer.h"
#include "font_manager.h"
#include "renderer.h"
#include "video.h"
#include "memory.h"

PRIVATE void Menu_DrawStatusBar (const char *string);
PRIVATE void Menulist_DoEnter (menulist_s *l);
PRIVATE void MenuList_Draw (menulist_s *l);
PRIVATE void Slider_DoSlide (menuslider_s *s, int dir);
PRIVATE void Slider_Draw (menuslider_s *s);
PRIVATE void SpinControl_DoEnter (menulist_s *s);
PRIVATE void SpinControl_Draw (menulist_s *s);
PRIVATE void SpinControl_DoSlide (menulist_s *s, int dir);

#define RCOLUMN_OFFSET  16
#define LCOLUMN_OFFSET -16


extern viddef_t viddef;

#define VID_WIDTH viddef.width
#define VID_HEIGHT viddef.height

colour3_t colourLGray = { 192, 192, 192 };
colour3_t colourDGray = { 140, 140, 140 };


PRIVATE void Separator_Draw (menuseparator_s *s)
{
    if (s->generic.name) {
        Menu_DrawStringR2LDark (s->generic.fs, s->generic.x + s->generic.parent->x, s->generic.y + s->generic.parent->y, s->generic.name);
    }
}

void DrawWindow (int x, int y, int w, int h,
                 colour3_t bg, colour3_t act, colour3_t deact)
{
    R_Draw_Fill (x, y, w, h, bg);


    R_Draw_Line (x,      y,      x + w, y,     1, deact); // Top
    R_Draw_Line (x,      y,      x,   y + h,   1, deact); // Left
    R_Draw_Line (x + w,    y,      x + w, y + h + 1, 1, act); // Right
    R_Draw_Line (x,      y + h,    x + w, y + h,   1, act); // Bottom
}


/////////////////////////////////////////////////////////////////////
//
//  Action
//
/////////////////////////////////////////////////////////////////////

PRIVATE void Action_DoEnter (menuaction_s *a)
{
    if (a->generic.callback) {
        a->generic.callback (a);
    }
}

PRIVATE void Action_Draw (menuaction_s *a, int i)
{

    if (a->generic.flags & MENUFONT_LEFT_JUSTIFY) {
        if ((i == a->generic.parent->cursor) && a->generic.fontHighColour)
            Menu_DrawString (a->generic.fs, a->generic.x + a->generic.parent->x, a->generic.y + a->generic.parent->y, a->generic.name, *a->generic.fontHighColour);
        else
            Menu_DrawString (a->generic.fs, a->generic.x + a->generic.parent->x, a->generic.y + a->generic.parent->y, a->generic.name, *a->generic.fontBaseColour);
    } else {
        if ((i == a->generic.parent->cursor) && a->generic.fontHighColour)
            Menu_DrawStringR2L (a->generic.fs, a->generic.x + a->generic.parent->x + LCOLUMN_OFFSET, a->generic.y + a->generic.parent->y, a->generic.name, *a->generic.fontHighColour);
        else
            Menu_DrawStringR2L (a->generic.fs, a->generic.x + a->generic.parent->x + LCOLUMN_OFFSET, a->generic.y + a->generic.parent->y, a->generic.name, *a->generic.fontBaseColour);


    }

    if (a->generic.ownerdraw) {
        a->generic.ownerdraw (a);
    }
}



/////////////////////////////////////////////////////////////////////
//
//  Field
//
/////////////////////////////////////////////////////////////////////

_boolean Field_DoEnter (menufield_s *f)
{
    if (f->generic.callback) {
        f->generic.callback (f);
        return true;
    }

    return false;
}

void Field_Draw (menufield_s *f)
{

}

_boolean Field_Key (menufield_s *f, int key)
{
    extern _boolean keydown[];

    switch (key) {
    case K_KP_SLASH:
        key = '/';
        break;

    case K_KP_MINUS:
        key = '-';
        break;

    case K_KP_PLUS:
        key = '+';
        break;

    case K_KP_HOME:
        key = '7';
        break;

    case K_KP_UPARROW:
        key = '8';
        break;

    case K_KP_PGUP:
        key = '9';
        break;

    case K_KP_LEFTARROW:
        key = '4';
        break;

    case K_KP_5:
        key = '5';
        break;

    case K_KP_RIGHTARROW:
        key = '6';
        break;

    case K_KP_END:
        key = '1';
        break;

    case K_KP_DOWNARROW:
        key = '2';
        break;

    case K_KP_PGDN:
        key = '3';
        break;

    case K_KP_INS:
        key = '0';
        break;

    case K_KP_DEL:
        key = '.';
        break;

    } // End switch key

    if (key > 127) {
        switch (key) {
        case K_DEL:
        default:
            return false;
        }
    }

    /*
    ** support pasting from the clipboard
    */
    if ((TOUPPER (key) == 'V' && keydown[K_CTRL]) ||
            (((key == K_INS) || (key == K_KP_INS)) && keydown[K_SHIFT])) {
        char *cbd;

        if ((cbd = Sys_GetClipboardData()) != 0) {
            strtok (cbd, "\n\r\b");

            strncpy (f->buffer, cbd, f->length - 1);
            f->cursor = strlen (f->buffer);
            f->visible_offset = f->cursor - f->visible_length;

            if (f->visible_offset < 0) {
                f->visible_offset = 0;
            }

            MM_FREE (cbd);
        }

        return true;
    }

    switch (key) {
    case K_KP_LEFTARROW:
    case K_LEFTARROW:
    case K_BACKSPACE:
        if (f->cursor > 0) {
            memmove (&f->buffer[ f->cursor - 1 ], &f->buffer[ f->cursor ], strlen (&f->buffer[ f->cursor ]) + 1);
            f->cursor--;

            if (f->visible_offset) {
                f->visible_offset--;
            }
        }

        break;

    case K_KP_DEL:
    case K_DEL:
        memmove (&f->buffer[ f->cursor ], &f->buffer[ f->cursor + 1 ], strlen (&f->buffer[ f->cursor + 1 ]) + 1);
        break;

    case K_KP_ENTER:
    case K_ENTER:
    case K_ESCAPE:
    case K_TAB:
        return false;

    case K_SPACE:
    default:
        if (! isdigit (key) && (f->generic.flags & MENUFONT_NUMBERSONLY)) {
            return false;
        }

        if (f->cursor < f->length) {
            f->buffer[ f->cursor++ ] = key;
            f->buffer[ f->cursor ] = 0;

            if (f->cursor > f->visible_length) {
                f->visible_offset++;
            }
        }

    } // End switch key

    return true;
}


/////////////////////////////////////////////////////////////////////
//
//  Menu
//
/////////////////////////////////////////////////////////////////////

void Menu_AddItem (menuframework_s *menu, void *item)
{
    if (menu->nitems == 0) {
        menu->nslots = 0;
    }

    if (menu->nitems < MAXMENUITEMS) {
        menu->items[menu->nitems] = item;
        ((menucommon_s *) menu->items[menu->nitems])->parent = menu;
        menu->nitems++;
    }

    menu->nslots = Menu_TallySlots (menu);
}

/*
** Menu_AdjustCursor
**
** This function takes the given menu, the direction, and attempts
** to adjust the menu's cursor so that it's at the next available
** slot.
*/
void Menu_AdjustCursor (menuframework_s *m, int dir)
{
    menucommon_s *citem;

    /*
    ** see if it's in a valid spot
    */
    if (m->cursor >= 0 && m->cursor < m->nitems) {
        if ((citem = Menu_ItemAtCursor (m)) != 0) {
            if (citem->type != MTYPE_SEPARATOR) {
                return;
            }
        }
    }

    /*
    ** it's not in a valid spot, so crawl in the direction indicated until we
    ** find a valid spot
    */
    if (dir == 1) {
        while (1) {
            citem = Menu_ItemAtCursor (m);

            if (citem)
                if (citem->type != MTYPE_SEPARATOR)
                    break;

            m->cursor += dir;

            if (m->cursor >= m->nitems)
                m->cursor = 0;
        }
    } else {
        while (1) {
            citem = Menu_ItemAtCursor (m);

            if (citem)
                if (citem->type != MTYPE_SEPARATOR)
                    break;

            m->cursor += dir;

            if (m->cursor < 0)
                m->cursor = m->nitems - 1;
        }
    }
}

void Menu_Center (menuframework_s *menu)
{
    int height;

    height = ((menucommon_s *) menu->items[menu->nitems - 1])->y;
    height += 10;

    menu->y = (VID_HEIGHT - height) >> 1;
}

void Menu_Draw (menuframework_s *menu)
{
    int i;
    menucommon_s *item;
    static const char ccursor[ 2 ] = { ' ', '>' };

    /*
    ** draw contents
    */
    for (i = 0; i < menu->nitems; ++i) {
        switch (((menucommon_s *) menu->items[i])->type) {
        case MTYPE_FIELD:
            Field_Draw ((menufield_s *) menu->items[i]);
            break;

        case MTYPE_SLIDER:
            Slider_Draw ((menuslider_s *) menu->items[i]);
            break;

        case MTYPE_LIST:
            MenuList_Draw ((menulist_s *) menu->items[i]);
            break;

        case MTYPE_SPINCONTROL:
            SpinControl_Draw ((menulist_s *) menu->items[i]);
            break;

        case MTYPE_ACTION:
            Action_Draw ((menuaction_s *) menu->items[i], i);
            break;

        case MTYPE_SEPARATOR:
            Separator_Draw ((menuseparator_s *) menu->items[i]);
            break;

        }
    }

    item = Menu_ItemAtCursor (menu);

    if (item && item->cursordraw) {
        item->cursordraw (item);
    } else if (menu->cursordraw) {
        menu->cursordraw (menu);
    } else if (item && item->type != MTYPE_FIELD) {

        Font_SetColour (item->fs, *item->fontBaseColour);

        if (item->flags & MENUFONT_LEFT_JUSTIFY) {
            //Draw_Char( menu->x + item->x - 24 + item->cursor_offset, menu->y + item->y, 12 + ( ( int ) ( Sys_Milliseconds()/250 ) & 1 ) );
            Font_put_character (item->fs, menu->x + item->x - 24 + item->cursor_offset, menu->y + item->y, ccursor[ 0 + ((int) (Sys_Milliseconds() / 250) & 1) ]);
        } else {
            //Draw_Char( menu->x + item->cursor_offset, menu->y + item->y, 12 + ( ( int ) ( Sys_Milliseconds()/250 ) & 1 ) );
            Font_put_character (item->fs, menu->x + item->cursor_offset, menu->y + item->y, ccursor[ 0 + ((int) (Sys_Milliseconds() / 250) & 1) ]);
        }
    }

}

void Menu_DrawStatusBar (const char *string)
{
    if (string) {
        int l = strlen (string);
        //int maxrow = VID_HEIGHT / 8;
        int maxcol = VID_WIDTH / 8;
        int col = maxcol / 2 - l / 2;

        R_Draw_Fill (0, VID_HEIGHT - 8, VID_WIDTH, 8, colourBlack);
        Menu_DrawString (FONT1, col * 8, VID_HEIGHT - 8, string, colourBlack);
    } else {
        R_Draw_Fill (0, VID_HEIGHT - 8, VID_WIDTH, 8, colourBlack);
    }
}

void Menu_DrawString (FONTSELECT fs, int x, int y, const char *string, colour3_t c)
{
    Font_SetColour (fs, c);
    Font_put_line (fs, x, y, string);
}

void Menu_DrawStringR2L (FONTSELECT fs, int x, int y, const char *string, colour3_t c)
{
    Font_SetColour (fs, c);
    Font_put_lineR2L (fs, x, y, string);
}

void Menu_DrawStringR2LDark (FONTSELECT fs, int x, int y, const char *string)
{

}

void *Menu_ItemAtCursor (menuframework_s *m)
{
    if (m->cursor < 0 || m->cursor >= m->nitems)
        return 0;

    return m->items[ m->cursor ];
}

_boolean Menu_SelectItem (menuframework_s *s)
{
    menucommon_s *item = (menucommon_s *) Menu_ItemAtCursor (s);

    if (item) {
        switch (item->type) {
        case MTYPE_FIELD:
            return Field_DoEnter ((menufield_s *) item) ;

        case MTYPE_ACTION:
            Action_DoEnter ((menuaction_s *) item);
            return true;

        case MTYPE_LIST:
//          Menulist_DoEnter( ( menulist_s * ) item );
            return false;

        case MTYPE_SPINCONTROL:
//          SpinControl_DoEnter( ( menulist_s * ) item );
            return false;

        }
    }

    return false;
}

void Menu_SetStatusBar (menuframework_s *m, const char *string)
{
    m->statusbar = string;
}

void Menu_SlideItem (menuframework_s *s, int dir)
{
    menucommon_s *item = (menucommon_s *) Menu_ItemAtCursor (s);

    if (item) {
        switch (item->type) {
        case MTYPE_SLIDER:
            Slider_DoSlide ((menuslider_s *) item, dir);
            break;

        case MTYPE_SPINCONTROL:
            SpinControl_DoSlide ((menulist_s *) item, dir);
            break;
        }
    }
}

int Menu_TallySlots (menuframework_s *menu)
{
    int i;
    int total = 0;

    for (i = 0; i < menu->nitems; i++) {
        if (((menucommon_s *) menu->items[i])->type == MTYPE_LIST) {
            int nitems = 0;
            const char **n = ((menulist_s *) menu->items[i])->itemnames;

            while (*n)
                nitems++, n++;

            total += nitems;
        } else {
            total++;
        }
    }

    return total;
}


/////////////////////////////////////////////////////////////////////
//
//  Menu List
//
/////////////////////////////////////////////////////////////////////

void Menulist_DoEnter (menulist_s *l)
{
    int start;

    start = l->generic.y / 10 + 1;

    l->curvalue = l->generic.parent->cursor - start;

    if (l->generic.callback)
        l->generic.callback (l);
}

void MenuList_Draw (menulist_s *l)
{
    const char **n;
    int y = 0;

    Menu_DrawStringR2LDark (l->generic.fs, l->generic.x + l->generic.parent->x + LCOLUMN_OFFSET, l->generic.y + l->generic.parent->y, l->generic.name);

    n = l->itemnames;

    R_Draw_Fill (l->generic.x - 112 + l->generic.parent->x, l->generic.parent->y + l->generic.y + l->curvalue * 10 + 10, 128, 10, colourBlack);

    while (*n) {
        Menu_DrawStringR2LDark (l->generic.fs, l->generic.x + l->generic.parent->x + LCOLUMN_OFFSET, l->generic.y + l->generic.parent->y + y + 10, *n);

        n++;
        y += 10;
    }
}


/////////////////////////////////////////////////////////////////////
//
//  Slider
//
/////////////////////////////////////////////////////////////////////

void Slider_DoSlide (menuslider_s *s, int dir)
{
    s->curvalue += dir;

    if (s->curvalue > s->maxvalue)
        s->curvalue = s->maxvalue;
    else if (s->curvalue < s->minvalue)
        s->curvalue = s->minvalue;

    if (s->generic.callback)
        s->generic.callback (s);
}

#define SLIDER_RANGE 10

void Slider_Draw (menuslider_s *s)
{
    int CharHeight = Font_GetSize (s->generic.fs);


    // Print out Label name
    Font_SetColour (s->generic.fs, *s->generic.fontBaseColour);
    Font_put_lineR2L (s->generic.fs, s->generic.x + s->generic.parent->x + LCOLUMN_OFFSET,
                      s->generic.y + s->generic.parent->y, s->generic.name);

    s->range = (s->curvalue - s->minvalue) / (float) (s->maxvalue - s->minvalue);

    if (s->range < 0) {
        s->range = 0;
    }

    if (s->range > 1) {
        s->range = 1;
    }

    // Draw Slide bar
    DrawWindow (s->generic.x + s->generic.parent->x + RCOLUMN_OFFSET, s->generic.y + s->generic.parent->y, 80, CharHeight,
                colourLGray, colourBlack, colourDGray);

    // Draw slider
    DrawWindow ((int) (RCOLUMN_OFFSET + s->generic.parent->x + s->generic.x + (SLIDER_RANGE - 1) * 8 * s->range), s->generic.y + s->generic.parent->y, 8, CharHeight,
                colourBlack, colourDGray, colourDGray);


}


/////////////////////////////////////////////////////////////////////
//
//  Spin
//
/////////////////////////////////////////////////////////////////////

void SpinControl_DoEnter (menulist_s *s)
{
    s->curvalue++;

    if (s->itemnames[ s->curvalue ] == 0) {
        s->curvalue = 0;
    }

    if (s->generic.callback) {
        s->generic.callback (s);
    }
}

void SpinControl_DoSlide (menulist_s *s, int dir)
{
    s->curvalue += dir;

    if (s->curvalue < 0) {
        s->curvalue = 0;
    } else if (s->itemnames[ s->curvalue ] == 0) {
        s->curvalue--;
    }

    if (s->generic.callback) {
        s->generic.callback (s);
    }
}


void SpinControl_Draw (menulist_s *s)
{
    if (s->generic.name) {
        Font_SetColour (s->generic.fs, *s->generic.fontBaseColour);
        Font_put_lineR2L (s->generic.fs, s->generic.x + s->generic.parent->x + LCOLUMN_OFFSET,
                          s->generic.y + s->generic.parent->y, s->generic.name);
    }


    Font_SetColour (s->generic.fs, *s->generic.fontHighColour);
    Font_put_line (s->generic.fs, RCOLUMN_OFFSET + s->generic.x + s->generic.parent->x, s->generic.y + s->generic.parent->y, s->itemnames[ s->curvalue ]);

}
