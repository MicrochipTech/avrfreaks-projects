;***************************************************************************
;*
;* DEMO PROGRAM FOR A REAL TIME CLOCK (see: function "RTC_Update")
;*
;* RTC-function is called once per second (typically via OVFL interrupt
;* from the async. timer, basically any other timer's OVFL or COMP int.
;* could be used as well as long as it is calibrated to give exactly
;* 1 second intervalls)
;*
;* Language: AVR assembler
;* Uses: 4 high registers, Z-pointer registers
;* Codesize: 79/93 words (without/with full leap year & century support)
;* Returns date/time etc. in SRAM $60 - $67
;*
;* Developed and tested on Atmel ATmega323
;*
;* (c) Andreas Lenze 2002 - 2004
;* (May be used under GPL)
;*
;* EDV-Systeme Lenze
;* Munich / Germany
;*
;* Contact: andreas.lenze@t-online.de
;* (comments welcome - especially if somebody manages to shave off another
;* couple of words from the function's code ...)
;*
;* **** --------------------------------------------------------------- ****
;*
;* Added 19-07-02: 2 functions to optionally be used with "RTC_Update"
;* during/after setting the clock manually or via GPS data:
;*	- "Perp_Cal" calculates the day-of-week (perpetual calendar feature)
;*	- "Max_Date" returns the 'days in month x' for a given year
;* Both function work over the full Gregorian calendar range (1583 -> ~4000)
;*
;* Added 03-01-03:
;*	- Function "CB_Week_Num" calculates the week number (ISO 8601, European convention))
;*
;*	- "Perp_Cal" speeded up to ~ double speed (280 cycles) while still
;*	  maintaining full "Gregorian range"
;*
;* Fixed 28-12-04: "CB_Week_Num" algorithm error
;*
;***************************************************************************


;***** This stuff just sets up the environment for the demo program
;***** to allow the RTC time keeping function to be run

; Note: AVR Studio's simulator (ver. 3.56) doesn't support timers in async mode.
; If you want to run the function in AVRStudio, just create an endless loop
; calling the function over and over again. Use the "memory watch" window
; in Studio or the "store default time/date data" in this program setup
; to "seed" start values.

.include "m323def.inc"

.cseg
.org	0

	jmp	RESET		; Reset Handler
	jmp	EXT_INT0	; IRQ0 Handler
	jmp	EXT_INT1	; IRQ1 Handler
	jmp	EXT_INT2	; IRQ2 Handler
	jmp	TIM2_COMP	; Timer2 Compare Handler
	jmp	TIM2_OVF	; Timer2 Overflow Handler
	jmp	TIM1_CAPT	; Timer1 Capture Handler
	jmp	TIM1_COMPA	; Timer1 CompareA Handler
	jmp	TIM1_COMPB	; Timer1 CompareB Handler
	jmp	TIM1_OVF	; Timer1 Overflow Handler
	jmp	TIM0_COMP	; Timer0 Compare Handler
	jmp	TIM0_OVF	; Timer0 Overflow Handler
	jmp	SPI_STC;	; SPI Transfer Complete Handler
	jmp	USART_RXC	; USART RX Complete Handler
	jmp	USART_UDRE	; UDR Empty Handler
	jmp	USART_TXC	; USART TX Complete Handler
	jmp	ADC		; ADC Conversion Complete Interrupt Handler
	jmp	EE_RDY		; EEPROM Ready Handler
	jmp	ANA_COMP	; Analog Comparator Handler
	jmp	TWSI		; 2-wire Serial Interface Interrupt Handler
	jmp	SPMR		; Store Program Memory Read Handler


;**** placeholders for currently unused interrupt vectors ****

EXT_INT0:	reti	; IRQ0 Handler
EXT_INT1:	reti	; IRQ1 Handler
EXT_INT2:	reti	; IRQ2 Handler
TIM2_COMP:	reti	; Timer2 Compare Handler
;TIM2_OVF:	reti	; Timer2 Overflow Handler
TIM1_CAPT:	reti	; Timer1 Capture Handler
TIM1_COMPA:	reti	; Timer1 CompareA Handler
TIM1_COMPB:	reti	; Timer1 CompareB Handler
TIM1_OVF:	reti	; Timer1 Overflow Handler
TIM0_COMP:	reti	; Timer0 Compare Handler
TIM0_OVF:	reti	; Timer0 Overflow Handler
SPI_STC:	reti	; SPI Transfer Complete Handler
USART_RXC:	reti	; USART RX Complete Handler
USART_UDRE:	reti	; UDR Empty Handler
USART_TXC:	reti	; USART TX Complete Handler
ADC:		reti	; ADC Conversion Complete Interrupt Handler
EE_RDY:		reti	; EEPROM Ready Handler
ANA_COMP:	reti	; Analog Comparator Handler
TWSI:		reti	; 2-wire Serial Interface Interrupt Handler
SPMR:		reti	; Store Program Memory Read Handler


