//=============================================================================
// Copyright (C) 2003 Atmel Corporation
//
// File:			loader.h
// Compiler:		IAR Atmel AVR C/EC++ Compiler
// Output Size:
// Based on work by:ØE, VU
// Created:			4-Feb-2003	JP (Atmel Finland)
// Modified:
//
// Support Mail:	avr@atmel.com
//
// Description:		Loader receives encrypted data frames (generated using
//					'create' tool and sent by 'update' tool), decrypts them and
//					executes the commands described in frames.
//
//					For more information, refer to the Application Note
//					Documentation.
//
// Other Info:		
//=============================================================================


#ifndef LOADER_H
#define LOADER_H

//=============================================================================
// Error Codes
//=============================================================================

#define ERROR_OK	0x11
#define ERROR_CRC	0x22


//=============================================================================
// Frame types
//=============================================================================

#define TYPE_EOF		0
#define TYPE_ERASE		1
#define TYPE_PREPARE	2
#define TYPE_DATA		3
#define TYPE_PROGRAM	4
#define TYPE_EEPROM		5
#define TYPE_LOCKBITS	6
#define TYPE_RESET		7


//=============================================================================
// To minimize the target size, the following types are declared
// depending on the target AVR.
//=============================================================================

#if BUFFER_SIZE < 256
	typedef unsigned char frameindex_t;
#else
	typedef unsigned int frameindex_t;
#endif

void loader(void);

#endif // LOADER_H
