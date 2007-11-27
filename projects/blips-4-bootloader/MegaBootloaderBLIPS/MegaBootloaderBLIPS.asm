
;***********************************************
; Atmel AVR Microprocessor 
;  Use this code with Atmel's AVR Studio
;   v3.1 Oct 07 added one line of code
;   V3.0
; Dec 06
; Test status
; mega8: 	tested
; mega16:	tested
; mega32:	tested
; mega128: 	tested
; mega168	tested
; m8535:	tested

; HOW TO CONFIGURE THIS FOR A PARTICULAR CHIP
; 1. Edit the .INCLUDE file name for the desired chip, below.
; 2. Edit the crystal clock and baud constants, below.
; 3. If the desired chip is one of the ones with a block of constants, below, you are done.
;    For dual-UART chips, edit the block of constants to choose between UART0 vs. UART1, using the crib sheet table
; 4. IF the desired chip is NOT one of the ones with a block of constants, below, then ...
;    Copy/Paste one of the existing blocks, similar to the chip you wish.
;    Edit the symbol names using the crib-sheet table below or the Atmel data sheet or the include file.
; 5. Assemble this file using AVRstudio with the second generation assembler
; 6. Use your ISP to set the fuses (if not already set) then burn the bootloader .hex file to the chip
; 7. Test the bootloader using the PC side software: BLIPS or AVRProg and a connection to the target
;    chip's serial port via a direct link to PC COMn or a LAN-to-ethernet device or Wireless LAN (use BLIPS on the PC)
; 8. The code as is loops for about 2 seconds after each RESET looking for an escape char from the UART at the.
;	 baud rate setup below.  If it times out, it executes the code at location 0. 
;	    You may want different logic, such as an I/O bit jumper. See code at label "boot_reset:"

;========================================================================================
;
;* This is a modified version of design note #032 'DN032' From http://www.avrfreaks.com
;** Modified by stevech@san.rr.com aka childresss@gmail.com
; 	1. Support BLOCK MODE TRANSFERS FOR FLASH WRITE/READ/VERIFY (EEPROM is byte-at-a-time)
;	2. Support RAMPZ bit for >64K Byte flash chips
;	3. Changed code so that switching chips doesn't affect code, just constants (see below)
;
; Conforms to Atmel's original serial bootloader protocol as in app note AVR109
; PC side can be any that conforms to this protocol, e.g., AVRprog.exe, a command line program
;  or a graphical user interface program such as BLIPS (see AVRFreaks.com)
; Protocol commands supported
;  
;
; ========= Previous authors' comments...
;* It is meant specifically for atmega-8 and avrprog.exe 1.37 or better, or equiv.
;* To write boot with an ISP, set the fuses: Boot Flash Section Size = 256 WORDS; Boot Reset vector enabled

;* The original DN did not seem to work properly with a atmega8, after some debugging it 
;* appeard that the problem was that the code did not re-enable RWW access after page write or erase. Fixed.
;* Notes:
;* Added an esc char test at the beginning of the code. Modify as necessary.
;* I left in the eeprom program counter.
;* Be sure to use avrprog.exe 1.37 or a version that supports atmega8 (the regular download from atmel is 1.33)
;* While debugging I also discovered that avrprog support a baud rate of 115200, seems to work okay.
;* Fuses: BLB11 BOOTSZ1 BOOTRST
;* comments or questions?  carlott@si.rr.com   http://users.rcn.com/carlott/

;********** B O O T L O A D E R **********
;* Assembler : AVR Studio

; the below have #ifdef conditional assembly directives later in the code:
;;;#define ENABLE_READFUSEANDLOCK		; comment-out to omit support fuse and lock bit read/write
;;;#define NONESUCH						; See disabled unecessary code for this


;================ CHIP DEPENDENT CONFIGURATION ITEMS ==============================
;===================PLEASE KEEP THE BLOCKS IN ALPHANUMERIC ORDER BY CHIP NAME======
;==================================================================================

; CHANGE the include file to choose the desired chip's include file.
;   Include file directory: C:\Program Files\Atmel\AVR Tools\AvrAssembler2\Appnotes

.INCLUDE "m128def.inc"	; <<< CHANGE THIS according to chip type <<<<<<<<<<<<<<<<<<<<<<<<<<
						; for example "m128def.inc"

; CHANGE clock 
.equ	clock 			= 16000000	; CPU clock in Hz, given the crystal in use
; CHANGE BAUD. CHANGE UART0 vs. UART1 in one of the blocks of constants, below
.equ	bootldr_baud	= 57600		; Baud rate. Note: PC side software may have limitations on choices

	 ; 
;		as of now, the code is about 486 bytes long; stevech did lots of work to fit in 512 

