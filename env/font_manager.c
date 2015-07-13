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
 * \file font_manager.c
 * \brief Font management.
 * \author Michael Liebscher
 * \date 2004-2012
 */



#include <string.h>

#include "font_manager.h"

#include "common.h"
#include "com_string.h"
#include "renderer.h"

/*

  ! " # $ % & ' ( ) * + , - . /
0 1 2 3 4 5 6 7 8 9 : ; ( = ) ?
@ A B C D E F G H I J K L M N O
P Q R S T U V W X Y Z [ / ] ^ -
` a b c d e f g h i j k l m n o
p q r s t u v w x y z { | } ~

*/


#define MAX_FONTS  4
PRIVATE font_t *myfonts[ MAX_FONTS ];
PRIVATE W32 num_fonts = 0;


typedef struct {
    char *start, *end;

} string_seg_t;


/**
 * \brief Load font details from file
 * \param[in] filename File name to load details
 * \return Valid pointer to font_t
 */
PUBLIC font_t *createFont (const char *filename)
{
    font_t *temp_font;
    char *datname;
    filehandle_t *fp;
    W32 size;
    W32 i;

    if (num_fonts == (MAX_FONTS - 1)) {
        return NULL;
    }

    temp_font = (font_t *)Z_Malloc (sizeof (font_t));
    temp_font->texfont = TM_FindTexture (filename, TT_Pic);

    if (NULL == temp_font->texfont) {
        Z_Free (temp_font);

        return NULL;
    }

    memset (temp_font->nCharWidth, 0, sizeof (temp_font->nCharWidth));

    datname = (char *)MM_MALLOC (strlen (filename) + 1);

    FS_RemoveExtension (filename, datname);

    com_strlcat (datname, ".dat", strlen (filename) + 1);

    fp = FS_OpenFile (datname, 0);

    if (NULL == fp) {
        MM_FREE (datname);
        Z_Free (temp_font);
        return NULL;
    }

    size = FS_GetFileSize (fp);

    // check header size
    if (size < 10) {
        MM_FREE (datname);
        Z_Free (temp_font);

        FS_CloseFile (fp);

        return NULL;
    }
    // Check sig of font dat file

    FS_ReadFile (&size, 1, 4, fp);


    FS_ReadFile (&temp_font->nMaxWidth, 1, 1, fp);
    FS_ReadFile (&temp_font->nMaxHeight, 1, 1, fp);


    FS_ReadFile (&size, 1, 4, fp);
    size = LittleLong (size);

    if (size > 127) {
        MM_FREE (datname);
        Z_Free (temp_font);

        FS_CloseFile (fp);

        return NULL;
    }

    FS_ReadFile (&temp_font->nCharWidth, 1, size, fp);
    FS_CloseFile (fp);

    temp_font->nSize = 2;
    temp_font->colour[ 3 ] = 255;

    temp_font->hFrac = (float) (temp_font->nMaxHeight / (float)temp_font->texfont->height);
    temp_font->wFrac = (float) (temp_font->nMaxWidth / (float)temp_font->texfont->width);

    for (i = 0 ; i < MAX_FONTS ; ++i) {
        if (! myfonts[ i ]) {
            break;
        }
    }

    if (i == (MAX_FONTS - 1)) {
        MM_FREE (datname);
        Z_Free (temp_font);
        return NULL;
    }

    myfonts[ i ] = temp_font;

    MM_FREE (datname);

    return temp_font;
}

/**
 * \brief Initialize font handler
 */
PUBLIC void Font_Init (void)
{
    W32 i;

    for (i = 0 ; i < MAX_FONTS ; ++i) {
        myfonts[ i ] = NULL;
    }

    (void)createFont ("pics/font1.tga");
    (void)createFont ("pics/font2.tga");
}

/**
 * \brief Get dimensions of text based on font
 * \param[in] fs Font to use
 * \param[in] string Text to get dimensions from
 * \param[out] w Width of text in pixels
 * \param[out] h Height of text in pixels
 */
