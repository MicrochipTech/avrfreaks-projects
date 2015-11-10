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
;| DATA:16/11/2000						|
;>--------------------------------------------------------------<
;| A D C  F O R  A V R  8 5 3 5  				|
;+--------------------------------------------------------------+
;| THIS DRIVE HAS INTEND TO USE WITH AVRS WITH SRAM		|
;+--------------------------------------------------------------+
;>------------------------------------------------------<
;| I M P L E M E N T E D  F U N C T I O N S 		|
;+------------------------------------------------------+---------------------------------------+
;| _ADC_INIT		INITIALIZE ADC8535 INTERNAL CONVERTERS					|
;|			INITIALIZATION EXAMPLE							|
;|												|
;|			LDI	Acc,6		;SET DIVISION FACTOR 2^6=64			|
;|			RCALL	_ADC_INIT	;AFTER THIS POINT ALL ADC ROUTINES WORKING 	|
;|----------------------------------------------------------------------------------------------|
;| _ADC_CHANNEL		SET ADC CHANNEL TO GET VALUES						|
;|												|
;|			LDI	Acc,3		;SET CHANNEL 3 TO GET VALUE			|
;|			RCALL	_ADC_GET	;AFTER THIS POINT ACC:ACCH HAVE A 10 BIT VALUE	|
;|----------------------------------------------------------------------------------------------|
;| _ADC_GET		TO GET VALUE FROM ADC8535 INTERNAL					|
;|												|
;|			RCALL	_ADC_GET	;AFTER THIS POINT ACC:ACCH HAVE A 10 BIT VALUE	|
;+----------------------------------------------------------------------------------------------+
#ifndef __ADCS_ADC8535__
	#define __ADCS_ADC8535__
	.message "+--------------+"
	.message "|              |"
	.message "| ADCS ADC8535 |"
	.message "|              |"
	.message "+--------------+"
	;   |  |  |  |  |  |  |  |  |  |
	;+--+--+--+--+--+--+--+--+--+--+--+
	;|   ADC GET VALUE		  |
	;|  				  |
	;|\			          |
	;| | Input  : clk prescaler factor|
	;| |          in power of 2	  |
	;|/	      Ex: 1=2,2=4,3=8..	  |		
	;|   Output : none		  |
	;|   Destroy: None	          |
	;|   			       	  |
	;+--+--+--+--+--+--+--+--+--+--+--+ 
	;   |  |  |  |  |  |  |  |  |  |
	;   \__\__\__\__\__\__\__\__\__\________________________________________________________________________
	_ADC_INIT:
		push	Acc				;Save used
		push	AccH
		push	AccT
		in	AccT,SREG			;Save SREG status
		push	AccT
		cli					;clear interrupts
		andi	Acc,7				;only prescaler factor
		in	AccH,ADCSR			;read Actual status
		andi	AccH,0b01111000
		or	Acc,AccH			;bound prescaler factor
		ori	Acc,(1<<ADEN) | (1<<ADSC)	;issue ADC enable and start fist dummy conversion
		out	ADCSR,Acc
	_ADC_I00:
		sbic	ADCSR,ADSC			;Wait 1st Convesion done
		rjmp	_ADC_I00
		pop	AccT
		out	SREG,AccT			;restore SREG status
		pop	AccT
		pop	AccH
		pop	Acc
		ret
	;   |  |  |  |  |  |  |  |  |  |
	;+--+--+--+--+--+--+--+--+--+--+--+
	;|   ADC SET CHANNEL		  |
	;|  				  |
	;|\			          |
	;| | Input  : channel number 0..7 |
	;|/  Output : None		  |
	;|   Destroy: None	          |
	;|   			       	  |
	;+--+--+--+--+--+--+--+--+--+--+--+ 
	;   |  |  |  |  |  |  |  |  |  |
	;   \__\__\__\__\__\__\__\__\__\________________________________________________________________________
	_ADC_CHANNEL:
		out	ADMUX,Acc			;set channel 
		ret
	;   |  |  |  |  |  |  |  |  |  |
	;+--+--+--+--+--+--+--+--+--+--+--+
	;|   ADC GET VALUE		  |
	;|  				  |
	;|\			          |
	;| | Input  : None	          |
	;|/  Output : Acc:AccH Value      |
	;|   Destroy: None	          |
	;|   			       	  |
	;+--+--+--+--+--+--+--+--+--+--+--+ 
	;   |  |  |  |  |  |  |  |  |  |
	;   \__\__\__\__\__\__\__\__\__\________________________________________________________________________
	_ADC_GET:
		sbi	ADCSR,ADSC			;set start conversion
	_ADC_G00:
		sbic	ADCSR,ADSC			;Wait conversion complete
		rjmp	_ADC_G00
		in	Acc,ADCL			;read value
		in	AccH,ADCH
		ret
#endif
.exit

