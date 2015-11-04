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
;| A V R   C O M M   R O U T I N E S	  		|
;| D T  C O M M  V 1 
;+------------------------------------------------------+
;| THIS DRIVE HAS INTEND TO USE WITH AVRS WITH SRAM	|
;+------------------------------------------------------+
;>------------------------------------------------------<
;| I M P L E M E N T E D  F U N C T I O N S 		|
;+------------------------------------------------------+---------------------------------------+
;| _DTCOMMV1_SLAVE_INIT	INITIALIZE COMM HOW SLAVE (ONLY ANSWER TX)				|
;|												|
;|.EQU	_DTCOMMV1_PORT_OUTPUT	= PORTD		;this sample use a portd to send data		|
;|.EQU	_DTCOMMV1_PORT_DIR	= DDRD		;and bit 2 INT0 as data bit			|
;|.EQU	_DTCOMMV1_PORT_INPUT	= PIND								|
;|												|
;|.EQU	_DTCOMMV1_DATA_BIT	= 2		;must be a INT0 pin				|
;|												|
;|												|
;|			RCALL	_DTCOMMV1_SLAVE_INIT	;port initialization			|
;+------------------------------------------------------+---------------------------------------+
;| _DTCOMMV1_MASTER_INIT INITIALIZE COMM HOW MASTER (ONLY REQUEST TX)				|
;|												|
;|.EQU	_DTCOMMV1_PORT_OUTPUT	= PORTD		;this sample use a portd to send data		|
;|.EQU	_DTCOMMV1_PORT_DIR	= DDRD		;and bit 2 INT0 as data bit			|
;|.EQU	_DTCOMMV1_PORT_INPUT	= PIND								|
;|												|
;|.EQU	_DTCOMMV1_DATA_BIT	= 2		;must be a INT0 pin				|
;|												|
;|												|
;|			RCALL	_DTCOMMV1_MASTER_INIT	;port initialization			|
;+----------------------------------------------------------------------------------------------+
;| _DTCOMMV1_SET_DATA	SET DATA THAT MASTER REQUEST						|
;|												|
;|			LDI	ACC,LOW(1234)		;set data 				|
;|			LDI	ACCH,HIGH(1234)							|
;|			RCALL	_DTCOMMV1_SET_DATA						|
;+----------------------------------------------------------------------------------------------+
;| _DTCOMMV1_GET_DATA	GET DATA FROM SLAVE							|
;|												|
;|			RCALL	_DTCOMMV1_GET_DATA	;after this point buffer is empty state |
;+----------------------------------------------------------------------------------------------+
;| _DTCOMMV1_DATA_EMPTY	SEE IF DATA BUFFER IS EMPTY						|
;|												|
;|			RCALL	_DTCOMMV1_DATA_EMPTY	;cy=1 if empty				|
;+----------------------------------------------------------------------------------------------+
;| _DTCOMMV1_REQUEST	REQUEST DATA FROM SLAVE AND RETURN VALUE INTO ACCH:ACC			|
;|												|
;|			RCALL	_DTCOMMV1_REQUEST	;after this point cy=1 if an error occur|
;|							;during transmission, use 		|
;|							;_DTCOMMV1_ERROR se see error status	|
;+----------------------------------------------------------------------------------------------+-------+
;| _DTCOMMV1_ERROR	RETURN ERROR DURING DATA RECEPTION						|
;|													|
;|			RCALL	_DTCOMMV1_ERROR		;after this point Acc=0 if no  error    	|
;|							;else check each of following mask		|
;|							;_DTCOMMV1_STATUS_TIMEOUT if time out error	|
;|							;_DTCOMMV1_STATUS_STARTBIT if invalid startbit	|
;|							;_DTCOMMV1_STATUS_PARITY if invalid parity	|
;+------------------------------------------------------------------------------------------------------+
;+----------------------------------------------------------------------------------------------------------------------+
;| DTCOMMV1 PROTOCOL DESCRIPTION											|
;| THIS PROTOCOL AS INTEND MAKE COMMUNICATION UNILATERAL FROM SLAVE TO MASTER WHERE MASTER 				|
;| REQUEST TRANSMISSION AND SLAVE RETURN DATA ALL DATA TRANSFERED ACCORDING BELOW FLOW CHART				|
;|															|
;|															|
;|		|tr |		|bt |											|
;|  DATA -------+   +-----------+   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+------	|
;|		|   |		| 0 |D0 |D1 |D2 |D3 |D4 |D5 |D6 |D7 |D8 |D9 |D10|D11|D12|D13|D14|D15|P  | 1 |		|
;|		+---+		+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+		|
;|		    |.....rt....|											|
;|		^													|
;|		|													|
;|		Request comm												|
;|															|
;| tr = 4 us														|
;| bt = 8 us														|
;| rt = 														|
;| DO..D15 data bits													|
;| P odd parity														|
;|															|
;| THIS DRIVE AS INTEND FOR USE WITH AVR WITH CRYSTAL >=3MHZ								|
;| THE DATA BIT MUST BE A EXTERNAL INTERRUPT PIN									|
;| USE EXTERNAL 0 INTERRUPT REQUEST											|
;+----------------------------------------------------------------------------------------------------------------------+
;+--------------------------------------+
;| CONSTANTS AND VARIABLES DEFINITIONS	|
;+--------------------------------------+
.EQU	_DTCOMMV1_STATUS_EMPTY		= 0b00000001
.EQU	_DTCOMMV1_STATUS_TIMEOUT	= 0b00000010
.EQU	_DTCOMMV1_STATUS_STARTBIT	= 0b00000100
.EQU	_DTCOMMV1_STATUS_PARITY		= 0b00001000
.DSEG
_DTCOMMV1_DATA:		.BYTE 2			;data to be send
_DTCOMMV1_STATUS:	.BYTE 1			;bit 0=1 DATA buffer empty
						;bit 1=1 time out
						;bit 2=1 start bit wrong
						;bit 3=1 parity error
