//
// Created by srdja on 9/25/15.
//

#ifndef WOLF3D_REDUX_COMPRESSION_H
#define WOLF3D_REDUX_COMPRESSION_H

void Lvl_CarmackExpand (uint16_t *source, uint16_t *dest, uint16_t length);
void Lvl_RLEWexpand (uint16_t *source, uint16_t *dest, long length, unsigned rlewtag);

#endif //WOLF3D_REDUX_COMPRESSION_H
