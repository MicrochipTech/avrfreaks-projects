; 	 ____________________________________________________________________________________________
;	|////////////////////////////////////////////////////////////////////////////////////////////|
;	|///|											|////|
;	|///| DRIVE	 : PS2 KEYBOARD AND MOUSE SPEAKER					|////|
;	|///| VERSAO	 : 2.0									|////|
;	|///| PROGRAMADOR: JO�O DARTAGNAN ANTUNES OLIVEIRA				   	|////|
;	|///| DATA 	 : 10 de Julho de 2007 (19:39)						|////|
;	|///| DESCRI��O	 : Emula Keyboard e Mouse PS2 						|////|
;	|///| OBJETIVO:	   Emular interface PS2 processar todos os comandos e enviar codigos    |////|
;	|///| 		   de teclado e rotinas de emula��o de transla��o de ASCII codes para	|////|
;	|///|	           keyboard scan codes.							|////|
;	|///|											|////|
;	|///| OBS:	   This software is inteded to use in AVR with SRAM			|////|
;	|///|		   PS2 LINE RECQUIRE 10K RESISTORS AT CLOCK AND DATA LINES		|////|
;	|///|___________________________________________________________________________________|////|
;-------|////////////////////////////////////////////////////////////////////////////////////////////|
;	>--------------------------------------------------------------------------------------------<
;-------|											     |
;	|   		P S 2  K E Y B O A R D  A N D  M O U S E  S P E A K E R 		     |
;	| 											     |
; 	 --------------------------------------------------------------------------------------------



;			 ///////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\
;			//						\\
;----------------------||  	       	     PS2_SPEAKER		||---------------------------
;			\\						//
			 \\\\\\\\\\\\\\\\\\\\\\\/////////////////////////