; these come from the .INCLUDE file
.equ 	SB1 = SIGNATURE_002 	; Atmel chip type Signature byte from include file
.equ 	SB2 = SIGNATURE_001 	; Atmel chip type Signature byte
.equ 	SB3 = SIGNATURE_000 	; Atmel chip type Signature byte (Atmel code)

; Optional Blinking LED while bootloader waits for PC to begin commands
.if	1			; change to 0 to disable blinking LED while boot waits for PC 
.equ	LED_DDR		=DDRB
.equ	LED_PORT	=PORTB
.equ	LED_BITNO	=0
.endif

; CHANGE THIS IF YOU WISH. Controls timeout inside this bootloader waiting for byte from UART
.equ	PCLOOPHZ = ((clock / 5) / 65536)	; Timeout loop's 16 bit counter part, in Hz
.equ	PCTIMEOUT	= PCLOOPHZ * 2	; desire is 0.5Hz outer loop (2 seconds)
							; 8 bits. An incrementing loop count for timeout waiting on PC to send download
							; 75 (decimal) gives 2 seconds with 14.746MHz crystal


;  CHANGE the constants below based on which chip to use. 
;  COPY one of the code blocks, below, to use as a template for a new type of chip.
;  the constants are summarized in the table below, and
;  are detailed in the relevant include file in C:\Program Files\Atmel\AVR Tools\AvrAssembler2\Appnotes



#ifdef _M8DEF_INC_	; is within the chosen include file 
;==========================================================
;==================== mega8 ===============================
;==========================================================
; where this bootloader code begins, Change if this code outgrows 512 bytes
.equ	MYORG =	SECONDBOOTSTART	; word address 0F80 = byte addr 1E00. SET FUSES accordingly, using an ISP device
; UART baud rate setup constant- formula varies by chip type
.equ	UBRRLval = (clock - 8 * bootldr_baud) / (16 * bootldr_baud) ; the 8* part helps rounding

;--- device code and signature bytes for a atmega8 with bootloader
.equ 	DT  	= 0x77	; atmega bootloader

; Chip-dependent names for UART registers (See Table, below)
.set	BAUDL 	=UBRRL		; baud rate low order bits
.set	UARTC	=UCSRB		; control
.set	USTAT	=UCSRA		; status
.set	UDATA	=UDR		; data I/O register
.set	URXC	=RXC		; rx char complete status bit number (data has arrived)
.set	UTXE	=UDRE		; tx data register is empty
.set	UTXC	=TXC		; tx complete status bit number
; values to use in configuring UART
.set	UARTENA	=((1<<RXEN) | (1<<TXEN))	; enable tx,rx for uart 0 or 1 (see table)
.set	SPMCSR	=SPMCR
.MACRO BOOTIDSTRING 
 	.DB "Mega8  ", 0	; change this 7 char string. bootloader ID string, information-only to PC
.ENDMACRO
;=========================================================
#endif


#ifdef _M16DEF_INC_	; is within the chosen include file 
;==========================================================
;==================== mega16 ==============================
;==========================================================
; where this bootloader code begins, Change if this code outgrows 512 bytes
.equ	MYORG =	SECONDBOOTSTART	; word address 0F80 = byte addr 1E00. SET FUSES accordingly, using an ISP device
; UART baud rate setup constant- formula varies by chip type
.equ	UBRRLval = (clock - 8 * bootldr_baud) / (16 * bootldr_baud) ; the 8* part helps rounding

;--- device code and signature bytes for a atmega8 with bootloader
.equ 	DT  	= 0x77	; atmega bootloader

; Chip-dependent names for UART registers (See Table, below)
.set	BAUDL 	=UBRRL		; baud rate low order bits
.set	UARTC	=UCSRB		; control
.set	USTAT	=UCSRA		; status
.set	UDATA	=UDR		; data I/O register
.set	URXC	=RXC		; rx char complete status bit number (data has arrived)
.set	UTXE	=UDRE		; tx data register is empty
.set	UTXC	=TXC		; tx complete status bit number
; values to use in configuring UART
.set	UARTENA	=((1<<RXEN) | (1<<TXEN))	; enable tx,rx for uart 0 or 1 (see table)
;;;;.set	SPMCSR	=SPMCR ; not needed for this chip
.MACRO BOOTIDSTRING 
 	.DB "Mega16 ", 0	; change this 7 char string. bootloader ID string, information-only to PC
.ENDMACRO
;=========================================================
#endif


