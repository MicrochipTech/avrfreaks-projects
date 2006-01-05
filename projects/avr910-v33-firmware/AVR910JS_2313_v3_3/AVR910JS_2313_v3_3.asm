;***************************************************************************
;*
;* Title	: AVR ISP (Auto adr inc, 19200bps)
;* Version	: 3.2
;* Last updated	: 17/Jan/05
;* Target	: AT90S2313 @ 4Mhz (orig. AT90S1200)
;* File		: AVR910_2313_v3_2.asm
;* Author(s)	: Ole Saether, Terje Frostad,
;*		  Ingar Fredriksen, Morten W. Lund
;*                Haakon Skar, Paal Kastnes
;*
;See original appnote for more info.
;*
;* HISTORY
;*  V3.3	05/Jan/06 (J.Samperi) Fixed bug with page mode. See note at w7:
;*
;*  V3.2	17/Jan/05 (J.Samperi)Added support for newer devices supported by AVRprog 1.4
;*			Some doubtfull or non working device codes commented out
;*			Make use on newer conditional assembler to switch between
;*			standard pinout for Mosi and Miso and the reversed pinout
;*			for the Silicon Chip kit (Australia)
;*
;*  V3.1	Modified by others to swap Miso and Mosi to original configuration
;*
;*	V3.0??	16/June/03 (J.Samperi)	This version works with AT90S2313 because it
;*			uses the LPM instruction and also uses the 
;*			hardware UART. Added lookup table for device
;*			codes and program mode. Device can be added or
;*			removed by simply changing parameters in one 
;*			location only.
;*
;*      V2.2c   03.06.07 (sjdavies)     Changed the 'x' and 'y' commands to make  
;*                                      them accept a parameter ('flushing' bugfix)
;*	V2.2b	02.08.16 (sc)		Swapped MOSI & MISO port bits
;*      V2.2    02.04.10 (sjdavies)     Ported to AT90S2313 running at 4MHz for
;*                                      Silicon Chip article.
;*      V2.2    02.01.23 (sjdavies)     Ported from a 4Mhz 1200 to an 8Mhz 8535.
;*                                      Serial IO modified to use hardware UART.
;*                                      Timing updated.
;*                                      Device specific modifications.
;*      V2.2    00.03.10 (pkastnes)     Added support for multiple new devices
;*	V2.1	98.10.26 (mlund)	New date marking.
;*					Removed support for AT90S1200C.
;*					Added support for AT90S4433A.
;*	V2.0	98.01.06 (mlund)	ATmega103 support
;*	V1.7	97.11.06 (mlund)	Universial command (':') implemented.
;*					Releases all pins when not in 
;*					programming mode.
;*	V1.6e	97.11.04 (mlund)	mega103 rev D support
;*	V1.6c	97.10.30 (mlund)	Auto incrementing / SPI sync
;*					also works for mega103.
;*      V1.6	97.09.09 (hskar)	Created Page Mode Version (mega103)
;*	V1.5	97.08.21 (mlund)	Modified / Bugfix / Major cleanup
;*	...	...			(no records)
;*	V?.?	97.03.15 (OS)		Created
;* 
;See original appnote for more info.
;* 
;* LEGEND:
;* -------
;* Signature - Device Signature Byte
;* Code      - Unique device code used by AVRProg to identify device
;* Flash     - Flash size in bytes
;* EEProm    - EEProm size in bytes
;* Lock      - Lockbits
;* Fuse      - Fusebits
;* PMode     - Indicates if device uses byte or page programming mode
;* 
;* R/W 		- Read and Write Access
;* R   		- Read Access Only
;* W   		- Write Access Only
;* NA  		- Not Accessible
;* 
;***************************************************************************
.nolist

.include "2313def.inc"

.list

.equ	silicon_chip=1	;If 1 Mosi and Miso pinout swapped
.equ	xtal_8mhz=0		;if 0 then 4MHz Xtal

;***************************************************************************
;*
;* CONSTANTS
;*	device codes
;*
;* DESCRIPTION
;*	The following device codes must be used by the host computer. Note
;*	that the device codes are arbitrary selected, they do not have any
;*	thing in common with the signature bytes stored in the device.
;*
;***************************************************************************

;See original appnote for more info.

;**** Revision Codes ****

