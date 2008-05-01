/*  Program to use single button for multiple decisions
** Author Justin Gaither
** 
** Operates quick press counter, timeout, and 2 hold settings
**
** Revision history
** 
*/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <avr/pgmspace.h>

// Button Press State Variable Enumeration
#define IPRESS 0x01  // 
#define RELEASE 0x02
#define IDLE 0x04

// Timeouts
#define TIME_OV 197 // 10 sec timeout
#define TIME_2S 40  // 2 second hold
#define TIME_5S 80  // 5 second hold

volatile uint8_t button_state,hold5, hold2, press;
volatile uint16_t time; // extended time counter

void start_timer0(void) {
  TCCR0B = _BV(CS02) | _BV(CS00);
}

void stop_timer0(void) {
  TCCR0B = 0;
}

void goto_idle(void) {
	button_state = IDLE; // goto idle state
	press=0;
	TCNT0 =0;
	time=0;
}

// ISR for Pin Change Interrupt
// Change to reflect Input pin for button
// Make sure there is debounce circuit on button
// inorder to eliminate bouncing.
ISR(PCINT0_vect) {
  stop_timer0();  // stop timer
  switch (button_state) {
  case IDLE: { //IDLE State
    if(bit_is_set(PINB,0)) { // press
      start_timer0();
      button_state = IPRESS;
    }
    break;
  }
  case IPRESS: { // Pressed State
    if(bit_is_clear(PINB,0)) { // release
      if(press == 0 && time >= TIME_5S) {
	goto_idle();
	hold5 = 1; // must be cleared in main routine
      } else if(press == 0 && time >= TIME_2S) {
	goto_idle();
	hold2 = 1; // must be cleared in main routine
      } else { // goto release state
	button_state = RELEASE;
	press++; // Quick press
	start_timer0(); // continue counter
      }
    } else { // Pin state is corrupted 
      goto_idle();
    }
    break;
  }
  case RELEASE: { // Released state
    if(bit_is_set(PINB,0)) { // press
      start_timer0(); // continue counting
      button_state = IPRESS;
    } else {
      goto_idle();
    }
    break;
  }
  default: { // states are messed up
    goto_idle();
    break;
  }
  }
  
}

// ISR for Timer overflow.  using an 8bit timer requires
// software overflow extension.
ISR(TIMER0_OVF_vect) { // use TIM0 for tiny
  //PORTD = PORTD ^ 0x20;
  if(time > TIME_OV) {
    stop_timer0();
    goto_idle();
  } else {
    time++;
  } 
}
 

void init_uc(void) {
  stop_timer0();
  TCNT0 = 0;
  // Enable Interrupts
  TIMSK0 = _BV(TOIE0); // enable overflow interrupt
  PCICR = _BV(PCIE0);  // enable pcint0
  PCMSK0 = _BV(PCINT0);
  MCUCR = _BV(PUD);
  hold5 = 0;
  hold2 = 0;
  press = 0;
  time = 0;
  button_state = IDLE;
  sei();
}


// example main routine
int16_t main( void ) {
  uint32_t i;
  uint8_t x,y;
  init_uc();
  }
  for(;;) // Infinite loop
    {
      if(hold5) { // Take action on 5 second hold
	hold5 = 0;
      } else if(hold2) { // Take action on 2 second hold
	hold2 = 0;
      } else if(press > 0) { // take action on multiple quick press 
      } else if(press == 0) {
      }
      // put time into loop
      for(i=0;i<=100000L;i++) {
	x=PINC;
      }
    } // endless loop
  return 0;
} // end of main()
