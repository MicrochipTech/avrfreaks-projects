//=============================================================================
// File:			spm.h   (declarations for spm.c)
//
// Compiler:		IAR EWAVR  v4.10B
//
// Created:			4-Feb-2003	JP  (Atmel Finland)
// Modified:        2-May-2007  MJB (GBC Scientific Australia)
//
// Description:		Self-programming routines to suit ATmega128
//
//=============================================================================

#ifndef SPM_H
#define SPM_H


//___________ Macros for reading from program memory space ____________________

#define  PEEK_CODE_BYTE(addr)  (*(unsigned char  __hugeflash *)(addr))
#define  PEEK_CODE_WORD(addr)  (*(unsigned short  __hugeflash *)(addr))


//___________ Functions for programming flash and EEPROM memory _______________
//
//  Address parameters are 24-bit pointers to data in flash program memory
//  assuming a byte-addressing scheme.


//  Function:    spmWriteWord( addr, data )
//               Writes a word (2 bytes) into flash page "cache" at addr
//  Entry Args:  addr = address of first byte (LSB) of word to be written
//               (Bit 0 of addr is discarded to form a word address)
//               data = word value to be written
//
extern void spmWriteWord(unsigned char __hugeflash *addr, unsigned int data);


//  Function:    spmErasePage( addr )
//               Erases flash page at the specified address.
//  Entry Args:  addr = address within page to be erased
//               (Bits 7..0 of addr are discarded to form a page address)
//  Note:        It cannot be assumed that data in the page cache remains
//               unchanged after execution of this function.
//
extern void spmErasePage(unsigned char __hugeflash *addr);


//  Function:    spmProgramPage( addr )
//               Programs flash page at the specified address with data
//               contained in the page cache.
//  Entry Args:  addr = address within page to be written
//               (Bits 7..0 of addr are discarded to form a page address)
//  Note:        It cannot be assumed that data in the page cache remains
//               unchanged after execution of this function. However, the
//               PWW (application) flash section can be read afterwards.
//
extern void spmProgramPage(unsigned char __hugeflash *addr);


//  Function:    spmWriteLockBits( data )
//               Programs the 'Lock Bits' byte (= data)
//
extern void spmWriteLockBits(unsigned char data);


//  Function:    spmEEWriteByte( addr, data )
//               Writes a byte into EEPROM at addr
//
extern void spmEEWriteByte(unsigned int addr, unsigned char data);


#endif  // SPM_H