RESET:	; system initialization

	; Set stack pointer to top of RAM
	ldi	Tmp1,high(RAMEND)
	out	SPH,Tmp1
	ldi	Tmp1,low(RAMEND)
	out	SPL,Tmp1


	; store default time/date data (01-01-2002, 00:00:00, TUE)
	; "to-be-zeroed" buffers should already be cleared by
	; your Init_SRAM routine
	ldi	Tmp1,20
	sts	cent_buf,Tmp1	; century (20nn)
	ldi	Tmp1,1
	sts	date_buf,Tmp1	; January 1st
	sts	month_buf,Tmp1
	inc	Tmp1
	sts	day_buf,Tmp1	; Tuesday
	sts	year_buf,Tmp1	; nn02

;**** Set up timer2 as RTC (clocked from 32.768 KHz crystal in async. mode)
	; (ATmega323, all precautions taken to *not* confuse anything)
	; disable JTAG
	in	Tmp1,MCUCSR
	sbr	Tmp1,0b10000000	; bit 6 "0" = int2 on falling edge
	out	MCUCSR,Tmp1	; bit 7 set = disable JTAG
	out	MCUCSR,Tmp1	; (to be written twice inside four cycles)

	; PORTC
	ldi	Tmp1,0b00011000	; PC3 output (test!), PC4 testpin OVFL int (1/s)
	out	DDRC,Tmp1
	ldi	Tmp1,0b00000000
	out	PORTC,Tmp1 	; PC2 input tri-state (RTS handshake)

;	call	Delay_1s	; wait for "el cheapo" crystal to stabilize
	; use your own most favorite delay routine for this -
	; delay *is* essential after a power-on (learned it the hard way ...)

        cli                     ; no interruptions from now on
	in	Tmp1,TIMSK	; fetch current timers int. configuration
	cbr	Tmp1,0b11000000	; clear bits 7&6 (OCIE2 & TOIE2) to disable
				; the timer2 interrupts
	out	TIMSK,Tmp1	; write back configuration
	ldi	Tmp1,0x08	; set timer2 to async mode
	out	ASSR,Tmp1
	clr	Tmp1
	out	TCNT2,Tmp1	; reset t2 counter
	out	OCR2,Tmp1	; reset t2 to-be-compared value

	ldi	Tmp1,5		; 32.768 KHz / 128 = 1 sec intervals
	out	TCCR2,Tmp1	; set prescaler bits to "5" (gives 1 sec intervals
				; for the overflow to occur)
	rcall	T2_Busy_Loop	; make sure timer2 hardware is ready

	in	Tmp1,TIFR	; fetch all pending timer interrupts configuration
	sbr	Tmp1,0b11000000	; set bits 7&6 (OCF2 & TOV2) to clear any
				; pending timer2 interrupts
	out	TIFR,Tmp1	; write back (= clear timer2) interrupt pending
				; flags without disturbing other timer's int. flags

	in	Tmp1,TIMSK	; fetch current timer interrupts configuration
	sbr	Tmp1,0b01000000	; set bit 6 (OCIE2) to enable
				; the timer2 overflow interrupt
	out	TIMSK,Tmp1	; write back configuration
	sei			; enable interrupts globally