#ifdef _M32DEF_INC_	; is within the chosen include file <<<<<<<<<<<<<<<<<<<<<<<<<<<
;==========================================================
;==================== mega8 ===============================
;==========================================================
; where this bootloader code begins, Change if this code outgrows 512 bytes
.equ	MYORG =	FIRSTBOOTSTART
; UART baud rate setup constant- formula varies by chip type
.equ	UBRRLval = (clock - 8 * bootldr_baud) / (16 * bootldr_baud) ; the 8* part helps rounding
;--- device code and signature bytes for a atmega8 with bootloader
.equ 	DT  	= 0x7F	; atmega bootloader

; Chip-dependent names for UART registers (See Table, below)
.set	BAUDL 	=UBRRL		; baud rate low order bits
.set	UARTC	=UCSRB		; control
.set	USTAT	=UCSRA		; status
.set	UDATA	=UDR		; data I/O register
.set	URXC	=RXC		; rx char complete status bit number (data has arrived)
.set	UTXE	=UDRE		; tx data register is empty
.set	UTXC	=TXC		; tx complete status bit number
; values to use in configuring UART
.set	UARTENA	=((1<<RXEN) | (1<<TXEN))	; enable tx,rx for uart 0 or 1 (see table)
.set	SPMCSR	=SPMCR
.MACRO BOOTIDSTRING 
 	.DB "Mega32 ", 0	; change this 7 char string. bootloader ID string, information-only to PC
.ENDMACRO
;=========================================================
#endif



#ifdef _M128DEF_INC_	; is within the chosen include file
;==========================================================
;==================== mega128 =============================
;==========================================================
; UART baud rate setup constant- formula varies by chip type
; where this bootloader code begins, Change if this code outgrows 512 bytes
.equ	MYORG =	FIRSTBOOTSTART ; NOTE! This chip doesn't support a boot memory area as small as 512Bytes; see fuses for it
.equ	UBRRLval = (clock - 8 * bootldr_baud) / (16 * bootldr_baud) ;the 8* part helps rounding
;--- device code and signature bytes for a atmega8 with bootloader
.equ 	DT  	= 0x44	; atmega bootloader
; Chip-dependent names for UART0 registers (See Table, below)
.set	BAUDL 	=UBRR0L		; baud rate low order bits
.set	UARTC	=UCSR0B		; control
.set	USTAT	=UCSR0A		; status
.set	UDATA	=UDR0		; data I/O register
.set	URXC	=RXC0		; rx char complete status bit number (data has arrived)
.set	UTXE	=UDRE0		; tx data register is empty
.set	UTXC	=TXC0		; tx complete status bit number
; values to use in configuring UART0
.set	UARTENA	=((1<<RXEN0) | (1<<TXEN0))	; enable tx,rx for uart 0 or 1 (see table)
.MACRO BOOTIDSTRING 
 	.DB "Mega128", 0	; change this 7 char string.  bootloader ID string, information-only to PC

.ENDMACRO
;=========================================================
#endif



#ifdef _M168DEF_INC_	; is within the chosen include file 
;==========================================================
;==================== meg168 ==============================
;==========================================================
; UART baud rate setup constant- formula varies by chip type
; where this bootloader code begins, Change if this code outgrows 512 bytes
.equ	MYORG =	SECONDBOOTSTART ; Use ISP to set fuses for 512 byte boot size (256 words)
.equ	UBRRLval = (clock - 8 * bootldr_baud) / (16 * bootldr_baud) ; the 8* part helps rounding

;--- device code and signature bytes for a atmega8 with bootloader
.equ 	DT  	= 0x77	; atmega bootloader

; Chip-dependent names for UART registers (See Table, below)
.set	BAUDL 	=UBRR0L		; baud rate low order bits
.set	UARTC	=UCSR0B		; control
.set	USTAT	=UCSR0A		; status
.set	UDATA	=UDR0		; data I/O register
.set	URXC	=RXC0		; rx char complete status bit number (data has arrived)
.set	UTXE	=UDRE0		; tx data register is empty
.set	UTXC	=TXC0		; tx complete status bit number
; values to use in configuring UART
.set	UARTENA	=((1<<RXEN0) | (1<<TXEN0))	; enable tx,rx for uart 0 or 1 (see table)
.MACRO BOOTIDSTRING 
 	.DB "Mega168", 0	; change this 7 char string. bootloader ID string, information-only to PC
.ENDMACRO
.set	SPMEN	=SELFPRGEN		;SPMEN to SELFPRGEN
.set	EEMWE	=EEMPE			;EEMWE to EEMPE
.set	EEWE	=EEPE			;EEWE to EEPE
;=========================================================
#endif


