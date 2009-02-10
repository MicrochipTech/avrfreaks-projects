/*
 * avrlirc.c - IR Remote control to RS323 receiver
 *
 * run-length encodes the ir pulse/space receive signal to
 * offload PC of low level timing.
 *
 * AVR ATTiny2313 pinout :
 *                         ----------
 *  (rst for prog) /reset |1       20| Vcc
 *  (dbg only)    rxd,pd0 |2       19| pb7,pci7,scl   (scl, for prog)
 *  (to pin 16)   txd,pd1 |3       18| pb6,pci6,miso  (miso, for prog)
 *              xtal2,out |4       17| pb5,pci5,mosi  (mosi, for prog)
 *              xtal1,in  |5       16| pb4,pci4,oc1b  (txout, from pin 3)
 *               int0,pd2 |6       15| pb3,pci3,oc1a  (/txout, to DB9-2)
 * (gnd for 'U' loop) pd3 |7       14| pb2,pci2,oc0a
 *  (dbg led #2)   t0,pd4 |8       13| pb1,pci1,ain1
 *  (act led #1)   t1,pd5 |9       12| pb0,pci0,ain0
 *                    Gnd |10      11| pd6,icp        (input, IR-Recv))
 *                         ----------
 *                                     (pins 12-19 available as inputs)
 *
 *
 * hardware setup:
 * ATTiny2313 running from internal 8Mhz osc.  pins 15/16 form a
 * "software inverter" which can be used in place of a hardware
 * RS232 line driver.  (i.e., pin 15 will drive the RS232 RX line
 * directly, at TTL levels.  most consumer-grade serial ports
 * will tolerate this.)
 *
 * for debug, if you want to send characters _to_ the avr, or if
 * you want more reliable/standard RS232 interfacing, use
 * MAX232/3 or equivalent to connect rxd/txd to db-9 pins 2 and
 * 3.  5V supplied from USB serial device, which is also used for
 * serial connection.
 * 
 * for high-speed (115200) operation, use a crystal with a couple
 * 22pf caps.  at high speeds, you'll probably need to use txd
 * (pin 3) directly, and connect it via an RS232 line driver
 * (max232 or equiv).
 *
 *
 *
 * the serial ouptput stream is RS232 data at 38400,8N1.  no flow
 * control.  (or 115200 if using crystal)
 *   - in binary (default) mode, we emit the lirc "udp" protocol: 
 *       packets consist of some number of little-endian 16-bit
 *       words.  the high bit signifies whether the received
 *       signal was high or low; the low 15 bits specify the
 *       number of 1/16384-second intervals the signal lasted,
 *       with a minimum value of 1.  since transmit data is buffered,
 *       baud rates slower than the pulse arrival rate are tolerated.
 *       two zero bytes in a row (which can't occur otherwise) are
 *       reserved as an escape mechanism for someday sending other
 *       types of data.
 *   - ascii mode is a simple command/response, for debugging.  requires
 *       max232 or equiv. line driver -- don't connect the RS232 TX
 *       signal directly to your AVR!!!  enable the ability to run
 *       in ascii mode by defining DO_RECEIVE.
 *
 * both LEDs are optional.  Led #1 (pin 9) is somewhat useful --
 * it indicates activity from the IR receiver.  Led #2 is only
 * used for debugging -- don't populate it unless you're sure you
 * need it.
 *
 * to assist in verifying that the data transmit path is okay,
 * the AVR will transmit an endless stream of 'U' characters if
 * pin 7 is grounded while the chip comes out of reset.  (A stream
 * of ascii 'U' characters appears as a square wave when viewed
 * on an oscilloscope.)  the rest of the interrupts are still active,
 * to allow for testing, but normal data transmission is
 * bypassed.
 *
 **********
 *
 * Copyright 2002 Karl Bongers (karl@turbobit.com)
 * Copyright 2007 Paul Fox (pgf@foxharp.boston.ma.us)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/sleep.h>
#include <avr/wdt.h>

/*
 * fuses:    default for tiny2313: low 0x64 high 0xdf 
 * low: 0xff (external crystal, 8Mhz and up, no div by 8)
 * high 0xdb (for brownout at 2.7v)
 *
 * default fuse values work fine
 */

