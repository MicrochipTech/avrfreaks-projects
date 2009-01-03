/************************************************************************/
/*                                                                      */
/*                      Debouncing 8 Keys				*/
/*			Sampling 4 Times				*/
/*                                                                      */
/*              Author: Peter Dannegger                                 */
/*                                                                      */
/************************************************************************/

#include <util\atomic.h>		// need "--std=c99"


typedef unsigned char	u8;
typedef signed short	s16;

#define	XTAL		8e6		// 8MHz

#define KEY_PIN		PINB
#define KEY_PORT	PORTB
#define KEY_DDR		DDRB
#define KEY0		0
#define KEY1		1
#define KEY2		2
#define KEY3		3
#define KEY4		4
#define KEY5		5
#define KEY6		6
#define KEY7		7

#define LED_DDR		DDRD
#define LED_PORT	PORTD
#define LED0		0
#define LED1		1
#define LED2		2
#define LED3		3
#define LED4		4
#define LED5		5
#define LED6		6
#define LED7		7


u8 key_state;				// debounced and inverted key state:
					// bit = 1: key pressed
u8 key_press;				// key press detect


ISR( TIMER0_COMPA_vect )		// every 10ms
{
  static u8 ct0 = 0xFF, ct1 = 0xFF;	// 8 * 2bit counters
  u8 i;

  i = ~KEY_PIN;				// read keys (low active)
  i ^= key_state;			// key changed ?
  ct0 = ~( ct0 & i );			// reset or count ct0
  ct1 = ct0 ^ (ct1 & i);		// reset or count ct1
  i &= ct0 & ct1;			// count until roll over ?
  key_state ^= i;			// then toggle debounced state
  key_press |= key_state & i;		// 0->1: key press detect
}


u8 get_key_press( u8 key_mask )
{
  ATOMIC_BLOCK(ATOMIC_FORCEON){		// read and clear atomic !
    key_mask &= key_press;		// read key(s)
    key_press ^= key_mask;		// clear key(s)
  }
  return key_mask;
}


int main( void )
{
  TCCR0A = 1<<WGM01;			// T0 Mode 2: CTC
  TCCR0B = 1<<CS02^1<<CS00;		// divide by 1024
  OCR0A = XTAL / 1024.0 * 10e-3 -1;	// 10ms
  TIMSK0 = 1<<OCIE0A;			// enable T0 interrupt

  KEY_DDR = 0;				// input
  KEY_PORT = 0xFF;			// pullups on
  LED_PORT = 0xFF;			// LEDs off (low active)
  LED_DDR = 0xFF;			// LED output
  key_state = ~KEY_PIN;			// no action on keypress during reset
  sei();

  for(;;){					// main loop
    if( get_key_press( 1<<KEY0 ))
      LED_PORT ^= 1<<LED0;		// toggle LED on keypress

    if( get_key_press( 1<<KEY1 ))
      LED_PORT ^= 1<<LED1;

    if( get_key_press( 1<<KEY2 ))
      LED_PORT ^= 1<<LED2;

    if( get_key_press( 1<<KEY3 ))
      LED_PORT ^= 1<<LED3;

    if( get_key_press( 1<<KEY4 ))
      LED_PORT ^= 1<<LED4;

    if( get_key_press( 1<<KEY5 ))
      LED_PORT ^= 1<<LED5;

    if( get_key_press( 1<<KEY6 ))
      LED_PORT ^= 1<<LED6;

    if( get_key_press( 1<<KEY7 ))
      LED_PORT ^= 1<<LED7;
  }
}
