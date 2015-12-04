/*

    Copyright (C) 2004-2005 Michael Liebscher <johnnycanuck@users.sourceforge.net>
    Copyright (C) 1995 Spencer Kimball and Peter Mattis

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
 * \file tga.c
 * \brief Handle Targa file format.
 * \author Michael Liebscher
 * \date 2004-2005
 * \note
 *      Portion of this code was derived from The GIMP -- an image manipulation
 *      program, and was originally written by Spencer Kimball and Peter Mattis.
 */

#include <string.h>
#include <stdlib.h>

#include "tga.h"
#include "texture_manager.h"


#define TGA_HEADER_SIZE     18


/* TRUEVISION-XFILE magic signature string */
static uint8_t magic[ 18 ] = {
    0x54, 0x52, 0x55, 0x45, 0x56, 0x49, 0x53, 0x49, 0x4f,
    0x4e, 0x2d, 0x58, 0x46, 0x49, 0x4c, 0x45, 0x2e, 0x0
};

typedef struct _TargaHeader {
    uint8_t idLength;
    uint8_t colorMapType;

    uint8_t imageType;
    /* Known image types. */
#define TGA_TYPE_MAPPED      1
#define TGA_TYPE_COLOR       2
#define TGA_TYPE_GRAY        3

    uint8_t imageCompression;
    /* Only known compression is RLE */
#define TGA_COMP_NONE        0
#define TGA_COMP_RLE         1

    /* Color Map Specification. */
    uint16_t colorMapIndex;
    uint16_t colorMapLength;
    uint8_t colorMapSize;

    /* Image Specification. */
    uint16_t xOrigin;
    uint16_t yOrigin;

    uint16_t width;
    uint16_t height;

    uint8_t bpp;
    uint8_t bytes;

    uint8_t alphaBits;
    uint8_t flipHoriz;
    uint8_t flipVert;

} TargaHeader;


static void flip_line (uint8_t *buffer, TargaHeader *info)
{
    uint8_t temp;
    uint8_t *alt;
    int32_t x, s;

    alt = buffer + (info->bytes * (info->width - 1));

    for (x = 0; x * 2 <= info->width; ++x) {
        for (s = 0; s < info->bytes; ++s) {
            temp = buffer[ s ];
            buffer[ s ] = alt[ s ];
            alt[ s ] = temp;
        }

        buffer += info->bytes;
        alt -= info->bytes;
    }
}

static void upsample (uint8_t *dest, uint8_t *src,
                       uint32_t width, uint32_t bytes, uint8_t alphaBits)
{
    uint32_t x;

    for (x = 0 ; x < width ; ++x) {
        dest[0] = ((src[1] << 1) & 0xf8);
        dest[0] += (dest[0] >> 5);

        dest[1] = ((src[0] & 0xe0) >> 2) + ((src[1] & 0x03) << 6);
        dest[1] += (dest[1] >> 5);

        dest[2] = ((src[0] << 3) & 0xf8);
        dest[2] += (dest[2] >> 5);

        switch (alphaBits) {
        case 1:
            dest[ 3 ] = (src[ 1 ] & 0x80) ? 0 : 255;
            dest += 4;
            break;

        default:
            dest += 3;
        }

        src += bytes;
    }
}

static void bgr2rgb (uint8_t *dest, uint8_t *src,
                      uint32_t width, uint32_t bytes, uint32_t alpha)
{
    uint32_t x;

    if (alpha) {
        for (x = 0 ; x < width ; ++x) {
            * (dest++) = src[2];
            * (dest++) = src[1];
            * (dest++) = src[0];

            * (dest++) = src[3];

            src += bytes;
        }
    } else {
        for (x = 0 ; x < width ; ++x) {
            * (dest++) = src[2];
            * (dest++) = src[1];
            * (dest++) = src[0];

            src += bytes;
        }
    }


}

static int32_t rle_read (filehandle_t *fp, uint8_t *buffer,
                       TargaHeader *info)
{
    static int32_t repeat = 0;
    static int32_t direct = 0;
    static uint8_t sample[ 4 ];
    int32_t head;
    uint8_t temphead;
    int32_t x, k;

    for (x = 0; x < info->width; ++x) {
        if (repeat == 0 && direct == 0) {
            FS_ReadFile (&temphead, 1, 1, fp);
            head = temphead;

            if (head >= 128) {
                repeat = head - 127;

                if (FS_ReadFile (sample, info->bytes, 1, fp) < 1) {
                    return EOF;
                }
            } else {
                direct = head + 1;
            }
        }

        if (repeat > 0) {
            for (k = 0 ; k < info->bytes ; ++k) {
                buffer[ k ] = sample[ k ];
            }

            repeat--;
        } else { /* direct > 0 */
            if (FS_ReadFile (buffer, info->bytes, 1, fp) < 1) {
                return EOF;
            }

            direct--;
        }

        buffer += info->bytes;
    }

    return 0;
}


