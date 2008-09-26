/*****************************************************************************
*
* Atmel Corporation
*
* File              : spm.h
* Compiler          : IAR EWAAVR 2.28a/3.10c
* Revision          : $Revision: 2.0 $
* Date              : $Date: Wednesday, February 15, 2006 09:52:02 UTC $
* Updated by        : $Author: raapeland $
*
* Support mail      : avr@atmel.com
*
* Supported devices : All devices with Bootloader Capabilities 
*                     , and at least 1-KB SRAM can be used.
*                     The example is written for • ATmega8
*                                                • ATmega16
*                                                • ATmega162
*                                                • ATmega169
*                                                • ATmega32
*                                                • ATmega64
*                                                • ATmega128
*
* AppNote           : AVR231 - AES Bootloader
*
* Description:	      Self-programming routines written in assembly
*
******************************************************************************/

#ifndef AESFLASH_H
#define AESFLASH_H

#if defined(RAMPZ)
#define pflash_byte3(adr)	({ uint8_t tmp; asm volatile("ldi %0, hh8(%1)" : "=a" (tmp) : "p" (adr)); tmp;})

extern void spmWritePage(const unsigned char *addr, unsigned char *data, unsigned int size, unsigned char Addr3);
extern void spmErasePage(const unsigned char *addr, unsigned char Addr3);
extern void spmEnableRWW(void);
extern void spmProgramPage(const unsigned char *addr, unsigned char Addr3);
extern void spmWriteLockBits(unsigned char data);
extern void memcpy_PF(unsigned char *buffer, unsigned char addr3, const unsigned char *addr, unsigned int size);
#else
extern void spmWritePage(const unsigned char *addr, unsigned char *data, unsigned int size);
extern void spmErasePage(const unsigned char *addr);
extern void spmEnableRWW(void);
extern void spmProgramPage(const unsigned char *addr);
extern void spmWriteLockBits(unsigned char data);
extern void memcpy_PN(unsigned char *buffer, const unsigned char *addr21, unsigned int size);
#endif
#endif  // SPM_H
