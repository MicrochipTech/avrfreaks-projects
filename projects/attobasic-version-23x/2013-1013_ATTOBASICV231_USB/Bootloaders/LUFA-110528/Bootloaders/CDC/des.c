//=============================================================================
// Copyright Atmel Corporation 2003. All Rights Reserved.
//
// File:			des.c
// Compiler:		IAR Atmel AVR C/EC++ Compiler
// Output Size:
// Based on work by:ØE, VU
// Created:			4-Feb-2003	JP (Atmel Finland)
// Modified:	
//
// Support Mail:	avr@atmel.com
//
// Description:		Please refer to Application Note Documentation for more
//					information.
//
//					For details on DES, please refer to the official FIPS 46-3
//					document:
//
//				http://csrc.nist.gov/publications/fips/fips46-3/fips46-3.pdf
//
//
// Other Info:		Since the numbers in the permutation-tables of the FIPS
//					document start at 1 instead of 0, the numbers in all
//					permutation tables in this file are 1 less. In other words:
//					The tables in this file are indexed with 0 as the first
//					bit.
//=============================================================================

#include <avr/pgmspace.h>
#include "des.h"
#include "bootldr.h"
#include "loader.h"

#if KEY_COUNT > 0

#include "deskeys.inc"

//=============================================================================
// S-boxes
//
// The original form of the S-box 1 is as follows.
//
// 14,  4, 13,  1,  2, 15, 11,  8,  3, 10,  6, 12,  5,  9,  0,  7,
//  0, 15,  7,  4, 14,  2, 13,  1, 10,  6, 12, 11,  9,  5,  3,  8,
//  4,  1, 14,  8, 13,  6,  2, 11, 15, 12,  9,  7,  3, 10,  5,  0,
// 15, 12,  8,  2,  4,  9,  1,  7,  5, 11,  3, 14, 10,  0,  6, 13
//
// Since every number only requires 4 bits, two numbers are stored
// as one byte.

PROGMEM unsigned char sTable[256] = {
  // S-box 1
  0xE4, 0xD1, 0x2F, 0xB8, 0x3A, 0x6C, 0x59, 0x07,
  0x0F, 0x74, 0xE2, 0xD1, 0xA6, 0xCB, 0x95, 0x38,
  0x41, 0xE8, 0xD6, 0x2B, 0xFC, 0x97, 0x3A, 0x50,
  0xFC, 0x82, 0x49, 0x17, 0x5B, 0x3E, 0xA0, 0x6D,

  // S-box 2
  0xF1, 0x8E, 0x6B, 0x34, 0x97, 0x2D, 0xC0, 0x5A,
  0x3D, 0x47, 0xF2, 0x8E, 0xC0, 0x1A, 0x69, 0xB5,
  0x0E, 0x7B, 0xA4, 0xD1, 0x58, 0xC6, 0x93, 0x2F,
  0xD8, 0xA1, 0x3F, 0x42, 0xB6, 0x7C, 0x05, 0xE9,

  // S-box 3
  0xA0, 0x9E, 0x63, 0xF5, 0x1D, 0xC7, 0xB4, 0x28,
  0xD7, 0x09, 0x34, 0x6A, 0x28, 0x5E, 0xCB, 0xF1,
  0xD6, 0x49, 0x8F, 0x30, 0xB1, 0x2C, 0x5A, 0xE7,
  0x1A, 0xD0, 0x69, 0x87, 0x4F, 0xE3, 0xB5, 0x2C,

  // S-box 4
  0x7D, 0xE3, 0x06, 0x9A, 0x12, 0x85, 0xBC, 0x4F,
  0xD8, 0xB5, 0x6F, 0x03, 0x47, 0x2C, 0x1A, 0xE9,
  0xA6, 0x90, 0xCB, 0x7D, 0xF1, 0x3E, 0x52, 0x84,
  0x3F, 0x06, 0xA1, 0xD8, 0x94, 0x5B, 0xC7, 0x2E,

  // S-box 5
  0x2C, 0x41, 0x7A, 0xB6, 0x85, 0x3F, 0xD0, 0xE9,
  0xEB, 0x2C, 0x47, 0xD1, 0x50, 0xFA, 0x39, 0x86,
  0x42, 0x1B, 0xAD, 0x78, 0xF9, 0xC5, 0x63, 0x0E,
  0xB8, 0xC7, 0x1E, 0x2D, 0x6F, 0x09, 0xA4, 0x53,

  // S-box 6
  0xC1, 0xAF, 0x92, 0x68, 0x0D, 0x34, 0xE7, 0x5B,
  0xAF, 0x42, 0x7C, 0x95, 0x61, 0xDE, 0x0B, 0x38,
  0x9E, 0xF5, 0x28, 0xC3, 0x70, 0x4A, 0x1D, 0xB6,
  0x43, 0x2C, 0x95, 0xFA, 0xBE, 0x17, 0x60, 0x8D,

  // S-box 7
  0x4B, 0x2E, 0xF0, 0x8D, 0x3C, 0x97, 0x5A, 0x61,
  0xD0, 0xB7, 0x49, 0x1A, 0xE3, 0x5C, 0x2F, 0x86,
  0x14, 0xBD, 0xC3, 0x7E, 0xAF, 0x68, 0x05, 0x92,
  0x6B, 0xD8, 0x14, 0xA7, 0x95, 0x0F, 0xE2, 0x3C,

  // S-box 8
  0xD2, 0x84, 0x6F, 0xB1, 0xA9, 0x3E, 0x50, 0xC7,
  0x1F, 0xD8, 0xA3, 0x74, 0xC5, 0x6B, 0x0E, 0x92,
  0x7B, 0x41, 0x9C, 0xE2, 0x06, 0xAD, 0xF3, 0x58,
  0x21, 0xE7, 0x4A, 0x8D, 0xFC, 0x90, 0x35, 0x6B };