PUBLIC void Font_GetMsgDimensions (FONTSELECT fs, const char *string, int *w, int *h)
{
    int width = 0;
    int mx = 0;
    W16 scale;
    int height;

    if (! myfonts[ fs ]) {
        *w = *h = 0;

        return;
    }

    scale = myfonts[ fs ]->nMaxHeight * myfonts[ fs ]->nSize;
    height = scale;

    while (*string) {
        if (*string == '\n') {
            if (mx > width) {
                width = mx;
            }

            mx = 0;
            height += scale;
            ++string;

            continue;
        }

        mx += myfonts[ fs ]->nCharWidth[ (*string) - 32 ] * myfonts[ fs ]->nSize;

        ++string;
    }

    if (mx > width) {
        width = mx;
    }

    *w = width;
    *h = height;
}

/**
 * \brief Set font size
 * \param[in] fs Font to use
 * \param[in] size Size of font
 */
PUBLIC void Font_SetSize (FONTSELECT fs, W16 size)
{
    if (myfonts[ fs ]) {
        myfonts[ fs ]->nSize = size;
    }
}

/**
 * \brief Get font size
 * \param[in] fs Font to use
 * \return Size of font, otherwise 0
 */
PUBLIC W16 Font_GetSize (FONTSELECT fs)
{
    if (myfonts[ fs ]) {
        return (myfonts[ fs ]->nMaxHeight * myfonts[ fs ]->nSize);
    }

    return 0;
}

/**
 * \brief Set font colour
 * \param[in] fs Font to use
 * \param[in] c Colour
 */
PUBLIC void Font_SetColour (FONTSELECT fs, colour3_t c)
{
    if (myfonts[ fs ]) {
        myfonts[ fs ]->colour[ 0 ] = c[ 0 ];
        myfonts[ fs ]->colour[ 1 ] = c[ 1 ];
        myfonts[ fs ]->colour[ 2 ] = c[ 2 ];
    }
}

/**
 * \brief Put a line of text on screen
 * \param[in] fs Font to use
 * \param[in] x X-Coordinate
 * \param[in] y Y-Coordinate
 * \param[in] string Text to put on screen
 */
PUBLIC void Font_put_line (FONTSELECT fs, int x, int y, const char *string)
{
    int mx = x;
    W16 scale;

    if (! myfonts[ fs ]) {
        return;
    }

    scale = myfonts[ fs ]->nSize;

    while (*string) {
        if (*string == '\n') {
            mx = x;
            y += myfonts[ fs ]->nMaxHeight * scale;
            ++string;
            continue;
        }

        R_Draw_Character (mx, y, *string, myfonts[ fs ]);
        mx += myfonts[ fs ]->nCharWidth[ (*string) - 32 ] * scale;
        ++string;
    }
}

/**
 * \brief Put a line of text on screen right-to-left
 * \param[in] fs Font to use
 * \param[in] x X-Coordinate
 * \param[in] y Y-Coordinate
 * \param[in] string Text to put on screen
 */
void Font_put_lineR2L (FONTSELECT fs, int x, int y, const char *string)
{
    int mx = x;
    unsigned int charindex;
    unsigned int i;

    if (! myfonts[ fs ]) {
        return;
    }

    for (i = 0; i < strlen (string); ++i) {
        charindex = strlen (string) - i - 1;
        mx -= myfonts[ fs ]->nCharWidth[ string[ charindex ] - 32 ] * myfonts[ fs ]->nSize;

        R_Draw_Character (mx, y, string[ charindex ], myfonts[ fs ]);
    }
}

/**
 * \brief Put character on screen right-to-left
 * \param[in] fs Font to use
 * \param[in] x X-Coordinate
 * \param[in] y Y-Coordinate
 * \param[in] num Character to put on screen
 */