.CSEG
;   |  |  |  |  |  |  |  |  |  |
;+--+--+--+--+--+--+--+--+--+--+--+
;|   DTCOMMV1 SLAVE INITIALIZE 	  |
;|  				  |
;|\  Input : None	          |
;| | Output: None		  |
;|/  Destroy:None		  |
;|   				  |
;|   			       	  |
;+--+--+--+--+--+--+--+--+--+--+--+ 
;   |  |  |  |  |  |  |  |  |  |
;   \__\__\__\__\__\__\__\__\__\_________________________________________________________________________
_DTCOMMV1_SLAVE_INIT:
	push	Acc					;save Acc
	in	Acc,SREG				;save SREG
	cli						;disable interrupts
	push	Acc
	sbi	_DTCOMMV1_PORT_OUTPUT,_DTCOMMV1_DATA_BIT	;line as input with pull up
	cbi	_DTCOMMV1_PORT_DIR,_DTCOMMV1_DATA_BIT
;+-----------------------------
;| set external interrupt handdle process at
;| falling interrupt generation
;+-----------------------------
	in	Acc,MCUCR				;falling interrupt
	andi	Acc,~(1<<ISC00)
	ori	Acc,(1<<ISC01)
	out	MCUCR,Acc
	in	Acc,GIMSK				;enable external interrrupt 0
	ori	Acc,(1<<INT0)
	out	GIMSK,Acc
	_SET_HANDDLE	_HDC_INT0_VECT,_DTCOMMV1_ANSWER	;attach interrupt handdle process
_DTCOMMV1_SL00:	
	ldi	Acc,_DTCOMMV1_STATUS_EMPTY		;set data buffer empty
	sts	_DTCOMMV1_STATUS,Acc	
	pop	Acc
	out	SREG,Acc				;restore SREG
	pop	Acc					;restore Acc
	ret	
