#ifndef __TETRIS_H_
#define __TETRIS_H_

#include <avr/io.h>
#include <stdint.h>
#include <stdlib.h>
#include "draw.h"

// shape types:
// 0 - I
// 1 - L
// 2 - J
// 3 - O
// 4 - Z
// 5 - S
// 6 - T
uint8_t ShouldPlace(uint8_t *field, uint8_t x, int8_t y, uint8_t shape, uint8_t rotation);
uint8_t CanMove(uint8_t *field, uint8_t x, int8_t y, uint8_t shape, uint8_t rotation, uint8_t direction); // direction == 0: left, else - right
void DrawShape(uint8_t *image, uint8_t x, uint8_t y, uint8_t shape, uint8_t rotation, uint8_t color);
void CheckX(uint8_t *x, uint8_t shape, uint8_t rotation);
#endif
