#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <avr/io.h>
#include <avr/delay.h>
#include <avr/signal.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#define SYSCLK 3686400
#define BAUDRATE 19200

#define BUFSZ 64
#define NO_CHAR 0xff

#define BSET(t, b) { (t) |= _BV(b); }
#define BCLR(t, b) { (t) &= ~_BV(b); }

volatile char obuf[BUFSZ];		// output circular buffer
volatile char *cin, *cout;		// store characters at cin, get from cout

volatile unsigned htime = 0;	// high values for timer (beyond 16 bits)

volatile unsigned long new_tm = 0;	// new timestamp capture buffer
volatile int timef = 0;				// new_tm was is valid

// enque a character
// drops characters on overflow
void cenq(char c) {
	char *np = (char *) cin + 1;
	if (np > &obuf[BUFSZ-1]) np = (char *) &obuf[0];
	
	if (np == cout) return;	// overflow
	
	*cin = c;
	cin = np;
}

// deque a character
// returns NO_CHAR on underflow
char cdeq(void) {
	char ret = NO_CHAR;
	
	if (cin != cout) { 
		ret = *cout++;
		if (cout > &obuf[BUFSZ-1]) cout = &obuf[0];
	}
	return ret;
}

// 'UART wants data' handler
SIGNAL(SIG_UART_DATA) {
	char c = cdeq();
	if (c != NO_CHAR)
		UDR = c;				// real char, stuff it into UART
	else
		BSET(UCR, UDRIE);      // empty que, disable interrupt
}

int uart_put(char c) {
	if (c == '\n') cenq('\r');
	cenq(c);
	BSET(UCR, UDRIE);			// turn on interrupts on empty UART
	return 0;
}

inline void dream(void) {
	sleep_mode();
}

void init_io(void) {

	// set up serial port output
	UBRR = SYSCLK / (16UL * BAUDRATE) - 1;
	BSET(UCR, TXEN);

	cin = cout = obuf;				// init buffer to empty

#ifdef USE_STDIO
	fdevopen(uart_put, NULL, 0);
#endif
	
	// set up data port
	BSET(DDRA, DDA0);		// output
	BSET(PORTA, PA0);		// initially hold high
	
	// set up the timer
	BSET(TCCR1B, CS11);		// CK/8
	BSET(TIMSK, TOIE1);		// interrupt on overflow
	BSET(TIMSK, TICIE1);	// interrupt on input capure (falling edge)
	
	// set up sleep mode
	BCLR(ACSR, ACIE);
	BSET(ACSR, ACD);
	set_sleep_mode(SLEEP_MODE_IDLE);

	// enable interrupts
	sei();
}

// reset timers and poke the external circuit
void poke_ext(void) {

	htime = 0;
	TCNT1 = 0;
	
	BCLR(PORTA, PA0);		// poke circuit low
	BSET(PORTA, PA0);		// reset the line
}

// timer overflow handler
SIGNAL(SIG_OVERFLOW1) {
	htime++;
}

// external circuit signals timer sample captured
SIGNAL(SIG_INPUT_CAPTURE1) {
	new_tm = (long) htime << 16;
	new_tm |= ICR1;
	timef++;		// set time captured flag
}

#ifndef USE_STDIO

// output a number w/o STDIO
void nput(long v) {
	char buf[10];
	int np = 0;
	
	while (v) {
		buf[np++] = '0' + v % 10;
		v /= 10;
	}
	if (np == 0) buf[np++] = '0';
	while (--np >= 0) uart_put(buf[np]);
}

// output a string w/o STDIO
void cput(char *s) {
	while(*s) uart_put(*s++);
}

#endif // !defined(USE_STDIO)
		
// process timer sample
void proc_sample(void) {
	static long otm = 0;
	static long ltm = 0;
	static long nskip = 0;
	long ntm = new_tm;
	long pov;

	new_tm = 0;
	ltm += ntm;
	if (ntm == 0) ltm++;
	
	if (ltm >> 14) {	// crude serial output damper
		pov = ntm * nskip;
		cli();
#ifdef USE_STDIO
		printf("%ld: %ld (%ld)\n", nskip, ntm, pov);
#else // !defined(USE_STDIO)
		nput(nskip);
		cput(": ");
		nput(ntm);
		cput("(");
		nput(pov);
		cput(")\n");
#endif // defined(USE_STDIO)
		sei();
		ltm = 0;
		nskip = 0;
	} else {
		nskip++;
	}
	
	otm = ntm;
	
	poke_ext();		// start the next sample
}
	

int main (void) {
	init_io();
	
	poke_ext();
	
	for (;;) {
		if (timef) {		// got interrupter by a timer
			timef = 0;
			proc_sample();
		} else
			dream();
	}
	return 0;
}
