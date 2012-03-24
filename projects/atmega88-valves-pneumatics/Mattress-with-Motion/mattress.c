// ------------------------------------------------
//
//  "Mattress with Motion"
//
//  File    : mattress.c
//  Version : February 2012
//
//  Using the ATmega88PA with ULN2803A drivers.
//  A led is connected between two pins,
//  to be able to use it also as a light sensor.
//  A switch is in the hardware, but not used yet.
//
//  With avr-gcc 4.3.5 in Linux Mint 11 and avrdude 5.10.
//

// The ATmega88PA runs default at 1MHz,
// but can be set to 8MHz with a fuse.
#define F_CPU 1000000UL

#include <avr/io.h>
#include <stdbool.h>
#include <stdlib.h>
#include <util/delay.h>
#include <avr/pgmspace.h>


// To set or clear a bit in a register, I use my own macro's.
#define BIT_SET(port,bit) port |= _BV(bit)
#define BIT_CLEAR(port,bit) port &= ~_BV(bit)
#define BIT_TEST(port,bit) (port & _BV(bit))
#define BIT_INVERT(port,bit) port ^= _BV(bit)

#define TRUE true
#define FALSE false


// Hardware specific definitions
#define SWITCH PB3
#define LED PB4
#define LED_GROUND PC3

// There are 14 valves.
// One output is used to switch the pump on and off.
// So one output is not used and is a spare.
// The wires of the valves are soldered without checking the output number.
// How the valves and air pockets are connected to the outputs is solved by software.

// The next definitions define all the valves and the pump.
// The order is not hardware related.
// The order is only for an index of the valve[][2] array.
enum output_names
{
  VALVE_1 = 0,
  VALVE_2,
  VALVE_3,
  VALVE_4,
  VALVE_5,
  VALVE_6,
  VALVE_7,
  VALVE_8,
  VALVE_9,
  VALVE_10,
  VALVE_OUTLET,
  VALVE_HIGH,
  VALVE_INLET,
  VALVE_LOW,
  PUMP,
  SPARE,
};


// The next definitions define how the air pockets are connected the valves.
// Most of the time, the first pocket is connected to the first valve, and so on.
enum air_pockets
{
  POCKET_1 = VALVE_1,
  POCKET_2 = VALVE_2,
  POCKET_3 = VALVE_3,
  POCKET_4 = VALVE_4,
  POCKET_5 = VALVE_5,
  POCKET_6 = VALVE_6,
  POCKET_7 = VALVE_7,
  POCKET_8 = VALVE_8,
  POCKET_9 = VALVE_9,
  POCKET_10 = VALVE_10,
};


// PORTB can not be used in a constant, because PORTB is a define with calculation.
// So define it with normal values to indicate the port.
enum port_defines
{
  _PORT_A,
  _PORT_B,
  _PORT_C,
  _PORT_D,
};


// Connections of valves.
// The next constant array is located in program code with 'PROGMEM'.
// It translates the definitions for the valves to the hardware connections.
// Both the order and the values should match the hardware.
const unsigned char valve[][2] PROGMEM =
{
  {_PORT_B, PB0},     //  VALVE_1
  {_PORT_B, PB1},     //  VALVE_2
  {_PORT_B, PB2},     //  VALVE_3
  {_PORT_B, PB6},     //  VALVE_4
  {_PORT_B, PB7},     //  VALVE_5
  {_PORT_C, PC0},     //  VALVE_6
  {_PORT_C, PC1},     //  VALVE_7
  {_PORT_C, PC2},     //  VALVE_8
  {_PORT_D, PD0},     //  VALVE_9
  {_PORT_D, PD1},     //  VALVE_10
  {_PORT_D, PD2},     //  VALVE_OUTLET
  {_PORT_D, PD3},     //  VALVE_HIGH
  {_PORT_D, PD4},     //  VALVE_INLET
  {_PORT_D, PD5},     //  VALVE_LOW
  {_PORT_D, PD6},     //  PUMP
  {_PORT_D, PD7},     //  SPARE
};


// defines for valves and pump.
#define OPEN true
#define CLOSE false
#define ON true
#define OFF false


// ###########################################################
// Delay functions
// 
// The library functions _delay_ms and _delay_us
// can only delay for about 20 ... 200.
// The actual maximum depends on the frequency of the chip.
// So a workaround should always be added.
// The slight overhead adds some extra delay.

void delay_ms (uint16_t milliseconds)
{
  while ( milliseconds )
  {
    _delay_ms (1);
    milliseconds--;
  }
}