;   |  |  |  |  |  |  |  |  |  |
;+--+--+--+--+--+--+--+--+--+--+--+
;|   DTCOMMV1 MASTER INITIALIZE   |
;|  				  |
;|\  Input : None	          |
;| | Output: None		  |
;|/  Destroy:None		  |
;|   				  |
;|   			       	  |
;+--+--+--+--+--+--+--+--+--+--+--+ 
;   |  |  |  |  |  |  |  |  |  |
;   \__\__\__\__\__\__\__\__\__\_________________________________________________________________________
_DTCOMMV1_MASTER_INIT:
	sbi	portb,0
	sbi	ddrb,0
	
	push	Acc					;save Acc
	in	Acc,SREG				;save SREG
	cli						;disable interrupts
	push	Acc
	sbi	_DTCOMMV1_PORT_OUTPUT,_DTCOMMV1_DATA_BIT	;line as input with pull up
	cbi	_DTCOMMV1_PORT_DIR,_DTCOMMV1_DATA_BIT
	rjmp	_DTCOMMV1_SL00
	
;   |  |  |  |  |  |  |  |  |  |
;+--+--+--+--+--+--+--+--+--+--+--+
;|   DTCOMMV1 SET DATA 		  |
;|  				  |
;|\  Input : AccH:Acc data        |
;| | Output: None		  |
;|/  Destroy:None		  |
;|   				  |
;|   			       	  |
;+--+--+--+--+--+--+--+--+--+--+--+ 
;   |  |  |  |  |  |  |  |  |  |
;   \__\__\__\__\__\__\__\__\__\_________________________________________________________________________
_DTCOMMV1_SET_DATA:
	push	AccT
	push	Acc
	in	AccT,SREG				;save SREG
	cli						;to corectly read
	sts	_DTCOMMV1_DATA+0,Acc
	sts	_DTCOMMV1_DATA+1,AccH
	lds	Acc,_DTCOMMV1_STATUS			;set buffer no empty
	andi	Acc,~(_DTCOMMV1_STATUS_EMPTY)
	sts	_DTCOMMV1_STATUS,Acc
	out	SREG,AccT				;restore SREG
	pop	Acc
	pop	AccT
	ret
;   |  |  |  |  |  |  |  |  |  |
;+--+--+--+--+--+--+--+--+--+--+--+
;|   DTCOMMV1 GET DATA 		  |
;|  				  |
;|\  Input : None		  |
;| | Output: AccH:Acc data	  |
;|/  Destroy:None		  |
;|   				  |
;|   			       	  |
;+--+--+--+--+--+--+--+--+--+--+--+ 
;   |  |  |  |  |  |  |  |  |  |
;   \__\__\__\__\__\__\__\__\__\_________________________________________________________________________
_DTCOMMV1_GET_DATA:
	push	AccT
	rcall	_DTCOMMV1_REQUEST			;request data
	in	AccT,SREG
	lds	Acc,_DTCOMMV1_STATUS			;set buffer empty
	ori	Acc,(_DTCOMMV1_STATUS_EMPTY)
	sts	_DTCOMMV1_STATUS,Acc
	lds	Acc,_DTCOMMV1_DATA+0
	lds	AccH,_DTCOMMV1_DATA+1
	out	SREG,AccT
	pop	AccT
	ret
;   |  |  |  |  |  |  |  |  |  |
;+--+--+--+--+--+--+--+--+--+--+--+
;|   DTCOMMV1 DATA EMPTY ?	  |
;|  				  |
;|\  Input : None		  |
;| | Output: cy=1 if empty	  |
;|/  Destroy:None		  |
;|   				  |
;|   			       	  |
;+--+--+--+--+--+--+--+--+--+--+--+ 
;   |  |  |  |  |  |  |  |  |  |
;   \__\__\__\__\__\__\__\__\__\_________________________________________________________________________
_DTCOMMV1_DATA_EMPTY:
	lds	Acc,_DTCOMMV1_STATUS			;set buffer no empty
	andi	Acc,~(_DTCOMMV1_STATUS_EMPTY)
	clc						;assume empty
	breq	_DTCOMMV1_DATA00			;yes, branch	
	sec						;else data empty
_DTCOMMV1_DATA00:	
	ret
