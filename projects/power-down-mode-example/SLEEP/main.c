/************************************************************************/
/*                                                                      */
/*              	AVR Power Down Mode Example                   	*/
/*									*/
/* The application light a LED and flash a LED		                */
/* until a timeout of 20sec or a key was pressed			*/
/* To wake up, the key must be pressed again				*/
/*                                                                      */
/* Peter Dannegger			                                */
/*                                                                      */
/************************************************************************/

// Target: ATmega48

#include <avr\interrupt.h>
#include <avr\sleep.h>
#include "main.h"
#include "getkey.h"


enum{ OFF, ON };

uint8_t state = OFF;				// CPU state
uint16_t time_tick = 0;				// 10ms timer


void init( void )
{
  PORTB = 0xFF;					// pullup on unused pins
  PORTC = 0xFF;
  PORTD = 0xFF;
  LED_0_DDR = 1;
  LED_1_DDR = 1;
  KEY_ON_OFF_PCI_REG = 1<<KEY_ON_OFF_PCI;	// select wake up key
  set_sleep_mode( SLEEP_MODE_IDLE );		// default: idle
  sleep_enable();
  get_key_init();
}


ISR( WAKEUP_vect )				// wake up from power down
{
  set_sleep_mode( SLEEP_MODE_IDLE );
  PCICR = 0;
}


void power_down( void )
{
  if( get_key_idle( 1<<KEY_ON_OFF )){		// if debounce done
    set_sleep_mode( SLEEP_MODE_PWR_DOWN );
    PCICR = 1<<KEY_ON_OFF_ENABLE;		// enable wake up
  }
}


void key_on_off( void )
{
  if( get_key_press( 1<<KEY_ON_OFF )){
    if( state != OFF )
      state = OFF;
    else
      state = ON;
    time_tick = 0;				// reset timeout
  }
}


int main( void )
{
  init();
  sei();

  for(;;){
    sleep_cpu();				// idle or power down
    key_on_off();				// on/off key
    time_tick++; 				// 10ms
    if( time_tick > 2000 )			// off after 20s
      state = OFF;

    if( state == OFF ){
      LED_0 = LED_OFF;
      LED_1 = LED_OFF;
      power_down();				// power down mode
    }else{
      LED_0 = LED_ON ^ !!(time_tick & 0x40);	// flasher 1.28s
      LED_1 = LED_ON;
    }
  }
}