T2_Loop:
	in	Tmp1,MCUCR	; activate sleep "power save" mode
	sbr	Tmp1,0b10110000
	out	MCUCR,Tmp1
	sleep			; hibernate
	nop			; main crystal has just restarted, act cautiously
	ldi	Tmp1,5		; 32.768 KHz / 128 = 1 sec
				; in case we directly go to sleep again, we need at
				; least one TOSC cycle delay after "wake up" -
	out	TCCR2,Tmp1	; writing something in the timer2 "area" (e.g. "set
	call	T2_Busy_Loop	; prescaler") & waiting for completion takes care of that

	rjmp	T2_Loop		; endless loop


T2_Busy_Loop:			; wait for timer2 write access completion
	ldi	Tmp2,0b00001000	; bitmask for bit tests (ASSR bit 3 is set
				; for "asynchronuous" mode)
	in	Tmp1,ASSR	; fetch status
	eor	Tmp1,Tmp2	; wait for all timer2 busy flags (bits 0-2)
	tst	Tmp1		; to become cleared
	brne	T2_Busy_Loop
	ret


;**************************************************************************
;*
;* Timer2 Overflow interrupt routine
;*
;**************************************************************************

TIM2_OVF:
	in      r1,SREG  	; flags may be changed by arithmetics,
        push    r1       	; so save the status register
	call	RTC_Update	; do the book-keeping

; further code plugged in here - create flags to output time/date info in
; main, blow up the house, whatever ...


; ** Testcode - toggles LED on PORTB.0 in 2 seconds intervalls
               ldi r17, $01 ; Register init
               ldi r16, $01 ; Program PORTB.0 as an output
               out DDRB, r16
	       in r16, PORTB
               eor r16, r17
               out PORTB, r16
; ** End Testcode

Tim2_Exit:
	pop     r1
	out     SREG,r1		; restore the status register
	reti

;**** End of Timer2 Overflow interrupt routine ------------------------ ****


;***************************************************************************
;*
;* This is the real (stand-alone) module (plug-in for your program)
;*
;***************************************************************************


;***************************************************************************
;*
;* Function "RTC_Update"
;*
;* Maintains RTC time-keeping:
;*	seconds (0-59)
;*	minutes (0-59
;*	hours (0-23, "military time")
;*	day-of-week (monday=1 ... sunday=7, ISO 8061 compliant)
;*	date (1-31)
;*	month (1-12, JAN=1)
;*	year (00-99)
;*	century
;*	- static "20" like in "20nn" w/o additional code
;*	- "00" incremented through "255" with additional code (14 words)
;*
;*	leap years are taken into account (month/date data processing)
;*	- correctly between years 2001 through 2099 (w/o additional code)
;*	- correctly from "1600" up to "25599" (with additional code, see below)
;*
;* Function is called once per second (typically via OVFL interrupt
;* from the async. timer)
;*
;* Uses: 4 high registers, Z-pointer registers
;* Code size: 79/93 words (IAR "C" compiler needs 202 words for this w/o
;*            "day-of-the-week" book-keeping functionality), module
;*	      optimized for code size
;*
;* Cycles: minimum 10 (w/o call/return), maximum 77/88 (worst case,
;*	   leap year Feb -> March & "new week" = true; again w/o call/return)
;*
;* Returns date/time etc. in array SRAM $60 - $67
;*
;* developed and tested on ATmega323
;*
;***************************************************************************

;**** SRAM usage ****

; array (data buffers) for date/time information
; the array's structure looks funny - it *does* make sense in the program from
; which this has been extracted

.equ	sec_buf		=	$60	; 0 - 59
.equ	cent_buf	=	$61	; century data (the "20" from "year 20xx")
.equ	min_buf		=	$62	; 0 - 59
.equ	hour_buf	=	$63	; 0 - 23
.equ	date_buf	=	$64	; 1 - 31
.equ	day_buf		=	$65	; 1 - 7 ("1" = monday)
.equ	month_buf	=	$66	; 1 - 12 ("1" = january)
.equ	year_buf	=	$67	; 0 - 99


.def	Tmp1		= R22   ; 1st scratchpad
.def	Tmp2		= R23   ; 2nd    "
.def    xortmp1		= R24   ; 3rd    "	  (use any "convenient"
.def    xortmp2		= R25   ; 4th    "	  high regs available)


RTC_Update: ; Main ("stand-alone") function for time keeping
	clr	ZH
	ldi	ZL,sec_buf	; set pointer to "seconds" buffer

	; Note: this SRAM-address is regarded as the "offset 0" of the array.
	; We use "store/load with displacement" throughout the module to
	; access the individual data buffers. If you use a different array
	; structure, you may want to adapt the "offsets" (e.g. "offset 2" as
	; in 'ldd Tmp2,Z+2' currently points to the "minutes" buffer).
	; Note: Indirect memory access with displacement works over a range of
	; +64 addresses

	ld	Tmp1,Z		; fetch "old" seconds value
	inc	Tmp1		; increment seconds
	cpi	Tmp1,60		; one minute complete?
	breq	RTC_Umin	; yes, start working
	st	Z,Tmp1		; no, just store new seconds data
	ret			; to buffer

RTC_Umin:	; one minute has passed, update the RTC data
	clr	Tmp1		; reset seconds data
	st	Z,Tmp1		; store reset seconds data to buffer
	ldd	Tmp2,Z+2	; get minutes data from buffer
	inc	Tmp2		; increment minutes value
	cpi	Tmp2,60		; hour passed?
	breq	RTC_Uh		; yes, branch
	std	Z+2,Tmp2	; no, store new minutes data
	ret

RTC_Uh:	; one hour has passed
	clr	Tmp2
	std	Z+2,Tmp2	; minutes are reset & stored
	ldd	Tmp2,Z+3	; get hour data from buffer
	inc	Tmp2		; increment hour value
	cpi	Tmp2,24		; day passed?
	breq	RTC_Ud		; yes, branch
	std	Z+3,Tmp2	; no, store new hour data
	ret

RTC_Ud:	; day over - week, too?
	clr	Tmp2
	std	Z+3,Tmp2	; hours are reset & stored
	ldd	Tmp2,Z+5	; get day data from buffer
	inc	Tmp2		; increment day value
	sbrc	Tmp2,3		; week passed? (true if bit 3 set -> day = 8)
	ldi	Tmp2,1		; yes, reset day to "Monday" (=1)
	std	Z+5,Tmp2	; store next day data

	; a day has passed - process and analyze date information
	ldd	Tmp1,Z+6	; get month data from buffer
	ldd	Tmp2,Z+4	; get date data from buffer
	inc	Tmp2		; increment date value (it's a brand new day)

	; evaluate new date value - we're either still in the current month, or this
	; one is over and we have to start time-keeping in the next one
	cpi	Tmp2,32		; proposed new date beyond upper limit of max. date range?
	breq	RTC_Unext_M	; yes, a "long" month definitively has passed (-> next month)
	cpi	Tmp2,31		; if new date would be "31st",
	breq	RTC_Ushort	; go check if we are in a "short" month
RTC_U1:				; return from "check if short month" = false
	cpi	Tmp2,30		; are we in a "leap year February"?
	breq	RTC_Ufeb_l	; go check
RTC_U2:				; return from "check if leap year February" = false
	cpi	Tmp2,29		; would new date be a *Feb.* 29 and "no leap year"?
	breq	RTC_Ufeb_nl	; go check, else we are ...

RTC_Usame_M:	; still in current month
	std	Z+4,Tmp2	; store new date data
	ret

RTC_Unext_M:	; current month has passed
	ldi	Tmp2,1
	std	Z+4,Tmp2	; date is reset and stored
	inc	Tmp1		; Tmp1 still contains month data
	cpi	Tmp1,13		; year passed?
	breq	RTC_Uy		; yes, update year & month data
	std	Z+6,Tmp1	; no, just store next month data
	ret

RTC_Uy:	; a year has passed
	ldd	Tmp1,Z+7	; get year data from buffer
	inc	Tmp1

	; ----- remove next block (1st of 4) to save code space --------------
	; an "abbreviated leap year test" & "no century update" is still
	; sufficient (valid) for the years from 2001 to 2099
	cpi	Tmp1,100	; century passed?
	breq	RTC_Uc
	; --------------------------------------------------------------------

RTC_Uy1:
	std	Z+7,Tmp1	; store new year data
	ldi	Tmp1,1		; reset month to "January"
	std	Z+6,Tmp1	; ... and store month data
	ret

	; ----- remove next block (2nd of 4) to save code space --------------
RTC_Uc:	; a century has passed
	ldd	Tmp1,Z+1	; get century data from buffer
	inc	Tmp1
	std	Z+1,Tmp1	; store new century data
	clr	Tmp1		; reset year ("00")
	rjmp	RTC_Uy1
	; --------------------------------------------------------------------

	;*********************************************************************
	; several decision-making subroutines for "new date data evaluation" -
	; we can't "call" them due to MCU instruction set limitations,
	; so we "jump" to them and exit with a "return jump" (sorry)
RTC_Ufeb_l:
	cpi	Tmp1,2		; February?
	breq	RTC_Unext_M	; yes, a "leap year February" has passed -> new month
	rjmp	RTC_U2		; no, go back to analyzing

RTC_Ufeb_nl:
	cpi	Tmp1,2		; February?
	breq	RTC_Uleap_nl1	; yes, check for "not a leap year"
	rjmp	RTC_Usame_M	; no, it's just a "29th" - still current month

RTC_Uleap_nl1:	; February 29 is legal only if we are in a leap year
		; Note: The calendar year is 365 days long, unless the year is exactly
		; divisible by 4, in which case an extra day is added to February
		; (29th) to make the year 366 days long. If the year is the last year
		; of a century, e.g. 1800, 1900, 2000, then it is a leap year only if
		; it is exactly divisible by 400. Therefore, 1900 wasn't a leap year
		; but 2000 was.

	ldd	xortmp2,Z+7	; get year data from buffer

	; ----- remove next block (3rd of 4) to save code space --------------
	tst	xortmp2		; if year is "0", century is divisible by 100
	breq	RTC_Uleap_nl3
	; --------------------------------------------------------------------

	mov	xortmp1,xortmp2	; use some (convenient) high regs as scrap
	cbr	xortmp1,0x03	; clear bits 0 & 1:
	eor	xortmp2,xortmp1	; year is divisible by 4 if bits 0 & 1 are "0"

RTC_Uleap_nl2:
	tst	xortmp2		; if year/century is divisible by 4, we're in a leap year -
	breq	RTC_Usame_M	; true = we're still in the current month
	rjmp	RTC_Unext_M	; no leap year, -> next month

	; ----- remove next block (4th of 4) to save code space --------------
RTC_Uleap_nl3:
	ldd	xortmp2,Z+1	; get century data from buffer -
	mov	xortmp1,xortmp2	; we are in the last year of a century
	cbr	xortmp1,0x03	; clear bits 0 & 1:
	eor	xortmp2,xortmp1	; century is divisible by 400 if bits 0 & 1 are "0"
	rjmp	RTC_Uleap_nl2
	; --------------------------------------------------------------------

RTC_Ushort:	; months 4,6,9,and 11 are 30-days-months
	cpi	Tmp1,4		; April?
	breq	RTC_Unext_M	; yes, -> month passed
	cpi	Tmp1,6		; June?
	breq	RTC_Unext_M	; yes, -> month passed
	cpi	Tmp1,9		; September?
	breq	RTC_Unext_M	; yes, -> month passed
	cpi	Tmp1,11		; November?
	breq	RTC_Unext_M	; yes, -> month passed
	rjmp	RTC_U1		; not a "short" month, go back to analyzing

;**** End function "RTC_Update" --------------------------------------- ****


;**************************************************************************
;*
;* Function "Perp_Cal" - perpetual calendar to calculate day-of-week from
;* date, month, year (Gregorian, since Oct. 15, 1583, correct til ~ AD 4000)
;* Day-of-week formula by Christian Zeller (1882 paper)
;*
;* Formula: a = (14 - month) / 12
;*          y = year - a
;*          m = month + 12*a - 2
;*
;*        day = (date + y + y/4 - y/100 + y/400 + 31*m/12) mod 7
;*
;* All divisions are modulo, day # 0 = Sunday, 1 = Monday ....
;*
;* Uses: X, Y, Z, r1:r0, Tmp1 (all regs destroyed)
;*
;* Code optimized for speed (255 cycles w/o call/return)
;*
;***************************************************************************

Perp_Cal:
	; determine "a"
 	lds	ZL,month_buf	; fetch month
	cpi	ZL,3
	brlo	PC+3		; if month is either 1 or 2, "a"
	clr	ZL		; is "1", else it will be "0"
	rjmp	PC+2		; a = 0
	ldi	ZL,1		; a = 1

	; determine "y"
	lds	YH,cent_buf	; load century (byte)
	ldi	YL,100		; x 100d
	mul	YH,YL		; unsigned mul, result in r1:r0
	lds	YL,year_buf	; load year (-> "tens/ones" value)
	clr	YH
	add     YL,r0		; 16-bit-addition ("hundreds" + "tens/ones")
	adc     YH,r1		; result in YH:YL
        clr     ZH              ; We have to use 16-bit-math
        sub     YL,ZL		; subtract "a" from 16-bit-year
        sbc     YH,ZH		; "y" now in YH:YL ("a" in ZL)

         ; determine "m"
        lds	ZH,month_buf
	tst	ZL
        breq	PC+3		; "a"=0, 12x0=0, -> no addition required
	ldi	ZL,12
	add	ZH,ZL		; "a"=1 -> add 12 to month (ZH)
	subi	ZH,2		; "m" now in ZH, "y" still in YH:YL, "a" now superfluous

	;* calculate value inside brackets *
	; -> 31*m/12 modulo
	ldi	ZL,31
	mul	ZL,ZH		; 31*"m", result in r1:r0
	movw    XH:XL, r1:r0	; move it to XH:XL


	; divide by 12
	push	r2
	push	r19
	push	r18
	push	r17
	push	r16
	push	YH
	push	YL


	ldi	YH,0xAA		; scaled reciprocal for /12
	ldi	YL,0xAB

	; Q = A * 0xAAAB
	; (r19:r18:r17:r16 = XH:XL * YH:YL)
        clr     r2
        mul     XH, YH		; ah * bh
        movw    r19:r18, r1:r0
        mul     XL, YL		; al * bl
        movw    r17:r16, r1:r0
        mul     XH, YL		; ah * bl
        add     r17, r0
        adc     r18, r1
        adc     r19, r2
        mul     YH, XL		; bh * al
        add     r17, r0
        adc     r18, r1
        adc     r19, r2

	; Q = Q >> 16: use r19:r18 as word
	; Q = Q >> 3
	lsr	r19		; do the last 3 shifts
	ror	r18
	lsr	r19
	ror	r18
	lsr	r19
	ror	r18
	; r19:r18 holds "Q"
	mov	ZL,r18		; In this context, 31*m/12 will be
				; byte value (only r18 of interest)
	pop	YL
	pop	YH
	pop	r16
	pop	r17
	pop	r18
	pop	r19
	pop	r2


	clr	ZH		; add "y" to result (in ZL, "31*m/12"),
        add     ZL,YL		; Z receives new intermediate number
        adc     ZH,YH

	; divide modulo "y"/4
	mov	r0,YL		; copy "y" value to r1:r0
	mov	r1,YH
	lsr	r1		; shift word right 2x
	ror	r0
	lsr	r1
	ror	r0
        add     ZL,r0		; 16-bit-addition
        adc     ZH,r1		; add modulo "y/4" to intermediate number

	; divide modulo "y"/400
	push	YL		; save "y" value
	push	YH
	ldi	YL,0x64		; 100d
	clr	YH
	ser	Tmp1

	inc	Tmp1
	sub	r0,YL		; r1:r0 already contain mod "y"/4
	sbc	r1,YH
	brmi	PC+2
	rjmp	PC-4

	mov	YL,Tmp1
	add	ZL,YL
	adc	ZH,YH

	; calculate modulo "y"/100 and subtract it from intermediate number
	pop	YH		; fetch original "y" value from stack
	pop	YL

	ldi	XL,low(1000)
	ldi	XH,high(1000)
	clr	Tmp1

	mov	r0,YL		; save value prior to subtraction to r1:r0
	mov	r1,YH
	sub     YL,XL		; start with subtracting "Thousands"
	sbc     YH,XH
	brmi    PC+3
	subi	Tmp1,-10
	rjmp    PC-6

	ldi	XL,low(100)	; subtract "Hundreds" from remainder
	ldi	XH,high(100)

	sub	r0,XL
	sbc	r1,XH
	brmi    PC+3
	inc	Tmp1
	rjmp    PC-4

	clr	r1
	sub	ZL,Tmp1		; 16-bit-subtraction (!)
	sbc	ZH,r1		; (- "y"/100)

	; add date
	clr	YH
	lds	YL,date_buf	; fetch date & add it to intermediate number
        add     ZL,YL		; 16-bit-addition
        adc     ZH,YH		; "bracket value" is now complete in Z

	; find day-of-week, day = (bracket_value) mod 7
	mov	XL,ZL		; load "bracket value" from Z
	mov	XH,ZH
	call	Div16_7		; divide it by 7 (remainder in XL = day number)
	tst	XL		; make sure to get the correct day # for RTC module:
	brne	PC+2		; Sunday here = 0, for RTC = 7
	ldi	XL,7		; (Mon to Sat = 1-6)

	sts	day_buf,XL	; store it to RTC buffer
	ret

;**** End of Function Perp_Cal ---------------------------------------- ****


;***************************************************************************
;*
;* Function "CB_Max_Date" - returns number of days/month in Tmp2
;*
;***************************************************************************

CB_Max_Date:
	lds	Tmp2,month_buf
	; month 2 has either 28 or 29 days
	cpi	Tmp2,2		; February?
	breq	CB_L_Y		; yes, -> test for leap year
	; months 4,6,9,and 11 are 30-days-months
	cpi	Tmp2,4		; April?
	breq	CB_M_S		; yes, -> short month
	cpi	Tmp2,6		; June?
	breq	CB_M_S		; yes, -> short month
	cpi	Tmp2,9		; September?
	breq	CB_M_S		; yes, -> short month
	cpi	Tmp2,11		; November?
	breq	CB_M_S		; yes, -> short month

CB_M_L:	ldi	Tmp2,31		; no, -> 31 days month
	ret

CB_M_S:	ldi	Tmp2,30		; 30 days month
	ret

CB_L_Y:	; month is Feb. test if it's a leap year February (29 days)
	call	Test_LY
	ldi	Tmp2,28		; no leap year, -> 28 days in February
	sbrc	B_Flags2,7	; set: leap year = true
	ldi	Tmp2,29		; leap year, -> 29 days in February
	ret

;**** End of Function CB_Max_Date ------------------------------------- ****


;***************************************************************************
;*
;* Function "CB_Week_Num" - stores week number of given year to "week_buf"
;*
;* Algorithms:
;*
;* weekday# = Jan. 1st of given year (1=Monday)
;* d_y	    = given date as contiguous day number in year
;*
;* If weekday# = 1-4 then week #1 covers dates Jan. 1 through (8-weekday#)
;*			  week #2 starts at Jan. (9-weekday#)
;*			  week #n =((d_y - (9-weekday#) / 7) + 2 (modulo division)
;*
;* If weekday# = 5-7 then week #52/53 of last year covers dates Jan. 1 through (8-weekday#)
;*			  week #1 starts at Jan. (9-weekday#)
;*			  week #n =((d_y - (9-weekday#) / 7) + 1 (modulo division)
;*
;* (Previous) year has/had) 53 weeks if Jan. 1st was/is
;*			  either weekday# = 4
;*			  or
;*			  if leap year weekday# = 3 or 4
;*
;***************************************************************************

;**** SRAM usage ****

.equ	mweek_buf	=	$70
.equ	week_buf	=	$71
.equ	d_y1st		=	$72

; temporary buffers for RTC data during "# of week in year" calculation
.equ	min_bufw	=	$7A	; 0 - 59
.equ	hour_bufw	=	$7B	; 0 - 23
.equ	date_bufw	=	$7C	; 1 - 31
.equ	day_bufw	=	$7D	; 1 - 7 ("1" = Monday)
.equ	month_bufw	=	$7E	; 1 - 12 ("1" = January)
.equ	year_bufw	=	$7F	; 0 - 99
.equ	cent_bufw	=	$80	; 16-39

.def	B_Flags2	= 	R21	; General purpose (bit-)flags (2)
                                	; bit 7 = set: leap year (week# calculation), 0x80


CB_Week_Num:
	rcall	CB_W_Inf	; get weekday# (Jan. 1st); check for leap year true/false
	rcall	CB_Max_W	; get max. # of weeks for year (returned in Tmp1)
	sts	mweek_buf,Tmp1	; store max. week number to scrap

	; convert date into day number
	lds	xortmp1,date_buf	; fetch date into xortmp1 (r24)
	clr	xortmp2		; clear high byte of "day # in year" word (r25)
	lds	Tmp2,month_buf	; fetch month number
	cpi	Tmp2,3		; if month >= 3 AND leap year = true, add 1 day
	brlo	PC+3

	sbrc	B_Flags2,7	; set: leap year = true
	inc	xortmp1		; bit 7 set = leap year -> add "1"

	ldi	ZH,high(2*CB_Table)	; day-of-year bases table
	ldi	ZL,low(2*CB_Table)

	dec	Tmp2		; month 1 = offset 0 in table
	lsl	Tmp2		; multiply by 2 (2 bytes per entry)
	add	ZL,Tmp2		; pointer to correct data offset in table
	adc	ZH,xortmp2

	lpm	XL,Z+		; day base word into X (little endian format!)
	lpm	XH,Z

	add	xortmp1,XL	; add date to "month days" base
	adc	xortmp2,XH

	lds	Tmp1,d_y1st	; fetch weekday# (1.1.)
	cpi	Tmp1,5		; weekday# 5-7?
	brsh	CB_W_N17	; yes

	; week 1 starts with 1.1., week 2 starts at (9-weekday#)
	; find out how long week 1 is
	ldi	Tmp2,9
	sub	Tmp2,Tmp1	; subtract weekday# from 9, Tmp2 now = (last day # of week 1) + 1
				; = start day # of week 2 !
	clr	Tmp1		; clear "high byte" of "last day # of week 1" (16 bit math)
	cp	xortmp1,Tmp2	; given date same or higher than start of week 2 ?
	cpc	xortmp2,Tmp1
	brsh	CB_W_N16	; yes
	ldi	Tmp1,1		; no, given date is in week number 1
	sts	week_buf,Tmp1
	rjmp	CB_W_Exit

CB_W_N16:
	; calculate week # > 1
	sub	xortmp1,Tmp2	; subtract week 1's days
	sbc	xortmp2,Tmp1
	rcall	CB_D7		; /7, week # result low now in YL
	ldi	Tmp2,2
	add	YL,Tmp2
	rcall	CB_MW_Cal	; check (if last week of year) if it's legal, store #
	rjmp	CB_W_Exit

CB_W_N17:	; week 1 starts at 9-weekday#, earlier dates belong to previous year's last week
	ldi	Tmp2,9		; find out how long last week of previous year is
	sub	Tmp2,Tmp1	; subtract weekday# from 9, Tmp2 now = (last day # of week 52/53) + 1
				; = start day # of week 1 !
	clr	Tmp1		; clear "high byte" of "last day # of week 52/53" (16 bit math)
	cp	xortmp1,Tmp2	; given date same or higher than start of week 1 ?
	cpc	xortmp2,Tmp1
	brsh	CB_W_N19	; yes, continue
	lds	xortmp2,cent_buf	; no - we need previous year's last week# (52/53) here
	lds	xortmp1,year_buf	; fetch given century/year
	push	xortmp2		; save data
	push	xortmp1
	cbr	B_Flags2,0x80	; make sure bit 7 (leap year flag) is cleared
	clr	Tmp1
	ldi	Tmp2,1
	sub	xortmp1,Tmp2	; for previous year, subtract "1" (16 bit math)
	sbc	xortmp2,Tmp1
	sts	cent_buf,xortmp2	; result to main buffer for "get weekday" routine
	sts	year_buf,xortmp1
	rcall	CB_W_Inf	; find prev. year's weekday# (1.1.) & leap year flag
	pop	xortmp1		; fetch & restore original century/year data
	pop	xortmp2
	sts	cent_buf,xortmp2
	sts	year_buf,xortmp1
	rcall	CB_Max_W	; get max. # of weeks for (prev.) year to Tmp1

CB_W_N18:
	sts	week_buf,Tmp1	; store previous year's last week# for this date
	rjmp	CB_W_Exit

CB_W_N19:
	; calculate week # >= 1
	sub	xortmp1,Tmp2	; subtract previous year's overlap (week 52/3) days
	sbc	xortmp2,Tmp1
	rcall	CB_D7		; /7, week # result low now in YL
	inc	YL
	rcall	CB_MW_Cal	; check (if last week of year) if it's legal, store #

CB_W_Exit:
	cbr	B_Flags2,0x80	; clear bit 7 (leap year flag)
	ret			; EXIT function


;**** local subroutines (not to be called from somewhere else!) ****

CB_D7:	;**** divide modulo by 7, result (low byte) in YL
	mov	XL,xortmp1	; load dividend
	mov	XH,xortmp2
	call	Div16_7
	ret


CB_W_Inf:	;**** Stores weekday# of Jan. 1st (d_y1st) & leap year flag in given year
	call	RTC2week	; save current date info
	ldi	Tmp1,1
	sts	month_buf,Tmp1	; Jan. 1st of this year
	sts	date_buf,Tmp1
	call	Perp_Cal	; calculate day-of-week (1.1.xxxx)
	lds	Tmp1,day_buf	; get day-of-week number of 1.1.
	sts	d_y1st,Tmp1	; store it
	call	Week2RTC	; restore RTC buffer data
	call	Test_LY		; leap year ?
	ret


CB_Max_W:	;**** calculate max. # of weeks in given year, return in Tmp1
	lds	Tmp1,d_y1st	; fetch weekday# of 1.1.
	cpi	Tmp1,4		; Jan. 1st of year a Thursday?
	breq	CB_M_W2		; yes
	cpi	Tmp1,3		; Jan. 1st of year a Wednesday?
	breq	CB_M_W1		; yes
	ldi	Tmp1,52		; no, 52-week-year
	ret

CB_M_W1:
	ldi	Tmp1,52		; Wednesday & no leap year -> 52 week year
	sbrc	B_Flags2,7	; set if year is a leap year
	ldi	Tmp1,53		; Wednesday & leap year -> 53 weeks
	ret

CB_M_W2:
	ldi	Tmp1,53		; jan 1st = Thursday -> 53 week year
	ret


CB_MW_Cal:	;**** check if (last) week# is legal, store week# (uses YL for week#)
	cpi	YL,53		; week# = 53?
	brne	CB_MW1		; no, store week#
	lds	Tmp2,mweek_buf	; yes, check if week# 53 is legal
	cpse	YL,Tmp2
	ldi	YL,1		; no, date belongs to next year's week #1
CB_MW1:
	sts	week_buf,YL	; store week#
	ret

CB_Table:	; day-of-year bases from Jan. to Dec.
.DW 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334

;**** End of Function CB_Week_Num ------------------------------------- ****


;***************************************************************************
;*
;* Function "Test_LY" - tests a year for leap year
;* (sets B_Flags2,7 if true)
;*
;***************************************************************************

Test_LY:
	lds	xortmp2,year_buf; get year data from buffer
	tst	xortmp2		; if year is "0", century is divisible by 100
	breq	T_LY2		; -> go check if it's also divisable by 400
	mov	xortmp1,xortmp2	; else check the "is it divisable by 4" rule
	cbr	xortmp1,0x03	; clear bits 0 & 1
	eor	xortmp2,xortmp1	; year is divisible by 4 if bits 0 & 1 = "0"

T_LYl:
	tst	xortmp2		; if year/century is divisible by 4 -> leap year:
	breq	T_LY3		; leap year = true
	cbr	B_Flags2,0x80	; no leap year, -> 365 days in year, clear bit 7
	ret

T_LY2:
	lds	xortmp2,cent_buf; get century data from buffer -
	mov	xortmp1,xortmp2	; we are in the last year of a century
	cbr	xortmp1,0x03	; clear bits 0 & 1:
	eor	xortmp2,xortmp1	; century is divisible by 4(00) if bits 0 & 1 = "0"
	rjmp	T_LYl

T_LY3:
	sbr	B_Flags2,0x80	; leap year, -> 366 days in year, set bit 7
	ret

;**** End of Test_LY -------------------------------------------------- ****


;***************************************************************************
;*
;* Data Buffer copy routines "Week2RTC", "RTC2week"
;*
;* - Exchange time/date data between clock data buffers
;*
;***************************************************************************


Week2RTC:; copy week # buffer data
        ldi     ZL,min_bufw	; (saved RTC data to RTC data buffer)
        ldi     YL,min_buf
	rjmp	ram2ram1

RTC2week:; copy RTC data
        ldi     ZL,min_buf	; (save RTC data to week # cal. data buffer)
        ldi     YL,min_bufw
	;rjmp	ram2ram1

ram2ram1:
        ldi     Tmp2,7
        clr     ZH
        clr     YH

ram2ram2:                       ; copy 7 bytes
        ld      Tmp1,Z+         ; get data
        st      Y+,Tmp1         ; store data
        dec     Tmp2            ; decrement counter
        brne    ram2ram2        ; if not done, loop more
	ret

;**** End of data buffer copy routines -------------------------------- ****


;**************************************************************************
;*
;* Function "Div16_7"
;* Divides a unsigned 16 bit word by 7
;* Returns quotient in YH:YL and remainder in XL
;*
;* Equations by D: W. Jones:
;*
;*	Reciprocal mul w. extra precision:
;*	unsigned int A;
;*	unsigned int scaled_reciprocal = 0x2493;
;*	unsigned int Q; /* the quotient */
;*
;*	Q = (((A * 0x2493) >> 16) + A) >> 3
;*
;* Uses: high regs: 8 (r16, r17, r18, r19, X, Y)
;*	 low regs:  3 (r0, r1, r2)
;*
;*	 words:     40 (w. push/pop)
;*	 cycles:    58 (w. push/pop)
;*
;* Note: Hardware multiplier required ("mul" instruction)
;*
;**************************************************************************

Div16_7:
	push	r2
	push	r19
	push	r18
	push	r17
	push	r16

	ldi	YH,0x24		; scaled reciprocal for /7
	ldi	YL,0x93

	; Q = A * 0x2493
	; (r19:r18:r17:r16 = XH:XL * YH:YL)
        clr     r2
        mul     XH, YH		; ah * bh
        movw    r19:r18, r1:r0
        mul     XL, YL		; al * bl
        movw    r17:r16, r1:r0
        mul     XH, YL		; ah * bl
        add     r17, r0
        adc     r18, r1
        adc     r19, r2
        mul     YH, XL		; bh * al
        add     r17, r0
        adc     r18, r1
        adc     r19, r2

	; Q = Q >> 16: use r19:r18 as word
	; Q = Q + A
	add	r18,XL
	adc	r19,XH

	; Q = Q >> 3
	ror	r19		; do the last 3 shifts, including
	ror	r18		; carry from previous addition

	lsr	r19
	ror	r18
	lsr	r19
	ror	r18

	; r19:r18 now "Q" (= result >> 19)
	; R = A - 7*Q;
	ldi	r16,7		; multiply r19:r18 by 7
	mul     r18, r16	; al * bl
	movw    YH:YL, r1:r0
	mul     r19, r16	; ah * bl
	add     YH, r0
	sub	XL,YL
	sbc	XH,YH
	mov	YH,r19		; make copy of Q
	mov	YL,r18

	; XL holds "R"
	; YH:YL holds "Q"
	pop	r16
	pop	r17
	pop	r18
	pop	r19
	pop	r2

	ret

;**** End of function Div16_7 ------------------------------------------****