.equ	SW_MAJOR = '3'		; Major SW revision number
.equ	SW_MINOR = '3'		; Minor SW revision number
.equ	HW_MAJOR = '1'		; Major HW revision number
.equ	HW_MINOR = '0'		; Minor HW revision number

;***************************************************************************
;*
;* MACROS
;*	Program Macros
;*
;* DESCRIPTION
;*	Change the following four macros if the RESET pin to the
;*	target moves and/or if the SCK/MISO/MOSO moves.
;*
;***************************************************************************

.equ	resetpin	= 4

.macro	set_reset
	sbi		portb,resetpin
.endm

.macro	clr_reset
	cbi		portb,resetpin
.endm

.macro	ddrd_init
	nop
;	sbi	ddrd,3
.endm

.macro	ddrb_init
	ldi		temp1,(0xFF ^ (1<<MISO))
	out		ddrb,temp1	; PB5 is input, the rest is output
.endm

.macro	ddrb_release
	ldi		temp1,(1<<resetpin)
	out		ddrb,temp1	; PB4 (RESET) is output, the rest is input
.endm

.macro	pulse_sck
	sbi		portb,SCK
	ldi		temp2,6		;value for 4MHz crystal
m0:	
	dec		temp2
	brne	m0
	cbi		portb,SCK
	ldi		temp2,3		;value for 4MHz crystal
m1:	
	dec		temp2
	brne	m1
.endm

;*****************
;* SPI Constants *
;*****************

.if	silicon_chip

.equ	MOSI	= 5		; Bit number on PORTB
.equ	MISO	= 6		; Bit number on PORTB

.else

.equ	MOSI	= 6		; Bit number on PORTB
.equ	MISO	= 5		; Bit number on PORTB
.endif

.equ	SCK	= 7			; Bit number on PORTB

;******************
;* UART Constants *
;******************
;

.if	xtal_8mhz
.equ	N = 25			; 19.2k data rate value (8MHz crystal)

.else

.equ	N = 12			; 19.2k data rate value (4MHz crystal)
.endif

;*****************************
;* Global Register Variables *
;*****************************
;New registers used for get_codes (V3.0)
.def	dev_code	= r1	;Device code returned by get_codes
.def	pgm_mode	= r2	;Program mode for device 0=byte mode
;							;<>0=page mode
.def	device		= r16	; Device code
.def	temp1		= r17
.def	temp2		= r18
.def	s_data		= r19	;SPI data
.def	u_data		= r20	;UART data
.def	addrl		= r21	;Low order byte of address
.def	addrh		= r22	;High order byte of address
.def	cmd3		= r23	;Used to be r30 (V3.0)
.def	rd_s_data	= r24	;Used to be r31 (V3.0)
.def	cmd			= r25	;Serial programming command
.def	count		= r26	;Time out variable for "enter programming mode"
.def	param1		= r27
.def	cmd1		= r28
.def	cmd2		= r29
;			=r30	;Leave Z register free for LPM
;			=r31	(V3.0)

;*********************
;* Interrupt Vectors *
;*********************

.CSEG
	rjmp	RESET		; Reset Handle


;Initialize UART.
u_init:	
	ldi		temp1,N		; set baud rate
	out		UBRR,temp1
	ldi		temp1,1<<TXEN|1<<RXEN ; initialize UART for TX and RX
	out		UCR,temp1
	ret
putc:
	sbis	USR,UDRE	; test for TX register empty
	rjmp	putc		; loop until TX empty
	out		UDR,u_data	; send the byte
	ret
getc:
	sbis	USR,RXC		; wait until a character has been received
	rjmp	getc
	in		u_data,UDR	; Read byte from the UART
	ret
delay:	
	ldi		temp2,0xff
dl:	dec		temp2
	brne	dl
	dec		temp1
	brne	delay
	ret

;***************************************************************************
;*
;* FUNCTION
;*	wrser
;*
;* DESCRIPTION
;*	 Software master SPI implementation.
;*
;*	 Writes and reads data to/from the target's SPI (programming) port. 
;*
;***************************************************************************

rdser:	
	clr		s_data
wrser:	
	ldi			temp1,8
	ldi		rd_s_data,0
wrs0:	
	rol		s_data
	brcc	wrs1
	sbi		portb,MOSI
	rjmp	wrs2
