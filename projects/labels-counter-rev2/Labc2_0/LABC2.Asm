;**********************************************************************
;* Project           : LABC2
;* Description       : Labels Counter Firmware 2.00
;* Author            : Nikos Bovos (tano@hol.gr)
;* Device To Use     : Atmel's ATtiny2313
;* Firmware Revision : 1.00 , July 17 2012 (TLRC1 ATmega8)
;*                   : 2.00 , March 17 2015 (ATtiny2313)
;* Timing Of MCU     : Internal 4 MHz Clock
;* File              : 0Info.Asm
;* Purpose           : Contains all usefull info for the project
;**********************************************************************
;*
;* This is the firmware for the device  " Labels Counter Version 2 "
;* It is used to count labels passing through 2 optical sensors
;* a) An Omron EE-SX398/498 transmissive sensor
;* b) A Vishay CNY-70 reflective sensor
;* Types of labels supported: Gap labels, Black Mark Labels, Notch
;* 
;**********************************************************************
;*
;* MCU's Resources Usage:
;*
;* PORTB Usage - 8 BIT PORT (PB0-PB7)
;* PB7 : OUTPUT , //Not Used in Firmware 2.00///
;* PB6 : OUTPUT , 7-Segments displays, segment g
;* PB5 : OUTPUT , 7-Segments displays, segment f
;* PB4 : OUTPUT , 7-Segments displays, segment e
;* PB3 : OUTPUT , 7-Segments displays, segment d
;* PB2 : OUTPUT , 7-Segments displays, segment c
;* PB1 : OUTPUT , 7-Segments displays, segment b
;* PB0 : OUTPUT , 7-Segments displays, segment a
;*
;*
;* PORTD Usage - 7 BIT PORT (PD0-PD6)
;* PD6 : OUTPUT , 10000's display drive (to bjt npn driver)
;* PD5:  OUTPUT , 1000's display drive (to bjt npn driver)
;* PD4 : OUTPUT , 100's display drive  (to bjt npn driver)
;* PD3 : INPUT Clear Counting Button (INT1)
;* PD2 : INPUT from EE/SX398/498 Circuit (INT0) 
;* PD1 : OUTPUT , 10's display drive   (to bjt npn driver)
;* PD0 : OUTPUT , 1's display drive   (to bjt npn driver)
;* 
;**********************************************************************
;*
;* Display consists of 5 Common Anode Seven Segment Digits (SA52)
;* So the segments to show must be given in negative logic (0=ON,1=OFF)
;*
;* The "driving" of the digits will be done in positive logic
;* The five digits are for TensThous,Thousands, Hundrens, Tens, Units
;* Labelled: TenThou, Thou, Hund, Decs, Unit and also the driving
;* pulses (drvThou, drvHund etc) to five npn bjts (BC547/8 etc) 
;*
;**********************************************************************

;**********************************************************************
;* Project           : LABC2
;* Description       : Labels Counter Firmware 2.00
;* Author            : Nikos Bovos (tano@hol.gr)
;* Device To Use     : Atmel's ATtiny2313
;* Firmware Revision : 1.00 , July 17 2012 (TLRC1 ATmega8)
;*                   : 2.00 , March 17 2015 (ATtiny2313)
;*		     : 2.10 , June/July 2016 2016 (LABC2)
;* Timing Of MCU     : Internal 1 MHz Clock
;* File              : 1Defs.Asm
;* Purpose           : Contains all definitions of the project
;**********************************************************************
.NOLIST
.INCLUDE "tn2313.inc"
.LIST

;**********************************************************************
;*
;*        Definitions and Equ's Used in this firmware revision
;*
;**********************************************************************
;
; Low Registers Definitions (r0-r15)

.def	saveEEP		=r4	; Used in ShowDigit Macro (for EEPROM access)
.def	saveSREG	=r5	; Low Register to save Status Register

;
; High-Registers Definition (r16-r31)

