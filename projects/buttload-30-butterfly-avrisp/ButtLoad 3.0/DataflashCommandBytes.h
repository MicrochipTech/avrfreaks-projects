/*
             BUTTLOAD - Butterfly ISP Programmer

              Copyright (C) Dean Camera, 2007.
              
             dean [at] fourwalledcubicle [dot] com
                  www.fourwalledcubicle.com

           Released under the GPL Licence, Version 2.
*/

// This file originally by Atmel, modified by Martin Thomas and Dean Camera.

#ifndef DFCB_H
#define DFCB_H

	// DATAFLASH OPCODES:
	#define DFCB_BLOCKERASE          0x50  // Block erase, added by Dean Camera
	#define DFCB_FLASHPAGEREAD       0x52  // Main memory page read
	#define DFCB_FLASHTOBUF1TRANSFER 0x53  // Main memory page to buffer 1 transfer
	#define DFCB_BUF1READ            0x54  // Buffer 1 read
	#define DFCB_FLASHTOBUF2TRANSFER 0x55  // Main memory page to buffer 2 transfer
	#define DFCB_BUF2READ            0x56  // Buffer 2 read
	#define DFCB_STATUSREG           0x57  // Status register
	#define DFCB_AUTOPAGEREWRBUF1    0x58  // Auto page rewrite through buffer 1
	#define DFCB_AUTOPAGEREWRBUF2    0x59  // Auto page rewrite through buffer 2
	#define DFCB_FLASHTOBUF1COMPARE  0x60  // Main memory page to buffer 1 compare
	#define DFCB_FLASHTOBUF2COMPARE  0x61  // Main memory page to buffer 2 compare
	#define DFCB_CONTARRAYREAD       0x68  // Continuous Array Read (Note : Only A/B-parts supported)
	#define DFCB_PAGEERASE           0x81  // Page erase, added by Martin Thomas
	#define DFCB_FLASHPROGBUF1       0x82  // Main memory page program through buffer 1
	#define DFCB_BUF1TOFLASHWE       0x83  // Buffer 1 to main memory page program with built-in erase
	#define DFCB_BUF1WRITE           0x84  // Buffer 1 write
	#define DFCB_FLASHPROGBUF2       0x85  // Main memory page program through buffer 2
	#define DFCB_BUF2TOFLASHWE       0x86  // Buffer 2 to main memory page program with built-in erase
	#define DFCB_BUF2WRITE           0x87  // Buffer 2 write
	#define DFCB_BUF1TOFLASH         0x88  // Buffer 1 to main memory page program without built-in erase
	#define DFCB_BUF2TOFLASH         0x89  // Buffer 2 to main memory page program without built-in erase

#endif
