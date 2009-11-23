// timeout timer
// AVR Butterfly (ATmega169), 8MHz
// ImageCraft ICCAVR compiler

#include <iom169v.h>
#include <macros.h>
#include "pmusic.h"
#include "ev_list.h"

// Ports:
//   Port D - 8 LEDs, bit 0 = leftmost, bit 7 = rightmost
//   Port B
//     bit 0 = heartbeat 1KHz output signal for o'scope debug/verify
//     bit 6 = pushbutton (same as UP Butterfly joystick)
//             pullup enabled, pushing button grounds input (reads as 0)
//     bit 5 = piezo element (timer1 PWM output)
//
// program flow:
//   if the Butterfly bootloader is used, you must press the button
//   to exit the bootloader and start the application.
//
//   on reset:
//     make startup noise
//     wait for button press & release
//     sequence through the 8 LEDs to let user pick number of minutes
//     wait for button press & release to select time, or
//     if 1 minute elapses, go into shutdown mode
//   when time (minutes) has been selected:
//     start timer for number of minutes selected, doing 8 light countdown
//     play song when done and go into shutdown mode
//     if button is pressed during timeout, reset
//  shutdown mode:
//     beep every second, 4 times
//     wait 12 seconds
//     reset if button pushed and released
//
// the Butterfly bootloader (at least newer versions) leaves a few loose
// ends that we'll clean up, including a 2 MHz clock setting.


// I/O bit definitions
#define HEART  0        // PB0 - timer heartbeat output
#define BUTTON 6        // PB6 - pushbutton input
#define PIEZO  5        // PB5 - piezo element output

// some odds and ends
#define T_SEQ 15        // number of 1/10 seconds between counting LEDs
#define T_KILL 600      // number of 1/10 seconds to die, generally
#define DEBOUNCE 2      // debounce time = 8 * DEBOUNCE milliseconds
#define D_CNT 4         // # repeats of beep during croaktime
#define T_BEEP 10       // time between croak beeps (1/10 sec)
#define T_QUIET 120     // time between croak bursts (1/10 sec)

// actions
#define ACT_NOP 0       // no operation - kill time
#define ACT_PUSH 1      // waiting for initial button push
#define ACT_RELEASE 2   // waiting for initial button release
#define ACT_SEQ 3       // start the LED counting sequence
#define ACT_B1 4        // waiting for button down during sequence
#define ACT_B2 5        // waiting for button up during sequence
#define ACT_TIME 6      // start the timeout period
#define ACT_TIMING 7    // during the timeout period
#define ACT_DIE 8       // start the final rites
#define ACT_DYING 9     // continue the final rites
#define ACT_ADVANCE 10  // move to next LED in timeout sequence

// variables
char led;               // LEDs as display (bit 0 = leftmost)
char mplex;             // LED multiplex bits (2 bits set)
volatile char user;     // debounced pushbutton (0=open, 0xff=pressed, other=transition)
char ucnt;              // debouncer interrupt counter
char mincnt;            // count of minutes of timeout
char freeze;            // user selected number of minutes of timeout
char act;               // current action
char diecnt;            // status of die sequence
int led_delay;          // time until led advances to next one (1/10 secs)
char led_active;        // which led we're flashing (bit 0 = leftmost)
int c_num;              // tic count for flashing LEDs
int c_num1;             // master copy of c_num
char c_cnt;             // number of times to use c_num
char c_cnt1;            // master copy of c_cnt

// function prototypes
void init(void);
void advance_seq(void);
void die_seq(void);
void reset_us(void);
void advance_led(void);
void toggle_led(void);
void first_cnt(void);
void advance_cnt(void);


// a song consists of a repeat count followed by notes and/or parameters
// (tempo, duration, volume) terminated by a 0.  all values are ints.  when
// the song starts the default tempo, duration, and volume are in effect,
// and each stays in effect until changed.

const int Beeper[] =                    // reminder to power off
  {1, N_LEN|8, N2_A, N0_A, 0 };

