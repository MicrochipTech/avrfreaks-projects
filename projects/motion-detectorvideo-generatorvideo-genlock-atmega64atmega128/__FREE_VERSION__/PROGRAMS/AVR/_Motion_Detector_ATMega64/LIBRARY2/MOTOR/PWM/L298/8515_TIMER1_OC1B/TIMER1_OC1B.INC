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
;| TIMER 1 PWM B OUTPUT IN OC1B			 	 	|
;+--------------------------------------------------------------+
;| THIS DRIVE HAS INTEND TO USE WITH AVRS WITH SRAM		|
;+--------------------------------------------------------------+
;+------------------------------------------------------------------------------+
;| THIS DRIVE USE A L298 CHIP TO CONTROL A MOTOR USING A CHANNEL B PWM		|
;| THE FOLLOWING SCHEME MUST BE USED						|
;| 										|
;| _L298_FORWARD_BIT ---> INPUT1 OR INPUT3 OF L298 CHIP			|
;| _L298_REVERSE_BIT ---> INPUT2 OR INPUT4 OF L298 CHIP			|
;| OC1B		      ---> ENABLE_A OR ENABLE_B OR L298 CHIP			|							
;|										|
;| OBS:										|
;|    USE INPUT1,INPUT2,ENABLE_A WITH _L298_FORWARD_BIT,_L298_REVERSE_BIT,OC1B|
;|    OR INPUT3,INPUT4,ENABLE_B  WITH _L298_FORWARD_BIT,_L298_REVERSE_BIT,OC1B|
;|										|
;| THIS DRIVE USE A TIMER1 HARDWARE RESOURCE					|
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
;|			RCALL	_L298_INIT	;AFTER THIS POINT ALL MOTOR ROUTINES OK	 	|
;+----------------------------------------------------------------------------------------------+
;|												|
;| _L298_SET_POWER	SET POWER TO THE MOTOR 0..255						|
;|												|
;|			ldi	Acc,128			;set power to half			|
;|			RCALL	_L298_SET_POWER	;call routines				|
;+----------------------------------------------------------------------------------------------+
;|												|
;| _L298_DISABLE	DISABLE MOTOR POWER FROM L298 CHIP					|
;|												|
;|			RCALL	_L298_DISABLE		;AFTER THIS MOTOR FREE WITHOUT POWER	|
;+----------------------------------------------------------------------------------------------+
;|												|
;| _L298_SET_DIRECTION	SET DIRECTION OF THE MOTOR TO FORWARD/REVERSE				|
;|												|
;|			LDI	ACC,_L298_FORWARD	;SET DIRECTION FORWARD			|
;|			RCALL	_L298_SET_DIRECTION	;AFTER THIS MOTOR AT FORWARD DIRECTION	|
;+----------------------------------------------------------------------------------------------+
.EQU	_L298_REVERSE		= 0X01
.EQU	_L298_FORWARD		= 0X02
;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  
;+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
;|  MOTOR INITIALIZE		 		 |
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
	ldi	Acc,(1<<COM1B1) | (1<<PWM10)		;enable normal NON INVERTED PWM OUTPUT & 8 bits
	out	TCCR1A,Acc
	ldi	Acc,_TIMER_DIV_1			;timer 1 prescaler factor 1
	out	TCCR1B,Acc
	clr	Acc
	rcall	_L298_SET_POWER
	pop	Acc
	out	SREG,Acc
	pop	Acc
	ret
;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  
;+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
;|  MOTOR SET POWER		 		 |
;|\			          		 |
;| | Input:   Acc motor Power 0..255		 |
;|/  Output:  None	          		 |
;|   Destroy: None     		  		 |
;+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
;   \__\__\__\__\__\__\__\__\__\__\__\__\__\__\____________________________________________________________________________
_L298_SET_POWER:
	push	Acc
	clr	Acc				;because a 8 bits interface
	out	OCR1BH,Acc
	pop	Acc
	out	OCR1BL,Acc
	ret
;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  
;+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
;|  MOTOR DISABLE		 		 |
;|\			          		 |
;| | Input:   None				 |
;|/  Output:  None	          		 |
;|   Destroy: None     		  		 |
;+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
;   \__\__\__\__\__\__\__\__\__\__\__\__\__\__\____________________________________________________________________________
_L298_DISABLE:
	push	Acc
	in	Acc,_L298_PORT_OUTPUT
	andi	Acc,~((1<<_L298_BIT_FORWARD) | (1<<_L298_BIT_REVERSE))
	out	_L298_PORT_OUTPUT,Acc
	pop	Acc
	ret	
;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  
;+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
;|  MOTOR SET DIRECTION		 		 |
;|\			          		 |
;| | Input:   Acc direction FORWARD/REVERSE	 |
;|/  Output:  None	          		 |
;|   Destroy: None     		  		 |
;+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
;   \__\__\__\__\__\__\__\__\__\__\__\__\__\__\____________________________________________________________________________
_L298_SET_DIRECTION:
	rcall	_L298_DISABLE
	cpi	Acc,_L298_FORWARD
	brne	_L298_SET_D00
	sbi	_L298_PORT_OUTPUT,_L298_BIT_FORWARD
	ret
_L298_SET_D00:
	cpi	Acc,_L298_REVERSE
	brne	_L298_SET_D10
	sbi	_L298_PORT_OUTPUT,_L298_BIT_REVERSE
_L298_SET_D10:
	ret