#ifdef _M8535DEF_INC_ ; is within the chosen include file <<<<<<<<<<<<<<<<<<<<<<<<<<<
;==========================================================
;=================== mega8535 =============================
;==========================================================
; where this bootloader code begins, Change if this code outgrows 512 bytes
.equ MYORG = SECONDBOOTSTART
; UART baud rate setup constant- formula varies by chip type
.equ UBRRLval = (clock - 8 * bootldr_baud) / (16 * bootldr_baud) ; the 8* part helps rounding
;--- device code and signature bytes for a atmega8 with bootloader
.equ DT = 0x64 ; atmega bootloader

; Chip-dependent names for UART registers (See Table, below)
.set BAUDL =UBRRL 	; baud rate low order bits
.set UARTC =UCSRB 	; control
.set USTAT =UCSRA 	; status
.set UDATA =UDR 	; data I/O register
.set URXC =RXC 		; rx char complete status bit number (data has arrived)
.set UTXE =UDRE 	; tx data register is empty
.set UTXC =TXC 		; tx complete status bit number
; values to use in configuring UART
.set UARTENA =((1<<RXEN) | (1<<TXEN)) ; enable tx,rx for uart 0 or 1 (see table)
.set SPMCSR =SPMCR
.MACRO BOOTIDSTRING
.DB "M8535  ", 0 ; change this 7 char string. bootloader ID string, information-only to PC
.ENDMACRO 
#endif



;==========================================================================================
; ASSUME THAT UART baud rate setup constant is always less than 256
.if HIGH(UBRRLval!=0)
.error "UBRRLval high register setup code needs to be added."
.endif


; AVR chip dependent {in/lds} versus {out/sts} automatic selection macros
;   The device include file must be used to define SRAM_START
; usage: InReg reg, addr
.macro INREG
    .if @1 < 0x40
        in @0, @1
    .elif ((@1 >= 0x60) && (@1 < SRAM_START))
        lds @0,@1
    .else
       .error "InReg: Invalid I/O register address"
    .endif
.endmacro

; usage: OutReg addr, reg
.macro OUTREG
    .if @0 < 0x40
        out @0, @1
    .elif ((@0 >= 0x60) && (@0 < SRAM_START))
        sts @0,@1
    .else
       .error "OutReg: Invalid I/O register address"
    .endif
.endmacro




.LISTMAC   ; expand macros in assembler output listing



.CSEG
.org	MYORG	; defined in one of the processor-dependent blocks, above

boot_reset:

;--- init stack
		ldi 	R24,low(RAMEND)		; constant is in the include file
		ldi 	R25,high(RAMEND)
		out 	SPL,R24
		out 	SPH,R25 			; SP = RAMEND

;--- init uart

		;ldi		R24,(1<<URSEL) | (1<<UCSZ1) | (1<<UCSZ0)	; 8 bits, 1 stop, async mode
		;out		UCSRC,R24
		
		; UBRRH is zero after a reset so no code here
		ldi 	R24,UBRRLval	; R24,(clock - 8 * bootldr_baud) / (16 * bootldr_baud) 	; Baud rate 
		OUTREG		BAUDL,R24
		; enable tx and rx
		ldi 	R24,UARTENA
		OUTREG 	UARTC,R24 			; Enable receiver & transmitter, 8-bit mode

.ifdef	LED_DDR
		ldi	R24,(1 << LED_BITNO)
		OUTREG	LED_DDR,R24			; v3.1
		OUTREG	LED_PORT,R24			; LED port and bit, LED will illuminate now
.endif

;--- following waits for esc char, if not received within a given period, jump to application code 
;--- about 1.5 seconds 	@ 7.3728 clock 
;--- delete or chose your own way of entering bootstrap code

		clr	R23		; used for PC timeout counters
		clr	R24		; R23:24 are a 16 bit counter incrementing R25 each roll-over (65536 iterations)
		ldi	R25,PCTIMEOUT		; R25 is increments up to PCTIMEOUT

; LOOP until a character other than <escape> is received or until too much time has elapsed.
;  Then go to user program at location 0.

boot_esc_lp:	
							;;rcall	nada		; kill some time
		dec		R23
		brne	boot_r_1
		dec		R24
		brne	boot_r_1		; 16 bit counter loop

.ifdef	LED_DDR
		OUTREG		LED_PORT,R25			; this makes LED blink visibly
.endif	
		dec		R25
		brne	boot_r_1

; --- TIMED OUT, STARTUP USER PROGRAM
boot_r_0:
		clr		R30				; ensure 0 if jump here from elsewhere
		clr		R31
		OUTREG	UARTC,R30			; disable UART
		ijmp					; timeout, jump to address zero

boot_r_1:
		INREG	R16,USTAT
		sbrs 	R16,URXC 			; Skip if UART byte has arrived
		rjmp 	boot_esc_lp			; not yet, loop

		INREG	R16,UDATA 			; get received data in R16
		cpi		R16,0x1b			; ESC?
		breq	boot_esc_lp			; 
		rjmp	L10					; decode 1st received char that's not an ESCAPE

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