wrs1:	
	cbi		portb,MOSI
wrs2:	
	lsl		rd_s_data
	sbic	pinb,MISO
	ori		rd_s_data,1
	pulse_sck
	dec		temp1
	brne	wrs0
	mov		s_data,rd_s_data
	ret


;***************************************************************************
;*
;* FUNCTION
;*	spiinit (Enter programming mode)
;*
;* DESCRIPTION
;*	Initialize SPI interface on AVR or 'AT89 device.
;*
;***************************************************************************

spiinit:
	ddrd_init			; initialize port D
	ddrb_init			; initialize port B
	cbi		portb,SCK	; clear SCK
	set_reset			;	set RESET = 1
	ldi		temp1,0xff	;	delay(0xff);
	rcall	delay
	clr_reset			;	set RESET = 0
	ldi		temp1,0xff	; delay(0xff);
	rcall	delay
	ldi		s_data,0xac	; wrser(0xac);		// SPI write (byte 1)
	rcall	wrser
	ldi		s_data,0x53	; wrser(0x53);		// SPI write (byte 2)
	rcall	wrser
						; // SPI Synchronization (fix!)
	cpi		device,0x20	; if ( (device >= 0x20) && (device <= 0x7F) )
	brlo	s2
	tst		device
	brmi	s2
s0b:					; {
	ldi		count,32	;	count = 32;
s1:						;	do {
	rcall	rdser		;		if (rdser == 0x53)	// SPI read (byte 3)
	cpi		s_data,0x53
	breq	s3		;			break;
	ldi		s_data,0x00	;		wrser(0x00);		// SPI write (byte 4)
	rcall	wrser
	pulse_sck		;		pulse SCK
	ldi		s_data,0xac	;		wrser(0xac);		// SPI write (byte 1)
	rcall	wrser
	ldi		s_data,0x53	;		wrser(0x53);		// SPI write (byte 2)
	rcall	wrser
	dec		count		;	} while(--count);
	brne	s1
	rjmp	s3			; }
						; else
s2:						; {
	ldi		s_data,0x00	;	wrser(0x00);		// SPI write (byte 3)
	rcall	wrser
s3:						; }
	ldi		s_data,0x00	;	wrser(0x00);	// SPI write (byte 4)
	rcall	wrser
	ldi		temp1,0x10	; delay(0x10);
	rcall	delay
	ret

;Show our ID ("AVR ISP") on the serial line.
;@@ Changes (V3.0)
show_id:
	ldi		zl,low(ID*2)		;Point Z to ID string
	ldi		zh,high(ID*2)
	rcall	msg_out
	ret
;@@

RESET:
	ldi 	temp1,low(RAMEND)
	out 	SPL,temp1	; init stack
	clr		temp1
	out		GIMSK,temp1	; disable external interrupt
	ser		temp1		; Initialize
	out		PORTD,temp1
	set_reset			; set RESET=1
	out		PORTB,temp1
	ddrb_release
	rcall	u_init		; Initialize UART


;***************************************************************************
;*
;* PROGRAM
;*	waitcmd -> main
;*
;* DESCRIPTION
;*	Wait for and execute commands.
;*
;***************************************************************************

waitcmd:
	rcall	getc		; while (getc() == ESC) {};
	cpi		u_data,0x1b
	breq	waitcmd

;**** Device Type ****

	cpi		u_data,0x54     ; 'T' Device type
	brne	w0
	rcall	getc		; getc();	// dummy
	mov		device,u_data	; putc(device);
	rjmp	put_ret

;**** Return Software Identifier ****

w0:	
	cpi		u_data,0x53	; 'S' Return software identifier
	brne	w1
	rcall	show_id		; show_id();
	rjmp	waitcmd

;**** Return Software Version ****

w1:	
	cpi		u_data,0x56	;'V' Return software version
	brne	w2
	ldi		u_data,SW_MAJOR
	rcall	putc
	ldi		u_data,SW_MINOR
	rcall	putc
	rjmp	waitcmd


;**** Return Hardware Version ****

w2:	
	cpi		u_data,0x76	;'v' Return hardware version
	brne	w3
