/*
 * port.c
 *
 *  Created on: Jan 14, 2012
 *      Author: Alex Costache
 */
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <port.h>

//Local data at RAM
static uint8_t ucMainCounterDisplay = 0;
volatile tDisplayControl  DisplayControlInfo;

//Global functions implementation
void gvPortInit(void)
{
   /*Set bits for display as outputs*/
   DDRC = (1<<DisplaySegA) | (1<<DisplaySegB) | (1<<DisplaySegC)| (1<<DisplaySegD) | (1<<DisplaySegE) | (1<<DisplaySegF) | (1<<DisplaySegG) | (1<<DisplaySegDP);
   /*Set cathodes as outputs*/
   DDRD = (1<<Display0GND) | (1<<Display1GND) | (1<<Display2GND) | (1<<PowerHeater);
   /*Turn off all three displays*/
   PORTD &= ~((1<<Display0GND) | (1<<Display1GND) | (1<<Display2GND) | (1<<PowerHeater));

   /* Encoder inputs are hardcoded in the decoder routine. If other pins are required adapt the decoder routine */
   DDRA  &= ~((1<<EncoderA) | (1<<EncoderB));  /* Encoder inputs: PA4,PA5 */
   DDRA  |= (1<<Spare) ;                       /* Spare pin as output */
   PORTA |= (1<<EncoderA) | (1<<EncoderB);     /* Activate pull up needed by encoder's mechanical switches */

//Alex
   /* For the two buttons, the port pins are initialized as inputs and pull up is activated */
   PORTD |=((1<<UpperButton) | (1<<LowerButton));

//end Alex
}

void gvDisplayInit(void)
{
   DisplayControlInfo.uiDisplayDigit0 = 10;
   DisplayControlInfo.uiDisplayDigit1 = 10;
   DisplayControlInfo.uiDisplayDigit2 = 10;
   DisplayControlInfo.bDisplayDigit0dot   = false;
   DisplayControlInfo.bDisplayDigit1dot   = false;
   DisplayControlInfo.bDisplayDigit2dot   = false;
}

void gvDisplayWriteDigit(uint8_t digit, uint8_t value, bool dot)
{
  switch (digit)
  {
    case 0:
    {
      PORTD &= ~((1 << Display1GND) | (1 << Display2GND));
      PORTC = gu8DisplayBcdTo7Segment(value, dot);
      PORTD |= (1 << Display0GND);
      break;
    }
    case 1:
    {
      PORTD &= ~((1 << Display0GND) | (1 << Display2GND));
      PORTC = gu8DisplayBcdTo7Segment(value, dot);
      PORTD |= (1 << Display1GND);
      break;
    }
    case 2:
    {
      PORTD &= ~((1 << Display0GND) | (1 << Display1GND));
      PORTC = gu8DisplayBcdTo7Segment(value, dot);
      PORTD |= (1 << Display2GND);
      break;
    }
    default:
    {
      break;
    }
  }
}

/*
         DisplaySegA  PC0
         DisplaySegB  PC7
         DisplaySegC  PC4
         DisplaySegD  PC2
         DisplaySegE  PC1
         DisplaySegF  PC5
         DisplaySegG  PC6
         DisplaySegDP PC3

              a
             ___
          f |   | b
            |_g_|
          e |   | c
            |___|.
              d   dp

   */
   /*Array index     |  0   |  1   |  2   |  3   |  4   |  5   |  6   |  7   |  8   |  9   | NULL |   E  |   r  |   L  |   o
    *No. to convert  |  0   |  1   |  2   |  3   |  4   |  5   |  6   |  7   |  8   |  9   |  10  |  11  |  12  |  13  |  14
    *Converted value | 0x3F | 0x06 | 0x5B | 0x4F | 0x66 | 0x6D | 0x7D | 0x07 | 0x7F | 0x67 | 0x00 | 0x79 | 0x50 | 0x38 | 0x5C
    *PortC value     | 0xB7 | 0x90 | 0xC7 | 0xD5 | 0xF0 | 0x75 | 0x77 | 0x91 | 0xF7 | 0xF5 | 0x00 | 0x67 | 0x42 | 0x26 | 0x56 */
uint8_t gu8DisplayBcdTo7Segment(uint8_t value, bool dot)
{
   const uint8_t uc7segmequ[15] = {0xB7 , 0x90 , 0xC7 , 0xD5 , 0xF0 , 0x75 , 0x77 , 0x91 , 0xF7 , 0xF5 , 0x00, 0x67, 0x42, 0x26, 0x56};
   uint8_t temp;

   temp = uc7segmequ[value];

   if (dot != false)
   {
     temp |= (1<<DisplaySegDP);
   }
   return (temp);
}

void gvDisplayRefreshFct(void)
{
   switch (ucMainCounterDisplay)
   {
      case 0:
      {
         gvDisplayWriteDigit(0,DisplayControlInfo.uiDisplayDigit0, DisplayControlInfo.bDisplayDigit0dot);
         break;
      }
      case 1:
      {
         gvDisplayWriteDigit(1,DisplayControlInfo.uiDisplayDigit1, DisplayControlInfo.bDisplayDigit1dot);
         break;
      }
      case 2:
      {
         gvDisplayWriteDigit(2,DisplayControlInfo.uiDisplayDigit2, DisplayControlInfo.bDisplayDigit2dot);
         break;
      }
      default:
      {
         break;
      }
   }
   ucMainCounterDisplay++;

   if (ucMainCounterDisplay > 2)
   {
     ucMainCounterDisplay = 0;
   }
}

