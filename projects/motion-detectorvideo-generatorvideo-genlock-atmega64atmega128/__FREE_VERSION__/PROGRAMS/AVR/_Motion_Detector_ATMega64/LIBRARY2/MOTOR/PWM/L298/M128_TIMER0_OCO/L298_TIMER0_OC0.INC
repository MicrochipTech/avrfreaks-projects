;<$DS_HEADER>
;+--------------------------------------------------------------------------------+              
;|João D´Artagnan A. Oliveira Programmer and Author; 				  |
;|										  |
;| All rights reserved.								  |
;|										  |
;|   Redistribution and use in source and binary forms, with or without		  |
;|  modification, are permitted provided that the following conditions are met:	  |
;|										  |
;|   * Redistributions of source code must retain the above copyright		  |
;|     notice, this list of conditions and the following disclaimer.		  |
;|										  |
;|   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"  |
;|   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE    |
;|   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE   |
;|   ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE     |
;|   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR	  |
;|   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF	  |
;|   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS	  |
;|   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN	  |
;|   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)	  |
;|   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE   |
;|   POSSIBILITY OF SUCH DAMAGE.						  |
;|										  |
;|   Brasília, Brazil November 30,2015 	06:28 am				  |
;+--------------------------------------------------------------------------------+
;</$DS_HEADER>
;+--------------------------------------------------------------+---+
;|								|
;| DATA:23/2/2004						|
;>--------------------------------------------------------------<
;| M O T O R  P W M  C O N T R O L  U S I N G  			|
;| TIMER 0 PWM OUTPUT IN OC0			 	 	|
;+--------------------------------------------------------------+
;| THIS DRIVE HAS INTEND TO USE ATMEGA128			|
;+--------------------------------------------------------------+
;+------------------------------------------------------------------------------+
;| THIS DRIVE USE A L298 CHIP TO CONTROL A MOTOR USING A CHANNEL 0 PWM		|
;| THE FOLLOWING SCHEME MUST BE USED						|
;| 										|
;| _L298_FORWARD_BIT ---> INPUT1 OR INPUT3 OF L298 CHIP				|
;| _L298_REVERSE_BIT ---> INPUT2 OR INPUT4 OF L298 CHIP				|
;| OC0		      ---> ENABLE_A OR ENABLE_B OR L298 CHIP			|							
;|										|
;| OBS:										|
;|    USE INPUT1,INPUT2,ENABLE_A WITH _L298_FORWARD_BIT,_L298_REVERSE_BIT,OC0 	|
;|    OR INPUT3,INPUT4,ENABLE_B  WITH _L298_FORWARD_BIT,_L298_REVERSE_BIT,OC0 	|
;|										|
;| THIS DRIVE USE A TIMER0 HARDWARE RESOURCE					|
;+------------------------------------------------------------------------------+
;>------------------------------------------------------<
;| I M P L E M E N T E D  F U N C T I O N S 		|
;+------------------------------------------------------+---------------------------------------+
;| _L298_INIT		INITIALIZE L298 CHIP							|
;|												|
;|.EQU	_L298_PORT_OUTPUT	= PORTA								|
;|.EQU	_L298_PORT_DIR		= DDRA								|
;|.EQU	_L298_PORT_INPUT	= PINA								|
;|												|
;|.EQU	_L298_BIT_FORWARD	= 7								|
;|.EQU	_L298_BIT_REVERSE	= 6								|
;|												|
;|			RCALL	_L298_INIT	;AFTER THIS POINT ALL L298 ROUTINES OK	 	|
;+----------------------------------------------------------------------------------------------+
;|												|
;| _L298_SET_POWER	SET POWER TO THE L298 0..255						|
;|												|
;|			ldi	Acc,128			;set power to half			|
;|			RCALL	_L298_SET_POWER		;call routine				|
;+----------------------------------------------------------------------------------------------+
;|												|
;| _L298_DISABLE	DISABLE MOTOR POWER FROM L298 CHIP					|
;|												|
;|			RCALL	_L298_DISABLE		;AFTER THIS MOTOR FREE WITHOUT POWER	|
;+----------------------------------------------------------------------------------------------+
;|												|
;| _L298_SET_DIRECTION	SET DIRECTION OF THE MOTOR TO FORWARD/REVERSE/NONE			|
;|												|
;|			LDI	ACC,_L298_FORWARD	;SET DIRECTION FORWARD			|
;|			RCALL	_L298_SET_DIRECTION	;AFTER THIS MOTOR AT FORWARD DIRECTION	|
;| if direction if none motor will stop								|
;+----------------------------------------------------------------------------------------------+
;|												|
;| _L298_GET_DIRECTION	GET DIRECTION OF THE MOTOR						|
;|												|
;|			RCALL	_L298_GET_DIRECTION	;Acc direction				|
;+----------------------------------------------------------------------------------------------+
#ifndef __M128_TIMER0_OC0__
	#define __M128_TIMER0_OC0__
	.message "[ (MOTOR) M128 TIMER0_OC0 ]"
	.EQU	_L298_REVERSE		= 0X01
	.EQU	_L298_FORWARD		= 0X02
	.EQU	_L298_DISABLE		= 0X03
	.DSEG
		_L298_DIRECTION:		.BYTE 1			;set direction state
	.CSEG
	;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  
	;+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
	;|  L298 INITIALIZE		 		 |
	;|\			          		 |
	;| | Input:   None				 |
	;|/  Output:  None	          		 |
	;|   Destroy: None     		  		 |
	;+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
	;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
	;   \__\__\__\__\__\__\__\__\__\__\__\__\__\__\____________________________________________________________________________
	_L298_INIT:
		push	Acc
		in	Acc,SREG
		cli
		cbi	_L298_PORT_OUTPUT,_L298_BIT_FORWARD	;set motor control as output and disable
		cbi	_L298_PORT_OUTPUT,_L298_BIT_REVERSE
		sbi	_L298_PORT_DIR,_L298_BIT_FORWARD
		sbi	_L298_PORT_DIR,_L298_BIT_REVERSE
		push	Acc
		ldi	Acc,(0<<WGM01)|(1<<WGM00)|(1<<COM01)|(0<<COM00)|(_TIMERM0_DIV_1) ;set pwm phase correct
		out	TCCR0,Acc
		sbi	DDRB,4					;portb4=OC0=output
		clr	Acc
		out	OCR0,Acc				;no power
		ldi	Acc,_L298_DISABLE			;set stopeed
		sts	_L298_DIRECTION,Acc
		pop	Acc
		out	SREG,Acc
		pop	Acc
		ret
	
	;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  
	;+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
	;|  L298 SET POWER		 		 |
	;|\			          		 |
	;| | Input:   Acc motor Power 0..255		 |
	;|/  Output:  None	          		 |
	;|   Destroy: None     		  		 |
	;+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
	;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
	;   \__\__\__\__\__\__\__\__\__\__\__\__\__\__\____________________________________________________________________________
	_L298_SET_POWER:
		out	OCR0,Acc				;set power
		ret
	;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  
	;+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
	;|  L298 GET POWER		 		 |
	;|\			          		 |
	;| | Input:   none				 |
	;|/  Output:  Acc motor Power 0..255  		 |
	;|   Destroy: None     		  		 |
	;+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
	;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
	;   \__\__\__\__\__\__\__\__\__\__\__\__\__\__\____________________________________________________________________________
	_L298_GET_POWER:
		in	Acc,OCR0
		ret
	;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  
	;+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
	;|  L298 DISABLE		 		 |
	;|\			          		 |
	;| | Input:   None				 |
	;|/  Output:  None	          		 |
	;|   Destroy: None     		  		 |
	;+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
	;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
	;   \__\__\__\__\__\__\__\__\__\__\__\__\__\__\____________________________________________________________________________
	_L298_SET_DISABLE:
		push	Acc
		in	Acc,_L298_PORT_OUTPUT
		andi	Acc,~((1<<_L298_BIT_FORWARD) | (1<<_L298_BIT_REVERSE))
		out	_L298_PORT_OUTPUT,Acc
		ldi	Acc,_L298_DISABLE
		sts	_L298_DIRECTION,Acc
		pop	Acc
		ret	
	;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  
	;+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
	;|  L298 SET DIRECTION		 		 |
	;|\			          		 |
	;| | Input:   Acc direction FORWARD/REVERSE/NONE |
	;|/  Output:  None	          		 |
	;|   Destroy: None     		  		 |
	;+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
	;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
	;   \__\__\__\__\__\__\__\__\__\__\__\__\__\__\____________________________________________________________________________
	_L298_SET_DIRECTION:
		push	Acc
		rcall	_L298_SET_DISABLE				;disable before change
		cpi	Acc,_L298_FORWARD				;foward ?
		brne	_L298_SET_D00
		sbi	_L298_PORT_OUTPUT,_L298_BIT_FORWARD
		ldi	Acc,_L298_FORWARD
		sts	_L298_DIRECTION,Acc
		pop	Acc
		ret
	_L298_SET_D00:
		cpi	Acc,_L298_REVERSE				;reverse ?
		brne	_L298_SET_D10
		sbi	_L298_PORT_OUTPUT,_L298_BIT_REVERSE
		ldi	Acc,_L298_REVERSE
		sts	_L298_DIRECTION,Acc
		pop	Acc
		ret
	_L298_SET_D10:							;else disable
		pop	Acc
		ret
	;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  
	;+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
	;|  L298 GET DIRECTION		 		 |
	;|\			          		 |
	;| | Input:   None				 |
	;|/  Output:  Acc direction			 |
	;|   Destroy: None     		  		 |
	;+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
	;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
	;   \__\__\__\__\__\__\__\__\__\__\__\__\__\__\____________________________________________________________________________
	_L298_GET_DIRECTION:
		lds	Acc,_L298_DIRECTION
		ret
#endif
.exit