//=============================================================================
// Selection Order

PROGMEM unsigned char sOrder[5] = {0, 5, 1, 2, 3};


//=============================================================================
// Initial Permutation bit selection logic
//
// Equivalent to:
//
// PROGMEM unsigned char ipTable[64] =
// {
//     57, 49, 41, 33, 25, 17,  9,  1,
//     59, 51, 43, 35, 27, 19, 11,  3,
//     61, 53, 45, 37, 29, 21, 13,  5,
//     63, 55, 47, 39, 31, 23, 15,  7,
//     56, 48, 40, 32, 24, 16,  8,  0,
//     58, 50, 42, 34, 26, 18, 10,  2,
//     60, 52, 44, 36, 28, 20, 12,  4,
//     62, 54, 46, 38, 30, 22, 14,  6
// };
//
// unsigned char ip(unsigned char i)
// {
//     return ipTable[i];
// }


unsigned char ip(unsigned char i)
{
	return 57 - ((i & 0x07) << 3) + ((i >> 2) & 0x06) - ((i >> 5) & 0x01);
}


//=============================================================================
// Inverse Initial Permutation bit selection logic
//
// Equivalent to:
//
// 
// PROGMEM unsigned char iipTable[64] =
// {
//     39,  7, 47, 15, 55, 23, 63, 31,
//     38,  6, 46, 14, 54, 22, 62, 30,
//     37,  5, 45, 13, 53, 21, 61, 29,
//     36,  4, 44, 12, 52, 20, 60, 28,
//     35,  3, 43, 11, 51, 19, 59, 27,
//     34,  2, 42, 10, 50, 18, 58, 26,
//     33,  1, 41,  9, 49, 17, 57, 25,
//     32,  0, 40,  8, 48, 16, 56, 24
// };
// 
// 
// unsigned char iip(unsigned char i)
// {
//     return iipTable[i];
// }

unsigned char iip(unsigned char i)
{
	return 39 - (i >> 3) + ((i & 0x06) << 2) - ((i & 0x01) << 5);
}


//=============================================================================
// Expansion bit selection table

PROGMEM unsigned char eTable[48] =
{
	31, 0,  1,  2,  3,  4,
	3,  4,  5,  6,  7,  8,
	7,  8,  9,  10, 11, 12,
	11, 12, 13, 14, 15, 16,
	15, 16, 17, 18, 19, 20,
	19, 20, 21, 22, 23, 24,
	23, 24, 25, 26, 27, 28,
	27, 28, 29, 30, 31, 0
};

unsigned char e(unsigned char i)
{
	return pgm_read_byte_far(GET_FAR_ADDRESS(eTable)+i);
}


//=============================================================================
// Permutation bit selection table

PROGMEM unsigned char pTable[32] =
{
	15,  6, 19, 20,
	28, 11, 27, 16,
	0,  14, 22, 25,
	4,  17, 30,  9,
	1,  7,  23, 13,
	31, 26,  2,  8,
	18, 12, 29,  5,
	21, 10,  3, 24
};

unsigned char p(unsigned char i)
{
	return pgm_read_byte_far(GET_FAR_ADDRESS(pTable)+i);
}


//=============================================================================
// Returns the value of the bit 'index' in the bit table 'table'.

unsigned char getBit(unsigned char *table, unsigned char index)
{
	return (table[index >> 3] & (0x80 >> (index & 0x07)));
}


//=============================================================================
// Sets or clears the bit 'index' in the bit table 'table', regarding the value
// of 'value'.

void putBit(unsigned char *table, unsigned char index, unsigned char value)
{
  unsigned char *p = &table[index >> 3];
  unsigned char m = 0x80 >> (index & 0x07);

  *p = (value) ? (m | *p) : (~m & *p);  
}


//=============================================================================
// Permutes 'in' using permutation table 'table' and puts the result to 'out'

void permute(
	unsigned char *out,
	unsigned char *in,
	unsigned char (*func)(unsigned char),
	unsigned char size
)
{
	do
	{
		size--;
		putBit(out, size, getBit(in, func(size)));
	}
	while (size);
}


//=============================================================================
// Calculates the exclusive or function of 'left' and 'right'. The result
// is written to 'left'. 'size' indicates the size of the two vectors in bytes.

