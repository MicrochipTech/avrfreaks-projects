; Headphone.asm   Run the LM4832N audio amp processor IC using I2C protocol
; August 6, 2007  version 0.81   ===  alan_probandt@yahoo.com    


; The National LM4832N is a full audio control system and headphone amp chip. 
; It is controlled by an I2C interface from the AVR Tiny13 microcontroller.
; 
; The control has one potentiometer and one pushswitch.  On power-up, the pot
; controls volume.  After the first push-switch press, the pot controls treble. 
; The next press selects bass.  The LM4832N keeps the previous pot settings.  
; The next button press cycles back to volume controlled by the pot knob.


.nolist
.include "tn13def.inc"
.list
;
;         	     Atmel AVR Tiny13
;     		    RESET/  1|8 VCC  +5V
;   	ADC3 potentiometer  2|7 PB2  PushSwitch 
;   		ADC2   PB4  3|6 PB1  SCK   bit_scl
;            	       Gnd  4|5 PB0  SDA   bit_sda

;                LM4832N		    7   6   5   4   3   2   1   0   LM4832N controls
;         Left 3D  1|28  Right 3D           -----------------------------------------------------
;          Bypass  2|27  Bypass   	    0   0   0   -   -   n   n   n   Input Volume Control 
;  Left Power Out  3|26  Right Power Out    0   0   1   -   n   n   n   n   Bass Level/Boost     
;       Power Gnd  4|25  Vdd +5V   	    0   1   0   -   n   n   n   n   Treble Level/Boost   
;    Left Loop In  5|24  Right Loop In      0   1   1   n   n   n   n   n   Right Volume         
;   Left Tone Out  6|23  Right Tone Out     1   0   0   n   n   n   n   n   Left Volume          
;    Left Tone In  7|22  Right Tone In      1   0   1   -   n   n   n   n   Mic Input and Gain   
;   Left Loop Out  8|21  Right Loop Out     1   1   0   n   n   n   n   n   Mic Volume           
;         Left In  9|20  Right In           1   1   1   n   n   n   n   n   General Control      
;      Analog Gnd 10|19  General Output
;        Mic In 1 11|18  SDA
;        Mic In 2 12|17  SCL  
;         Vdd +5v 13|16  AddressBit2
;         Mic Out 14|15  AddressBit1
     
     


; constants for specific AVR device type
;.equ	clock		= 8000000 ; ICE200 modeling a AT90S4433 AVR
.equ	clock		= 4600000 ; Tiny13

;; Timer equates
.equ	Tcount	= TCNT0
.equ	TcontrolReg	= TCCR0b
.equ	TmaskReg	= TIMSK0

; I/O port equates- IN/OUT OK and can use CBI and SBI instructions to change individual bits
.equ	port_i2c  	=  portb
.equ	ddr_i2c   	=  ddrb
.equ	pin_i2c		=  pinb
.equ	PushSwitchInPort =  pinb
.equ	pushswitch	=  2
.equ	bit_scl   	=  1
.equ	bit_sda   	=  0


; constants that are the same for all AVR devices
.equ	PotChannel    	= 3
.equ	DEBOUNCE_OK	= 0xff
.equ	DEBOUNCE_BAD	= 0


;*********************************************************************************************
;  Register definitions for PCKB
;
;  lower registers  r0 - r15
; r0 is temporary storage of byte retrieved from LPM instruction
.def	SREGtemp	= r1
.def	ADCstore	= r2
.def	PrevADC		= r3
.def	PushSwitchReg	= r4


;  upper registers
.def	temp		= r16	; scratch register
.def	irqtemp		= r17 ; interrupt's scratch register
.def	temp2		= r18
.def	counter		= r19
.def	SwitchMode	= r20

.cseg

.org $0000       
	rjmp  reset	; Reset handler   $000
	
; interrupt vector table for AVR Tiny13	
	reti		;= 0x0001 External Interrupt 0
	reti		;= 0x0002 External Interrupt Request 0
	reti		;= 0x0003 Timer Overflow
	reti		;= 0x0004 EEPROM Ready
	reti		;= 0x0005 Analog Comparator
	reti		;= 0x0006 Timer/Counter Compare Match A 
	reti		;= 0x0007 Timer/Counter Compare Match B 
	reti		;= 0x0008 Watchdog Time-out
	reti		;= 0x0009 ADC Conversion Complete


