#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "p_defines.h"
#include "JG_Modbus.h"
#include "Modbus_uart.h"

//#define ActTEMP(a) (uint16_t((5000UL * a) / 1024))

/* This structure reduces the number of params in functions and so
 * optimizes the speed of execution (~ 37%). */
typedef struct {
	int protocol;
	int slave;
	int function;
	int address;
	int nb;
	int t_id;
} sft_t;

extern volatile uint16_t actProfileData[MAX_WordAddress];
extern volatile uint8_t  systemBits[MaxActionBytes];
extern tMBChangedBit changedBit;


/* Table of CRC values for high-order byte */
static uint8_t table_crc_hi[] = { 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80,
		0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80,
		0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80,
		0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81,
		0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80,
		0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80,
		0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80,
		0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80,
		0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80,
		0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80,
		0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80,
		0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81,
		0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80,
		0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81,
		0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
		0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81,
		0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80,
		0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80,
		0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80,
		0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81,
		0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80,
		0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40 };

/* Table of CRC values for low-order byte */
static uint8_t table_crc_lo[] = { 0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02,
		0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D,
		0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 0x08,
		0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF,
		0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16,
		0xD6, 0xD2, 0x12, 0x13, 0xD3, 0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31,
		0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34,
		0xF4, 0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 0x3B,
		0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A,
		0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25,
		0xE5, 0x27, 0xE7, 0xE6, 0x26, 0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20,
		0xE0, 0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7,
		0x67, 0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 0x6E,
		0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9,
		0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC,
		0x7C, 0xB4, 0x74, 0x75, 0xB5, 0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3,
		0x73, 0xB1, 0x71, 0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52,
		0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C, 0x5D,
		0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58,
		0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F,
		0x4F, 0x8D, 0x4D, 0x4C, 0x8C, 0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46,
		0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80, 0x40 };

/* Fast CRC */
static uint16_t crc16(uint8_t *buffer, uint16_t buffer_length) {
	uint8_t crc_hi = 0xFF; // high CRC byte initialized
	uint8_t crc_lo = 0xFF; // low CRC byte initialized
	unsigned int i; // will index into CRC lookup

	//pass through message buffer
	while (buffer_length--) {
		i = crc_hi ^ *buffer++; /* calculate the CRC  */
		crc_hi = crc_lo ^ table_crc_hi[i];
		crc_lo = table_crc_lo[i];
	}

	return (crc_hi << 8 | crc_lo);
}

/* If CRC is correct returns msg_length else returns INVALID_CRC */
static int check_crc16(uint8_t *msg, const int msg_length) {
	int ret;
	uint16_t crc_calc;
	uint16_t crc_received;

	crc_calc = crc16(msg, msg_length - 2);
	crc_received = (msg[msg_length - 2] << 8) | msg[msg_length - 1];

	// Check CRC of msg
	if (crc_calc == crc_received) {
		ret = msg_length;
	} else {
		ret = INVALID_CRC;
	}
	return ret;
}


/* Sends a query/response over a serial communication */
static int modbus_send(uint8_t *query, int query_length) {

	uint16_t s_crc;
	int i;

	s_crc = crc16(query, query_length);
	query[query_length++] = s_crc >> 8;
	query[query_length++] = s_crc & 0x00FF;
	for (i = 0; i <= query_length; i++) { //for (i = 0; i < query_length; i++) { original part ... modificed by MiBu
		uart_putc(query[i]);
	}
	return i;
}

// Builds a RTU response header 
static int build_response_basis_rtu(sft_t *sft, uint8_t *response) {

	response[0] = sft->slave;
	response[1] = sft->function;

	return PRESET_RESPONSE_LENGTH_RTU;
}

// Build the exception response 
static int response_exception(sft_t *sft, int exception_code, uint8_t *response) {
	int response_length;

	sft->function = sft->function + 0x80;
	response_length = build_response_basis_rtu(sft, response);

	// Positive exception code 
	response[response_length++] = -exception_code;

	return response_length;
}

