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

#include "VFD_162S.h"

//where the display is located, PORT and DDR
#define VFD_162S_PORT		PORTD
#define VFD_162S_DDR			DDRD
//what pins on the VFD connect to what AVR bits of the above PORT
#define VFD_162S_CLK_PIN	5 	//CLOCK pin
#define VFD_162S_DAT_PIN	6 	//DATA pin
#define VFD_162S_RST_PIN	7  //RESET pin

//VFD commands
//to send just use VFD_162S_putchar(COMMAND);
#define VFD_162S_CLEAR				0x01
#define VFD_162S_BRIGHTNESS_UP   0x02
#define VFD_162S_BRIGHTNESS_DOWN 0x03

/*
Setup ports, reset VFD
*/
void												VFD_162S_init
	(
   void
   )
   {
   volatile unsigned char	counter;

   //set pins to output
   VFD_162S_DDR |= 1<<VFD_162S_CLK_PIN | 1<<VFD_162S_DAT_PIN | 1<<VFD_162S_RST_PIN;

   //set two of them (CLK and DATA) high (default value)
   VFD_162S_PORT |= 1<<VFD_162S_CLK_PIN | 1<<VFD_162S_DAT_PIN;

   //reset the VFD module
   VFD_162S_PORT |= 1<<VFD_162S_RST_PIN;

   //wait a few cycles (number not really important, so this delay is done in C)
   counter = 0;
   while(counter != 200)
   	{
      counter++;
      }

   //take reset low
   VFD_162S_PORT &= ~(1<<VFD_162S_RST_PIN);

   //wait a while before writing new data to VFD (_2ms_ so adjust delays
   //for your processor speed (these seemed to work on 1 MHz) MIN)
   counter = 0;
   while(counter != 0xFF)
   	{
      counter++;
      }

   counter = 0;
   while(counter != 0xFF)
   	{
      counter++;
      }

   counter = 0;
   while(counter != 0xFF)
   	{
      counter++;
      }


   return;
   }


/*
The routine one normally calls when passing data to the display. Legal arguments:
>>Normal characters such as VFD_162S_putchar('A');
>>Commands such as VFD_162S_putchar(VFD_162S_CLEAR);
>>Newline character VFD_162S_putchar('\n');

When a newline is encountered, the display cursor is set to the bottom line
furthest left (even if it is currently on that line).
*/
void												VFD_162S_putchar
	(
   char							data
   )
   {
   static unsigned char		brightness = 0xFF;

   //check data is within printable characters
   if (data >= 0x20 && data <= 0xF3)
   	{
      VFD_162S_putbyte((unsigned char)data);
      }

   else if (data == '\n')
   	{
      //set cursor to bottem left
      VFD_162S_putbyte(0x02);
      VFD_162S_putbyte(0x11);
      }

   else if (data == VFD_162S_CLEAR)
   	{
      //clear display, reset cursor to default location
      VFD_162S_putbyte(0x01);
      }

   else if (data == VFD_162S_BRIGHTNESS_UP)
   	{
      //brightness up
      if (brightness != 0xFF)
      	{
         brightness = brightness + 10;
         }

      //send brightness out
      VFD_162S_putbyte(0x04);
      VFD_162S_putbyte(brightness);
      }

	else if (data == VFD_162S_BRIGHTNESS_DOWN)
   	{
      //brightness down
      if (brightness != 0x01)
      	{
         brightness = brightness - 10;
         }
      //send brightness out
      VFD_162S_putbyte(0x04);
      VFD_162S_putbyte(brightness);
      }


   }

/* Move the cursor to the specified location. For example to move it to the
5th character from the left on the second line:

VFD_162S_locatecursor(2,5);
*/

void												VFD_162S_locatecursor
	(
   unsigned char				line,
   unsigned char				character
   )
   {
   //need to convert line + char into an absolute character of 1-32
   unsigned char				final_location;

   if (line == 1)	//first line
   	{
      final_location = character;
      }
   else 				//second line
   	{
      final_location = character + 16;		//16 being characters on top line
      }

   //check data is valid
   if ((final_location < 0x01) || (final_location > 0x20))
   	{
      return;
      }

   //send it out
   VFD_162S_putbyte(0x02);
   VFD_162S_putbyte(final_location);

   return;
   }


/*
Sends one byte directly to the VFD, no error checking (out of range)
is performed. This byte can be anything, be a command (see datasheet) or text
to display.
*/
void												VFD_162S_putbyte
	(
   unsigned char				data
   )
   {
   unsigned char				counter;
   volatile unsigned char	counter2;

   counter = 0;
   //output 8 bits
   while(counter != 8)
   	{
   	//put CLK high
   	VFD_162S_PORT |= 1<<VFD_162S_CLK_PIN;

      //delay (make longer if processor is faster than 1 MHz)
      counter2 = 0;
      while(counter2 != 0xFF)
      	{
         counter2++;
         }

      //output highest bit of 'data' register
      if ((data & 0x80) == 0x00)
      	{
         //MSB is low
         VFD_162S_PORT &= ~(1<<VFD_162S_DAT_PIN);
         }
      else
      	{
         //MSB is high
         VFD_162S_PORT |= 1<<VFD_162S_DAT_PIN;
         }

      //put CLK low
      VFD_162S_PORT &= ~(1<<VFD_162S_CLK_PIN);

      //delay (make longer if processor is faster than 1 MHz)
      counter2 = 0;
      while(counter2 != 0xFF)
      	{
         counter2++;
         }

      //shift data left
      data = data<<1;

      counter++;
      }

   //make sure CLK goes high again for display to process data
   VFD_162S_PORT |= 1<<VFD_162S_CLK_PIN;

      //delay (make longer if processor is faster than 1 MHz)
   counter2 = 0;
   while(counter2 != 0xFF)
     	{
      counter2++;
      }

   return;
   }

