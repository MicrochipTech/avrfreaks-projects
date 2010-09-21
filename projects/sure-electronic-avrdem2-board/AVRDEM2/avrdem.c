// avrdem.c - mainline for Sure Electronics AVRDEM2 board test routines
// target : ATMega16, 12 Mhz
//
// source file for Sure Electronics AVRDEM2 board test routines
// written by Chuck Baird (avrfreaks.net, user zbaird)
// you may freely use this code for any purpose; attribution is appreciated
//
// support files (.c and .h)
//
//   ad_usart  - USART support (original default is 19.2)
//   ad_7seg   - 7 segment LED support
//   ad_switch - switch support
//   ad_leds   - standalone LED support
//   ad_lcd    - LCD support
//   ad_spkr   - speaker support
//   ad_temp   - temperature support
//   ad_EEPROM - standalone EEPROM support
//   ad_twi    - TWI support (needed for temp and EEPROM -
//               the .h file is automatically included when
//               needed. the .c file has to be in the project)
//
// by including the header file for one of these, you activate its
// initialization code, variables, and ISR support, and make its
// support routines callable. by omitting what you don't use (and
// the corresponding .c files) the resulting code size will be reduced.
//
// there are also virtual timers which measure intervals of up to
// about 10 minutes with a resolution of approximately 0.01 seconds.
// these are always active - see the header file for details.
//
// -------------------------------------------------------------
// hardware configuration:
//
// Port A:
//   bit 0 - standalone LED (bottom)     0 = on
//   bit 1 - standalone LED (third)      0 = on
//   bit 2 - standalone LED (second)     0 = on
//   bit 3 - standalone LED (top)        0 = on
//   bit 4 - thousands 7 seg LED select  1 = on
//   bit 5 - hundreds 7 seg LED select   1 = on
//   bit 6 - tens 7 seg LED select       1 = on
//   bit 7 - ones 7 seg LED select       1 = on
//
// Port B: LCD data (? = on)
//         LED segments (1 = on)
//           -        bit 0
//          | |       bits 5, 1
//           -        bit 6
//          | |       bits 4, 2
//           -  X     bits 3, 7
//
// Port C:
//   bit 0 - SCL, temp and EEPROM (with ext pullup)
//   bit 1 - SDA. temp and EEPROM (with ext pullup)
//   bit 2 - 
//   bit 3 - 
//   bit 4 - 
//   bit 5 - 
//   bit 6 - switch 2 (1 if open, 0 if closed)
//   bit 7 - switch 1 (1 if open, 0 if closed)
//
// Port D:
//   bit 0 - RxD
//   bit 1 - TxD
//   bit 2 - LCD - RS
//   bit 3 - LCD - R/W
//   bit 4 - LCD - E
//   bit 5 - LCD contrast
//   bit 6 - 
//   bit 7 - speaker (OC2)
//
// -------------------------------------------------------------

#include "avrdem.h"

// include whichever of the following that you are using

//#include "ad_usart.h"           // USART support
//#include "ad_7seg.h"            // 7 segment LED support
//#include "ad_switch.h"          // switch support
//#include "ad_leds.h"            // standalone LED support
//#include "ad_lcd.h"             // LCD support
//#include "ad_spkr.h"            // speaker support
//#include "ad_temp.h"            // temperature support
//#include "ad_EEPROM.h"          // standalone EEPROM support

// put this file in your project as the primary source file, plus the
// virtual timers source file (ad_vtimers.c). if you use any of the
// above features, add their source files as well. if you use either
// the temperature or EEPROM, add the TWI source file too (ad_twi.c).
// you do not need to directly reference the TWI header file.

char i10x;                      // virtual timer interrupt counter

// your global variables, function prototypes, etc. go here

// -------------------------------------------------------------
// mainline

void main(void) {
  // your local variables go here

  init();
  SEI();

  // your "one time" code goes here

  while (1) {
    
    //  your "repeat indefinitely" code goes here

  }
}

// ------------------------------------------------------------- 
// TWI interrupt handler
//
// we'll come here any time the TWI does something interesting. it
// generally means stepping through our current TWI script.
//
// snow points to one of the set of status/action pairs for the
// current entry. we'll check the current status against all those
// in the set to see (hopefully) if we have a match.
//
// when we come in, snow may or may not be pointing at the first
// entry in the current set. we'll first move it back up to the
// first entry if we need to, then we'll look for a match among
// all the entries that make up the current set. if we don't find
// a match, it's curtains.

