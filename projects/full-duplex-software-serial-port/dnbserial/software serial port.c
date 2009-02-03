////////////////////////////////////////////////////////////////////////////////////////////
//
// Software uart routines
//
// we need two serial lines, and there's only one usart on the M32
// so we implement another in software
//
// we set up a clock with a timeout of 883 cycles - four times baud rate at 2400 baud - 
// and use it to sample an input line and clock an output line
// we also use it to maintain a 20mS tick for timouts and such
//
///////////////////////////////////////////////////////////////////////////////////////////

#define	uarttx PD3
#define uartrx PD2				// will be PD2,3 to talk to the laptop

// clock times
static volatile uint8_t ticks;			// 192 of these make 1/50th of a second
static volatile uint8_t fiftieths;
static volatile uint8_t seconds;
static volatile uint8_t minutes;
static volatile uint8_t hours;			// we don't keep time of day, just time from start

// software uart registers
static uint8_t	uart_rxd;		// the byte being received
static uint8_t	uart_txd;		// the byte being transmitted
static uint8_t	uart_txtick;	// tick count for the transmitter
static uint8_t	uart_rxtick;	// tick count for the receiver
static uint8_t	uart_txbit;		// bit count for tx
static uint8_t	uart_rxbit;		// bit count for rx
static uint8_t	uart_status;	// uart status
static uint8_t next_read_2;
static uint8_t next_write_2;	// the pointers to the receive2 buffer

#define BUFFERSIZE 32
static unsigned char buffer2[BUFFERSIZE];
								// circular buffer for receiver 2

// status bits
#define txbusy 0				// set if a byte is in transmission
#define rxbusy 1				// set if a byte is being received
#define rxframe 2				// set if the stop byte isn't a one

SIGNAL (SIG_OUTPUT_COMPARE1A)
{
	// the interrupt signal on a compare for the timer 1
	
	// we arrive here four times per baud rate
	// we first send the output signal, if there's anything to send,
	// since it needs to be somewhere close to accurate...
	// then we read the incoming signal, if there is one,
	// and finally we update the nearly-real-time clock
	// (we never display the clock but it timestamps the output data)
	
	if (uart_status & _BV(txbusy))
	{
		// we're transmitting something
		// increment the tick - we only do things every four ticks
		uart_txtick++;
		if (uart_txtick == 4)
		{
			// okay, we've work to do
			uart_txtick = 0;
	
			// is it the start bit?
			if (uart_txbit == 0)
			{
				// yes...
				PORTD &= ~(_BV(uarttx));		// clear the start bit output
				uart_txbit++;
			}
			else
			{
				if (uart_txbit != 9)
				{
					// deal with the data bits
					if (uart_txd & 1)		// low bit set?
						PORTD |= _BV(uarttx);	// then set the data stream bit
					else
						PORTD &= ~(_BV(uarttx));
												// or clear, as required
					uart_txbit++;				// increment the bit count
					
					// and shift the data right
					uart_txd /= 2;
				}
				else
				{
					// deal with the stop bit
					PORTD |= _BV(uarttx);
					uart_txbit++;
				}
			}
			
			// and finally, if txbit is more than 9, we've done
			if (uart_txbit > 9)
			{
				uart_txbit = 0;					// clear the bit counter
				uart_status &= ~(_BV(txbusy)); // and the busy status
			}
		}
	}
	
	// we may be receiving something
	// if we're *not* yet receiving, we check every clock tick to see if the input
	// line has gone into a stop bit
	// if it has, we wait for half a bit period and then sample every four ticks
	// to put together the rx data

uint8_t uart_bitpattern[9] = {0,1,2,4,8,16,32,64,128};

	if ((uart_status & _BV(rxbusy)) == 0)
	{
		// we're idling
		// check to see if there's a start
		if ((PIND & _BV(uartrx)) == 0)
		{
			// we found a start bit!
			// set the tick count to 2, so we get the sample near the middle of the bit
			uart_rxtick = 2;
			// and flag that we're now busy
			uart_status |= _BV(rxbusy);
			// we're expecting the start bit now...
			uart_rxbit = 0;
			// and carry on with life
		}
	}
	else
	{
		// we're receiving something
		// inc the tick count
		uart_rxtick++;
		if (uart_rxtick == 4)
		{
			// we only sample when the tick = 0
			uart_rxtick = 0;
			
			// what we do depends on the bit count
			// 0 = start,
			// 1-8 = data
			// 9 = stop
			if (uart_rxbit == 0)
			{
				// start bit
				// it had better be 0 or it was a line glitch
				if ((PIND & _BV(uartrx)) == 0)
				{
					// it's a real start bit (probably) so deal with it
					// next bit will be data
					uart_rxbit ++;
					uart_rxd = 0;
				}
				else
				{
					// bad start bit, flag us back as not busy
					uart_status |= ~(_BV(rxbusy));
				}
			}
			else
			{
				if (uart_rxbit < 9)
				{
					// data bit
					// if the data bit is a one, we add the bit value to the rxd value
					if (PIND & _BV(uartrx)) 
					{
						uart_rxd += uart_bitpattern[uart_rxbit];
					}
					uart_rxbit ++;
				}
				else
				{
					if (uart_rxbit == 9)
					{
					// stop bit
						// we're going to assume it's a valid bit, though we could check for
						// framing error here, and simply use this bit to wait for the first
						// stop bit period
						uart_rxbit++;
					}
					else
					{
						// finished, ready to start again
						uart_status &= ~(_BV(rxbusy));
						// store the data into the buffer
						buffer2[next_write_2++] = uart_rxd;
						if (next_write_2 == BUFFERSIZE)
							next_write_2 = 0;
					}
				}
			}
		}
	}
	
	// increment the clock
	ticks++;
	if (ticks == 192)
	{
		ticks = 0;
		fiftieths++;
		if (fiftieths == 40)
		{
			// turn led on
			PORTD |= 0x80;
		}
		if (fiftieths == 50)
		{
			// turn led off
			PORTD &= 0x7f;
			
			fiftieths = 0;
			seconds++;
			if (seconds == 60)
			{
				seconds = 0;
				minutes ++;
				if (minutes == 60)
				{
					minutes = 0;
					hours++;
				}
			}
		}
	}
	


}

