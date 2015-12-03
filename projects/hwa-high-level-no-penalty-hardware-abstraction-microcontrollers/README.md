# HWA : high-level, no penalty hardware abstraction for microcontrollers

Uploaded by duparq on 2015-12-02 18:03:00 (rating 0 out of 5)

## Summary

HWA is a hardware abstraction tool for programming microcontrollers in C language. A few features of HWA are useful for programming in assembler too.


 


* HWA provides the programmer with an object-oriented interface to the hardware that consists of a small set of generic instructions designed to be applied on various types of objects that represent the peripheral controllers embedded in a microcontroller.
* Most of these instructions accept a variable-length list of arguments, starting with the name of a hardware object, followed by key-value pairs.
* Using these instructions makes the source code easier to read and to port between different targets since they describe clearly and concisely their effect, instead of manipulating numerical values and registers.
* HWA is implemented only using standard C language ([C11](https://en.wikipedia.org/wiki/C11_%28C_standard_revision%29)) macro definitions and function-like macros.
* The binary code produced is free of any penalty, either in terms of size, execution speed or memory used (better than Arduino!).
* HWA can provide software-emulated controllers (two software UARTs are available for Atmel AVRs) and objects that give access to external hardware.
* HWA produces compile-time error messages that try to guide the developer quickly to the solution.

 


HWA currently has support for ATtiny24/44/84, ATtiny25/45/85, ATmega88/168/328. Support of Espressif's ESP8266 has just begun (GPIOs and UARTs).


 


HWA is free software and is hosted on Github : <https://github.com/duparq/hwa>


 


 


**A small example : blinking a LED using timer interrupts, device in sleep mode between interrupts.**


 



```

//  Target device
//
#include <hwa/attiny85\_20pu.h>

//  The pin at which the LED is connected
//
#define PIN\_LED                 hw\_pin\_5

//  The blinking period (in seconds)
//
#define PERIOD                  0.5

/*  The counter and its clock prescaling factor
 */
#define COUNTER                 hw\_counter0

#define CLKDIV                  64

/*  Service the counter overflow IRQ
 */
HW\_ISR( COUNTER, overflow )
{
  static uint8\_t n ;
  n++ ;
  if ( n >= (uint8\_t)(0.5 + PERIOD / 0.001 / 2) ) {
    n = 0 ;
    hw\_toggle( PIN\_LED );
  }
}

int main ( )
{
  /*  Create a HWA context to collect the hardware configuration.
   *  Preload this context with RESET values.
   */
  hwa\_begin\_from\_reset();

  /*  Configure the LED pin.
   */
  hwa\_config( PIN\_LED, direction, output );

  /*  Have the CPU enter idle mode when the 'sleep' instruction is executed.
   */
  hwa\_config( hw\_core0,
              sleep,      enabled,
              sleep\_mode, idle );

  /*  Configure the counter to overflow every 0.001 s.
   *
   *  The compare unit `compare0` of the counter (OCRxA) is used to store the top value.
   *  Unless otherwise stated, the overflow will be automatically set to occur
   *  when the 'count' value equals the 'top' value in 'loop\_up' counting mode.
   */
  hwa\_config( COUNTER,
              clock,     prescaler\_output(CLKDIV),
              countmode, loop\_up,
              bottom,    0,
              top,       compare0
              );

  /*  Set the TOP value of the counter.
   */
  hwa\_write( hw\_rel(COUNTER, compare0), 0.001 * hw\_syshz / CLKDIV );

  /*  Enable overflow IRQs.
   */
  hwa\_turn\_irq( COUNTER, overflow, on );

  /*  Write this configuration into the hardware.
   */
  hwa\_commit();

  hw\_enable\_interrupts();

  /*  Keep the device in sleep mode between interrupts.
   */
  for(;;)
    hw\_sleep();
}
```

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)
- #attiny #atmega
- #hardware #abstraction #library #arduino