b_main: rcall 	uartGet 			; repeat (R16 = uartGet)
		cpi 	R16,0x1B			; while (R16 == ESCAPE)
		breq 	b_main

L10:	cpi		R16,'a'				; if(R16=='a') 'a' = inquiry: Autoincrement supported?
		brne 	L12
		ldi 	R16,'Y' 			; Reply with Yes, autoincrement is quicker
		rjmp 	L70 				; uartSend(R16)


L12: 	cpi 	R16,'A' 			; else if(R16=='A') set write address
		brne 	L12x1

;--- get word address from UART, convert to byte address
		rcall 	uartGet
		mov 	R27,R16 			; R27 = address high byte
		rcall 	uartGet
		mov 	R26,R16 			; R26 = address low byte

		lsl 	R26 				; address=address<<1
		rol 	R27 				; convert from byte to word address; save bit 15 to carry

.ifdef RAMPZ
		; added this bst instruction for large flash support (stevech)
		clr		R16					; Carry unchanged by clr
		rol		R16					; get carry from previous ROL R27 into bit 0
		out		RAMPZ,R16			; bit 0 is word address bit 15
.endif
		rjmp 	send_cr 			; uartSend('\r')

; --- Block write command inquiry added June 2006 Steve Childress
L12x1:	cpi		R16,'b'				; block mode suport inquiry by host
		brne	L13

		ldi		R16,'Y'
		rcall	uartSend
		ldi		R16,high(PAGESIZE*2); this chip's flash buffer size in bytes
		rcall	uartSend
		ldi		R16,low(PAGESIZE*2)	
		rjmp	L70					; send and continue


L13:	cpi		R16,'B'				; 'B' is write block of EEPROM or FLASH data
		brne	L14

; --- Block write command added June 2006 Steve Childress
		rcall	L13x4				; get block size then E or F command
		cpi		R16,'F'
		brne	L13x5

	; R26:27 have byte address from assumed previous 'A' command
	; R28:29 have block size in bytes
L13x2:	rcall	uartGet				; get upper byte of flash word
		mov		R0,R16	
		rcall	uartGet
		mov		R1,R16				; word is now in R1:R0
		movw	R30,R26				; Z register must hold byte addr from the 'A' command       
		ldi		R19,(1<< SPMEN)		; setup for write to flash buffer
		OUTREG	SPMCSR,R19				; must set this bit 
		spm							; store r0:r1 to chip's special flash buffer
		adiw	r26,2				; byte address
		sbiw	r28,2				; block size counter
		brne	L13x2				; loop for all bytes in the block
	; Write block - R30:31 have last used addr within block
		rjmp	L30					; do the page write and reply to PC

	; subroutine common to 'B' and 'g' commands. 
	; get block size from uart, but in R28:29. Get next uart byte in R16 and return
L13x4:		
		rcall 	uartGet				; block size high order byte
		mov		R29,R16
		rcall 	uartGet				; block size low order byte
		mov		R28,R16				;  have block size
		rjmp 	uartGet				; next is 'F' for Flash data or 'E'


L13x5:	
;;; Block mode EEPROM not supported (makes bootloader smaller)
;;;		cpi		R16,'E'				; 'E' for block mode EEPROM data
;;;		brne	L14				; 
;;;		rjmp	cmdUnknown			; NOT IMPLEMENTED - will wind up at cmdUnknown
	

L14: 	cpi 	R16,'c' 			; else if(R16=='c') write program memory, low byte
		brne 	L16
;---  get program memory low byte from UART
		rcall 	uartGet
		mov 	R22,R16 			; R22 = data low byte, keep for use for the next "C" command
		rjmp 	send_cr 			; uartSend('\r')

;---  get program data high byte from UART and store in on-chip buffer 
L16: 	cpi 	R16,'C' 			;else if(R16=='C') write program memory,high byte
		brne 	L17

		rcall 	uartGet
		mov 	R23,R16 			; R23 = data high byte. R22 has low byte from the "c" command

		movw 	R30,R26 			; Z pointer = byte address
		movw 	R0,R22 				; R0&R1 = data
	
		ldi 	R24,1 				; SPMCR = 0x01
		OUTREG	SPMCSR,R24			; page load (fill temporary buffer)
		spm 						; Store program memory

		adiw 	R26,2 				; address+=2   A new "A" command should come after addr rollover to 0
		rjmp 	send_cr 			; uartSend('\r')

L17:
		cpi		R16,'g'				; read block?
		brne	L18


