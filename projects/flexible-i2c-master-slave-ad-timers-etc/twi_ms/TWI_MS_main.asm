
; Craig's AVR monitor/I2C project, ATmega128 this time.  All timings assume an 8 MHz clock.

; The point is to implement a completely reliable master and slave I2C using the AVR
;	TWI hardware.  Possible applications include use in applications requiring I2C interfaced
;	smart subsystems, IPMI, etc.

; The API for the I2C master transactions is intended to be much easier to use than the Atmel
;	application note model.  Everything except the UART is interrupt driven with reasonable attention to latency
;	so that a real application can be supported as well as the I2C interface.  The goal is for the
;	slave interface to always be active, i.e. no device address NACKs.
;
; Serial port 0 is used to implement a simple monitor that allows I2C reads and writes of various types.
;	 The serial port is configured for 9600 baud, 8 bit, no parity.
;
; The I2C slave interface implements a couple of bytes of status register, a couple of bytes of control
;	register, a fixed version register, and a group of A/D conversions just for fun.
;
; To avoid hangs, infinite loops, etc., the WD timer is used rather than implementing timers and escapes
;	in every loop.  WDR instructions are carefully allocated to avoid putting them where they should not be.
;
; 9/27/2006 - this initial version is based on previous project work and has a slave access failure
;	rate of about 1 in 200,000 accesses when there are device master bursts of about 20 accesses every second.
;   Failures include device addr NACKs, incorrect read data, and very rarely incorrect write data.
;
.NOLIST
.include "m128def.inc" 	;Includes the ATmega128 definitions file
.LIST
;
;	This is the software version number that is available from the I2C interface
;	The monitor startup string below should be edited to match this value
.equ	SW_Version = 0x01
;
.def Temp = R16 		
.def ASCII_Offset = R17		;just a handy place to keep a constant
.def count = R18
.def T2 = R19
.def T3 = R20
.def T4 = R21

.def I2C_Data = R8			;Registers used for the I2C master APIs
.def I2C_DataLB = R9
.def I2C_DevAddr = R22
.def I2C_Addr = R10

.equ ASCII_Num_Offset = 0x30
.equ ASCII_CR = 0x0D
.equ ASCII_LF = 0x0A

.equ ERROR = 0xFF


;Memory defs
.equ TopofStack = 0x10FF	;Memory address to initialize the top of stack to,
.equ ToS_HB = 0x10			; normally the top of internal memory, in this case for
.equ ToS_LB = 0xFF			; the ATmega128, but this is chip dependant.

.equ ToDS_HB = 0x0F			;set the top of the Data Stack to 0xFFF, allowing 64B for the call stack
.equ TODS_LB = 0xFF

;Variables, starting at lowest SRAM
.DSEG
.org	SRAM_START

;Monitor commands
.equ cmd_i2c_write_adr = 'W'
.equ cmd_i2c_write_byte = 'w'
.equ cmd_i2c_read_adr = 'R'
.equ cmd_i2c_read_byte = 'r'
.equ cmd_prt_GIR = 's'
.equ cmd_set_ctrl = 'S'
.equ cmd_prt_AD = 'p'

.CSEG
.org 0x0000 			;Places the following code from address 0x0000

	jmp RESET 			;Take a Relative Jump to the RESET Label