.def	genio			=r16	; General I/O Register
.def    TensThou        =r17    ; Tens of thousands number
.def    Thou            =r18    ; Thousands Digit (Number to display when working)
.def    Hund            =r19    ; Hundrends Digit (Number to display when working)
.def    Decs            =r20    ; Tens      Digit (Number to display when working)
.def    Unit            =r21    ; Units     Digit (Number to display when working)
.def    medium          =r22    ; Medium counter for delays
.def    fine            =r23    ; Fine counter for delays
.def	large			=r24	; Large counter for delays
.def	Mask			=r25	; Mask for ON Leds

;
; Equivalents (equ's) for driving signals
;
.equ    drvTensThou     =PORTD6
.equ    drvThou         =PORTD5 
.equ    drvHund         =PORTD4  
.equ    drvDecs         =PORTD1
.equ    drvUnit         =PORTD0


;**********************************************************************
;*
;*    Definitions of usefull MACROS Used in this firmware revision
;*
;**********************************************************************

;****************************************************************************
;* Macro	: Turn off all 7-segment segments
;* Name		: DigitsOFF
;* Parameters	: None
;* Notes	: Turn off all driving of BJT's for displays
;****************************************************************************
.MACRO DigitsOFF			; Turn OFF All Digits
        cbi     PORTD, drvUnit
        cbi     PORTD, drvDecs
        cbi     PORTD, drvHund  
        cbi     PORTD, drvThou
        cbi     PORTD, drvTensThou
.ENDMACRO


;****************************************************************************
;* Macro        : Enable Tens Thousands Digit
;* Name         : dispTensThou
;* Parameters	: None
;* Notes	: Turn on Thousands digit
;****************************************************************************
.MACRO dispTensThou                ; Display Digit "Thousands" Macro
        cbi     PORTD, drvThou     ; All others remain OFF
        cbi     PORTD, drvHund     ; All others remain OFF
        cbi     PORTD, drvDecs     ; All others remain OFF
        cbi     PORTD, drvUnit     ; All others remain OFF
        sbi     PORTD, drvTensThou ; Fire Tens of Thousands Digit
.ENDMACRO


;****************************************************************************
;* Macro	: Enable Thousands Digit
;* Name		: dispThou
;* Parameters	: None
;* Notes	: Turn on Thousands digit
;****************************************************************************
.MACRO dispThou                    ; Display Digit "Thousands" Macro
        cbi     PORTD, drvTensThou ; All others remain OFF
        cbi     PORTD, drvHund     ; All others remain OFF
        cbi     PORTD, drvDecs     ; All others remain OFF
        cbi     PORTD, drvUnit     ; All others remain OFF
        sbi     PORTD, drvThou     ; Set Thousands Digit
.ENDMACRO


;****************************************************************************
;* Macro	: Enable Hundrends Digit
;* Name		: dispHund
;* Parameters	: None
;* Notes	: Turn on Hundrends digit
;****************************************************************************
.MACRO dispHund                    ; Display Digit 3 Macro
        cbi     PORTD, drvTensThou ; All others remain OFF
        cbi     PORTD, drvThou     ; All others remain OFF
        cbi     PORTD, drvDecs     ; All others remain OFF
        cbi     PORTD, drvUnit     ; All others remain OFF
        sbi     PORTD, drvHund     ; Set Hundrends Digit
.ENDMACRO


;****************************************************************************
;* Macro	: Enable Decades Digit
;* Name		: dispDecs
;* Parameters	: None
;* Notes	: Turn on Decades digit
;****************************************************************************
.MACRO dispDecs                    ; Display Digit 2 Macro
        cbi     PORTD, drvTensThou ; All others remain OFF
        cbi     PORTD, drvThou     ; All others remain OFF
        cbi     PORTD, drvHund     ; All others remain OFF
        cbi     PORTD, drvUnit     ; All others remain OFF
        sbi     PORTD, drvDecs     ; Set Decades ON
.ENDMACRO


;****************************************************************************
;* Macro	: Enable Units Digit
;* Name		: dispUnit
;* Parameters	: None
;* Notes	: Turn on Units digit
;****************************************************************************
.MACRO dispUnit                    ; Display Digit 1 Macro
        cbi     PORTD, drvTensThou ; All others remain OFF
        cbi     PORTD, drvThou     ; All others remain OFF
        cbi     PORTD, drvHund     ; All others remain OFF
        cbi     PORTD, drvDecs     ; All others remain OFF
        sbi     PORTD, drvUnit     ; Set Units ON
.ENDMACRO


;****************************************************************************
;* Macro	: ShowDigit @0
;* Parameters   : @0 = The Numeric Value To Show On 7-Segment Displays
;* Notes	: Read from EEPROM equivalent value of digit to show
;*              : All equivalent to 7-Segment display digits are
;*              : stored on positions 0 to 9 on EEPROM, otherwise
;*              : in the command after mov saveEEP,@0 we had to
;*              : add the offset (i.e +10 positions will be
;*              : saveEEP = saveEEP + 0A (+ 10dec) and then out ...)
;*              : Since we don't write to EEPROM there is no need
;*              : to wait finishing of previous write before reading it.
;****************************************************************************
.MACRO ShowDigit                ; Show Digit Macro
        mov     saveEEP,@0      ; Copy to save_EEP the parameter to show
        out     EEAR,saveEEP    ; Define address to access
        sbi     EECR,EERE       ; Set READ EEPROM signal (EECR bit 0 =1)
        in      saveEEP,EEDR    ; Retrieve EEPROM contents to saveEEP
        out     PORTB,saveEEP   ; Out the result to PORTB
.ENDMACRO	

;**********************************************************************
;* Project           : LBC2
;* Description       : Labels Counter Firmware 2.00
;* File              : 2Inits.Asm
;* Purpose           : Initializations (Setup MCU peripherals)
;**********************************************************************

.cseg                       	; Code Segment of MCU
.org	$0000  					; Reset Vector (with no bootloader)
    	rjmp	Initialize		; On Reset Jump To Initialize Routine
    	rjmp	GapDetect		; On INT0=0 Jump to Gap Detect ISR
    	reti					; Not used INT1 Interrupt
    	reti                    ; Not used Timer1 Capture
    	reti                    ; Not used Timer1 Compare
    	reti                    ; Not used Timer1 Overflow
    	reti                    ; Not used Timer0 Overflow
    	reti                    ; Not used USART, Rx Complete
    	reti                    ; Not used UDR Empty
    	reti                    ; Not used USART, Tx Complete
    	reti                    ; Not used Analog Comparator Interrupt
    	reti                    ; Not used Pin Change
    	reti                    ; Not used Timer1 Compare B
    	reti                    ; Not used Timer0 Compare A
    	reti                    ; Not used Timer0 Compare A 
    	reti                    ; Not used USI Start
    	reti                    ; Not used USI Overflow
    	reti                    ; Not used EEPROM Ready
		reti                   	; Not used Watchdog Overflow


;**********************************************************************
;*
;*                       Initialize (Reset ISR) 
;*
;**********************************************************************
;
Initialize:
	ldi	genio,low(RAMEND)		; genio <- Last SRAM (low byte)
	out	SPL,genio            	; (128Bytes SRAM - no SP high byte)

; ---------------------------------------------------------------------
; All interrupts will be disabled, until all initialization is done

	cli                         ; ### Disable Interrupts ###

;------------------------ PORTB Initialization ------------------------
;
	ldi	genio, $FF				; genio <- 11111111 (all outputs)
	out	DDRB, genio				; Define Data Direction Register as all OUTPUTS
	out 	PORTB, genio		; Default Values = 1 (all segments off)

;------------------------ PORTD Initialization ------------------------
;               5 bits set to outputs, 2 inputs - 7 BIT PORT
;
; Set 0<<PD3 if INT1 is used otherwise leave it as is...
	ldi     genio, (1<<PD6)|(1<<PD5)|(1<<PD4)|(1<<PD3)|(0<<PD2)|(1<<PD1)|(1<<PD0)
	out     DDRD, genio     	;Define Data Direction (1=Output  0=Input)
	ldi     genio, (0<<PD6)|(0<<PD5)|(0<<PD4)|(0<<PD3)|(0<<PD2)|(0<<PD1)|(0<<PD0)
	out     PORTD, genio    	;Outs=0 to BJTs and disable pull-ups on inputs

;----------------------- INT0 Initialization --------------------------
; INT0 will trigger by the falling edge (5V -> 0V or 1 to 0 transition) 
;
	ldi		genio, (0<<ISC00)|(1<<ISC01)
	out		MCUCR, genio
	ldi		genio, (1<<INT0)
	out     GIMSK, genio    

;----------------------- INT1 Initialization --------------------------
; INT1 will trigger in low level (0 Volt) when button is pressed
; To be defined in a future implementation - currently not used
;
;	ldi	genio, (0<<ISC11)|(0<<ISC10)
;	out	MCUCR, genio
;	ldi     genio, (1<<INT1)
;	out     GIMSK, genio
        
;**********************************************************************
;* Project           : LABC2
;* Description       : Labels Counter Firmware 2.00
;* File              : 3Main.Asm
;* Purpose           : Main Program Routines
;* Created           : 17 March 2015 Nikos
;**********************************************************************
;
; Display Lbc21 message for about 1 sec
; Load equivalents on the digits
;
MainStart:
	rcall	DisplayStart	;Show Startup Messsage

	clr     TensThou		;Clear the counter
	clr     Thou            ;Clear the counter
	clr     Hund            ;Clear the counter
	clr		Decs			;Clear the counter
	clr		Unit			;Clear the counter
	clr		Mask			;Clear the Mask
	
	sei                     ;Enable Interrupts

;**********************************************************************
; The main routine displays the current number indefinetely
; If an INT0 occurs (gap detected) increase counters and show
;**********************************************************************
TheMain:
	rcall	ShowCountAlt	;Show alternative number on displays
	rjmp    TheMain         ;Do for ever until interrupted



;**********************************************************************
; Display Starting Message (could be saved on Subs File also)
;**********************************************************************
DisplayStart:
	ldi		Mask, $1F		;Load 11111 to Mask (all leds ON)

	ldi		TensThou, $47	;Load 'L' equivalent for CA displays
	ldi		Thou, $03		;Load 'b' equivalent for CA displays
	ldi		Hund, $27		;Load 'c' equivalent for CA displays
	ldi		Decs, $24		;Load '2' equivalent for CA displays
	ldi		Unit, $79		;Load '1' equivalent for CA displays

	DigitsOFF				;OFF All digits
	rcall	del500ms		;Delay 0,5seconds

	ldi		genio, 250		;Load genio with 250 (times to show message)

StartLoop:
	dispTensThou			;Enable 10000s digit
	out		PORTB, TensThou	;Output value to 7segment displays
	rcall	delay1ms		;and make a small delay

	dispThou		
	out		PORTB, Thou		;Output value to 7segment displays
	rcall	delay1ms		;and make a small delay

	dispHund
	out		PORTB, Hund		;Output value to 7segment displays
	rcall	delay1ms		;and make a small delay

	dispDecs
	out		PORTB, Decs		;Output value to 7segment displays
	rcall	delay1ms		;and make a small delay

	dispUnit
	out		PORTB, Unit		;Output value to 7segment displays
	rcall	delay1ms		;and make a small delay

	dec		genio			;Decrease counter (genio=genio-1)
	brne	StartLoop		;While genio>0 repeat

	ret						;Return from routine (came here with rcall);**********************************************************************
;* Project           : LABC2  
;* Description       : Labels Counter Version 2.00
;* File              : 4ISR.Asm
;* Purpose           : Interrupt Service Routines
;**********************************************************************

;**********************************************************************
;*                   INT0 Interrupt Service Routine
;*
;* INT0 is triggered when a high to low level transision is detected
;* Load the appropriate 'led mask' to Mask register
;* i.e if we come here, begin with units mask and continue to others
;**********************************************************************
GapDetect:
	in		saveSREG,SREG		;Copy Status Register to saveSREG
	
	ori		Mask, 1				;Set bit 0 of Mask (00000001)
    inc     Unit            	;Unit=Unit+1
	cpi		Unit,10         	;Compare Unit with 10
	breq    incDecs         	;Branch to incDecs if Unit=10
	rjmp	DoneISR				;else jump to out of ISR

incDecs:
	clr     Unit	        	;Load 0 to unit, iuncrease decs
	ori		Mask, 3				;Set bits 0,1 of Mask
	inc     Decs            	;Decs=Decs+1
	cpi     Decs,10         	;Compare with 10
	breq    incHund         	;Branch to incHund if desc=10
	rjmp	DoneISR				;else jump to out of ISR
	
incHund:
	clr     Unit	        	
	clr     Decs
	ori		Mask, 7				;Set 3 lower bits of Mask
	inc     Hund
	cpi     Hund,10
	breq    incThou
	rjmp	DoneISR
	
incThou:
	clr     Unit	        	
	clr     Decs
	clr     Hund
	ori		Mask, 15			;Set low nibble of Mask 
	inc     Thou            	;Increase Thousands
	cpi     Thou,10         	;Compare with 10
	breq    incTensThou
	rjmp	DoneISR
	
incTensThou:
	clr     Unit	        
	clr     Decs
	clr     Hund
	clr     Thou
	ori		Mask, 31			;Set the 5 lower bits on Mask
	inc     TensThou
	cpi     TensThou,10
	breq    Overlap
	rjmp    DoneISR

;Overlap occurs from 99999 to 100000 
;Clear counters & Re-apply mask
Overlap:
	clr		Mask
    clr     Unit
    clr     Decs
    clr     Hund
    clr     Thou
    clr     TensThou

DoneISR:
	out     SREG,saveSREG   ;Copy to SREG back (as it was before ISR)
	reti					;Return from Interrupt




;**********************************************************************
;*                   INT1 Interrupt Service Routine
;*
;* INT1 is triggered on low level signal - Not currently used
;**********************************************************************
;INT1ISR:
;	... Commands
;	reti                    ;Return from Interrupt        
;**********************************************************************
;* Project           : LABC2
;* Description       : Labels Counter Firmware 2.00
;* File              : 6Subs.Asm
;* Purpose           : Subroutines
;**********************************************************************




;**********************************************************************
;* Subroutine        : ShowCount
;* Parameters        : -
;* Purpose           : Display all digits, one by one at 1kHz refresh
;* Created           : 10 Nov 2014
;* Updated           : 17 March 2015 for 2.00
;**********************************************************************
ShowCount:
	dispTensThou            ;Enable Tens Thousands Digit
	ShowDigit TensThou	;Show Digit Tens Thousands
	rcall delay1ms         	;Delay for 7segments        
	dispThou		;Enable Thou digit
	ShowDigit Thou		;Show Digit Thousands
	rcall delay1ms          ;Delay for 7segments        
	dispHund		;Enable Thou digit
	ShowDigit Hund		;Show Digit Thousands
	rcall delay1ms         	;Delay for 7segments        
	dispDecs		;Enable Thou digit
	ShowDigit Decs		;Show Digit Thousands
	rcall delay1ms          ;Delay for 7segments        
	dispUnit		;Enable Thou digit
	ShowDigit Unit		;Show Digit Thousands
	rcall delay1ms          ;Delay for 7segments        
	ret			;Came here with rcall/icall



;**********************************************************************
;* Subroutine        : ShowCountAlt
;* Parameters        : -
;* Purpose           : Display necessary digits (Alternative display)
;* Created           : 10 Nov 2014
;* Updated           : 17 March 2015 for 2.00 , July 2016 fro 2.10
;**********************************************************************
ShowCountAlt:
	digitsOFF
	rcall delay1ms         	;Delay for 7segments        

	sbrs	Mask, 4		;Skip next if bit4 of Mask = 1 (ON)
	rjmp	ShowCnt3	;Continue with next digit
	dispTensThou            ;Enable Tens Thousands Digit
	ShowDigit TensThou	;Show Digit Tens Thousands
	rcall delay1ms         	;Delay for 7segments        

ShowCnt3:
	sbrs	Mask, 3		;Skip next if bit3 of Mask = 1 (ON)
	rjmp	ShowCnt2	;Continue with next digit
	dispThou		;Enable Thou digit
	ShowDigit Thou		;Show Digit Thousands
	rcall delay1ms          ;Delay for 7segments        
	
ShowCnt2:
	sbrs	Mask, 2		;Skip next if bit2 of Mask = 1 (ON)
	rjmp	ShowCnt1	;Continue with next digit
	dispHund		;Enable Thou digit
	ShowDigit Hund		;Show Digit Thousands
	rcall delay1ms         	;Delay for 7segments        

ShowCnt1:
	sbrs	Mask, 1		;Skip next if bit1 of Mask = 1 (ON)
	rjmp	ShowCnt0	;Continue with next digit
	dispDecs		;Enable Thou digit
	ShowDigit Decs		;Show Digit Thousands
	rcall delay1ms          ;Delay for 7segments        

ShowCnt0:
	sbrs	Mask, 0		;Skip next if bit0 of Mask = 1 (ON)
	rjmp	ShowOut		;Continue with next digit
	dispUnit		;Enable Thou digit
	ShowDigit Unit		;Show Digit Thousands
	rcall delay1ms          ;Delay for 7segments        
ShowOut:
	ret			;Came here with rcall/icall







;**********************************************************************
;* Delay Routine By TDC_AVR 0.02 By Nikos Bovos
;* XTAL/Res Frequency : 1 MHz
;* AVR Clock Cycle    : 1 usec
;* Delay Produced     : 1003 Clock Cycles aprox
;* Equivalent Time    : 1 msec approx
;*                    : medium=2 , fine=165 for 1ms
;*                    : medium=17, fine=200 for 10ms
;**********************************************************************
delay1ms:
	ldi	medium, 2	; Load Medium Counter ($02)
lfine1ms:
	ldi	fine, 165	; Load Fine Counter ($A5)
stdec1ms:
	dec	fine		; Decrease Fine Counter
	brne	stdec1ms	; If fine Not Zero Goto stdec
	dec	medium		; fine = 0, Decrease Medium
	brne	lfine1ms	; If Medium <>0, Reload Fine
	ret			; Return From Subroutine



;*************************************************************************************
;* Delay Routine By TDC_AVR 0.02 By Nikos Bovos
;* XTAL/Res Frequency : 1 MHz
;* AVR Clock Cycle    : 1 usec
;* Delay Produced     : 551209 Clock Cycles
;* Equivalent Time    : 551,209 msec
;*************************************************************************************
del500ms:
	ldi	large,3		; Load Large Counter ($03)
lmed500:
	ldi	medium,230	; Load Medium Counter ($E6)
lfine500:
	ldi	fine,241	; Load Fine Counter ($F1)
stdec500:
	dec	fine		; Decrease Fine Counter
	brne	stdec500	; If fine Not Zero Goto stdec
	dec	medium		; fine = 0, Decrease Medium
	brne	lfine500	; If Medium <>0, Reload Fine
	dec	large		; Decrease Large Counter
	brne	lmed500		; If Large <>0, Reload Medium
	ret			; Return From Subroutine
;**********************************************************************
;* Project           : LABC2
;* Description       : Labels Counter Firmware 2.00
;* File              : 9Eeprom.Asm
;* Purpose           : Contains values to be stored on EEPROM
;*                   : Since common anode 7-segment displays are used
;*                   : we use negative logic values (0=on, 1=off)
;**********************************************************************

.eseg           ;EEPROM segment
.org    $000    ;Start from position 0
EEtable:      
.db 	$40,$79,$24,$30,$19,$12,$02,$78,$00,$18
;            0   1   2   3   4   5   6   7   8   9 
;End Of Firmware 2.00
;March 17, 2015
