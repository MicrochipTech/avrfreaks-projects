/*
 * lanaCOM - 4 bit logic analyzer using COM-port interface
 * Copyright (c) 2008, Dipl.-Ing. Hans-Juergen Heinrichs
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */


/* ---------------------------------------------------------------------------
 *    t c n t 2
 *
 *    Access/control functions for standard PC timer/counter 2
 *
 * ---------------------------------------------------------------------------
 */

/*
Modes of timer channels (CLK=1.193MHz)
0: single timeout

CLK   __--__--__--__--__--__--__--__--__--__--__--__--__--__--
GATE  ____________--------------------------------____________
OUT   ________________________--------------------------------
Count | 4 | 4 | 4 | 3 | 2 | 1 | 0 | FF| FE| FD| FC| FB| FA| F9

1: retriggerable one-shot

CLK   __--__--__--__--__--__--__--__--__--__--__--__--__--__--
GATE  __________-__________________________-__________________
OUT   ------------____________----------------____________----
Count | 3 | 3 | 3 | 3 | 2 | 1 | 0 | 3 | 3 | 3 | 3 | 2 | 1 | 0

2: rate generator (auto reload)

      (Timer1  66.3KHz)
CLK   __--__--__--__--__--__--__--__--__--__--__--__--__--__--
GATE  ------------------------------____________--------------
OUT   --------____--------____--------------------____--------
Count | 3 | 2 | 1 | 3 | 2 | 1 | 3 | 2 | 2 | 2 | 2 | 1 | 3 | 2

3: square wave mode (auto reload)

     (Timer0  18.2Hz)
CLK  -__--__--__--__--__--__--__--__--__--__--__--__--__--__--_
GATE -------------------------------------------________-------
OUT  _------------____________------------____________________-
Count | 6 | 4 | 2 | 6 | 4 | 2 | 6 | 4 | 2 | 6 | 4 | 4 | 4 | 2

For odd counts, the timer output line will be high for (N+1)/2
counts. The output line will return low for (N-1)/2 counts.
N is the loaded count value.

4: software triggered strobe

CLK   __--__--__--__--__--__--__--__--__--__--__--__--__--__--
GATE  -------------------________-----------------------------
OUT   ----------------------------------------____------------
Count | x | x | 6 | 5 | 4 | 4 | 4 | 3 | 2 | 1 | 0 | FF| FF| FF

5: hardware retriggerable strobe

CLK   __--__--__--__--__--__--__--__--__--__--__--__--__--__--
GATE  ______-_________________________________________________
OUT   --------------------------------____--------------------
Count | 6 | 6 | 6 | 5 | 4 | 3 | 2 | 1 | 0 | FF| FF| FF| FF| FF



Port 43h
         _______________________________
  bit:  | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
         -------------------------------
          |   |   |   |   |   |   |   |
  +-------+---+   +---+   +---+---+   +--------- 0 - Binary
  |               |               |              1 - BCD
  0 0 - Timer0    |               +------- 0 0 0 - Mode 0
  0 1 - Timer1    0 0 - Latch Counter      0 0 1 - Mode 1
  1 0 - Timer2    0 1 - LSB                x 1 0 - Mode 2
  1 1 --------+   1 0 - MSB                x 1 1 - Mode 3
              |   1 1 - 16-bit             1 0 0 - Mode 4
              |                            1 0 1 - Mode 5
              1 1 0 1 - General Counter Latch
              1 1 1 0 - Latch Status of Timers

   LSB  - r/w least significant byte only
   MSB  - r/w most significant byte only
   16-bit - r/w least significant byte first, then most
            significant byte.

Latch Counter --
counter value is transferred into the output latch register. The latched
value is held until read by port 40h..42h or a new counter value is
loaded into timer 0..2. Subsequent counter latch commands are ignored until
the value is read. The lower 4 bits of this command byte are ignored.

When not latched, the value read from port 40h..42h is the immediate
contents timer 0..2.

General Counter Latch --
similar to Latch Counter. Timers 0..2 can be latched independently or in
combination:
    bit   3 - select timer 2,
          2 - select timer 1,
          1 - select timer 0,
          0 - unused.

Latch Status of Timers --
latches the status of the selected timer(s) into the output latch. Once a
timer's status has been latched, the status is read from the respective
timer port 40h..42h. Bits 0..3 used as well as in the General Counter Latch
command. Subsequent status latch commands are ignored for any timer that
have already been latched but have not yet been read. Status byte information
shown below.

    bit   7     - State of timer's output pin
          6     - 0 Null Count, a new count has been loaded into counter
          5,4   - r/w mode (LSB, MSB, 16-bit)
          3,2,1 - Mode 0..5
          0     - binary/BCD
*/

#include "inout.h"

#define TMR0        0x40
#define TMR1        0x41
#define TMR2        0x42
#define TMR_CNTRL   0x43

/* -------------------------------------------------------------------------------- */
/*  Function name:      t c n t 2 _ s t a r t ()                                    */
/*                                                                                  */
/*  Returns:            -                                                           */
/*  Parameters:         -                                                           */
/*  Description:        Configures timer/counter 2 as One shot timer (see above).   */
/*                      The counter itself if free running.                         */
/*                      8 bit or 16 bit access width can be configured for          */
/*                      subsequent reads                                            */
/* -------------------------------------------------------------------------------- */
void
tcnt2_start(void)
{
#if (TIMER_WIDTH == 2)
        _outp( TMR_CNTRL, 0xb2 );  /* CNTR2, LSB+MSB, One Shot.    */
#else
        _outp( TMR_CNTRL, 0x92 );  /* CNTR2, LSB, One Shot.        */
#endif
}

/* -------------------------------------------------------------------------------- */
/*  Function name:      t c n t 2 _ r e a d ()                                      */
/*                                                                                  */
/*  Returns:            Current counter value                                       */
/*  Parameters:         -                                                           */
/*  Description:        Reads out the current counter value.                        */
/*                      Depending on the configuration one or two bytes are read.   */
/*                      TMR2 is a down counter - this function returns the          */
/*                      inverted value which actually make ist upcounting.          */
/* -------------------------------------------------------------------------------- */
unsigned short
tcnt2_read(void)
{
    // Timer2 is a decrementing counter. That's why we return the
    // invers timer value to have incrementing counting time.
    unsigned int result = 0;

#if (TIMER_WIDTH==2)
    unsigned char lsb;
    unsigned char msb;
    _outp( TMR_CNTRL, 0x80 );   /* Latch to make the result atomic. */
    lsb = _inp(TMR2);
    msb = _inp(TMR2);
    result = ~((msb<<8) + lsb) & 0xff;
    return result;
#else
    result = _inp(TMR2);
    return ~result & 0xff;
#endif
}


/* -------------------------------------------------------------------------------- */
/*  Unused functions - for reference only                                           */
/* -------------------------------------------------------------------------------- */
void speaker_on(void)  { _outp(0x61,_inp(0x61) |  0x03); }
void speaker_off(void) { _outp(0x61,_inp(0x61) & ~0x03); }

void
speaker_sound(unsigned frequenz)
{
    unsigned teiler;
    teiler = 1193180L/frequenz;
    _outp( TMR_CNTRL, 0xB6 );
    _outp( TMR2 ,teiler & 0xFF );
    _outp( TMR2 ,teiler >> 8   );
    speaker_on();
    // to stop the sound, call 'speaker_off'
}