#ifdef OK_TWI

#pragma interrupt_handler twi_isr:iv_TWI
void twi_isr(void)
{
  char act, nxt, adv;

  if (twi_sts != TWI_RUN) {     // there had better be a script running
    twi_sts = TWI_DIS;          // disabled, no twi script running
    TWCR = 1 << TWINT;          // disable twi
    return;
  }

  // first we'll find the first of possibly multiple current entries
  // if the MSK_1 bit is zero, it's the first of the set
  while ((*snow) & MSK_1) snow--;   // move upward if necessary

  // now look at each of the set for a status match
  while ((TWSR & 0xf8) != ((*snow) & 0xf8)) {
    if ((*snow) & MSK_N) {      // any more in set?
      snow++;                   // yes - move to next
    } else {                    // no - we have failed
      twi_sts = TWI_ERR;        // show error condition
      TWCR = 1 << TWINT;        // disable twi
      return;
    }
  }

  // we found a status match - now decide what to do
  // store indirect and stop may be combined with other actions, but
  // the rest are mutually exclusive.

  act = *snow >> 8;             // grab the action byte
  // nxt will be what happens next (stored into TWCR)
  nxt = (1<<TWINT) | (1<<TWEN) | (1<<TWIE);  // always include these
  adv = 0;                      // 0 means advance the script

  if (act & MS_STORE) {         // are we storing data (receiving)?
    *use.dptr++ = TWDR;         // yes - put data away and adv pointer
    use.dcnt--;                 // count it
  }

  // stop overrides everything but store
  if (act & MS_STOP) {          // are we stopping?
    twi_sts = (act & MS_XTRA) ? TWI_NRDY : TWI_OK;   // user status
    TWCR = nxt | (1 << TWSTO);  // put the brakes on
    return;
  }

  // it's a normal command - break into sending and other
  // (yes, it could be one large switch statement)

  if (act & MS_SEND) {          // are we sending data?
    switch (act & 0x0f) {       // yes - we have 4 kinds
      case TS_SNDF:             // fixed data
        TWDR = use.fixed;
        break;
      case TS_SNDW:             // write command
        TWDR = use.slw;
        break;
      case TS_SNDR:             // read command
        TWDR = use.slr;
        break;
      case TS_SNDI_A:           // (also ..._H) send from indirect
        adv = act & MS_XTRA;    // extra bit tells to hold or advance
        if (use.dcnt < 2) adv = 0;  // advance on last byte
        TWDR = *use.dptr++;     // send data
        use.dcnt--;             // count it
        break;
      default:                  // unknown
        break;
    }
  } else {                      // we are not sending data
    switch (act & 0x0f) {
      case TS_RERUN:            // restart script
        // note that on a restart we are assuming that the count
        // and data pointer are still intact. this is true for the
        // EEPROM scripts.
        snow = use.sptr;            // move pointer to beginning
        TWCR = nxt | (1 << TWSTA);  // do a repeat start to hold bus
        return;
      case TS_RS:               // send repeat start
        nxt |= (1 << TWSTA);    // add the rs bit
        break;
      case TS_RCV_A:            // (also ..._H) start receiver
        if (use.dcnt > 1) nxt |= (1 << TWEA);   // ACK or NAK response
        adv = act & MS_XTRA;    // extra bit = hold or advance
        break;
      default:                  // nop (placeholder for store & stop)
        break;
    }
  }

  // maybe advance to the next set of script entries
  if (!adv) {
    snow++;                       // get off the current one
    while (*snow & MSK_1) snow++; // move down 'til a first
  }
  TWCR = nxt;                     // goose the TWI hardware
}
#endif

// -------------------------------------------------------------  
// Timer0 interrupt handler
//
// we'll come here approximately every millisecond. LCD communications
// (if any) will be done elsewhere in real time (with interrupts off, since
// the LCD and 7 segment drivers share a port).
//
// on every 10th interrupt maybe we'll update a virtual timer
//
// if we're using the switches:
//   on every other interrupt, we'll do switch debouncing. this will give a
//   debounce window of 16ms.
//
// if we're using the standalone LEDs
//   on every 4th interrupt, we'll do the standalone LED multiplexing (16
//   ints to do all 4 leds), which gives us a 62.5 Hz update rate for each.
//
//  if we're using the 7 segment displays
//    on every other interrupt we'll do 1/2 digit. this gives us a 62.5 Hz
//    refresh rate for all 4 digits.
//
//  if we're using sound
//    we'll update the sound queue on each interrupt