/* personal preference macros/typedefs */
#define bit(x) _BV(x)
typedef uint16_t word;
typedef uint8_t byte;

/* the UART reception code should probably remain off -- it was really only
 * needed for initial debugging.
 */
#define DO_RECEIVE 0

/*
 * speed selection
 */
// #define FOSC 3686400		// STK500, ext. clock, for example
// #define FOSC 7372800		// with crystal, or int. RC w/ OSCCAL recal.
#define FOSC 8000000		// 38.4Kbaud) (internal osc)
// #define FOSC 12000000
// #define FOSC 11059200
// #define FOSC 14745600


/*
 * GPIO usage
 */
// #define unused	PB0
// #define unused	PB1
// #define unused	PB2
#define TX_INVERT_OUT	PB3
#define TX_INVERT_IN	PB4
// #define MOSI_BITNUM	PB5   // pin17, MOSI, input
// #define MISO_BITNUM	PB6   // pin18, MISO (output)
// #define SCK_BITNUM	PB7   // pin19, SCK, input

// #define RX_BITNUM	PD0   // pin 1
// #define TX_BITNUM	PD1   // pin 2
// #define unused	PD2
#define DO_DEBUG	PD3   // pin 7, input:  ground to force debug loop
#define LED2_BITNUM	PD4   // pin 8, output
#define LED1_BITNUM	PD5   // pin 9, output
#define IRREC_BITNUM	PD6   // pin 11 -- input:  from IR receiver

/* hardware access macros */
#define Led1_Off()	{ PORTD |=	 bit(LED1_BITNUM); }
#define Led1_On()	{ PORTD &=      ~bit(LED1_BITNUM); }
#define Led1_Flip()	{ PORTD ^=	 bit(LED1_BITNUM); }
#define Led2_Off()	{ PORTD |=	 bit(LED2_BITNUM); }
#define Led2_On()	{ PORTD &=      ~bit(LED2_BITNUM); }
#define Led2_Flip()	{ PORTD ^=	 bit(LED2_BITNUM); }
#define IR_high()	(PIND & bit(IRREC_BITNUM))
#define do_debug()	((PIND & bit(DO_DEBUG)) == 0)

// values for TCCR1B
#define CLKDIV_8    2
#define CLKDIV_64   3
#define CLKDIV_256  4
#define CLKDIV_1024 5

/* alternate version of ISR() macro, which doesn't disable all
 * interrupts.
 */
#define INTERRUPTIBLE_ISR(vector)  \
    void vector (void) __attribute__((interrupt)); \
    void vector (void)

volatile word pulse_length;
volatile byte pulse_is_high;
volatile byte had_overflow;

#if DO_RECEIVE

static prog_char error_s[] = "try (h)elp";
static prog_char usage_s[] = "Asc Bin Ir Vers";
static prog_char ascii_s[] = "ascii";
static prog_char binary_s[] = "binary";
static prog_char version_s[] = "AVRlirc " AVRLIRC_VERSION;
static prog_char crnl_s[] = "\r\n";
volatile byte ascii;

#endif

#define TX_QLEN 64  // NB!  power of 2
#define TX_QLEN_MASK (TX_QLEN - 1)
volatile byte tx_r, tx_w;
volatile byte tx_queue[TX_QLEN];

volatile byte mcusr_mirror;

// verify the crystal freq. config
#if FOSC != 14745600 && FOSC != 12000000 && \
    FOSC != 11059200 && FOSC !=  8000000 && \
    FOSC !=  7372800 && FOSC !=  3686400
# error unsupported FOSC value
#endif

/*
 * set up initial chip conditions
 */
