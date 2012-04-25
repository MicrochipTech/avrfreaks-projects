//************************************************************************
// File:		AX25.C
//
//			(C) 2007 George Scolaro
//************************************************************************



#include <avr/io.h>

#include "main.h"
#include "ax25.h"
#include "nmea.h"

void tx_pack_bld(uint8_t data);

// Home address
//  37deg 21.88 North
// 121deg 59.88 West
// Speed = 0, course = 0 degrees
// Altitude = 50 feet


uint8_t mic_dst_addr[7] =	// Mic-E destination field = Latitude + TO_SSID
 {'S', 'W', '2', 'Q', 'X', 'W', 0x30};	// reserved bits SET, SSID = 0 ****

uint8_t mic_src_addr[7]  = 
 {'K', 'I', '6', 'I', 'B', 'X', 0x40 | 0x30};	// reserved bits set

uint8_t mic_path1[6] = 
 {'W', 'I', 'D', 'E', '2', ' '}; 

uint8_t mic_long[4] =		// Mic-E longitude field
 {0x60, '1', 'W', 'r'};

uint8_t mic_sp_crs[3] =		// Mic-E speed and course field
 {'l', ' ', 0x1c};

uint8_t mic_symbol[2] =		// Mic-E symbol type
 {'j', '/'};

uint8_t mic_altitude[4] =	// Mic-E altitude
 {34, 52, 38, '}'};

uint8_t flag_mode = 0;		// bit stuffing & crc not done for Flag byte

//*******************************************************************
// This function builds a Mic-E formatted position packet in the bit
// stream buffer. This allows the tone generator function to run fully
// interrupt driven, freeing up the mainline to check on NMEA sentences
// and do other house keeping.
//

extern void ax25_mice_packet(void)
{
	uint8_t i;
	uint8_t crchi;

	for (i = 0; i < sizeof(tx_pack); i++)
		tx_pack[i] = 0;					// pre-clear buffer

	tx_bit_cnt = tx_ptr = 0;

	ax25_txbyte(0);						// put out one pre-preamble byte

	crc = 0xffff;						// Seed the CRC16 register

	flag_mode = 1;						// flag, don't stuff, don't crc
	ax25_txbyte(X25_FLAG);				// put out one FLAG byte
	flag_mode = 0;						// end of flags, enable CRC/stuffing

	for (i = 0; i < sizeof(mic_dst_addr); i++)	// send Mic-E Latitude + SSID
		ax25_txbyte(mic_dst_addr[i] << 1);

	for (i = 0; i < sizeof(mic_src_addr); i++)	// send Mic-E Source address
		ax25_txbyte(mic_src_addr[i] << 1);

	for (i = 0; i < sizeof(mic_path1); i++)	// send Mic-E Digipeater
		ax25_txbyte(mic_path1[i] << 1);
	ax25_txbyte(0x63);					// WIDE2-1 & LSB set (string terminator)


	ax25_txbyte(AX25_UI);				// APRS UI-frame (0x3)
	ax25_txbyte(AX25_NL3);				// no layer 3 protocol(0xf0)

	for (i = 0; i < sizeof(mic_long); i++)		// send Mic-E Longitude
		ax25_txbyte(mic_long[i]);

	for (i = 0; i < sizeof(mic_sp_crs); i++)	// send Mic-E speed/course
		ax25_txbyte(mic_sp_crs[i]);

	for (i = 0; i < sizeof(mic_symbol); i++)	// send Mic-E symbol type
		ax25_txbyte(mic_symbol[i]);

	if (alt_valid)
		for (i = 0; i < sizeof(mic_altitude); i++)	// send Mic-E altitude
			ax25_txbyte(mic_altitude[i]);

// output the tail end of the packet: CRC16 and FLAG

	crchi = (uint8_t)(crc >> 8) ^ 0xff;	// copy -> ax25_txbyte alters crc

	ax25_txbyte((uint8_t)crc ^ 0xff);	// tx low byte of CRC16
	ax25_txbyte(crchi);					// tx high byte of CRC16
	flag_mode = 1;
	ax25_txbyte(X25_FLAG);				// tx flag to end the packet
	ax25_txbyte(X25_FLAG);				// tx flag to end the packet
	flag_mode = 0;

	tx_bit_count = ((uint16_t)tx_ptr << 3) + tx_bit_cnt;
}

//*******************************************************************
// This function processes a txbyte, computes CRC16-8408 and puts the
// the data (with stuffing as needed) into the tx_pack buffer ready for
// the interrupt routines to then generate the tones and output the bits
// at the specified baudrate.

extern void ax25_txbyte(uint8_t txbyte)
{
	uint8_t i;
	uint8_t shift_byte;
	static uint8_t seq_ones = 0;			// this resets every X25_FLAG time!

	shift_byte = txbyte;					// 8 bits that will shift right

	for (i = 0; i < 8; i++) {				// Process 8 bits
		if (flag_mode)						// are we in flag mode?
			seq_ones = 0;					// yes, no bit stuffing required
		else {								// data, compute running CRC
			if (((uint8_t) crc ^ shift_byte) & 0x1)	{	// If lsb set
				crc >>= 1;					// Shift CRC right
				crc ^= 0x8408;				// xor in polynomial
			}
			else
				crc >>= 1;					// Shift CRC right
		}

		if (!(shift_byte & 0x1)) {			// lsb low?
			seq_ones = 0;					// reset sequential ones counter
			tx_pack_bld(0);					// store 0 bit
		}
		else {
			tx_pack_bld(1);					// store 1 bit
			if (++seq_ones == 5) {			// reach 5 one's in a row?
				seq_ones = 0;				// reset sequential ones counter
				tx_pack_bld(0);				// stuff 0 bit
			}
		}
		shift_byte >>= 1;					// Shift the reference byte right
	}
}

//*******************************************************************
// Builds a bit string (with zero stuffing) buffer for output.
// Special aspect, 1st byte = 0, 2nd byte = FLAG.
// It is the Interrupt driver's responsibility to repeat the 1st and
// 2nd bytes as many times as neccessary for the pre-preamble and
// preamble before moving on to the rest of the packet.

void tx_pack_bld(uint8_t data)
{
	if (tx_ptr < sizeof(tx_pack)) {			// only if buffer not full
		tx_pack[tx_ptr] |= data << tx_bit_cnt;	// insert bit LSB first
		tx_bit_cnt = (tx_bit_cnt+1) & 0x7;	// wrap count to byte
		if (!tx_bit_cnt)					// if wrapped
			tx_ptr++;						// move to next byte
	}
}