#pragma interrupt_handler timer0_comp_isr:iv_TIM0_COMP
void timer0_comp_isr(void)
{
  unsigned int cc;

  // --------------- virtual timer updates
  i10x++;                       // count every 10th interrupt
  if (i10x > 9) i10x = 0;       // keep in the 0 - 9 range

  if (i10x < NVTIMER) {         // do we have a virtual timer for this one?
    if (atime[i10x].active = V_GO) {      // is this one running?
      cc = atime[i10x].count;             // current count
      if (cc == 0) cc = atime[i10x].max;  // safety net
      cc--;                        // another 0.01 secs gone byebye
      if (cc == 0) {               // did the timer run out?
        *atime[i10x].flag = 1;     // set the user flag
        if (atime[i10x].mode) {    // are we free running?
          atime[i10x].active = 0;  // one shot - stop timer
        } else {
          cc = atime[i10x].max;    // free run - start count over
        }
      }
      atime[i10x].count = cc;      // remaining count
    }
  }


 // --------------- switch debouncing
#ifdef OK_SWITCH
  if (i10x & 0x01) {            // every other interrupt
    char jp = PINC;             // read the switches
    
    sw1 = (sw1 << 1) | ((jp & 0x80) ? 1 : 0);
    sw2 = (sw2 << 1) | ((jp & 0x40) ? 1 : 0);

    if ((sw1 == SW_OPEN) && (sw1_last == SW_CLOSED)) {
      sw1_last = SW_OPEN;
      sw1_chg = 1;
    }
    if ((sw1 == SW_CLOSED) && (sw1_last == SW_OPEN)) {
      sw1_last = SW_CLOSED;
      sw1_chg = 2;
    }
    if ((sw2 == SW_OPEN) && (sw2_last == SW_CLOSED)) {
      sw2_last = SW_OPEN;
      sw2_chg = 1;
    }
    if ((sw2 == SW_CLOSED) && (sw2_last == SW_OPEN)) {
      sw2_last = SW_CLOSED;
      sw2_chg = 2;
    }
  }
#endif

 // --------------- individual LED display
#ifdef OK_LEDS
  led_cnt++;                    // do something every 4th interrupt
  if (led_cnt > 3) {
    led_cnt = 0;                // start count over
    led_mask <<= 1;             // shift mask
    if (led_mask & 0xf0) led_mask = 0x01;   // keep it within bits 0 - 3
    PORTA |= 0x0f;              // kill all the LEDs by writing 1s
    if (!(sa_led & led_mask)) PORTA &= ~led_mask;  // copy a zero to the port
  }
#endif

 // --------------- 7 segment LED display
#ifdef OK_7SEG
  // s7cnt is controlling both the multiplexing and the selection of
  // every other interrupt. bit 4 toggles; when set we use that interrupt.
  // bits 0-1 tell which bit pattern (seg7_s[]) to use, and bit 2 tells
  // whether we're going to output the low nibble or the high nibble

  if (seg7_stat == SEG7_ON) {           // are we using the 7 seg display?
    char msk, c, oval;

    s7cnt ^= 0x10;                      // toggle the interrupt gate bit
    if (s7cnt & 0x10) {                 // we'll take every other one
      s7cnt = (s7cnt + 1) & 0x17;       // counter bits go 0 - 7

      c = s7cnt & 0x03;                 // which digit (0 - 3)
      msk = 0x10 << c;                  // digit select mask for PORTA
      oval = seg7_s[c];                 // the full segment bit pattern
      if (s7cnt & 0x04) {               // doing high or low nibble?
        oval &= 0xf0;                   // high
      } else {
        oval &= 0x0f;                   // low
      }
      PORTA &= 0x0f;                    // turn all segs off
      PORTB = oval;                     // set up the output bits
      PORTA |= msk;                     // select our digit
    }
  }
#endif

 // --------------- sound
#ifdef OK_SPKR
  if (playing == 1) {                   // time to start a song?
    playing = 2;
    cnt_dur = 0;                        // force some action
    cnt_tempo = 0;
  }

  // when we're playing, on most interrupts we won't do anything
  // usually things happen every TEMPO * DURATIONS interrupts

  if (playing == 2) {                   // time to continue a song?
    char doit = 0;                      // assume we're ignoring this int

    if (cnt_dur) {                      // counter for this duration
      cnt_dur--;
    } else {
      if (cnt_tempo) {                  // counter for this tempo interval
        cnt_dur = cur_dur;              // reset the duration counter
        cnt_tempo--;                    // one less round for the tempo
      } else doit = 1;                  // hey - it's time to do something!
    }

    if (doit) {                         // we have ignition
      while (doit) {
        int nxt;

        nxt = *cur_note++;              // the next note or command
        if (nxt) {                      // are we at the end of the song?
          switch (nxt & MSK_CMD) {      // no - is it a command?
            case N_TEMP:                // redefine the tempo
              cur_tempo = (nxt & 0x0ff);
              if (!cur_tempo) cur_tempo++;  // minimum of 1
              break;
            case N_DUR:                 // redefine the duration
              cur_dur = (nxt & 0xff);
              if (!cur_dur) cur_dur++;  // minimum of 1
              break;
            default:                    // it's a note (or illegal)
              doit = 0;                 // break out of while loop
              switch (nxt & MSK_LEN) {  // quarter, half, etc?
                case N_1:                  // whole note
                  cnt_dur = cur_dur << 2;  // four times as long
                  break;
                case N_2:                  // half note
                  cnt_dur = cur_dur << 1;  // twice as long
                  break;
                case N_8:                  // eighth note
                  cnt_dur = cur_dur >> 1;  // half as long
                  break;
                default:                   // quarter note
                  cnt_dur = cur_dur;       // normal - no change
                  break;
              }
              cnt_tempo = cur_tempo;    // tempo to counter

              TCCR2 = 0x00;             // stop timer 2
              if ((nxt & 0x07ff) != N_REST) {  // do we have a real note?
                OCR2 = nxt & 0xff;      // compare count
                TCNT2 = 0;              // counter value
                TCCR2 = 0x18 | ((nxt >> 8)  & 0x07);  // ctc, toggle, prescaler
              }
              break;
          }
        } else {                        // we are at the end of the song
          doit = 0;                     // break out of while loop
          cur_rpt--;                    // one less repeat to go
          if (cur_rpt) {
            cur_note = cur_song;        // play it again, sam
            cur_dur = DURATION;         // default note duration
            cur_tempo = TEMPO;          // default tempo
          } else {
            TCCR2 = 0x00;               // stop timer 2
            playing = 3;                // tell the user we ended
          }
        }
      }
    }
  }

#endif

}

