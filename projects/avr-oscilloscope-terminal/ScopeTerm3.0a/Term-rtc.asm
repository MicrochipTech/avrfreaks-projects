; Term-rtc.asm	-- Included source for Dutchtronix Oscilloscope Terminal
;
;  Copyright © 2008 Jan P.M. de Rie
;
;  All Rights Reserved
;
;  This file is part of the Dutchtronix Oscilloscope Clock Distribution.
;  Use, modification, and re-distribution is permitted subject to the
;  terms in the file named "LICENSE.TXT", which contains the full text
;  of the legal notices and should always accompany this Distribution.
;
;  This software is provided "AS IS" with NO WARRANTY OF ANY KIND.
;
;  This notice (including the copyright and warranty disclaimer)
;  must be included in all copies or derivations of this software.
;
; Terminal program for Dutchtronix AVR Oscilloscope Clock hardware
;
.EQU	RTCID	=	0b10100010

#define DODELAY	rcall I2CDELAY
;
; RTC Routines
;
InitRTCInt:
	push	r16
	push	r17
	rcall	StopRTCClockOut
	rcall	StartRTCInt
	pop		r17
	pop		r16
	ret

StartRTCInt:
	ldi		r17,RTCCNT			;set countdown value
	ldi		r16,64
	rcall	I2CRndWrt
	ldi		r17,RTCTCR			;update Timer/Control Register
	ldi		r16,0b10000001		;enable TE, set 64 Hz source Clock
	rcall	I2CRndWrt
	ldi		r17,RTCCS2			;update Control/Status 2
	ldi		r16,0b00010001		;clear TF, set TI/TP set TIE
	rcall	I2CRndWrt
	ret

ResetRTCInt:
;
; reload Countdown, clear TF flag in RTC
; Apparently it is not necessary to reload these timer values
; after a timer interrupt.
;
	ldi		r17,RTCCNT
	ldi		r16,64
	rcall	I2CRndWrt
	ldi		r17,RTCCS2			;update Control/Status 2
	ldi		r16,0b00010001		;clear TF, set TIE
	rcall	I2CRndWrt
	ret

;
; Note ClockOut is Open Drain Signal
;
StartRTCClockOut:
	ldi		r17,RTCCLK
	ldi		r16,0b10000000		;enable ClockOut at 32.768 KHz
	rcall	I2CRndWrt
	ret

StopRTCClockOut:
	ldi		r17,RTCCLK
	clr		r16					;disable ClockOut (power saver)
	rcall	I2CRndWrt
	ret

I2CDELAY:
	nop
	nop
	nop
	nop
	nop
	nop
#if Mega168
	nop
	nop
#endif
	ret
;
; Configure these RTC pins as input (default), possibly with pull-ups,
; When accessing the RTC, change the direction as needed, leaving
; them as input again on exit.
; This way, we can write to these bits when accessing the DAC without
; affecting the RTC in any way
;
I2CInit:
	sbi     PORT_RTC,BIT_SDA
	sbi     PORT_RTC,BIT_SCL
	ret
;
; RANDOM READ:	Address in r17	Data returned in r16 and r24
;	No other registers used
;
I2CRndRd:
	push	r19
	clr		r19
I2CRRdL100:
	dec		r19			;max 256 tries
	brne	I2CRRdL150	;still good
	rjmp	I2CRRdL200
I2CRRdL150:
	breq	I2CRRdL200
	rcall	I2CStart
	DODELAY
	ldi		r16,RTCID|0	;address|write command to set RTC Register Pointer
	rcall	I2CTX
	DODELAY
	rcall	I2CAck		;wait acknowledge
	brcs	I2CRRdL100
	mov		r16,r17		;address high
	rcall	I2CTX
	DODELAY
	rcall	I2CAck		;wait acknowledge
	brcs	I2CRRdL100
	rcall	I2CStop
	rcall	I2CStart
	ldi		r16,RTCID|1	;address|read command
	rcall	I2CTX
	DODELAY
	rcall	I2CAck
	brcs	I2CRRdL100
	rcall	I2CRX
	sbi		PORT_RTC,BIT_SDA	;NOT acknowledge
	sbi		PORT_RTC,BIT_SCL
