//=============================================================================
// Copyright Atmel Corporation 2003. All Rights Reserved.
//
// File:			des.h
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
//=============================================================================

#ifndef DES_H
#define DES_H

extern void desDecrypt(unsigned char *buffer, unsigned char *cipherBlock);

#endif // DES_H