; ***** INTERRUPT VECTORS for Mega128 *******************************************
	nop
	reti			;	INT0addr	= 0x0002	; External Interrupt Request 0
	nop
	reti			;	INT1addr	= 0x0004	; External Interrupt Request 1
	nop
	reti			;	INT2addr	= 0x0006	; External Interrupt Request 2
	nop
	reti			;	INT3addr	= 0x0008	; External Interrupt Request 3
	nop
	reti			;	INT4addr	= 0x000a	; External Interrupt Request 4
	nop
	reti			;	INT5addr	= 0x000c	; External Interrupt Request 5
	nop
	reti			;	INT6addr	= 0x000e	; External Interrupt Request 6
	nop
	reti			;	INT7addr	= 0x0010	; External Interrupt Request 7
	nop
	reti			;	OC2addr		= 0x0012	; Timer/Counter2 Compare Match
	nop
	reti			;	OVF2addr	= 0x0014	; Timer/Counter2 Overflow
	nop
	reti			;	ICP1addr	= 0x0016	; Timer/Counter1 Capture Event
	jmp IRQ_1sec	;	OC1Aaddr	= 0x0018	; Timer/Counter1 Compare Match A
	nop
	reti			;	OC1Baddr	= 0x001a	; Timer/Counter Compare Match B
	nop
	reti			;	OVF1addr	= 0x001c	; Timer/Counter1 Overflow
	nop
	reti			;	OC0addr		= 0x001e	; Timer/Counter0 Compare Match
	nop
	reti			;	OVF0addr	= 0x0020	; Timer/Counter0 Overflow
	nop
	reti			;	SPIaddr		= 0x0022	; SPI Serial Transfer Complete
	nop
	reti			;	URXC0addr	= 0x0024	; USART0, Rx Complete
	nop
	reti			;	UDRE0addr	= 0x0026	; USART0 Data Register Empty
	nop
	reti			;	UTXC0addr	= 0x0028	; USART0, Tx Complete
	jmp A2D_ISR		;	ADCCaddr	= 0x002a	; ADC Conversion Complete
	nop
	reti			;	ERDYaddr	= 0x002c	; EEPROM Ready
	nop
	reti			;	ACIaddr		= 0x002e	; Analog Comparator
	nop
	reti			;	OC1Caddr	= 0x0030	; Timer/Counter1 Compare Match C
	nop
	reti			;	ICP3addr	= 0x0032	; Timer/Counter3 Capture Event
	jmp	IRQ_10ms	;	OC3Aaddr	= 0x0034	; Timer/Counter3 Compare Match A
	nop
	reti			;	OC3Baddr	= 0x0036	; Timer/Counter3 Compare Match B
	nop
	reti			;	OC3Caddr	= 0x0038	; Timer/Counter3 Compare Match C
	nop
	reti			;	OVF3addr	= 0x003a	; Timer/Counter3 Overflow
	nop
	reti			;	URXC1addr	= 0x003c	; USART1, Rx Complete
	nop
	reti			;	UDRE1addr	= 0x003e	; USART1, Data Register Empty
	nop
	reti			;	UTXC1addr	= 0x0040	; USART1, Tx Complete
;	nop
;	reti
	jmp TWI_ISR		;	TWIaddr		= 0x0042	; 2-wire Serial Interface
	nop
	reti			;	SPMRaddr	= 0x0044	; Store Program Memory Read


; string constants
;.CSEG
;.org FLASH_DATA

str_I2C_MON: .DB	"I2C Monitor Version 01 for Freaks", ASCII_CR, ASCII_LF, ASCII_LF
			.DB		"Write Byte to Device:                  w <dev addr> <data>", ASCII_CR, ASCII_LF
			.DB		"Write Byte to Device w/ Register Adr:  W <dev addr> <reg addr> <data> ", ASCII_CR, ASCII_LF
			.DB		"Read Byte from Device:                 r <dev addr> ", ASCII_CR, ASCII_LF
			.DB		"Read Byte from Device w/ Register Adr: R <dev addr> <reg addr>", ASCII_CR, ASCII_LF
			.DB		"Print GIR and CTRL Registers:  s", ASCII_CR, ASCII_LF
			.db		"Set Control Registers:	        S <msb> <lsb>", ASCII_CR, ASCII_LF
			.db		"Print AD:                      p ", ASCII_CR, ASCII_LF, 0

str_No_Dev:	.DB		"No I2C Device Response ", ASCII_CR, ASCII_LF, 0

Clk_cal_bytes:	.DB	0,0,0,0xAF		;the place to put clock calibration data in FLASH.  This is a part programmer issue.


RESET: 					;Reset Label

InitSys:
			;Set the stack point to the top of internal RAM,
			;Set the Y register for DataStack use
			;
	ldi	Temp, ToS_HB	;load the stack pointer
	out SPH, Temp
	ldi	Temp, ToS_LB
	out SPL, Temp

	ldi	YH, ToDS_HB		;Use Y for a data stack
	ldi	YL, ToDS_LB

	in Temp, MCUCSR
	sts	Boot_MCSR, Temp		;store the MCSR so we can see the reset reason

;	ldi	XH, 1				;if you have slow starting peripherals, you may need to delay a bit here.
;	ldi	XL, 0
;Delay:
;	adiw	XH:XL, 1
;	cpi	XH, 0
;	brne	Delay

.equ WD_Timer_Config = 0x0B		;Watchdog enabled, timeout around 110ms
.equ WD_Change_mask	= 0x10		;the WDCE bit

; Init the watchdog timer, assuming safetly level 1
	wdr
	ldi	Temp, WD_Timer_Config | WD_Change_mask
	out	WDTCR, Temp
	cbr	Temp, WD_Change_mask
	out WDTCR, Temp
			
	ldi ASCII_Offset, ASCII_Num_Offset

; Initialize the clock calibration factor, checking for 0 to make sure that it has been programmed before loading it

	ldi	ZH, HIGH(Clk_cal_bytes<<1)
	ldi	ZL, LOW(Clk_cal_bytes<<1)		;setup the base of the table pointer
	adiw	ZH:ZL, 3					;plus the offest to the 8mhz byte
						
	lpm	Temp, Z							;get the value in Temp

	cpi	Temp, 0
	breq	Skip_cal
	cpi	Temp, 0xff
	breq	Skip_cal

	sts	OSCCAL, Temp

