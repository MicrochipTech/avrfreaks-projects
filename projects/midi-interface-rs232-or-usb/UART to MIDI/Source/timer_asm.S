/* 

 Timer-Interrupt routines to reduce overhead in interrupt,
 for use with timer.c/timer.h/timer_asm.h
 
*/

 #include <avr/io.h>
 #include "hardware.h"
 
 #define	temp 	R26
 #define 	temp1	R27
 #define 	temp2	R30

// globals...
//extern unsigned long    	timer_value;
 .comm	timer_value, 4

 
// Interrupt on timer0-overflow, increase 32-bit software timer
.global		SIG_OVERFLOW0
SIG_OVERFLOW0:
	; save registers
	push	R2
	in		R2, _SFR_IO_ADDR(SREG)
	push	temp

	; increment Software-Timer
	lds		temp, (timer_value + 0)	; 0. Byte
	inc		temp
	sts		(timer_value + 0), temp
	brne	SO_RAUS
	
	lds		temp, (timer_value + 1)	; 1. Byte
	inc		temp
	sts		(timer_value + 1), temp
	brne	SO_RAUS
	
	lds		temp, (timer_value + 2)	; 2. Byte
	inc		temp
	sts		(timer_value + 2), temp
	brne	SO_RAUS

	lds		temp, (timer_value + 3)	; 3. Byte
	inc		temp
	sts		(timer_value + 3), temp
	brne	SO_RAUS
	
SO_RAUS:	

	; restore registers
	pop		temp 
	out		_SFR_IO_ADDR(SREG), R2
	pop		R2
	
	reti
	
	
// returns actual timer value atomar
//
// unsigned long timer_getvalue( void )
// uses: temp (and indirect R22-R25)
.global		timer_getvalue
timer_getvalue:
	in		temp, _SFR_IO_ADDR(SREG)
	cli
	
	lds		R22, (timer_value + 0)
	lds		R23, (timer_value + 1)
	lds		R24, (timer_value + 2)
	lds		R25, (timer_value + 3)
	
	out		_SFR_IO_ADDR(SREG), temp
	ret
	
// unsigned long timer_elapsed_time( unsigned long first_timepoint, unsigned long second_timepoint)	
.global		timer_elapsed_time	
	
// EOF