reset:
; initialize input/output  ports and pull-up resistors
	sbi	ddr_i2c , bit_scl
  	sbi	ddr_i2c , bit_sda  
  	cbi	ddr_i2c , PushSwitch ; done by default on power-up
  	
  	sbi	port_i2c, bit_scl  ; start with high output
  	sbi	port_i2c, bit_sda  ; start with high output 
  	sbi	port_i2c, PushSwitch  ; pull-up resistor on
		
; initialize timer
; timer has /1024 prescale, overflow every 1/18 second at 4.8MHz clock.  
;  256*1024=262144 clocks   54613 microseconds
	ldi	temp, (1<<TOIE0)
	out	TmaskReg, temp	; enable interrupt
	ldi	temp, 0
	out	Tcount, temp
	ldi	temp,  (   (1<<CS02) | (0<<CS01) | (1<<CS00)  )
	out 	TcontrolReg , temp  ; start incrementing timer at every 1024 system clocks

; initialize global interrupt enable
	sei	; switch on the AVR general interrupt enabler

; initialize application program variables
	ldi	temp,  (0<<PUD) | (0<<SE) | (0<<SM1) | (0<<SM0) | (0<<ISC01) | (0<<ISC00) ; idle mode
	out	MCUCR, temp
	ori	temp,  (1<<SE) ; enable sleep mode
	out	MCUCR, temp     
	ldi	temp, (1<<PushSwitch)
	mov	PushSwitchReg, temp
	clr	SwitchMode ; set to volume adjust mode for start
	


;%%%%%%%%%%%%%%%%%%%%%%%%%%%
; 	 MAIN LOOP	%%%%
;%%%%%%%%%%%%%%%%%%%%%%%%%%%

; Each pushswitch press puts the AVR into a different control mode. Each mode controls a different aspect of the LM4832.
; The AVR stays in the control mode until a new pushswitch press happens.

Main:
	sleep
	mov	PrevADC, ADCstore
	        
	in	temp, PushSwitchInPort    ; check for switch = gnd
	and	temp, PushSwitchReg    ; 0b00000100=PshSwReg   If pushswitch is not pressed,
	brne	doSwitch ; then bit is high and zero flag is not set.  So go to current Switchmode's routine.
	
	rcall	SwitchDebounce	; wait until push button is released
	inc	SwitchMode
	
doSwitch:
	cpi	SwitchMode, 0
	breq	doVolume
	
	cpi	SwitchMode, 1
	breq	doTreble
	
	cpi	SwitchMode, 2
	breq	doBass
 	
	clr	SwitchMode

doVolume:
	ldi	temp, PotChannel
	rcall	doADC
	lsr	ADCstore ; max value 0x7f
	lsr	ADCstore ; max value 0x3f
	lsr	ADCstore ; max value 0x1f
	cp	PrevADC, ADCstore
	breq	Main
	
	rcall	NewVolume
	rjmp	Main
	
	
doTreble:
	ldi	temp, PotChannel         
	rcall	doADC                    
	lsr	ADCstore ; max value 0x7f
	lsr	ADCstore ; max value 0x3f
	lsr	ADCstore ; max value 0x1f
	lsr	ADCstore ; max value 0x0f
	cp	PrevADC, ADCstore        
	breq	Main                     
    
	rcall	NewTreble
	rjmp	Main

doBass:
	ldi	temp, PotChannel         
	rcall	doADC                    
	lsr	ADCstore ; max value 0x7f
	lsr	ADCstore ; max value 0x3f
	lsr	ADCstore ; max value 0x1f
	lsr	ADCstore ; max value 0x0f
	cp	PrevADC, ADCstore        
	breq	Main                     

	rcall	NewBass
	rjmp	Main

		
	
;%%%%%%%%%%%%%%%%%%%%%%%%%%%
;
;   Subroutines
;
;%%%%%%%%%%%%%%%%%%%%%%%%%%%


;**************************************************************************
;  NewVolume   send new ADC reading to LM4832N
;
; send:		ADCstore
; return:  
;**************************************************************************
NewVolume:
	rcall	DoStart
	ldi	temp, 0b10000000     ; write I2C addr for LM4832N
	rcall	TX_BYTE
	rcall	Acknow               ; check for correct acknowledge
	brcs 	NVexit
   	mov  	temp, ADCstore       ; write left volume byte 0-0x1f
	andi	temp, 0b00011111 
	ori	temp, 0b10000000     ; left volume command 0b100-----
	rcall	TX_BYTE              ; command byte
	rcall	Acknow               ; check for correct acknowledge
	brcs 	NVexit
	mov	temp, ADCstore       ; write right volume byte 0-0x1f
	andi	temp, 0b00011111 
	ori	temp, 0b01100000     ; right volume command 0b011-----
	rcall	TX_BYTE              ; command byte
	rcall	Acknow               ; check for correct acknowledge
	brcs 	NVexit
	rcall	DoStop