;*** deleted sd 02.04.10 - correct what looks like an Atmel error
;	ldi	u_data,0x30+HW_MAJOR ; putc(0x30+HW_MAJOR);
;*** end delete
;*** added sd 02.04.10 - correct what looks like an Atmel error
	ldi		u_data,HW_MAJOR ; putc(HW_MAJOR);
;*** end add
	rcall	putc
;*** deleted sd 02.04.10 - correct what looks like an Atmel error
;	ldi	u_data,0x30+HW_MINOR ; putc(0x30+HW_MINOR);
;*** end delete
;*** added sd 02.04.10 - correct what looks like an Atmel error
	ldi		u_data,HW_MINOR ; putc(HW_MINOR);
;*** end add
	rcall	putc
	rjmp	waitcmd

;**** Show Supported Devices ****
w3:	
	cpi		u_data,0x74	; 't' Show supported devices
	brne	w4
;@@ Changes (V3.0)
	ldi		zl,low(device_codes*2)	;Point Z to device code table
	ldi		zh,high(device_codes*2)
show_devices:
	rcall	get_codes		;Returns device code in dev_code
	mov		u_data,dev_code
	cpi		u_data,0xff		;Check for end of table
	breq	end_of_devices	;Finished
	rcall	putc			;Not finished, send out
	rjmp	show_devices
end_of_devices:
	clr		u_data
	rcall	putc			;End of device list
	rjmp	waitcmd
;@@

;**** Return Programmer Type ****
w4:	
	cpi		u_data,0x70     ; 'p' Return programmer type
	brne	w51
	ldi		u_data,0x53     ; putc('S');	// serial programmer
	rcall	putc
	rjmp	waitcmd
;**** Return autoincrement address support
w51:	
	cpi		u_data,'a'	; 'a' Return address auto increment
	brne	w5
	ldi		u_data,'Y'	; putc('Y'); 	// supports autoinc
	rcall	putc
	rjmp	waitcmd
w5:    
	cpi		u_data,0x78	; 'x' Set LED (ignored)
	brne    w6
	rcall	getc        	; ignore data byte
	rjmp    put_ret
;**** Clear LED ****
w6:    
	cpi		u_data,0x79     ; 'y' Clear LED (ignored)
	brne    w7
	rcall   getc		; ignore data byte
	rjmp    put_ret

;**** Enter Programming Mode ****
; We require that the device code be selected before any of the other commands
w7:
;@@ Changes (V3.0)** V3.3 uses temp2 instead of r16 in V3.0 as temp reg
						;r16 is already used for device and caused a bug

	push	temp2		;Save temp2 
	ldi		zl,low(device_codes*2)	;Point Z to device code table
	ldi		zh,high(device_codes*2)
w7_lp:
	rcall	get_codes	;Returns device code in dev_code
	mov		temp2,dev_code
	cpi		temp2,0xff	;Check for end of table
	breq	device_err	;Device code not found
	cp		temp2,device	;Found device code?
	breq	x_w7_lp		;Yes
	rjmp	w7_lp		;Next device code
x_w7_lp: 
	pop		temp2		;Restore temp2	
	rjmp	w72
device_err:
	pop		temp2		;Restore temp2
	rjmp	put_err		;Goto put_err();
;@@

w72:	
	cpi		u_data,0x50		; 'P' Enter programming mode
	brne	w8
	rcall	spiinit			; spiinit();
	rjmp	put_ret
;**** Write Program Memory, High Byte ****
w8:	
	cpi		u_data,0x43		; 'C' Write program memory, high byte
	brne	w9
	rcall	getc
w81:	
	ldi		s_data,0x48		; wrser(0x48);		// SPI write (byte 1)
	rcall	wrser
	mov		s_data,addrh	; wrser(addrh);		// SPI write (byte 2)
	rcall	wrser
	mov		s_data,addrl	; wrser(addrl);		// SPI write (byte 3)
	rcall	wrser

	mov		s_data,u_data	; wrser(u_data);	// SPI write (byte 4)
	rcall	wrser

	ldi		temp1,0x01		; Auto increment address!!!!
	clr		temp2
	add		addrl,temp1
	adc		addrh,temp2
	rjmp	writeFLASHdelay
