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
;| BY JOÃO DARTAGNAN ANTUNES OLIVEIRA			|
;| DATA:21/10/2002					|
;>------------------------------------------------------<
;| A V R   M 1 2 8  U A R T  R O U T I N E S	  	|
;+------------------------------------------------------+
;| THIS DRIVE HAS INTEND TO USE WITH AVRS WITH SRAM	|
;+------------------------------------------------------+
;>------------------------------------------------------<
;| I M P L E M E N T E D  F U N C T I O N S 		|
;+------------------------------------------------------+---------------------------------------+
;| _USART_SET_NORMAL_SPEED  SET USART NORMAL SPEED 						|
;|												|
;|			RCALL	_USART_SET_NORMAL_SPEED	;NOW USART IN NORMAL SPEED	 	|
;|----------------------------------------------------------------------------------------------|
;| _USART_SET_DOUBLE_SPEED  SET USART DOUBLE SPEED 						|
;|												|
;|			RCALL	_USART_SET_DOUBLE_SPEED	;NOW USART IN DOUBLE SPEED	 	|
;|----------------------------------------------------------------------------------------------|
;| _USART_SET_BAUD_RATE DEFINE USART BAUD RATE	 						|
;|												|
;|			RCALL	_USART_SET_NORMAL_SPEED	;FOR RIGHT SPEED SETTINGS		|
;|			LDIAW	9600			;SET USART FOR 9600 BAUDS		|
;|			RCALL	_USART_SET_BAUD_RATE	;NOW USART OPERATING IN 9600 BAUDS	|
;|----------------------------------------------------------------------------------------------|
;| _USART_TX_ENABLE	ENABLE/DISABLE TX							|
;|												|
;|			LDI	ACC,_ON		;ENABLE TX					|
;|			RCALL	_USART_TX_ENABLE						|
;|----------------------------------------------------------------------------------------------|
;| _USART_RX_ENABLE	ENABLE/DISABLE RX							|
;|												|
;|			LDI	ACC,_ON		;ENABLE RX					|
;|			RCALL	_USART_RX_ENABLE						|
;|----------------------------------------------------------------------------------------------|
;| _USART_TX_INT_ENABLE	ENABLE/DISABLE TX INTERRUPT 						|
;|												|
;|			LDI	ACC,_ON		;ENABLE TX					|
;|			RCALL	_USART_TX_ENABLE						|
;|----------------------------------------------------------------------------------------------|
;| _USART_RX_INT_ENABLE	ENABLE/DISABLE RX INTERRUPT						|
;|												|
;|			LDI	ACC,_ON		;ENABLE RX					|
;|			RCALL	_USART_RX_ENABLE						|
;|----------------------------------------------------------------------------------------------|
;| _USART_SET_TIMEOUT	SET USART TIMEOUT IN MILESECONDS					|
;|												|
;|			LDIAW	1000		;SET TIMEOUT FOR 1SECOND			|
;|			RCALL	_USART_SET_TIMEOUT						|
;|----------------------------------------------------------------------------------------------|
;| _USART_TX		SEND A BYTE TO USART							|
;|												|
;|			LDI	ACC,'A'		;send a ascii 'A'				|
;|			RCALL	_USART_TX							|
;|												|
;| OBS: This routine wait last transmission done before send a new byte				|
;+----------------------------------------------------------------------------------------------+
;| _USART_RX		WAIT A BYTE FROM USART							|
;|												|
;|			LDIAWT	100		;wait 100ms for receive a byte			|
;|						;set this register to -1 to infinit wait	|
;|			RCALL	_USART_RX							|
;|												|
;+----------------------------------------------------------------------------------------------+
#ifndef __INTEGER_DIV_U32U32U32__
	.INCLUDE "MATH\INTEGER\DIV_U32U32U32\DIV_U32U32U32.INC"
