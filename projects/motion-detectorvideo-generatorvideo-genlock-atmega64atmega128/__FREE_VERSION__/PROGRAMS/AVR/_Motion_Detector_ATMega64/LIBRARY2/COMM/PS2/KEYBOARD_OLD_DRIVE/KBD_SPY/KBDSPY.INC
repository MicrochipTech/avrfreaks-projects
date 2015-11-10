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
;| DATA:2/9/2003					|
;>------------------------------------------------------<
;| P S 2  K E Y B O A R D  S P Y	 		|
;+------------------------------------------------------+
;| THIS DRIVE HAS INTEND TO USE WITH AVRS WITH SRAM	|
;+------------------------------------------------------+
;>------------------------------------------------------<
;| I M P L E M E N T E D  F U N C T I O N S 		|
;+------------------------------------------------------+---------------------------------------+
;| _PS2KBD_INIT		INITIALIZE I2C LINE							|
;|												|
;|.EQU	_PS2SPY_PORT_OUTPUT	 = PORTB	;SET PORTB TO SPY				|
;|.EQU	_PS2SPY_PORT_DIR	 = DDRB								|
;|.EQU	_PS2SPY_PORT_INPUT	 = PINB								|
;|												|
;|.EQU  _PS2SPY_DATA_BIT	 = 0		;SET DATA BIT AS 0				|
;|.EQU  _PS2SPY_CLOCK_BIT	 = 1		;SET CLOCK BIT AS 1				|
;|												|
;|			RCALL	_PS2SPY_INIT	;AFTER THIS POINT ALL KEYBOARD SPY ROUTINES 	|
;|						;AS READY					|
;|----------------------------------------------------------------------------------------------|
;| _PS2KBD_READ		READ KEYBOARD LINE							|
;|												|
;|			RCALL	_PS2KBD_READ	;GET ACK STATUS					|
;|						;ACK STATUS INTO C FLAG				|
;+----------------------------------------------------------------------------------------------+
#ifndef __KDBSPY__
	#define __KBDSPY__
	
	.message "+------------+"
	.message "|   KBDSPY   |" 
	.message "+------------+"
	
	;   |  |  |  |  |  |  |  |  |  |
	;+--+--+--+--+--+--+--+--+--+--+--+
	;|   KBD SPY INITIALIZE		  |
	;|  				  |
	;|\			          |
	;| | Input  : None	          |
	;|/  Output : None	          |
	;|   Destroy: None	          |
	;|   			       	  |
	;+--+--+--+--+--+--+--+--+--+--+--+ 
	;   |  |  |  |  |  |  |  |  |  |
	;   \__\__\__\__\__\__\__\__\__\________________________________________________________________________
	_PS2SPY_INIT:
		push	Acc
		in	Acc,SREG					;Save SREG state
		cli							;to handdling ports correctly
		cbi	_PS2SPY_PORT_OUTPUT,_PS2SPY_CLOCK_BIT		;set keyboard clock with high impedance
		cbi	_PS2SPY_PORT_OUTPUT,_PS2SPY_DATA_BIT		;Set keyboard data with high impedance
		cbi	_PS2SPY_PORT_DIR,_PS2SPY_CLOCK_BIT		;Set keyboard clock as input
		cbi	_PS2SPY_PORT_DIR,_PS2SPY_DATA_BIT		;Set keyboard data as input	
		out	SREG,Acc					;Restore SREG state
		pop	Acc
		ret
	;   |  |  |  |  |  |  |  |  |  |  |  |  |
	;+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
	;|   PS2SPY_READ		  	   |
	;|   				  	   |
	;|   Input  : none	          	   |
	;|   Output : Acc data arrived	  	   |
	;|\	      AccH='K' data from KEYBOARD  |
	;| |	      AccH='P' data form PC	   |
	;|/	      AccH='T' time out error  	   |
	;|	      AccH='E' parity error	   |	
	;|   Destroy: Flags,r0,r1,r2,r3        	   |
	;|   obs    : Interrupts disable  	   |
	;|				  	   |
	;+--+--+--+--+--+--+--+--+--+--+--+--+--+--+ 
	;   |  |  |  |  |  |  |  |  |  |  |  |  |
	;   \__\__\__\__\__\__\__\__\__\__\__\__\_______________________________________________________________________________
	;				/ KEYBOARD RECEPTION EVENTS NUMBERS	 						\
	;				|											|
	;				| EVENT NUMBER  -   2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1 					|
	;				| 		    5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0			|
	;				| KEYBOARD CLOCK  ---+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-----------------	|
	;				|		   ^ | | | | | | | | | | | | | | | | | | | | | |			|
	;				|		   | +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ 			|
	;				|		   |									|
	;				| 		   +- RECEIVE CONDITION	(CLOCK HIGH DATA LOW)				|
	;				| KEYBOARD DATA    | +---+---+---+---+---+---+---+---+---+---+				|
	;				|		   v | D0| D1| D2| D3| D4| D5| D6| D7| P | 1 +-------------------	|
	;				|		  ---+---+---+---+---+---+---+---+---+---+---+				|
	;				|		      \------ DATA BITS -------------/	      zero inserted by KEYBOARD |
	;				|       		      Send by computer 	       ^   ^  at clock line (ack)	|
	;				|						       |   |				|
	;				|						       |   +---- STOP BIT		|
	;				|						       +-------- PARITY BIT (ODD)	|
	;				| REMARKS										|
	;				| EVENT NUMBER     STATUS								|
	;				|											|
	;				|          24	    Clear parity counter						|
	;				|		    Program data bit as input						|
	;				|		    Put Clock line low							|
	;				|          23	    get DO clock line high						|
	;				|          21	    get D1 clock line high						|
	;				|          19	    get D2 clock line high						|
	;				|          17	    get D3 clock line high						|
	;				|          15	    get D4 clock line high						|
	;				|          13	    get D5 clock line high						|
	;				|          11	    get D6 clock line high						|
	;				|           9	    get D7 clock line high						|
	;				|           7	    get P clock line high						|
	;				|           5	    get (Stop Bit) clock line high					|
	;				|		    clock line low							|
	;				|           3      Program data bit as output						|
	;				|	     2      process data decode							|
	;				| 24..4 step 2	    Output clock line low						|
	;				|											|
	;				| KEYBOARD TRANSMISITION EVENTS NUMBERS							|
	;				|											|
	;				| EVENT NUMBER -    2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1 					|
	;				| 		    5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0			|
	;				| KEYBOARD CLOCK  --+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +----------------	|
	;				|		    | | | | | | | | | | | | | | | | | | | | | |	| |			|
	;				|		    +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+			|
	;				|											|
	;				| KEYBOARD DATA     +---+---+---+---+---+---+---+---+---+---+---+			|
	;				|		 ---+ 0 | D0| D1| D2| D3| D4| D5| D6| D7| P | 1 +------------------	|
	;				|		    +---+---+---+---+---+---+---+---+---+---+---+			|
	;				|		      ^  \------ DATA BITS -------------/	 zero inserted by PC 	|
	;				|       START BIT ----+				   	   ^   ^ at clock line (ack)	|
	;				|							   |   |			|
	;				|							   |   +---- STOP BIT		|
	;				|							   +-------- PARITY BIT (ODD)	|
	;				| REMARKS										|
	;				| EVENT NUMBER     STATUS								|
	;				|											|
	;				|          24	    Output (Start Bit) 							|
	;				|		    Clear parity counter						|
	;				|		    Programa data bit as output						|
	;				|		    Put Clock line low							|
	;				|          22	    Output DO clock line low						|
	;				|          20	    Output D1 clock line low						|
	;				|          18	    Output D2 clock line low						|
	;				|          16	    Output D3 clock line low						|
	;				|          14	    Output D4 clock line low						|
	;				|          12	    Output D5 clock line low						|
	;				|          10	    Output D6 clock line low						|
	;				|           8	    Output D7 clock line low						|
	;				|           6	    Output P clock line low						|
	;				|           4	    Output (Stop Bit)							|
	;				|		    clock line low							|
	;				|           2      Program data bit as input						|
	;				|											|
	;				| 25..3 step 2	    Output clock line High						|
	;				|											|
	;				| Line status										|
	;				| 											|
	;				| Clock Data	   Status								|
	;				| low   X	   Keyboard communication inhibit					|
	;				| high  low	   PC resquest keyboard comunication					|
	;				| high  high	   Keyboard communication accepted					|
	;				\---------------------------------------------------------------------------------------/
	_PS2SPY_READ:
		ldi	Acc,11					;data counter
		clr	r2					;clear data registers
		clr	r3
	_PS2SPY_READ00:
		rcall	_PS2SPY_LH_CLOCK			;get data
		tst	AccH					;time out ?
		brne	_PS2SPY_TIMEOUT_ERROR			;yes, branch
		ror	r3					;transfer data bit to r1:r0
		ror	r2
		dec	Acc					;data bit counter--
		brne	_PS2SPY_READ00				;until all bits read
		rcall	_PS2SPY_LH_CLOCK			;get data
		tst	AccH					;time out ?
		brne	_PS2SPY_TIMEOUT_ERROR			;yes, branch
		ldi	AccT,low((15*_AVR_CLOCK)/400000)	;wait 150us
		ldi	AccTH,high((15*_AVR_CLOCK)/400000)
	_PS2SPY_READ05:
		subi	AccT,1					;decrement time out counter
		sbci	AccTH,0
		brcc	_PS2SPY_READ05				;branch if time out
	;+--------------------------------------------------------------+
	;| r1:r2 data format according data reception			|
	;|								|
	;|             R3       R2					|
	;| bits --> 76543210 76543210					|
	;| value--> 1p765432 100xxxxx data format when keyboard send	|
	;| value--> 01p76543 210xxxxx data format when pc send		|
	;+--------------------------------------------------------------+
		ldi	AccH,'K'				;assume data form KEYBOARD
	;     	sbrc	r3,7					;skip if possible transfer from PC
	;     	rjmp	_PS2SPY_READ10				;else process transfer from KEYBOARD
	;+-----------------------
	;| process data from PC  
	;+-----------------------
	;	lsl	r2					;put data into appropriate position
	;	rol	r3	
	;	ldi	AccH,'P'				;set data from PC
	;+-----------------------
	;| process data from KEYBOARD  
	;+-----------------------
	_PS2SPY_READ10:
		mov	r0,r3					
		mov	r1,r2
		lsl	r1					;put data into r0
		rol	r0
		lsl	r1
		rol	r0
		rcall	_PARITY_CHECK				;see if valid parity
		brcc	_PS2SPY_READ30				;branch if parity is even
		sbrc	r3,6					;skip if parity bit ok
		rjmp	_PS2SPY_PARITY_ERROR			;else parity error
	_PS2SPY_READ20:
		mov	Acc,r0					;Acc=Data
		ret
	_PS2SPY_READ30:
		sbrc	r3,6					;skip if parity error
		rjmp	_PS2SPY_READ20				;else branch
	_PS2SPY_PARITY_ERROR:
		ldi	AccH,'E'				;set parity error
		ret
	_PS2SPY_TIMEOUT_ERROR:
		ldi	AccH,'T'
		ret
			
	;+--------------------------------------------------+
	;| WAIT KEYBOARD CLOCK LINE TRANSICTION LOW TO HIGH |
	;| Input  : None				    |
	;| Output : Cy data AccH<>0 if time out		    |
	;| Destroy: AccT,AccTH				    |
	;+--------------------------------------------------+	
	_PS2SPY_LH_CLOCK:
		clr	AccH					;set no overflow
		ldi	AccT,low(_AVR_CLOCK/6000)		;set AccTH:AccT with time out constant value to 1ms
		ldi	AccTH,high(_AVR_CLOCK/6000)
	_PS2SPY_LH00:
		subi	AccT,1					;decrement time out counter
		sbci	AccTH,0
		brcs	_PS2SPY_LH20				;branch if time out
		sbic	_PS2SPY_PORT_INPUT,_PS2SPY_CLOCK_BIT	;skip if clock line low
		rjmp	_PS2SPY_LH00				;
		ldi	AccT,low(_AVR_CLOCK/6000)		;set AccTH:AccT with time out constant value to 1ms
		ldi	AccTH,high(_AVR_CLOCK/6000)
	_PS2SPY_LH10:
		subi	AccT,1					;decrement time out counter
		sbci	AccTH,0
		brcs	_PS2SPY_LH20				;branch if time out
		sbis	_PS2SPY_PORT_INPUT,_PS2SPY_CLOCK_BIT	;wait low to high transiction
		rjmp	_PS2SPY_LH10
		clc						;cy=0
		sbic	_PS2SPY_PORT_INPUT,_PS2SPY_DATA_BIT	;yes, skip
		sec						;else cy=1
		ret
	_PS2SPY_LH20:
		ldi	AccH,1					;set time out flag
		ret
#endif
.exit

