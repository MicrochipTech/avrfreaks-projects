//************************************************************************
// int_handler.c
//			All the interrupt service routines are in this module.
//
//			(C) 2007 George Scolaro
//************************************************************************


#include <avr/interrupt.h>

#include "main.h"
#include "avrio.h"
#include "ax25.h"
#include "nmea.h"

//*******************************************************************
// This interrupt routine performs two tasks depending on operating mode.
//
// For TRANSMIT mode: it runs at the bit time (1200 baud) and if bits are
// in the tx_buff it sets the tx_tone to the appropriate tone (1200Hz or
// 2200Hz).
//
//
// If not transmitting, only the "ticker" flag is set.
//

SIGNAL(SIG_OUTPUT_COMPARE0A)

{
	static uint8_t icount;

	// If in transmit 'mode', process any remaining bits and toggle
	// tx_tone appropriately (for zero bits only, due to NRZI).
	// tx_pack buffer is already bit stuffed, with crc etc etc.

	ticker++;						// another tic gone by
	icount++;						// internal counter

	if (delay_ticker < 254)
		delay_ticker++;

	// check if either switch clicked down

	if ((tx_mode && icount > 12) || (!tx_mode && icount > 96)) {
		icount = 0;

		if (!(PIND & SWA) && switcha == 0)	// button A down?
			switcha = 1;					// yes, it is down

		if (!(PIND & SWB) && switchb == 0)	// button B down?
			switchb = 1;					// yes, it is down


		if ((PIND & SWA) && switcha == 2)	// button A released & processed?
			switcha = 0;					// yes, we can clear it now

		if ((PIND & SWB) && switchb == 2)	// button B released & processed?
			switchb = 0;					// yes, we can clear it now
	}

	if (tx_mode) {		
		if (tx_bit_count > 0) {			// still bits to transmit
			if (!((tx_pack[tx_ptr] >> tx_bit_cnt) & 1)) {
				tx_tone = (tx_tone == MARK)? SPACE : MARK; // NRZI toggle
				tone_change = 1;
			}
			tx_bit_cnt = (tx_bit_cnt+1) & 0x7;	// wrap count to byte
			if (!tx_bit_cnt) {					// if wrapped

				// Special, for the first byte (pre-preamble of zeroes)
				// we repeat it tx_delay_cnt times to give the receive
				// PLL time to lock into the sequence and for the transceiver
				// to kick in after the PPT was activated.

				if (tx_ptr == 0) {			// send pre-preamble tx_delay times
					if (!tx_delay_cnt--) {
						tx_bit_count -= 8;
						tx_delay_cnt = NUM_FLAGS;	// # of FLAGs to send
						tx_ptr++;			// move to next byte
					}
				}

				// Special, for the second byte (FLAG preamble) we repeat
				// it several times to allow the receiver to sync to the
				// FLAG sequence.

				else if (tx_ptr == 1) {		// send FLAG a couple of times
					if (!tx_delay_cnt--) {
						tx_bit_count -= 8;
						tx_ptr++;			// move to next byte
					}
				}
				else {
					tx_ptr++;				// move to next byte
					tx_bit_count--;
				}
			}
			else if (tx_ptr > 1)			// decrement bit count if data
				tx_bit_count--;
		}
		else {
			tx_packet_done = 1;				// transmission of packet complete
			PORTC &= ~_BV(PTT);				// PTT = OFF
		}
	}
}

//*******************************************************************
// This interrupt routine handles the counter2 overflow interrupts.
//  The AFSK sine wave is generated via a 4 resistor D/A converter using
//  a digital approximation in the sine table.


SIGNAL(SIG_OUTPUT_COMPARE2A)
{
	static uint8_t sine[16] = {
		0xe, 0x5, 0xb, 0x7, 0xf, 0x7, 0xb, 0x5,
		0x1, 0xa, 0x4, 0x8, 0x0, 0x8, 0x4, 0xa};
	static uint8_t sine_index;		// persistent variable

	if (tx_mode) {
		++sine_index;				// increment sine table index
		sine_index &= 15;			// wrap at 15
		PORTA = (PORTA & 0xf0) | sine[sine_index];	// get sinewave value and send to D/A

		// If a change of tone is required, do it in the interrupt routine
		// to be sure that it is done AFTER TCNT2 has already wrapped back
		// to zero. This ensures consistent D/A timing.

		if (tone_change) {			// switched MARK/SPACE?
			OCR2A = tx_tone;		//  yes, change tone frequency
			tone_change = 0;
		}
	}
	else
		OCR2A = 0xff;				// not running, might as well save ints
}