void
hw_init(void)
{

    mcusr_mirror = MCUSR;
    MCUSR = 0;
    wdt_disable();

    // eliminate div-by-8 (no-op if 'div by 8' clock fuse not programmed)
    CLKPR = bit(CLKPCE);
    CLKPR = 0;


    // setup outputs

    // port B
    DDRB = bit(TX_INVERT_OUT);	// just one output bit
    // PORTB |= bit(TX_INVERT_IN);	// enable pull-up on input
    PORTB |= bit(TX_INVERT_OUT); // set output high

    // port D -- just leds are outputs
    DDRD |= bit(LED1_BITNUM);
    DDRD |= bit(LED2_BITNUM);
    // turn off leds
    PORTD |= bit(LED1_BITNUM);
    PORTD |= bit(LED2_BITNUM);
    // enable pullup on IR recvr, and on the debug-mode pin
    PORTD |= bit(IRREC_BITNUM);
    PORTD |= bit(DO_DEBUG);

    // set up pin-change interrupt for doing inversion on uart TX
    PCMSK = bit(PCINT4); // enable pin-change for PB4
    GIMSK = bit(PCIE);  // enable pin-change interrupts

    // disable analog comparator -- saves power
    ACSR = bit(ACD);

    // these baud rate divisor values taken from datasheet
#if FOSC == 14745600
    UBRRL = 7;		// for 115200 baud at 14.7456Mhz
#elif FOSC == 12000000
    UBRRL = 12;		// for 115200 baud at 12Mhz
    UCSRA |= bit(U2X);	// doubletime
#elif FOSC == 11059200
    UBRRL = 5;		// for 115200 baud at 11.0592Mhz
#elif FOSC == 8000000
    UBRRL = 12;		// for 38400 baud at 8Mhz
    // UBRRL = 51;		// for 19200 baud at 8Mhz,
    // UCSRA |= bit(U2X);	//  needs doubletime
#elif FOSC == 7372800
    UBRRL = 3;		// for 115200 baud at 7.3728Mhz, or detuned 8Mz
#elif FOSC == 3686400
    UBRRL = 1;		// for 115200 baud at 3.6864Mhz
#endif
    // (UBRRH = 0;)			    // default poweron value

    // enable rx and tx uart functions, and their interrupts
    UCSRB = bit(TXEN);
#if DO_RECEIVE
    UCSRB |= bit(RXEN) | bit(RXCIE);
#endif

    // set 8 bits, no parity, 1 stop bit
    // (UCSRC = bit(UCSZ0) | bit(UCSZ1);)	// default poweron value
    // (TCCR1A = 0;)				// default poweron value
    TCCR1B = bit(ICNC1) | CLKDIV_256;	// see comments at emit_pulse_data()
    // timer1 overflow int enable, and input capture event int enable.
    TIMSK = bit(TOIE1) | bit(OCIE1A) | bit(ICIE1);

    // we use the output compare interrupt to turn off the
    // "activity" LED.  this value is around 1/20th of a
    // second for all the "interesting" clock rates (see comments
    // at emit_pulse_data(), below
    OCR1A = 3000;

    // (set_sleep_mode(SLEEP_MODE_IDLE);)   // default poweron value

}

/*
 * delay - wait a bit
 */
void
delay(word dly)
{
    volatile word i;
    volatile byte j;

    for (i = dly; i != 0; i--)
	for (j = 255; j != 0; j--)
	    /* nothing */;
}

/*
 * wiggling light pattern, to show life at startup.  very useful
 * for visually detecting watchdog or crash.
 */
void
blinky(void)
{
    byte i;
    for (i = 0; i < 6; i++) {
	delay(1000);
	if (i & 1) {
	    Led1_Off();
	} else {
	    Led1_On();
	}
    }
}

/*
 * tx_char - send a serial character
 */
void
tx_char(byte t)
{
    byte tmp;

    tmp = (tx_w + 1) & TX_QLEN_MASK;

#define WAIT_FOR_TX_SPACE 1 // tx_char() only called with ints enabled
#if WAIT_FOR_TX_SPACE
    // if we were to call tx_char() with interrupts disabled,
    // this condition will never be met.  in that case, we could
    // probably still make it work, but it's probably not worth
    // it.
    while (tmp == tx_r)
	/* spin for freespace */;
#else
    if (tmp == tx_r) {
	return;  // drop character
    }
#endif

    tx_queue[tx_w = tmp] = t;

    UCSRB |= bit(UDRIE);
}

