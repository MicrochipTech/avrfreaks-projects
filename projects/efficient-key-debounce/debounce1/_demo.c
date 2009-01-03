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
#define	KEY1		1

#define LED_DDR		DDRD
#define LED_PORT	PORTD
#define LED0		0
#define	LED1		1
#define LED3		3
#define LED4		4
#define LED5		5
#define LED6		6
#define LED7		7


u8 key_state;				// debounced and inverted key state:
					// bit = 1: key pressed
u8 key_press;				// key press detect

u8 ct0 = 0xFF, ct1 = 0xFF;		// internal debouncing states


ISR( TIMER1_COMPA_vect )
{
  u8 i;

  i = key_state ^ ~KEY_PIN;		// key changed ?
  ct0 = ~( ct0 & i );			// reset or count ct0
  ct1 = ct0 ^ (ct1 & i);		// reset or count ct1
  i &= ct0 & ct1;			// count until roll over ?
  key_state ^= i;			// then toggle debounced state
  key_press |= key_state & i;		// 0->1: key press detect
}


u8 get_key_press( u8 key_mask )
{
  ATOMIC_BLOCK(ATOMIC_FORCEON){
    key_mask &= key_press;		// read key(s)
    key_press ^= key_mask;		// clear key(s)
  }
  return key_mask;
}


void display_debounce_key0( void )	// demonstrate debounce working
{
  u8 i;

  ATOMIC_BLOCK(ATOMIC_FORCEON){
    i = ct0 & 1<<KEY0;			// collect the two bits for KEY0
    if( ct1 & 1<<KEY0 )			// from vertical counter
      i |= 2;
  }
  switch( i ){				// LED7..4 = debounce counter for KEY0
    case 0:
	      LED_PORT &= ~(1<<LED4);
	      LED_PORT |= 1<<LED5;
	      LED_PORT |= 1<<LED6;
	      LED_PORT |= 1<<LED7;
	      break;
    case 1:
	      LED_PORT |= 1<<LED4;
	      LED_PORT &= ~(1<<LED5);
	      LED_PORT |= 1<<LED6;
	      LED_PORT |= 1<<LED7;
	      break;
    case 2:
	      LED_PORT |= 1<<LED4;
	      LED_PORT |= 1<<LED5;
	      LED_PORT &= ~(1<<LED6);
	      LED_PORT |= 1<<LED7;
	      break;
    case 3:
	      LED_PORT |= 1<<LED4;
	      LED_PORT |= 1<<LED5;
	      LED_PORT |= 1<<LED6;
	      LED_PORT &= ~(1<<LED7);
	      break;
  }
  if( key_state & 1<<KEY0 )		// LED3 = debounced state
    LED_PORT &= ~(1<<LED3);
  else
    LED_PORT |= 1<<LED3;
}


int main( void )
{
  TCCR1A = 0;
  TCCR1B = 1<<WGM12			// Mode 4: CTC
	 ^ 1<<CS02^1<<CS00;		// divide by 1024
  OCR1A = XTAL / 1024.0 * 1 - 1;	// 1s
  TCCR1C = 0;
  TIMSK1 = 1<<OCIE1A;			// enable T1 interrupt

  KEY_DDR = 0;				// input
  KEY_PORT = 0xFF;			// pullups on
  LED_PORT = 0xFF;			// LEDs off (low active)
  LED_DDR = 0xFF;			// LED output
  key_state = ~KEY_PIN;			// no action on keypress during reset
  sei();

  for(;;){					// main loop
    display_debounce_key0();

    if( get_key_press( 1<<KEY0 ))	// LED0 = toggle on keypress
      LED_PORT ^= 1<<LED0;

    if( get_key_press( 1<<KEY1 ))	// LED1 = toggle on keypress
      LED_PORT ^= 1<<LED1;
  }
}