// this is taken from the Butterfly's application code - Beethoven's Fur Elise
const int FreeAtLast[] =                // end of timeout period song
  {1, N_LEN|12, N2_E, N2_Ds, N2_E, N2_Ds, N2_E, N1_B, N2_D, N2_C, N_LEN|6, N1_A,
   N_LEN|12, N_REST, N1_C, N1_E, N1_A, N_LEN|6, N1_B, N_LEN|12, N_REST, N1_E, N1_Gs,
   N1_B, N_LEN|6, N2_C, N_LEN|12, N_REST, N1_E, N2_E, N2_Ds, N2_E, N2_Ds, N2_E,
   N1_B, N2_D, N2_C, N_LEN|6, N1_A, N_LEN|12, N_REST, N1_C, N1_E, N1_A, N_LEN|6,
   N1_B, N_LEN|12, N_REST, N1_E, N2_C, N1_B, N_LEN|6, N1_A, 0 };

const int StartUp[] =                   // startup sound
  {1, N_LEN|24, N1_C, N1_C, N1_C, N_LEN|48, N0_G, 0 };

// -------------------------------------------------------
//      main() - mainline

void main(void) {
  init();                       // do all initialization
  SEI();                        // let the games begin

  add_event(die_seq, T_KILL);   // make an appointment with death if user too slow
  act = ACT_PUSH;               // waiting for initial button press
  freeze = 0;                   // we don't have a minutes count yet
  start_song(StartUp);

  while (1) {
    switch (act) {
      case ACT_PUSH:            // waiting for initial button press
        if (user == 0xff)
          act = ACT_RELEASE;    // yup - so now wait for them to let up
        break;

      case ACT_RELEASE:         // waiting for initial button release
        if (user == 0) act = ACT_SEQ;  // start the counting sequence
        break;

      case ACT_SEQ:             // start the counting sequence
        clear_events();         // throw out the old death threat
        add_event(die_seq, T_KILL);  // add a new death threat
        led = 0x01;             // light leftmost LED
        mincnt = 1;             // 1 LED = 1 minute of timeout
        add_event(advance_seq, T_SEQ);  // advance the LED counter in a little while
        act = ACT_B1;           // keep an eye on the button
        break;

      case ACT_B1:              // button pushed during sequence?
        if (user == 0xff) {
          act = ACT_B2;         // yup - wait for release
          freeze = mincnt;      // use the number of minutes showing
        }
        break;

      case ACT_B2:              // button released during sequence?
        if (user == 0) act = ACT_TIME;  // start the timeout period
        break;

      case ACT_TIME:            // start the poor kid's timeout period
        clear_events();         // no death threats
        led_delay = 75 * freeze;  // 0.1 secs per LED for selected #minutes
        led = 0xff;             // light all the leds
        led_active = 0x80;      // which one to toggle (the rightmost one)
        add_event(advance_led, led_delay);  // this will advance the leds
        first_cnt();            // get initial values for flashing the LEDs
        c_num1 = c_num;         // save the flash time
        c_cnt1 = c_cnt;         // and the number of times to use it
        add_event(toggle_led, c_num);  // this will toggle the active one
        act = ACT_TIMING;       // watch for button press to cancel
        break;

      case ACT_TIMING:          // during the timing period
        if (user == 0xff) {     // if they push a button, start over
          clear_events();       // stop any other nonsense
          while (user) ;        // once they let up on it
          reset_us();
        }
        break;

      case ACT_ADVANCE:         // move to the next LED, or stop the timeout
        break;

      case ACT_DIE:             // begin act III - the denouement
        clear_events();         // clean up any loose ends
        if (++diecnt == D_CNT) {
          add_event(die_seq, T_QUIET);  // lay low for a while
          diecnt = 0;                   // then start sequence from beginning
        } else {
          add_event(die_seq ,T_BEEP);   // set up for next beep
        }
        act = ACT_DYING;          // waste some time between beeps
        start_song(Beeper);
        break;

      case ACT_DYING:           // continuing to die - watch for button press
        if (user == 0xff) {     // if they push a button, start over
          while (user) ;        // once they let up on it
          reset_us();
        }
        break;

      default:                  // nop
        break;
    }
    check_events();             // see if an event is due to execute
  }
}

// -------------------------------------------------------
//      advance_led() - move to the next LED during timeout period

void advance_led(void) {
  clear_events();               // get rid of spurious toggle event
  led &= (~led_active);         // drop the active led in case it's on
  led_active >>= 1;             // shift active led left (bit 0 is leftmost led)
  if (led_active) {             // if not done, set up for next toggle & advance
    add_event(advance_led, led_delay);  // this will advance the leds
    c_cnt = c_cnt1;             // restore the initial toggle parameters
    c_num = c_num1;
    add_event(toggle_led, c_num);   // set toggle event for the active one
  } else {                      // otherwise clean up loose ends
    led = 0;                    // all leds off
    start_song(FreeAtLast);
    add_event(die_seq, T_KILL); // in a little while, start the death throes
  }
}