NVexit:
	ret
;**************************************************************************




;**************************************************************************
;  NewTreble   send new ADC reading to LM4832N
;
; send:		ADCstore
; return:  
;**************************************************************************
NewTreble:
	rcall	DoStart
	ldi	temp, 0b10000000     ; write I2C addr for LM4832N
	rcall	TX_BYTE
	rcall	Acknow               ; check for correct acknowledge
	brcs	doTexit
   	mov	temp, ADCstore       ; write new Treble value 0-12 (6 is flat response)
	andi	temp, 0b00001111 
	ori	temp, 0b01000000     ; treble command 0b010-----
	rcall	TX_BYTE              ; command byte
	rcall	Acknow               ; wait acknowledge
	brcs	doTexit
	rcall	doStop
doTexit:	     
	ret
;***************************************************************************




;***************************************************************************
;  NewBass   send new ADC reading to LM4832N
;
; send:		ADCstore
; return:  
;***************************************************************************
NewBass:
	rcall	DoStart
	ldi	temp, 0b10000000     ; write I2C addr for LM4832N
	rcall	TX_BYTE
	rcall	Acknow               ; check for correct acknowledge
	brcs	doBexit
   	mov	temp, ADCstore       ; write new Bass value 0-12 (6 is flat response)
	andi	temp, 0b00001111 
	ori	temp, 0b00100000     ; bass command 0b001-----
	rcall	TX_BYTE              ; command byte
	rcall	Acknow               ; wait acknowledge
	brcs	doBexit
	rcall	DoStop
doBexit:	     
	ret
;***************************************************************************





;***************************************************************************
;  DoStart      I2C start condition is lowering SDA while SCK is high
;
; send:
; return:
;***************************************************************************
DoStart:
	cbi	PORT_I2C, BIT_SDA      ;start condition
;============================ delay 10 cycles 1.25uS
	ldi	counter, 3  ;
DoStr0: dec	counter     ;
	brne	DoStr0      ;
;============================ 
	cbi 	PORT_I2C,BIT_SCL
;============================ delay 10 cycles 1.25uS
	ldi	counter, 3  ;
DoStr1: dec	counter     ;
	brne	DoStr1      ;
;============================
	ret
;***************************************************************************




;***************************************************************************
;  DoStop      I2C stop condition is raising SDA while SCK is high
;
; send:
; return:
;***************************************************************************
DoStop:
	cbi	PORT_I2C,BIT_SDA     ;stop condition
;============================ delay 10 cycles 1.25uS
	ldi	counter, 3  ;
Stop0:	dec	counter     ;
	brne	Stop0       ;
;============================
	sbi     PORT_I2C,BIT_SCL  
;============================ delay 10 cycles 1.25uS
     	ldi	counter, 3  ;
Stop1:  dec	counter     ;
	brne	Stop1       ;
;============================
	sbi	PORT_I2C,BIT_SDA
;============================ delay 10 cycles 1.25uS
     	ldi	counter, 3  ; 
Stop2:  dec	counter     ; 
	brne	Stop2       ; 
;============================ 
	ret
;**************************************************************************





;**************************************************************************
; Acknow:    Check bit ACKNOWLEDGE 
;
; send:                                     
; return:   CARRY = 1: FAIL     CARRY = 0: PASS  
;**************************************************************************
Acknow: 
	cbi	DDR_I2C, BIT_SDA  ;make data bit input
	sbi	PORT_I2C, BIT_SDA ; turn on pull-up resistor
;============================ delay 10 cycles 1.25uS
	ldi	counter, 3  ;
Ack0:	dec	counter     ;
	brne	Ack0        ;
;============================
 	sbi	PORT_I2C, BIT_SCL ; clock made high by Master AVR
;============================ delay 10 cycles 1.25uS
	ldi	counter, 3  ;
Ack1:	dec	counter     ;
	brne	Ack1        ;
;============================
      ; Check state of Data pin for Low acknowledge
	sec     ; return flag = fail ACK
  	sbis	PIN_I2C, BIT_SDA
	clc	; return flag = ACK OK
	cbi	PORT_I2C, BIT_SCL; clock pulled low by Master AVR
;============================ delay 16 cycles 2uS
	ldi	counter, 5  ; 
Ack2: 	dec	counter     ; 
	brne	Ack2        ; 
;============================ 
	sbi	DDR_I2C,BIT_SDA  ;BIT_SDA output port
	ret
