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
;| A V R   U A R T  R O U T I N E S	  		|
;+------------------------------------------------------+
;| THIS DRIVE HAS INTEND TO USE WITH AVRS WITH SRAM	|
;+------------------------------------------------------+
;>------------------------------------------------------<
;| I M P L E M E N T E D  F U N C T I O N S 		|
;+------------------------------------------------------+---------------------------------------+
;| _UART_SET_BAUD_RATE	INITIALIZE UART								|
;|												|
;|.EQU	_UART_BAUD_RATE	  =9600		;DEFINE UART BAUD RATE					|
;|												|
;|			RCALL	_UART_SET_BAUD_RATE	;BAUD RATE AT 9600		 	|
;|----------------------------------------------------------------------------------------------|
;| _UART_TX_ENABLE	ENABLE/DISABLE TX							|
;|												|
;|			LDI	ACC,_ON		;ENABLE TX					|
;|			RCALL	_UART_TX_ENABLE							|
;|----------------------------------------------------------------------------------------------|
;| _UART_RX_ENABLE	ENABLE/DISABLE RX							|
;|												|
;|			LDI	ACC,_ON		;ENABLE RX					|
;|			RCALL	_UART_RX_ENABLE							|
;|----------------------------------------------------------------------------------------------|
;| _UART_TX_INT_ENABLE	ENABLE/DISABLE TX INTERRUPT 						|
;|												|
;|			LDI	ACC,_ON		;ENABLE TX					|
;|			RCALL	_UART_TX_ENABLE							|
;|----------------------------------------------------------------------------------------------|
;| _UART_RX_INT_ENABLE	ENABLE/DISABLE RX INTERRUPT						|
;|												|
;|			LDI	ACC,_ON		;ENABLE RX					|
;|			RCALL	_UART_RX_ENABLE							|
;|----------------------------------------------------------------------------------------------|
;| _UART_TX		SEND A BYTE DIRECT TO UDR AND TX					|
;|												|
;|			LDI	ACC,'A'		;send a ascii 'A'				|
;|			RCALL	_UART_TX							|
;|												|
;| OBS: This routine wait last transmission done before send a new byte				|
;+----------------------------------------------------------------------------------------------+
#ifndef __AVR_UART__
	#define __AVR_UART__
	.message "+---------------------+"
	.message "|           		|"
	.message "| AVR NORMAL UART  	|"
	.message "|           		|"
	.message "+---------------------+"
	;   |  |  |  |  |  |  |  |  |  |
	;+--+--+--+--+--+--+--+--+--+--+--+
	;|   UART BAUD RATE		  |
	;|  				  |
	;|\  Input : None	          |
	;| | Output: None		  |
	;|/  Destroy:None		  |
	;|   				  |
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
	_UART_SET_BAUD_RATE:
		ldi	Acc,_AVR_CLOCK/(16*_UART_BAUD_RATE)-1 	;set BAUD rate
		out	UBRR,Acc
		ret
	;   |  |  |  |  |  |  |  |  |  |
	;+--+--+--+--+--+--+--+--+--+--+--+
	;|   UART ENABLE/DISABLE TX	  |
	;|  				  |
	;|\  Input : Acc ON/OFF	          |
	;| | Output: None		  |
	;|/  Destroy:None		  |
	;|   				  |
	;|   			       	  |
	;+--+--+--+--+--+--+--+--+--+--+--+ 
	;   |  |  |  |  |  |  |  |  |  |
	;   \__\__\__\__\__\__\__\__\__\_________________________________________________________________________
	_UART_TX_ENABLE:
		sbi	UCR,TXEN				;assume tx on
		cpi	Acc,_ON					;turn tx on ?
		breq	_UART_TX_EN00				;yes, branch
		cbi	UCR,TXEN				;else turn tx off
	_UART_TX_EN00:
		ret
	;   |  |  |  |  |  |  |  |  |  |  |  |
	;+--+--+--+--+--+--+--+--+--+--+--+--+--+
	;|   UART ENABLE/DISABLE TX INTERRUPT   |
	;|  				  	|
	;|\  Input : Acc ON/OFF	          	|
	;| | Output: None		  	|
	;|/  Destroy:None		  	|
	;|   				  	|
	;|   			       	  	|
	;+--+--+--+--+--+--+--+--+--+--+--+--+--+ 
	;   |  |  |  |  |  |  |  |  |  |  |  |
	;   \__\__\__\__\__\__\__\__\__\_________________________________________________________________________
	_UART_TX_INT_ENABLE:
		sbi	UCR,TXCIE				;assume tx interrupt on
		cpi	Acc,_ON					;turn tx on ?
		breq	_UART_TX_IEN00				;yes, branch
		cbi	UCR,TXCIE				;else turn tx interrupt off
	_UART_TX_IEN00:
		ret
	;   |  |  |  |  |  |  |  |  |  |
	;+--+--+--+--+--+--+--+--+--+--+--+
	;|   UART ENABLE/DISABLE RX	  |
	;|  				  |
	;|\  Input : Acc ON/OFF	          |
	;| | Output: None		  |
	;|/  Destroy:None		  |
	;|   				  |
	;|   			       	  |
	;+--+--+--+--+--+--+--+--+--+--+--+ 
	;   |  |  |  |  |  |  |  |  |  |
	;   \__\__\__\__\__\__\__\__\__\_________________________________________________________________________
	_UART_RX_ENABLE:
		sbi	UCR,RXEN				;assume Rx on
		cpi	Acc,_ON					;turn Rx on ?
		breq	_UART_RX_EN00				;yes, branch
		cbi	UCR,RXEN				;else turn Rx off
	_UART_RX_EN00:
		ret
	;   |  |  |  |  |  |  |  |  |  |  |  |
	;+--+--+--+--+--+--+--+--+--+--+--+--+--+
	;|   UART ENABLE/DISABLE RX INTERRUPT	|
	;|  				 	|
	;|\  Input : Acc ON/OFF	          	|
	;| | Output: None		  	|
	;|/  Destroy:None		  	|
	;|   				  	|
	;|   			       	  	|
	;+--+--+--+--+--+--+--+--+--+--+--+--+--+ 
	;   |  |  |  |  |  |  |  |  |  |  |  |
	;   \__\__\__\__\__\__\__\__\__\_________________________________________________________________________
	_UART_RX_INT_ENABLE:
		sbi	UCR,RXCIE				;assume Rx interrupt on
		cpi	Acc,_ON					;turn Rx on ?
		breq	_UART_RX_IEN00				;yes, branch
		cbi	UCR,RXCIE				;else turn Rx interrupt off
	_UART_RX_IEN00:
		ret
	;   |  |  |  |  |  |  |  |  |  |  |  |
	;+--+--+--+--+--+--+--+--+--+--+--+--+--+
	;|   UART ENABLE/DISABLE RX INTERRUPT	|
	;|  				 	|
	;|\  Input : Acc ON/OFF	          	|
	;| | Output: None		  	|
	;|/  Destroy:None		  	|
	;|   				  	|
	;|   			       	  	|
	;+--+--+--+--+--+--+--+--+--+--+--+--+--+ 
	;   |  |  |  |  |  |  |  |  |  |  |  |
	;   \__\__\__\__\__\__\__\__\__\_________________________________________________________________________
	_UART_EMPTY_INT_ENABLE:
		sbi	UCR,UDRIE				;assume empty interrupt on
		cpi	Acc,_ON					;turn empty on ?
		breq	_UART_EM_IEN00				;yes, branch
		cbi	UCR,UDRIE				;else turn empty interrupt off
	_UART_EM_IEN00:
		ret
	;   |  |  |  |  |  |  |  |  |  |
	;+--+--+--+--+--+--+--+--+--+--+--+
	;|   UART TX DIRECT		  |
	;|  				  |
	;|\  Input : Acc byte to be send  |
	;| | Output: None		  |	
	;|/  Destroy:None		  |
	;|   				  |
	;|   			       	  |
	;+--+--+--+--+--+--+--+--+--+--+--+ 
	;   |  |  |  |  |  |  |  |  |  |
	;   \__\__\__\__\__\__\__\__\__\________________________________________________________________________
	_UART_TX:
		sbis	USR,UDRE		;wait last transmission done
		rjmp	_UART_TX
		out	UDR,Acc			;seta data to be sent
		ret
	;   |  |  |  |  |  |  |  |  |  |  |  |
	;+--+--+--+--+--+--+--+--+--+--+--+--+--+
	;|   UART RX DIRECT			|
	;|  					|
	;|\  Input : Acc byte received		|
	;| |         AccT:AccTH Mileseconds	|
	;| |			Time Out	|
	;| | Output: cy=1 if time out occur 	|	
	;|/  Destroy:None		  	|
	;|   				  	|
	;|   			       	  	|
	;+--+--+--+--+--+--+--+--+--+--+--+--+--+ 
	;   |  |  |  |  |  |  |  |  |  |  |  |
	;   \__\__\__\__\__\__\__\__\__\__\__\____________________________________________________________________
	;					/
	;					| Obs: if Time Out = 65535 then this routine no exit until
	;					|      UDR not receive a byte]
	;					\_________________________________________________________________
	_UART_RX:
		#ifdef __DEBUG_MODE__
			pushw	Z
			push	r0
			ldiw	Z,_UART_DATA*2
			lds	Acc,_UART_PTR
			inc	Acc
			sts	_UART_PTR,Acc
			dec	Acc
			add	ZL,Acc
			ldi	Acc,0
			adc	ZH,Acc
			lpm	
			mov	Acc,r0
			pop	r0
			popw	Z
			clc
			ret
		_UART_DATA:
			.DB	_ASCII_SOH,0x01
			.DB	0x01,_ASCII_STX
			.DB	0x03,0
			.DB	_ASCII_EOT,0
			.dseg
				_UART_PTR:	.byte 1
			.cseg
		#endif
		push	AccT			;save useds
		push	AccTH
		cpi	AccT,0xFF		;see if time out = 65535
		brne	_UART_RX30		;no, branch to timeout receive
		cpi	AccTH,0xFF
		brne	_UART_RX30		;no, branch to timeout receive
	_UART_RX00:
		sbis	USR,RXC			;wait character reception for infinit time until character received
		rjmp	_UART_RX00
	_UART_RX10:
		in	Acc,UDR			;get reseived data
		clc				;set no time out 
	_UART_RX20:
		pop	AccTH			;restore useds
		pop	AccT
		ret
	_UART_RX30:
		subi	AccT,1			;decrement mileseconds time counter
		sbci	AccTH,0
		brcs	_UART_RX20		;time expired? branch if yes
		push	AccT			;save AccT:AccTH
		push	AccTH
		ldi	AccT,low((_AVR_CLOCK-2000)/6000)		;load constant for 1ms delay
		ldi	AccTH,high((_AVR_CLOCK-2000)/6000)
		#ifdef __DEBUG_MODE__
			ldi	AccT,1
			ldi	AccTH,0
		#endif
	_UART_RX40:
		subi	AccT,1			;1ms delay expired ?
		sbci	AccTH,0
		brcs	_UART_RX50		;yes, brach
		sbis	USR,RXC			;else see if data received
		rjmp	_UART_RX40		;no, wait data and process time out
		pop	AccTH			;received, restore AccT:AccTH and get data
		pop	AccT
		rjmp	_UART_RX10
	_UART_RX50:
		pop	AccTH			;restore AccT:AccTH
		pop	AccT
		rjmp	_UART_RX30		;decrement mileseconds time counter
#endif
.exit