PUBLIC W16 Font_put_character (FONTSELECT fs, int x, int y, W16 num)
{
    if (! myfonts[ fs ] || num > 126) {
        return 0;
    }

    R_Draw_Character (x, y, num, myfonts[ fs ]);

    return (myfonts[ fs ]->nCharWidth[ num - 32 ] * myfonts[ fs ]->nSize);
}

/**
 * \brief Put line of text on screen
 * \param[in] fs Font to use
 * \param[in] x X-Coordinate
 * \param[in] y Y-Coordinate
 * \param[in] start Start of string
 * \param[in] end End of string
 */
PUBLIC void Font_put_line_size (FONTSELECT fs, int x, int y, const char *start, const char *end)
{
    int mx = x;
    W16 scale;

    if (! myfonts[ fs ]) {
        return;
    }

    scale = myfonts[ fs ]->nSize;

    while (start != end) {
        R_Draw_Character (mx, y, *start, myfonts[ fs ]);
        mx += myfonts[ fs ]->nCharWidth[ (*start) - 32 ] * scale;
        ++start;
    }

}


/**
 * \brief Get line of text that will fit in width
 * \param[in] fs Font to use
 * \param[in] line_width Length of line
 * \param[in\out] sst String segment that fits
 * \return true if line fits, otherwise false.
 */
PRIVATE W8 Font_get_line (FONTSELECT fs, int line_width, string_seg_t *sst)
{
    int x = 0;
    int in_a_word = 0;
    int t_words = 0;
    int t_spaces = 0;
    int chars_width = 0;
    W16 scale;
    const char *word_start = sst->start;


    if (! myfonts[ fs ]) {
        return false;
    }


    scale = myfonts[ fs ]->nSize;


    if (line_width < 0) {
        line_width = 1000000;
    }


    while (*sst->end != '\0' && *sst->end != '\n') {
        char c = *sst->end;

        int c_width = myfonts[ fs ]->nCharWidth[ c - 32 ] * scale; // FIX ME

        // we exceeded the space available for this line
        if (x + c_width > line_width) {
            if (in_a_word) {
                sst->end = (char *)word_start;
            }

            return t_words ? true : false;
        }


        x += c_width;

        if (c != ' ') {
            if (! in_a_word) {
                word_start = sst->end;
            }

            in_a_word = 1;
            chars_width += c_width;
        } else {
            if (in_a_word) {
                in_a_word = 0;
                t_words++;
            }

            t_spaces++;
        }

        ++sst->end;
    }

    if (in_a_word) {
        t_words++;
    }

    if (*sst->end != '\0' && *sst->end == '\n') {
        ++sst->end;
    }

    return t_words ? true : false;

}

/**
 * \brief Put paragraph on screen
 * \param[in] fs Font to use
 * \param[in] x X-Coordinate
 * \param[in] y Y-Coordinate
 * \param[in] string Text to put on screen
 * \param[in] space_between_lines Space between rows
 * \param[in] line_width_in_pixel Line width in pixels
 */
PUBLIC void Font_put_paragraph (FONTSELECT fs, short x, short y,
                                const char *string,
                                int space_between_lines,
                                int line_width_in_pixel)
{
    string_seg_t sst;
    sst.start = sst.end = (char *)string;

    if (! myfonts[ fs ]) {
        return;
    }

    while (Font_get_line (fs, line_width_in_pixel, &sst)) {
        Font_put_line_size (fs, x, y, sst.start, sst.end);

        if (*sst.end != '\0' && *sst.end == ' ') {
            sst.start = sst.end;
            ++sst.start;
            sst.end = sst.start;
        } else if (*sst.end != '\0' && *sst.end == '\n') {
            while (*sst.end == '\n') {
                ++sst.end;
                y += Font_GetSize (fs) + space_between_lines;
            }

            sst.start = sst.end;
        } else {
            sst.start = sst.end;
        }

        y += Font_GetSize (fs) + space_between_lines;
    }

}