Skip_cal:
	clr Temp
	out RAMPZ, Temp

	rcall	Init_IO		;this inits all of the ports to startup defaults

	rcall Init232

	call I2C_Regs_Init

	rcall InitA2D

	rcall InitTimer

	call TWI_Init		;this has the side effect of enabling irq's

	ldi	ZH, HIGH(str_I2C_MON<<1)
	ldi	ZL, LOW(str_I2C_MON<<1)
	rcall	USART_PStr

	rcall Mon_Prompt	;output the monitor prompt

; Here is the main loop, so any application that needs to be done goes here.

Loop:

	wdr
	lds	Temp, Test_Chk_Flg		;see if it's time to check something.  This flag is set in the timer
	sbrs	Temp, 0				; isr, and is just here as an example of a periodic monitoring with minimal	
	rjmp	Lp_0				; irq latency.  Obviously there can be lots of these depending on the app.

	rcall	Test_Check
	clr	Temp
	sts	Test_Chk_Flg, Temp


Lp_0:
	lds	Temp, CReg_Status			;The two control registers need to be handled whenever they are written
	sbrc	Temp, CTRL0_Changed_bit	; over the I2C bus, so they are checked and dealt with here.  The ISR sets
	call	CTRL0_Change			; the flags and saves the previous values.

	lds	Temp, CReg_Status
	sbrc	Temp, CTRL1_Changed_bit
	call	CTRL1_Change

	call SCP_Check			;do these things every time through the loop, nothing better to do...
	call Alarm_Check

	sbis UCSR0A, RXC0		; check for a serial port character received
	rjmp Loop

; This is the command interpreter for the serial port monitor...

	rcall Get_Cmd	;get a monitor command

	cpi Temp, cmd_i2c_write_adr		; write a byte to a dev, reg addr over i2c
	brne Nxt1

	rcall USART_Space
	rcall Get_Hex_Byte
	mov	I2C_DevAddr, Temp 			;get device addr,
	rcall USART_Space

	rcall Get_Hex_Byte
	mov	 I2C_Addr,Temp				;and register addr
	rcall USART_Space

	rcall Get_Hex_Byte
	mov	I2C_Data, Temp 				;and data

	rcall Wr_I2C_Adr_Data			;do it

	rcall	Mon_Prompt

	rjmp	Loop


Nxt1:
	cpi Temp, cmd_i2c_write_byte		; write a byte to a dev addr over i2c
	brne Nxt2

	rcall USART_Space
	rcall Get_Hex_Byte
	mov	I2C_DevAddr, Temp 			;get device addr,
	rcall USART_Space

	rcall Get_Hex_Byte
	mov	I2C_Data, Temp 				;and data

	rcall Wr_I2C_Data			;do it

	rcall	Mon_Prompt

	rjmp	Loop

Nxt2:
	cpi Temp, cmd_i2c_read_adr		; read a byte from i2c with dev and reg addr
	brne Nxt3

	rcall USART_Space
	rcall Get_Hex_Byte
	mov	I2C_DevAddr, Temp			;get device addr,
	rcall USART_Space

	rcall Get_Hex_Byte
	mov	I2C_Addr, Temp				;and register addr
	rcall USART_Space

	rcall Rd_I2C_Adr_Byte			;do it
	cp	Temp, Temp					;this checks for 0 in Temp (OK)
	brbc	1, Nxt2_1				;if there was an error, skip the printing

	rcall USART_Space
	ldi	Temp, ':'
	rcall USART_Transmit
	rcall USART_Space

	mov	Temp, I2C_Data
	rcall	USART_Hex_Tx
	rcall USART_Space
	mov Temp, I2C_Data
	rcall	USART_Bin_Tx
Nxt2_1:
	rcall	Mon_Prompt

	rjmp	Loop

Nxt3:
	cpi Temp, cmd_i2c_read_byte		; read a byte from i2c device, no reg adr
	brne Nxt4

	rcall USART_Space
	rcall Get_Hex_Byte
	mov	I2C_DevAddr, Temp			;get device addr,

	rcall Rd_I2C_Byte				;do it
	cp	Temp, Temp					;this checks for 0 in Temp (OK)
	brbc	1, Nxt3_1				;if there was an error, skip the printing

	rcall USART_Space
	ldi	Temp, ':'
	rcall USART_Transmit
	rcall USART_Space

	mov	Temp, I2C_Data
	rcall	USART_Hex_Tx
	rcall USART_Space
	mov Temp, I2C_Data
	rcall	USART_Bin_Tx
