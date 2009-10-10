////////////////////////////////////////////////////////////////////////////////////////////
//
// Real time clock interface
//
//      CE   -> PB2  
//      IO   -> PB1  
//      SCLK -> PB0  
//
// The DS1302 uses three control signals. All data transfers occur on the
// edge of the sclk line; either data is read from or written to the io line.
// Data transfers only occur when the ce line is high.
//
// The procedure is:
// raise the ce line
// send eight bits which define the address and read/write plus enable:
//
// [rw][a0][a1][a2][a3][a4][rc][ 1]
//
// then either send or receive the data itself
//
// [d0][d1][d2][d3][d4][d5][d6][d7]
//
// when r/w = 1, the device sends data to the controller which should be
// sampled on the *falling* edge of sclk
//
// when r/w = 0, the device reads data from the controller which is sampled
// on the *rising* edge of sclk
//
// after the address and data byte have been sent/received, ce is dropped again.
//
// burst mode is available but is not used in this project.
//
////////////////////////////////////////////////////////////////////////////////////////////

//#ifndef _RTC_
//#define _RTC_

#include "rtc.h"

#define CE PB2
#define IO PB1
#define SCLK PB0


void rtc_init(void)
{
   // initialise the rtc interface
   // set CE and SCLK as output and set them low
   // set IO input high impedance no pullup

   DDRB |= _BV(CE);
   DDRB |= _BV(SCLK);         // make CE and SCLK as outputs
   DDRB &= ~(_BV(IO));         // and IO as input

   PORTB &= ~(_BV(CE));
   PORTB &= ~(_BV(SCLK));   // make CE and SCLK low to start

}
 

void rtc_comms(unsigned char *data, char address, char rw)
{
   // read or write a single byte at address
   // if rw = 0, write the contents of data
   // if rw = 1, read the contents of address to data
   // the ram/~ck control bit is passed as the high bit of the address

	unsigned char control;
	int q;

   // set up the control byte
   control = (address<<1) + 0x80 + rw;

   // set CE high to mark start of control
   PORTB |= _BV(CE);

   // now spit out the control byte   
   // ce and sclk are low when we get here
   // but we need to set the direction for io to output
   DDRB |= _BV(IO);

   for (q=0; q<8; q++)
   {
      // set the data bit
      if (control & 1)
      {
         PORTB |= _BV(IO);
      }
      else
      {
         PORTB &= ~(_BV(IO));
      }

      // drive the clock high to latch the bit
      PORTB |= _BV(SCLK);

      // shift the data so the next higher bit is sent next time
      control >>= 1;

      // hopefully C codes this slowly enough to meet the minimum clock period
      // of a microsecond, even though we're working at 8MHz
      // and having latched, send it low again

      PORTB &= ~(_BV(SCLK));
	  }

   // now we are different for reads and writes
   if (rw == READ)
   {
      // read a byte
      
      // set the data direction to input
      DDRB &= ~(_BV(IO));
      
      // clear the receiving byte
      control = 0;

      for (q=0; q<8; q++)
      {
         // as we finish the control byte cycle, the clock is low
         // which means that the data is available to be read
         // we get data available on the falling edge of the clock

         // shift the data to make space
         control >>= 1;

         if (PINB & _BV(IO))
         {
            control |= 0x80;
         }
         else
         {
            control &= 0x7f;
         }

         // drive the clock high
         PORTB |= _BV(SCLK);

        // and send it low again
         PORTB &= ~(_BV(SCLK));
      }
      
      // and now we have the data, put it where we need it
      *data = control;

   }
   else
   {
      // write a byte
      
      // we continue to write on the rising edge
      control = *data;
      for (q=0; q<8; q++)
      {
         // set the data bit
         if (control & 1)
         {
            PORTB |= _BV(IO);
         }
         else
         {
            PORTB &= ~(_BV(IO));
         }

         // drive the clock high to latch the bit
         PORTB |= _BV(SCLK);

         // shift the data so the next higher bit is sent next time
         control >>= 1;

         // hopefully C codes this slowly enough to meet the minimum clock period
         // of a microsecond, even though we're working at 8MHz
         // and having latched, send it low again
         PORTB &= ~(_BV(SCLK));
      }
   }

   // and one last thing... bring CE low again to mark completion
   // and make the io line high impedance input
   PORTB &= ~(_BV(CE));
   DDRB &= ~(_BV(IO));
   PORTB &= ~(_BV(IO));

}

void rtc_get_time(struct rtc_time *time)
{
// get the time
unsigned char data;

      rtc_comms(&data, 0x02, READ);
      time->hour = data;

      rtc_comms(&data, 0x01, READ);
      time->minute = data;

      rtc_comms(&data, 0x00, READ);
      time->second = data;
      
      rtc_comms(&data, 0x03, READ);
      time->day = data;
      
      rtc_comms(&data, 0x04, READ);
      time->month = data;
      
	  rtc_comms(&data, 0x05, READ);
      time->weekDay = data;

      rtc_comms(&data, 0x06, READ);
      time->year = data;
}

//#endif 
