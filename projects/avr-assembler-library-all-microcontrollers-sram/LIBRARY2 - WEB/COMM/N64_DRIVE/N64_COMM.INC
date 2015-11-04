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
;| DATA:19/04/2002					|
;>------------------------------------------------------<
;| N 6 4  J O Y S T I C K   P R O T O C O L  		|
;+------------------------------------------------------+
;| THIS DRIVE HAS INTEND TO USE WITH AVRS WITH SRAM	|
;| AND RUNNING ONLY 4MHZ CRISTAL 			|
;+------------------------------------------------------+
;>------------------------------------------------------<
;| I M P L E M E N T E D  F U N C T I O N S 		|
;+------------------------------------------------------+---------------------------------------+
;| _N64_INIT		INITIALIZE N64 LINE							|
;|												|
;|.EQU	_N64_DATAINOUT_OUT=PORTB	;N64 OUTPUT PORT					|
;|.EQU	_N64_DATAINOUT_DIR=DDRB		;N64 DIRECTION PORT					|
;|.EQU	_N64_DATAINOUT_IN =PINB		;N64 INPUT PORT						|
;|												|
;|.EQU	_N64_BIT_DATAINOUT=0		;N64 DATAINOUT BIT					|
;|												|
;|			RCALL	_N64_INIT	;AFTER THIS POINT ALL N64 ROUTINES WORKING 	|
;|						;AT PORTB					|
;|----------------------------------------------------------------------------------------------|
;| _N64_STATUS		READ ALL STATUS KEYS OF JOYSTICK PAD					|
;|												|
;|			RCALL	_N64_STATUS	;AFTER THIS COMMAND YOU CAN READ ALL KEY AND 	|
;|						;LEVER POSITION					|
;|----------------------------------------------------------------------------------------------|
;| _N64_GET_XY		RETURN X,Y JOYSTICK LEVER POSITION					|
;|												|
;|			RCALL	_N64_GET_XY	;AFTER THIS COMMAND Acc=X position 		|
;|						;AccH=Y position				|
;|												|
;| obs: The user must be call _N64_STATUS before to update lever position status		|
;+----------------------------------------------------------------------------------------------+
#ifndef __N64_DRIVE__
	#define __N64_DRIVE__
	
	.message "+---------------+"
	.message "|   N64 DRIVE   |" 
	.message "+---------------+"
	;+------------------------------+
	;| KEY DEFINITIONS CONSTANTS	|
	;+------------------------------+
	.EQU	_N64_KEY_NONE		= 0
	.EQU	_N64_KEY_A		= 1
	.EQU	_N64_KEY_B		= 2
	.EQU	_N64_KEY_Z		= 3
	.EQU	_N64_KEY_START		= 4
	.EQU	_N64_KEY_DIR_UP		= 5
	.EQU	_N64_KEY_DIR_DOWN	= 6
	.EQU	_N64_KEY_DIR_LEFT	= 7
	.EQU	_N64_KEY_DIR_RIGHT	= 8
	.EQU	_N64_KEY_UNUSED1	= 9
	.EQU	_N64_KEY_UNUSED2	= 10
	.EQU	_N64_KEY_L		= 11
	.EQU	_N64_KEY_R		= 12
	.EQU	_N64_KEY_C_UP		= 13
	.EQU	_N64_KEY_C_DOWN		= 14
	.EQU	_N64_KEY_C_LEFT		= 15
	.EQU	_N64_KEY_C_RIGHT	= 16
	;   |  |  |  |  |  |  |  |  |  |
	;+--+--+--+--+--+--+--+--+--+--+--+
	;|   N64 INITIALIZE		  |
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
	;				| Obs:  After this initialization DATAINOUT is configured as input
	;			        |       to prevent short circuit during initialization 
	;				\_______________________________________________________________________
	_N64_INIT:
		push	Acc
		in	Acc,SREG					;Save SREG state
		cli							;to handdling ports correctly
		cbi	_N64_DATAINOUT_OUT,_N64_BIT_DATAINOUT		;set DATAINOUT as high impedance
		cbi	_N64_DATAINOUT_DIR,_N64_BIT_DATAINOUT		;set DATAINOUT as input
		out	SREG,Acc					;Restore SREG state
		pop	Acc
		ret
	;   |  |  |  |  |  |  |  |  |  |
	;+--+--+--+--+--+--+--+--+--+--+--+
	;|   N64_STATUS			  |
	;|   				  |
	;|\			          |
	;| | Input  : None	          |
	;|/  Output : None	          |
	;|   Destroy: NOne	          |
	;|   			       	  |
	;+--+--+--+--+--+--+--+--+--+--+--+ 
	;   |  |  |  |  |  |  |  |  |  |
	;   \__\__\__\__\__\__\__\__\__\________________________________________________________________________
	;				/
	;				| Timing
	;			        |
	;				| bit format 
	;				|
	;				| Bit 0	.....  -+	     		+--------
	;				|		|	     		|
	;				|		+-----------------------+	 
	;				|		 \-------- 3us --------/ \- 1us-/
	;				|
	;				| Bit 1	.....  -+   	 +-----------------------
	;				|		|   	 |
	;				|		+--------+......
	;				|		 \- 1us-/ \--------- 3us -------/
	;				|
	;				| Command format
	;				|
	;				|	        +--+--+--+--+--+--+--+--+
	;				|		|d0|d1|d2|d3|d4|d5|d7|sp|
	;				|	        +--+--+--+--+--+--+--+--+
	;				| d0..d7 = data bits
	;				| sp	 = stop bit always 1(one)
	;				|
	;				| Protocol format
	;				|			  
	;				| send this byte to return buttons state
	;				|
	;				| +--+--+--+--+--+--+--+--+
	;				| | 0| 0| 0| 0| 0| 0| 1|sp| hexa 0x01
	;				| +--+--+--+--+--+--+--+--+
	;				|
	;				| after 2~3us delay joystick return then folowing sequence
	;				|
	;				| +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
	;				| | A| B| Z|ST|DU|DD|DL|DR|U1|U2| L| R|CU|CD|CL|CD|X0|X1|X2|X3|X4|X5|X6|X7|Y0|Y1|Y2|Y3|Y4|Y5|Y6|Y7|sp|
	;				| +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
	;				|
	;				| all bits state coded as follow 
	;				|
	;				| Key pressed=1 Key release=0
	;				|
	;				| ST Start key
	;				| DU Directional UP
	;				| DD Directional DOWN
	;				| DL Directional LEFT
	;				| DR Directional RIGHT
	;				| CU C UP
	;				| CD C DOWN
	;				| CL C LEFT
	;				| CR C RIGHT
	;				| X0..X7 Signed horizontal lever movement left negative right positive
	;				| Y0..Y7 Signed vertical lever movement down negative up positive
	;				\_______________________________________________________________________
	.DSEG
	_N64_STATUS0:	.BYTE	1
	_N64_STATUS1:	.BYTE	1
	_N64_STATUS2:	.BYTE	1
	_N64_STATUS3:	.BYTE	1
	.CSEG
	;+----------------
	;| First set DATAINOUT as output 
	;+----------------
	_N64_STATUS:
		push	r0						;save used registers
		push	r1
		push	r2
		push	r3
		push	Acc
		sbi	_N64_DATAINOUT_OUT,_N64_BIT_DATAINOUT		;set data out 1
		sbi	_N64_DATAINOUT_DIR,_N64_BIT_DATAINOUT		;set data as output
	;+----------------
	;| Send a start protocol
	;+----------------
		cbi	_N64_DATAINOUT_OUT,_N64_BIT_DATAINOUT		;send 0
		rcall	_N64D
		sbi	_N64_DATAINOUT_OUT,_N64_BIT_DATAINOUT
		nop
		nop
	;---------------------------------------------------------------		
		cbi	_N64_DATAINOUT_OUT,_N64_BIT_DATAINOUT		;send 0
		rcall	_N64D
		sbi	_N64_DATAINOUT_OUT,_N64_BIT_DATAINOUT
		nop
		nop
	;---------------------------------------------------------------		
		cbi	_N64_DATAINOUT_OUT,_N64_BIT_DATAINOUT		;send 0
		rcall	_N64D
		sbi	_N64_DATAINOUT_OUT,_N64_BIT_DATAINOUT
		nop
		nop
	;---------------------------------------------------------------		
		cbi	_N64_DATAINOUT_OUT,_N64_BIT_DATAINOUT		;send 0
		rcall	_N64D
		sbi	_N64_DATAINOUT_OUT,_N64_BIT_DATAINOUT
		nop
		nop
	;---------------------------------------------------------------		
		cbi	_N64_DATAINOUT_OUT,_N64_BIT_DATAINOUT		;send 0
		rcall	_N64D
		sbi	_N64_DATAINOUT_OUT,_N64_BIT_DATAINOUT
		nop
		nop
	;---------------------------------------------------------------		
		cbi	_N64_DATAINOUT_OUT,_N64_BIT_DATAINOUT		;send 0
		rcall	_N64D
		sbi	_N64_DATAINOUT_OUT,_N64_BIT_DATAINOUT
		nop
		nop
	;---------------------------------------------------------------		
		cbi	_N64_DATAINOUT_OUT,_N64_BIT_DATAINOUT		;send 0
		rcall	_N64D
		sbi	_N64_DATAINOUT_OUT,_N64_BIT_DATAINOUT
		nop
		nop
	;---------------------------------------------------------------		
		cbi	_N64_DATAINOUT_OUT,_N64_BIT_DATAINOUT		;send 1
		nop
		nop
		sbi	_N64_DATAINOUT_OUT,_N64_BIT_DATAINOUT
		rcall	_N64D
	;---------------------------------------------------------------		
		cbi	_N64_DATAINOUT_OUT,_N64_BIT_DATAINOUT		;send stop bit 1
		nop
		nop
		sbi	_N64_DATAINOUT_OUT,_N64_BIT_DATAINOUT
	;---------------------------------------------------------------
		cbi	_N64_DATAINOUT_DIR,_N64_BIT_DATAINOUT		;set DATAINOUT as input to get status bits
		nop
		
	;+-------------------------------
	;| Wait return of status bits
	;+-------------------------------
	_N64_S00:
		sbic	_N64_DATAINOUT_IN,_N64_BIT_DATAINOUT		;wait a start bit	
		rjmp	_N64_S00
		nop
		ldi	Acc,32						;set number of bits to be read	
	_N64_S10:
		clc							;assume bit=0
		sbic	_N64_DATAINOUT_IN,_N64_BIT_DATAINOUT
		sec							;else bit=1
		rol	r0						;transfer this bit to r0:r1:r2:r3
		rol	r1
		rol	r2
		rol	r3	
		nop							;to math 4us period
		nop
		nop
		nop
		nop
		nop
		dec	Acc						;next bit
		brne	_N64_S10					;until all bits read
		sts	_N64_STATUS0,r0					;store read bits
		sts	_N64_STATUS1,r1	
		sts	_N64_STATUS2,r2
		sts	_N64_STATUS3,r3
		pop	Acc						;restore useds
		pop	r3
		pop	r2
		pop	r1
		pop	r0
		ret
	;+------------------------
	;| Delay to complete 3Us 
	;+------------------------
	_N64D:
		nop
		nop
		nop
		ret
	;   |  |  |  |  |  |  |  |  |  |
	;+--+--+--+--+--+--+--+--+--+--+--+
	;|   N64_GET_A			  |
	;|   				  |
	;|\			          |
	;| | Input  : Acc Key code        |
	;|/  Output : Acc ON or OFF	  |
	;|   Destroy: None		  |
	;|   			       	  |
	;+--+--+--+--+--+--+--+--+--+--+--+ 
	;   |  |  |  |  |  |  |  |  |  |
	;   \__\__\__\__\__\__\__\__\__\________________________________________________________________________
	_N64_GET_KEY:
		push	r0
		push	r1
		push	AccH
		lds	r0,_N64_STATUS3
		lds	r1,_N64_STATUS2
		ldi	AccH,0b10000000
		cpi	Acc,_N64_KEY_A
		breq	_N64_GK0
		lsr	AccH
		cpi	Acc,_N64_KEY_B
		breq	_N64_GK0
		lsr	AccH
		cpi	Acc,_N64_KEY_Z
		breq	_N64_GK0
		lsr	AccH
		cpi	Acc,_N64_KEY_START
		breq	_N64_GK0
		lsr	AccH
		cpi	Acc,_N64_KEY_DIR_UP
		breq	_N64_GK0
		lsr	AccH
		cpi	Acc,_N64_KEY_DIR_DOWN
		breq	_N64_GK0
		lsr	AccH
		cpi	Acc,_N64_KEY_DIR_LEFT
		breq	_N64_GK0
		lsr	AccH
		cpi	Acc,_N64_KEY_DIR_RIGHT
		breq	_N64_GK0
		ldi	AccH,0b00100000
		cpi	Acc,_N64_KEY_L
		breq	_N64_GK1
		lsr	AccH
		cpi	Acc,_N64_KEY_R
		breq	_N64_GK1
		lsr	AccH
		cpi	Acc,_N64_KEY_C_UP
		breq	_N64_GK1
		lsr	AccH
		cpi	Acc,_N64_KEY_C_DOWN
		breq	_N64_GK1
		lsr	AccH
		cpi	Acc,_N64_KEY_C_LEFT
		breq	_N64_GK1
		lsr	AccH
		cpi	Acc,_N64_KEY_C_RIGHT
		breq	_N64_GK1
	_N64_GK0:
		ldi	Acc,_OFF
		and	r0,AccH
		breq	_N64_GK0_00	
		ldi	Acc,_ON
	_N64_GK0_00:
		rjmp	_N64_GK_EXIT
	_N64_GK1:
		ldi	Acc,_OFF
		and	r1,AccH
		breq	_N64_GK_EXIT	
		ldi	Acc,_ON
	_N64_GK_EXIT:
		pop	AccH
		pop	r1
		pop	r0
		ret
		
	;   |  |  |  |  |  |  |  |  |  |
	;+--+--+--+--+--+--+--+--+--+--+--+
	;|   N64_GET_X			  |
	;|   				  |
	;|\			          |
	;| | Input  : None	          |
	;|/  Output : Acc X		  |
	;|   Destroy: None		  |
	;|   			       	  |
	;+--+--+--+--+--+--+--+--+--+--+--+ 
	;   |  |  |  |  |  |  |  |  |  |
	;   \__\__\__\__\__\__\__\__\__\________________________________________________________________________
	_N64_GET_X:
		lds	Acc,_N64_STATUS1				;get X lever value
		ret
	;   |  |  |  |  |  |  |  |  |  |
	;+--+--+--+--+--+--+--+--+--+--+--+
	;|   N64_GET_Y			  |
	;|   				  |
	;|\			          |
	;| | Input  : None	          |
	;|/  Output : Acc Y		  |
	;|   Destroy: None		  |
	;|   			       	  |
	;+--+--+--+--+--+--+--+--+--+--+--+ 
	;   |  |  |  |  |  |  |  |  |  |
	;   \__\__\__\__\__\__\__\__\__\________________________________________________________________________
	_N64_GET_Y:
		lds	Acc,_N64_STATUS0				;get Y lever value
		ret
		
#endif
.exit