//*******************************************************************
// This interrupt routine processes the USART RX interrupts.
// Reads the data in the RX buffer and writes it to the i_buf.

SIGNAL(USART0_RX_vect)
{
	uint8_t data;

	data = UDR0;					// read character in
	if (gps_eol) {					// while eol found, do nothing
		return;
	}
	if (gps_start && data != '$')
		return;

	gps_start = 0;					// found '$', get packet
	i_buf0[i_wrptr0] = data;  		// store data into buffer
	if (++i_wrptr0 == IBUF0_SIZE)	// if write pointer reached buffer end
		i_wrptr0 = 0;				//  wrap back to zero
	if (data == CR_CHAR) {			// end of line, freeze for now
		gps_eol = 1;
		i_wrptr0 = 0;				//  wrap back to zero
	}
}

//*******************************************************************
// This interrupt routine processes USART TX interrupts.
// Transfers data from the output circular buffer to the TX register.

SIGNAL(USART0_TX_vect)
{
	if (o_rdptr0 != o_wrptr0)	{		// buffer is not empty
		if (++o_rdptr0 == OBUF0_SIZE)	// if read pointer at buffer end
			o_rdptr0 = 0;				//  wrap back to zero
		UDR0 = o_buf0[o_rdptr0];		// output data to TX buffer
	}
}

//*******************************************************************
// This interrupt routine processes the USART RX interrupts.
// Reads the data in the RX buffer and writes it to the i_buf.

SIGNAL(USART1_RX_vect)
{
	uint8_t data;

	data = UDR1;					// read character in
	if (pc_eol) {					// while eol found, do nothing
		return;
	}
	if (pc_start && data != '$')
		return;

	pc_start = 0;					// found '$', get packet
	i_buf1[i_wrptr1] = data;	 	// store data into buffer
	if (++i_wrptr1 == IBUF1_SIZE)	// if write pointer reached buffer end
		i_wrptr1 = 0;				//  wrap back to zero
	if (data == CR_CHAR) {			// end of line, freeze for now
		pc_eol = 1;
		i_wrptr1 = 0;				//  wrap back to zero
	}
}

//*******************************************************************
// This interrupt routine processes USART TX interrupts.
// Transfers data from the output circular buffer to the TX register.

SIGNAL(USART1_TX_vect)
{
	if (o_rdptr1 != o_wrptr1)	{		// buffer is not empty
		if (++o_rdptr1 == OBUF1_SIZE)	// if read pointer at buffer end
			o_rdptr1 = 0;				//  wrap back to zero
		UDR1 = o_buf1[o_rdptr1];		// output data to TX buffer
	}
}

//*******************************************************************
// This interrupt routine handles the comparator interrupt.
// Interrupts are disabled during TCNT1 access to prevent 16-bit corruption.
// A 1200 tone is 768 counts and 2200 tone is 419 at 14.7456 MHz.
//

SIGNAL(SIG_COMPARATOR)
{
	uint16_t count;
	static uint8_t last;

	if (!(msg_end))	{					// If not waiting to process a message
		count = TCNT1;					// Read counts since last interrupt

		if (count > 542) {				// Below 1700 Hz?	(542 @ 14.7456 MHZ)
			TCNT1 = 0;					// Clear the counter
			if (last == SPACE) {		// If the last tone detected was a SPACE
				rxtoggled = 1;			// Toggle detected
				dcd = count;			// Set "random" timeout
			}
			last = MARK;				// MARK is detected
		}
		else if (count > 100) {			// Ignore a frequency above 4.6 kHz
			TCNT1 = 0;					// Clear the counter
			if (last == MARK) {			// If the last tone detected was a SPACE
				rxtoggled = 1;			// Toggle detected
				dcd = count;			// Set "random" timeout
			}
			last = SPACE;				// SPACE is detected
		}
	}
}
