;*******************************************************************************
; Title:	SH11, Temperature and relative humidity sensor
; Author: 	Rolf Bakke
;		Modified by Anders Runeson, arune@users.sf.net
; Version:	1.0.0
; Date:		2003-06-29
;
; Target:	AT90Sxxxx (almost All AVR Devices with SRAM, can be modified
; 		to be used with any device)
;
; DESCRIPTION
; This application is routines for retreiving data from an SH11.
; The timing is adapted for 4 MHz crystal
;
; USAGE
; Call readsh11, see GetTempAndHumid
; after this sub the data can be found in sram
; rh_high at 0x80, rh_low at 0x81
; t_high  at 0x82, t_low  at 0x83
; if no timeout occured
;
;
; Credits to rolf r bakke for his sh11 code, it had to be partly rewritten
; It can be found at http://www.avrfreaks.net under projects, project 92
;*******************************************************************************




; using 2323 (8 pin avr)
.include "2323def.inc"

;***** Pin definitions

;Hardware: http://www.sensirion.com/sensors/humidity/sensors_devices/sensorSHT11.htm
.equ	Sens_clock			=PB0
.equ	Sens_data			=PB2
; these pins are direct connected, no pullups och any other hardware required
;if pins are changed to D then replace all DDRB, PINB and PORTB to DDRD, PIND and PORTD


;***** Global register variables
;**Regs 0 to 15 cannot handle SBCI, SUBI, CPI, ANDI, ORI, LDI, SER, SBR, CBR

.def	trt				=r14
.def	rhrt				=r15

;**

.def	temp				=R17	

.def	timer				=R21
.def	counter 			=R22
.def	data_RxTx			=R23


;***** Other 	
.equ	xtal				=4000000	;Clock frequency connected to AVR


.equ	Start_of_temp_rh_data		=0x80





;#############################################################
;#              RESET                                        #
;#############################################################
	;interrupt vectors
.ORG	0x00
		rjmp	RESET				;Reset Handler


RESET:

		cli
;#############################################################
;#              INIT                                         #
;#############################################################

		;INIT STACKPOINTER
		ldi 	temp,low(RAMEND)
		out 	SPL,temp



		;INIT PORT D



		;INIT PORT B
		cbi	PORTB,Sens_clock	;init clock line
		sbi 	DDRB,Sens_clock
	
		sbi 	PORTB,Sens_data		;init data line
		sbi 	DDRB,Sens_data



		;INIT TIMER



		sei				;Enable interrupts



;#############################################################
;#              MAIN                                         #
;#############################################################
;***** Program Execution Starts Here **************************************
Start:

;call GetTempAndHumid when you want to read data.


		rjmp	Start





GetTempAndHumid:

		cli				;don't interrupt it
	
		rcall	readsh11		;read sensor

		tst 	trt			;timeout occurred?
		breq 	dp_out			;yes, out		

		tst 	rhrt			;timeout occurred?
		breq 	dp_out			;yes, out

			
;now rh_high i stored at sram 0x80, rh_low at sram 0x81, t_high at 0x82, t_low at 0x83
;se manual for info how to calculate temp and rel humid!!!

		;PLACE YOUR CODE HERE
		;...
		;...
		
	dp_out:
		sei
ret



				;-------read humidity