; --- 'g' comamnd read flash block added. Steve Childress June 2006
		rcall	L13x4				; get block size
		cpi		R16,'F'				; flash?
		brne	L17x6
		movw	R30,R26				; get address in R30:31 (Z)
L17x2:
.ifdef RAMPZ						; chips with more than 64K Bytes of
		elpm 	R16,Z+ 				; read program memory MSB; store MSB in R16 and Z pointer ++
		; this reads just one block so if Z rolls over to 0 it doesn't matter as there's a "A" command every block
		rcall 	uartSend 			; uartSend(R16) MSB
		elpm 	R16,Z+ 				; read program memory LSB
.else
		lpm 	R16,Z+ 				; read program memory MSB; store MSB in R16 and Z pointer ++
		rcall 	uartSend 			; uartSend(R16) MSB
		lpm		R16,Z+				; read program memory LSB
.endif
		rcall 	uartSend 			; uartSend(R16) LSB+
		sbiw	R28,2				; R28:29 is counter based on block size
		brne	L17x2				; loop while using Z register

		movw	R26,R30				; update for auto-incrm
		rjmp	b_main				; wait for another command like 'g'

L17x6:
;;;		cpi		R16,'E'				; 'E' for block mode EEPROM data			; 
;;;		rjmp	cmdUnknown			; NOT IMPLEMENTED - will wind up at cmdUnknown


;--- chip erase?
L18: 	cpi 	R16,'e' 			; else if(R16=='e') Chip erase
		brne 	L28
		
;  erase all flash, page at a time

		clr		zl					; word address 0
		clr		zh
L19:
		lsl		zl					; get byte address within 64KByte space of flash
		rol		zh
.ifdef RAMPZ
		clr		R16					; doesn't affect carry
		rol		R16					; get 16th bit of word addr from carry to bit 0
		out		RAMPZ,R16			; and to bit 0 of RAMPZ
		ror		R16					; put it back into carry
.endif 				
		ldi		R17,( (1<<PGERS) | (1<<SPMEN) )	  ; page_erase
		OUTREG	SPMCSR,R17 
		spm
		rcall	wait_spm			; wait for done; preserving C bit
		ror		zh					; back to word addr, C held 16th word addr bit
		ror		zl						
								
		subi 	zl,low(-(PAGESIZE)) 		; address +=  ; PAGESIZE is in words
		sbci 	zh,high(-(PAGESIZE))		; subtract negative PAGESIZE in words = add

		ldi		R24,low(MYORG)		; is a word addr
		ldi		R25,high(MYORG)
		cp		zl,R24				; compare 16 bit word addr in Z, spans 128KByte Flash
		cpc		zh,R25
		brlo	L19	
		rjmp 	send_cr				; uartSend('\r')

;===========================================
#ifdef	NONESUCH	; old version not right for 128KB flash
		clr 	R26 				; start at addr 0
		clr 	R27
		rjmp	L24				; test for end

L20: 	movw 	R30,R26 			; Z-pointer = byte address
		ldi 	R24,3 				; SPMCR = 0x03
		WRITEPMCR R24 				; page_erase
		spm 						; Store program memory
									; do the page erase
		rcall	wait_spm			; wait for done
		subi 	R26,low(-2*PAGESIZE) 		; address += (2*PAGESIZE)
		sbci 	R27,high(-2*PAGESIZE)		; subtract negative PAGESIZE = add

L24: 	ldi 	R24,low(2*MYORG)
		ldi 	R25,high(2*MYORG)
		cp 		R26,R24 			; address < Boot Flash address(byte address) 0x3E00 ?
		cpc 	R27,R25
		brlo 	L20					; loop for all flash blocks up to the bootloader area
#endif
;===========================================

;=======
#ifdef EEPROM_ERASE_e	; this legacy code doesn't loop, doesn't work, isn't useful
; erase EEPROM
		ldi 	R26,low(E2END-1) 		; increment Chip Erase Counter located at address E2END-1
		ldi 	R27,high(E2END-1) 		; 
		movw 	R22,R26 			; Save Chip Erase Counter Address in R22
		
		ldi 	R17,1 				; read EEPROM
		rcall 	EepromTalk
		
		mov 	R24,R16 			; R24 = Chip Erase Counter low byte
		rcall 	EepromTalk
		mov 	R25,R16 			; R25 = Chip Erase Counter high byte
		adiw 	R24,1 				; counter ++
		out 	EEDR,R24 			; EEDR = R24 Chip Erase Counter low byte
		movw 	R26,R22 			; R26 = Chip Erase Counter Address
		
		ldi 	R17,6 				; write EEPROM
		rcall 	EepromTalk
		out 	EEDR,R25 			; EEDR = R25 Chip Erase Counter high byte
		rcall 	EepromTalk
		rjmp 	send_cr				; uartSend('\r')