#if DO_RECEIVE
/*
 * tx_char_hex - send a serial nibble, in ascii hex
 */
void
tx_char_hex(byte t)
{
    if (t <= 9)
	tx_char(t + '0');
    else
	tx_char(t + 'a' - 10);

}

void
tx_str_p(const prog_char *s)
{
    char c;

    while ( (c = pgm_read_byte(s++)) )
	tx_char(c);
}

/*
 *  tx_hexword - send up to a word as hex
 */
void
tx_hexword(word v)
{
    tx_char_hex((v >> 12) & 0xf);
    tx_char_hex((v >>  8) & 0xf);
    tx_char_hex((v >>  4) & 0xf);
    tx_char_hex((v >>  0) & 0xf);
}
#endif

/*
 * tx_word - send 16 bits, little-endian, optionally in ascii
 */
void
tx_word(word t)
{
#if DO_RECEIVE
    if (ascii) {
	tx_hexword(t);
	tx_str_p(crnl_s);
	return;
    }
#endif
    tx_char(t & 0xff);
    tx_char((t >> 8) & 0xff);
}


/*
 * timer1 overflow interrupt handler.
 * if we hit the overflow without getting a transition on the IR
 * line, then we're certainly "between" IR packets.  we save the
 * overflow indication until just before the next "real" pulse --
 * lirc wants to see it then (just before the "real" data),
 * rather than at the end.
 */
INTERRUPTIBLE_ISR(TIMER1_OVF_vect)
{
    byte tmp;
    if (IR_high())
	tmp = 0xff;  // high byte of eventual dummy pulselen
    else
	tmp = 0x7f;
    had_overflow = tmp;
}

/*
 * timer1 compare match interrupt handler.  this is simply a way
 * of turning off the "IR message received" LED sooner. 
 * otherwise we could do it in the overflow handler.  this has no
 * affect on the timing protocol, but doing it here makes the LED
 * behavior match the user's button presses a little more
 * closely.
 */
INTERRUPTIBLE_ISR(TIMER1_COMPA_vect)
{
    Led1_Off();
}

/*
 * input capture event handler
 * the "event" is a transition on the IR line.  we save the
 * captured count, and restart the timer from zero again.
 */
INTERRUPTIBLE_ISR(TIMER1_CAPT_vect)
{

    // read the event
    pulse_length = ICR1;
    // and save the new state of the IR line.
    pulse_is_high = IR_high();

    // restart the timer
    TCNT1 = 0;


    // change detection edge, and clear interrupt flag -- it's
    // set as result of detection edge change
    cli();
    TCCR1B ^= bit(ICES1);
    TIFR &= ~bit(ICF1);
    sei();

}

/*
 * uart transmit interrupt handler.
 */
ISR(USART_UDRE_vect)
{
    UCSRB &= ~bit(UDRIE);
    sei();

    // if data in queue, send it, else disable the interrupt
    if (tx_r != tx_w) {
	tx_r = (tx_r + 1) & TX_QLEN_MASK;
	UDR = tx_queue[tx_r];
	UCSRB |= bit(UDRIE);
    }
}

/*
 * pc_int - pin change interrupt.
 * this is purely and simply an inversion function -- we want to
 * present an inverted copy of the USART's TX output to the host's
 * serial port.  so we watch for changes on one pin, and update the other.
 * since this operation needs minimal latency, we reenable interrupts
 * wherever possible in the other interrupt handlers.
 */
ISR(PCINT_vect)
{
    if (PINB & bit(TX_INVERT_IN))
	PORTB &= ~bit(TX_INVERT_OUT);
    else
	PORTB |= bit(TX_INVERT_OUT);
}


#if DO_RECEIVE
/*
 * uart_rx - Uart Rx interrupt handler.
 * this command-line interface was used early in development.
 */
