/************************************************************************/
/*                                                                      */
/*                      Debouncing 8 Keys				*/
/*			Sampling 4 Times				*/
/*                                                                      */
/*              Author: Peter Dannegger                                 */
/*                                                                      */
/************************************************************************/
#include "getkey.h"
#include "main.h"


uint8_t key_state;			// debounced and inverted key state:
					// bit = 1: key pressed
uint8_t key_press;			// key press detect


ISR( TIMER0_COMPA_vect )		// every 10ms
{
  static uint8_t ct0 = 0xFF, ct1 = 0xFF;	// 8 * 2bit counters
  uint8_t i;

  i = ~KEY_PIN;				// read keys (low active)
  i ^= key_state;			// key changed ?
  ct0 = ~( ct0 & i );			// reset or count ct0
  ct1 = ct0 ^ (ct1 & i);		// reset or count ct1
  i &= ct0 & ct1;			// count until roll over ?
  key_state ^= i;			// then toggle debounced state
  key_press |= key_state & i;		// 0->1: key press detect
}


uint8_t get_key_press( uint8_t key_mask )
{
  ATOMIC_BLOCK(ATOMIC_FORCEON){		// read and clear atomic !
    key_mask &= key_press;		// read key(s)
    key_press ^= key_mask;		// clear key(s)
  }
  return key_mask;
}


uint8_t get_key_idle( uint8_t key_mask ) 	// check if ready for power down
{
  return (KEY_PIN ^ key_state) & key_mask;
}


void get_key_init( void )
{
  TCCR0A = 1<<WGM01;				// T0 Mode 2: CTC
  TCCR0B = 1<<CS02;				// divide by 256
  OCR0A = (uint16_t)(XTAL / 256.0 * 10e-3 - 1);	// 10ms
  TIMSK0 |= 1<<OCIE0A;				// enable T0 interrupt

  key_state = ~KEY_PIN;			// no action on keypress during reset
}
