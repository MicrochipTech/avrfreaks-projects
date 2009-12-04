;****************************************************************************
;Title:     Small STK500v2 compatible bootloader
;Varsion:   1.0 beta 2
;Author:    Alexey V. Kozodoev <kav@iao.ru>,<kavka@fromru.com>
;Compiler:  AvrAssembler2 (AVR Studio)
;Hardware:  Must fit to almost all AVRs with bootloader support 
;           and FLASH up to 64Kb and FLASH page size up to 256 byte.
;           Tested with ATmega16, ATmega8535
;License:   GNU General Public License v2
;
;
;DESCRIPTION:
;    This program allows an AVR with bootloader capabilities to
;    read/write FLASH/EEPROM, read fuses and lock bits, 
;    write some lock bits (device dependent).
;    Bootloader realize subset of STK500v2 protocol.
;    To enter Programming mode defined pin is checked. If this pin
;    is signaled (pulled low), bootloader started. If not, normal execution is
;    done from $0000 "reset" vector in application area.
;    Fits in 256 words (512 bytes) bootloader section.
;
;USAGE:
;    - set AVR MCU type
;    - set clock-frequency
;    - set baud rate
;      standart speed for AVR Studio 4 is 115200
;      AVRDUDE can work with almost any speed
;    - set PROGLED_*
;      if no LED indication needed then uncoment REMOVE_BOOTLOADER_LED definition
;    - set PROG_* (MCU pin to test)
;    - compile/link the bootloader
;    - program the BOOTSZ fuses for boot-size = 256 words
;    - enable the BOOT Reset Vector (program BOOTRST)
;    - program Boot Lock Bits 1 to mode 3 (real values BLB1=0  or by bits  BLB11=0 and BLB12=0 )
;      Boot Lock Bit1 Protection Mode for Boot Loader Section (see datasheet for details)
;      Required next fetures:
;       1) SPM is not allowed to write to the Boot Loader section
;       2) If interrupt vectors are placed in
;          the Application section, interrupts are disabled while executing
;          from the Boot Loader section
;    - Upload the hex file to the AVR any programmer
;    - Reset MCU while keeping PROG_PIN pulled low, bootloader will start
;    - Start AVRISP Programmer (AVRStudio/Tools/Program AVR), use AVRdude or whatever supporting STK500v2 protocol
;    - Program your MCU
;    - Reset MCU with PROG_PIN released to exit bootloader.
;
;NOTES:
;    - Based on Atmel Application Note AVR109 - Self-programming
;    - Based on Atmel Application Note AVR068 - STK500v2 Protocol
;    - Based on STK500v2 compatible bootloader by Peter Fleury (http://jump.to/fleury)
;    - Erasing the device without flashing, through AVRISP GUI button
;      "Erase Device" is optional (EEPROM and applation section of FLASH).
;
;LICENSE:
;    This program is free software; you can redistribute it and/or modify
;    it under the terms of the GNU General Public License as published by
;    the Free Software Foundation; either version 2 of the License, or
;    any later version.
;
;    This program is distributed in the hope that it will be useful,
;    but WITHOUT ANY WARRANTY; without even the implied warranty of
;    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;    GNU General Public License for more details.
;
;HISTORY:
; v1.0 beta - First public release
;
;*****************************************************************************/

#define __AVR_ATmega16__
;#define __AVR_ATmega8535__

;#define __AVR_ATmega8__
;#define __AVR_ATmega88__
;#define __AVR_ATmega16__
;#define __AVR_ATmega162__
;#define __AVR_ATmega168__
;#define __AVR_ATmega32__
;#define __AVR_ATmega128__
;#define __AVR_AT90CAN32__
;#define __AVR_AT90CAN64__
;#define __AVR_AT90CAN128__

;*
;* include files
;*
#if defined (__AVR_ATmega8__)
.include "m8def.inc"
#elif defined (__AVR_ATmega16__)
.include "m16def.inc"
#elif defined (__AVR_ATmega32__)
.include "m32def.inc"
#elif defined (__AVR_ATmega8515__)
.include "m8515def.inc"
#elif defined (__AVR_ATmega8535__)
.include "m8535def.inc"
#elif defined (__AVR_ATmega88__)
.include "m88def.inc"
#elif defined (__AVR_ATmega162__)
.include "m162def.inc"
#elif defined (__AVR_ATmega168__)
.include "m168def.inc"
#elif defined (__AVR_ATmega128__)
.include "m128def.inc"
#elif defined (__AVR_AT90CAN32__)
.include "can32def.inc"
#elif defined (__AVR_AT90CAN64__)
.include "can64def.inc"
#elif defined (__AVR_AT90CAN128__)
.include "can128def.inc"
#else
	#error "no signature definition for MCU available"
#endif


;*
;* define CPU frequency in Mhz here if not defined in Makefile 
;*
#ifndef F_CPU
#define F_CPU 16000000
#endif


;*
;* Uncomment the following lines to switch off features
;*
; disable LED to show active bootloader
;#define REMOVE_BOOTLOADER_LED

; disable internal pullup, when external used
;#define REMOVE_PROG_PIN_PULLUP

; enable signature readings with SPI_MULTI commands
; AVRdude do signature read this way 
; if SPI_MULTI removed then use -F option of avrdude to skip signature checking
#define ENABLE_SPI_MULTI

; if you want to leave bootloader (go to application) after programming
;#define ENABLE_LEAVE_BOOTLOADER

; enable programing of "LOCK BITS"
; it seems reasonable to keep this feture disabled
; !!!! only programming is possible
; !!!! reset is possible only by "chip erase" with external hardware programmer
; !!!! bit values: 0 = programmed, 1 = UNprogrammed
;#define ENABLE_PROGRAM_LOCK

; enable lock and fuse read capability
#define ENABLE_READ_LOCK_FUSE

; enable EEPROM writing
#define ENABLE_PROGRAMM_EEPROM

; enable EEPROM reading
#define ENABLE_READ_EEPROM

; enable FLASH reading
#define ENABLE_READ_FLASH


; enable address reset by "chip erase" command
; in Atmel Application Note AVR068 (STK500v2 Protocol) not stated that address reset must be done
; when "chip erase" issued
; seems that avrdude and avrisp (AVR Studio) do "set addres" every time before programming
; so it works without this
;#define ENABLE_CHIP_ERASE_RESET_ADDRESS

; do FLASH erase on "chip erase" and do not erase pages before programming
;#define ENABLE_CHIP_ERASE_FLASH

; do EEPROM erase on "chip erase" (fill with 0xFF)
;#define ENABLE_CHIP_ERASE_EEPROM

;*
;* Pin "PROG_PIN" on port "PROG_PORT" has to be pulled low
;* (active low) to start the bootloader 
;* uncomment #define REMOVE_PROG_PIN_PULLUP if using an external pullup
;*
#define PROG_PORT  PORTD
#define PROG_DDR   DDRD
#define PROG_IN    PIND
#define PROG_PIN   PIND4

;*
;* LED on pin "PROGLED_PIN" on port "PROGLED_PORT" 
;* indicates that bootloader is active
;*
#define PROGLED_ACTIVE_HIGH
#define PROGLED_PORT PORTD
#define PROGLED_DDR  DDRD
#define PROGLED_PIN  PIND5


;*
;* define which UART channel will be used, if device with two UARTs is used
;*
;define USE_USART1        // undefined means use USART0


;*
;* UART Baudrate
;* AVRStudio usualy accepts 115200 bps
;* AVRdude works with almost any standard speed
;* See MCU datasheet for details of how to choose suitable bauderate for used F_CPU
;*
#define BAUDRATE 115200
;#define BAUDRATE 38400


;*
;*  Enable (1) or disable (0) USART double speed operation
;*
#define UART_BAUDRATE_DOUBLE_SPEED 1


; !!! experimental features
; !!! you will need it in rare situations

; protect bootloader section of FLASH
; it seems that this option is overkill because protection is done by proper BLB set
;#define PROTECT_BOOTSECTION

; usual or not send sequence
;#define USART_STD_SEND

;* !!! no configuration needed below
;*************************************************

#ifdef ENABLE_PROGRAM_LOCK
#define	POINTER_CMD_PROGRAM_LOCK_ISP PMSG_CMD_PROGRAM_LOCK_ISP
#else
#define	POINTER_CMD_PROGRAM_LOCK_ISP PMSG_RESULT_FAIL
#endif 

#ifdef ENABLE_READ_LOCK_FUSE
#define POINTER_CMD_READ_FUSE_ISP PMSG_CMD_READ_FUSE_ISP
#define POINTER_CMD_READ_LOCK_ISP PMSG_CMD_READ_LOCK_ISP
#else
#define POINTER_CMD_READ_FUSE_ISP PMSG_RESULT_FAIL
#define POINTER_CMD_READ_LOCK_ISP PMSG_RESULT_FAIL
#endif

#ifdef ENABLE_PROGRAMM_EEPROM
#define POINTER_CMD_PROGRAM_EEPROM_ISP PMSG_CMD_PROGRAM_EEPROM_ISP
#else
#define POINTER_CMD_PROGRAM_EEPROM_ISP PMSG_RESULT_FAIL
#endif

#ifdef ENABLE_READ_EEPROM
#define POINTER_CMD_READ_EEPROM_ISP PMSG_CMD_READ_EEPROM_ISP
#else
#define POINTER_CMD_READ_EEPROM_ISP PMSG_RESULT_FAIL
#endif

#ifdef ENABLE_READ_FLASH
#define POINTER_CMD_READ_FLASH_ISP PMSG_CMD_READ_FLASH_ISP
#else
#define POINTER_CMD_READ_FLASH_ISP PMSG_RESULT_FAIL
#endif

#if defined(ENABLE_CHIP_ERASE_RESET_ADDRESS) || defined(ENABLE_CHIP_ERASE_FLASH) || defined(ENABLE_CHIP_ERASE_EEPROM)
#define POINTER_CMD_CHIP_ERASE PMSG_CMD_CHIP_ERASE
#else
#define POINTER_CMD_CHIP_ERASE PMSG_END
#endif
.include "command.inc"

;*
;* HW and SW version, reported to AVRISP
;*
; code optimized for BUILD_NUMBER_* parameters == 0
#define CONFIG_PARAM_BUILD_NUMBER_LOW	0
#define CONFIG_PARAM_BUILD_NUMBER_HIGH	0
; code optimized for HW_VER == SW_MAJOR parameters == 2
#define CONFIG_PARAM_HW_VER				2
#define CONFIG_PARAM_SW_MAJOR			2
#define CONFIG_PARAM_SW_MINOR			0x0A


;*
;* Calculate the address where the bootloader starts from FLASHEND and BOOTSIZE
;* BOOTSIZE in words
;*
#define BOOTSIZE 256
#define BL_START_WORD  ((FLASHEND - BOOTSIZE) + 1)


;*
;*  Defines for the various USART registers
;*
#if  defined(__AVR_ATmega8__)    || defined(__AVR_ATmega16__) || defined(__AVR_ATmega32__) \
  || defined(__AVR_ATmega8515__) || defined(__AVR_ATmega8535__) 
;*
;* ATMega with one USART
;*
#define	UART_BAUD_RATE_LOW	     UBRRL
#define UART_BAUD_RATE_HIGH	     UBRRH
#define	UART_STATUS_REG	         UCSRA
#define	UART_CONTROL_REG	     UCSRB
#define	UART_ENABLE_TRANSMITTER	 TXEN
#define	UART_ENABLE_RECEIVER	 RXEN
#define	UART_TRANSMIT_COMPLETE	 TXC
#define	UART_RECEIVE_COMPLETE	 RXC
#define	UART_DATA_REG	         UDR
#define UART_DOUBLE_SPEED        U2X
#define UART_DATA_REGISTER_EMPTY UDRE
#elif  defined(__AVR_ATmega64__) || defined(__AVR_ATmega128__) || defined(__AVR_ATmega162__) || defined(__AVR_ATmega88__) || defined(__AVR_ATmega168__) \
	|| defined(__AVR_ATmega88__) || defined(__AVR_AT90CAN32__) || defined(__AVR_AT90CAN64__) || defined(__AVR_AT90CAN128__)
;*
;*  ATMega with two USART, select USART for bootloader using USE_USART1 define
;*
#ifndef USE_USART1

#define	UART_BAUD_RATE_LOW	     UBRR0L
#ifdef UBRR0H
#define UART_BAUD_RATE_HIGH	     UBRR0H
#endif
#define	UART_STATUS_REG	         UCSR0A
#define	UART_CONTROL_REG	     UCSR0B
#define	UART_ENABLE_TRANSMITTER	 TXEN0
#define	UART_ENABLE_RECEIVER	 RXEN0
#define	UART_TRANSMIT_COMPLETE	 TXC0
#define	UART_RECEIVE_COMPLETE	 RXC0
#define	UART_DATA_REG	         UDR0
#define UART_DOUBLE_SPEED        U2X0
#define UART_DATA_REGISTER_EMPTY UDRE0

#else

#define	UART_BAUD_RATE_LOW	     UBRR1L
#ifdef UBRR1H
#define UART_BAUD_RATE_HIGH      UBRR1H
#endif
#define	UART_STATUS_REG	         UCSR1A
#define	UART_CONTROL_REG	     UCSR1B
#define	UART_ENABLE_TRANSMITTER	 TXEN1
#define	UART_ENABLE_RECEIVER	 RXEN1
#define	UART_TRANSMIT_COMPLETE	 TXC1
#define	UART_RECEIVE_COMPLETE	 RXC1
#define	UART_DATA_REG	         UDR1
#define UART_DOUBLE_SPEED        U2X1
#define UART_DATA_REGISTER_EMPTY UDRE1

#endif

#else
	#error "no UART definition for MCU available"
#endif


;*
;* Macros to map the new ATmega88/168 EEPROM bits
;*
#ifdef EEMPE
#define EEMWE EEMPE
#define EEWE  EEPE
#endif


;*
;* Macro to calculate UBBR from XTAL and baudrate
;*
#if UART_BAUDRATE_DOUBLE_SPEED
#define UART_BAUD_SELECT(baudRate,xtalCpu) \
(((xtalCpu))/(((baudRate))*8.0)-1.0+0.5)
#else
#define UART_BAUD_SELECT(baudRate,xtalCpu) \
(((xtalCpu))/(((baudRate))*16.0)-1.0+0.5)
#endif

;*
;* States used in the data receiving state machine
;*
;* state machine combined with indirect jump

; high byte of address of state machine routine in FLASH
; it equals to high(BL_START_WORD) - high byte of bootloader section
#define ST_BASE_HIGH high(PARSE_start)

;* states is addresses within one 256-word page
#define	ST_START		low(PARSE_start)
#define	ST_GET_SEQ_NUM	low(PARSE_seq_num)
#define	ST_MSG_SIZE_1	low(PARSE_msg_sz1)
#define	ST_MSG_SIZE_2	low(PARSE_msg_sz2)
#define	ST_GET_TOKEN	low(PARSE_token)
#define	ST_GET_DATA		low(PARSE_get_data)
#define	ST_GET_CHECK	low(PARSE_checksum)


#define GET_LOW_FUSE_BITS           (0x0000)
#define GET_LOCK_BITS               (0x0001)
#define GET_EXTENDED_FUSE_BITS      (0x0002)
#define GET_HIGH_FUSE_BITS          (0x0003)


; =====================================  MACROs ===========================================================
; =========================================================================================================

.MACRO UART_getChar
USART_Receive:
; Wait for data to be received
	sbis 	UART_STATUS_REG, UART_RECEIVE_COMPLETE
	rjmp 	USART_Receive
; Get and return received data from buffer
	in		@0, UART_DATA_REG
.ENDMACRO


.MACRO UART_putChar
USART_Transmit:
; Wait for empty transmit buffer
	sbis	UART_STATUS_REG,UART_DATA_REGISTER_EMPTY
	rjmp	USART_Transmit
; Put data (r16) into buffer, sends the data
	out 	UART_DATA_REG,@0
.ENDMACRO

.MACRO UART_putChar2
; Put data (r16) into buffer, sends the data
	out 	UART_DATA_REG,@0
USART_Transmit:
; Wait for empty transmit buffer
	sbis	UART_STATUS_REG,UART_DATA_REGISTER_EMPTY
	rjmp	USART_Transmit
.ENDMACRO

; =========================================================================================================
; ============================  DATA ======================================================================
; =========================================================================================================
.dseg

BUFFER:		.BYTE	PAGESIZE+24
;BUFFER:		.BYTE	280

; =========================================================================================================
; ============================  CODE ======================================================================
; =========================================================================================================


// size of starting part of output message (see STK500v2 protocol)
.equ STARTING_SIZE = 5

.cseg 
.org	BL_START_WORD	; set code location to bootloader section of FLASH memory

.def	temp1 = r25


; ========================================== initialization ===============================================

;===========================================
; SET BLB1 mode =3 (BLB12 programmed (=0), BLB11 programmed (=0) )
; no interrupts
; SPM unable to write to bootloader section
;-------------------------------------------


#ifndef REMOVE_PROG_PIN_PULLUP
    sbi		PROG_PORT, PROG_PIN		; Enable internal pullup
#endif

; Initialize stack pointer to end of internal RAM
	ldi 	temp1,low(RAMEND)
	out		SPL,temp1
	ldi 	temp1,high(RAMEND)
	out 	SPH, temp1


	sbic	PROG_IN, PROG_PIN	; test "bootloader start" condition, if input is low, skip next jump

LEAVE_BOOTLOADER:

	rjmp FLASHEND+1

; ========================================== bootloader body begins =======================================

#ifndef REMOVE_BOOTLOADER_LED

    sbi		PROGLED_DDR, PROGLED_PIN	; set pin direction to "output" for LED
#ifdef PROGLED_ACTIVE_HIGH 
	sbi		PROGLED_PORT, PROGLED_PIN	; PROG_PIN pulled high, indicate with LED that bootloader is active 
#else
    cbi		PROGLED_PORT, PROGLED_PIN	; PROG_PIN pulled low, indicate with LED that bootloader is active 
#endif

#endif ; REMOVE_BOOTLOADER_LED

; ========================================== initialize UART ==============================================
 
#if UART_BAUDRATE_DOUBLE_SPEED
    sbi		UART_STATUS_REG, UART_DOUBLE_SPEED
#endif

#ifdef UART_BAUD_RATE_HIGH 
    ldi		temp1, high(UART_BAUD_SELECT(BAUDRATE,F_CPU))
	out		UART_BAUD_RATE_HIGH, temp1
#endif

	ldi		temp1, low(UART_BAUD_SELECT(BAUDRATE,F_CPU))
	out		UART_BAUD_RATE_LOW, temp1
	ldi		temp1, (1 << UART_ENABLE_RECEIVER) | (1 << UART_ENABLE_TRANSMITTER)
	out		UART_CONTROL_REG, temp1

; ========================================== initialize global variables ==================================
; X r26(low)-r27(high) - working with BUFFER
; Y r28(low)-r29(high) - working with BUFFER (main)
; Z r30(low)-r31(high) - used to programm FLASH and EEPROM memory, store address


.def addr_E_L = r4
.def addr_E_H = r5		; storing addres for EEPROM read/write operations

.def addr_F_L = r6
.def addr_F_H = r7		; storing addres for FLASH read/write operations


; ========================================== MAIN LOOP begin ==============================================
MAIN_LOOP:

.def seqNum = r2

.def outMsgSize_H = r23	; length of output message
.def outMsgSize_L = r22

; ========================================== get input message ============================================
.def ParseState = r16
.def in_char = r17
.def checksum = r18


; regY - pointer to buffer positon for input message
; receive message from offset 5
; starting 5 bytes reserved for use in output message


PARSE_INIT: ; set initial PARSE state

	ldi 	ParseState, ST_START

PARSE_new_state:

PARSE_loop:


; Wait for data to be received
USART_Receive:
	sbis 	UART_STATUS_REG, UART_RECEIVE_COMPLETE
	rjmp 	USART_Receive
; Get and return received data from buffer
	in		in_char, UART_DATA_REG


	eor		checksum, in_char		; cheksum every byte here
	
	ldi		ZH, ST_BASE_HIGH		; == high(BL_START_WORD), bootloader section (FLASH memory)
	mov		ZL, ParseState			; all state machine fits with 256-byte page -> no 16-bit math
	ijmp

// -------------------------------- process state ST_START
PARSE_start:
	cpi		in_char, MESSAGE_START
	brne	PARSE_INIT
	ldi		checksum, MESSAGE_START		; initialize checksum with message begining
	ldi	    ParseState, ST_GET_SEQ_NUM
	rjmp	PARSE_new_state

// -------------------------------- process state ST_GET_SEQ_NUM
PARSE_seq_num:
	cpi		in_char, 1
	breq	PARSE_seq_num2			; if received seqNum==1 - go to next state
	cp		in_char, seqNum
	brne	PARSE_INIT				; if received seqNum!=stored seqNum - go to initial state
PARSE_seq_num2:
	mov		seqNum, in_char
	ldi	    ParseState, ST_MSG_SIZE_1
	rjmp	PARSE_new_state

// -------------------------------- process state ST_MSG_SIZE_1
PARSE_msg_sz1:
	mov		XH, in_char					; mov		inMsgLen_H, in_char
	ldi	    ParseState, ST_MSG_SIZE_2
	rjmp	PARSE_new_state

// -------------------------------- process state ST_MSG_SIZE_2
PARSE_msg_sz2:
	mov		XL, in_char					; mov		inMsgLen_L, in_char
	ldi	    ParseState, ST_GET_TOKEN
	rjmp	PARSE_new_state

// -------------------------------- process state ST_GET_TOKEN
PARSE_token:
	cpi		in_char, TOKEN
	brne	PARSE_INIT
	ldi		YL, low(  BUFFER+5 )	; Y is pointer in BUFFER+5
	ldi		YH, high( BUFFER+5 )	; Y is pointer in BUFFER+5

	ldi	    ParseState, ST_GET_DATA
	rjmp	PARSE_new_state

// -------------------------------- process state ST_GET_DATA
PARSE_get_data:
	st		Y+, in_char
	sbiw	X, 1					; get only specified number of bytes
	brne	PARSE_loop
; here X will be always = zero
	ldi	    ParseState, ST_GET_CHECK
	rjmp	PARSE_new_state

PARSE_checksum:
	eor		checksum, in_char		; rollback last byte
	cp		in_char, checksum
	brne	PARSE_INIT				; if checksum is bad - reset state to initial
									; else we go to ST_PROCESS state...

PARSE_end:


;.def ParseState = r16
;.def in_char = r17
;.def checksum = r18

.undef ParseState
.undef in_char
.undef checksum

.def spm_temp = r3
.def spm_cmd = r24

.def	zero_reg = r10
	clr		zero_reg

// ========================================== process message ==============================================
.def buf4 = r16
.def buf3 = r17
.def buf2 = r18
.def buf1 = r19
.def command = r20
.def r_STATUS_CMD_OK = r21


// read some variables and set popular values
	ldi		r_STATUS_CMD_OK, STATUS_CMD_OK
	ldi		YL, low(  BUFFER+5 )	; Y is pointer in BUFFER+5
	ldi		YH, high( BUFFER+5 )	; Y is pointer in BUFFER+5
	ld		command, Y+				; Get BUFFER+5[0] get STK500v2 command, Y++
	ld		buf1, Y					; Get BUFFER+5[1] size, high byte (for commands where it meanfull)
	st		Y+, r_STATUS_CMD_OK		; Set BUFFER+5[1] = STATUS_CMD_OK (popular value), Y++
	ld		buf2, Y					; Get BUFFER+5[2] size, low byte (for commands where it meanfull)
	ldd		buf3, Y+1				; Get BUFFER+5[3]
	std		Y+1, r_STATUS_CMD_OK	; Set BUFFER+5[3] = STATUS_CMD_OK (popular value)
	ldd		buf4, Y+2				; Get BUFFER+5[4]
; here Y points to BUFFER+5+2

	ldi		outMsgSize_H, 0			; total message length from BUFFER (popular value)
	ldi		outMsgSize_L, 2			; starting part size + answer part size
	
#ifdef ENABLE_SPI_MULTI
	cpi		command, CMD_SPI_MULTI+1	; CMD_SPI_MULTI is the last supported command (code)
#else
	cpi		command, CMD_READ_SIGNATURE_ISP+1
#endif

	brlo	PMSG_GO_PROCESS
PMSG_GO_FAIL:
	rjmp	PMSG_RESULT_FAIL		; if same or higher

PMSG_GO_PROCESS:
; set popular values
	ldi		spm_cmd,  (1<<BLBSET) | (1<<SPMEN)	; popular value for lock and fuse bits operations
	movw	X, buf1:buf2						; DataSize_H:DataSize_L	; counter of bytes

	ldi		ZH, high(CMD_TBL*2)
	ldi		ZL, low(CMD_TBL*2)
	add		ZL, command
	lpm		temp1, Z
	ldi		ZH, high(BL_START_WORD)
	mov		ZL, temp1
	ijmp


; --------------------------------------- process command CMD_SIGN_ON
PMSG_CMD_SIGN_ON:
	ldi		ZL, low(SIGN_MSG*2)
	ldi		ZH, high(SIGN_MSG*2)
	ldi		XL, 9	; counter
	ldi		XH, 0
	ldi		outMsgSize_L, 11		; starting part size + answer part size
									; use X as pointer to BUFFER

#ifdef ENABLE_READ_FLASH
	rcall	copy_flash2sram			; *Z -> *Y , X = size
#else

copy_flash2sram:
; INPUT:	*Z -> *Y, X == byte_size
; OUTPUT: 	Z = Z + byte_size
;			Y = Y + byte_size
;			X = 0
; USE:		r0
	lpm		r0, Z+
	st		Y+, r0
	sbiw	X ,1
	brne	copy_flash2sram
	ret

#endif

	rjmp	PMSG_END


; --------------------------------------- process command CMD_READ_SIGNATURE_ISP
PMSG_CMD_READ_SIGNATURE_ISP:

#ifdef ENABLE_SPI_MULTI
	rcall	load_signature_byte		; read signature byte number 'buf4' to 'temp1'
#else
	ldi		ZL, low(  (SIGN_B*2) + 1 )
	ldi		ZH, high( (SIGN_B*2) + 1 )
	add		ZL, buf4				; signature bytes placed within one 256-byte page
	lpm		temp1, Z
#endif

	rjmp	PMSG_END_store_Y_temp1_size4

; --------------------------------------- process command CMD_LOAD_ADDRESS
PMSG_CMD_LOAD_ADDRESS:
											; 16bit address only
	movw	addr_E_H:addr_E_L, buf3:buf4	; for EEPROM
	movw	addr_F_H:addr_F_L, buf3:buf4	; for FLASH

	lsl		addr_F_L				; multiply by 2, get byte value
	rol		addr_F_H

	rjmp	PMSG_END


#ifdef ENABLE_PROGRAMM_EEPROM

; --------------------------------------- process command CMD_PROGRAM_EEPROM_ISP
PMSG_CMD_PROGRAM_EEPROM_ISP:
	adiw	Y, 8					; BUFFER+5 +10
	movw	Z, addr_E_H:addr_E_L	; points to first byte in FLASH to programm

do_eeprom_write:
	ld		temp1, Y+
	rcall	common_eeprom
	out		EEDR, temp1
	sbi		EECR, EEMWE
	sbi		EECR, EEWE
do_eeprom_write_wait:
	sbic	EECR, EEWE
	rjmp	do_eeprom_write_wait
	brne	do_eeprom_write			; from last SBIW instruction

	movw	addr_E_H:addr_E_L, Z
	rjmp	PMSG_END


#endif ;ENABLE_PROGRAMM_EEPROM

#ifdef ENABLE_READ_FLASH

; --------------------------------------- process command CMD_READ_FLASH_ISP
PMSG_CMD_READ_FLASH_ISP:
	ldi		outMsgSize_L, 3			; high byte is 0
	add		outMsgSize_L, buf2		; DataSize_L, DataSize +3
	adc		outMsgSize_H, buf1		; DataSize_H

	movw	Z, addr_F_H:addr_F_L	; use Z as pointer to memory, source

									; use X as pointer to BUFFER+5, destination
	rcall	copy_flash2sram			; copy_flash2sram  Z -> Y, X == size

	movw	addr_F_H:addr_F_L, Z	; points to first byte in memory, after last access

#ifdef ENABLE_READ_EEPROM
	rjmp	common_read_finalization
#endif


#endif ;ENABLE_READ_FLASH

#ifdef ENABLE_READ_EEPROM

; --------------------------------------- process command CMD_READ_EEPROM_ISP
PMSG_CMD_READ_EEPROM_ISP:
	ldi		outMsgSize_L, 3			; high byte is 0
	add		outMsgSize_L, buf2		; DataSize_L, DataSize +3
	adc		outMsgSize_H, buf1		; DataSize_H

	movw	Z, addr_E_H:addr_E_L	; use Z as pointer to memory, source

do_eeprom_read:
	rcall	common_eeprom
	sbi		EECR, EERE
	in		temp1, EEDR
	st		Y+, temp1
	brne	do_eeprom_read 			; from last SBIW instruction
	
	movw	addr_E_H:addr_E_L, Z	; points to first byte in memory, after last access

#endif ;ENABLE_READ_EEPROM

#if defined(ENABLE_READ_EEPROM) || defined(ENABLE_READ_FLASH)

common_read_finalization:
	st		Y+, r_STATUS_CMD_OK
	rjmp	PMSG_END

#endif

; --------------------------------------- process command CMD_PROGRAM_FLASH_ISP
; it seems that one command for one page programming issued
; so assume one page <=256 bytes
; no care about "page write" bit 
PMSG_CMD_PROGRAM_FLASH_ISP:
	
#ifdef PROTECT_BOOTSECTION
	ldi		temp1, low(BL_START_WORD*2)
	cp		addr_F_L, temp1
	ldi		temp1, high(BL_START_WORD*2)
	cpc		addr_F_H, temp1
	brsh	PMSG_RESULT_FAIL
#endif

	movw	Z, addr_F_H:addr_F_L	; points to first byte in FLASH to programm
	adiw	Y, 8					; BUFFER+5 +10

#ifndef ENABLE_CHIP_ERASE_FLASH
	rcall	do_PE_SPM				; do page erase pointed by Z + SPM + WAIT
#endif

// do_SPM:
// INPUT: spm_cmd in r24(spm_cmd), Z, r0, r1

fill_flash_buffer:
	ld		r0, Y+
	ld		r1, Y+
	ldi		spm_cmd, (1<<SPMEN)
	rcall	do_SPM
	adiw	Z, 2
	sbiw	X, 2
	brne	fill_flash_buffer

	movw	X, Z					; points to first byte in memory, after last access
	movw	Z, addr_F_H:addr_F_L	; Z to address for page programming

	movw	addr_F_H:addr_F_L, X	; points to first byte in memory, after last access


	ldi		spm_cmd, (1<<PGWRT) | (1<<SPMEN)	; do page write pointed by Z
	rcall	do_SPM

	ldi		spm_cmd, (1<<RWWSRE) | (1<<SPMEN)	; RWW enable after page erase and programming
	rcall	do_SPM

	rjmp	PMSG_END


; ---------------------------------------


#ifdef ENABLE_PROGRAM_LOCK
; --------------------------------------- process command CMD_PROGRAM_LOCK_ISP
PMSG_CMD_PROGRAM_LOCK_ISP:

;	ldi		ZL, GET_LOCK_BITS	; recomended for future compatibility (ATmega16A datasheet)
;	clr		ZH					; recomended for future compatibility (ATmega16A datasheet)

	andi	buf4, 0b11001111	; keep boot block protection
	ori		buf4, 0b11000011	; keep boot block protection
	mov		r0, buf4
	rcall	do_SPM				; spm_cmd set before

	mov		temp1, r_STATUS_CMD_OK
	rjmp	PMSG_END_store_Y_temp1_size3



#endif ; ENABLE_PROGRAM_LOCK


#ifdef ENABLE_READ_LOCK_FUSE

; --------------------------------------- process command CMD_READ_LOCK_ISP
PMSG_CMD_READ_LOCK_ISP:
	ldi		ZL, GET_LOCK_BITS
	rjmp	common_read_fuse_lock


; --------------------------------------- process command CMD_READ_FUSE_ISP
PMSG_CMD_READ_FUSE_ISP:
	ldi		ZL, GET_HIGH_FUSE_BITS
	cpi		buf2, 0x50
	brne	common_read_fuse_lock
	cpi		buf3, 0x08
	brne	PMSG_CMD_READ_FUSE_ISP_low_fuse
	ldi		ZL, GET_EXTENDED_FUSE_BITS
	rjmp	common_read_fuse_lock
PMSG_CMD_READ_FUSE_ISP_low_fuse:
	ldi		ZL, GET_LOW_FUSE_BITS
common_read_fuse_lock:
	clr		ZH		; LOCK and FUSE operation needs ZH = 0
	out		SPMCR, spm_cmd	; spm_cmd set befor
	lpm		temp1, Z

#endif ;ENABLE_READ_LOCK_FUSE


PMSG_END_store_Y_temp1_size4:
	ldi		outMsgSize_L, 4
	rjmp	PMSG_END_store_Y_temp1



; --------------------------------------- process command CMD_GET_PARAMETER
PMSG_CMD_GET_PARAMETER:
	ldi		temp1, 0
	cpi		buf1, PARAM_SW_MAJOR
	breq	label3
	cpi		buf1, PARAM_HW_VER
	brne	label4
label3:
	ldi		temp1, CONFIG_PARAM_HW_VER	;  CONFIG_PARAM_HW_VER == CONFIG_PARAM_SW_MAJOR
label4:
	cpi		buf1, PARAM_SW_MINOR
	brne	PMSG_END_store_Y_temp1_size3
	ldi		temp1, CONFIG_PARAM_SW_MINOR
PMSG_END_store_Y_temp1_size3:
	ldi		outMsgSize_L, 3
	rjmp	PMSG_END_store_Y_temp1


#ifdef ENABLE_SPI_MULTI
; --------------------------------------- process command CMD_SPI_MULTI
PMSG_CMD_SPI_MULTI:
	cpi		buf4, 0x30
	brne	PMSG_RESULT_FAIL

	st		Y+, zero_reg			; Set BUFFER+5[2]
	st		Y+, buf4				; Set BUFFER+5[3]
	ldd		temp1, Y+1				; Get BUFFER+5[5]
	st		Y+, temp1				; Set BUFFER+5[4]
	ldd		buf4, Y+1				; Get BUFFER+5[6]
	std		Y+1, r_STATUS_CMD_OK	; Set BUFFER+5[6]

	rcall	load_signature_byte		; read signature byte number 'buf4' to 'temp1'
	ldi		outMsgSize_L, 7
	rjmp	PMSG_END_store_Y_temp1

#endif

#if defined(ENABLE_CHIP_ERASE_RESET_ADDRESS) || defined(ENABLE_CHIP_ERASE_FLASH) || defined(ENABLE_CHIP_ERASE_EEPROM)
PMSG_CMD_CHIP_ERASE:

#ifdef ENABLE_CHIP_ERASE_RESET_ADDRESS
	clr		addr_E_H
	clr		addr_E_L
	clr		addr_F_L
	clr		addr_F_H
#endif

#ifdef ENABLE_CHIP_ERASE_FLASH
	ldi		ZH, high(BL_START_WORD*2-PAGESIZE)
	ldi		ZL, low(BL_START_WORD*2-PAGESIZE)
	ldi		temp1, high(PAGESIZE)

do_page_erase:
	rcall	do_PE_SPM				; do page erase pointed by Z + SPM + WAIT

	subi	ZL, low(PAGESIZE)
	sbc		ZH, temp1
	brne	do_page_erase


#endif

#ifdef ENABLE_CHIP_ERASE_EEPROM
	clr		ZH
	clr		ZL
	ldi		XH, high(EEPROMEND)
	ldi		XL, low(EEPROMEND)
	ldi		temp1, 0xFF

do_eeprom_write2:
	rcall	common_eeprom
	out		EEDR, temp1
	sbi		EECR, EEMWE
	sbi		EECR, EEWE
do_eeprom_write_wait2:
	sbic	EECR, EEWE
	rjmp	do_eeprom_write_wait2
	brne	do_eeprom_write2			; from last SBIW instruction

#endif

	rjmp	PMSG_END
#endif


; --------------------------------------- process commands CMD_ENTER_PROGMODE_ISP, CMD_SET_PARAMETER
PMSG_RESULT_OK:

; --------------------------------------- Default -> fail 
PMSG_RESULT_FAIL:
	ldi		temp1, STATUS_CMD_FAILED
PMSG_END_store_Y_temp1:
	st		Y, temp1


PMSG_END:

;.undef command
.undef buf3
;.undef buf4



; zero_reg = r10
.undef	zero_reg
.def	checksum = r10
; so, checksum is already zeroed

.def	out_char = r17

SEND_MESSAGE:

	ldi		YL, low(BUFFER)
	ldi		YH, high(BUFFER)
;									; Y is pointer in BUFFER, prepare whole output buffer
									; answer part is filled in "processing command" section of code
									; fill starting part
	ldi		temp1, MESSAGE_START
	st		Y, temp1			; BUFFER[0]
	std		Y+1, seqNum			; BUFFER[1]
	std		Y+2, outMsgSize_H	; BUFFER[2]
	std		Y+3, outMsgSize_L	; BUFFER[3]
	ldi		temp1, TOKEN
	std		Y+4, temp1			; BUFFER[4]

	movw	Z, outMsgSize_H:outMsgSize_L
	adiw	Z, STARTING_SIZE
								

local_loop2:
	ld		out_char, Y+
	eor		checksum, out_char

#ifdef USART_STD_SEND
	UART_putChar out_char
#else

	out 	UART_DATA_REG, out_char
USART_Transmit:
	sbis	UART_STATUS_REG, UART_DATA_REGISTER_EMPTY
	rjmp	USART_Transmit
#endif


	sbiw	Z, 1
	brne	local_loop2

#ifdef USART_STD_SEND
	UART_putChar checksum
#else

	out 	UART_DATA_REG, checksum		; transmit buffer is empty here because of wait before
										; skip waiting for end of transmition because this is last
										; char of message...

#endif


;.undef out_char
.undef checksum

	inc		seqNum

#ifdef ENABLE_LEAVE_BOOTLOADER
	cpi		command, CMD_LEAVE_PROGMODE_ISP
	brne	continue_loop

#ifndef REMOVE_BOOTLOADER_LED

; switch off LED
#ifdef PROGLED_ACTIVE_HIGH
    cbi		PROGLED_PORT, PROGLED_PIN	; PROG_PIN pulled low
#else
	sbi		PROGLED_PORT, PROGLED_PIN	; PROG_PIN pulled high, 
#endif
    cbi		PROGLED_DDR, PROGLED_PIN	; set pin direction to "input"

#endif ; REMOVE_BOOTLOADER_LED

	rjmp	LEAVE_BOOTLOADER

continue_loop:

#endif ; ENABLE_LEAVE_BOOTLOADER


// ========================================== MAIN LOOP end ================================================
	rjmp	MAIN_LOOP

// ------------------------------------------ subroutines --------------------------------------------------	

do_PE_SPM:
	ldi		spm_cmd, (1<<PGERS) | (1<<SPMEN)	; do page erase pointed by Z

; INPUT: spm_cmd in r24(spm_cmd), Z, r0, r1
do_SPM:
	out		SPMCR, spm_cmd
	spm
do_SPM_wait:
	in		spm_temp, SPMCR
	sbrc	spm_temp, SPMEN	; "sbrc" test bit by its number
	rjmp	do_SPM_wait
	ret


common_eeprom:
	out		EEARL, ZL
	out		EEARH, ZH
	adiw	Z, 1
	sbiw	X, 1
	ret


#ifdef ENABLE_READ_FLASH

copy_flash2sram:
; INPUT:	*Z -> *Y, X == byte_size
; OUTPUT: 	Z = Z + byte_size
;			Y = Y + byte_size
;			X = 0
; USE:		r0
	lpm		r0, Z+
	st		Y+, r0
	sbiw	X ,1
	brne	copy_flash2sram
	ret

#endif

#ifdef ENABLE_SPI_MULTI
load_signature_byte:		; read signature byte number 'buf4'(=r16) to 'temp1'
	ldi		ZL, low(  (SIGN_B*2) + 1 )
	ldi		ZH, high( (SIGN_B*2) + 1 )
	add		ZL, buf4		; signature bytes placed within one 256-byte page
	lpm		temp1, Z
	ret
#endif


; avoid space wasting by placing even number of bytes on labels
; begining of sign-message
SIGN_MSG:	.DB		8,'A','V','R','I','S','P','_'
; last char of sign-message and signatuge bytes of MCU
SIGN_B:		.DB		'2', SIGNATURE_000, SIGNATURE_001, SIGNATURE_002


CMD_TBL:
;                                           0x00	; unsupported commands answer as FAIL
;define CMD_SIGN_ON                         0x01
	.DB	low(PMSG_RESULT_FAIL), low(PMSG_CMD_SIGN_ON)

;define CMD_SET_PARAMETER                   0x02	; just answer OK
;define CMD_GET_PARAMETER                   0x03
	.DB	low(PMSG_END),		low(PMSG_CMD_GET_PARAMETER)

;define CMD_SET_DEVICE_PARAMETERS           0x04	; unsupported commands answer as FAIL
;define CMD_OSCCAL                          0x05	; unsupported commands answer as FAIL
	.DB	low(PMSG_RESULT_FAIL),	low(PMSG_RESULT_FAIL)

;define CMD_LOAD_ADDRESS                    0x06
;define CMD_FIRMWARE_UPGRADE                0x07	; unsupported commands answer as FAIL
	.DB	low(PMSG_CMD_LOAD_ADDRESS), low(PMSG_RESULT_FAIL)

;                                           0x08	; unsupported commands answer as FAIL
;                                           0x09	; unsupported commands answer as FAIL
	.DB	low(PMSG_RESULT_FAIL),	low(PMSG_RESULT_FAIL)

;                                           0x0A	; unsupported commands answer as FAIL
;                                           0x0B	; unsupported commands answer as FAIL
	.DB	low(PMSG_RESULT_FAIL),	low(PMSG_RESULT_FAIL)

;                                           0x0C	; unsupported commands answer as FAIL
;                                           0x0D	; unsupported commands answer as FAIL
	.DB	low(PMSG_RESULT_FAIL),	low(PMSG_RESULT_FAIL)

;                                           0x0E	; unsupported commands answer as FAIL
;                                           0x0F	; unsupported commands answer as FAIL
	.DB	low(PMSG_RESULT_FAIL),	low(PMSG_RESULT_FAIL)


;define CMD_ENTER_PROGMODE_ISP              0x10	; just answer OK
;define CMD_LEAVE_PROGMODE_ISP              0x11	; just answer OK
	.DB	low(PMSG_END),		low(PMSG_END)

;define CMD_CHIP_ERASE_ISP                  0x12
;define CMD_PROGRAM_FLASH_ISP               0x13
	.DB	low(POINTER_CMD_CHIP_ERASE),		low(PMSG_CMD_PROGRAM_FLASH_ISP)

;define CMD_READ_FLASH_ISP                  0x14
;define CMD_PROGRAM_EEPROM_ISP              0x15
	.DB	low(POINTER_CMD_READ_FLASH_ISP),	low(POINTER_CMD_PROGRAM_EEPROM_ISP)

;define CMD_READ_EEPROM_ISP                 0x16
;define CMD_PROGRAM_FUSE_ISP                0x17	; unsupported commands answer as FAIL
	.DB	low(POINTER_CMD_READ_EEPROM_ISP),	low(PMSG_RESULT_FAIL)

;define CMD_READ_FUSE_ISP                   0x18
;define CMD_PROGRAM_LOCK_ISP                0x19
	.DB	low(POINTER_CMD_READ_FUSE_ISP),	low(POINTER_CMD_PROGRAM_LOCK_ISP)

;define CMD_READ_LOCK_ISP                   0x1A
;define CMD_READ_SIGNATURE_ISP              0x1B
	.DB	low(POINTER_CMD_READ_LOCK_ISP),	low(PMSG_CMD_READ_SIGNATURE_ISP)

#ifdef ENABLE_SPI_MULTI
;define CMD_READ_OSCCAL_ISP                 0x1C	; unsupported commands answer as FAIL
;define CMD_SPI_MULTI                       0x1D
	.DB	low(PMSG_RESULT_FAIL),		low(PMSG_CMD_SPI_MULTI)
#endif
