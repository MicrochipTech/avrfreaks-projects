/*
Colin O'Flynn  coflynn@newae.com  c_oflynn@yahoo.com c_oflynn on AVRFreaks.net
My website is http://www.newae.com

This file revision: 2003.03.15

These routines are FREE and in public domain. If you use them keep this header
the same. If you expand on these routines add your name to the header as well,
but keep the name(s) of previous people who added thier name the same.

These routines have no warrenties at all, not even that they will work like
described.

VFD (Vacuum Flourscent Display) Driver for 162SD03C Made by Futaba Corporation
Hopefully you get the 162SD03C Datasheet along with this, but if you didn't
the connections are:
1-VCC
2-CLOCK
3-GND
4-DATA
5-RESET (active high)

VCC is 5 volts, 600ma MAX (450ma typ) current draw. There could be a current
spike at power-on as high as 5 amps for 1ms, but that depends on the
characteristics of the power supply.

WARNING: PARTS OF THE DISPLAY ARE CHARGED TO HIGH VOLTAGES (50 to 60 volts typ)
SO TAKE CAUTION. (C16 and C17 have around 50 volts measure across them)

These routines basically define the simplist routines that you need, but its
probably all you will need. The VFD module supports other commands such as
-flash specified characters
-different font
-custom characters
*/

//the one AVR-GCC specific line
#include <avr/io.h>

/*
Setup ports, reset VFD
*/
void												VFD_162S_init
	(
   void
   );

/*
The routine one normally calls when passing data to the display. Legal arguments:
>>Normal characters such as VFD_162S_putchar('A');
>>Commands such as VFD_162S_putchar(VFD_162S_CLEAR);
>>Newline character VFD_162S_putchar('\n');

When a newline is encountered, the display cursor is set to the bottom line
furthest left (even if it is currently on that line).
*/
void												VFD_162S_putbyte
	(
   unsigned char				data
   );

/* Move the cursor to the specified location. For example to move it to the
5th character from the left on the second line:

VFD_162S_locatecursor(2,5);
*/
void												VFD_162S_locatecursor
	(
   unsigned char				line,
   unsigned char				character
   );

/*
Sends one byte directly to the VFD, no error checking (out of range)
is performed. This byte can be anything, be a command (see datasheet) or text
to display.
*/
void												VFD_162S_putchar
	(
   char				data
   );