// -------------------------------------------------------
//      toggle_led() - toggle the active LED during timeout period

void toggle_led(void) {
  led ^= led_active;
  advance_cnt();                // get next toggle period (they get shorter)
  add_event(toggle_led, c_num); // and set event for next toggle
}

// -------------------------------------------------------
//      die_seq() - handle the dying gasps
//
// when we die we'll try to get them to shut the power off by beeping a few times
// (D_CNT) a few seconds apart (T_BEEP / 10), then wait a few seconds (T_QUIET / 10),
// then repeat until the cows come home

void die_seq(void) {
  act = ACT_DIE;
}

// -------------------------------------------------------
//      reset_us - do a reset via the watchdog timer
//
// we don't have a software reset, and while we can execute reset code it isn't
// the same. we could wire an I/O pin to the reset line and hit ourselves on the
// head, but getting Fido involved is a reasonable compromise

void reset_us(void) {
  WDTCR |= (1<<WDCE) | (1<<WDE); // enable watchdog timer write
  WDTCR = 1<<WDE;               // start watchdog with about 15ms timer
  while (1) ;                   // wait for Godot
}

// -------------------------------------------------------
//      advance_seq() - advance the time selection LEDs
//
// mincnt will have a value from 1 to 8 and then repeat; led will have 1
// to 8 bits set (bit 0 corresponds to mincnt = 1)

void advance_seq(void) {
  if (!freeze) {                // if they've selected already, ignore this
    add_event(advance_seq, T_SEQ);   // call again in a while
    if (mincnt > 7) {
      led = 0x01;               // start over with 1 minute & LED
      mincnt = 1;
    } else {
      led <<= 1;                // shift bit(s) left 1 place
      led++;                    // turn on bit 0
      mincnt++;                 // #minutes is now 1 greater
    }
  }
}

// -------------------------------------------------------
//      first_cnt() - calculate the first tic count for flashing LEDs
//
// we want to flash the LEDs at an increasing faster rate, which means smaller and
// smaller times when adding events. this function generates the first time, and
// advance_cnt() generates each of the following times.
//
// we will have a total tic count (1/10 secs) in led_delay (= 75,150,...,600 for 1
// to 8 minutes of timeout).  this is the total time each LED will flash while it's
// the active one. here we're generating its on and off times while it's active.
// if we look at the on/off times in reverse order (from the shortest time, right
// before the LED goes inactive), the pattern is: 4,4,4,5,5,5,5,6,6,6,6,6,..., where
// for each value the number n appears n-1 times (that is, three 4s, four 5s, etc.).
//
// so we look down this list until the sum of the numbers before our target exceeds
// led_delay, and c_num will be the target number (4,5,6,...), and c_cnt will be
// the number of times that number will be used.  after c_num is used c_cnt times,
// c_num is decremented, and c_cnt will be set to c_cnt - 1 for the next round.
// repeat until the total time is exhausted.
//
// instead of starting at the beginning of the list, we start with c_num = 7 since
// all the numbers up to that point sum up to 62.  our lowest possible sum is 75,
// so we can save a few iterations of the loop.

void first_cnt(void) {
  int sum;

  c_num = 7;                    // start in the middle of the list
  sum = 62;                     // since led_delay >= 75
  while (1)
    for (c_cnt = 1; c_cnt < c_num; c_cnt++) {
      sum += c_num;
      if (sum > led_delay) return;
    }
}

// -------------------------------------------------------
//      advance_cnt() - calculate the next tick count for flashing LEDs
//
// the variable c_num is the time to use when scheduling the on/off event.
// return it c_cnt times, then decrement c_num and return it c_cnt-1 times.
// repeat until c_num goes to 4, and use it thereafter.

void advance_cnt(void) {
  if (--c_cnt < 1) {
    if (c_num > 4) c_num--;
    c_cnt = c_num - 1;
  }
}

// -------------------------------------------------------
//      init() - general initialization

