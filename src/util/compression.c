//
// Created by srdja on 9/25/15.
//

#include <stdint.h>

#include "compression.h"



/**
 * \brief Expand Carmack compressed data
 * \param[in] source Source data
 * \param[in/out] dest Destination buffer
 * \param[in] length The length of the EXPANDED data.
 */
void Lvl_CarmackExpand (uint16_t *source, uint16_t *dest, uint16_t length)
{
#define NEARTAG 0xA7
#define FARTAG  0xA8

    uint32_t chhigh, offset;
    uint16_t *copyptr, *outptr;
    uint8_t *inptr;
    uint16_t ch, count;

    length /= 2;

    inptr = (uint8_t *)source;
    outptr = dest;

    while (length) {
        ch = * (uint16_t *)inptr;
        inptr += 2;
        chhigh = ch >> 8;

        if (chhigh == NEARTAG) {
            count = ch & 0xff;

            if (! count) {
                // have to insert a word containing the tag byte
                ch |= *inptr++;
                *outptr++ = (uint16_t)ch;
                length--;
            } else {
                offset = *inptr++;
                copyptr = outptr - offset;
                length -= count;

                while (count--) {
                    *outptr++ = *copyptr++;
                }
            }
        } else if (chhigh == FARTAG) {
            count = ch & 0xff;

            if (! count) {
                // have to insert a word containing the tag byte
                ch |= *inptr++;
                *outptr++ = ch;
                length--;
            } else {
                offset = * (uint16_t *)inptr;
                inptr += 2;
                copyptr = dest + offset;
                length -= count;

                while (count--) {
                    *outptr++ = *copyptr++;
                }
            }
        } else {
            *outptr++ = ch;
            length--;
        }
    }
}

/**
 * \brief Expand Run-Length-Encoded data
 * \param[in] source Source data
 * \param[in/out] dest Destination buffer
 * \param[in] length The length of the EXPANDED data.
 * \param[in] rlewtag tag
 */
void Lvl_RLEWexpand (uint16_t *source, uint16_t *dest, long length, unsigned rlewtag)
{
    unsigned value, count, i;
    uint16_t *end;
//
// expand it
//
    end = dest + (length >> 1);

    do {
        value = *source++;

        if (value != rlewtag) {
            //
            // uncompressed
            //
            *dest++ = value;
        } else {
            //
            // compressed string
            //
            count = *source++;
            value = *source++;

            for (i = 1 ; i <= count ; ++i) {
                *dest++ = value;
            }
        }

    } while (dest < end);

}