void xor(unsigned char size, unsigned char *left, unsigned char *right)
{
	do
	{
		*left++ ^= *right++;
	}
	while (--size);
}


//=============================================================================
// Calculates the exclusive or function of 'left' and 'right'. The result
// is written to 'left'. 'right' resides in flash memory.

void xorFlash(unsigned char *left, uint_farptr_t right)
{
	unsigned char index = 6;
	do
	{
		*left++ ^= pgm_read_byte_far(right++);
	}
	while (--index);
}


//=============================================================================
// Generates 32 bits of output with 48 bits input, according to the S-boxes.

void s(unsigned char *out, unsigned char *in)
{
	unsigned char bitCount = 0;
	uint_farptr_t sBox = GET_FAR_ADDRESS(sTable);
	uint_farptr_t _sOrder = GET_FAR_ADDRESS(sOrder);
	unsigned char bytes = 4;

	do
	{
		unsigned char nibbles = 2;
		unsigned char assemblyByte = 0;

		do
		{
			unsigned char sboxByte;
			// Initiate the Index to point to the correct byte in the S-box
			uint_farptr_t order = _sOrder;
			unsigned char index = 0;
			unsigned char bit = 5;
			
			do
			{
				index <<= 1;
				if (getBit(in, bitCount + pgm_read_byte_far(order++)))
					index |= 0x01;
			}
			while (--bit);
	
			// Read the specific S-box-byte
			sboxByte = pgm_read_byte_far(sBox+index);

			// Decide to use the LOW- or the HIGH-nibble of the S-box-byte
			if (!(getBit(in, bitCount + 4)))
				sboxByte >>= 4;

			// Store results
			assemblyByte <<= 4;
			assemblyByte |= sboxByte & 0x0f;

			bitCount += 6;
			sBox += 32;
		}
		while (--nibbles);

		*out++ = assemblyByte;
	}
	while (--bytes);
}


//=============================================================================
// Copies 8 bytes from 'src' to 'dst'.

void copyBlock(unsigned char *dst, unsigned char *src)
{
	unsigned char i = 8;
	
	do
	{
		*dst++ = *src++;
	}
	while (--i);
}


//=============================================================================
// Decrypts 'buffer' using DES/3DES encryption algorithm. 'buffer' must be 8
// bytes long. 'chainBlock' points to the previous cipher block.

#if KEY_COUNT == 1
	#define DES_ITERATIONS 16
	#define DES_MASK 0xff
#elif KEY_COUNT == 2
	#define DES_ITERATIONS 48
	#define DES_MASK 0x1f
	#define DES_TRIPLE
#else
	#define DES_ITERATIONS 48
	#define DES_MASK 0xff
	#define DES_TRIPLE
#endif

void desDecrypt(unsigned char *buffer, unsigned char *chainBlock)
{
	unsigned char x;
	unsigned char *l, *r, tmpBuf[4];
	unsigned char temp1[6], temp2[4];
	unsigned char ipOut[8], iipIn[8];
	unsigned char swapBuffer[8];
	uint_farptr_t _kTable = GET_FAR_ADDRESS(kTable);

	l = &ipOut[0];
	r = &ipOut[4];

	// Initial Permutation
	permute(ipOut, buffer, ip, 64);

	for (x = 0; x < DES_ITERATIONS; )
	{    
		// f(......)
		permute(temp1, r, e, 48);
		xorFlash(temp1, _kTable+(6 * (x & DES_MASK)));
		s(temp2, temp1);
		permute(tmpBuf, temp2, p, 32);
		xor(4, l, tmpBuf);

		x++;

		// If not the 16th, 32nd, or the 48th iteration
		#ifdef DES_TRIPLE
			if (x & 0x0F)
		#endif
			{
				unsigned char *swapPointer;
				
				// swap R and L
				swapPointer = l;
				l = r;
				r = swapPointer;
			}
	}

	// Swap the two buffers L and R (not just the pointers) before Inverse
	// Initial Permutation
	#ifdef DES_TRIPLE
		iipIn[0] = l[0];
		iipIn[1] = l[1];
		iipIn[2] = l[2];
		iipIn[3] = l[3];
		iipIn[4] = r[0];
		iipIn[5] = r[1];
		iipIn[6] = r[2];
		iipIn[7] = r[3];
	#else
		iipIn[0] = r[0];
		iipIn[1] = r[1];
		iipIn[2] = r[2];
		iipIn[3] = r[3];
		iipIn[4] = l[0];
		iipIn[5] = l[1];
		iipIn[6] = l[2];
		iipIn[7] = l[3];
	#endif

	// Save cipher block before storing a decrypted one in buffer
	copyBlock(swapBuffer, buffer);

	// Inverse Initial Permutation
	permute(buffer, iipIn, iip, 64);

	// Cipher-Block-Chaining. Exclusive or with the previous cipher-text.
	xor(8, buffer, chainBlock);

	// Store saved cipher block to chain block buffer
	copyBlock(chainBlock, swapBuffer);	  
}

#endif // KEY_COUNT > 0