;**** Write Program Memory, Low Byte ****
w9:	
	cpi		u_data,0x63     ; 'c' Write program memory, low byte
	brne	w12
	rcall	getc
	ldi		s_data,0x40		;	wrser(0x40);	// SPI write (byte 1)
	rcall	wrser
	mov		s_data,addrh	;	s_data = addrh;
w91:						; }
	rcall	wrser			; wrser(s_data);	// SPI write (byte 2)
	mov		s_data,addrl	; wrser(addrl);		// SPI write (byte 3)
	rcall	wrser
	mov		s_data,u_data	; wrser(u_data);	// SPI write (byte 4)
	rcall	wrser
writeFLASHdelay:
;@@ Changes (V3.0)
	tst		pgm_mode
	brne	w92		;No delay if page mode (non zero value)	
;@@
	ldi		temp1,0x20	; delay(0x20);		// 24585 cycles delay
	rcall	delay		;			// Page mode requires no delay!
w92:
	rjmp	put_ret		; goto reply();

;**** Read Program Memory ****

w12:	
	cpi	u_data,0x52	; 'R' Read program memory
	brne	w10		;
	ldi	s_data,0x28	;	wrser(0x28);	// SPI write (byte 1)
	rcall	wrser
	mov	s_data,addrh	;	s_data = addrh;
	rcall	wrser		; wrser(s_data);	// SPI write (byte 2)
	mov	s_data,addrl	; wrser(addrl);		// SPI write (byte 3)
	rcall	wrser
	rcall	rdser		; putc(rdser());	// Send data (byte 4)
	mov	u_data,s_data
	rcall	putc
	ldi	s_data,0x20	;	wrser(0x20);	// SPI write (byte 1)
	rcall	wrser
	mov	s_data,addrh	;	wrser(addrh);	// SPI write (byte 2)
	rcall	wrser
	mov	s_data,addrl	;	wrser(addrl);	// SPI write (byte 3)
	rcall	wrser
	rcall	rdser		;	putc(rdser());	// Send data (byte 4)
	mov	u_data,s_data
	rcall	putc
	rjmp	readaddrinc

;**** Load Address ****

w10:	
	cpi	u_data,0x41	; 'A' Load address
	brne	w11
	rcall	getc		; addrh = getc();
	mov	addrh,u_data
	rcall	getc		; addrl = getc();
	mov	addrl,u_data
	rjmp	put_ret		; goto reply();
;**** Write Data Memory ****
w11:	
	cpi		u_data,0x44	; 'D' Write data memory
	brne	w13
	rcall	getc
	ldi		s_data,0xc0
	rcall	wrser
	mov		s_data,addrh
	rcall	wrser
	mov		s_data,addrl
	rcall	wrser
	mov		s_data,u_data
	rcall	wrser
	ldi		temp1,0x20
	rcall	delay
	ldi		temp1,0x01	; Auto increment address!!!!
	clr		temp2
	add		addrl,temp1
	adc		addrh,temp2
	rjmp	put_ret

;**** Read Data Memory ****
w13:	
	cpi		u_data,0x64	; 'd' Read data memory
	brne	w14
	ldi		s_data,0xa0	;	wrser(0xA0);	// SPI write (byte 1)
	rcall	wrser
	mov		s_data,addrh	;	s_data = addrh;
	rcall	wrser		; wrser(s_data);	// SPI write (byte 2)
	mov		s_data,addrl	; wrser(addrl);		// SPI write (byte 3)
	rcall	wrser
	rcall	rdser		; putc(rdser());	// Send data (byte 4)
	mov	u_data,s_data
	rcall	putc
readaddrinc:
	ldi		temp1,0x01	; Auto increment address!!!!
	clr		temp2
	add		addrl,temp1
	adc		addrh,temp2
	rjmp	waitcmd		; goto waitcmd();
;**** Leave Programming Mode ****
w14:	
	cpi		u_data,0x4c	; 'L' Leave programming mode
	brne	w15
	ddrb_release
	set_reset			; set RESET = 1
	rjmp    put_ret
;**** Chip Erase ****
w15:	
	cpi		u_data,0x65     ; 'e' Chip erase
	brne	w16
	ldi		s_data,0xac
	rcall	wrser
	ldi		s_data,0x80
	rcall	wrser
	ldi		s_data,0x04
	rcall	wrser
	ldi		s_data,0x00
	rcall	wrser
	ldi		temp1,0x30
	rcall	delay
	rjmp	put_ret