void ser2_init (void)
{
	// we wake up the timer, preset the clock and uart variables, and enable the ocr1a interrupt
	ticks = 0;
	fiftieths = 0;
	seconds = 0;
	minutes = 0;
	hours = 0;

	TCCR1A = 0;					// ctc, all pins normal
	TCCR1B = 9;					// ctc, no prescaler
	
	// 833 = 0x341
	
	OCR1AH = (3);			
	OCR1AL = (0x41);			// preset compare counter to 833

	timer_enable_int(_BV(OCIE1A));
								// allow interrupts on output mask a
	
	next_read_2 = 0;
	next_write_2 = 0;			// set up the buffer pointers
	
	uart_status = 0;			// nothing happening either tx or rx
	
	DDRD |= _BV(uarttx);		// and set the input and output pins
	PORTD = _BV(uarttx);		// we're not using port d for anything else and
								// the usart overrides the pin directions anyway
}

void ser2_out (char ch)
{
	// output the character to the software serial port, waiting until the output
	// buffer is available
	
	while ((uart_status & _BV(txbusy)) == 1);		// wait...
	{
		// once the system isn't busy...
		// load the register
		uart_txd = ch;
		
		// and tell it there's something to send
		uart_status |= _BV(txbusy);
	}
}

void ser2_print (char * text)
{
	// print a string of characters to the soft uart
	for (int q=0; q<strlen(text); q++)
		ser2_out(text[q]);
}

char ser2_ischar (void)
{
	// returns true if there is a character available from the soft uart
	// false if there is nothing waiting
	// call ser2_in after this to retrieve the data
	
	if (next_read_2 != next_write_2)
		return 1;
	else
		return 0;
}

char ser2_in (void)
{
	// return a character from the soft uart
	// do *not* call without first testing with ser2_ischar as it modifies the
	// next_read pointer
char ch;
	ch = buffer2[next_read_2++];
	if (next_read_2 == BUFFERSIZE)
		next_read_2 = 0;
	return ch;
}
