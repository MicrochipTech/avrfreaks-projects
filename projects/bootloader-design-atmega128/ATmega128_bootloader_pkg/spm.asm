//=============================================================================
// File:			spm.asm
//
// Compiler:		IAR EWAVR  v4.10B
//
// Created:			4-Feb-2003	JP  (Atmel Finland)
// Modified:        2-May-2007  MJB (GBC Scientific Australia)
//
// Description:		Self-programming routines to suit ATmega128
//
//=============================================================================

NAME	spm(16)

PUBLIC	spmWriteWord
PUBLIC	spmWriteLockBits
PUBLIC	spmErasePage
PUBLIC	spmProgramPage
PUBLIC	spmEEWriteByte

#define __ENABLE_BIT_DEFINITIONS__
#include  <iom128.h >


RSEG	CODE

//=============================================================================
// Write one word to the flash page cache at specified address
// Entry args:  (uint8 __hugeflash *) address in r18:r17:r16  (24 bits)
//              (uint16) data word in r21:r20

spmWriteWord:
	movw	r1:r0, r21:r20
	ldi		r22, (1 << SPMEN)
	rjmp	spm_exec


//=============================================================================
// Erase specified flash page
// Entry args:  (uint8 __hugeflash *) address in r18:r17:r16  (24 bits)
//              LSB of address (r16) is ignored.

spmErasePage:
	ldi		r22, (1 << PGERS) | (1 << SPMEN)
	rcall	spm_exec
	rjmp    spm_wait


//=============================================================================
// Program the flash page cache to actual flash memory and
// re-enable flash read in RWW section
// Entry args:  (uint8 __hugeflash *) address in r18:r17:r16  (24 bits)
//              LSB of address (r16) is ignored.

spmProgramPage:
	ldi		r22, (1 << PGWRT) | (1 << SPMEN)
	rcall	spm_exec
	ldi		r22, (1 << RWWSRE) | (1 << SPMEN)
	rcall	spm_exec
	rjmp    spm_wait


//=============================================================================
// Write Lock Bits
// Entry args:  (uint8) data byte in r16  (lock bits)

spmWriteLockBits:
	mov		r0, r16
	ldi		r22, (1 << BLBSET) | (1 << SPMEN)
	rcall	spm_exec
	rjmp    spm_wait


//=============================================================================
// Execute self-programming command

spm_exec:
	rcall	spm_wait            ; wait for any previous SPM command to complete
	in		r20, SREG           ; save IRQ status and disable global IRQ
	cli
	in		r21, RAMPZ          ; save RAMPZ register
	out		RAMPZ, r18          ; load RAMPZ register with MSB of flash address
	movw	r31:r30, r17:r16    ; load the Z register with flash address (LS 16 bits)
	sts		SPMCSR, r22         ; load SPMCSR with command byte
	espm                        ; make it happen
	nop
	out		RAMPZ, r21          ; restore previous RAMPZ
	out		SREG, r20           ; restore IRQ status
	ret

spm_wait:
	lds		r23, SPMCSR
	andi	r23, (1 << SPMEN)
	brne	spm_wait
	ret


//=============================================================================
// Write one byte to EEPROM memory

spmEEWriteByte:
	rcall	spm_wait
	rcall	spmEEWriteByteComplete

	out		EEARL, r16
	out		EEARH, r17
	out		EEDR, r18

	sbi		EECR, EEMWE
	sbi		EECR, EEWE

spmEEWriteByteComplete:
	sbic	EECR, EEWE
	rjmp	spmEEWriteByteComplete
	ret


END
