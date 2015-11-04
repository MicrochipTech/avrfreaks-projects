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
;| BY JOÃO DARTAGNAN ANTUNES OLIVEIRA				|
;| DATA:22/03/2001						|
;>--------------------------------------------------------------<
;| F L A S H , E E P R O M  A N D  F U S E S  P R O G R A M  -	|
;| M M I N G  R O U T I N E S					|
;+--------------------------------------------------------------+
;| THIS DRIVE HAS INTEND TO USE WITH AVRS WITH SRAM		|
;+--------------------------------------------------------------+
;>------------------------------------------------------<
;| I M P L E M E N T E D  F U N C T I O N S 		|
;+------------------------------------------------------+---------------------------------------+
;| _AVR_PROG_INIT   INITIALIZE AVR PROGRAMMER							|
;|		    SAMPLE USING PROG_INIT TO START THE AVR PROGRAMMER				|
;|												|
;|		    rcall	_AVR_PROG_INIT							|
;|												|
;|----------------------------------------------------------------------------------------------|
;| _AVR_PROG_ENABLE   ENABLE THE HARDWARE PROGRAMMING						|
;|		      SAMPLE USING PROG_ENABLE TO ENABLE THE HARDWARE				|
;|												|
;|		    rcall	_AVR_PROG_ENABLE	;turn hardware programming enable	|
;|												|
;|----------------------------------------------------------------------------------------------|
;| _AVR_PROG_WRITE_READ_FOUR   WRITE AND READ FOUR BYTES OF DATA				|
;|			       SAMPLE USING PROG_WRITE_READ_FOUR TO WRITE AND READ THE DATA	|
;|												|
;|												|
;|												|
;|												|
;|												|
;|		    rcall	_AVR_PROG_WRITE_READ_FOUR					|
;|												|
;|----------------------------------------------------------------------------------------------|
;| _AVR_PROG_WRITE_READ_ONE   WRITE AND READ ONE BYTE OF DATA					|
;|			      SAMPLE USING PROG_WRITE_READ_ONE TO WRITE AND READ THE DATA	|
;|												|
;|		    rcall	_AVR_PROG_WRITE_READ_ONE					|
;|												|
;|----------------------------------------------------------------------------------------------|
;| _AVR_PROG_SETUP_TIME										|
;|												|
;|												|
;|												|
;|												|
;|----------------------------------------------------------------------------------------------|
#ifndef __AVR_PROG_SPI__
	
	#define __AVR_PROG_SPI__
	
	.message "+---------------------+
	.message "| AVR PROGRAMMING SPI |
	.message "+---------------------+
	
	;   |  |  |  |  |  |  |  |  |  |
	;+--+--+--+--+--+--+--+--+--+--+--+
	;|   AVR PROGRAMMING INIT	  |
	;|  				  |
	;| 			          |
	;|   Input  : None		  |
	;|\  Output : None		  |
	;| | Destroy: None		  |
	;|/	    			  |
	;|	          		  |
	;|			          |
	;|   			       	  |
	;+--+--+--+--+--+--+--+--+--+--+--+ 
	;   |  |  |  |  |  |  |  |  |  |
	;   \__\__\__\__\__\__\__\__\__\________________________________________________________________________
	_AVR_PROG_INIT:
		push	Acc				;save SREG
		in	Acc,SREG
		cli					;disable interrupts during initialization
		
		sbi	_AVR_PROG_PORT_OUTPUT,_AVR_PROG_BIT_SCK		;set sck is high
		sbi	_AVR_PROG_PORT_OUTPUT,_AVR_PROG_BIT_MISO	;set as this port as input with pull up
		sbi	_AVR_PROG_PORT_OUTPUT,_AVR_PROG_BIT_MOSI	;set mosi as high
		sbi	_AVR_PROG_PORT_OUTPUT,_AVR_PROG_BIT_RESET	;set reset as high
		sbi	_AVR_PROG_PORT_DIR,_AVR_PROG_BIT_SCK		;sck as output
		cbi	_AVR_PROG_PORT_DIR,_AVR_PROG_BIT_MISO		;miso as input
		sbi	_AVR_PROG_PORT_DIR,_AVR_PROG_BIT_MOSI		;mosi as output
		sbi	_AVR_PROG_PORT_DIR,_AVR_PROG_BIT_RESET		;reset as output
	_AVR_PROG_SREG_ACC_RESTORE:	
		out	SREG,Acc			;restore SREG
		pop	Acc				
		ret
		
	;   |  |  |  |  |  |  |  |  |  |
	;+--+--+--+--+--+--+--+--+--+--+--+
	;|   AVR PROGRAMMING ENABLE	  |
	;|  				  |
	;| 			          |
	;|   Input  : None		  |
	;|\  Output : None		  |
	;| | Destroy: None		  |
	;|/	    			  |
	;|	          		  |
	;|			          |
	;|   			       	  |
	;+--+--+--+--+--+--+--+--+--+--+--+ 
	;   |  |  |  |  |  |  |  |  |  |
	;   \__\__\__\__\__\__\__\__\__\________________________________________________________________________
	_AVR_PROG_ENABLE:
		push	Acc
		in	Acc,SREG					;save sreg
		cli							;disable interrupts
		cbi	_AVR_PROG_PORT_OUTPUT,_AVR_PROG_BIT_SCK		;turn SCK LOW
		_M_DELAY_MILESECONDS	1				;wait a 1 ms
		sbi	_AVR_PROG_PORT_OUTPUT,_AVR_PROG_BIT_RESET	;set reset as high
		_M_DELAY_MILESECONDS	20				;wait 20ms
		cbi	_AVR_PROG_PORT_OUTPUT,_AVR_PROG_BIT_RESET	;turn low again
		_M_DELAY_MILESECONDS	20				;wait 20ms
		rjmp	_AVR_PROG_SREG_ACC_RESTORE			;restore SREG
		
	;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  
	;+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
	;|   AVR WRITE/READ 4 BYTES	  	      |
	;|  				  	      |
	;| 			          	      |
	;|   Input  : r0,r1,r2,r3 bytes to be write   |
	;|\  Output : r4,r5,r6,r7 bytes to be read    |
	;| | Destroy: None		  	      |
	;|/	    			  	      |
	;|	          		  	      |
	;|			          	      |
	;|   			       	  	      |
	;+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+ 
	;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  
	;   \__\__\__\__\__\__\__\__\__\__\__\__\__\________________________________________________________________
		
	_AVR_PROG_WRITE_READ_FOUR:
		push	Acc
		in	Acc,SREG					;save SREG state and Disable interrupts
		cli
		push	Acc
		mov	Acc,r0
		rcall	_AVR_PROG_WRITE_READ_ONE			;write first 1st byte and read 1st byte
		mov	r4,Acc
		mov	Acc,r1
		rcall	_AVR_PROG_WRITE_READ_ONE			;write first 2st byte and read 2st byte
		mov	r5,Acc
		mov	Acc,r2
		rcall	_AVR_PROG_WRITE_READ_ONE			;write first 3st byte and read 3st byte
		mov	r6,Acc
		mov	Acc,r3
		rcall	_AVR_PROG_WRITE_READ_ONE			;write first 4st byte and read 4st byte
		mov	r7,Acc
		pop	Acc						;restore SREG state and restore interrupts
		out	SREG,Acc
		pop	Acc
		ret
	;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  
	;+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
	;|   AVR WRITE/READ 1 BYTE	  	      |
	;|  				  	      |
	;| 			          	      |
	;|   Input  : Acc byte to be write   	      |
	;|\  Output : Acc byte to be read	      |
	;| | Destroy: None		  	      |
	;|/	    			  	      |
	;|	          		  	      |
	;|			          	      |
	;|   			       	  	      |
	;+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+ 
	;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  
	;   \__\__\__\__\__\__\__\__\__\__\__\__\__\________________________________________________________________
		
	_AVR_PROG_WRITE_READ_ONE:
		push	AccH
		push	AccT
		ldi	AccT,8
	_AVR_PROG_ONE_00:
		rol	Acc						;get fist digit to be send
		brcc	_AVR_PROG_ONE_10				;branch if zero
		sbi	_AVR_PROG_PORT_OUTPUT,_AVR_PROG_BIT_MOSI	;set MOSI=1
		rjmp	_AVR_PROG_ONE_20				;branch
	_AVR_PROG_ONE_10:
		cbi	_AVR_PROG_PORT_OUTPUT,_AVR_PROG_BIT_MOSI	;set MOSI=0
	_AVR_PROG_ONE_20:
	;	rcall	_AVR_PROG_SETUP_TIME				;setup time
		nop
		nop
	;	nop
	;	nop
		sbi	_AVR_PROG_PORT_OUTPUT,_AVR_PROG_BIT_SCK		;set SCK=1
	;	rcall	_AVR_PROG_SETUP_TIME				;setup time
	;	nop
	;	nop
		nop
		nop
		clc							;assume read bit=0
		sbic	_AVR_PROG_PORT_INPUT,_AVR_PROG_BIT_MISO		;read a bit
		sec							;else read bit=1
		rol	AccH
	;	rcall	_AVR_PROG_SETUP_TIME				;setup time
	;	nop
	;	nop
		nop
		nop
		cbi	_AVR_PROG_PORT_OUTPUT,_AVR_PROG_BIT_SCK		;set SCK=0
	;	rcall	_AVR_PROG_SETUP_TIME				;setup time
	;	nop
	;	nop
		nop
		nop
		dec	AccT
		brne	_AVR_PROG_ONE_00				;until all done
		mov	Acc,AccH					;plase read byte into Acc
		pop	AccT
		pop	AccH
		ret
		
	;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  
	;+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
	;|   AVR SETUP TIME TO WARANTY MINIMAL TIME   |
	;|  				  	      |
	;| 			          	      |
	;|   Input  : None   	      		      |
	;|\  Output : None			      |
	;| | Destroy: None		  	      |
	;|/	    			  	      |
	;|	          		  	      |
	;|			          	      |
	;|   			       	  	      |
	;+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+ 
	;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  
	;   \__\__\__\__\__\__\__\__\__\__\__\__\__\________________________________________________________________
		
	_AVR_PROG_SETUP_TIME:
		nop							;in this case i use 8 clocks 3(call)+1(nop)+4(ret)
		nop
		ret
#endif
.exit