#endif
;=======




;--- command: page write for byte-at-a-time mode
L28: 	cpi 	R16,'m' 			; else if(R16== 'm') Write page
		brne 	L34
		movw 	R30,R26 			; Z-pointer = address

; !!note: 'B' command shares this code too
L30:	ldi 	R17,5 				; SPMCR = 0x05 Write page
		OUTREG	SPMCSR,R17
		spm 					; Store program memory
		rcall	wait_spm
L32: 	rjmp 	send_cr 			; uartSend('\r')
				

;--- enter programming mode
L34:	cpi 	R16,'P' 			; else if(R16=='P') Enter programming mode
		breq 	L32 				; uartSend('\r')

		cpi 	R16,'L' 			; else if(R16=='L') Leave programming mode
		breq 	L32 				; uartSend('\r')

;--- Exit boot loader to user program
		cpi		R16, 'E'			; leave bootloader
		brne 	L36

		;ldi	R16,(1<<UTXC)	; not necessary per data sheet on TXC status but
		;OUTREG	USTAT,R16
		ldi 	R16,0x0D			; uartSend('\r')
 		rcall 	uartSend 			; uartSend(R16)
 		
L35:	INREG	R16,USTAT
		sbrs 	R16,UTXC 			; wait for char to have been transmitted (see Data Sheet on TXC bit)
		rjmp 	L35		
		rjmp	boot_r_0			; EXECUTE USER PROGRAM

L36:
;--- return programing type
		cpi 	R16,'p' 			; else if (R16=='p') Return programmer type
		brne 	L38

		ldi 	R16,'S' 			; uartSend('S') Serial
		rjmp 	L70 				; uartSend(R16)

;--- read program memory for byte-at-a-time mode
L38: 	cpi 	R16,'R' 			; else if(R16=='R') Read program memory
		brne 	L40

		movw 	R30,R26 			; Z-pointer <= address
.ifdef RAMPZ
		elpm 	R24,Z+ 				; read program memory LSB; store LSB in R24 and Z pointer ++
		elpm	R16,Z+ 				; read program memory MSB; store MSB in R16 and Z pointer ++
.else
		lpm 	R24,Z+ 				; read program memory LSB; store LSB in R24 and Z pointer ++
		lpm 	R16,Z+ 				; read program memory MSB; store MSB in R16 and Z pointer ++
.endif
		rcall 	uartSend 			; uartSend(R16) MSB

		movw 	R26,R30 			; address from Z register (incremented now)
		mov 	R16,R24 			; LSB stored in R16
		rjmp 	L70 				; uartSend(R16) LSB


;--- eeprom write for byte-at-a-time mode
L40: 	cpi 	R16,'D' 			; else if (R16=='D') Write data to EEPROM
		brne 	L42

		rcall 	uartGet
		out 	EEDR,R16 			; EEDR = uartGet()

		ldi 	R17,6 				; write EEPROM
		rcall 	EepromTalk
		rjmp 	send_cr 			; uartSend('\r')

;--- eeprom read for byte-at-a-time mode
L42: 	cpi 	R16,'d' 			; else if (R16=='d') Read data from EEPROM
		brne 	L44

		ldi 	R17,1 				; read EEPROM
		rcall 	EepromTalk 			; R16 = EEPROM data
		rjmp 	L70 				; uartSend(R16)


L44:
;--- read fuse bits
#ifdef ENABLE_READFUSEANDLOCK 	
		cpi 	R16,'F' 			; else if(R16=='F') Read fuse bits
		brne 	L46

		clr 	R30 				; Z-pointer = 0000
		rjmp 	L50 				; rcall readFuseAndLock

;--- read lock bits
L46: 	cpi 	R16,'r' 			; else if(R16=='r') Read lock bits
		brne 	L48

		ldi 	R30,1 				; Z pointer = 0001
		rjmp 	L50 				; rcall readFuseAndLock

;--- read high fuse bits
L48: 	cpi 	R16,'N' 			; else if(R16=='N') Read high fuse bits
		brne 	L52
		ldi 	R30,3 				; Z-pointer = 0003

;--- read fuse and lock bits
L50: 	rcall 	readFuseAndLock
		rjmp	L70 				; uartSend(R16)
#endif

;--- return supported devices
L52:	cpi 	R16,'t' 			; else if(R16=='t') Return supported devices code
		brne 	L54

		ldi 	R16,DT 				; Device Type
		rcall 	uartSend 			; uartSend(DT) send Device Type

		clr 	R16
		rjmp 	L70 				; uartSend(0)