int errorInFrame(uint8_t *frame, uint8_t frameLength) {
	int frameHasError = FALSE;
	if (check_crc16(frame, frameLength) != frameLength) {
		return INVALID_CRC;
	}

	if ((frame[0] != SLAVE_ID) && (frame[0] != MODBUS_BROADCAST_ADDRESS)) {
		return NOT_FOR_US;
	}

	switch (frame[1]) { // check function code and requested addresses	case
	case FC_FORCE_SINGLE_COIL:
	case FC_READ_COIL_STATUS:
		if ((frame[2] > MAX_BitAddresses) || (frame[2] > MAX_BitAddresses)) {
			frameHasError = ILLEGAL_DATA_ADDRESS;
		}
		break;
	case FC_READ_INPUT_STATUS:
		if ((frame[2] > MAX_BitAddresses) || (frame[2] > MAX_BitAddresses)) {
			frameHasError = ILLEGAL_DATA_ADDRESS;
		}
		break;
	case FC_READ_HOLDING_REGISTERS:
		if ((frame[3] > MAX_WordAddress) || (frame[2] > MAX_WordAddress)) {
			frameHasError = ILLEGAL_DATA_ADDRESS;
		}
		break;
	case FC_PRESET_SINGLE_REGISTER:
	case FC_READ_INPUT_REGISTERS:
		if ((frame[3] > MAX_WordAddress) || (frame[2] > MAX_WordAddress)) {
			frameHasError = ILLEGAL_DATA_ADDRESS;
		}

	case FC_FORCE_MULTIPLE_COILS:
		//to do implementing that part
	case FC_PRESET_MULTIPLE_REGISTERS:
		//to do implementing that part
	case FC_READ_EXCEPTION_STATUS:
		//to do implementing that part
	case FC_REPORT_SLAVE_ID:
		//to do implementing that part
	default:
		frameHasError = ILLEGAL_FUNCTION;
		break;
	}
	return frameHasError;
}


static int response_io_status(sft_t *sft, uint8_t *stab,
								uint8_t *response, int offset) {
	uint8_t sBit = sft->address;
	uint8_t startByte = (uint8_t)(sBit / 8);
	uint8_t nOBytes = (uint8_t)(sft->nb / 8) + ((sft->nb % 8)?1:0);
	uint8_t bytesToThread = nOBytes + ((sft->address % 8)?1:0);
	uint8_t ic;
	uint8_t destByte = 0, intermByte = 0;

	for (ic = 0; ic < bytesToThread; ic++){
		// getting the first byte where we have to get Bit's from
		destByte = stab[startByte + ic];
		// shift it to push out unnecessary bits
		// and place wanted Bits starting from the Low side of the Byte
		destByte = destByte >> (sBit % 8);
		// now we have 8 - (sBit % 8) bits to fill from the next byte
		intermByte = stab[startByte + ic + 1] << (8 - (sBit % 8));

		// now we have missing Bits left aligned in intermByte
		// stick them together
		destByte |= intermByte;
		response[offset++] = destByte;
	}

	return offset;
}