void init(void) {
  // clean up some odds and ends, some of which aren't necessary
  CLI();                        // kill interrupts

  WDTCR |= (1<<WDCE)|(1<<WDE);  // enable watchdog timer write
  WDTCR = 0x00;                 // kill the watchdog
  CLKPR = 1 << CLKPCE;          // enable clock prescale write
  CLKPR = 0x00;                 // prescaler of 1 (8 MHz)

  LCDCRA = 0x00;                // LCD off
  TCCR1B = 0x00;                // stop timer 1
  TCCR2A = 0x00;                // stop timer 2

  TIMSK0 = 0x00;                // timer 0 - no interrupts
  TIMSK1 = 0x00;                // timer 1 - no interrupts
  TIMSK2 = 1 << OCIE2A;         // timer 2 - compare match A
  EIMSK = 0x00;                 // external & pin change ints off
  PCMSK0 = 0x00;                // pin change mask - no ints
  PCMSK1 = 0x00;                // pin change mask - no ints

  // configure our ports
  DDRD = 0xff;                        // our 8 LEDs are outputs
  DDRB = (1 << PIEZO) | (1 << HEART); // piezo & heartbeat are output
  PORTB = 1 << BUTTON;                // the pushbutton needs its pullup

  // initial values for variables
  diecnt = 0;                   // not in final sequence
  playing = 0;                  // no song is playing
  etime = 0;                    // interrupt counter for updating events
  locked = 0;                   // event list is not locked
  bypass = 0;                   // we haven't been locked on an interrupt
  nevents = 0;                  // there are no events
  led = 0;                      // no LEDs are on (1 = on)
  diecnt = 0;                   // not dying yet
  mplex = 0x11;                 // next LEDs to be multiplexed
  ucnt = 0;                     // debouncer interrupt counter
  user = 0x55;                  // arbitrary transitional state for pushbutton

  // timer2 is used in CTC mode, about 1KHz, for main clock
  ASSR = 0x00;                  // set async mode
  TCNT2 = 0x00;                 // initial timer value
  OCR2A = 0x3E;                 // compare value when interrupt occurs
  TCCR2A = 0x0D;                // start timer 2 (CTC, prescaler = 128)
}

// -------------------------------------------------------
//      1 KHz interrupt handler for timer 2
//
// on each interrupt, 2 of the LEDs will be active (and will be lit if appropriate);
// this will cycle through 4 stages round robin, servicing all 8 LEDs in 4 interrupts.
// every 100 interrupts (1/10 sec) the active events will have their timers updated.
// this also manages the playing of sounds via timer1 PWM, and debounces the
// pushbutton input

#pragma interrupt_handler timer2_comp_isr:iv_TIM2_COMP
void timer2_comp_isr(void)
{
  // --- maintain the debug heartbeat output for verifying interrupts
  PORTB ^= (1 << HEART);        // toggle the heartbeat output

  // --- put out 2 multiplexed bits for LEDs
  PORTD = ~(led & mplex);       // put out 2 bits of led (0 turns on)
  mplex <<= 1;                  // shift the multiplex bits
  if (mplex == 0x10) mplex = 0x11;      // shifted right out of town

  // --- maintain the event list timers
  etime++;                      // count these doggies
  if (etime > 99) {             // every 1/10 sec, update event list
    if (locked) bypass = 1; else update_events();
    etime = 0;                  // start counter over
  } else {
    if (bypass) {                    // did we skip it last time?
      if (!locked) update_events();  // if we did, and it's now unlocked, do it
    }
  }

  // --- debounce the pushbutton input (pushed button reads as 0)
  ucnt++;                       // counting interrupts between reads
  if (ucnt >= DEBOUNCE) {       // total debounce time is 8 * DEBOUNCE msec
    ucnt = 0;                   // start counter over
    user <<= 1;                 // shift the debounce variable 1 left
    if ((PINB & (1 << BUTTON)) == 0) user++;    // bit 0 = 1 if button pressed
  }

  // --- if we're playing, see if we need to shoot out the next note
  // the tempo (cur_tempo) is the number of interrupts to skip between checking
  // the duration (cur_dur) is how many checks to hold note
  if (playing) {
    cur_tempo--;                // tempo is number of ints to skip
    if (!cur_tempo) {           // if zero, we do duration count
      cur_tempo = cnt_tempo << 3;    // restore the tempo counter
      cur_dur--;                // and decrement the duration counter
      if (!cur_dur) {           // if it went to zero, time for next note
        if (get_next_note()) {  // process any leading commands
          cur_dur = cnt_dur;    // next duration to use
          cur_tempo = cnt_tempo;   // next tempo to use
          play_note();          // load timer 1 with note value
        } else {
          stop_playing();
          PORTB &= ~(1<<PIEZO); // drop the piezo output
        }
      }
    }
  }
}
