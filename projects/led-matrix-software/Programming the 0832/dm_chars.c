//      dm_chars.c - support functions for 5x8 characters
//
//      dm_str()   - write a string to the display
//      dm_char()  - write a character to the display
//      dm_ccol()  - write a column of mono font
//      dm_cload() - load mono or proportional font char

#include <iom16v.h>
#include <AVRdef.h>
#include "dm2.h"
#include "dm_chars.h"


//      these characters are defined in 5 columns on an 8
//      row cell.  the bottom line (bit 0) is off except
//      for mini descenders.

__flash char font[480] = {
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFA,0x00,0x00,   //  !
   0x00,0xC0,0x00,0xC0,0x00,0x28,0xFE,0x28,0xFE,0x28,   // "#
   0x24,0x54,0xFE,0x54,0x48,0xC4,0xC8,0x10,0x26,0x46,   // $%
   0x6C,0x92,0xAA,0x44,0x0A,0x00,0xA0,0xC0,0x00,0x00,   // &'
   0x00,0x38,0x44,0x82,0x00,0x00,0x82,0x44,0x38,0x00,   // ()
   0x28,0x10,0x7C,0x10,0x28,0x10,0x10,0x7C,0x10,0x10,   // *+
   0x00,0x05,0x06,0x00,0x00,0x10,0x10,0x10,0x10,0x10,   // ,-
   0x00,0x06,0x06,0x00,0x00,0x04,0x08,0x10,0x20,0x40,   // ./
   0x7C,0x82,0x82,0x82,0x7C,0x00,0x42,0xFE,0x02,0x00,   // 01
   0x42,0x86,0x8A,0x92,0x62,0x84,0x82,0xA2,0xD2,0x8C,   // 23
   0x18,0x28,0x48,0xFE,0x08,0xE4,0xA2,0xA2,0xA2,0x9C,   // 45
   0x3C,0x52,0x92,0x92,0x0C,0x80,0x8E,0x90,0xA0,0xC0,   // 67
   0x6C,0x92,0x92,0x92,0x6C,0x60,0x92,0x92,0x94,0x78,   // 89
   0x00,0x6C,0x6C,0x00,0x00,0x00,0x65,0x66,0x00,0x00,   // :;
   0x10,0x28,0x44,0x82,0x00,0x28,0x28,0x28,0x28,0x28,   // <=
   0x00,0x82,0x44,0x28,0x10,0x40,0x80,0x8A,0x90,0x60,   // >?
   0x4C,0x92,0x9E,0x82,0x7C,0x7E,0x88,0x88,0x88,0x7E,   // @A
   0xFE,0x92,0x92,0x92,0x6C,0x7C,0x82,0x82,0x82,0x44,   // BC
   0xFE,0x82,0x82,0x44,0x38,0xFE,0x92,0x92,0x92,0x82,   // DE
   0xFE,0x90,0x90,0x90,0x80,0x7C,0x82,0x92,0x92,0x5E,   // FG
   0xFE,0x10,0x10,0x10,0xFE,0x00,0x82,0xFE,0x82,0x00,   // HI
   0x04,0x02,0x82,0xFC,0x80,0xFE,0x10,0x28,0x44,0x82,   // JK
   0xFE,0x02,0x02,0x02,0x02,0xFE,0x40,0x30,0x40,0xFE,   // LM
   0xFE,0x20,0x10,0x08,0xFE,0x7C,0x82,0x82,0x82,0x7C,   // NO
   0xFE,0x90,0x90,0x90,0x60,0x7C,0x82,0x8A,0x84,0x7A,   // PQ
   0xFE,0x90,0x98,0x94,0x62,0x62,0x92,0x92,0x92,0x8C,   // RS
   0x80,0x80,0xFE,0x80,0x80,0xFC,0x02,0x02,0x02,0xFC,   // TU
   0xF8,0x04,0x02,0x04,0xF8,0xFC,0x02,0x1C,0x02,0xFC,   // VW
   0xC6,0x28,0x10,0x28,0xC6,0xE0,0x10,0x0E,0x10,0xE0,   // XY
   0x86,0x8A,0x92,0xA2,0xC2,0x00,0xFE,0x82,0x82,0x00,   // Z[
   0x40,0x20,0x10,0x08,0x04,0x00,0x82,0x82,0xFE,0x00,   // \]
   0x20,0x40,0x80,0x40,0x20,0x01,0x01,0x01,0x01,0x01,   // ^_
   0x00,0x80,0x40,0x20,0x00,0x04,0x2A,0x2A,0x2A,0x1E,   // `a
   0xFE,0x12,0x22,0x22,0x1C,0x1C,0x22,0x22,0x22,0x04,   // bc
   0x1C,0x22,0x22,0x12,0xFE,0x1C,0x2A,0x2A,0x2A,0x18,   // de
   0x10,0x7E,0x90,0x80,0x40,0x18,0x25,0x25,0x25,0x3E,   // fg
   0xFE,0x10,0x20,0x20,0x1E,0x00,0x22,0xBE,0x02,0x00,   // hi
   0x02,0x01,0x21,0xBE,0x00,0xFE,0x08,0x14,0x22,0x00,   // jk
   0x00,0x82,0xFE,0x02,0x00,0x3E,0x20,0x18,0x20,0x1E,   // lm
   0x3E,0x10,0x20,0x20,0x1E,0x1C,0x22,0x22,0x22,0x1C,   // no
   0x3F,0x24,0x24,0x24,0x18,0x18,0x24,0x24,0x34,0x1F,   // pq
   0x3E,0x10,0x20,0x20,0x10,0x12,0x2A,0x2A,0x2A,0x04,   // rs
   0x20,0xFC,0x22,0x02,0x04,0x3C,0x02,0x02,0x04,0x3E,   // tu
   0x38,0x04,0x02,0x04,0x38,0x3C,0x02,0x0C,0x02,0x3C,   // vw
   0x22,0x14,0x08,0x14,0x22,0x38,0x05,0x05,0x05,0x3E,   // xy
   0x22,0x26,0x2A,0x32,0x22,0x00,0x10,0x6C,0x82,0x00,   // z{
   0x00,0x00,0xFE,0x00,0x00,0x00,0x82,0x6C,0x10,0x00,   // |}
   0x08,0x10,0x10,0x08,0x10,0x00,0x00,0x40,0xA0,0x40};   // ^degree 