;   |  |  |  |  |  |  |  |  |  |
;+--+--+--+--+--+--+--+--+--+--+--+
;|   DTCOMMV1 ANSWER TO MASTER	  |
;|  				  |
;|\  Input : None	          |
;| | Output: None		  |
;|/  Destroy:None		  |
;|   				  |
;|   			       	  |
;+--+--+--+--+--+--+--+--+--+--+--+ 
;   |  |  |  |  |  |  |  |  |  |
;   \__\__\__\__\__\__\__\__\__\_________________________________________________________________________
_DTCOMMV1_ANSWER:
	push	Acc					;save useds
	in	Acc,SREG				;save SREG
	push	Acc
	push	AccH
	ldi	Acc,_AVR_CLOCK/250000			;computation for 16us
	nop
	dec	Acc
	brne	PC-2
	lds	Acc,_DTCOMMV1_DATA+0			;get data and tx
	lds	AccH,_DTCOMMV1_DATA+1
	rcall	_DTCOMMV1_TX				;answer request
	lds	Acc,_DTCOMMV1_STATUS			;set buffer empty
	ori	Acc,(_DTCOMMV1_STATUS_EMPTY)
	sts	_DTCOMMV1_STATUS,Acc
	in	Acc,GIFR				;clear pending interrupts
	ori	Acc,(1<<INTF0)
	out	GIFR,Acc
	pop	AccH
	pop	Acc
	out	SREG,Acc				;restore SREG
	pop	Acc					;restore Acc
	ret
;   |  |  |  |  |  |  |  |  |  |
;+--+--+--+--+--+--+--+--+--+--+--+
;|   DTCOMMV1 REQUEST COMM	  |
;|  				  |
;|\  Input : None	          |
;| | Output: AccH:Acc data	  |
;| |	     cy=1 if slave no answer |
;| |	     or start bit invalid |
;| | 	     or parity error	  |
;|/  Destroy:None		  |
;|   				  |
;|   			       	  |
;+--+--+--+--+--+--+--+--+--+--+--+ 
;   |  |  |  |  |  |  |  |  |  |
;   \__\__\__\__\__\__\__\__\__\_________________________________________________________________________
_DTCOMMV1_REQUEST:
	push	Acc
	in	Acc,SREG					;read sreg
	push	Acc
	push	AccH
	push	Temp
	push	AccT
	lds	Acc,_DTCOMMV1_STATUS				;clear error status bits
	andi	Acc,~((_DTCOMMV1_STATUS_TIMEOUT)|(_DTCOMMV1_STATUS_STARTBIT)|(_DTCOMMV1_STATUS_PARITY))
	sts	_DTCOMMV1_STATUS,Acc
	cli							;no interrupt allowed
	cbi	_DTCOMMV1_PORT_OUTPUT,_DTCOMMV1_DATA_BIT	;Put Data line low for 8 us
	sbi	_DTCOMMV1_PORT_DIR,_DTCOMMV1_DATA_BIT
	ldi	Acc,_AVR_CLOCK/500000				;computation for 8us
	nop
	dec	Acc
	brne	PC-2
	sbi	_DTCOMMV1_PORT_OUTPUT,_DTCOMMV1_DATA_BIT	;release line
	cbi	_DTCOMMV1_PORT_DIR,_DTCOMMV1_DATA_BIT		;return line to input with pull up
	ldi	Acc,-1						;set time out to slave respond
	ldi	AccH,-1
_DTCOMMV1_REQ00:
	subi	Acc,1
	sbci	AccH,0
	brcs	_DTCOMMV1_REQ_TIMEOUT				;time out set cy
	sbic	_DTCOMMV1_PORT_INPUT,_DTCOMMV1_DATA_BIT		;slave answer ?
	rjmp	_DTCOMMV1_REQ00					;no, wait
	ldi	Acc,_AVR_CLOCK/1000000				;computation for 4us middle of start bit
	nop
	dec	Acc
	brne	PC-2
	sbic	_DTCOMMV1_PORT_INPUT,_DTCOMMV1_DATA_BIT		;really start bit ?
	rjmp	_DTCOMMV1_REQ_TIMEOUT				;no, set error
	ldi	Temp,17						;number of bits to receiver(16 data)+(1 parity)
	clr	AccT						;parity counter