I2CRRdL200:
	rcall	I2CStop
	mov		r24,r16
	pop		r19
	ret
;
;  RANDOM WRITE: Address in r17	Data to write in r16
;	No other registers used
;
I2CRndWrt:
	push	r19
	clr		r19
I2CRWrL100:
	dec		r19			;max 256 tries
	brne	I2CRWrL150	;still good
	rjmp	I2CRWrL200
I2CRWrL150:
	rcall	I2CStart
	DODELAY
	push	r16
	ldi		r16,RTCID|0	;address|write command to set RTC Register Pointer
	rcall	I2CTX
	DODELAY
	rcall	I2CAck		;wait acknowledge
	pop		r16
	brcs	I2CRWrL100
	push	r16
	mov		r16,r17		;address high
	rcall	I2CTX
	rcall	I2CAck		;wait acknowledge
	pop		r16
	brcs	I2CRWrL100
	push	r16
	rcall	I2CTX
	rcall	I2CAck		;wait acknowledge
	pop		r16			;Data to write
	brcs	I2CRWrL100
	nop
I2CRWrL200:
	rcall	I2CStop
	pop		r19
	ret
;
; CHECK BIT I2C_ACKNOWLEDGE BIT
;	CARRY = 1: FAIL 
;	CARRY = 0: PASS 
;
I2CAck:
	cbi		DDR_RTC,BIT_SDA    ;BIT_SDA input port
	sbi		PORT_RTC,BIT_SDA
	sec
	sbi		PORT_RTC,BIT_SCL
	DODELAY
	sbis	PIN_RTC,BIT_SDA
	clc
	cbi		PORT_RTC,BIT_SCL
	sbi		DDR_RTC,BIT_SDA   ;BIT_SDA output port
	ret
;
;   Transmit 1 byte to RTC
;
I2CTX:
	push	r19
	ldi		r19,8
I2CTX1:
	sbi		PORT_RTC,BIT_SDA
	lsl		r16
	brcs	I2CTX2
	cbi		PORT_RTC,BIT_SDA
I2CTX2:
	sbi		PORT_RTC,BIT_SCL
	DODELAY
	cbi		PORT_RTC,BIT_SCL
	dec		r19
	brne	I2CTX1
	pop		r19
	ret
;
;   Receive 1 byte from RTC 
;
I2CRX:
	push	r19
	cbi		DDR_RTC,BIT_SDA
	sbi		PORT_RTC,BIT_SDA
	clr		r16
	ldi		r19,8
I2CRX1:
	sbi		PORT_RTC,BIT_SCL
	sec
	DODELAY
	sbis	PIN_RTC,BIT_SDA
	clc
	rol		r16
	cbi		PORT_RTC,BIT_SCL
	DODELAY
	dec		r19
	brne	I2CRX1
	sbi		DDR_RTC,BIT_SDA	
	pop		r19
	ret

I2CStop:
	cbi		PORT_RTC,BIT_SDA
	sbi		PORT_RTC,BIT_SCL	;stop condition
	nop
#if Mega168
	nop							;critical
#endif
	sbi		PORT_RTC,BIT_SDA
;
; Configure the I2C bits as inputs
;
	nop
	cbi     DDR_RTC,BIT_SDA
	cbi     DDR_RTC,BIT_SCL 
	ret

I2CStart:
;
; Configure the I2C bits for output
;
	sbi     DDR_RTC,BIT_SDA
	sbi     DDR_RTC,BIT_SCL 

	sbi		PORT_RTC,BIT_SDA
	sbi		PORT_RTC,BIT_SCL	;start condition
	nop
#if Mega168
	nop
#endif
	cbi		PORT_RTC,BIT_SDA
	nop
#if Mega168
	nop
#endif
	cbi		PORT_RTC,BIT_SCL	;critical
#if Mega168
	nop
#endif
	ret