ISR(USART_RX_vect)
{
    byte c;

    Led2_Flip();

    c = UDR;

    switch (c) {
    case 'h':
	tx_str_p(usage_s);
	break;
    case 'a':
	ascii = 1;
	tx_str_p(ascii_s);
	break;
	;;
    case 'b':
	ascii = 0;
	tx_str_p(binary_s);
	break;
    case 'B':		// silent
	ascii = 0;
	return;
    case 'i':
	tx_char(IR_high() ? '1':'0');
	break;
    case 'm':
	tx_hexword(mcusr_mirror);	/* reset reason, etc */
	break;
    case 'v':
	tx_str_p(version_s);	/* version */
	break;
    default:
	tx_str_p(error_s);
	break;
    }
    tx_str_p(crnl_s);
}
#endif

/*
 *  we want the timer overflow to be (a lot) longer than the
 *  longest interval we need to record using ICR1, which is
 *  something like .25 sec.  we also need to convert from
 *  timer count intervals to 16384'ths of a second.
 *     
 *  14.7456Mhz
 *     14745600 counts/sec, prescaled by 256, gives 57600 counts/sec,
 *     or 17.36usec/count, times 65536 gives overflow at 1.14sec.  good.
 *     want 16384'ths:  scale count by 16384 / 57600. ( 4096 / 14400 ) 
 *
 *  12.0000Mhz
 *     12000000 counts/sec, prescaled by 256, gives 46875 counts/sec,
 *     or 21.33usec/count, times 65536 gives overflow at 1.40sec.  good.
 *     want 16384'ths:  scale count by 16384 / 46875. ( 4096 / 11719 )
 *
 *  11.0592
 *     11059200 counts/sec, prescaled by 256, gives 43200 counts/sec,
 *     or 23.15usec/count, times 65536 gives overflow at 1.51sec.  good.
 *     want 16384'ths:  scale count by 16384 / 43200. ( 4096 / 10800 )
 *
 *  8.0000Mhz
 *     8000000 counts/sec, prescaled by 256, gives 31250 counts/sec,
 *     or 32usec/count, times 65536 gives overflow at 2.09sec.  good.
 *     want 16384'ths:  scale count by 16384 / 31250. ( 4096 / 7812 )
 *
 *  3.6864Mhz
 *     3686400/256 --> 14400, so scale by 16384 / 14400 --> 4096 / 3600
 *
 */

#define scale_denom(fosc) ((fosc / 256) / 4)

void
emit_pulse_data(void)
{
    word len;
    byte high;
    byte overflow;

    while (pulse_length) {
	cli();
	len = pulse_length;
	high = pulse_is_high;
	overflow = had_overflow;

	pulse_length = had_overflow = 0;

	sei();

	Led1_On();
	if (overflow) {
	    // if we had an overflow, then the current pulse_length
	    // is meaningless -- it's just the last remnant of a
	    // long gap.  just send the previously recorded
	    // overflow value to indicate that gap.  this is
	    // effectively the start of a "packet".
	    tx_word((overflow << 8) | 0xff);
	} else {
	    uint32_t l;

	    /* do long arithmetic.  expensive, but we have time. */

	    l = (uint32_t)len * 4096 / scale_denom(FOSC);

	    if (l > 0x7fff)	// limit range.
		len = 0x7fff;
	    else
		len = l;

	    if (len == 0)	// pulse length never zero.
		len++;

	    if (!high)	// report the state we transitioned out of
		len |= 0x8000;

	    tx_word(len);
	}
    }
}

/*
 * main -
 */
int
main(void)
{
    hw_init();
    blinky();

    wdt_enable(WDTO_4S);

    sei();

    if (do_debug()) {
	/* enter a loop transmitting a perfect square wave.  useful
	 * for debugging the TX inversion, and baud rate stability.
	 */
	for(;;) {
	    wdt_reset();
	    tx_char('U');
	}
	/* not reached */
    }

    for(;;) {
	wdt_reset();
	cli();
	if (!pulse_length) {
	    // only sleep if there's no pulse data to emit
	    // (see <sleep.h> for explanation of this snippet)
	    sleep_enable();
	    sei();
	    sleep_cpu();
	    sleep_disable();
	}
	sei();
	emit_pulse_data();
    }
    /* not reached */

}