static void read_line (filehandle_t *fp,
                        uint8_t *row,
                        uint8_t *buffer,
                        TargaHeader     *info)
{
    if (info->imageCompression == TGA_COMP_RLE) {
        if (rle_read (fp, buffer, info) == EOF) {
            return;
        }
    } else {
        FS_ReadFile (buffer, info->bytes, info->width, fp);
    }

    if (info->flipHoriz) {
        flip_line (buffer, info);
    }

    if (info->imageType == TGA_TYPE_COLOR) {
        if (info->bpp == 16 || info->bpp == 15) {
            upsample (row, buffer, info->width, info->bytes, info->alphaBits);
        } else {
            bgr2rgb (row, buffer, info->width, info->bytes, info->bytes == 4 ? 1 : 0);
        }
    } else {
        memcpy (row, buffer, info->width * info->bpp);
    }
}



void LoadTGA (const char *filename, uint8_t **pic, uint16_t *width, uint16_t *height, uint16_t *bytes)
{
    TargaHeader     targa_header;
    uint8_t header[ 18 ];
    uint8_t footer[ 26 ];
    uint8_t extension[ 495 ];
    uint32_t cmap_bytes;
    int32_t offset;
    uint8_t tga_cmap[4 * 256], gimp_cmap[3 * 256];
    uint8_t *buffer, *data, *row;
    int i;
    int32_t datalength;
    filehandle_t *hFile;


    *pic = NULL;

//
// Load the file
//
    hFile = FS_OpenFile (filename);

    if (! hFile) {
        printf("Could not open (%s) for reading\n", filename);

        goto TGALOADFAILED;
    }

    datalength = FS_GetFileSize (hFile);

    if (! FS_FileSeek (hFile, -26L, SEEK_END)) {
        /* Is file big enough for a footer? */
        if (FS_ReadFile (footer, sizeof (footer), 1, hFile) != 1) {
            printf("Cannot read footer from (%s)\n", filename);

            goto TGALOADFAILED;
        } else if (memcmp (footer + 8, magic, sizeof (magic)) == 0) {
            /* Check the signature. */
            offset = footer[ 0 ] + (footer[ 1 ] * 256) + (footer[ 2 ] * 65536)
                     + (footer[ 3 ] * 16777216);

            if (offset != 0) {
                if (FS_FileSeek (hFile, offset, SEEK_SET) ||
                        FS_ReadFile (extension, sizeof (extension), 1, hFile) != 1) {
                    printf("Cannot read extension from '%s'\n", filename);

                    goto TGALOADFAILED;
                }

                /* Eventually actually handle version 2 TGA here */

            }
        }

    }


//
// Get header information.
//
    if (datalength < TGA_HEADER_SIZE) {
        goto TGALOADFAILED;
    }

    if (FS_FileSeek (hFile, 0, SEEK_SET) ||
            FS_ReadFile (header, sizeof (header), 1, hFile) != 1) {
        goto TGALOADFAILED;
    }



    targa_header.idLength = header[ 0 ];
    targa_header.colorMapType = header[ 1 ];

    switch (header[ 2 ]) {
    case 1:
        targa_header.imageType = TGA_TYPE_MAPPED;
        targa_header.imageCompression = TGA_COMP_NONE;
        break;

    case 2:
        targa_header.imageType = TGA_TYPE_COLOR;
        targa_header.imageCompression = TGA_COMP_NONE;
        break;

    case 3:
        targa_header.imageType = TGA_TYPE_GRAY;
        targa_header.imageCompression = TGA_COMP_NONE;
        break;



    case 9:
        targa_header.imageType = TGA_TYPE_MAPPED;
        targa_header.imageCompression = TGA_COMP_RLE;
        break;

    case 10:
        targa_header.imageType = TGA_TYPE_COLOR;
        targa_header.imageCompression = TGA_COMP_RLE;
        break;

    case 11:
        targa_header.imageType = TGA_TYPE_GRAY;
        targa_header.imageCompression = TGA_COMP_RLE;
        break;


    default:
        targa_header.imageType = 0;
    }

    targa_header.colorMapIndex  = header[ 3 ] + header[ 4 ] * 256;
    targa_header.colorMapLength = header[ 5 ] + header[ 6 ] * 256;
    targa_header.colorMapSize   = header[ 7 ];

    targa_header.xOrigin = header[ 8  ] + header[ 9  ] * 256;
    targa_header.yOrigin = header[ 10 ] + header[ 11 ] * 256;
    targa_header.width   = header[ 12 ] + header[ 13 ] * 256;
    targa_header.height  = header[ 14 ] + header[ 15 ] * 256;

    targa_header.bpp       = header[ 16 ];
    targa_header.bytes     = (targa_header.bpp + 7) / 8;
    targa_header.alphaBits = header[ 17 ] & 0x0f; /* Just the low 4 bits */
    targa_header.flipHoriz = (header[ 17 ] & 0x10) ? 1 : 0;
    targa_header.flipVert  = (header[ 17 ] & 0x20) ? 0 : 1;

//
// Analyze header information.
//
    switch (targa_header.imageType) {
    case TGA_TYPE_MAPPED:
        if (targa_header.bpp != 8) {
            printf("Unhandled sub-format in (%s)\n", filename);

            goto TGALOADFAILED;
        }

        goto TGALOADFAILED;

        break;

    case TGA_TYPE_COLOR:
        if (targa_header.bpp != 15 && targa_header.bpp != 16 && targa_header.bpp != 24
                && targa_header.bpp != 32) {
            printf("Unhandled sub-format in (%s)\n", filename);
            goto TGALOADFAILED;
        }

        break;

    case TGA_TYPE_GRAY:
        if (targa_header.bpp != 8 && (targa_header.alphaBits != 8 || (targa_header.bpp != 16 && targa_header.bpp != 15))) {
            printf("Unhandled sub-format in (%s)\n", filename);
            goto TGALOADFAILED;
        }


        goto TGALOADFAILED;


        break;

    default:
        printf("Unknown image type for (%s)\n", filename);
        goto TGALOADFAILED;

    } /* end of switch targa_header.imageType */

    /* Plausible but unhandled formats */
    if (targa_header.bytes * 8 != targa_header.bpp && ! (targa_header.bytes == 2 && targa_header.bpp == 15)) {
        printf("No support yet for TGA with these parameters\n");

        goto TGALOADFAILED;
    }

    /* Check that we have a color map only when we need it. */
    if (targa_header.imageType == TGA_TYPE_MAPPED && targa_header.colorMapType != 1) {
        printf("Indexed image has invalid color map type %d\n",
               targa_header.colorMapType);

        goto TGALOADFAILED;
    } else if (targa_header.imageType != TGA_TYPE_MAPPED && targa_header.colorMapType != 0) {
        printf("Non-indexed image has invalid color map type %d\n",
               targa_header.colorMapType);

        goto TGALOADFAILED;
    }

    /* Skip the image ID field. */
    if (targa_header.idLength && FS_FileSeek (hFile, targa_header.idLength, SEEK_CUR)) {
        printf("File (%s) is truncated or corrupted\n", filename);

        goto TGALOADFAILED;
    }

    /* Handle colormap */
    if (targa_header.colorMapType == 1) {
        cmap_bytes = (targa_header.colorMapSize + 7) / 8;

        if (cmap_bytes <= 4 &&
                FS_ReadFile (tga_cmap, targa_header.colorMapLength * cmap_bytes, 1, hFile) == 1) {
            if (targa_header.colorMapSize == 32) {
                bgr2rgb (gimp_cmap, tga_cmap, targa_header.colorMapLength, cmap_bytes, 1);
            } else if (targa_header.colorMapSize == 24) {
                bgr2rgb (gimp_cmap, tga_cmap, targa_header.colorMapLength, cmap_bytes, 0);
            } else if (targa_header.colorMapSize == 16 || targa_header.colorMapSize == 15) {
                upsample (gimp_cmap, tga_cmap, targa_header.colorMapLength, cmap_bytes, targa_header.alphaBits);
            }

        } else {
            printf("File (%s) is truncated or corrupted\n", filename);

            goto TGALOADFAILED;
        }
    }


    /* Allocate the data. */
    data = malloc (targa_header.width * targa_header.height * targa_header.bytes);

    buffer = malloc (targa_header.width * targa_header.bytes);

    if (buffer == NULL) {
        free (data);
    }

    if (targa_header.flipVert) {
        for (i = targa_header.height - 1 ; i >= 0 ; --i) {
            row = data + (targa_header.width * targa_header.bytes * i);
            read_line (hFile, row, buffer, &targa_header);
        }
    } else {
        for (i = 0 ; i < targa_header.height ; ++i) {
            row = data + (targa_header.width * targa_header.bytes * i);
            read_line (hFile, row, buffer, &targa_header);
        }
    }


    free (buffer);

    FS_CloseFile (hFile);

    *pic = data;
    *width = targa_header.width;
    *height = targa_header.height;
    *bytes = targa_header.bytes;

    return;



TGALOADFAILED:
    *pic = NULL;
    *width = 0;
    *height = 0;
    *bytes = 0;

    if (hFile) {
        FS_CloseFile (hFile);
    }

}