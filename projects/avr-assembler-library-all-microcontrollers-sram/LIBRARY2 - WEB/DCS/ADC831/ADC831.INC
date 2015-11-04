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
;| DATA:16/11/2000						|
;>--------------------------------------------------------------<
;| A D C 8 3 1  R O U T I N E S					|
;+--------------------------------------------------------------+
;| THIS DRIVE HAS INTEND TO USE WITH AVRS WITH SRAM		|
;+--------------------------------------------------------------+
;>------------------------------------------------------<
;| I M P L E M E N T E D  F U N C T I O N S 		|
;+------------------------------------------------------+---------------------------------------+
;| _ADC831_INIT		INITIALIZE ADC831 CHIP							|
;|			INITIALIZATION EXAMPLE							|
;|												|
;|.EQU	_ADC831_BIT_CLK	= 0			;ADC831 CLOCK BIT				|
;|.EQU	_ADC831_BIT_CS	= 1			;ADC831 CHIP SELECT BIT				|
;|.EQU	_ADC831_BIT_DATA= 2			;ADC831 DATA BIT				|
;|												|
;|.EQU	_ADC831_PORT_OUT= PORTB			;ADC831 OUTPUT PORT				|
;|.EQU	_ADC831_PORT_IN	= PINB			;ADC831 INPUT PORT				|
;|.EQU	_ADC831_PORT_DIR= DDRB			;ADC831 DIRECTION PORT				|
;|												|
;|			RCALL	_ADC831_INIT	;AFTER THIS POINT ALL ADC831 ROUTINES WORKING 	|
;|						;AT PORTB					|
;|----------------------------------------------------------------------------------------------|
;|												|
;| _ADC831_GET		TO GET VALUE FROM ADC831_CHIP 						|
;|												|
;|			RCALL	_ADC831_GET	;AFTER THIS POINT ACC HAVE A 8 BIT VALUE	|
;+----------------------------------------------------------------------------------------------+
;+----------------------------------------------------------------------------------------------+
;| HARDWARE DEFINITIONS	  									|
;|												|
;| BEFORE USE THIS DRIVE SET FOLLOWING PARAMETERS						|
;|												|
;|.EQU _ADC831_BIT_CLK	=0	TO DEFINE BIT NUMBER INTO ADC FOR EACH SIGNAL CLK,CS,DATA	|
;|.EQU _ADC831_BIT_CS	=1									|
;|.EQU _ADC831_BIT_DATA	=2									|
;|												|
;|.EQU _ADC831_PORT_OUT	=PORTD  TO DEFINE AVR PORT THAT HAVE BITS TO CONTROL ADC INTERFACE	|
;|.EQU _ADC831_PORT_IN 	=PIND									|
;|.EQU _ADC831_PORT_DIR	=DDRD									|
;|												|
;+----------------------------------------------------------------------------------------------+
#ifndef __ADCS_ADC831__
	#define __ADCS_ADC831__
	.message "+-------------+"
	.message "|             |"
	.message "| ADCS ADC831 |"
	.message "|             |"
	.message "+-------------+"
	;   |  |  |  |  |  |  |  |  |  |
	;+--+--+--+--+--+--+--+--+--+--+--+
	;|   ADC831 INITIALIZE		  |
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
	;				| Obs:  After this initialization CLK,CS,DATA are in high level
	;				|	CLK,CS  = Output
	;				|	DATA	= Input
	;				\_______________________________________________________________________
	_ADC831_INIT:
		push	Acc
		in	Acc,SREG					;Save SREG state
		cli							;to handdling ports correctly
		sbi	_ADC831_PORT_OUT,_ADC831_BIT_CLK		;CLK  high
		sbi	_ADC831_PORT_OUT,_ADC831_BIT_CS		;CS   high
		sbi	_ADC831_PORT_OUT,_ADC831_BIT_DATA		;DATA high with pull up
		sbi	_ADC831_PORT_DIR,_ADC831_BIT_CLK		;CLK  Output
		sbi	_ADC831_PORT_DIR,_ADC831_BIT_CS		;CS   Output
		cbi	_ADC831_PORT_DIR,_ADC831_BIT_DATA		;DATA Input
		out	SREG,Acc					;Restore SREG state
		pop	Acc
		ret
	;   |  |  |  |  |  |  |  |  |  |
	;+--+--+--+--+--+--+--+--+--+--+--+
	;|   ADC831 GET VALUE		  |
	;|  				  |
	;|\			          |
	;| | Input  : None	          |
	;|/  Output : Acc Value	          |
	;|   Destroy: None	          |
	;|   			       	  |
	;+--+--+--+--+--+--+--+--+--+--+--+ 
	;   |  |  |  |  |  |  |  |  |  |
	;   \__\__\__\__\__\__\__\__\__\________________________________________________________________________
	;				/
	;				| Timing
	;				|
	;				|       1   2   3   4   5   6   7   8   9  10                        
	;				|      +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+       
	;				|  CLK | |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |       
	;				|                                                                    
	;				|  __ +                                                +------       
	;				|  CS |________________________________________________|             
	;				|                                                                    
	;				|                                                                    
	;				|  D0 ---+   +---+---+---+---+---+---+---+-------------+______Hi Z   
	;				|        |___|___|___|___|___|___|___|___|_____________|             
	;				|                                                                    
	;				|              7   6   5   4   3   2   1   0    
	;				|
	;				| Average conversion time     	Frequency
	;				| -----------------------	----------
	;				|        151.0uS		   1Mhz
	;				|         37.6uS		   4Mhz
	;				|         25.2uS		   6Mhz
	;				|         18.9uS		   8Mhz
	;				|         22.0uS		  10Mhz
	;				|         20.1uS		14.3Mhz
	;				|         22.3uS		  16Mhz
	;				\_______________________________________________________________________
	.MACRO	_M_ADC831_DELAY
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
	.MACRO	_M_ADC831_GET_BIT
		cbi	_ADC831_PORT_OUT,_ADC831_BIT_CLK		;clock low half pulse
		_M_ADC831_DELAY
		sbic	_ADC831_PORT_IN,_ADC831_BIT_DATA		;read data out from AD
		ori	AccH,@0
		sbi	_ADC831_PORT_OUT,_ADC831_BIT_CLK		;clock high last half clock			
		_M_ADC831_DELAY	
	.ENDMACRO
	_ADC831_GET:
		push	AccH						;save registers
		push	AccT
		cbi	_ADC831_PORT_OUT,_ADC831_BIT_CLK		;cs low
		_M_ADC831_DELAY
		cbi	_ADC831_PORT_OUT,_ADC831_BIT_CS			;cs low
		_M_ADC831_DELAY
   		sbi	_ADC831_PORT_OUT,_ADC831_BIT_CLK		;clk high
		_M_ADC831_DELAY
		cbi	_ADC831_PORT_OUT,_ADC831_BIT_CLK		;clk low 1 1/2 start clock
		_M_ADC831_DELAY
		sbi	_ADC831_PORT_OUT,_ADC831_BIT_CLK		;clk high
		_M_ADC831_DELAY
		cbi	_ADC831_PORT_OUT,_ADC831_BIT_CLK		;clk low
		_M_ADC831_DELAY
		ldi	Acc,8						;bit counter
		clr	AccH						;data register
	_AD31_GET00:
		_M_ADC831_GET_BIT 0x80					;get bit 7
		_M_ADC831_GET_BIT 0x40					;get bit 6
		_M_ADC831_GET_BIT 0x20					;get bit 5
		_M_ADC831_GET_BIT 0x10					;get bit 4
		_M_ADC831_GET_BIT 0x08					;get bit 3
		_M_ADC831_GET_BIT 0x04					;get bit 2
		_M_ADC831_GET_BIT 0x02					;get bit 1
		_M_ADC831_GET_BIT 0x01					;get bit 0
		sbi	_ADC831_PORT_OUT,_ADC831_BIT_CS			;cs high
		_M_ADC831_DELAY
		sbi	_ADC831_PORT_OUT,_ADC831_BIT_CLK		;clk high
		mov	Acc,AccH					;Acc=data read 
		pop	AccT
		pop	AccH						;restore registers
		ret
#endif
.exit

