#include    <avr/io.h>
#include    "Ports.h"

/*******************************************************************/
// Function name: IoBitSet
// Task: Sets an IO bit (bit = 1 or high)
// Input: bit name from defined list
// Return:
/*******************************************************************/
extern void IoBitSet (pins bit)
{
   switch (bit) 
   {
      case pinA: 
         PORTA |= 0x04; 
         break; 
      case pinB: 
         PORTB |= 0x08; 
         break; 
      case pinC: 
         PORTC |= 0x10; 
         break; 
      case pinD: 
         PORTD |= 0x80; 
         break; 
   }
}

/*******************************************************************/
// Function name: IoBitClr
// Task: clears an IO bit (bit = 0 or low)
// Input: bit name from defined list
// Return:
/*******************************************************************/
extern void IoBitClr (pins bit)
{
   switch (bit) 
   {
      case pinA: 
         PORTA &= ~(0x04); 
         break; 
      case pinB: 
         PORTB &= ~(0x08); 
         break; 
      case pinC: 
         PORTC &= ~(0x10); 
         break; 
      case pinD: 
         PORTD &= ~(0x80); 
         break; 
   }
}

/*******************************************************************/
// Function name: IoInit
// Task: Set the four ports to correct input or output
// Input: 
// Return:
/*******************************************************************/
extern void IoInit (void)
{
   DDRA = 0xFC;
   PORTA = 0xFF;
   DDRB = 0xFF;
   PORTB = 0xFF;
   DDRC = 0xFF;
   PORTC = 0x00;
   DDRD = 0xFF;                     // all digital pins are output, low
   PORTD = 0x00;
}