void delay_s (uint16_t seconds)
{
  while (seconds)
  {
    delay_ms(1000);
    seconds--;
  }
}


// ###########################################################
// Function Valve
//
// This function translates the requested valve to the hardware output.
// A valve is closed (or off) if not activated.
// Turning it 'ON' or 'OPEN' activates the output and activates the valve.
// This function can be used with either VALVE_1, or PUMP, and also with POCKET_1, etc.
// The second parameter can be TRUE, FALSE, ON, OFF, OPEN, CLOSE.

void Valve (uint8_t byteValve, uint8_t byteOpen)
{
  uint8_t portdef, pin;
  volatile uint8_t *pPort=NULL;

  portdef = pgm_read_byte (&valve[byteValve][0]);
  pin     = pgm_read_byte (&valve[byteValve][1]);

  // Translate the definition for the port to the actual io-port.
  // Since 'PORTB' is the actual port in the io-register area,
  // using a pointer seems to be the best way to do this.
  switch (portdef)
  {
  case _PORT_B:
    pPort = &PORTB;
    break;
  case _PORT_C:
    pPort = &PORTC;
    break;
  case _PORT_D:
    pPort = &PORTD;
    break;
  }

  if (pPort != NULL)
  {
    // Set the valve (or pump).
    if (byteOpen)
    {
      BIT_SET (*pPort, pin);
    }
    else
    {
      BIT_CLEAR (*pPort, pin);
    }
  }
}


// ###########################################################
// Function SlowWave_Next
//
// A sequence for the air pockets.
// This sequence is a slow wave.
// A static variable is used to remember the previous air pocket.
// This way some of the pressurized air can be used for the next air pocket.
//
// This function assumes that all valves and the pump are off.

void SlowWave_Next (uint8_t nNewPocket)
{
  static uint8_t nOldPocket = POCKET_1;

  // Let the pressure flow to new air pocket.
  Valve (nOldPocket, OPEN);
  Valve (nNewPocket, OPEN);
  delay_s (10);

  // let first one deflate.
  Valve (nNewPocket, CLOSE);
  Valve (VALVE_HIGH, OPEN);
  Valve (VALVE_OUTLET, OPEN);
  delay_s (10);
  Valve (VALVE_HIGH, CLOSE);
  Valve (VALVE_OUTLET, CLOSE);
  Valve (nOldPocket, CLOSE);

  // Pressurize second one further.
  Valve (VALVE_HIGH, OPEN);
  Valve (VALVE_INLET, OPEN);
  Valve (PUMP, ON);
  Valve (nNewPocket, OPEN);
  delay_s (50);

  // Deflate first one further, using the pump.
  Valve (nNewPocket, CLOSE);
  Valve (VALVE_HIGH, CLOSE);
  Valve (VALVE_INLET, CLOSE);
  Valve (nOldPocket, OPEN);
  Valve (VALVE_LOW, OPEN);
  Valve (VALVE_OUTLET, OPEN);
  delay_s (10);

  // Hold pressure in the second one.
  Valve (PUMP, OFF);
  Valve (VALVE_LOW, CLOSE);
  Valve (VALVE_OUTLET, CLOSE);
  Valve (nOldPocket, CLOSE);
  delay_s (40);

  nOldPocket = nNewPocket;
}


// ###########################################################
// main

int main(void)
{
  uint8_t i;


  // The next code is very hardware specific.
  //    The code could be like this:
  //      PORTB = 0xC7;
  //    Or the definitions of valve[][] could be used,
  //    but that would add extra program code.
  DDRB = 0xC7;                        // Select pins for output
  DDRC = 0x07;                        // Select pins for output
  DDRD = 0xFF;                        // Select pins for output

  // The next code is normal code, for the led and switch.
  BIT_SET (DDRB, LED);                // make it output
  BIT_SET (DDRC, LED_GROUND);         // make it output
  BIT_CLEAR (DDRB, SWITCH);           // make it input.
  BIT_SET (PORTB, SWITCH);            // activate internal pull-up resistor

  BIT_CLEAR (PORTC, LED_GROUND);      // make ground for led a '0'.

  // The best sequence for the air pockets is just a wild guess.
  // At this moment there is only one sequence: a slow wave.
  // This slow wave is done with the first 5 air pockets.

  while (1)
  {
    for (i = POCKET_1; i <= POCKET_5; i++)
    {
      SlowWave_Next (i);
    }
  }
}

