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
;+--------------------------------------------------------------+
;|								|
;| DATA:16/11/2000						|
;>--------------------------------------------------------------<
;| A D C 8 3 2  R O U T I N E S					|
;+--------------------------------------------------------------+
;| THIS DRIVE HAS INTEND TO USE WITH AVRS WITH SRAM		|
;+--------------------------------------------------------------+
;>------------------------------------------------------<
;| I M P L E M E N T E D  F U N C T I O N S 		|
;+------------------------------------------------------+---------------------------------------+
;| _ADC832_INIT		INITIALIZE ADC832 CHIP							|
;|			INITIALIZATION EXAMPLE							|
;|												|
;|.EQU	_ADC832_BIT_CLK	= 0			;ADC832 CLOCK BIT				|
;|.EQU	_ADC832_BIT_CS	= 1			;ADC832 CHIP SELECT BIT				|
;|.EQU	_ADC832_BIT_DATA_OUT= 2			;ADC832 DATA BIT OUTPUT				|
;|.EQU	_ADC832_BIT_DATA_IN = 3			;ADC832 DATA BIT INPUT				|
;|												|
;|.EQU	_ADC832_PORT_OUT= PORTB			;ADC832 OUTPUT PORT				|
;|.EQU	_ADC832_PORT_IN	= PINB			;ADC832 INPUT PORT				|
;|.EQU	_ADC832_PORT_DIR= DDRB			;ADC832 DIRECTION PORT				|
;|												|
;|			RCALL	_ADC832_INIT	;AFTER THIS POINT ALL ADC832 ROUTINES WORKING 	|
;|						;AT PORTB					|
;|----------------------------------------------------------------------------------------------|
;|												|
;| _ADC832_GET		TO GET VALUE FROM ADC832_CHIP CHANNEL 1					|
;|												|
;|			LDI	ACC,1		;SET CHANNEL TO BE READ				|
;|			RCALL	_ADC832_GET	;AFTER THIS POINT ACC HAVE A 8 BIT VALUE	|
;+----------------------------------------------------------------------------------------------+
;+----------------------------------------------------------------------------------------------+
;| HARDWARE DEFINITIONS	  									|
;|												|
;| BEFORE USE THIS DRIVE SET FOLLOWING PARAMETERS						|
;|												|
;| _ADC832_BIT_CLK		TO DEFINE BIT NUMBER INTO ADC FOR EACH SIGNAL CLK,CS,DATA	|
;| _ADC832_BIT_CS										|
;| _ADC832_BIT_DATA_OUT										|
;| _ADC832_BIT_DATA_IN										|
;|												|
;| _ADC832_PORT_OUT		TO DEFINE AVR PORT THAT HAVE BITS TO CONTROL ADC INTERFACE	|
;| _ADC832_PORT_IN										|
;| _ADC832_PORT_DIR										|
;|												|
;| EXAMPLE:											|
;|												|
;|.EQU _ADC832_BIT_CLK	=0	TO DEFINE BIT NUMBER INTO ADC FOR EACH SIGNAL CLK,CS,DATA	|
;|.EQU _ADC832_BIT_CS	=1									|
;|.EQU _ADC832_BIT_DATA_OUT=2									|
;|.EQU _ADC832_BIT_DATA_OUT=3									|
;|												|
;|.EQU _ADC832_PORT_OUT	=PORTD  TO DEFINE AVR PORT THAT HAVE BITS TO CONTROL ADC INTERFACE	|
;|.EQU _ADC832_PORT_IN 	=PIND									|
;|.EQU _ADC832_PORT_DIR	=DDRD									|
;|												|
;+----------------------------------------------------------------------------------------------+
#ifndef __ADCS_ADC832__
	#define __ADCS_ADC832__
	.message "+-------------+"
	.message "|             |"
	.message "| ADCS ADC832 |"
	.message "|             |"
	.message "+-------------+"
	;   |  |  |  |  |  |  |  |  |  |
	;+--+--+--+--+--+--+--+--+--+--+--+
	;|   ADC832 INITIALIZE		  |
	;|  				  |
	;|\			          |
	;| | Input  : None	          |
	;|/  Output : None	          |
	;|   Destroy: None	          |
	;|   			       	  |
	;+--+--+--+--+--+--+--+--+--+--+--+ 
	;   |  |  |  |  |  |  |  |  |  |
	;   \__\__\__\__\__\__\__\__\__\________________________________________________________________________
	;				/
	;				| Obs:  After this initialization CLK,CS,DATA_OUT,DATA_INT are in high level
	;				|	CLK,CS,DATA_OUT  = Output
	;				|	DATA_IN	= Input
	;				\_______________________________________________________________________
	_ADC832_INIT:
		push	Acc
		in	Acc,SREG					;Save SREG state
		cli							;to handdling ports correctly
		sbi	_ADC832_PORT_OUT,_ADC832_BIT_CLK		;CLK  high
		sbi	_ADC832_PORT_OUT,_ADC832_BIT_CS			;CS   high
		sbi	_ADC832_PORT_OUT,_ADC832_BIT_DATA_OUT		;DATA in with pull up 
		sbi	_ADC832_PORT_OUT,_ADC832_BIT_DATA_IN		;DATA out
		sbi	_ADC832_PORT_DIR,_ADC832_BIT_CLK		;CLK  Output
		sbi	_ADC832_PORT_DIR,_ADC832_BIT_CS			;CS   Output
		cbi	_ADC832_PORT_DIR,_ADC832_BIT_DATA_OUT		;DATA_OUT at ADC832 chip
		sbi	_ADC832_PORT_DIR,_ADC832_BIT_DATA_IN		;DATA_IN at ADC832 chip
		out	SREG,Acc					;Restore SREG state
		pop	Acc
		ret
	;   |  |  |  |  |  |  |  |  |  |
	;+--+--+--+--+--+--+--+--+--+--+--+
	;|   ADC832 GET VALUE		  |
	;|  				  |
	;|\			          |
	;| | Input  : Acc Channel Number  |
	;|/  Output : Acc Value	          |
	;|   Destroy: None	          |
	;|   			       	  |
	;+--+--+--+--+--+--+--+--+--+--+--+ 
	;   |  |  |  |  |  |  |  |  |  |
	;   \__\__\__\__\__\__\__\__\__\________________________________________________________________________
	;				/
	;				| Timing
	;				|
	; 				|      1   2   3   4   5   6   7   8   9  10
	; 				|     +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+
	; 				| CLK_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |
	; 				|
	; 				| __+                                                	      +------
	; 				| CS|_________________________________________________________|
	; 				|
	; 				|     SB DIF  ODD
	; 				| DI ---+   +---+---------------------------------------------+______Hi Z
	; 				|       |___|___|_____________________________________________|
	; 				|
	; 				|                     7   6   5   4   3   2   1   0
	; 				| D0 -----------+   +---+---+---+---+---+---+---+-------------+______Hi Z
	; 				|       	|___|___|___|___|___|___|___|___|_____________|
	; 				|
	;				| Average conversion time     	Frequency
	;				| -----------------------	----------
	;				|        166.0uS		   1Mhz
	;				|         41.5uS		   4Mhz
	;				|         27.7uS		   6Mhz
	;				|         20.7uS		   8Mhz
	;				|         23.8uS		  10Mhz
	;				|         21.7uS		14.3Mhz
	;				|         23.8uS		  16Mhz
	;				\_______________________________________________________________________
	.MACRO	_M_ADC832_DELAY
		.if _AVR_CLOCK>=6000000 
			ldi	AccT,(_AVR_CLOCK/3000000)-1
			dec	AccT
			brne	PC-1
		.else
			ldi	AccT,1
			dec	AccT
			brne	PC-1
		.endif
	.ENDMACRO
	.MACRO	_M_ADC832_GET_BIT
		cbi	_ADC832_PORT_OUT,_ADC832_BIT_CLK	;clock low half pulse
		_M_ADC832_DELAY
		sbic	_ADC832_PORT_IN,_ADC832_BIT_DATA_OUT	;read data out from AD
		ori	AccH,@0
		sbi	_ADC832_PORT_OUT,_ADC832_BIT_CLK	;clock high last half clock			
		_M_ADC832_DELAY	
	.ENDMACRO
	_ADC832_GET:
		push	AccH
		push	AccT
		sbi	_ADC832_PORT_OUT,_ADC832_BIT_DATA_IN	;start bit high
		cbi	_ADC832_PORT_OUT,_ADC832_BIT_CLK	;turn clock low
		cbi	_ADC832_PORT_OUT,_ADC832_BIT_CS		;turn CS low
		_M_ADC832_DELAY					;wait half clock
		sbi	_ADC832_PORT_OUT,_ADC832_BIT_CLK	;turn clock high
		_M_ADC832_DELAY
		cbi	_ADC832_PORT_OUT,_ADC832_BIT_CLK	;turn clock low 2nd pulse
		_M_ADC832_DELAY	
		sbi	_ADC832_PORT_OUT,_ADC832_BIT_CLK	;turn clock high
		_M_ADC832_DELAY		
		sbrc	Acc,0					;skip if channel 0 select 
		rjmp	_AD82_GET00				;else, channel 1 and jump
		cbi	_ADC832_PORT_OUT,_ADC832_BIT_DATA_IN	;set channel 0 as input
		rjmp	_AD82_GET10
	_AD82_GET00:
		sbi	_ADC832_PORT_OUT,_ADC832_BIT_DATA_IN	;else channe 1 as input
	_AD82_GET10:
		cbi	_ADC832_PORT_OUT,_ADC832_BIT_CLK	;channel pulse clock
		_M_ADC832_DELAY
		sbi	_ADC832_PORT_OUT,_ADC832_BIT_CLK
		_M_ADC832_DELAY
		cbi	_ADC832_PORT_OUT,_ADC832_BIT_CLK	;dummy pulse clock
		_M_ADC832_DELAY
		sbi	_ADC832_PORT_OUT,_ADC832_BIT_CLK
		_M_ADC832_DELAY
		clr	AccH					;dummy data clear
		ldi	Acc,8
	_AD82_GET20:
		_M_ADC832_GET_BIT 0x80				;get bit 7
		_M_ADC832_GET_BIT 0x40				;get bit 6
		_M_ADC832_GET_BIT 0x20				;get bit 5
		_M_ADC832_GET_BIT 0x10				;get bit 4
		_M_ADC832_GET_BIT 0x08				;get bit 3
		_M_ADC832_GET_BIT 0x04				;get bit 2
		_M_ADC832_GET_BIT 0x02				;get bit 1
		_M_ADC832_GET_BIT 0x01				;get bit 0
;		ldi	Acc,9					;dummy read last bits
;	_AD82_GET30:
;		cbi	_ADC832_PORT_OUT,_ADC832_BIT_CLK	;<--- DELETED BECAUSE ONLY A CHANNEL A READ AT TIME
;		_M_ADC832_DELAY
;		sbi	_ADC832_PORT_OUT,_ADC832_BIT_CLK
;		_M_ADC832_DELAY
;		dec	Acc
;		brne	_AD82_GET30
		sbi	_ADC832_PORT_OUT,_ADC832_BIT_CS		;turn CS high end of conversion
		mov	Acc,AccH
		pop	AccT
		pop	AccH
		ret
#endif
.exit