void gvDisplaySetNewValue(uint16_t value, bool digitdot0, bool digitdot1, bool digitdot2)
{
   uint16_t tempvalue;

   /*convert and display values only if they are in interval 0 .. 999 - maximum number that can be written on
   the 3 digit display*/
   if (value < 1000)
   {
      /*update the digits*/
      tempvalue = value/100;
      if (tempvalue > 0)
      {
         DisplayControlInfo.uiDisplayDigit0 = tempvalue;
      }
      else
      {
         DisplayControlInfo.uiDisplayDigit0 = 10;
      }
      value = value%100;
      tempvalue = value/10;
      if (tempvalue > 0)
      {
         DisplayControlInfo.uiDisplayDigit1 = tempvalue;
      }
      else
      {
         DisplayControlInfo.uiDisplayDigit1 = 10;
      }
      value = value%10;
      DisplayControlInfo.uiDisplayDigit2 = value;
      /*update the dots for each digit*/
      DisplayControlInfo.bDisplayDigit0dot = digitdot0;
      DisplayControlInfo.bDisplayDigit1dot = digitdot1;
      DisplayControlInfo.bDisplayDigit2dot = digitdot2;
   }
   else
   {
      /*do nothing*/
   }
}

// returns number of digits converted
// result array must be large enough to hold
// the number of digits converted.

void itobcd(unsigned int val, bool digitdot0, bool digitdot1, bool digitdot2)
{
  char BCD_text[4]; // max 4 digits

  /* Input (Celsius degree) cannot be higher than 512 */
  //val &= 0x01FF;


  utoa(val, BCD_text, 10);

  if (val >= 100)
  {
    DisplayControlInfo.uiDisplayDigit0 = BCD_text[0] - '0';
    DisplayControlInfo.uiDisplayDigit1 = BCD_text[1] - '0';
    DisplayControlInfo.uiDisplayDigit2 = BCD_text[2] - '0';
  }
  else if (val >= 10)
  {
    DisplayControlInfo.uiDisplayDigit0 = 10;
    DisplayControlInfo.uiDisplayDigit1 = BCD_text[0] - '0';
    DisplayControlInfo.uiDisplayDigit2 = BCD_text[1] - '0';
  }
  else if (val >= 0)
  {
    DisplayControlInfo.uiDisplayDigit0 = 10;
    DisplayControlInfo.uiDisplayDigit1 = 10;
    DisplayControlInfo.uiDisplayDigit2 = BCD_text[0] - '0';
  }

  DisplayControlInfo.bDisplayDigit0dot = digitdot0;
  DisplayControlInfo.bDisplayDigit1dot = digitdot1;
  DisplayControlInfo.bDisplayDigit2dot = digitdot2;

}

//Alex
void gvWriteErrorDisplay()
{
   DisplayControlInfo.uiDisplayDigit0 = 11; /*index for letter E*/
   DisplayControlInfo.uiDisplayDigit1 = 12; /*index for letter r*/
   DisplayControlInfo.uiDisplayDigit2 = 12; /*index for letter r*/
   DisplayControlInfo.bDisplayDigit0dot = false;
   DisplayControlInfo.bDisplayDigit1dot = false;
   DisplayControlInfo.bDisplayDigit2dot = false;
}

void gvWriteLowDisplay()
{
   DisplayControlInfo.uiDisplayDigit0 = 10; /*index for Null*/
   DisplayControlInfo.uiDisplayDigit1 = 13; /*index for letter L*/
   DisplayControlInfo.uiDisplayDigit2 = 14; /*index for letter o*/
   DisplayControlInfo.bDisplayDigit0dot = false;
   DisplayControlInfo.bDisplayDigit1dot = false;
   DisplayControlInfo.bDisplayDigit2dot = false;

}

uint8_t  guiPortReadPin(uint8_t port, uint8_t pin)
{
   uint8_t rtnval;

   if ((port & _BV(pin)) != 0)
   {
      rtnval = true;
   }
   else
   {
      rtnval = false;
   }

   return rtnval;
}

bool  gvReadUpperButtonPressed(void)
{
   bool rtnvalue = false;
   static uint8_t     uibtnupvalue;
   static uint8_t     uibtnupvalueold = 1;

   uibtnupvalue = guiPortReadPin(PIND, UpperButton);
   if ((uibtnupvalue == 0) & (uibtnupvalueold == 1))
   {
      uibtnupvalueold = uibtnupvalue;
      rtnvalue = true;
   }
   else
   {
      if ((uibtnupvalue == 1) & (uibtnupvalueold == 0))
      {
         uibtnupvalueold = 1;
      }
      else
      {
         //do nothing
      }
      rtnvalue = false;
   }

   return rtnvalue;
}

bool  gvReadLowerButtonPressed(void)
{
   bool rtnvalue = false;
   static uint8_t     uibtnlowvalue;
   static uint8_t     uibtnlowvalueold = 1;

   uibtnlowvalue = guiPortReadPin(PIND, LowerButton);
   if ((uibtnlowvalue == 0) & (uibtnlowvalueold == 1))
   {
      uibtnlowvalueold = uibtnlowvalue;
      rtnvalue = true;
   }
   else
   {
      if ((uibtnlowvalue == 1) & (uibtnlowvalueold == 0))
      {
         uibtnlowvalueold = 1;
      }
      else
      {
         //do nothing
      }
      rtnvalue = false;
   }

   return rtnvalue;
}

//end Alex
