// ad_7seg.c
//
// source file for Sure Electronics AVRDEM2 board test routines
// written by Chuck Baird (avrfreaks.net, user zbaird)
// you may freely use this code for any purpose; attribution is appreciated

#include "ad_7seg.h"

char seg7_stat;         // flag for 7 segment display on
// digits[0-15] are numbers 0 - 9, a - f
char seg7_dig[] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x67,
                   0x77, 0x7c, 0x58, 0x5e, 0x79, 0x71 };
char seg7_s[4];         // segment outputs ([0] = leftmost)
char s7cnt;             // interrupt counter

// -------------------------------------------------------------
// init_7seg() - initialization for 7 segment display

void init_7seg(void) {
  DDRB = 0xff;                  // all of B is output
  DDRA = DDRA | 0xf0;           // top half of A is output

  seg7(SEG7_OFF);               // initially it's off
  s7cnt = 0;                    // interrupt counter
}

// -------------------------------------------------------------
// seg7(cmd) - turn 7 segment display on/off
//
// cmd - 0  turn display off (SEG7_OFF)
//     - 1  turn display on  (SEG7_ON)

void seg7(char cmd) {
  seg7_stat = cmd & 0x01;
}

// -------------------------------------------------------------
// seg7_hex4(num, pos) - display 4 hex digits on 7 segment display
//
// num - unsigned int to display

void seg7_hex4(unsigned int num) {

  seg7_hex2(num >> 8, 0);       // left 2 digits
  seg7_hex2(num, 1);            // right 2 digits

}

// -------------------------------------------------------------
// seg7_hex2(num, pos) - display 2 hex digits on 7 segment display
//
// num - value to display (0 - 255)
// pos - position (0 = left, 1 = right, add 2 to blank other side

void seg7_hex2(char num, char pos) {

  pos <<= 1;                    // position 0 or 2, maybe blanked
  seg7_hex1(num >> 4, pos);
  pos &= 3;                     // drop blanking
  seg7_hex1(num, pos + 1);      // position 1 or 3

}

// -------------------------------------------------------------
// seg7_hex1(num, pos) - display hex digit on 7 segment display
//
// num - number (0 - 15)
// pos - position (0 - 3), add 4 to blank display first
//       0 = thousands (left), 3 = ones (right)

void seg7_hex1(char num, char pos) {

  if (pos > 3) {
    char i;
    for (i = 0; i < 4; i++) seg7_s[i] = 0;
    pos &= 0x03;
  }

  seg7_s[pos] = seg7_dig[num & 0x0f];
  seg7(SEG7_ON);                        // turn display on
}

// -------------------------------------------------------------
// seg7_num(num, mode) - display number on 7 segment display
//
// num  - integer (-999 to 9999, out of range shows '----')
// mode - decimal point & leading zero
//          bits 0 - 1 : leading zero fill flag
//                       00 - fill to one place    (FILL7_1)
//                       01 - fill to two places   (FILL7_2)
//                       10 - fill to three places (FILL7_3)
//                       11 - fill to four places  (FILL7_4)
//          bits 2 - 5 : if 1, light decimal in this place
//                       bit 2 = 1's dp; bit 5 = 1000's dp
//                       (may use DP_1, DP_10, DP_100, and DP_1000)

void seg7_num(int n, char m) {
  int j, d;
  char i, neg, fill, mask, last, supp;

  if (n < -999 || n > 9999) {
    for (i = 0; i < 4; i++) seg7_s[i] = 0x40; // out of range shows minus signs
  } else {
    fill = m & 0x03;            // digits to fill indicator

    if (n < 0) {                // value negative?
      n = -n;                   // yup - get abs val
      neg = 1;                  // and remember it
      if (fill > FILL3) fill = FILL3;  // negs can only fill to 3 places
    } else {
      neg = 0;                  // already positive
    }

    mask = DP_1;                // bit 2 mask
    last = 3;                   // show at least one zero

    for (j = 3; j >= 0; j--) {  // go from 3 to 0 (1's to 1000's)
      d = n % 10;               // get numeric value of place
      if (d) last = j;          // remember pass that was last non-zero
      n = n / 10;               // ready for next time place
      seg7_s[j] = seg7_dig[d] | ((m & mask) ? 0x80 : 0x00);  // decimal pt?
      mask <<= 1;               // shift mask to next higher bit
    }

    // we have built the digits, zero filled. now go correct for sign
    // and leading zero suppression, as appropriate. there are 32 cases,
    // but fortunately it all falls out nicely.

    supp = last;                // number of digits to suppress
    if (supp > 3 - fill) supp = 3 - fill;

    // if we're suppressing, change zeroes to blanks (or maybe minus)
    if (supp) {
      for (i = 0; i < supp; i++) {
        seg7_s[i] &= 0x80;      // drop all but decimal point (if any)
        if ((i == supp - 1) && neg) seg7_s[i] |= 0x40;  // minus sign
      }
    }
  }
  seg7(SEG7_ON);                        // turn display on
}

// -------------------------------------------------------------
// seg7_disp(*s) - display segments on 7 segment display
//
// s - pointer to 4 chars defining segments
//     LED segments mapping (1 = on)
//       -        bit 0
//      | |       bits 5, 1
//       -        bit 6
//      | |       bits 4, 2
//       -  X     bits 3, 7

void seg7_disp(char *s) {
  char i;

  for (i = 0; i < 4; i++) seg7_s[i] = *s++;
  seg7(SEG7_ON);                        // turn display on
}