// ------------------------------------------------------------
//      dm_cload(cc, *buf, adj) - return a character definition
//
//      cc - character (dec 32 to 127)
//      buf[] - place to return (5 bytes)
//      adj - adjustment:
//              0 = none (returns 5)
//              1 = left, minimum (returns # of bytes in char)
//                  (leaves trailing bytes as is)
//              2 = left, zero (blank) filled to 5
//                  (returns # of bytes in char)
//
//      returns: 5 or number of nonzero bytes (width of character)
//
//      while each character is defined on a 5x8 grid, the actual
//      character may take less than 5 columns.  if adj is nonzero,
//      the return is for the number of nonblank bytes returned.
//      if adj = 1, then only that number of bytes is returned.
//      if adj = 2, then 5 bytes are returned, with zeroes following
//      the nonzero bytes that make up the character.

char dm_cload(char cc, char *p, char adj) {
  int offset;
  char i, n, v, skip;

  if (cc < 32 || cc > 127) cc = 32;
  offset = (int) (cc - 32) * 5;   // blank is first in table

  if (!adj) {
    for (i = 0; i < 5; i++) *p++ = font[offset++];
    return 5;
  }

  if (adj == 2) {
    for (i = 0; i < 5; i++) *(p + i) = 0;
  }

  n = skip = 0;
  for (i = 0; i < 5; i++) {     // 5 columns per character
    v = font[offset++];         // a column of definition
    if (n) {                    // have we seen anything yet?
      if (v) {                  // yes - is this anything?
        while (skip) {          // yes - fill in any zeroes
          *p++ = 0;
          n++;                  // count them
          skip--;
        }
        *p++ = v;               // and put away this value
        n++;                    // count it
      } else {                  // no - remember we skipped it
        skip++;
      }
    } else {                    // haven't seen anything yet
      if (v) {                  // is this anything?
        *p++ = v;               // our first nonzero - save it
        n = 1;                  // and count it
      }
    }
  }
  return n;
}

// ------------------------------------------------------------
//      dm_ccol(col, cc) - return single column of a whole character
//
//      col - column (0 - 4)
//      cc - ascii character
//
//      this returns one column of the full 5x8 character definition

char dm_ccol(char col, char cc) {
  int offset;

  if (cc < 32 || cc > 127) cc = 32;
  offset = (int) (cc - 32) * 5 + col;   // blank is first in table
  return font[offset];
}