;---  grab another char and ignore
L54: 						; else if ((R16=='l')||(R16=='x')||(R16=='y')||(R16=='T'))
#ifdef NONESUCH
		cpi 	R16,'l' 			; 'l' = Write Boot Loader lockbits 
		breq 	L56
		cpi 	R16,'x' 			; 'x' = Set LED
		breq 	L56
		cpi 	R16,'y' 			; 'y' = Clear LED
		breq 	L56
#endif

		cpi 	R16,'T' 			; 'T' = Select device type (ignored)
		brne 	L60



L56: 	rcall 	uartGet 			; R16 = uartGet()
; YOU CAN INSERT LEDs CODE HERE
		rjmp 	send_cr				; uartSend('\r')

;--- send return software id
L60: 	cpi 	R16,'S' 			; else if (R16=='S') Return software identifier
		brne 	L62

boot_id:ldi 	R30,low(2*Soft_Id)
		ldi 	R31,high(2*Soft_Id)


L61: 	
; Send ID string on uart from flash
.ifdef RAMPZ	; as on mega128
		ldi		R16,(1 << RAMPZ0)	; 128MB Flash
		out		RAMPZ,R16			; string is in 2nd 64K block
		elpm	R16,Z+	
.else
		lpm 	R16,Z+
.endif
		tst 	R16
		breq 	L72 				; branch is end of string ((Z) == 0)
		rcall 	uartSend 			; else send char
		rjmp 	L61

;--- send version
L62: 	cpi 	R16,'V' 			; else if (R16=='V') Return Software Version
		brne 	L64

		ldi 	R16,'1' 			; uartSend('1')
		rcall 	uartSend
		ldi 	R16,'2' 			; uartSend('2')
		rjmp 	L70 				; uartSend(R16)

;--- send signature bytes
L64: 	cpi 	R16,'s' 			; else if (R16=='s') Return Signature Byte
		brne 	L66

		ldi 	R16,SB1 			; uartSend(SB1) Signature Byte 1
		rcall 	uartSend
		ldi 	R16,SB2 			; uartSend(SB2) Signature Byte 2
		rcall 	uartSend
		ldi 	R16,SB3 			; uartSend(SB3) Signature Byte 3
		rjmp 	L70 				; uartSend(R16)

;;; Unsupported command received
cmdUnknown:
L66:	ldi	R16,'?' 			; else uartSend('?')
		rjmp 	L70 				; uartSend(R16)

;--- send cr
send_cr: 	ldi 	R16,13 				; uartSend('\r')

;--- send byte via uart

L70: 		rcall 	uartSend 			; uartSend(R16)
L72: 		rjmp 	b_main

#ifdef ENABLE_READFUSEANDLOCK
readFuseAndLock:
		clr 	R31 				; Z pointer high byte = 0
		ldi 	R24,9 				; SPMCR = 0x09
		WRITEPMCR R24 			; read fuse and lock
		lpm 	R16,Z 				; read program memory
		ret
#endif

; --- EEPROM I/O
EepromTalk:		 				; if R17 == 6 then Write, if R17 == 1 then Read
		out 	EEARL,R26 			; EEARL = address low
		out 	EEARH,R27 			; EEARH = address high
		adiw 	R26,1 				; address++
		sbrc 	R17,1 				; skip if R17 == 1 (read Eeprom)
		sbi 	EECR,EEMWE 			; EEMWE = 1 (write Eeprom)
		out 	EECR,R17 			; EECR = R17 (6 write, 1 read)
L90:	sbic 	EECR,EEWE 			; wait until EEWE == 0
		rjmp 	L90
		in 		R16,EEDR 			; R16 = EEDR
		ret

; --- UART I/O

; ==== UART Send Byte
uartSend: 							; send R16
		INREG	R2,USTAT
		sbrs 	R2,UTXE 			; wait for empty transmit buffer (until UDRE==1)
		rjmp 	uartSend
		OUTREG 	UDATA,R16 			; UDR = R16, start transmission
		ret

; ==== UART Get Byte
uartGet:	
		INREG	R16,USTAT
		sbrs	R16,URXC 			; wait for incoming data (until RXC==1)
		rjmp 	uartGet
		INREG	R16,UDATA 			; return received data in R16
		ret

; --- Wait for previous SPM operation to finish 
;      then send SPM for RWWSRE
;      then do another SPM

; must preserve Carry bit
wait_spm:
		INREG 	R16,SPMCSR
		sbrc	R16,SPMEN		; skip if bit is clear
		rjmp	wait_spm
		ldi		R16,(1<<RWWSRE) | (1<<SPMEN) 
		OUTREG	SPMCSR,R16
		spm
		ret 


Soft_Id: 
	BOOTIDSTRING	; macro

; END OF CODE