;**** Write Lock Bits ****
w16:	
	cpi		u_data,0x6c	; 'l' Write lock bits
	brne	w17
	rcall	getc
	ldi		s_data,0xac
	rcall	wrser
	mov		s_data,u_data
	andi	s_data,0x06
	ori		s_data,0xe0
	rcall	wrser
	ldi		s_data,0x00
	rcall	wrser
w162:	
	ldi		s_data,0x00
	rcall	wrser
	ldi		temp1,0x30
	rcall	delay
	rjmp	put_ret
;**** Read Signature Bytes ****

w17:	
	cpi		u_data,0x73	; 's' Read signature bytes
	brne	w18
	ldi		param1,0x02
	rcall	w17call
	ldi		param1,0x01
	rcall	w17call
	ldi		param1,0x00
	rcall	w17call
	rjmp	waitcmd

w17call:
	ldi		s_data,0x30
	rcall	wrser
	ldi		s_data,0x00
	rcall	wrser
	mov		s_data,param1
	rcall	wrser
	rcall	rdser
	mov		u_data,s_data
	rcall	putc
	ret
;**** Write Program Memory Page ****
w18:	
	cpi		u_data,0x6D		; 'm' Write Program Memory Page
	brne	w19
	ldi		s_data,0x4c		;	wrser(0x4c);	// SPI write (byte 1)
	rcall	wrser
	mov		s_data,addrh	;	wrser(addrh);	// SPI write (byte 2)
	rcall	wrser
	mov		s_data,addrl	;	wrser(addrl);	// SPI write (byte 3)
	rcall	wrser
	ldi		s_data,0x00		;	wrser(0x00);	// SPI write (byte 4)
	rcall	wrser
	ldi		temp1,0xff		; 	delay(0xFF);	// 0x20 = 24585 cycles delay
	rcall	delay
	rjmp	put_ret
;**** Universal Command ****
w19:	
	cpi		u_data,0x3A		; ':' Universal Command
	brne	w191
	rcall	getc
	mov		cmd1,u_data
	rcall	getc
	mov		cmd2,u_data
	rcall	getc
	mov		cmd3,u_data
	clr		u_data
	rcall	NewUniversal
	ldi		temp1,0xff		; 	delay(0xFF);	// 0x20 = 24585 cycles delay
	rcall	delay
	rjmp	put_ret
;**** New Universal Command ****
w191:	
	cpi		u_data,'.'		; '.' New Universal Command
	brne	w99
	rcall	getc
	mov		cmd1,u_data
	rcall	getc
	mov		cmd2,u_data
	rcall	getc
	mov		cmd3,u_data
	rcall	getc
	rcall	NewUniversal
	ldi		temp1,0xff		; 	delay(0xFF);	// 0x20 = 24585 cycles delay
	rcall	delay
	rjmp	put_ret
NewUniversal:
	mov		s_data,cmd1
	rcall	wrser
	mov		s_data,cmd2
	rcall	wrser
	mov		s_data,cmd3
	rcall	wrser
	mov		s_data,u_data
	rcall	wrser
	mov		u_data,rd_s_data
	rcall	putc
	ret
w99:
;**** Command Error ****

put_err:
	ldi		u_data,0x3f		; putc('?');	\\ send '?'
	rcall	putc
	rjmp	waitcmd
;**** Reply Command ****
put_ret:
	ldi		u_data,0x0d	; putc(0x0D);	\\ send CR
	rcall	putc
	rjmp	waitcmd
delay1us:			;    count               R16
	nop                                                       
	cpi     temp1,0x03
	breq    finished
	nop
	nop
loop:   
	dec     temp1 
	cpi     temp1,0x03
	brne    loop               
finished:
	ret     

;@@ Changes (V3.0)

;MSG_OUT Output string of ASCII bytes starting at Z 
;until end of message ($00)
MSG_OUT:
    lpm					;read char into r0
    mov		u_data,r0	;Copy to r20
    cpi		u_data,0x00	;End of message?
    breq	END_MSG		;Yes
	rcall	PUTC		;output character
	adiw	ZL,1
	rjmp	MSG_OUT
END_MSG:
	ret

