#include <stdlib.h>
#include <string.h>

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "TIMER.h"
#include "LCD.h"
#include "IRremote.h"

enum IRstatus
{
  IRidle    = 0,
  IRstarted = 1
};


uint8_t  IRmode     = IRidle;
uint32_t IRtime0    = 0x00;
uint32_t IRtime1    = 0x00;
uint32_t IRinput    = 0x00;
uint32_t IRbit;
uint32_t IRreceived = 0;

// IGR codes for Digits
uint32_t IRdigits[] PROGMEM = { LG_0, LG_1, LG_2, LG_3, LG_4, LG_5, LG_6, LG_7, LG_8, LG_9 };


ISR( PCINT1_vect )
{
  uint32_t duree;
  if( ( PINC & _BV(PINC3) ) != 0 ) 
  {
    // PINC3 High
    IRtime1 = micros;
    duree = IRtime1 - IRtime0;
    if( !( IRmode == IRstarted ) )
    {
      if( duree > 5000 || duree < 3000 )
      {
        // Bad signal - too long
        IRmode = IRidle;
      }
      else
      {
        // Start pulse received
        IRmode = IRstarted;
        IRinput = 0x00;
        IRbit = 0x80000000;
      }
    }
  }
  else
  {
    // PINC3 Low
    IRtime0 = micros;
    duree = IRtime0 - IRtime1;
    if( duree > 5000 )
    {
      // Bad signal - too long
      IRmode = IRidle;
    }
    else if( IRmode == IRstarted )
    {
      // Received 1 if duree > 1000 else Received 0
      if( duree > 1000 ) IRinput |= IRbit;
      IRbit >>= 1;
      if( IRbit == 0 && IRreceived == 0 )
      {
        // Received 32 bits
        IRreceived = IRinput;
        IRmode = IRidle;
      }
    }
  }
}

void IRinit(void)
{
  // Pull up Pin C3
  PORTC |= _BV(PINC3);
  // PCINT8  = PINC0
  // PCINT9  = PINC1
  // PCINT10 = PINC2
  // PCINT11 = PINC3
  // Pin Change Mask Register 0 : PCINT0..7 / 1 : PCINT8..14 / 2 : PCINT16..23
  PCMSK1 = _BV(PCINT11);
  // Pin Change Interrupt Control Register
  PCICR = _BV(PCIE1);
  // SET INTERRUPTS
  sei();
}

uint32_t IRcode(void)
{
  uint32_t v = IRreceived;
  IRreceived = 0;
  return v;
}