// ------------------------------------------------------------
//      dm_char(col, cc, fmt) - put out a char to board(s)
//
//      col - left hand column of where to start character:
//         1 board,  0 - 31
//         2 boards, 0 - 63
//         3 boards, 0 - 95
//         4 boards, 0 - 127
//      cc - ascii character (32 - 127)
//      fmt - format (OR in the following):
//         0-7 - right padding
//         8   - raw character (5 columns wide)
//               otherwise, the actual character width is used
//               (proportional font)
//         #   - width of blank * 16:
//               0x00 - 0 columns wide  (CC_BL_0)
//               0x10 - 1 column wide   (CC_BL_1)
//                ...   ...
//               0x70 - 7 columns wide  (CC_BL_7)
//               note: blanks are 5 columns wide in mono font,
//                 so this applies only to proportional
//         example: to have 2 columns between characters, with
//           blanks taking 4 columns, fmt = 0x42 = 66
//
//      returns: number of columns output (0 - 12), and
//          bit CC_TRUNC will be set if something was truncated 
//          bit CC_TCHAR will be set if the truncation occurred within
//              the character definition itself (not in the padding)
//
//      zero will be returned if the character is outside of the
//      range (32 - 127), the column is out of range, or for blanks
//      with no padding if a blank width has not been specified.
//
//      this will truncate the character at rightmost column

char dm_char(char col, char cc, char fmt) {
  char wbl, rf, pad, buf[5], w;
  char cnt;

  if (col > dm_cmax) return 0;          // column too big?
  if (cc < 32 || cc > 127) return 0;    // character out of range

  cnt = 0;                              // number of columns written
  pad = fmt & 0x07;                     // number of padding columns (0 - 7)
  rf = (fmt & 0x08) ? 0 : 1;            // =0 for mono, =1 for proportional
  wbl = (fmt >> 4) & 0x07;              // width of blank (0 - 7)
  
  w = dm_cload(cc, buf, rf);            // load pattern into buf[]
  if (!w) {                             // w = 0 means proportional blank
    if (wbl) {                          // are we putting out blanks?
      cnt = dm_fill(col, (int) wbl, 0); // put out some blank columns
      if (cnt != wbl) return (cnt | CC_TRUNC | CC_TCHAR);  // truncated
    }
  } else {                              // normal character
    cnt = dm_wbytes(col, buf, (int) w); // output the columns
    if (cnt != w) return (cnt | CC_TRUNC | CC_TCHAR);  // truncated
  }

  if (pad) {                            // we got the character out OK
    col += cnt;                         // skip over what just went out
    w = dm_fill(col, (int) pad, 0);     // put out some blank columns
    cnt += w;                           // how many columns we put out
    if (w != pad) cnt |= CC_TRUNC;      // did we truncate the padding?
  }
  return cnt;
}

// ------------------------------------------------------------
//      dm_str(col, *msg, fmt) - write a string to the display
//
//      col - starting column number (0 - 31, 63, 95, or 127)
//      msg - pointer to string
//      fmt - format (OR in the following):
//         0-7 - right padding
//         8   - mono (raw characters, 5 columns wide)
//               otherwise, the actual character width is used
//               (proportional font)
//         #   - width of blank * 16:
//               0x00 - 0 columns wide  (CC_BL_0)
//               0x10 - 1 column wide   (CC_BL_1)
//                ...   ...
//               0x70 - 7 columns wide  (CC_BL_7)
//         example: to have 2 columns between characters, with
//           blanks taking 4 columns, fmt = 0x42 = 66
//
//      returns: number of columns output (includes padding on
//        final character if no trucation), plus optional:
//        CC_ST_ERR - truncation error (some sort of failure)
//        CC_ST_OK  - truncation occurred on padding of final char

int dm_str(char col, char *p, char fmt) {
  char c, r, ot;
  int rtn;

  rtn = 0;
  while (c = *p++) {                    // get next char
    r = dm_char(col, c, fmt);           // try to write it
    ot = r & 0x0f;                      // columns we got out
    rtn += ot;                          // total columns written
    col += ot;                          // advance column number
    if (r & CC_TRUNC) {                 // did we truncate?
      if (!(r & CC_TCHAR) && !(*p)) rtn |= CC_ST_OK; // yes, in final pad
      return rtn | CC_ST_ERR;
    }
  }
  return rtn;
}