;get_codes return code in dev_code and program mode in pgm_mode
get_codes:
	lpm					;Read device code from table
	mov		dev_code,r0	;Copy to dev_code
	adiw	ZL,1		;Increment pointer
	lpm					;Read program mode from table
	mov		pgm_mode,r0	;Copy to pgm_mode
	adiw	ZL,1		;Increment pointer
	ret

;Programmer ID
ID:
.db	"AVR ISP",0

;Device codes and program mode table (0(zero)=byte mode, <>0=page mode)
;i.e. 'P' can be used for page mode for easier reading.

device_codes:

;AT90S1200 rev. A	;From 0x00-0x0f unused yet
	.db	0x10,0
;AT90S1200 rev. B	
	.db	0x11,0
;AT90S1200 rev. C
	.db	0x12,0
;AT90S1200
	.db	0x13,0		;From 0x14-0x1f unused yet
;AT90S2313
	.db	0x20,0		;From 0x21-0x27 unused yet
;AT90S4414
	.db	0x28,0		;From 0x29-0x2f unused yet
;AT90S4433
	.db	0x30,0		;From 0x31-0x33 unused yet
;AT90S2333
	.db	0x34,0		;From 0x35-0x37 unused yet
;AT90S8515
	.db	0x38,0		;0x39 unused yet
;ATmega8515
	.db	0x3A,'P'
;ATmega8515 BOOT
	.db	0x3B,'P'	;From 0x3c-0x40 unused yet
;ATmega103
	.db	0x41,'P'
;ATmega603
	.db	0x42,'P'
;ATmega128
	.db	0x43,'P'
;ATmega128 BOOT
	.db	0x44,'P'
;ATmega64
	.db	0x45,'P'	;* v1.40
;ATmega64 BOOT
	.db	0x46,'P'	;* v1.40  0x47 unused yet

;AT90S2323
	.db	0x48,0		;From 0x49-0x4b unused yet		
;AT90S2343
	.db	0x4C,0		;From 0x4d-0x4f unused yet
					;0x50,0x51 used. From 0x52-0x54 unused yet
;
;ATtiny12
	.db	0x55,0
;ATtiny15
	.db	0x56,0		;0x57 unused yet
;ATtiny19
	.db	0x58,0		;From 0x59-0x5b unused yet	
;ATtiny28
	.db	0x5C,0		;0x5d unused yet
;ATtiny26
	.db	0x5E,'P'	;0x5f unused yet
;
;ATmega161
	.db	0x60,'P'
;ATmega161 BOOT
	.db	0x61,'P'	;0x62-0x63 unused yet
;ATmega163
	.db	0x64,'P'
;ATmega83
	.db	0x65,'P'
;ATmega163 BOOT
	.db	0x66,'P'
;ATmega83 BOOT
	.db	0x67,'P'
;AT90S8535
	.db	0x68,0
;ATmega8535
	.db	0x69,'P'	;* v1.40 From 0x6a-0x6b unused yet	
;***ATmega8535 BOOT??
;	.db	0x6a,'P'
;
;AT90S4434
	.db	0x6C,0		;From 0x6d-0x6f unused yet	
;AT90C8534
	.db	0x70,0
;AT90C8544
	.db	0x71,0
;ATmega32
	.db	0x72,'P'
;ATmega32 BOOT
	.db	0x73,'P'
;ATmega16
	.db	0x74,'P'
;ATmega16 BOOT
	.db	0x75,'P'
;ATmega8
	.db	0x76,'P'
;ATmega8 BOOT
	.db	0x77,'P'
;ATmega169
	.db	0x78,'P'	;* v1.40
;ATmega169 BOOT
	.db	0x79,'P'	;* v1.40 From 0x7a-0x7f unused yet

;test
;	.db	0x08,'P'
;	.db	0x09,'P'
;	.db	0x0a,'P'
;	.db	0x0b,'P'

;These devices are not supported by this hardware
;ATtiny10
;	.db	0x51,0
;ATtiny11
;	.db	0x50,0
;AT89C1051
;	.db	0x80,0
;AT89C2051
;	.db	0x81,0		;From 0x82-0x85 unused yet
;AT89S8252
;	.db	0x86,0
;AT89S53
;	.db	0x87,0		;From 0x88-0x9f unused yet

end_of_device_codes:
	.dw	0xffff

;@@