// -------------------------------------------------------------
// init() - initialization
//

void init(void) {
  char i, lcdarg = 0;

  CLI();                        // disable interrupts

  MCUCR = 0x00;
  GICR  = 0x00;

  DDRA = 0x00;                  // clean house (not really needed)
  PORTA = 0x00;
  DDRB = 0x00;
  PORTB = 0x00;
  DDRC = 0x00;
  PORTC = 0x00;
  DDRD = 0x00;
  PORTD = 0x00;

  i10x = 0;                     // 10x interrupt counter
  init_vtimers();               // initializae the virtual timers

// ---- call the initialization for the devices being used
// note that the lcd code needs to know whether the 7 segment display
// is being used or not since they share a port
#ifdef OK_TWI
  init_twi();                   // initialize the TWI hardware
#endif
#ifdef OK_USART
  init_usart(BRATE_192);        // initialize the USART @ 19.2K
#endif
#ifdef OK_7SEG
  init_7seg();                  // initialize 7 segment display
  lcdarg = 1;                   // the 7 segment code is alive
#endif
#ifdef OK_SWITCH
  init_switch();                // initialize switches
#endif
#ifdef OK_LEDS
  init_leds();                  // initialize standalone LEDs
#endif
#ifdef OK_LCD
  init_lcd(lcdarg);             // initialize LCD
#endif
#ifdef OK_SPKR
  init_spkr();                  // initialize the speaker
#endif
#ifdef OK_TEMP
  init_temp();                  // initialize the temperature device
#endif
#ifdef OK_EEPROM
  init_eeprom();                // initialize the external EEPROM
#endif
// ---- end of conditional initializations

  // TIMER0 initialization, ~1KHz. used for the multiplexing, debouncing,
  // and virtual timer maintenance - prescale:64, WGM: CTC, value: 0.997 KHz
  TIMSK = 1 << OCIE0;           // timer interrupt sources
  TCCR0 = 0x00;                 // stop the timer
  TCNT0 = 0x00;                 // initial count value
  OCR0  = 0xbb;                 // set compare value (0.997 KHz)
  TCCR0 = 0x0b;                 // start timer in CTC mode, prescale = 64
}

// -------------------------------------------------------------