#ifndef __PS2_PEAKER__

	#define __PS2_SPEAKER__
	
	.message "+-------------+
	.message "| PS2 SPEAKER |
	.message "+-------------+
	
	;>--------------------------------------------------------------<
	;| OBJECT     : PS2_SPEAKER					|
	;>--------------------------------------------------------------<
	;| DESCRIPTION: This object has a lot of function to control	|
	;|		PS2 line in Speaker mode allowing read and wri- |
	;|		te data thru this interface used commonly in 	|
	;|		MOUSE and KEYBOARD				|
	;>--------------------------------------------------------------<

	;>--------------------------------------------------------------<
	;| OBJECT DEFINITIONS						|
	;| COPY BELOW COMMENTS LINE CODE AND PUT INTO MAIN PROGRAM	|
	;| AND SET PORTS AND SAMPLE BELOW PORTD IS USED			|
	;>--------------------------------------------------------------<


	;-------------------------< PORT DEFINITONS >----------------------------

	;.EQU	_PS2_SPEAKER_KEYBOARD_PORT_OUTPUT	= PORTD				;PS2 KEYBOARD PORTS
	;.EQU	_PS2_SPEAKER_KEYBOARD_PORT_INPUT	= PIND
	;.EQU	_PS2_SPEAKER_KEYBOARD_PORT_DIR		= DDRD

	;.EQU	_PS2_SPEAKER_MOUSE_PORT_OUTPUT		= PORTD				;PS2 MOUSE PORTS
	;.EQU	_PS2_SPEAKER_MOUSE_PORT_INPUT		= PIND
	;.EQU	_PS2_SPEAKER_MOUSE_PORT_DIR		= DDRD

	;-------------------------< CONSTANT DEFINITONS >------------------------

	;.EQU	_PS2_SPEAKER_KEYBOARD_BIT_CLOCK		= 2				;PS2 KEYBOARD CLOCK LINE
	;.EQU	_PS2_SPEAKER_KEYBOARD_BIT_DATA		= 4				;PS2 KEYBOARD DATA LINE
	;.EQU	_PS2_SPEAKER_MOUSE_BIT_CLOCK		= 3				;PS2 MOUSE CLOCK LINE
	;.EQU	_PS2_SPEAKER_MOUSE_BIT_DATA		= 5				;PS2 MOUSE DATA LINE

	;-------------------------< DEVICE DEFINITIONS >--------------------------

	.EQU	_PS2_SPEAKER_DEVICE_BIT			= 0
	.EQU	_PS2_SPEAKER_DEVICE_KEYBOARD		= 0
	.EQU	_PS2_SPEAKER_DEVICE_MOUSE		= 1

	.EQU	_PS2_SPEAKER_STATUS_IDLE		= 1				;STATE=(DATA=HIGH,CLOCK=HIGH) ready to transmite data
	.EQU	_PS2_SPEAKER_STATUS_INHIBIT		= 2				;STATE=(DATA=HIGH,CLOCK=LOW) transmisions inhibit
	.EQU	_PS2_SPEAKER_STATUS_HOST_REQUEST	= 3				;STATE=(DATA=LOW,CLOCK=HIGH) host request transmision

	.EQU	_PS2_SPEAKER_ERROR_OK			= 0				;no error
	.EQU	_PS2_SPEAKER_ERROR_PARITY		= 1				;parity error

	;-------------------------< SRAM  DEFINITONS >------------------------

	.DSEG
		_PS2_SPEAKER_DEVICE:			.BYTE	_BYTE			;Device Type
		_PS2_SPEAKER_DATA_OUT:			.BYTE	_BYTE			;PS2 data out
		_PS2_SPEAKER_DATA_IN:			.BYTE	_BYTE			;PS2 data in
		_PS2_SPEAKER_ERROR:			.BYTE	_BYTE			;PS2 error code
		;/////////////////////////////
		;// KEYBOARD DEFINITIONS
		;/////////////////////////////
		_PS2_SPEAKER_KEYBOARD_DATA_OUT:		.BYTE	_BYTE			;KEYBOARD data to send via PS2 line
		_PS2_SPEAKER_KEYBOARD_DATA_IN:		.BYTE	_BYTE			;KEYBOAD data to be read from PS2 line
		;/////////////////////////////
		;// MOUSE DEFINITIONS
		;/////////////////////////////
	.CSEG
		;>--------------------------------------------------------------<
		;| OBJECT     : PS2_SPEAKER					|
		;>--------------------------------------------------------------<
		;| FUNCTION   : PS2_SPEAKER_INIT				|
		;>--------------------------------------------------------------<
		;| DESCRIPTION: Initialize PS2_LISTENER engine			|
		;>--------------------------------------------------------------<
		;| INPUT      :	None						|
		;| OUTPUT     :	None						|
		;| DESTROYED  :	None						|
		;| TIME	      :	----						|
		;>--------------------------------------------------------------<
		;| OBSERVATION:	Interrupt are disabled during interrupt		|
		;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
		;|																		|
		_PS2_SPEAKER_INIT:
			push	Acc					;save Acc
			in	Acc,SREG				;save SREG
			cli						;disable interrupts
			push	Acc
			;////////////////////////////////////////
			;// PROGRAM PS2 KEYBOARD AND MOUSE LINE
			;// WITH INPUT AND HI-IMPEDANCE
			;////////////////////////////////////////
			cbi	_PS2_SPEAKER_KEYBOARD_PORT_OUTPUT,	\
				_PS2_SPEAKER_KEYBOARD_BIT_CLOCK		;set KEYBOARD clock line to hiz
			cbi	_PS2_SPEAKER_KEYBOARD_PORT_OUTPUT,	\
				_PS2_SPEAKER_KEYBOARD_BIT_DATA		;set KEYBOARD data line to hiz
			cbi	_PS2_SPEAKER_MOUSE_PORT_OUTPUT,		\
				_PS2_SPEAKER_MOUSE_BIT_CLOCK		;set MOUSE clock line to hiz
			cbi	_PS2_SPEAKER_MOUSE_PORT_OUTPUT,		\
				_PS2_SPEAKER_MOUSE_BIT_DATA		;set MOUSE data line to hiz
			cbi	_PS2_SPEAKER_KEYBOARD_PORT_DIR,		\
				_PS2_SPEAKER_KEYBOARD_BIT_CLOCK		;set KEYBOARD clock line as input
			cbi	_PS2_SPEAKER_KEYBOARD_PORT_DIR,		\
				_PS2_SPEAKER_KEYBOARD_BIT_DATA		;set KEYBOARD data line as input
			cbi	_PS2_SPEAKER_MOUSE_PORT_DIR,		\
				_PS2_SPEAKER_MOUSE_BIT_CLOCK		;set MOUSE clock line as input
			cbi	_PS2_SPEAKER_MOUSE_PORT_DIR,		\
				_PS2_SPEAKER_MOUSE_BIT_DATA		;set MOUSE data line as input
			ldi	Acc,_PS2_SPEAKER_ERROR_OK		;no error
			sts	_PS2_SPEAKER_ERROR,Acc
			ldi	Acc,_PS2_SPEAKER_DEVICE_KEYBOARD	;set KEYBOARD as default device 
			rcall	_PS2_SPEAKER_SET_DEVICE
			pop	Acc
			out	SREG,Acc				;restore SREG
			pop	Acc					;restore Acc
			ret
		;>--------------------------------------------------------------<
		;| OBJECT     : PS2_SPEAKER					|
		;>--------------------------------------------------------------<
		;| FUNCTION   : PS2_SPEAKER_SET_DEVICE				|
		;>--------------------------------------------------------------<
		;| DESCRIPTION: Set DEVICE for PS2 				|
		;>--------------------------------------------------------------<
		;| INPUT      :	Acc DEVICE Keyboard or Mouse			|
		;| OUTPUT     :	None						|
		;| DESTROYED  :	None						|
		;| TIME	      :	----						|
		;>--------------------------------------------------------------<
		;| OBSERVATION:	DEVICE may be as follow				|
		;|								|
		;|		_PS2_SPEAKER_DEVICE_KEYBOARD			|
		;|		_PS2_SPEAKER_DEVICE_MOUSE			|
		;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
		;|																		|
		_PS2_SPEAKER_SET_DEVICE:
			sts	_PS2_SPEAKER_DEVICE,Acc
			ret
		;>--------------------------------------------------------------<
		;| OBJECT     : PS2_SPEAKER					|
		;>--------------------------------------------------------------<
		;| FUNCTION   : PS2_SPEAKER_GET_LINE_STATUS			|
		;>--------------------------------------------------------------<
		;| DESCRIPTION: Check status of PS2 line 			|
		;>--------------------------------------------------------------<
		;| INPUT      :	None						|
		;| OUTPUT     :	Acc State					|
		;| DESTROYED  :	None						|
		;| TIME	      :	----						|
		;>--------------------------------------------------------------<
		;| OBSERVATION:	States may be one as bellow			|
		;|								|
		;|		_PS2_SPEAKER_STATE_IDLE				|
		;|		_PS2_SPEAKER_STATE_INHIBIT			|
		;|		_PS2_SPEAKER_STATE_HOST_REQUEST			|
		;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
		;|																		|
		_PS2_SPEAKER_GET_LINE_STATUS:
			;/////////////////////////////////
			;// GET DEVICE NAME
			;/////////////////////////////////
			push	AccH
			lds	AccH,_PS2_SPEAKER_DEVICE				;get current device
			in	Acc,_PS2_SPEAKER_KEYBOARD_PORT_INPUT			;get KEYBOARD status
			sbrc	AccH,_PS2_SPEAKER_DEVICE_BIT				;skip if KEYBOARD
			in	Acc,_PS2_SPEAKER_MOUSE_PORT_INPUT			;else get MOUSE status
			pop	AccH
			;/////////////////////////////////
			;// RETURN STATUS
			;/////////////////////////////////
			andi	Acc,(1<<_PS2_SPEAKER_KEYBOARD_BIT_DATA) | \
				    (1<<_PS2_SPEAKER_KEYBOARD_BIT_CLOCK) 		;mask data and clock
			cpi	Acc,(1<<_PS2_SPEAKER_KEYBOARD_BIT_DATA) | \
				    (1<<_PS2_SPEAKER_KEYBOARD_BIT_CLOCK) 		;line IDLE?
			brne	_PS2_SPEAKER_KEYBOARD_GET_LINE_STATE_10			;no, branch
			ldi	Acc,_PS2_SPEAKER_STATUS_IDLE				;else return IDLE state
			ret
		_PS2_SPEAKER_KEYBOARD_GET_LINE_STATE_10:
			cpi	Acc,(1<<_PS2_SPEAKER_KEYBOARD_BIT_DATA) | \
				    (0<<_PS2_SPEAKER_KEYBOARD_BIT_CLOCK) 		;line INHIBIT?
			brne	_PS2_SPEAKER_KEYBOARD_GET_LINE_STATE_20			;no, branch
			ldi	Acc,_PS2_SPEAKER_STATUS_INHIBIT				;else return INHIBIT
			ret
		_PS2_SPEAKER_KEYBOARD_GET_LINE_STATE_20:
			cpi	Acc,(0<<_PS2_SPEAKER_KEYBOARD_BIT_DATA) | \
				    (1<<_PS2_SPEAKER_KEYBOARD_BIT_CLOCK) 		;line INHIBIT?
			brne	_PS2_SPEAKER_KEYBOARD_GET_LINE_STATE_30
			ldi	Acc,_PS2_SPEAKER_STATUS_HOST_REQUEST			;HOST REQUEST
			ret
		_PS2_SPEAKER_KEYBOARD_GET_LINE_STATE_30:
			ldi	Acc,0							;else INVALID
			ret

		;>--------------------------------------------------------------<
		;| OBJECT     : PS2_SPEAKER					|
		;>--------------------------------------------------------------<
		;| FUNCTION   : PS2_SPEAKER_DEVICE_TO_HOST			|
		;>--------------------------------------------------------------<
		;| DESCRIPTION: Send DEVICE data to HOST			|
		;>--------------------------------------------------------------<
		;| INPUT      :	Acc Data					|
		;| OUTPUT     :	None						|
		;| DESTROYED  :	Flags						|
		;| TIME	      :	~660us						|
		;>--------------------------------------------------------------<
		;| OBSERVATION:	---						|
		;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
		;|						
		;/ KEYBOARD TRANSMITION EVENTS NUMBERS	 						\
		;|											|
		;| EVENT NUMBER -   2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1 					|
		;| 		    5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0			|
		;| KEYBOARD CLOCK --+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-----------------	|
		;|		    | | | | | | | | | | | | | | | | | | | | | |				|
		;|		    +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ 			|
		;|											|
		;| KEYBOARD DATA    +---+---+---+---+---+---+---+---+---+---+---+			|
		;|		 ---+ 0 | D0| D1| D2| D3| D4| D5| D6| D7| P | 1 +---------------	|
		;|		    +---+---+---+---+---+---+---+---+---+---+---+			|
		;|		      ^  \------ DATA BITS -------------/				|
		;|      START BIT ----+				   	   ^   ^			|
		;|							   |   |			|
		;|							   |   +---- STOP BIT		|
		;|							   +-------- PARITY BIT (ODD)	|
		;| REMARKS										|
		;| EVENT NUMBER     STATUS								|
		;|											|
		;|          24	    Output (Start Bit) 							|
		;|		    Clear parity counter						|
		;|		    Programa data bit as output						|
		;|		    Put Clock line low							|
		;|          22	    Output DO clock line low						|
		;|          20	    Output D1 clock line low						|
		;|          18	    Output D2 clock line low						|
		;|          16	    Output D3 clock line low						|
		;|          14	    Output D4 clock line low						|
		;|          12	    Output D5 clock line low						|
		;|          10	    Output D6 clock line low						|
		;|           8	    Output D7 clock line low						|
		;|           6	    Output P clock line low						|
		;|           4	    Output (Stop Bit)							|
		;|		    clock line low							|
		;|           2      Program data bit as input						|
		;|											|
		;| 25..3 step 2	    Output clock line High						|
		;\---------------------------------------------------------------------------------------/
		_PS2_SPEAKER_DEVICE_TO_HOST:
			pushaw								;save useds
			pushawt
			sts	_PS2_SPEAKER_DATA_OUT,Acc				;set DATA temp
			rcall	_PS2_SPEAKER_GET_LINE_STATUS				;get PS2 line status
		_PS2_SPEAKER_DEVICE_TO_HOST_10:
			cpi	Acc,_PS2_SPEAKER_STATUS_INHIBIT				;line INHIBIT?
			breq	_PS2_SPEAKER_DEVICE_TO_HOST_10			;yes, wait line release
			cpi	Acc,_PS2_SPEAKER_STATUS_HOST_REQUEST			;host REQUEST ?
			breq	_PS2_SPEAKER_DEVICE_TO_HOST_EXIT			;yes, exit
			;//////////////////////////////////
			;// START DATA TRANSMISSION PS2 LINE STATE
			;//////////////////////////////////
			lds	Acc,_PS2_SPEAKER_DATA_OUT				;get data
			clc								;DATA=LO(START BIT)
			rcall	_PS2_SPEAKER_SET_DATA
			clc
			rcall	_PS2_SPEAKER_SET_CLOCK					;clock lo
			sec
			rcall	_PS2_SPEAKER_SET_CLOCK					;clock hi
			ldi	AccH,0							;parity counter
			;//////////////////////////////////
			;// SEND DATA 8 DATA BITS
			;//////////////////////////////////
			ldi	AccT,8							;bit counter
		_PS2_SPEAKER_DEVICE_TO_HOST_20:
			sbrc	Acc,0							;skip if bit=0
			inc	AccH							;else increment parity count
			lsr	Acc							;output a bit
			rcall	_PS2_SPEAKER_SET_DATA
			clc
			rcall	_PS2_SPEAKER_SET_CLOCK					;clock lo
			sec
			rcall	_PS2_SPEAKER_SET_CLOCK					;clock hi
			dec	AccT							;bit.count--
			brne	_PS2_SPEAKER_DEVICE_TO_HOST_20			;until all transfered
			;//////////////////////////////////
			;// SEND PARITY BIT
			;//////////////////////////////////
			ldi	AccT,1							;put parity bit into cy
			eor	AccH,AccT
			ror	AccH
			rcall	_PS2_SPEAKER_SET_DATA
			clc
			rcall	_PS2_SPEAKER_SET_CLOCK					;clock lo
			sec
			rcall	_PS2_SPEAKER_SET_CLOCK					;clock hi
			;//////////////////////////////////
			;// SEND STOP BIT
			;//////////////////////////////////
			sec
			rcall	_PS2_SPEAKER_SET_DATA					;DATA=HI
			clc
			rcall	_PS2_SPEAKER_SET_CLOCK					;clock lo
			sec
			rcall	_PS2_SPEAKER_SET_CLOCK					;clock hi
		_PS2_SPEAKER_DEVICE_TO_HOST_EXIT:
			popawt
			popaw
			ret
		;>--------------------------------------------------------------<
		;| OBJECT     : PS2_SPEAKER					|
		;>--------------------------------------------------------------<
		;| FUNCTION   : PS2_SPEAKER_SET_DATA				|
		;>--------------------------------------------------------------<
		;| DESCRIPTION: SET PS2 DATA LINE LO OR HI			|
		;>--------------------------------------------------------------<
		;| INPUT      :	CY   0 data LO 1 data HI			|
		;| OUTPUT     :	None						|
		;| DESTROYED  :	None						|
		;| TIME	      :	----						|
		;>--------------------------------------------------------------<
		;| OBSERVATION:	----						|
		;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
		_PS2_SPEAKER_SET_DATA:
			push	AccH
			;//////////////////////////////////
			;// CHECK GET DEVICE
			;//////////////////////////////////
			lds	AccH,_PS2_SPEAKER_DEVICE				;get current device
			sbrc	AccH,_PS2_SPEAKER_DEVICE_BIT				;skip if KEYBOARD
			rjmp	_PS2_SPEAKER_SET_DATA_20				;else jump to mouse
			;//////////////////////////////////
			;// SET KEYBOARD DATA
			;//////////////////////////////////
			brcc	_PS2_SPEAKER_SET_DATA_10				;brach if carry=0
			cbi	_PS2_SPEAKER_KEYBOARD_PORT_DIR,	\
				_PS2_SPEAKER_KEYBOARD_BIT_DATA				;KEYBOARD DATA =HI
			rjmp	_PS2_SPEAKER_SET_DATA_EXIT
		_PS2_SPEAKER_SET_DATA_10:
			sbi	_PS2_SPEAKER_KEYBOARD_PORT_DIR,	\
				_PS2_SPEAKER_KEYBOARD_BIT_DATA				;KEYBOARD DATA LO
			rjmp	_PS2_SPEAKER_SET_DATA_EXIT
			;//////////////////////////////////
			;// SET MOUSE DATA
			;//////////////////////////////////
		_PS2_SPEAKER_SET_DATA_20:
			brcc	_PS2_SPEAKER_SET_DATA_30				;brach if carry=0
			cbi	_PS2_SPEAKER_MOUSE_PORT_DIR,	\
				_PS2_SPEAKER_MOUSE_BIT_DATA				;MOUSE DATA HI
			rjmp	_PS2_SPEAKER_SET_DATA_EXIT
		_PS2_SPEAKER_SET_DATA_30:
			sbi	_PS2_SPEAKER_MOUSE_PORT_DIR,	\
				_PS2_SPEAKER_MOUSE_BIT_DATA				;MOUSE DATA LO
		_PS2_SPEAKER_SET_DATA_EXIT:
			pop	AccH
			ret
		;>--------------------------------------------------------------<
		;| OBJECT     : PS2_SPEAKER					|
		;>--------------------------------------------------------------<
		;| FUNCTION   : PS2_SPEAKER_SET_CLOCK				|
		;>--------------------------------------------------------------<
		;| DESCRIPTION: SET PS2 CLOCK LINE LO OR HI and make 30us Delay	|
		;>--------------------------------------------------------------<
		;| INPUT      :	CY   0 data LO 1 data HI			|
		;| OUTPUT     :	None						|
		;| DESTROYED  :	None						|
		;| TIME	      :	----						|
		;>--------------------------------------------------------------<
		;| OBSERVATION:	----						|
		;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
		_PS2_SPEAKER_SET_CLOCK:
			push	AccH
			;//////////////////////////////////
			;// CHECK GET DEVICE
			;//////////////////////////////////
			lds	AccH,_PS2_SPEAKER_DEVICE				;get current device
			sbrc	AccH,_PS2_SPEAKER_DEVICE_BIT				;skip if KEYBOARD
			rjmp	_PS2_SPEAKER_SET_CLOCK_20				;else jump to mouse
			;//////////////////////////////////
			;// SET KEYBOARD CLOCK
			;//////////////////////////////////
			brcc	_PS2_SPEAKER_SET_CLOCK_10				;brach if carry=0
			cbi	_PS2_SPEAKER_KEYBOARD_PORT_DIR,	\
				_PS2_SPEAKER_KEYBOARD_BIT_CLOCK				;KEYBOARD CLOCK HI
			rcall	_PS2_SPEAKER_CLOCK_TIME					;Delay 30Us
			rjmp	_PS2_SPEAKER_SET_CLOCK_EXIT
		_PS2_SPEAKER_SET_CLOCK_10:
			sbi	_PS2_SPEAKER_KEYBOARD_PORT_DIR,	\
				_PS2_SPEAKER_KEYBOARD_BIT_CLOCK				;KEYBOARD CLOCK LO
			rcall	_PS2_SPEAKER_CLOCK_TIME					;Delay 30Us
			rjmp	_PS2_SPEAKER_SET_CLOCK_EXIT
			;//////////////////////////////////
			;// SET MOUSE CLOCK
			;//////////////////////////////////
		_PS2_SPEAKER_SET_CLOCK_20:
			brcc	_PS2_SPEAKER_SET_CLOCK_30				;brach if carry=0
			cbi	_PS2_SPEAKER_MOUSE_PORT_DIR,	\
				_PS2_SPEAKER_MOUSE_BIT_CLOCK				;MOUSE CLOCK HI
			rcall	_PS2_SPEAKER_CLOCK_TIME					;Delay 30Us
			rjmp	_PS2_SPEAKER_SET_CLOCK_EXIT
		_PS2_SPEAKER_SET_CLOCK_30:
			sbi	_PS2_SPEAKER_MOUSE_PORT_DIR,	\
				_PS2_SPEAKER_MOUSE_BIT_CLOCK				;MOUSE CLOCK LO
			rcall	_PS2_SPEAKER_CLOCK_TIME					;Delay 30Us
		_PS2_SPEAKER_SET_CLOCK_EXIT:
			pop	AccH
			ret
		;>--------------------------------------------------------------<
		;| OBJECT     : PS2_SPEAKER					|
		;>--------------------------------------------------------------<
		;| FUNCTION   : PS2_SPEAKER_CLOCK_TIME				|
		;>--------------------------------------------------------------<
		;| DESCRIPTION: PS2 CLOCK DELAY 30us				|
		;>--------------------------------------------------------------<
		;| INPUT      :	None						|
		;| OUTPUT     :	None						|
		;| DESTROYED  :	None						|
		;| TIME	      :	----						|
		;>--------------------------------------------------------------<
		;| OBSERVATION:	----						|
		;+--------------------------------------------------------------+-------------------------------------------------------------------------------
		;| Make a 30us Delay 		          1	 
		;| time equation: (9+Acc*3+3(rcall))*----------- 
		;|				      AVR_CLOCK  
		_PS2_SPEAKER_CLOCK_TIME:
			push	Acc
			ldi	Acc,((30*_AVR_CLOCK)/1000000-11)/3
			dec	Acc
			brne	PC-1
			pop	Acc
			ret
		;>--------------------------------------------------------------<
		;| OBJECT     : PS2_SPEAKER					|
		;>--------------------------------------------------------------<
		;| FUNCTION   : PS2_SPEAKER_KEYBOARD_GET_FROM_HOST		|
		;>--------------------------------------------------------------<
		;| DESCRIPTION: Send Keyboard data to HOST			|
		;>--------------------------------------------------------------<
		;| INPUT      :	None						|
		;| OUTPUT     :	None						|
		;| DESTROYED  :	Flags						|
		;| TIME	      :	----						|
		;>--------------------------------------------------------------<
		;| OBSERVATION:	Use _PS2_SPEAKER_KEYBOARD_GET_DATA_IN to get	|
		;|		data received form host				|
		;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
		;/ KEYBOARD RECEPTION EVENTS NUMBERS	 						\
		;|											|
		;| EVENT NUMBER  -  2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1 					|
		;| 		    5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0			|
		;| KEYBOARD CLOCK ---+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-----------------	|
		;|		   ^ | | | | | | | | | | | | | | | | | | | | | |			|
		;|		   | +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ 			|
		;|		   |									|
		;| 		   +- RECEIVE CONDITION	(CLOCK HIGH DATA LOW)				|
		;| KEYBOARD DATA   | +---+---+---+---+---+---+---+---+---+---+---+			|
		;|		   v | D0| D1| D2| D3| D4| D5| D6| D7| P | 1 | 0 +-------------------	|
		;|		  ---+---+---+---+---+---+---+---+---+---+---+---+			|
		;|		      \------ DATA BITS -------------/	       zero inserted by KEYBOARD |
		;|       		      Send by computer 	       ^   ^				|
		;|						       |   |				|
		;|						       |   +---- STOP BIT		|
		;|						       +-------- PARITY BIT (ODD)	|
		;| REMARKS										|
		;| EVENT NUMBER     STATUS								|
		;|											|
		;|          24	    Clear parity counter						|
		;|		    Program data bit as input						|
		;|		    Put Clock line low							|
		;|          23	    get DO clock line high						|
		;|          21	    get D1 clock line high						|
		;|          19	    get D2 clock line high						|
		;|          17	    get D3 clock line high						|
		;|          15	    get D4 clock line high						|
		;|          13	    get D5 clock line high						|
		;|          11	    get D6 clock line high						|
		;|           9	    get D7 clock line high						|
		;|           7	    get P clock line high						|
		;|           5	    get (Stop Bit) clock line high					|
		;|		    clock line low							|
		;|           3      Program data bit as output						|
		;|	     2      process data decode							|
		;| 24..4 step 2	    Output clock line low						|
		;\---------------------------------------------------------------------------------------/

		_PS2_SPEAKER_KEYBOARD_GET_FROM_HOST:
			pushaw
			pushawt
	;		rcall	_PS2_SPEAKER_CLOCK_TIME
	;		clr	AccH						;parity counter
	;		clr	Acc
	;		;//////////////////////////////////
	;		;// START DATA RECEPTION PS2 LINE STATE
	;		;//////////////////////////////////
	;		ldi	AccT,8						;bit counter
	;	_PS2_SPEAKER_KEYBOARD_GET_FROM_HOST_10:
	;		rcall	_PS2_SPEAKER_CLOCK_LO				;clock=lo
	;		clc
	;		sbic	_PS2_SPEAKER_KEYBOARD_PORT_INPUT,_PS2_SPEAKER_KEYBOARD_BIT_DATA	;get bit from HOST
	;		sec
	;		brcc	PC+2						;jump if parity even
	;		inc	AccH						;else increment parity bit
	;		ror	Acc						;Acc<--new bit
	;		rcall	_PS2_SPEAKER_CLOCK_HI				;clock=hi
	;		dec	AccT						;until all bits
	;		brne	_PS2_SPEAKER_KEYBOARD_GET_FROM_HOST_10
	;;		;//////////////////////////////////
	;		;// GET PARITY BIT
	;		;//////////////////////////////////
	;		rcall	_PS2_SPEAKER_CLOCK_LO				;clock=lo
	;		push	Acc						;save data
	;		clc
	;		sbic	_PS2_SPEAKER_KEYBOARD_PORT_INPUT,_PS2_SPEAKER_KEYBOARD_BIT_DATA	;get bit from HOST
	;		sec
	;		rol	Acc	
	;		dec	AccH
	;		eor	Acc,AccH					;parity odd ?
	;		andi	Acc,1						;see bit 0
	;		breq	_PS2_SPEAKER_KEYBOARD_GET_FROM_HOST_20		;no, set error
	;	_PS2_SPEAKER_KEYBOARD_GET_FROM_HOST_20:
	;		pop	Acc						;restore data
	;		;//////////////////////////////////
	;		;// GET STOP BIT
	;		;//////////////////////////////////
	;		rcall	_PS2_SPEAKER_CLOCK_HI				;clock=hi
	;		
	;		rcall	_PS2_SPEAKER_CLOCK_LO				;clock=hi
	;		rcall	_PS2_SPEAKER_CLOCK_HI				;clock=hi
	;		
	;		sbi	_PS2_SPEAKER_KEYBOARD_PORT_DIR,_PS2_SPEAKER_KEYBOARD_BIT_DATA	;data=lo, answer ack
	;		rcall	_PS2_SPEAKER_CLOCK_LO				;clock=lo
	;		cbi	_PS2_SPEAKER_KEYBOARD_PORT_DIR,_PS2_SPEAKER_KEYBOARD_BIT_DATA	;data=hi
	;		rcall	_PS2_SPEAKER_CLOCK_HI				;clock=hi
	;		rcall	_PS2_SPEAKER_KEYBOARD_PUT_DATA_IN		;put data into circular queue
			popawt
			popaw
			ret

#endif
.exit