void modbus_slave_manage(const uint8_t *query, int query_length) {

	int offset = HEADER_LENGTH_RTU;
	int resp_length = 0;
	int data, i;
	sft_t sft;
	uint8_t response[MAX_MESSAGE_LENGTH];

	sft.slave = query[offset - 1];
	sft.function = query[offset];
	sft.address = (query[offset + 1] << 8) + query[offset + 2];

	if (sft.slave != SLAVE_ID && sft.slave != MODBUS_BROADCAST_ADDRESS) {
		// Ignores the query (not for me)
		return;
	}

	query_length -= CHECKSUM_LENGTH_RTU;

	switch (sft.function) {

	case FC_READ_COIL_STATUS:

		sft.nb = (query[offset + 3] << 8) + query[offset + 4];

		if ((sft.address / 8 + ((sft.nb)?1:0)) > MAX_BitAddresses) {
			resp_length = response_exception(&sft, ILLEGAL_DATA_ADDRESS,response);
		} else {
			resp_length = build_response_basis_rtu(&sft, response);
			// calculate amount of bytes to return
			response[resp_length++] = (sft.nb / 8) + ((sft.nb % 8) ? 1 : 0);
			// give him the bit pattern of coils requested

			resp_length = response_io_status(
					&sft,
					((uint8_t*)systemBits),
					response,
					resp_length);
		}
		break;

	case FC_READ_INPUT_STATUS:
		sft.nb = (query[offset + 3] << 8) + query[offset + 4];

		if ((sft.address / 8 + ((sft.nb)?1:0)) > MAX_BitAddresses) {
			resp_length = response_exception(&sft, ILLEGAL_DATA_ADDRESS,response);
		} else {
			resp_length = build_response_basis_rtu(&sft, response);
			// calculate amount of bytes to return
			response[resp_length++] = (sft.nb / 8) + ((sft.nb % 8) ? 1 : 0);
			// give him the bit pattern of coils requested

			resp_length = response_io_status(
						&sft,
						((uint8_t*)systemBits),
						response,
						resp_length);


		}
		break;
	case FC_READ_HOLDING_REGISTERS:

		sft.nb = (query[offset + 3] << 8) + query[offset + 4];

		if ((sft.address + sft.nb) >= MAX_WordAddress) {
			resp_length = response_exception(&sft, ILLEGAL_DATA_ADDRESS,response);
		} else {
			resp_length = build_response_basis_rtu(&sft, response);
			response[resp_length++] = sft.nb << 1;
			for (i = (sft.address); i < (sft.address) + sft.nb; i++) {
				response[resp_length++] = actProfileData[i] >> 8;
				response[resp_length++] = actProfileData[i] & 0xFF;
			}
		}
		break;
	case FC_READ_INPUT_REGISTERS:
		sft.nb = (query[offset + 3] << 8) + query[offset + 4];
		if ((sft.address + sft.nb) > MAX_WordAddress) {
			resp_length = response_exception(&sft, ILLEGAL_DATA_ADDRESS,response);
		} else {
			resp_length = build_response_basis_rtu(&sft, response);
			response[resp_length++] = sft.nb << 1;
		}
		break;
	case FC_FORCE_SINGLE_COIL:
		data = (query[offset + 3] << 8) + query[offset + 4];
		if (sft.address > MAX_BitAddresses) {
			resp_length = response_exception(&sft, ILLEGAL_DATA_ADDRESS,
					response);
		} else {

			if (data == 0xFF00 || data == 0x00) {
				changedBit.cBit = sft.address;

				if (data) {
					changedBit.cVal = TRUE;
					((uint8_t*)systemBits)[(uint8_t)(sft.address / 8)] |= (1 << (sft.address % 8));
				} else {
					changedBit.cVal = FALSE;
					((uint8_t*)systemBits)[(uint8_t)(sft.address / 8)] &= ~(1 << (sft.address % 8));
				}

				// In RTU mode, the CRC is computed and added
				//   to the query by modbus_send, the computed
				//   CRC will be same and optimization is
				//   (FIXME) possible here .
				memcpy(response, query, query_length);
				resp_length = query_length;
			} else {
				resp_length = response_exception(&sft, ILLEGAL_DATA_VALUE,
						response);
				changedBit.cBit = 99;
			}
		}
		break;
	case FC_PRESET_SINGLE_REGISTER:

		data = (query[offset + 3] << 8) + query[offset + 4];

		if (sft.address >= MAX_WordAddress) {

			resp_length = response_exception(&sft, ILLEGAL_DATA_ADDRESS,
					response);
		} else {

			if ((sft.address < MAX_WordAddress)){
				actProfileData[sft.address] = data;
				break;
			}

			memcpy(response, query, query_length);
			resp_length = query_length;
		}
		break;
	case FC_FORCE_MULTIPLE_COILS:

		resp_length = response_exception(&sft, ILLEGAL_DATA_ADDRESS, response);

		break;
	case FC_PRESET_MULTIPLE_REGISTERS:

		sft.nb = (query[offset + 3] << 8) + query[offset + 4];

		if ((sft.address + sft.nb) > MAX_WordAddress) {

			resp_length = response_exception(&sft, ILLEGAL_DATA_ADDRESS,
					response);
		} else {
			int i, j;

			for (i = sft.address, j = 6; i < sft.address + sft.nb; i++, j += 2) {
				// 6 and 7 = first value
				// FIXME give him the right arrays here
//				holdingRegs[i] = (query[offset + j] << 8) + query[offset + j + 1];
			}

			resp_length = build_response_basis_rtu(&sft, response);
			// 4 to copy the address (2) and the no. of registers
			memcpy(response + resp_length, query + resp_length, 4);
			resp_length += 4;
		}
		break;
	case FC_READ_EXCEPTION_STATUS:

		break;
	case FC_REPORT_SLAVE_ID:
		break;
	}
	modbus_send(response, resp_length);	
}

void modbus_processSlaveFrame(uint8_t *query, int query_length) {
	uint8_t cnt = 0;

	if (check_crc16(query, query_length) == query_length) {
		modbus_slave_manage(query, query_length);
	} else {
		for (cnt = 0; cnt < UART_RX_BUFFER_SIZE; cnt++) {
			query[cnt] = 0;
		}
	}	
}

