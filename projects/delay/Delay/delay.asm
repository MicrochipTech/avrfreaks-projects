;***** Delay ****************************************************
;* Written by Anders Mårtensson 2001				*
;* anders.martensson@teracom.se					*
;* Use the code in any way you want.				*
;*								*
;* Used to get small delay (0,025-6,4s).			*
;*								*
;* This project uses a 8-bit counter and is not very exact.	*
;* If you are doing a clock you would have to change the code	*
;* so that you would get a smaller error. The best way of doing *
;* this is by counting with a 16-bit counter.			*
;*								*
;* You have to do some math to get going.			*
;* My application uses a clock at 8.000Mhz.			*
;* That means that every cycle is (T=1/f) 1/8000000=125nSec.	*
;* I use a prescaler at 1024 witch devides the clock by 1024	*
;* (8000000/1024). So the length of the devided clock is 	*
;* (T=1/f) 1/(8000000/1024)=1024/8000000=128uSec.		*
;* This means that if you would like to get 0.025 sec delay you	*
;* would have to count the devided clock to: 			*
;* 0.025Sec/128uSec=195,3.					*
;* 								*
;* So, if you use a 8.0Mhz clock and a prescaler at 1024 	*
;* you would have to count to 195 to get 0,025sec second delay.	*
;* Note that you will get a small error as 195,3 is 		*
;* rounded to 195.						*
;* As the counter count from 0 to 255 i restart the counter at	*
;* 256-195=61=0x3D						*
;* 								*
;* The error is 0,3*1024*125nSec=40uSec. 			*
;* This will give you an error of 5,76Sec 			*
;* in one hour (3600*40uSec*40=5,76Sec).			*
;****************************************************************

.include "8515def.inc"

.def	Temp_R		=r16

.def	DelayTime_R	=r17
.equ	StartoverTime_K	=0x3D

.def	InOut_R		=r18


	rjmp	RESET
	reti			;IRQ0 Handler
	reti			;IRQ1 Handler
	reti			;Timer1 Capture Handler
	reti			;Timer1 CompareA Handler
	reti			;Timer1 CompareB Handler
	reti			;Timer1 Overflow Handler
	rjmp	INT_TIM0_OF	;Timer0 Overflow Handler
	reti			;SPI Transfer Ccomplete Handler
	reti			;UART RX Complete Handler
	reti			;UDR Empty handler Handler
	reti			;UART TX Complete Handler
	reti			;Analog Comparator Handler

;***** Interupt; Timer/Counter0 overflow *****
INT_TIM0_OF:
	ldi	Temp_R,StartoverTime_K
	out	TCNT0,Temp_R		;Starta om räknaren
	cpi	DelayTime_R,0x00
	breq	DELAY_DONE
	dec	DelayTime_R		;Minska antalet loopar som är kvar
DELAY_DONE:
	reti
	
;***** Delay: DelayTime_R * 0,025s (0,02496s) ************
;* Interupt enable and disable may have to be removed if *
;* you are using other interupts in youre program        *
;*********************************************************
TIME_DELAY:
	sei				;Interupt enabled.
	ldi	Temp_R,StartoverTime_K
	out	TCNT0,Temp_R
DELAY_LOOP:
	cpi	DelayTime_R,0x00	;Loop until delay is done
	brne	DELAY_LOOP
	cli				;Interupt disabled
	ret

RESET:
	;***** Init stackpointer *****
	ldi	Temp_R,high(RAMEND)
	out	SPH,Temp_R
	ldi	Temp_R,low(RAMEND)
	out	SPL,Temp_R

	;***** Init ports, A = out *****
	ser	Temp_R
	out	DDRA,Temp_R

	
	;***** Init Timer0 *****
	ldi	Temp_R,0b00000010	;overflow int enabled
	out	TIMSK,Temp_R
	out	TIFR,Temp_R
	ldi	Temp_R,0b00000101	;/1024
	out	TCCR0,Temp_R
	
	;***** Start flashing LED lsb on PORTA *****
	ldi	InOut_R,0x01
LOOP:
	ldi	Temp_R,0x01
	eor	InOut_R,Temp_R
	out	PORTA,InOut_R
	
	ldi	DelayTime_R,0x0A	;0x0A = 10 => 10*0,025s=0,25s
	rcall	TIME_DELAY		;Delay
	
	rjmp	LOOP			;Loop forever
	
