#ifndef __DRAW_H_
#define __DRAW_H_

#include <avr/io.h>
#include <stdint.h>
#include <stdlib.h>

// image = 96 bytes array; x = [1, 16]; y = [1, 16]; color = BGR (3-bits);
/* Color:
** 0	Black (LEDs off)
** 1	Red
** 2	Green
** 3	Yellow
** 4	Blue
** 5	Pink
** 6	Cyan
** 7	White (Full on)
*/
void SetPixel(uint8_t *image, int8_t x, int8_t y, uint8_t color);
uint8_t GetPixel(uint8_t *image, int8_t x, int8_t y);
void DrawLine(uint8_t *image, int8_t x0, int8_t y0, int8_t x1, int8_t y1, uint8_t color);
void FillRect(uint8_t *image, int8_t x0, int8_t y0, int8_t x1, int8_t y1, uint8_t color);
void FloodFill(uint8_t *image, uint8_t color);
void BitBlt(uint8_t *src, int8_t src_x, int8_t src_y, uint8_t width, uint8_t height, uint8_t *desc, int8_t desc_x, int8_t desc_y);
void PrintChar(uint8_t *image, int8_t x, int8_t y, uint8_t character, uint8_t color);
/******************************************************************************************************
** CHAR TABLE *****************************************************************************************
*******************************************************************************************************
+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+----+-----+-----+-----+-----+-----+
| 0  0| 1  1| 2  2| 3  3| 4  4| 5  5| 6  6| 7  7| 8  8| 9  9|10  A|11 B|12  C|13  D|14  E|15  F|16  G|
+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+----+-----+-----+-----+-----+-----+
|17  H|18  I|19  J|20  K|21  L|22  M|23  N|24  O|25  P|26  Q|27  R|28 S|29  T|30  U|31  V|32  W|33  X|
+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+----+-----+-----+-----+-----+-----+
|34  Y|35  Z|36  a|37  b|38  c|39  d|40  e|41  f|42  g|43  h|44  i|45 j|46  k|47  l|48  m|49  n|50  o|
+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+----+-----+-----+-----+-----+-----+
|51  p|52  q|53  r|54  s|55  t|56  u|57  v|58  w|59  x|60  y|61  z|
+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
*******************************************************************************************************/

#endif