_DTCOMMV1_REQ10:
	push	Acc
	ldi	Acc,_AVR_CLOCK/500000-5				;computation for 8us middle of D0
	nop
	dec	Acc
	brne	PC-2
	pop	Acc
	clc							;data=0
	sbic	_DTCOMMV1_PORT_INPUT,_DTCOMMV1_DATA_BIT		;get bit
	sec							;else data=1
	brcc	_DTCOMMV1_REQ20					;only ones are summed
	inc	AccT						;increment parity counter
_DTCOMMV1_REQ20:
	ror	AccH						;transfer it to AccH:Acc
	ror	Acc
	nop
	nop
	nop
	nop
	nop
	nop
	dec	Temp						;next bit
	brne	_DTCOMMV1_REQ10
	sbrs	AccT,0						;skip if parity odd
	rjmp	_DTCOMMV1_REQ_TIMEOUT				;else error
	rol	Acc						;put data correctly extracting parity bit
	rol	AccH
	sts	_DTCOMMV1_DATA+0,Acc				;store new data
	sts	_DTCOMMV1_DATA+1,AccH
	lds	Acc,_DTCOMMV1_STATUS				;set buffer no empty
	andi	Acc,~(_DTCOMMV1_STATUS_EMPTY)
	sts	_DTCOMMV1_STATUS,Acc
;+-----------------------
;| EXIT WITH ERROR
;+-----------------------
	pop	AccT
	pop	Temp
	pop	AccH
	pop	Acc
	out	SREG,Acc					;restore SREG
	pop	Acc
	clc							;set ok
	ret
;+-----------------------
;| SET TIME OUT ERROR
;+-----------------------
_DTCOMMV1_REQ_TIMEOUT:
	lds	Acc,_DTCOMMV1_STATUS
	ori	Acc,_DTCOMMV1_STATUS_TIMEOUT
	rjmp	_DTCOMMV1_REQ_ERROR_EXIT
;+-----------------------
;| SET START BIT  ERROR
;+-----------------------
_DTCOMMV1_REQ_STARTBIT:
	lds	Acc,_DTCOMMV1_STATUS
	ori	Acc,_DTCOMMV1_STATUS_STARTBIT
	rjmp	_DTCOMMV1_REQ_ERROR_EXIT
;+-----------------------
;| SET PARITY ERROR
;+-----------------------
_DTCOMMV1_REQ_PARITY:
	lds	Acc,_DTCOMMV1_STATUS
	ori	Acc,_DTCOMMV1_STATUS_PARITY
;+-----------------------
;| EXIT WITH ERROR
;+-----------------------
_DTCOMMV1_REQ_ERROR_EXIT:
	sts	_DTCOMMV1_STATUS,Acc
	pop	AccT
	pop	Temp
	pop	AccH
	pop	Acc						;restore SREG
	out	SREG,Acc
	sec		
	pop	Acc						;set time out flag
	ret
;   |  |  |  |  |  |  |  |  |  |
;+--+--+--+--+--+--+--+--+--+--+--+
;|   DTCOMMV1 GET ERROR		  |
;|  				  |
;|\  Input : None		  |
;| | Output: Acc Error bits with  |
;| |	     same position of     |
;| |	     STATUS or zero if no |
;| |	     error		  |
;|/  Destroy:None		  |
;|   	     			  |
;|	     			  |
;|   			       	  |
;+--+--+--+--+--+--+--+--+--+--+--+ 
;   |  |  |  |  |  |  |  |  |  |
;   \__\__\__\__\__\__\__\__\__\_________________________________________________________________________
_DTCOMMV1_ERROR:
	lds	Acc,_DTCOMMV1_STATUS
	andi	Acc,~(_DTCOMMV1_STATUS_EMPTY)		;no empyt status to return
	ret
	