#endif
#ifndef __AVR_M8_USART__
	#define __AVR_M8_USART__
	.message "+----------------+"
	.message "|                |"
	.message "| AVR M8 USART   |"
	.message "|                |"
	.message "+----------------+"
	.EQU	_USART_MODE_POLLING	= 1			;pulling receiving mode default
	.EQU	_USART_MODE_BUFFERED	= 2			;using interrupt and receiver circurlar buffer
	.EQU	_USART_BUFFER_SIZE	= 32
	.DSEG
		_USART_TIMEOUT:		.BYTE	2		;time out in mileseconds
		_USART_MODE:		.BYTE	1
		_USART_G:		.BYTE	1		;queue G pointer
		_USART_P:		.BYTE	1		;queue P pointer
		_USART_BUFFER:		.BYTE	_USART_BUFFER_SIZE	;fixed must be power of 2
	.CSEG
	;   |  |  |  |  |  |  |  |  |  |
	;+--+--+--+--+--+--+--+--+--+--+--+
	;|   USART INIT			  |
	;|  				  |
	;|\  Input : Acc mode		  |
	;| | Output: None		  |
	;|/  Destroy:None		  |
	;|   				  |
	;|   			       	  |
	;+--+--+--+--+--+--+--+--+--+--+--+ 
	;   |  |  |  |  |  |  |  |  |  |
	;   \__\__\__\__\__\__\__\__\__\_______
	;				       \
	_USART_INIT:
		pushaw
		in	AccH,SREG
		cli
		push	Acc
		sts	_USART_MODE,Acc
		cpi	Acc,_USART_MODE_POLLING				;polling mode ?
		breq	_USART_INIT_10					;yes, branch
		ldi	Acc,0
		sts	_USART_G,Acc
		sts	_USART_P,Acc
		ldi	Acc,_ON
		rcall	_USART_RX_INT_ENABLE				;enable interrupt receiver enable
		_SET_HANDDLE _HDC_URXC_VECT,_USART_RX_PROCESS		;set Drive Process handdle routine
		rjmp	_USART_INIT_EXIT
	_USART_INIT_10:
	_USART_INIT_EXIT:
		pop	Acc
		out	SREG,AccH
		popaw
		ret
	;   |  |  |  |  |  |  |  |  |  |
	;+--+--+--+--+--+--+--+--+--+--+--+
	;|   USART BAUD RATE		  |
	;|  				  |
	;|\  Input : AccH:Acc BAUD RATE   |
	;| | Output: None		  |
	;|/  Destroy:Flags,r0..r11	  |
	;|   obs: assume normal speed	  |
	;|   			       	  |
	;+--+--+--+--+--+--+--+--+--+--+--+ 
	;   |  |  |  |  |  |  |  |  |  |
	;   \__\__\__\__\__\__\__\__\__\_______
	;				       \
	;					+---------------------------------------+
	;					| BAUD rate equation 			|
	;					|					|
	;					|		AVR_CLOCK		|
	;					| BAUD = ---------------------		|
	;					|		16(UBRR+1)		|
	;					|					|
	;					| Resolvin in UBRR,			|
	;					|					|
	;					|		AVR_CLOCK		|
	;					| UBRR = ------------------ - 1		|
	;					|		16.BAUD			|
	;					+---------------------------------------+
	_USART_SET_BAUD_RATE:
		pushaw
		mov	r4,Acc							;move baud rate to appropriate registers
		mov	r5,AccH
		clr	r6
		clr	r7
		ldi	Acc,4							;multiply by 16
	_USART_SET_BAU00:
		lsl	r4
		rol	r5
		rol	r6
		rol	r7
		dec	Acc
		brne	_USART_SET_BAU00
		ldi	Acc,low(lwrd(_AVR_CLOCK))				;get avr clock
		mov	r0,Acc
		ldi	Acc,high(lwrd(_AVR_CLOCK))				;get avr clock
		mov	r1,Acc
		ldi	Acc,low(hwrd(_AVR_CLOCK))				;get avr clock
		mov	r2,Acc
		ldi	Acc,high(hwrd(_AVR_CLOCK))				;get avr clock
		mov	r3,Acc
		rcall	_DIV_U32U32U32						;AVR_CLOCK/16*BAUD
		ldi	Acc,-1
		add	r0,Acc							;AVR_CLOCK/(16*BAUD)-1
		adc	r1,Acc
		adc	r2,Acc
		adc	r3,Acc
		out	UBRRL,r0						;set baud rate
		out	UBRRH,r1
		popaw
		ret
	;   |  |  |  |  |  |  |  |  |  |
	;+--+--+--+--+--+--+--+--+--+--+--+
	;|   USART SET NORMAL SPEED	  |
	;|  				  |
	;|\  Input : None		  |
	;| | Output: None		  |
	;|/  Destroy:None		  |
	;|   				  |
	;|   			       	  |
	;+--+--+--+--+--+--+--+--+--+--+--+ 
	;   |  |  |  |  |  |  |  |  |  |
	;   \__\__\__\__\__\__\__\__\__\_______
	;				       \
	_USART_SET_NORMAL_SPEED:
		push	Acc
		;comm 0
		in	Acc,UCSRA
		andi	Acc,~(1<<U2X)
		out	UCSRA,Acc
		pop	Acc
		ret	
	;   |  |  |  |  |  |  |  |  |  |
	;+--+--+--+--+--+--+--+--+--+--+--+
	;|   USART SET DOUBLE SPEED	  |
	;|  				  |
	;|\  Input : None		  |
	;| | Output: None		  |
	;|/  Destroy:None		  |
	;|   				  |
	;|   			       	  |
	;+--+--+--+--+--+--+--+--+--+--+--+ 
	;   |  |  |  |  |  |  |  |  |  |
	;   \__\__\__\__\__\__\__\__\__\_______
	;				       \
	_USART_SET_DOUBLE_SPEED:
		push	Acc
		;comm 0
		in	Acc,UCSRA
		ori	Acc,(1<<U2X)
		out	UCSRA,Acc
		pop	Acc
		ret
 	;   |  |  |  |  |  |  |  |  |  |
	;+--+--+--+--+--+--+--+--+--+--+--+
	;|   USART ENABLE/DISABLE TX	  |
	;|  				  |
	;|\  Input : Acc ON/OFF	          |
	;| | Output: None		  |
	;|/  Destroy:None		  |
	;|   				  |
	;|   			       	  |
	;+--+--+--+--+--+--+--+--+--+--+--+ 
	;   |  |  |  |  |  |  |  |  |  |
	;   \__\__\__\__\__\__\__\__\__\_________________________________________________________________________
	_USART_TX_ENABLE:
		push	Acc
		cpi	Acc,_ON						;ON ?
		brne	_USART_TX_ENABLE_20				;no, branch
		in	Acc,UCSRB					;enable tx
		ori	Acc,(1<<TXEN)
	_USART_TX_ENABLE_10:
		out	UCSRB,Acc
		pop	Acc
		ret
	_USART_TX_ENABLE_20:
		in	Acc,UCSRB					;disable tx
		andi	Acc,~(1<<TXEN)
		rjmp	_USART_TX_ENABLE_10
	;   |  |  |  |  |  |  |  |  |  |  |  |
	;+--+--+--+--+--+--+--+--+--+--+--+--+--+
	;|   USART ENABLE/DISABLE TX INTERRUPT   |
	;|  				  	|
	;|\  Input : Acc ON/OFF	          	|
	;| | Output: None		  	|
	;|/  Destroy:None		  	|
	;|   				  	|
	;|   			       	  	|
	;+--+--+--+--+--+--+--+--+--+--+--+--+--+ 
	;   |  |  |  |  |  |  |  |  |  |  |  |
	;   \__\__\__\__\__\__\__\__\__\__\__\____________________________________________________________________
	_USART_TX_INT_ENABLE:
		push	Acc
		cpi	Acc,_ON						;ON ?
		brne	_USART_TX_INT_ENABLE_20				;no, branch
		in	Acc,UCSRB					;enable tx comm 0
		ori	Acc,(1<<TXCIE)
	_USART_TX_INT_ENABLE_10:
		out	UCSRB,Acc
		pop	Acc
		ret
	_USART_TX_INT_ENABLE_20:
		in	Acc,UCSRB					;disable tx comm 0
		andi	Acc,~(1<<TXCIE)
		rjmp	_USART_TX_INT_ENABLE_10
	;   |  |  |  |  |  |  |  |  |  |
	;+--+--+--+--+--+--+--+--+--+--+--+
	;|   USART ENABLE/DISABLE RX	  |
	;|  				  |
	;|\  Input : Acc ON/OFF	          |
	;| | Output: None		  |
	;|/  Destroy:None		  |
	;|   				  |
	;|   			       	  |
	;+--+--+--+--+--+--+--+--+--+--+--+ 
	;   |  |  |  |  |  |  |  |  |  |
	;   \__\__\__\__\__\__\__\__\__\_________________________________________________________________________
	_USART_RX_ENABLE:
		push	Acc
		cpi	Acc,_ON						;ON ?
		brne	_USART_RX_ENABLE_20				;no, branch
		in	Acc,UCSRB					;enable tx comm 0
		ori	Acc,(1<<RXEN)
	_USART_RX_ENABLE_10:
		out	UCSRB,Acc
		pop	Acc
		ret
	_USART_RX_ENABLE_20:
		in	Acc,UCSRB					;disable tx comm 0
		andi	Acc,~(1<<RXEN)
		rjmp	_USART_RX_ENABLE_10
	;   |  |  |  |  |  |  |  |  |  |  |  |
	;+--+--+--+--+--+--+--+--+--+--+--+--+--+
	;|   USART ENABLE/DISABLE RX INTERRUPT	|
	;|  				 	|
	;|\  Input : Acc ON/OFF	          	|
	;| | Output: None		  	|
	;|/  Destroy:None		  	|
	;|   				  	|
	;|   			       	  	|
	;+--+--+--+--+--+--+--+--+--+--+--+--+--+ 
	;   |  |  |  |  |  |  |  |  |  |  |  |
	;   \__\__\__\__\__\__\__\__\__\_________________________________________________________________________
	_USART_RX_INT_ENABLE:
		push	Acc
		cpi	Acc,_ON						;ON ?
		brne	_USART_RX_INT_ENABLE_20				;no, branch
		in	Acc,UCSRB					;enable rx comm 0
		ori	Acc,(1<<RXCIE)
	_USART_RX_INT_ENABLE_10:
		out	UCSRB,Acc
		pop	Acc
		ret
	_USART_RX_INT_ENABLE_20:
		in	Acc,UCSRB					;disable rx comm 0
		andi	Acc,~(1<<RXCIE)
		rjmp	_USART_RX_INT_ENABLE_10
	;   |  |  |  |  |  |  |  |  |  |  |  |
	;+--+--+--+--+--+--+--+--+--+--+--+--+--+
	;|   USART RX FLUSH			|
	;|  				 	|
	;|\  Input : None	          	|
	;| | Output: None		  	|
	;|/  Destroy:None		  	|
	;|   				  	|
	;|   			       	  	|
	;+--+--+--+--+--+--+--+--+--+--+--+--+--+ 
	;   |  |  |  |  |  |  |  |  |  |  |  |
	;   \__\__\__\__\__\__\__\__\__\_________________________________________________________________________
	_USART_RX_FLUSH:
		push	Acc
	_USART_RX_FLUSH_10:
		sbis 	UCSRA,RXC
		rjmp	_USART_RX_FLUSH_20
		in 	Acc, UDR
		rjmp _USART_RX_FLUSH_10
	_USART_RX_FLUSH_20:
		ldi	Acc,0				;clear receiver queue
		sts	_USART_P,Acc
		sts	_USART_G,Acc
		pop	Acc
		ret
	;   |  |  |  |  |  |  |  |  |  |  |  |
	;+--+--+--+--+--+--+--+--+--+--+--+--+--+
	;|   USART SET TIMEOUT			|
	;|  				 	|
	;|\  Input : AccH:Acc timeout          	|
	;| | Output: None		  	|
	;|/  Destroy:None		  	|
	;|   				  	|
	;|   			       	  	|
	;+--+--+--+--+--+--+--+--+--+--+--+--+--+ 
	;   |  |  |  |  |  |  |  |  |  |  |  |
	;   \__\__\__\__\__\__\__\__\__\_________________________________________________________________________
	_USART_SET_TIMEOUT:
		stsaw	_USART_TIMEOUT
		ret
	;   |  |  |  |  |  |  |  |  |  |
	;+--+--+--+--+--+--+--+--+--+--+--+
	;|   USART TX DIRECT		  |
	;|  				  |
	;|\  Input : Acc byte to be send  |
	;| | Output: None		  |	
	;|/  Destroy:none		  |
	;|   				  |
	;|   			       	  |
	;+--+--+--+--+--+--+--+--+--+--+--+ 
	;   |  |  |  |  |  |  |  |  |  |
	;   \__\__\__\__\__\__\__\__\__\________________________________________________________________________
	_USART_TX:
		sbis	UCSRA,UDRE					;wait last transmission done
		rjmp	_USART_TX					;*<NOCHANGE>*
		out	UDR,Acc						;set data to be sent
		ret
	;   |  |  |  |  |  |  |  |  |  |  |  |
	;+--+--+--+--+--+--+--+--+--+--+--+--+--+
	;|   USART PROCESS INTERRUPT RX 	|
	;|  					|
	;|\  Input : None			|
	;| |					|
	;| | Output: None		 	|
	;| |	     				|
	;|/  Destroy:none		  	|
	;|   				  	|
	;|   			       	  	|
	;+--+--+--+--+--+--+--+--+--+--+--+--+--+ 
	;   |  |  |  |  |  |  |  |  |  |  |  |
	;   \__\__\__\__\__\__\__\__\__\__\__\____________________________________________________________________
	_USART_RX_PROCESS:
		push	Acc
		in	Acc,SREG
		pushaw
		pushw	Z
		in	Acc,UDR							;get data
		ldiw	Z,_USART_BUFFER						;Z-->QUEUE
		lds	AccH,_USART_P						;*Z(P)<-data
		add	ZL,AccH
		ldi	AccH,0
		adc	ZH,AccH
		st	Z,Acc
		lds	AccH,_USART_P
		inc	AccH
		andi	AccH,_USART_BUFFER_SIZE-1
		sts	_USART_P,AccH
		popw	Z
		popaw									;set ok
		out	SREG,Acc
		pop	Acc
		ret
	;   |  |  |  |  |  |  |  |  |  |  |  |
	;+--+--+--+--+--+--+--+--+--+--+--+--+--+
	;|   USART CHECK IF DATA IN BUFFER RX 	|
	;|  					|
	;|\  Input : None			|
	;| |					|
	;| | Output: cy=1 if data in buffer 	|
	;| |	     				|
	;|/  Destroy:none		  	|
	;|   				  	|
	;|   time:18		       	  	|
	;+--+--+--+--+--+--+--+--+--+--+--+--+--+ 
	;   |  |  |  |  |  |  |  |  |  |  |  |
	;   \__\__\__\__\__\__\__\__\__\__\__\____________________________________________________________________
	_USART_DATA_READY:
		pushaw
		lds	Acc,_USART_P
		lds	AccH,_USART_G
		cp	Acc,AccH
		clc
		breq	_USART_DATA_READY_10
		sec
	_USART_DATA_READY_10:
		popaw
		ret	
	;   |  |  |  |  |  |  |  |  |  |  |  |
	;+--+--+--+--+--+--+--+--+--+--+--+--+--+
	;|   USART GET DATA BUFFER	 	|
	;|  					|
	;|\  Input : None			|
	;| |					|
	;| | Output: Acc data			|
	;| |	     				|
	;|/  Destroy:none		  	|
	;|   				  	|
	;|   time:18		       	  	|
	;+--+--+--+--+--+--+--+--+--+--+--+--+--+ 
	;   |  |  |  |  |  |  |  |  |  |  |  |
	;   \__\__\__\__\__\__\__\__\__\__\__\____________________________________________________________________
	_USART_GET_DATA_BUFFER:
		push	AccH
		pushw	Z
		ldiw	Z,_USART_BUFFER
		lds	AccH,_USART_G
		add	ZL,AccH
		ldi	AccH,0
		add	ZH,AccH
		ld	Acc,Z
		lds	AccH,_USART_G
		inc	AccH
		andi	AccH,_USART_BUFFER_SIZE-1
		sts	_USART_G,AccH
		popw	Z
		pop	AccH
		ret
	;   |  |  |  |  |  |  |  |  |  |  |  |
	;+--+--+--+--+--+--+--+--+--+--+--+--+--+
	;|   USART RX DIRECT			|
	;|  					|
	;|\  Input : None			|
	;| |					|
	;| | Output: cy=1 if time out occur 	|
	;| |	     Acc byte received		|
	;|/  Destroy:none		  	|
	;|   				  	|
	;|   			       	  	|
	;+--+--+--+--+--+--+--+--+--+--+--+--+--+ 
	;   |  |  |  |  |  |  |  |  |  |  |  |
	;   \__\__\__\__\__\__\__\__\__\__\__\____________________________________________________________________
	;					/
	;					| Obs: if Time Out = 65535 then this routine no exit until
	;					|      UDR not receive a byte]
	;					\_________________________________________________________________
	_USART_RX:
		#ifdef __DEBUG_MODE__
			.DSEG
				_USART_CT:	.BYTE	_BYTE
			.CSEG
			pushw	Z
			ldiw	Z,_USART_DATA_TABLE*2
			lds	Acc,_USART_CT
			add	ZL,Acc
			ldi	Acc,0
			adc	ZH,Acc
			lpm	Acc,Z
			push	Acc
			lds	Acc,_USART_CT
			inc	Acc
			sts	_USART_CT,Acc
			pop	Acc
			popw	Z
			ret
			_USART_DATA_TABLE:
				.DB	0x27,0x23,0xA4,0x12,0x47,0x6c,0x99,00
		#endif
		pushawt
		lds	AccT,_USART_MODE
		cpi	AccT,_USART_MODE_BUFFERED	;buffered mode?
		breq	_USART_RX_BUF		;yes, branch
		;//////////////////////////
		;// MODE POOLING
		;//////////////////////////
		ldsawt	_USART_TIMEOUT		;get time out
		cpi	AccT,0xFF		;see if time out = 65535
		brne	_USART_RX30		;no, branch to timeout receive
		cpi	AccTH,0xFF
		brne	_USART_RX30		;no, branch to timeout receive
	_USART_RX00:
		sbis	UCSRA,RXC		;wait character reception for infinit time until character received
		rjmp	_USART_RX00
	_USART_RX10:
		in	Acc,UDR			;get reseived data
		clc				;set no time out 
	_USART_RX20:
		popawt				;return with no timeout
		ret
	_USART_RX20A:
		sec					
		popawt				;return with timeout
		ret
	_USART_RX30:
		addiawt	1			;to compensate next decrement
	_USART_RX30A:
		subiawt	1			;decrement mileseconds time counter
		breq	_USART_RX20A		;time expired? branch if yes
		pushawt
		ldiawt	((_AVR_CLOCK-2000)/6000+1);load constant for 1ms delay
		#ifdef __DEBUG_MODE__
			ldiawt	1
		#endif
	_USART_RX40:
		subiawt	1			;1ms delay expired ?
		breq	_USART_RX50		;yes, brach
		sbis	UCSRA,RXC		;else see if data received
		rjmp	_USART_RX40		;no, wait data and process time out
		popawt
		rjmp	_USART_RX10
	_USART_RX50:
		popawt
		rjmp	_USART_RX30A		;decrement mileseconds time counter
		;//////////////////////////
		;// MODE BUFFERED
		;//////////////////////////
	_USART_RX_BUF:
		ldsawt	_USART_TIMEOUT		;get time out
		cpi	AccT,0xFF		;see if time out = 65535
		brne	_USART_RX30B		;no, branch to timeout receive
		cpi	AccTH,0xFF
		brne	_USART_RX30B		;no, branch to timeout receive
	_USART_RX00B:
		rcall	_USART_DATA_READY
		brcc	_USART_RX00B
	_USART_RX10B:
		rcall	_USART_GET_DATA_BUFFER
		clc				;set no time out 
	_USART_RX20B:
		popawt				;return with no timeout
		ret
	_USART_RX20AB:
		sec					
		popawt				;return with timeout
		ret
	_USART_RX30B:
		addiawt	1			;to compensate next decrement
	_USART_RX30AB:
		subiawt	1			;decrement mileseconds time counter
		breq	_USART_RX20AB		;time expired? branch if yes
		pushawt
		ldiawt	((_AVR_CLOCK-2000)/25000+1);load constant for 1ms delay
		#ifdef __DEBUG_MODE__
			ldiawt	1
		#endif
	_USART_RX40B:
		subiawt	1			;1ms delay expired ?
		breq	_USART_RX50B		;yes, brach
		rcall	_USART_DATA_READY
		brcc	_USART_RX40B
		popawt
		rjmp	_USART_RX10B
	_USART_RX50B:
		popawt
		rjmp	_USART_RX30AB		;decrement mileseconds time counter
#endif
.exit