;*************************************************************************



;*************************************************************************
;*  Transmit one byte to the I2C device: after start condition, before SDA MSB
;
; Transmit data byte 0xaa - use Terminal font  (---- = logic high +5v)
;SCL----\____/--\__/--\__/--\__/--\__/--\__/--\__/--\__/--\__/--\___/-----   
;SDA--\____/------\____/------\____/------\____/------\_______________/---
;     start   7     6     5     4     3     2     1      0   ACK   stop
;
;I2C start or stop is a change on SDA when SCL is high.  SDA is input port at ACK.
;
; send:   temp with byte to be written
; return: 
; register use:  temp2                            
;*************************************************************************
TX_BYTE:   
	ldi	Temp2, 0x08
TXloop:	sbi	PORT_I2C, BIT_SDA ; set logic level on output pin
	lsl	Temp           ; MSB first
	brcs	TX1 
	cbi	PORT_I2C, BIT_SDA  ; clear logic level on output pin 
;=========================== delay 10 cycles 1.25uS
TX1:	ldi	counter, 3 ;
Txd0:	dec	counter    ;
	brne	Txd0       ;
;===========================
 	sbi	PORT_I2C, BIT_SCL ; clock up
;=========================== delay 10 cycles 1.25uS
	ldi	counter, 3 ;
TXd1:	dec	counter    ;
	brne	TXd1       ;
;===========================
	cbi	PORT_I2C, BIT_SCL ;clock down, data read here
;=========================== delay 19 cycles 2.25uS
	ldi	counter, 6 ; 
TXd2:	dec	counter    ; 
	brne	TXd2       ; 
;=========================== 
	dec	Temp2    
	brne	TXloop
	ret




;*************************************************************************
;   doADC   - set up, run analog-to-digital convertor, then return high bits (9-2) of result
;	Tiny13 version.  Each AVR device has slightly different ADC configuration.
;
;   send:	The ADC channel in Temp register bits 1 and 0
;   return:	left aligned upper 8 bits of result in ADCstore register
;*************************************************************************
doADC:	; Tiny13 version
	andi	temp, 0b00000011 ; preserve MUX bits
	ori	temp, (1<<ADLAR); return bits 9-2 in ADCH; REFS0 cleared for VCC reference
	out	ADMUX, temp; VCC is reference; ADCh=bits 9-2
; ADEN = ADC enable;  ADIE = irq enable; ADPS2 set=clock/16 for ADCclk
 	ldi	temp, (1 << ADEN) | (1 << ADPS2)
	out	ADCSRA, temp
	sbi	ADCSRA, ADSC ; start conversion
doADC0:	sbic	ADCSRA, ADSC; set means ADC conversion still in progress
	rjmp	doADC0
	in	ADCstore, ADCH
	ret
 


;******************************************************************************************
; SwitchDebounce:  push-switch debounce code with in-line poll delays - returns when switch released 
;  sys clock 4.8MHz = 1/24 second debounce delay
;
;  send:  PushSwitchReg (one bit only set) , PushSwitchInPort
;  return:  temp == 0xff >> debounce OK; temp2 == 0 >> bad switchpress
;  uses:  temp, temp2
;******************************************************************************************
SwitchDebounce:
	ldi	temp2, 0xff     ;  switch press down debounce delay period 1/24 second 
	ldi	temp,  0xff	; (3 * 255)  * 255   = 195075 cycles 40640uSec    @24.6 per second
psdelay0: dec	temp
	brne	psdelay0
	dec	temp2
	brne	psdelay0
 
	in	temp, PushSwitchInPort
	and	temp, PushSwitchReg
	brne	PSbad  ;should be stable low at this point

; stable down press (pin = 0), now wait for release (pin <> 0)

psTEST2: in	temp, PushSwitchInPort
	and	temp, PushSwitchReg
	breq	psTEST2  ; AND result will be low as long as switch is being pressed

	ldi	temp2, 0xff  ;  switch press up debounce delay period 1/24 second
	ldi	temp, 0xff
psdelay1: dec	temp
	brne	psdelay1
	dec	temp2
	brne	psdelay1
 
	in	temp, PushSwitchInPort
	and	temp, PushSwitchReg
	breq	PSbad

; stable push-switch release, continue program
	ldi	temp, DEBOUNCE_OK
	ret
PSbad:	ldi	temp, DEBOUNCE_BAD
	ret



;.db  "****  Headphone.asm  ver 0.81 "
.db   "****  eHdahpno.esa mv re0 8.1 "