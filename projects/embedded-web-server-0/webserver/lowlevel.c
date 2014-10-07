#include <avr/io.h>


/******************************************************************************/
extern void ExternIntInit (void)
{
   DDRD &= ~(0x04);     // INT 0 = PD2 = input with pull up
   PORTD |= 0x04;
   MCUCR &= 0xFE;       // INT 0 sense on falling edge
   MCUCR |= 0x02;
   GICR |= 0x40;        // enable interrupt on INT 0
}


/******************************************************************************/
extern void Timer1Init (void)
{
   TCNT1 = 0;                       // count from 0
   OCR1A = 19999;                   // divide 16 MHz with 8 * 20000 to 100 Hz
   TIMSK = TIMSK | 0x10;            // enable intr on OCR1A
   TCCR1A = 0x00;                   // mode = 4, 
   TCCR1B = 0x0A;                   // prescaler = 8, start running
}


//*****************************************************************************
extern void InitIo (void)
{
   DDRA = 0xFF;         // all output high
   DDRB = 0xFF;
   DDRC = 0xFF;
   DDRD = 0xFB;
   PORTA = 0xFF;
   PORTB = 0xFF;
   PORTC = 0xFF;
   PORTD = 0xFF;
}


//*****************************************************************************
extern void DelayUs (volatile uint8_t us)
{
   volatile char dummy = 0;

   while (us) 
   {
      us--;                // OK within 10 %
      dummy++;
   }
}


//*****************************************************************************
extern void DelayMs (volatile uint16_t ms)
{
   volatile int i;

   while (ms > 0)                      // count the number of ms
   {
      for (i = 0; i < 857; i++);       // found by test, = 1 ms
      ms--;
   }
}