Nxt3_1:
	rcall	Mon_Prompt

	rjmp	Loop

Nxt4:
	cpi	Temp, cmd_prt_GIR
	brne	Nxt9

	rcall USART_CRLF
	rcall USART_Space
	ldi	Temp, 'G'
	rcall 	USART_Transmit
	rcall USART_Space
	lds	Temp, I2C_GIR1
	rcall	USART_Hex_Tx
	rcall USART_Space
	lds	Temp, I2C_GIR0
	rcall	USART_Hex_Tx
	rcall USART_CRLF

	rcall USART_Space
	ldi	Temp, 'C'
	rcall	USART_Transmit
	rcall USART_Space
	lds	Temp, I2C_CTRL1
	rcall	USART_Hex_Tx
	rcall USART_Space
	lds	Temp, I2C_CTRL0
	rcall	USART_Hex_Tx
	rcall USART_CRLF

	rcall USART_Space
	ldi	Temp, 'M'
	rcall	USART_Transmit
	rcall USART_Space
	lds	Temp, Boot_MCSR
	rcall	USART_Hex_Tx
	rcall USART_CRLF

	rcall	Mon_Prompt
	rjmp	Loop

Nxt9:
	cpi	Temp, cmd_set_ctrl
	brne	Nxt10

	rcall USART_Space
	rcall Get_Hex_Byte
	sts	I2C_CTRL1, Temp			;get device addr,
	rcall USART_Space

	rcall Get_Hex_Byte
	sts	I2C_CTRL0, Temp				;and register addr

	lds	Temp, CReg_Status
	sbr	Temp, CTRL0_Changed_mask | CTRL1_Changed_mask
	sts	CReg_Status, Temp

	rcall	Mon_Prompt
	rjmp	Loop

Nxt10:
	cpi	Temp, cmd_prt_AD
	brne	Nxt11

	rcall USART_Space
	lds	Temp, I2C_AD0
	rcall	USART_Dec_Tx
	rcall USART_Space
	lds	Temp, I2C_AD1
	rcall	USART_Dec_Tx
	rcall USART_Space
	lds	Temp, I2C_AD2
	rcall	USART_Dec_Tx
	rcall USART_Space
	lds	Temp, I2C_AD3
	rcall	USART_Dec_Tx
	rcall USART_CRLF

	rcall	Mon_Prompt
	rjmp	Loop

Nxt11:		;unknown command

	rcall USART_Space
	ldi	Temp, '?'
	rcall USART_Transmit

	rcall Mon_Prompt
	
	rjmp Loop


	
Get_Hex_Byte:	;two hex digits from serial port, return value in Temp

	rcall USART_Receive
	rcall USART_Transmit
	mov	T2, Temp
	sub	T2, ASCII_Offset
	mov T3, T2
	cpi	T2, 10	;if it's greater than 10, deal with upper and lower
	brlo	GHB1

	mov	T2, Temp
	andi	T2, 0xDF	;make it uppper
	cpi	T2, 'G'
	brlo	GHB2
				
	rcall USART_Space		;we have an invalid hex, so just say so and reset
	ldi	Temp, '?'
	rcall USART_Transmit
	rjmp	RESET

GHB2:
	andi	T2, 0x07
	ldi		T3, 9
	add		T3, T2
GHB1:
	lsl	T3
	lsl	T3
	lsl	T3
	lsl T3

GHB3:			;get the second nibble
	rcall USART_Receive
	rcall USART_Transmit
	mov	T2, Temp
	sub	T2, ASCII_Offset
	cpi	T2, 10	;if it's greater than 10, deal with upper and lower
	brlo	GHB4

	mov	T2, Temp
	andi	T2, 0xDF	;make it upper
	cpi	T2, 'G'
	brlo	GHB5
				
	rcall USART_Space		;we have an invalid hex, so just say so and just reset
	ldi	Temp, '?'
	rcall USART_Transmit
	rjmp	RESET

GHB5:
	andi	T2, 0x07
	ldi		Temp, 9
	add		T2, Temp
GHB4:
	or	T3, T2
GHB6:
	mov Temp, T3
	com Temp
	out PORTB, Temp

	com Temp
	ret

Get_Cmd:	;for now, one character from serial port
	rcall	USART_Receive
	st	-Y, Temp
	rcall	USART_Transmit
	ld	Temp, Y+
	ret	

.include "pinout.asm"
.include "a2d.asm"
.include "usart.asm"
.include "I2C-polled.asm"
.include "timers.asm"
.include "SCP-things.asm"
.include "i2c-regs.asm"
.include "I2C-ms-irq.asm"
.include "alarms.asm"
.include "ctrl-regs.asm"