;   |  |  |  |  |  |  |  |  |  |
;+--+--+--+--+--+--+--+--+--+--+--+
;|   DTCOMMV1 TX DATA		  |
;|  				  |
;|\  Input : AccH:Acc Data        |
;| | Output: None	  	  |
;|/  Destroy:None		  |
;|   				  |
;|   			       	  |
;+--+--+--+--+--+--+--+--+--+--+--+ 
;   |  |  |  |  |  |  |  |  |  |
;   \__\__\__\__\__\__\__\__\__\_________________________________________________________________________
;				| obs: interrupt are disable during transmission
;				\________________________________________________________________________
_DTCOMMV1_TX:
	push	r0
	push	Acc						;save useds
	push	AccH
	push	AccT
	push	AccTH
	push	Temp
	in	r0,SREG						;save SREG
	cli							;no interrupt allowed during transmission	
	sbi	_DTCOMMV1_PORT_OUTPUT,_DTCOMMV1_DATA_BIT	;line as output
	sbi	_DTCOMMV1_PORT_DIR,_DTCOMMV1_DATA_BIT
	ldi	Temp,16						;bit count
	clr	AccT						;clear parity count
	clc							;send start bit=0
	rcall	_DTCOMMV1_TX_BIT
	nop							;to perfect tx time
	nop
	nop
_DTCOMMV1_TX00:
	lsr	AccH						;get bit to tx
	ror	Acc
	rcall	_DTCOMMV1_TX_BIT				;send a bit
	dec	Temp						;next bit
	brne	_DTCOMMV1_TX00
	clc							;set parity bit
	sbrs	AccT,0						;skip if parity is odd
	sec							;else turn odd
	rcall	_DTCOMMV1_TX_BIT				;send parity bit
	nop							;to perfect tx time
	nop
	nop
	nop
	sec	
	rcall	_DTCOMMV1_TX_BIT				;send stop bit
	sbi	_DTCOMMV1_PORT_OUTPUT,_DTCOMMV1_DATA_BIT	;turn line as input with pull up
	cbi	_DTCOMMV1_PORT_DIR,_DTCOMMV1_DATA_BIT
	out	SREG,r0						;restore SREG
	pop	Temp
	pop	AccTH						;restore useds
	pop	AccT	
	pop	AccH
	pop	Acc
	pop	r0
	ret
	
;   |  |  |  |  |  |  |  |  |  |
;+--+--+--+--+--+--+--+--+--+--+--+
;|   DTCOMMV1 TX BIT		  |
;|  				  |
;|\  Input : cy bit to send       |
;| | Output: None	  	  |
;|/  Destroy:None		  |
;|   				  |
;|   			       	  |
;+--+--+--+--+--+--+--+--+--+--+--+ 
;   |  |  |  |  |  |  |  |  |  |
;   \__\__\__\__\__\__\__\__\__\_________________________________________________________________________
;				| this function increment AccT to hold a parity count odd
;				| TX_BIT equation time is
;				|
;				|	7+4N-1+2+4+3rcall+5(tx loop)
;				|
;				| in simple form
;				|
;				|	T=(4N+20)/F	where T=0.000008=8us
;				|
;				| put into integer formula and resolve in N
;				|
;				|	N=_AVR_CLOCK/500000-5
;				\________________________________________________________________________
_DTCOMMV1_TX_BIT:
	brcc	_DTCOMMV1_TX_B00				
	sbi	_DTCOMMV1_PORT_OUTPUT,_DTCOMMV1_DATA_BIT	;assume bit 1
	inc	AccT						;increment parity count
	rjmp	_DTCOMMV1_TX_B10
_DTCOMMV1_TX_B00:
	cbi	_DTCOMMV1_PORT_OUTPUT,_DTCOMMV1_DATA_BIT	;assume bit 0
	nop
	nop
_DTCOMMV1_TX_B10:
	ldi	AccTH,_AVR_CLOCK/500000-5			;computation for 8us
	nop
	dec	AccTH
	brne	PC-2
	nop
	nop
	ret	