readsh11:

		clr 	trt			;trt for timeout
		clr 	rhrt			;rhrt  for timeout
		
		rcall	rdelay
		
		ldi 	counter,10		;connection reset sequence
	
	re1:	
		rcall	chigh
		rcall	clow
		dec 	counter	
		brne 	re1
	
	
		rcall	chigh			;transmission start
		rcall	dlow
		rcall	clow
		rcall	chigh
		rcall	dhigh
		rcall 	clow
			
			
		ldi 	data_RxTx,0b00000101	;send byte: command for 'check RH'
		rcall 	sendbyte
	
					;wait for sensor ready 
	re3:	
		ldi 	zl,10
		rcall 	g_wms
		sbis 	PINB,Sens_data
		rjmp 	re2
		inc 	rhrt
		brne 	re3
	
		;räknare för hur många timeouts
		;re4:	rjmp 	reto			;sensor timed out (512 ms)

		;här går den vid timedout så:
		rjmp	out_readsh11	
	
	re2:	
		rcall 	readbyte		;read one byte
	
		sts	Start_of_temp_rh_data,data_RxTx ;save data
	
		cbi 	PORTB,Sens_data		;low output
		sbi 	DDRB,Sens_data		;output
		rcall	rdelay

		rcall 	chigh
		rcall 	clow

		sbi 	PORTB,Sens_data		;use pullup
		cbi 	DDRB,Sens_data		;input
		rcall	rdelay
		
		rcall 	readbyte		;readbyte
		
		sts	Start_of_temp_rh_data+1,data_RxTx ;save data
		
		rcall 	chigh			;skip acknowlegde to end communication, (no crc-byte fetch)
		rcall 	clow
	

		sbi 	PORTB,Sens_data		;high output
		sbi 	DDRB,Sens_data		;output

	

			;--------read temperature
		rcall 	chigh			;transmission start
		rcall	dlow
		rcall	clow
		rcall 	chigh
		rcall 	dhigh
		rcall 	clow
			
			
		ldi 	data_RxTx,0b00000011	;send byte: command for 'check temp'
		rcall 	sendbyte
	
					;wait for sensor ready 
	rew3:	
		ldi 	zl,10
		rcall 	g_wms
		sbis 	PINB,Sens_data
		rjmp 	rew2
		inc 	trt
		brne 	rew3

		;räknare för hur många timeouts	
		;rew4:	rjmp 	reto			;sensor timed out (512 ms)
	
		;här går den vid timedout så:
		rjmp	out_readsh11
	
	rew2:	
		rcall 	readbyte		;read one byte

		sts	Start_of_temp_rh_data+2,data_RxTx ;save data

		cbi 	PORTB,Sens_data		;low output
		sbi 	DDRB,Sens_data		;output
		rcall	rdelay

		rcall 	chigh
		rcall 	clow

		sbi 	PORTB,Sens_data		;use pullup
		cbi 	DDRB,Sens_data		;input
		rcall	rdelay
	
		rcall 	readbyte		;readbyte
		
		sts	Start_of_temp_rh_data+3,data_RxTx ;save data
	
		rcall 	chigh			;skip acknowlegde to end communication
		rcall 	clow

	out_readsh11:
		sbi 	PORTB,Sens_data		;high output
		sbi 	DDRB,Sens_data		;output

	ret
	
;-----------time out

;reto:	movw 	xl,rel								;£
;	adiw 	xl,1			;increase number of timeouts		;£
;	movw 	rel,xl								;£

;	ret
	


;------------	
	
readbyte:
		ldi 	counter,8
		
	ree1:	
		clc			;Read bit
		sbic 	PINB,Sens_data
		sec			
			
		rol 	data_RxTx
		
		rcall 	chigh
		rcall 	clow
		
		dec 	counter	
		brne 	ree1

	ret
	
;----------


sendbyte:
		ldi 	counter,8	;send 8 bits	
	se3:
		lsl 	data_RxTx
		brcc 	se1
		rcall 	dhigh
		rjmp 	se2
	se1:	
		rcall	 dlow
	se2:	
		rcall	 chigh
		rcall 	clow
		dec 	counter		
		brne 	se3
	
		;release dataline
		sbi 	PORTB,Sens_data		;use pullup
		cbi 	DDRB,Sens_data		;input
		rcall	rdelay

		clc					;check ack
		sbic 	PINB,Sens_data			;c=0: ack recieved
		sec					;c=1: ack not recieved

	
		in 	temp,sreg
		push 	temp
		
		rcall 	chigh		;flush ack bit
		rcall 	clow
		
		pop 	temp
		out 	sreg,temp
	
	
	ret


	
g_wms:	
		ldi 	timer,50	;wait zl (1/10)msec  (at 4MHz clock)
	g_wmsa:	
		rcall	 g_wmsb
		nop
		nop
		nop
		nop
		nop
		dec 	timer
		brne 	g_wmsa
		dec 	zl
		brne 	g_wms
	g_wmsb:	

	ret


dlow:	
		cbi 	PORTB,Sens_data		;set data low	
		rjmp 	rdelay
	
dhigh:	
		sbi 	PORTB,Sens_data		;set data high	
		rjmp 	rdelay
		

clow:	
		cbi 	PORTB,Sens_clock	;set clock low
		rjmp 	rdelay
	
chigh:	
		sbi 	PORTB,Sens_clock	;set clock high	


rdelay:	
		ldi 	zl,1
		rcall 	g_wms
	
	ret
			




