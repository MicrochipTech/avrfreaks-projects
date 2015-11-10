;
;	 	 _                     _  ______      __ _       _ _   _                 
;		| |                   | | |  _  \    / _(_)     (_) | (_)                
;		| |     ___   ___ __ _| | | | | |___| |_ _ _ __  _| |_ _  ___  _ __  ___ 
;		| |    / _ \ / __/ _` | | | | | / _ \  _| | '_ \| | __| |/ _ \| '_ \/ __|
;		| |___| (_) | (_| (_| | | | |/ /  __/ | | | | | | | |_| | (_) | | | \__ \
;		\_____/\___/ \___\__,_|_| |___/ \___|_| |_|_| |_|_|\__|_|\___/|_| |_|___/
;                                                                         
                                                                         
; 	 ____________________________________________________________________________________________
;	|////////////////////////////////////////////////////////////////////////////////////////////|
;	|///|											|////|
;	|///| PROJECT	 : Motion Detector ATMEGA64						|////|
;	|///| VERSION	 : 1.20									|////|
;	|///| PROGRAMMER : JOÃO DARTAGNAN ANTUNES OLIVEIRA				   	|////|
;	|///| DATE 	 : Dezember, 6 of 2006 (00:06)						|////|
;	|///| DESCRIPTION: This project has 2 conectors BNC, 1 for Video input and another      |////|
;	|///|		   for output, 1 Relay output Normally Open and NOrmally closed,	|////|
;	|///| 		   communication port RS485 and TX and RX leds, 1 expansion port and 1  |////|
;	|///|		   ISP for device programming.						|////|
;	|///| OBJETIVE:	   This program had developed for Motion detection sensor using a       |////|
;	|///| 		   standard NTSC(60Hz) or PAL(50Hz) color or BW composite video input	|////|
;	|///| 		   for digitizing, genloking, and video generation for capturing motion |////|
;	|///| 		   of pattern in video frame with a grid of 24x24 size, eath cell grid  |////|
;	|///| 		   has capabilities to measure motion in scale of 8bits, those motion	|////|
;	|///| 		   can be advised to user using a Relay or RS484 line.			|////|
;	|///| 											|////|
;	|///| OBSERVATION: This software run only AVRMega family chips				|////|
;	|///|___________________________________________________________________________________|////|
;-------|////////////////////////////////////////////////////////////////////////////////////////////|
;	>--------------------------------------------------------------------------------------------<
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
;	>--------------------------------------------------------------------------------------------<
;-------|											     |
;	| C O N S T A N T S , V A R I A B L E S , L O C A L  F U N C T I O N S  A N D  M O D U L E S |
;	| 											     |
; 	 --------------------------------------------------------------------------------------------


;	 -----------------------------------------------------------------------------------------------
;	|												|
;=======> 		G E N E R A L   C O N S T A N T S   D E F I N I T I O N S	     		|
;	| 												|
;	 -----------------------------------------------------------------------------------------------


	;//////////////////////////
	;//   VERSION NUMBER	 //
	;//////////////////////////

	.EQU	VERSION	= 1020

	;//////////////////////////
	;//   SERIAL  NUMBER	 //
	;//////////////////////////


	.EQU	SERIAL_NUMBER = 12345678

	;//////////////////////////
	;//   DEFAULT PASSWORD	 //
	;//////////////////////////
	;i.e. 14142135
	ATMEGA64_DEFALT_PASSWORD:	.DB	_KEY_UP   ,_KEY_LEFT,_KEY_UP  ,_KEY_LEFT
					.DB	_KEY_RIGHT,_KEY_UP  ,_KEY_DOWN,_KEY_ENTER

	;//////////////////////////
	;//      ANIMATIONS      //
	;//////////////////////////

	.EQU	ANIMATION_TIME			= 70

	ANIMATION_NO_VIDEO_SIGNAL:
				.DW	BMP_BNC_MACHO_ANI_6
				.DW	BMP_BNC_MACHO_ANI_5
				.DW	BMP_BNC_MACHO_ANI_4
				.DW	BMP_BNC_MACHO_ANI_3
				.DW	BMP_BNC_MACHO_ANI_2
				.DW	BMP_BNC_MACHO_ANI_1
				.DW	BMP_BNC_MACHO_ANI_2
				.DW	BMP_BNC_MACHO_ANI_3
				.DW	BMP_BNC_MACHO_ANI_4
				.DW	BMP_BNC_MACHO_ANI_5
				.DW	BMP_BNC_MACHO_ANI_6
				.DW	0
	ANIMATION_VIDEO_PRESENT:
				.DW	BMP_BNC_OK_ANI_1
				.DW	BMP_BNC_OK_ANI_2
				.DW	BMP_BNC_OK_ANI_3
				.DW	BMP_BNC_OK_ANI_4
				.DW	0


	;//////////////////////////
	;//       SCREEN         //
	;//////////////////////////

	.EQU	SCREEN_WIDTH	= 128
	.EQU	SCREEN_HEIGHT	= 96

	;////////////////////////////////////
	;//    KEYBOARD PANEL CONSTANTS    //
	;////////////////////////////////////
	.EQU	KEYBOARD_INPUT_COLS		= 15		;keyboard panel number of cols
	.EQU	KEYBOARD_INPUT_ROWS		= 5		;keyboard panel number of rows
	.EQU	KEYBOARD_INPUT_EDIT_ROWS	= 12
	.EQU	KEYBOARD_INPUT_EDIT_COLS	= 20
	.EQU	KEYBOARD_INPUT_EDIT_BUFFER_SIZE	= KEYBOARD_INPUT_EDIT_ROWS*KEYBOARD_INPUT_EDIT_COLS
	.EQU	KEYBOARD_INPUT_OMEGA_X		= 0
	.EQU	KEYBOARD_INPUT_OMEGA_Y		= 25


;	 -----------------------------------------------------------------------------------------------
;	|												|
;=======>    	     G E N E R A L  S R A M  D A T A  V A R I A B L E S  D E F I N I T I O N S    	|
;	| 												|
;	 -----------------------------------------------------------------------------------------------


	;////////////////////
	;// MENUS VARIABLES
	;////////////////////
	.DSEG
		_MENU_ADDR_START:
			_MENU_CONFIGURE_SYSTEM_CHECK:				.BYTE	_BYTE
			_MENU_CONFIGURE_CAPTURE_MOVEMENT:			.BYTE	_BYTE
			_MENU_CONFIGURE_CAPTURE_MOVEMENT_SENSIBILITY:		.BYTE	_BYTE	;Hold sensibility value 0..255
			_MENU_CONFIGURE_CAPTURE_MOVEMENT_TRACE_TIME_CHECK:	.BYTE	_BYTE
			_MENU_CONFIGURE_CAPTURE_MOVEMENT_TRACE_TIME_VALUE:	.BYTE	_BYTE	;decay value 0..255
			_MENU_CONFIGURE_CAPTURE_MOVEMENT_TRACE_TYPE_CHECK:	.BYTE	_BYTE
			_MENU_CONFIGURE_ACTIVATION_CHECK:			.BYTE	_BYTE
			_MENU_CONFIGURE_ACTIVATION_CONFIG_RELAY_REPEATS_CHECK:	.BYTE	_WORD
			_MENU_CONFIGURE_COMMUNICATION_CHECK:			.BYTE	_BYTE
			_MENU_CONFIGURE_COMMUNICATION_IP:			.BYTE	_LONG	;reserve 4 bytes for IP
			_MENU_CONFIGURE_SCREEN_CHECK:				.BYTE	_BYTE
			_MENU_MESSAGE_APRESENTATION_CHECK:			.BYTE	_BYTE
			_MENU_MESSAGE_IDENTIFICATION_CHECK:			.BYTE	_BYTE
			_MENU_MESSAGE_ALARM_CHECK:				.BYTE	_BYTE
			_MENU_IDIOM_CHECK:					.BYTE	_BYTE
                        _MENU_PROGRAMS_CHECK:                                   .BYTE   _BYTE
		_MENU_ADDR_END:
	.CSEG
	;////////////////////
	;// MENUS CONSTANTS
	;////////////////////
	.EQU	_MENU_SIZE			= _MENU_ADDR_END-_MENU_ADDR_START


	;////////////////////
	;// KEYBORD PANEL VARIABLES
	;////////////////////
	.DSEG
		KEYBOARD_INPUT_X:		.BYTE	_WORD		;X coordinate of keyboard panel
		KEYBOARD_INPUT_Y:		.BYTE	_WORD		;Y coordinate of keyboard panel
		KEYBOARD_INPUT_ROW:		.BYTE	_BYTE		;keyboard row
		KEYBOARD_INPUT_COL:		.BYTE	_BYTE		;keyboard col
		KEYBOARD_INPUT_BAR_ROW:		.BYTE	_BYTE		;scroll bar position
		KEYBOARD_INPUT_EDIT_ROW:	.BYTE	_BYTE
		KEYBOARD_INPUT_EDIT_COL:	.BYTE	_BYTE
		KEYBOARD_INPUT_CAPS:		.BYTE	_BYTE		;_ON/_OFF
		KEYBOARD_INPUT_MODE:		.BYTE	_BYTE		;NORMAL OR INVERTED
		POINTS:							;POINTS ARE PAIRS OF BYTES THAT MEANS
									;X,Y COORDINATES USED BY GRAFIC PROCESSING
									;THIS USE SAME AREA OF KEYBOARD_BUFFER
									;OR SEQUENCE OF Y POINTS REPRESENTING MOTION
									;INTENSITY
		KEYBOARD_INPUT_EDIT_BUFFER:	.BYTE	KEYBOARD_INPUT_EDIT_BUFFER_SIZE
	.CSEG
	;////////////////////
	;// PASSWORD VARIABLES
	;////////////////////
	.DSEG
		PASSWORD_DIGIT_COUNT:	.BYTE	_BYTE			;digit count
		PASSWORD_DIGITS:	.BYTE	8			;store password digits
	.CSEG

	;/////////////
	;// RELAY 
	;/////////////
	.DSEG
		RELAY_TIME_ACTIVATION:	.BYTE	_WORD
	.CSEG

	;/////////////
	;// COPYRIGHT
	;/////////////
	COPYRIGHT:
		;    01234567890123456789
		.DB "Motion Detector     "
		.DB "ATMega64 is a Prog. "
		.DB "Developed by Joao   "
		.DB "D´Artagnan A. O.,   "
		.DB "this version 1,20 is"
		.DB "a Free Release.     "
		.DB "                    "
		.DB "MOTION DETECTOR     "
		.DB "ATMega64 Copyright. "
		.DB "2006-2008           "
		.DB "                    "
		.DB "Bsb 4,November 2015 "
	COPYRIGHT_END:

;	 -----------------------------------------------------------------------------------------------
;	|												|
;=======>    	     G E N E R A L  E E P R O M  D A D A  V A R I A B L E S  D E F I N I T I O N S    	|
;	| 												|
;	 -----------------------------------------------------------------------------------------------

	.ESEG
		E_PASSWORD_MAIN:		.BYTE	8		;main password
		E_PASSWORD_BACKUP:		.BYTE	8		;backup password
		;/////////////
		;// GRID
		;/////////////
		E_VIDEO_DIGITIZING_MASK_AREAS:	.BYTE	_VIDEO_DIGITIZING_MASK_SIZE	;reserve space for grid move areas
		;/////////////
		;// MENUS
		;/////////////
		E_MENU:				.BYTE	_MENU_SIZE			;reserve space for menu
		;/////////////
		;// MESSAGES
		;/////////////
		E_MSG_APRESENTACAO:		.BYTE 	KEYBOARD_INPUT_EDIT_BUFFER_SIZE	;reserve space for msgs
		E_MSG_APRESENTACAO_MODE:	.BYTE	1
		E_MSG_IDENTIFICACAO:		.BYTE	KEYBOARD_INPUT_EDIT_BUFFER_SIZE
		E_MSG_IDENTIFICACAO_MODE:	.BYTE	1
		E_MSG_ALARME:			.BYTE	KEYBOARD_INPUT_EDIT_BUFFER_SIZE
		E_MSG_ALARME_MODE:		.BYTE	1
		;/////////////
		;// RELAY 
		;/////////////
		E_RELAY_TIME_ON:		.BYTE	_WORD
		E_RELAY_TIME_OFF:		.BYTE	_WORD
		E_RELAY_TIME_ACTIVATION:	.BYTE	_WORD
	.CSEG
;	 -----------------------------------------------------------------------------------------------
;	|												|
;=======> 			G E N E R A L  L O C A L   F U N C T I O N S   				|
;	| 												|
;	 -----------------------------------------------------------------------------------------------

	
;+------------------------------------------------------------------------------+
;| FUNCTION  : FADE IN								|
;+------------------------------------------------------------------------------+	
;| Description:									|
;|	Start fade of screen from 0 to max white level at desiderate time	|
;|										|
;+------------------------------------------------------------------------------+
;| Input    : AccH:Acc time im mileseconds					|
;| Output   : None								|
;+------------------------------------------------------------------------------+
fn_FADE_IN:
	push	AccT
	ldi	AccT,_VIDEO_OUT_LEVEL_BLACK					;AccT=white level min
fn_FADE_IN_10:
	push	Acc
	mov	Acc,AccT
	call	_VIDEO_OUT_SET_WHITE_LEVEL
	pop	Acc

	call	_DELAY_MILLISECONDS
	subi	AccT,-1
	cpi	AccT,_VIDEO_OUT_LEVEL_WHITE_TOP					;max white level ?
	brlo	fn_FADE_IN_10							;no, branch
	pop	AccT
	ret

;+------------------------------------------------------------------------------+
;| FUNCTION  : FADE OUT								|
;+------------------------------------------------------------------------------+	
;| Description:									|
;|	Start fade of screen from max white to 0 level at desiderate time	|
;|										|
;+------------------------------------------------------------------------------+
;| Input    : AccH:Acc time im mileseconds					|
;| Output   : None								|
;+------------------------------------------------------------------------------+
fn_FADE_OUT:
	push	AccT
	push	Acc
	call	_VIDEO_OUT_GET_WHITE_LEVEL					;get actual level
	mov	AccT,Acc
	pop	Acc
fn_FADE_OUT_10:
	push	Acc
	mov	Acc,AccT
	call	_VIDEO_OUT_SET_WHITE_LEVEL
	pop	Acc
	call	_DELAY_MILLISECONDS
	subi	AccT,1
	cpi	AccT,_VIDEO_OUT_LEVEL_BLACK					;min white level ?
	brsh	fn_FADE_OUT_10							;no, branch
	pop	AccT
	ret
;+------------------------------------------------------------------------------+
;| FUNCTION  : DRAW MENU BACKGROUND						|
;+------------------------------------------------------------------------------+	
;| Description:									|
;|	Draw MENU background							|
;|										|
;+------------------------------------------------------------------------------+
;| Input    : None								|
;| Output   : None								|
;+------------------------------------------------------------------------------+

fn_MENU_DRAW_BACKGROUND:
	pushw	X
	pushw	Y
	pushw	Z
	pushaw
	ldiw	X,0						;at position(0,0)
	ldiw	Y,0
	ldiw	Z,BMP_CAMERA_7*2				;Z-->CAMERA
	call 	_RASTER_SET_BMP_FAST				;show CAMERA
	popaw
	popw	Z
	popw	Y
	popw	X
	ret


;+------------------------------------------------------------------------------+
;| FUNCTION  : UFO ANIMATION							|
;+------------------------------------------------------------------------------+	
;| Description:									|
;|	Make UFO animation							|
;|										|
;+------------------------------------------------------------------------------+
;| Input    : None								|
;| Output   : None								|
;+------------------------------------------------------------------------------+

	.INCLUDE	"BMPS\BMP_UFO_1_1.INC"			;UFO images
	.INCLUDE	"BMPS\BMP_UFO_1_2.INC"
	.INCLUDE	"BMPS\BMP_UFO_1_3.INC"

	.INCLUDE	"BMPS\BMP_UFO_2_1.INC"			;MOTION_DETECTOR BEAM
	.INCLUDE	"BMPS\BMP_UFO_2_2.INC"
	.INCLUDE	"BMPS\BMP_UFO_2_3.INC"
	.INCLUDE	"BMPS\BMP_UFO_2_4.INC"
	.INCLUDE	"BMPS\BMP_UFO_2_5.INC"
	.INCLUDE	"BMPS\BMP_UFO_2_6.INC"
	.INCLUDE	"BMPS\BMP_UFO_2_7.INC"
	.INCLUDE	"BMPS\BMP_UFO_2_8.INC"

.EQU	UFO_Y		= 96-32
.EQU	UFO_POS_START	= 200
.EQU	UFO_POS_END	= SCREEN_WIDTH/2-33+3
.EQU	UFO_STEP	= 1
.EQU	UFO_FRAME_TIME	= 1
.EQU	UFO_SEQ_ENTER	= 1
.EQU	UFO_SEQ_MOTION_DETECTOR	= 2
.EQU	UFO_SEQ_GOAWAY	= 3
.EQU	UFO_SEQ_VANISH	= 4
.EQU	DSQ_Y		= 96-32+17
.EQU	DSQ_X		= SCREEN_WIDTH/2-20+2
.EQU	DSQ_FRAME_TIME	= 50

.DSEG
	UFO_X:		.BYTE	_BYTE
	UFO_SEQ:	.BYTE	_BYTE
	UFO_FRAME:	.BYTE	_BYTE
	DSQ_FRAME:	.BYTE	_BYTE
.CSEG

fn_UFO_ANIMATION_CLEAR:
	pushw	Z
	pushaw
	ldi	Acc,UFO_POS_START
	sts	UFO_X,Acc
	ldi	Acc,UFO_SEQ_ENTER
	sts	UFO_SEQ,Acc
	ldi	Acc,0
	sts	UFO_FRAME,Acc
	sts	DSQ_FRAME,Acc
	popaw
	popw	Z
	ret
	
fn_UFO_ANIMATION:
	pushaw
	pushw	X
	pushw	Y
	pushw	Z
	;/////////////////////////////////
	;// EXECUTE ACCORDING SEQUENCE
	;/////////////////////////////////
	lds	Acc,UFO_SEQ
	cpi	Acc,UFO_SEQ_ENTER			;ENTER sequence ?
	breq	fn_UFO_ANIMATION_ENTER			;yes, branch
	cpi	Acc,UFO_SEQ_MOTION_DETECTOR			;MOTION_DETECTOR sequence ?
	breq	fn_UFO_ANIMATION_MOTION_DETECTOR		;yes, branch
	cpi	Acc,UFO_SEQ_GOAWAY			;GOAWAY sequence ?
	breq	fn_UFO_ANIMATION_GOAWAY			;yes, branch
	rjmp	fn_UFO_ANIMATION_EXIT			;none, then exit
	;/////////////////////////////////
	;// ENTER SEQUENCE
	;/////////////////////////////////
	fn_UFO_ANIMATION_ENTER:
		lds	XL,UFO_X				;get X position
		subi	XL,UFO_STEP				;x-=UFO_STEP
		sts	UFO_X,XL
		cpi	XL,UFO_POS_END				;x<end?
		brsh	fn_UFO_ANIMATION_ENTER_10		;no, branch
		ldi	Acc,UFO_SEQ_MOTION_DETECTOR		;set MOTION_DETECTOR sequence
		sts	UFO_SEQ,Acc
		rjmp	fn_UFO_ANIMATION_EXIT			;exit
	fn_UFO_ANIMATION_ENTER_10:
		rcall	fn_UFO_UPDATE_FRAME			;update frame
		_M_DELAY_MILLISECONDS	UFO_FRAME_TIME
		rjmp	fn_UFO_ANIMATION_EXIT
	;/////////////////////////////////
	;// MOTION_DETECTOR SEQUENCE
	;/////////////////////////////////
	fn_UFO_ANIMATION_MOTION_DETECTOR:
		lds	XL,UFO_X				;get X position
		rcall	fn_UFO_UPDATE_FRAME			;update frame
		_M_DELAY_MILLISECONDS	UFO_FRAME_TIME
	fn_UFO_ANIMATION_MOTION_DETECTOR_10:
		rjmp	fn_UFO_ANIMATION_EXIT
	;/////////////////////////////////
	;// GO AWAY SEQUENCE
	;/////////////////////////////////
	fn_UFO_ANIMATION_GOAWAY:
		lds	XL,UFO_X				;get X position
		addi	XL,UFO_STEP				;x+=UFO_STEP
		sts	UFO_X,XL
		cpi	XL,UFO_POS_START			;x>start?
		brlo	fn_UFO_ANIMATION_GOAWAY_10		;no, branch
		ldi	Acc,UFO_SEQ_ENTER			;set ENTER sequence
		sts	UFO_SEQ,Acc
		ldi	Acc,UFO_POS_START
		sts	UFO_X,Acc
		call	_VIDEO_OUT_CLS
		rjmp	fn_UFO_ANIMATION_EXIT			;exit
	fn_UFO_ANIMATION_GOAWAY_10:
		rcall	fn_UFO_UPDATE_FRAME			;update frame
		_M_DELAY_MILLISECONDS	UFO_FRAME_TIME
		rjmp	fn_UFO_ANIMATION_EXIT
fn_UFO_ANIMATION_EXIT:
	popw	Z
	popw	Y
	popw	X	
	popaw
	ret
	;/////////////////////////////////
	;/// UPDATE UFO FRAME
	;/////////////////////////////////
fn_UFO_UPDATE_FRAME:
		lds	Acc,UFO_FRAME				;update frame
		inc	Acc					;frame++
		cpi	Acc,3					;last frame ?
		brlo	fn_UFO_UPDATE_FRAME_10			;no, branch
		ldi	Acc,0					;else clear frame
	fn_UFO_UPDATE_FRAME_10:
		sts	UFO_FRAME,Acc
		ldiw	Z,BMP_UFO_1_1*2				;assume frame 1
		cpi	Acc,0
		breq	fn_UFO_UPDATE_FRAME_20			;yes, branch
		ldiw	Z,BMP_UFO_1_2*2				;assume frame 2
		cpi	Acc,1
		breq	fn_UFO_UPDATE_FRAME_20			;yes, branch
		ldiw	Z,BMP_UFO_1_3*2				;assume frame 3
	fn_UFO_UPDATE_FRAME_20:
		ldi	YL,UFO_Y				;set coordinates
		ldi	YH,0
		ldi	XH,0
		call	_RASTER_SET_BMP_FAST			;draw UFO
		lds	Acc,UFO_SEQ				;get seq
		cpi	Acc,UFO_SEQ_MOTION_DETECTOR			;MOTION_DETECTOR SEQ?
		brne	fn_UFO_UPDATE_FRAME_EXIT
		lds	Acc,DSQ_FRAME				;get MOTION_DETECTOR frame
		ldiw	Z,BMP_UFO_2_1*2				;select frame
		cpi	Acc,0
		breq	fn_UFO_UPDATE_FRAME_30
		ldiw	Z,BMP_UFO_2_2*2
		cpi	Acc,1
		breq	fn_UFO_UPDATE_FRAME_30
		ldiw	Z,BMP_UFO_2_3*2
		cpi	Acc,2
		breq	fn_UFO_UPDATE_FRAME_30
		ldiw	Z,BMP_UFO_2_4*2
		cpi	Acc,3
		breq	fn_UFO_UPDATE_FRAME_30
		ldiw	Z,BMP_UFO_2_5*2
		cpi	Acc,4
		breq	fn_UFO_UPDATE_FRAME_30
		ldiw	Z,BMP_UFO_2_6*2
		cpi	Acc,5
		breq	fn_UFO_UPDATE_FRAME_30
		ldiw	Z,BMP_UFO_2_7*2
		cpi	Acc,6
		breq	fn_UFO_UPDATE_FRAME_30
		ldiw	Z,BMP_UFO_2_8*2
	fn_UFO_UPDATE_FRAME_30:
		ldiw	X,DSQ_X
		ldiw	Y,DSQ_Y
		call	_RASTER_SET_BMP_FAST			;draw MOTION_DETECTOR beam
		_M_DELAY_MILLISECONDS	DSQ_FRAME_TIME
		lds	Acc,DSQ_FRAME
		inc	Acc
		sts	DSQ_FRAME,Acc
		cpi	Acc,30
		brlo	fn_UFO_UPDATE_FRAME_40
		ldi	Acc,0
		sts	DSQ_FRAME,Acc
		ldi	Acc,UFO_SEQ_GOAWAY			;set GOAWAY sequence
		sts	UFO_SEQ,Acc
	fn_UFO_UPDATE_FRAME_40:
	fn_UFO_UPDATE_FRAME_EXIT:
		ret
	
;+------------------------------------------------------------------------------+
;| FUNCTION  : DRAW BIG DIGIT							|
;+------------------------------------------------------------------------------+	
;| Description:									|
;|	Draw BIG DIGIT								|
;|										|
;+------------------------------------------------------------------------------+
;| Input    : Acc  digit 0..9 ASCII or binary					|
;|	      X,Y  Coordinates of screen					|
;| Output   : X,Y  New Coordinates						|
;+------------------------------------------------------------------------------+

	.INCLUDE	"BMPS\BMP_DIGITO_SP_1.INC"		;digito SPACE
	.INCLUDE	"BMPS\BMP_DIGITO_0_1.INC"		;digito 0
	.INCLUDE	"BMPS\BMP_DIGITO_1_1.INC"		;digito 1
	.INCLUDE	"BMPS\BMP_DIGITO_2_1.INC"		;digito 2
	.INCLUDE	"BMPS\BMP_DIGITO_3_1.INC"		;digito 3
	.INCLUDE	"BMPS\BMP_DIGITO_4_1.INC"		;digito 4
	.INCLUDE	"BMPS\BMP_DIGITO_5_1.INC"		;digito 5
	.INCLUDE	"BMPS\BMP_DIGITO_6_1.INC"		;digito 6
	.INCLUDE	"BMPS\BMP_DIGITO_7_1.INC"		;digito 7
	.INCLUDE	"BMPS\BMP_DIGITO_8_1.INC"		;digito 8
	.INCLUDE	"BMPS\BMP_DIGITO_9_1.INC"		;digito 9


fn_DRAW_BIG_DIGIT:
	pushaw
	pushw	Z
	andi	Acc,0x0f				;extract if ASCII
	ldiw	Z,BMP_DIGITO_0_1*2			;assume digit 0
	cpi	Acc,0					;digit 0?
	breq	fn_DRAW_BIG_DIGIT_10
	ldiw	Z,BMP_DIGITO_1_1*2			;assume digit 1
	cpi	Acc,1
	breq	fn_DRAW_BIG_DIGIT_10
	ldiw	Z,BMP_DIGITO_2_1*2			;assume digit 2
	cpi	Acc,2
	breq	fn_DRAW_BIG_DIGIT_10
	ldiw	Z,BMP_DIGITO_3_1*2			;assume digit 3
	cpi	Acc,3
	breq	fn_DRAW_BIG_DIGIT_10
	ldiw	Z,BMP_DIGITO_4_1*2			;assume digit 4
	cpi	Acc,4
	breq	fn_DRAW_BIG_DIGIT_10
	ldiw	Z,BMP_DIGITO_5_1*2			;assume digit 5
	cpi	Acc,5
	breq	fn_DRAW_BIG_DIGIT_10
	ldiw	Z,BMP_DIGITO_6_1*2			;assume digit 6
	cpi	Acc,6
	breq	fn_DRAW_BIG_DIGIT_10
	ldiw	Z,BMP_DIGITO_7_1*2			;assume digit 7
	cpi	Acc,7
	breq	fn_DRAW_BIG_DIGIT_10
	ldiw	Z,BMP_DIGITO_8_1*2			;assume digit 8
	cpi	Acc,8
	breq	fn_DRAW_BIG_DIGIT_10
	ldiw	Z,BMP_DIGITO_9_1*2			;assume digit 9
	cpi	Acc,9
	breq	fn_DRAW_BIG_DIGIT_10			;assume digit SP
	ldiw	Z,BMP_DIGITO_SP_1*2
fn_DRAW_BIG_DIGIT_10:
	call 	_RASTER_SET_BMP_FAST			;show DIGIT
	call	_RASTER_BMP_GET_WIDTH			;get bmp width
	add	XL,Acc					;X+=BMP.Width
	adc	XH,AccH
	popw	Z
	popaw
	ret
;+------------------------------------------------------------------------------+
;| FUNCTION  : DRAW RTC CLOCK							|
;+------------------------------------------------------------------------------+	
;| Description:									|
;|	Draw RTC clock at screen position					|
;|										|
;+------------------------------------------------------------------------------+
;| Input    : X,Y screen coordinates to put clock				|
;| Output   : None								|
;+------------------------------------------------------------------------------+

fn_DRAW_RTC:
	pushw	X
	pushw	Y
	pushw	Z
	pushaw	
	call	_RASTER_GET_CHAR_MODE				;save actual mode
	push	Acc
	ldi	Acc,_RASTER_CHAR_MODE_NORMAL			;set NORMAL MODE
	call	_RASTER_SET_CHAR_MODE
	call	_RTC_GET_HOUR					;get hour
	call	_UBTOSU						;convert to ASCII
	adiw	ZL,2
	call	_RASTER_SET_TEXT_OUT_SRAM			;draw on screen
	ldi	Acc,':'						;draw ':' separator
	call	_RASTER_SET_CHAR
	call	_RTC_GET_MINUTE					;get minute
	call	_UBTOSU						;convert to ASCII
	adiw	ZL,2
	call	_RASTER_SET_TEXT_OUT_SRAM			;draw on screen
	pop	Acc
	call	_RASTER_SET_CHAR_MODE				;restore old char mode
	popaw	
	popw	Z
	popw	Y
	popw	X
	ret

;+------------------------------------------------------------------------------+
;| FUNCTION  : DRAW OPERATION EYE						|
;+------------------------------------------------------------------------------+	
;| Description:									|
;|	Draw operation eye on screen						|
;|										|
;+------------------------------------------------------------------------------+
;| Input    : None								|
;| Output   : None								|
;+------------------------------------------------------------------------------+

.DSEG
	EYE_COUNT:		.BYTE	_BYTE
.CSEG

	.INCLUDE	"BMPS\BMP_OLHO_1.INC"
	.INCLUDE	"BMPS\BMP_OLHO_2.INC"

fn_DRAW_EYE:
	pushw	X
	pushw	Y
	pushw	Z
	pushaw
	;/////////////////////////
	;/// PUMP EYE COUNT
	;/////////////////////////
	push	Temp
	lds	Acc,EYE_COUNT
	inc	Acc
	cpi	Acc,4
	sts	EYE_COUNT,Acc
	brlo	fn_DRAW_EYE_10
	clr	Acc
	sts	EYE_COUNT,Acc
fn_DRAW_EYE_10:
	cpi	Acc,2
	brsh	fn_DRAW_EYE_20
	;/////////////////////////
	;/// SHOW EYE OPEN
	;/////////////////////////
	ldi	Acc,0
	ldiw	X,SCREEN_WIDTH-16
	ldiw	Y,SCREEN_HEIGHT-8
	ldiw	Z,BMP_OLHO_1*2					;Z-->OLHO OPEN
	call 	_RASTER_SET_BMP_FAST				;show OLHO OPEN
	rjmp	fn_DRAW_EYE_30
fn_DRAW_EYE_20:
	;/////////////////////////
	;/// SHOW EYE CLOSE
	;/////////////////////////
	ldiw	X,SCREEN_WIDTH-16
	ldiw	Y,SCREEN_HEIGHT-8
	ldiw	Z,BMP_OLHO_2*2					;Z-->OLHO CLOSE
	call 	_RASTER_SET_BMP_FAST				;show OLHO CLOSE
fn_DRAW_EYE_30:
	ldiw	X,0
	ldiw	Y,SCREEN_HEIGHT-_RASTER_CHAR_HEIGHT
	call	fn_DRAW_RTC
	pop	Temp
	popaw
	popw	Z
	popw	Y
	popw	X
	ret

;+------------------------------------------------------------------------------+
;| FUNCTION  : LEVEL BAR							|
;+------------------------------------------------------------------------------+	
;| Description:									|
;|	Draw LEVEL BAR at bootom o screen and left side				|
;|										|
;+------------------------------------------------------------------------------+
;| Input    : Acc Level 0..255							|
;| Output   : None								|
;+------------------------------------------------------------------------------+

	.INCLUDE	"BMPS\BMP_LEVEL_BAR_1.INC"

fn_DRAW_LEVEL_BAR:
	pushw	X
	pushw	Y
	pushw	Z
	pushaw
	;@--------------------------@
	;| get repeats		    |
	;@--------------------------@
	push	Acc
	ldiw	Z,_MENU_PORTUGUESE_CONFIGURAR_ATIVACAO_CONFIGURACAO_REPETICAO*2	;get number of repeats
	call	_MENU_GET_ITEM_CHECK_FIRST			;get item number checked
	pop	Acc
	cpi	AccH,11						;free?
	breq	fn_DRAW_LEVEL_BAR_10				;yes, branch
	;@-----------------------------------@
	;| pulse relay if value>=sensibility |
	;@-----------------------------------@
	push	Acc
	call	_VIDEO_DIGITIZING_GET_SENSIBILITY		;get sensibility actual
	mov	Temp,Acc
	pop	Acc
	cp	Acc,Temp					;motion great than sensibility?
	brlo	fn_DRAW_LEVEL_BAR_20				;no,branch
	;@-------------------------@
	;| alarm with pulsed relay |
	;@-------------------------@
	push	Acc
	mov	Acc,AccH					;set repeats event
	call	_MOTION_RELAY_SET_EVENT_START			;start relay new event
	pop	Acc
	rjmp	fn_DRAW_LEVEL_BAR_20
fn_DRAW_LEVEL_BAR_10:
	;@-----------------------------------------------@
	;| alarm if above disarm if below of sensibility |
	;@-----------------------------------------------@
	push	Acc
	call	_VIDEO_DIGITIZING_GET_SENSIBILITY		;get sensibility actual
	mov	Temp,Acc
	pop	Acc
	cp	Acc,Temp					;above?
	ldi	Acc,_ON						;assume on
	brsh	PC+2						;yes, branch
	ldi	Acc,_OFF					;else off
	call	_MOTION_RELAY_SET_STATE
fn_DRAW_LEVEL_BAR_20:
	;@----------------------@
	;| draw empty spaces	|
	;@----------------------@
	lds	YL,_VIDEO_OUT_GRAPH_HEIGHT			;get height
	subi	YL,9
	ldi	XL,30
	ldi	XH,8
	ldi	YH,9
	ldi	Temp,_OFF
	call	_RASTER_SET_BOX_FULL_FAST			;draw empty space left
	ldi	XL,88
	ldi	XH,24
	ldi	YH,9
	call	_RASTER_SET_BOX_FULL_FAST			;draw empty space right
	ldi	XL,0
	ldi	XH,30
	call	_RASTER_SET_HLINE_FAST				;draw empty space right
	lds	XL,_VIDEO_OUT_GRAPH_WIDTH
	subi	XL,16
	ldi	XH,16
	call	_RASTER_SET_HLINE_FAST				;draw empty space right
	;@----------------------@
	;| draw level bar	|
	;@----------------------@
	ldiw	Z,BMP_LEVEL_BAR_1*2				;get width
	ldiw	X,SCREEN_WIDTH
	call	_RASTER_BMP_GET_WIDTH
	sub	XL,Acc						;center on width
	sbc	XH,AccH
	lsrw	X
	call	_RASTER_BMP_GET_HEIGHT
	ldiw	Y,SCREEN_HEIGHT
	sub	YL,Acc
	sbc	YH,AccH
	call 	_RASTER_SET_BMP_FAST				;show LEVEL BAR
	popaw
	;@----------------------@
	;| draw level		|
	;@----------------------@
	push	Acc
	mov	r0,Acc						;draw bar
	call	_RASTER_BMP_GET_WIDTH				;get width again
	mov	r1,Acc
	ldi	Acc,255
	mov	r2,Acc
	call	_MULDIV_U8U8U8U8
	mov	XH,r0
	call	_RASTER_BMP_GET_HEIGHT				;get height again
	subi	Acc,4
	mov	YH,Acc
	addi	YL,2
	ldi	Temp,_ON
	call	_RASTER_SET_BOX_FULL_FAST			;draw LEVEL BAR
	pop	Acc
	popw	Z
	popw	Y
	popw	X
	ret

;+------------------------------------------------------------------------------+
;| FUNCTION  : CLEAR MOTION AREA						|
;+------------------------------------------------------------------------------+	
;| Description:									|
;|	Clear motion area full if motion status if off else clear all area 	|
;|	not including status area						|
;|										|
;+------------------------------------------------------------------------------+
;| Input    : None								|
;| Output   : None								|
;+------------------------------------------------------------------------------+

fn_CLEAR_MOTION_AREA:
	pushw	Y
	push	Acc
	ldi	Acc,0x00
	ldi	YL,0
	lds	YH,_VIDEO_OUT_GRAPH_HEIGHT			;get actual screen height
	subi	YH,9
	call	_RASTER_FILL_ROWS
	pop	Acc
	popw	Y
	ret

;+------------------------------------------------------------------------------+
;| FUNCTION  : ANIMATION							|
;+------------------------------------------------------------------------------+	
;| Description:									|
;|		Draw animation of sequency of bmps dispose as follow		|
;|										|
;|	Animation:	.dw	bmp_animation_1	;sequence 1			|
;|			.dw	bmp_animation_2	;sequence 2			|
;|			.dw	..						|
;|			.dw	bmp_animation_n	;sequence N			|
;|			.dw	0		;word(0) to finish		|
;|										|
;+------------------------------------------------------------------------------+
;| Input    : X,Y  Anination Coordinates					|
;|	      Z--> Animation							|
;|	      AccH:Acc Animation delay in mileseconds				|	
;| Output   : Cy=1 if any key pressed during animation				|
;+------------------------------------------------------------------------------+
fn_ANIMATION:
	pushw	X
	pushw	Y
	pushw	Z
	pushaw
	pushawt
	push	Temp
fn_ANIMATION_10:
	lpm	AccT,Z+			;get animation address
	lpm	AccTH,Z+
	cpiawt	0			;animation end ?
	clc				;set normal end
	breq	fn_ANIMATION_EXIT	;yes, exit
	push	Acc
	call	_KEYBOARD_READ_FLASH	;read keyboard
	cpi	Acc,_KEY_NONE		;any key pressed?
	pop	Acc
	sec
	brne	fn_ANIMATION_EXIT	;yes exit with key pressed flag cy=1
	pushw	Z			;save animation pointer
	movw	ZL,AccT			;get animation address
	lsl	ZL			;byte to word
	rol	ZH
	call	_RASTER_SET_BMP_FAST	;show animation frame
	call	_DELAY_MILLISECONDS	;wait a time
	popw	Z
	rjmp	fn_ANIMATION_10		;next frame
fn_ANIMATION_EXIT:
	pop	Temp
	popawt
	popaw
	popw	Z
	popw	X
	popw	X
	ret

;+------------------------------------------------------------------------------+
;| FUNCTION  : VIDEO_PRESENT							|
;+------------------------------------------------------------------------------+	
;| Description:									|
;|		Check if video signal present and show animation and message	|
;|		according your state						|
;|										|
;+------------------------------------------------------------------------------+
;| Input    : None								|
;| Output   : cy=1 if video no present						|
;+------------------------------------------------------------------------------+

	;/////////////////////
	;// ANIMATIONS BMPS
	;/////////////////////
	.INCLUDE	"BMPS\BMP_BNC_MACHO_ANI_1.INC"		;NO VIDEO INPUT ANIMATION 1..6
	.INCLUDE	"BMPS\BMP_BNC_MACHO_ANI_2.INC"
	.INCLUDE	"BMPS\BMP_BNC_MACHO_ANI_3.INC"
	.INCLUDE	"BMPS\BMP_BNC_MACHO_ANI_4.INC"
	.INCLUDE	"BMPS\BMP_BNC_MACHO_ANI_5.INC"
	.INCLUDE	"BMPS\BMP_BNC_MACHO_ANI_6.INC"
	.INCLUDE	"BMPS\BMP_BNC_OK_ANI_1.INC"			;VIDEO PRESENT ANIMATION 1..4
	.INCLUDE	"BMPS\BMP_BNC_OK_ANI_2.INC"
	.INCLUDE	"BMPS\BMP_BNC_OK_ANI_3.INC"
	.INCLUDE	"BMPS\BMP_BNC_OK_ANI_4.INC"

fn_VIDEO_PRESENT:
	call	_VIDEO_OUT_GET_SYNC_STATE			;get sync state
	cpi	Acc,_ON						;present
	brne	fn_VIDEO_PRESENT_10				;no, branch
	clc
	ret							;else return
fn_VIDEO_PRESENT_10:
	call	_KEYBOARD_READ_FLASH				;wait keyboard release
	cpi	Acc,_KEY_NONE					;none key pressed?
	brne	fn_VIDEO_PRESENT_10				;no, wait
	call	_KEYBOARD_DEBOUNCE
	call	_RASTER_GET_CHAR_MODE				;get actual mode
	push	Acc						;save
	ldi	Acc,_RASTER_CHAR_MODE_NORMAL			;set normal mode
	call	_RASTER_SET_CHAR_MODE
	call	_VIDEO_OUT_CLS					;clear screen
	ldiw	X,10						
	ldiw	Y,0
	ldiw	Z,_MSG_NO_SIGNAL				;show no video signal message
	call	_MENU_TRANSLATE_MESSAGE
	call	_RASTER_SET_TEXT_OUT
	ldiw	Z,BMP_BNC_MACHO_ANI_1*2				;show no video signal animation
	call	_RASTER_BMP_GET_WIDTH				;center animation on screen
	ldiw	X,SCREEN_WIDTH
	sub	XL,Acc
	sbc	XH,AccH
	lsr	XH
	ror	XL
	call	_RASTER_BMP_GET_HEIGHT
	ldiw	Y,SCREEN_HEIGHT
	sub	YL,Acc
	sbc	YH,AccH
	lsr	YH
	ror	YL
fn_VIDEO_PRESENT_20:
	ldiw	Z,ANIMATION_NO_VIDEO_SIGNAL*2			;show no video present animation
	ldiaw	ANIMATION_TIME
	rcall	fn_ANIMATION
	brcs	fn_VIDEO_PRESENT_40				;exit if any key pressed
	call	_VIDEO_OUT_GET_SYNC_STATE			;get sync state
	cpi	Acc,_ON						;present ?
	brne	fn_VIDEO_PRESENT_20				;no, continue show animation
	pushw	X
	pushw	Y
	ldiw	X,10						
	ldiw	Y,0
	ldiw	Z,_MSG_SIGNAL_PRESENT				;show no video signal message
	call	_MENU_TRANSLATE_MESSAGE
	call	_RASTER_SET_TEXT_OUT
	popw	Y
	popw	X
	addiw	X,10
	addiw	Y,8
	ldi	Acc,5						;5 times
fn_VIDEO_PRESENT_30:
	push	Acc
	ldiw	Z,ANIMATION_VIDEO_PRESENT*2			;show video present animation
	ldiaw	ANIMATION_TIME
	rcall	fn_ANIMATION
	pop	Acc
	dec	Acc
	brne	fn_VIDEO_PRESENT_30
	call	_VIDEO_OUT_CLS					;clear screen before exit
	pop	Acc						;restore old mode
	call	_RASTER_SET_CHAR_MODE
	clc							;set video present
	ret
fn_VIDEO_PRESENT_40:
	pop	Acc						;restore old mode
	call	_RASTER_SET_CHAR_MODE
	sec							;set cy=1 to return
	ret


;+------------------------------------------------------------------------------+
;| FUNCTION  : SET_GRID_CELL							|
;+------------------------------------------------------------------------------+	
;| Description:									|
;|		Set capture grid state ON or OFF				|
;|										|
;+------------------------------------------------------------------------------+
;| Input    : AccT	row							|
;|	      AccTH	col							|
;|	      Temp	_ON/_OFF						|
;| Output   : None								|
;+------------------------------------------------------------------------------+
fn_SET_GRID_CELL:
	pushw	X
	pushw	Y
	pushw	Z
	push	Acc
	ldiw	Z,_VIDEO_DIGITIZING_MASK_AREAS
	ldi	XH,_VIDEO_DIGITIZING_MASK_COLS
	ldi	YH,_VIDEO_DIGITIZING_MASK_ROWS
	mov	XL,AccTH
	mov	YL,AccT
	mov	Acc,Temp
	call	_RASTER_SET_BIT_XY
	pop	Acc
	popw	Z
	popw	Y
	popw	X
	ret
;+------------------------------------------------------------------------------+
;| FUNCTION  : GET_GRID_CELL							|
;+------------------------------------------------------------------------------+	
;| Description:									|
;|		Get capture grid state ON or OFF				|
;|										|
;+------------------------------------------------------------------------------+
;| Input    : AccT	row							|
;|	      AccTH	col							|
;| Output   : Temp	_ON/_OFF						|
;+------------------------------------------------------------------------------+
fn_GET_GRID_CELL:
	pushw	X
	pushw	Y
	pushw	Z
	push	Acc
	ldiw	Z,_VIDEO_DIGITIZING_MASK_AREAS
	ldi	XH,_VIDEO_DIGITIZING_MASK_COLS
	ldi	YH,_VIDEO_DIGITIZING_MASK_ROWS
	mov	XL,AccTH
	mov	YL,AccT
	call	_RASTER_GET_BIT_XY
	mov	Temp,Acc
	pop	Acc
	popw	Z
	popw	Y
	popw	X
	ret


;+------------------------------------------------------------------------------+
;| FUNCTION  : COMPUTE_GRID_CELL_POSITION					|
;+------------------------------------------------------------------------------+	
;| Description:									|
;|		Compute capture grid cell position on screen			|
;|										|
;+------------------------------------------------------------------------------+
;| Input    : AccT	row							|
;|	      AccTH	col							|
;| Output   : X,Y	COORDINATES						|
;+------------------------------------------------------------------------------+
fn_COMPUTE_GRID_CELL_POSITION:
	pushaw
	pushawt
	push	Temp
	pushawt
	ldiw	X,0					;select COL
	ldiaw	0					;diference control SCREEN_WIDTH/COLS
fn_COMPUTE_GRID_CELL_POSITION_10:
	cpi	AccTH,0					;on position ?
	breq	fn_COMPUTE_GRID_CELL_POSITION_10B	;yes, branch
	dec	AccTH
fn_COMPUTE_GRID_CELL_POSITION_10A:
	addiw	X,1
	addiaw	_VIDEO_DIGITIZING_MASK_COLS				;f+=COLS
	cpiaw	SCREEN_WIDTH				;f>=SCREEN_WIDTH?
	brlo	fn_COMPUTE_GRID_CELL_POSITION_10A	;yes, branch
	subiaw	SCREEN_WIDTH
	rjmp	fn_COMPUTE_GRID_CELL_POSITION_10
fn_COMPUTE_GRID_CELL_POSITION_10B:
	ldiw	Y,0					;select row
	ldiaw	0					;diference control SCREEN_HEIGHT/ROWS
fn_COMPUTE_GRID_CELL_POSITION_20:
	cpi	AccT,0					;on position ?
	breq	fn_COMPUTE_GRID_CELL_POSITION_20B	;yes, branch
	dec	AccT
fn_COMPUTE_GRID_CELL_POSITION_20A:
	addiw	Y,1
	addiaw	_VIDEO_DIGITIZING_MASK_ROWS		;f+=ROWS
	cpiaw	SCREEN_HEIGHT				;f>=SCREEN_HEIGHT?
	brlo	fn_COMPUTE_GRID_CELL_POSITION_20A	;yes, branch
	subiaw	SCREEN_HEIGHT
	rjmp	fn_COMPUTE_GRID_CELL_POSITION_20
fn_COMPUTE_GRID_CELL_POSITION_20B:
	popawt
	cpi	AccT,_VIDEO_DIGITIZING_MASK_ROWS	;MAX ROW?
	brne	fn_COMPUTE_GRID_CELL_POSITION_30	;no, branch
	subiw	Y,1					;else y--
fn_COMPUTE_GRID_CELL_POSITION_30:
	cpi	AccTH,_VIDEO_DIGITIZING_MASK_COLS	;MAX COL?
	brne	fn_COMPUTE_GRID_CELL_POSITION_40	;no, branch
	subiw	X,1					;else y--
fn_COMPUTE_GRID_CELL_POSITION_40:
	pop	Temp
	popawt
	popaw
	ret
	
;+------------------------------------------------------------------------------+
;| FUNCTION  : DRAW_CAPTURE_GRID						|
;+------------------------------------------------------------------------------+	
;| Description:									|
;|		Draw capture grid cells on screen				|
;|										|
;+------------------------------------------------------------------------------+
;| Input    : None								|
;|	      None								|
;| Output   : None								|
;+------------------------------------------------------------------------------+
fn_DRAW_CAPTURE_GRID:
	pushw	X
	pushw	Y
	pushaw
	pushawt
	;///////////////////////////////
	;// DRAW BACKGROUND BOX
	;///////////////////////////////
	ldiw	X,0			;draw border
	ldiw	Y,0
	ldiaw	SCREEN_WIDTH
	ldiawt	SCREEN_HEIGHT
	ldi	Temp,_ON
	call	_RASTER_SET_BOX
	;///////////////////////////////
	;// DRAW VERTICAL LINES
	;///////////////////////////////
	ldi	AccT,0
	ldi	AccTH,_VIDEO_DIGITIZING_MASK_COLS-1
fn_DRAW_CAPTURE_10:
	call	fn_COMPUTE_GRID_CELL_POSITION
	pushawt
	ldiw	Y,0
	ldiawt	SCREEN_HEIGHT
	ldi	Acc,_ON
	call	_RASTER_SET_VLINE
	popawt
	dec	AccTH
	brne	fn_DRAW_CAPTURE_10
	;///////////////////////////////
	;// DRAW HORIZONTAL LINES
	;///////////////////////////////
	ldi	AccT,_VIDEO_DIGITIZING_MASK_ROWS-1
	ldi	AccTH,0
fn_DRAW_CAPTURE_20:
	call	fn_COMPUTE_GRID_CELL_POSITION
	pushawt
	ldiw	X,0
	ldiawt	SCREEN_WIDTH
	ldi	Acc,_ON
	call	_RASTER_SET_HLINE
	popawt
	dec	AccT
	brne	fn_DRAW_CAPTURE_20
	;///////////////////////////////
	;// DRAW SELECTED AREAS
	;///////////////////////////////
	ldi	AccT,0
	ldi	AccTH,0
fn_DRAW_CAPTURE_30:
	call	fn_GET_GRID_CELL			;get state
	call	fn_DRAW_CAPTURE_GRID_CELL		;draw cell state
	inc	AccT
	cpi	AccT,_VIDEO_DIGITIZING_MASK_COLS
	brlo	fn_DRAW_CAPTURE_30
	ldi	AccT,0
	inc	AccTH
	cpi	AccTH,_VIDEO_DIGITIZING_MASK_ROWS
	brlo	fn_DRAW_CAPTURE_30
	popawt
	popaw
	popw	Y
	popw	X
	ret
;+------------------------------------------------------------------------------+
;| FUNCTION  : DRAW_CAPTURE_GRID_SELECTION					|
;+------------------------------------------------------------------------------+	
;| Description:									|
;|		Draw Capture grid Cells selection				|
;|										|
;+------------------------------------------------------------------------------+
;| Input    : AccT  row set							|
;|	      AccTH col set							|
;|	      Temp  _ON/_OFF							|
;| Output   : None								|
;+------------------------------------------------------------------------------+
fn_DRAW_CAPTURE_GRID_SELECTION:
	pushw	X
	pushw	Y
	pushaw
	pushawt

	call	fn_COMPUTE_GRID_CELL_POSITION
	addiw	X,1
	addiw	Y,1
	pushw	X
	pushw	Y
	inc	AccT
	inc	AccTH
	call	fn_COMPUTE_GRID_CELL_POSITION
	subiw	X,1
	subiw	Y,1
	movw	Acc,XL
	movw	AccT,YL
	popw	Y
	popw	X

	sub	Acc,XL				;compute width and height
	sbc	AccH,XH
	addiaw	1
	sub	AccT,YL
	sbc	AccTH,YH
	addiawt	1
	call	_RASTER_SET_BOX

	popawt
	popaw
	popw	Y
	popw	X
	ret

;+------------------------------------------------------------------------------+
;| FUNCTION  : DRAW_CAPTURE_GRID_CELL						|
;+------------------------------------------------------------------------------+	
;| Description:									|
;|		Draw capture grid cell state ON or OFF				|
;|										|
;+------------------------------------------------------------------------------+
;| Input    : AccT  row set							|
;|	      AccTH col set							|
;|	      Temp  _ON/_OFF							|
;| Output   : None								|
;+------------------------------------------------------------------------------+
fn_DRAW_CAPTURE_GRID_CELL:
	pushw	X
	pushw	Y
	pushaw
	pushawt

	call	fn_COMPUTE_GRID_CELL_POSITION
	addiw	X,2
	addiw	Y,2
	pushw	X
	pushw	Y
	inc	AccT
	inc	AccTH
	call	fn_COMPUTE_GRID_CELL_POSITION
	subiw	X,1
	subiw	Y,1
	movw	Acc,XL
	movw	AccT,YL
	popw	Y
	popw	X

	sub	Acc,XL				;abs(xf-xi)
	sbc	AccH,XH
	
	sub	AccT,YL				;abs(yf-yi)
	sbc	AccTH,YH
		
	call	_RASTER_BITBLT_SET_DEST	
	ldi	Acc,_RASTER_BITBLT_FUNCTION_WHITE
	cpi	Temp,_ON
	breq	PC+2
	ldi	Acc,_RASTER_BITBLT_FUNCTION_BLACK
	call	_RASTER_BITBLT

	popawt
	popaw
	popw	Y
	popw	X
	ret
;+------------------------------------------------------------------------------+
;| FUNCTION  : LOAD_CAPTURE_GRID						|
;+------------------------------------------------------------------------------+	
;| Description:									|
;|		Load capture grid from EEPROM					|
;|										|
;+------------------------------------------------------------------------------+
;| Input    : None								|
;| Output   : None								|
;+------------------------------------------------------------------------------+
fn_LOAD_CAPTURE_GRID:
	pushaw
	pushw	X
	pushw	Y
	ldiw	X,_VIDEO_DIGITIZING_MASK_AREAS		;X--> SRAM ADDRESS
	ldiw	Y,E_VIDEO_DIGITIZING_MASK_AREAS		;Y--> EEPROM ADDRESS
	ldiaw	_VIDEO_DIGITIZING_MASK_SIZE		;size
	call	_EEPROM_R_STR_SRAM			;read
fn_LOAD_CAPTURE_GRID_EXIT:
	popw	Y
	popw	X
	popaw
	ret

;+------------------------------------------------------------------------------+
;| FUNCTION  : LOAD_MENU							|
;+------------------------------------------------------------------------------+	
;| Description:									|
;|		Load MENU from EEPROM						|
;|										|
;+------------------------------------------------------------------------------+
;| Input    : None								|
;| Output   : None								|
;+------------------------------------------------------------------------------+
fn_LOAD_MENU:
	pushaw
	pushw	X
	pushw	Y
	ldiw	Y,E_MENU				;Y--> EEPROM address
	ldiw	X,_MENU_ADDR_START			;X--> SRAM ADDRESS
	ldiaw	_MENU_SIZE				;size
	call	_EEPROM_R_STR_SRAM			;read data
	rjmp	fn_LOAD_CAPTURE_GRID_EXIT

;+------------------------------------------------------------------------------+
;| FUNCTION  : SAVE_CAPTURE_GRID						|
;+------------------------------------------------------------------------------+	
;| Description:									|
;|		Save capture grid into EEPROM					|
;|										|
;+------------------------------------------------------------------------------+
;| Input    : None								|
;| Output   : None								|
;+------------------------------------------------------------------------------+
fn_SAVE_CAPTURE_GRID:
	pushaw
	pushw	X
	pushw	Y
	ldiw	X,_VIDEO_DIGITIZING_MASK_AREAS		;X--> SRAM address
	ldiw	Y,E_VIDEO_DIGITIZING_MASK_AREAS		;Y--> EEPROM address
	ldiaw	_VIDEO_DIGITIZING_MASK_SIZE		;size
	call	_EEPROM_W_STR_SRAM			;write EEPROM data
	rjmp	fn_LOAD_CAPTURE_GRID_EXIT

;+------------------------------------------------------------------------------+
;| FUNCTION  : SAVE_MENU							|
;+------------------------------------------------------------------------------+	
;| Description:									|
;|		Save MENU into EEPROM						|
;|										|
;+------------------------------------------------------------------------------+
;| Input    : None								|
;| Output   : None								|
;+------------------------------------------------------------------------------+
fn_SAVE_MENU:
	pushaw
	pushw	X
	pushw	Y
	ldiw	X,_MENU_ADDR_START			;X--> SRAM address
	ldiw	Y,E_MENU				;Y--> EEPROM address
	ldiaw	_MENU_SIZE				;size
	call	_EEPROM_W_STR_SRAM			;write
	rjmp	fn_LOAD_CAPTURE_GRID_EXIT
;+------------------------------------------------------------------------------+
;| FUNCTION  : SAVE_MESSAGE							|
;+------------------------------------------------------------------------------+	
;| Description:									|
;|		Save message into EEPROM					|
;|										|
;+------------------------------------------------------------------------------+
;| Input    : Y-->EEPROM MESSAGE AREA						|
;| Output   : None								|
;+------------------------------------------------------------------------------+
fn_SAVE_MESSAGE:
	pushaw
	pushw	X
	pushw	Y
	ldiw	X,KEYBOARD_INPUT_EDIT_BUFFER		;X--> SRAM address
	ldiaw	KEYBOARD_INPUT_EDIT_BUFFER_SIZE		;size
	call	_EEPROM_W_STR_SRAM			;write data
	lds	Acc,KEYBOARD_INPUT_MODE			;get actual mode
	call	_EEPROM_W_BYTE				;write in eeprom
	rjmp	fn_LOAD_CAPTURE_GRID_EXIT

;+------------------------------------------------------------------------------+
;| FUNCTION  : LOAD_MESSAGE							|
;+------------------------------------------------------------------------------+	
;| Description:									|
;|		Load message from EEPROM					|
;|										|
;+------------------------------------------------------------------------------+
;| Input    : X-->EEPROM MESSAGE AREA						|
;| Output   : None								|
;+------------------------------------------------------------------------------+
fn_LOAD_MESSAGE:
	pushaw
	pushw	X
	pushw	Y
	movw	YL,XL					;Y-->EEPROM ADDRESS
	ldiw	X,KEYBOARD_INPUT_EDIT_BUFFER		;X--> SRAM ADDRESS
	ldiaw	KEYBOARD_INPUT_EDIT_BUFFER_SIZE		;size
	call	_EEPROM_R_STR_SRAM			;read data
	call	_EEPROM_R_BYTE				;read byte
	sts	KEYBOARD_INPUT_MODE,Acc			;set mode
	rjmp	fn_LOAD_CAPTURE_GRID_EXIT
;+------------------------------------------------------------------------------+
;| FUNCTION  : fn_INPUT_BOX_CONFIRM						|
;+------------------------------------------------------------------------------+	
;| Description:									|
;|	show input box with confirm message yes or no				|
;|										|
;+------------------------------------------------------------------------------+
;| Input    : Z message (will be translated before)				|
;| Output   : Acc Input box key code						|
;+------------------------------------------------------------------------------+
fn_INPUT_BOX_CONFIRM:
	pushw	Z
	pushw	X
	pushw	Y
	call	_RASTER_GET_CHAR_MODE				;get actual mode
	push	Acc						;save it
	ldi	Acc,_RASTER_CHAR_MODE_INVERTED			;set inverted mode
	call	_RASTER_SET_CHAR_MODE
;	ldiw	Z,_MSG_CONFIRM					;confirm msg?
	call	_MENU_TRANSLATE_MESSAGE				;translate
	call	_RASTER_GET_TEXT_WIDTH				;get width
	ldiw	X,SCREEN_WIDTH					;show confirm msg in midle screen
	sub	XL,Acc
	sbc	XH,AccH
	lsr	XH
	ror	XL
	ldiw	Y,32
	ldi	Acc,_MENU_INPUT_BOX_TYPE_YES_NO			;INPUT BOX YES, NO
	call	_MENU_INPUT_BOX
	mov	AccH,Acc					;copy code to AccH
	pop	Acc
	call	_RASTER_SET_CHAR_MODE				;restore old mode
	mov	Acc,AccH					;get code
	popw	Y
	popw	X
	popw	Z
	ret

;+------------------------------------------------------------------------------+
;| FUNCTION  : KEYBOARD_INPUT							|
;+------------------------------------------------------------------------------+	
;| Description:									|
;|	Get ASCII input codes from KEYBOARD PANEL				|
;|										|
;+------------------------------------------------------------------------------+
;| Input    : X,Y KEYBOARD PANEL COORDINATES					|
;| Output   : Acc Ascii Key							|
;+------------------------------------------------------------------------------+

	;//////////////////////////////////
	;// KEYBOARD BMPS AND SCROLL BAR
	;//////////////////////////////////

	.INCLUDE	"BMPS\BMP_KEYBOARD_5.INC"			;INPUT KEYBOARD
	.INCLUDE	"BMPS\BMP_KEYBOARD_SPECIAL_1.INC"		;INPUT KEYBOARD SPECIAL CHARACTERS
	.INCLUDE	"BMPS\BMP_TEXT_SCROLL_BAR_1.INC"		;TEXT SCROLL BAR
	.INCLUDE	"BMPS\BMP_TEXT_SCROLL_BAR_BUTTON_1.INC"		;TEXT SCROLL BAR BUTTON

	;/////////////////////////////////
	;//				//
	;// Initialize keyboard panel 	//
	;//				//
	;/////////////////////////////////
	fn_KEYBOARD_INPUT_INIT:
		push	Acc
		ldi	Acc,2				;positione at key "A"
		sts	KEYBOARD_INPUT_ROW,Acc
		ldi	Acc,1
		sts	KEYBOARD_INPUT_COL,Acc
		ldi	Acc,_ON				;caps ON
		sts	KEYBOARD_INPUT_CAPS,Acc
		ldi	Acc,1
		sts	KEYBOARD_INPUT_BAR_ROW,Acc
		pop	Acc
		ret
	;/////////////////////////////////
	;//				//
	;// Perform keyboard data input	//
	;//				//
	;/////////////////////////////////
	fn_KEYBOARD_INPUT:
		pushw	X
		pushw	Y
		pushw	Z
		push	AccH
		pushawt
		;/////////////////////////////////
		;// store coordinates x,y	//
		;// initialize row,col if needed//
		;/////////////////////////////////
		stsw	KEYBOARD_INPUT_X,X				;store keyboard coordinates
		stsw	KEYBOARD_INPUT_Y,Y
		lds	Acc,KEYBOARD_INPUT_ROW				;check if row is valid
		cpi	Acc,1						;row<1
		brlo	fn_KEYBOARD_INPUT_10				;yes, branch
		cpi	Acc,(KEYBOARD_INPUT_ROWS+1)			;row>max rows?
		brsh	fn_KEYBOARD_INPUT_10				;yes, branch
		rjmp	fn_KEYBOARD_INPUT_20				;
	fn_KEYBOARD_INPUT_10:
		ldi	Acc,2						;set row to 2
		sts	KEYBOARD_INPUT_ROW,Acc
	fn_KEYBOARD_INPUT_20:
		lds	Acc,KEYBOARD_INPUT_COL				;check if col is valid
		cpi	Acc,1						;col<1 ?
		brlo	fn_KEYBOARD_INPUT_30				;yes, branch
		cpi	Acc,(KEYBOARD_INPUT_COLS+1)			;col>max col?
		brsh	fn_KEYBOARD_INPUT_30				;yes, branch
		rjmp	fn_KEYBOARD_INPUT_40
	fn_KEYBOARD_INPUT_30:
		ldi	Acc,1						;set col to 1
		sts	KEYBOARD_INPUT_COL,Acc
	fn_KEYBOARD_INPUT_40:
		;///////////////////
		;// DRAW KEYBOARD //
		;///////////////////
	fn_KEYBOARD_INPUT_LOOP:
		ldsw	X,KEYBOARD_INPUT_X				;get panel coordinates
		ldsw	Y,KEYBOARD_INPUT_Y
		ldiw	Z,BMP_KEYBOARD_5*2				;Z-->BMP.Keyboard
		call 	_RASTER_SET_BMP_FAST				;draw keyboard
		call	fn_KEYBOARD_INPUT_CAPS				;draw caps keys
		call	fn_KEYBOARD_INPUT_GET_MATRIZ_PARMS		;get parms
		mov	AccT,Acc					;AccT=ASCII code
		ldi	Temp,_OFF
		call	_RASTER_SET_BOX_FAST				;draw selection box
		subi	XL,1
		subi	YL,1
		subi	XH,-2
		subi	YH,-2
		ldi	Temp,_OFF
		call	_RASTER_SET_BOX_FAST
		subi	XL,1
		subi	YL,1
		subi	XH,-2
		subi	YH,-2
		ldi	Temp,_ON
		call	_RASTER_SET_BOX_FAST
		;////////////////////////////////////////////
		;// GET COMMAND FROM KEYBOARD COMMAND KEYS //
		;////////////////////////////////////////////
		call	_KEYBOARD_READ					;read keyboard
		cpi	Acc,_KEY_UP					;KEY UP ?
		breq	fn_KEYBOARD_INPUT_KEY_UP			;yes, process
		cpi	Acc,_KEY_DOWN					;KEY DOWN ?
		breq	fn_KEYBOARD_INPUT_KEY_DOWN			;yes, process
		cpi	Acc,_KEY_LEFT					;KEY LEFT ?
		breq	fn_KEYBOARD_INPUT_KEY_LEFT			;yes, process
		cpi	Acc,_KEY_RIGHT					;KEY RIGHT ?
		breq	fn_KEYBOARD_INPUT_KEY_RIGHT			;yes, process
		cpi	Acc,_KEY_ENTER					;KEY ENTER ?
		breq	fn_KEYBOARD_INPUT_KEY_ENTER			;yes, process
		rjmp	fn_KEYBOARD_INPUT_LOOP				;none of above redraw keyboard
		;////////////////////
		;// PROCESS KEY UP //	
		;////////////////////
	fn_KEYBOARD_INPUT_KEY_UP:
		lds	Acc,KEYBOARD_INPUT_ROW				;get row
		cpi	Acc,1						;at first row ?
		brne	fn_KEYBOARD_INPUT_KEY_UP_10			;no, branch
		ldi	Acc,KEYBOARD_INPUT_ROWS				;else set to last row
		rjmp	fn_KEYBOARD_INPUT_KEY_UP_20
	fn_KEYBOARD_INPUT_KEY_UP_10:
		dec	Acc						;row--
	fn_KEYBOARD_INPUT_KEY_UP_20:
		sts	KEYBOARD_INPUT_ROW,Acc
		rjmp	fn_KEYBOARD_INPUT_LOOP				;draw keyboard
		;//////////////////////
		;// PROCESS KEY DOWN //	
		;//////////////////////
	fn_KEYBOARD_INPUT_KEY_DOWN:
		lds	Acc,KEYBOARD_INPUT_ROW				;get row
		cpi	Acc,KEYBOARD_INPUT_ROWS				;at last row ?
		brne	fn_KEYBOARD_INPUT_KEY_DOWN_10				;no, branch
		ldi	Acc,1						;else set to first row
		rjmp	fn_KEYBOARD_INPUT_KEY_DOWN_20
	fn_KEYBOARD_INPUT_KEY_DOWN_10:
		inc	Acc						;row++
	fn_KEYBOARD_INPUT_KEY_DOWN_20:
		sts	KEYBOARD_INPUT_ROW,Acc
		rjmp	fn_KEYBOARD_INPUT_LOOP				;draw keyboard
		;//////////////////////
		;// PROCESS KEY LEFT //	
		;//////////////////////
	fn_KEYBOARD_INPUT_KEY_LEFT:
		cpi	AccT,' '					;on space key?
		brne	fn_KEYBOARD_INPUT_KEY_LEFT_10A
		ldi	Acc,5						;set on 'Z' row=5,col=2
		sts	KEYBOARD_INPUT_ROW,Acc
		ldi	Acc,2
		sts	KEYBOARD_INPUT_COL,Acc
		rjmp	fn_KEYBOARD_INPUT_LOOP
	fn_KEYBOARD_INPUT_KEY_LEFT_10A:
		lds	Acc,KEYBOARD_INPUT_COL				;get col
		cpi	Acc,1						;at first col ?
		brne	fn_KEYBOARD_INPUT_KEY_LEFT_10				;no, branch
		ldi	Acc,KEYBOARD_INPUT_COLS				;else set to last col
		rjmp	fn_KEYBOARD_INPUT_KEY_LEFT_20
	fn_KEYBOARD_INPUT_KEY_LEFT_10:
		dec	Acc						;col--
	fn_KEYBOARD_INPUT_KEY_LEFT_20:
		sts	KEYBOARD_INPUT_COL,Acc
		rjmp	fn_KEYBOARD_INPUT_LOOP				;draw keyboard
		;///////////////////////
		;// PROCESS KEY RIGHT //	
		;///////////////////////
	fn_KEYBOARD_INPUT_KEY_RIGHT:
		cpi	AccT,' '					;on space key?
		brne	fn_KEYBOARD_INPUT_KEY_RIGHT_10A
		ldi	Acc,5						;set on KEY_ENTER row=5,col=9
		sts	KEYBOARD_INPUT_ROW,Acc
		ldi	Acc,9
		sts	KEYBOARD_INPUT_COL,Acc
		rjmp	fn_KEYBOARD_INPUT_LOOP
	fn_KEYBOARD_INPUT_KEY_RIGHT_10A:
		lds	Acc,KEYBOARD_INPUT_COL				;get col
		cpi	Acc,KEYBOARD_INPUT_COLS				;at last col ?
		brne	fn_KEYBOARD_INPUT_KEY_RIGHT_10			;no, branch
		ldi	Acc,1						;else set to first row
		rjmp	fn_KEYBOARD_INPUT_KEY_RIGHT_20
	fn_KEYBOARD_INPUT_KEY_RIGHT_10:
		inc	Acc						;col++
	fn_KEYBOARD_INPUT_KEY_RIGHT_20:
		sts	KEYBOARD_INPUT_COL,Acc
		rjmp	fn_KEYBOARD_INPUT_LOOP				;draw keyboard
		;///////////////////////
		;// PROCESS KEY ENTER //	
		;///////////////////////
	fn_KEYBOARD_INPUT_KEY_ENTER:
		mov	Acc,AccT					;get ASCII code
		popawt
		pop	AccH
		popw	Z
		popw	Y
		popw	X
		ret

	;/////////////////////////////////////////////////////////
	;//							//
	;// keyboard draw caps					//
	;//							//
	;// input   : KEYBOARD_INPUT_CAPS			//
	;//							//
	;/////////////////////////////////////////////////////////
	fn_KEYBOARD_INPUT_CAPS:
		pushw	X
		pushw	Y
		push	Acc
		call	_RASTER_GET_CHAR_MODE			;get mode
		pushaw	
		ldi	Acc,_RASTER_CHAR_MODE_INVERTED		;set normal mode
		call	_RASTER_SET_CHAR_MODE
		lds	Acc,KEYBOARD_INPUT_CAPS			;CAP ON?
		cpi	Acc,_ON
		breq	fn_KEYBOARD_INPUT_CAPS_EXIT		;yes, exit
		lds	Acc,KEYBOARD_INPUT_ROW			;save row and col
		lds	AccH,KEYBOARD_INPUT_COL
		pushaw
		ldi	Acc,2					;set to first col
		sts	KEYBOARD_INPUT_ROW,Acc
	fn_KEYBOARD_INPUT_CAPS_10:
		ldi	Acc,1					;set to second row
		sts	KEYBOARD_INPUT_COL,Acc
	fn_KEYBOARD_INPUT_CAPS_20:
		call	fn_KEYBOARD_INPUT_CAPS_PLOT		;plot char
		lds	Acc,KEYBOARD_INPUT_COL			;col++
		inc	Acc
		sts	KEYBOARD_INPUT_COL,Acc
		cpi	Acc,9					;col=8?
		brlo	fn_KEYBOARD_INPUT_CAPS_20		;no, until reach
		lds	Acc,KEYBOARD_INPUT_ROW
		inc	Acc					;row++
		sts	KEYBOARD_INPUT_ROW,Acc
		cpi	Acc,5					;col=4
		brlo	fn_KEYBOARD_INPUT_CAPS_10		;no, until reach
		ldi	Acc,1
		sts	KEYBOARD_INPUT_COL,Acc			;col=1
		call	fn_KEYBOARD_INPUT_CAPS_PLOT		;plot char y
		ldi	Acc,2
		sts	KEYBOARD_INPUT_COL,Acc			;col=2
		call	fn_KEYBOARD_INPUT_CAPS_PLOT		;plot char y
		popaw
		sts	KEYBOARD_INPUT_ROW,Acc			;restore row and col
		sts	KEYBOARD_INPUT_COL,AccH
	fn_KEYBOARD_INPUT_CAPS_EXIT:
		popaw
		call	_RASTER_SET_CHAR_MODE			;set old mode
		pop	Acc
		popw	Y
		popw	X
		ret
		;//////////////
		;// PLOT CHAR
		;//////////////
		fn_KEYBOARD_INPUT_CAPS_PLOT:
			call	fn_KEYBOARD_INPUT_GET_MATRIZ_PARMS	;get parms from keyboard matriz
			subi	XL,-2
			subi	YL,-2
			ldi	XH,0
			ldi	YH,0
			call	_RASTER_SET_CHAR			;set char on screen
			ret
	;/////////////////////////////////////////////////////////
	;//							//
	;// keyboard get data from coordinates row,col		//
	;//							//
	;// input   : KEYBOARD_INPUT_ROW			//
	;//	      KEYBOARD_INPUT_COL			//
	;// output:   Acc        ASCII code or control		//
	;//	      XL,YL      Box coordinate of selected cell//
	;//	      XH	 Box width of selected cell	//
	;//	      YH	 Box height of selected cell	//	
	;/////////////////////////////////////////////////////////
	fn_KEYBOARD_INPUT_GET_MATRIZ_PARMS:
		push	AccH
		pushawt
		pushw	Z
		ldiw	Z,fn_KEYBOARD_MATRIZ*2				;Z-->KEYBOARD.MATRIZ
		lpm	XH,Z+						;get width
		lpm	YH,Z+						;get height
		ldi	AccT,KEYBOARD_INPUT_COLS*2+2			;keyboard rows(char,col)+row+dummy
		lds	AccTH,KEYBOARD_INPUT_ROW			;positione row
		dec	AccTH						;adjust to 0..N
		mul	AccTH,AccT
		add	ZL,r0
		adc	ZH,r1
		lpm	YL,Z+						;get Y position
		lds	AccT,KEYBOARD_INPUT_COL				;positione col
		dec	AccT						;adjust to 0..N
		lsl	AccT						;by two (char,col)
		add	ZL,AccT
		ldi	AccT,0
		adc	ZH,AccT
		lpm	Acc,Z+						;Acc=CHAR
		lpm	XL,Z+						;get X position
		lds	AccT,KEYBOARD_INPUT_X				;get panel X position
		add	XL,AccT						;update character position X
		lds	AccT,KEYBOARD_INPUT_Y				;get panel Y position
		add	YL,AccT						;update character position Y
		cpi	Acc,' '						;is space
		brne	fn_KEYBOARD_INPUT_GET_MATRIZ_PARMS_10		;no, branch
		ldi	XL,17						;start X and Width of space bar
		ldi	XH,50
	fn_KEYBOARD_INPUT_GET_MATRIZ_PARMS_10:
		lds	AccH,KEYBOARD_INPUT_CAPS			;get caps state
		cpi	AccH,_ON					;ON ?
		breq	fn_KEYBOARD_INPUT_GET_MATRIZ_PARMS_EXIT		;yes, exit
		cpi	Acc,'A'						;else code in A..Z
		brlo	fn_KEYBOARD_INPUT_GET_MATRIZ_PARMS_EXIT		;exit if out of range
		cpi	Acc,'Z'+1
		brsh	fn_KEYBOARD_INPUT_GET_MATRIZ_PARMS_EXIT		;exit if out of range
		ori	Acc,(1<<5)					;else convert to lower case	
	fn_KEYBOARD_INPUT_GET_MATRIZ_PARMS_EXIT:
		popw	Z
		popawt	
		pop	AccH
		ret

	;/////////////////////////////////////
	;// KEYBOARD PANEL CHARACTERS CODES
	;/////////////////////////////////////
	.EQU	K_CH_EXIT	= 0XFF
	.EQU	K_CH_CAPS	= 0XFE
	.EQU	K_CH_OMEGA	= 0XFD
	.EQU	K_CH_HOME	= 0XFC
	.EQU	K_CH_END	= 0XFB
	.EQU	K_CH_NEW	= 0XFA
	.EQU	K_CH_PREVIEW	= 0XF9
	.EQU	K_CH_LEFT	= 0XF8
	.EQU	K_CH_RIGHT	= 0XF7
	.EQU	K_CH_UP		= 0XF6
	.EQU	K_CH_DOWN	= 0XF5
	.EQU	K_CH_ENTER	= 0XF4
	.EQU	K_CH_MODE	= 0XF3
	.EQU	K_CH_NONE	= 0X00
	.EQU	K_CH_BACK	= 0X08
	;/////////////////////////////////////
	;// KEYBOARD PANEL CHARACTERS MATRIZ, WITH RELATIVE POSITION,WIDTH AND HEIGHT OF CELL
	;/////////////////////////////////////
	fn_KEYBOARD_MATRIZ:
	;	width,height
	.DB	    9,11
	;		  1        2        3        4        5        6        7        8              9             10               11       12       13       14            15
	;	row,char,col,char,col,char,col,char,col,char,col,char,col,char,col,char,col,char      ,col,char      ,col,char        ,col,char,col,char,col,char      ,col,char     ,col,dummy
	.DB	  2,"!" ,  3,34  , 11,"#" , 19,"$" , 27,"%" , 35,"&" , 43,"(" , 51,")" , 59,"<"       , 67,"?"       , 75,">"         , 83,"[" , 91,"]" , 99,K_CH_MODE ,112,K_CH_EXIT,120,0
	.DB	 12,"A" ,  1,"B" ,  9,"C" , 17,"D" , 25,"E" , 33,"F" , 41,"G" , 49,"H" , 57,K_CH_BACK , 67,K_CH_NEW  , 76,K_CH_PREVIEW, 84,"7" , 97,"8" ,104,"9"       ,111,"+"      ,118,0
	.DB	 20,"I" ,  1,"J" ,  9,"K" , 17,"L" , 25,"M" , 33,"N" , 41,"O" , 49,"P" , 57,K_CH_ENTER, 67,K_CH_CAPS , 76,K_CH_OMEGA  , 84,"4" , 97,"5" ,104,"6"       ,111,"-"      ,118,0
	.DB	 28,"Q" ,  1,"R" ,  9,"S" , 17,"T" , 25,"U" , 33,"V" , 41,"W" , 49,"X" , 57,K_CH_HOME , 67,K_CH_UP   , 76,K_CH_END    , 84,"1" , 97,"2" ,104,"3"       ,111,"*"      ,118,0
	.DB	 36,"Y" ,  1,"Z" ,  9," " , 17," " , 25," " , 33," " , 41," " , 49," " , 57,K_CH_LEFT , 67,K_CH_DOWN , 76,K_CH_RIGHT  , 84,"+" , 97,"0" ,104,"-"       ,111,"/"      ,118,0

	;/////////////////////////////////////////////////////////
	;//							//
	;// KEYBOARD EDIT					//
	;//							//
	;// input   : NONE					//
	;//	      						//
	;/////////////////////////////////////////////////////////

	fn_KEYBOARD_INPUT_EDIT:
		;///////////////////////////////////////////
		;// DRAW INPUT PANEL AT BOTTOM OF SCREEN
		;///////////////////////////////////////////
		ldiw	Z,BMP_KEYBOARD_5*2			;positione keyboard panel
		ldi	Acc,1
		sts	KEYBOARD_INPUT_EDIT_ROW,Acc
		sts	KEYBOARD_INPUT_EDIT_COL,Acc
		ldiw	X,0
		call	_RASTER_BMP_GET_HEIGHT
		ldiw	Y,SCREEN_HEIGHT
		sub	YL,Acc
		sub	YH,AccH
		;///////////////////////////////////////////
		;// DRAW INPUT PANEL AND WAIT ASCII KEY OR CONTROL
		;///////////////////////////////////////////
	fn_KEYBOARD_INPUT_EDIT_LOOP:
		lds	Acc,KEYBOARD_INPUT_MODE		;SET SCREEN MODE
		call	_RASTER_SET_CHAR_MODE
		call	fn_KEYBOARD_INPUT_DRAW_EDIT_BUFFER	;DRAW BUFFER
		call	fn_KEYBOARD_INPUT_DRAW_SCROLL_BAR	;DRAW SCROLL BAR
		call	fn_KEYBOARD_INPUT_DRAW_CURSOR		;DRAW CURSOR
		call	fn_KEYBOARD_INPUT			;get character
		;///////////////////////////////////////////
		;// CHECK IF CONTROL OR ASCII KEY AND PERFORM ACCORDING
		;///////////////////////////////////////////
		cpi	Acc,K_CH_RIGHT				;RIGHT ?
		breq	fn_KEYBOARD_INPUT_EDIT_RIGHT		;yes, process
		cpi	Acc,K_CH_LEFT				;LEFT ?
		breq	fn_KEYBOARD_INPUT_EDIT_LEFT		;yes, process
		cpi	Acc,K_CH_UP				;UP ?
		breq	fn_KEYBOARD_INPUT_EDIT_UP		;yes, process
		cpi	Acc,K_CH_DOWN				;DOWN ?
		breq	fn_KEYBOARD_INPUT_EDIT_DOWN		;yes, process
		cpi	Acc,K_CH_END				;END ?
		breq	fn_KEYBOARD_INPUT_EDIT_END		;yes, process
		cpi	Acc,K_CH_HOME				;HOME ?
		breq	fn_KEYBOARD_INPUT_EDIT_HOME		;yes, process
		cpi	Acc,K_CH_BACK				;BACK ?
		breq	fn_KEYBOARD_INPUT_EDIT_BACK		;yes, process
		cpi	Acc,K_CH_MODE				;MODE ?
		breq	fn_KEYBOARD_INPUT_EDIT_MODE		;yes, process
		cpi	Acc,K_CH_CAPS				;CAPS ?
		lbreq	fn_KEYBOARD_INPUT_EDIT_CAPS		;yes, process
		cpi	Acc,K_CH_PREVIEW			;PREVIEW ?
		lbreq	fn_KEYBOARD_INPUT_EDIT_PREVIEW		;yes, process
		cpi	Acc,K_CH_NEW				;NEW ?
		lbreq	fn_KEYBOARD_INPUT_EDIT_NEW		;yes, process
		cpi	Acc,K_CH_ENTER				;ENTER ?
		lbreq	fn_KEYBOARD_INPUT_EDIT_ENTER		;yes, process
		cpi	Acc,K_CH_EXIT				;EXIT ?
		lbreq	fn_KEYBOARD_INPUT_EDIT_EXIT		;yes, process
		cpi	Acc,K_CH_OMEGA				;OMEGA ?
		brne	fn_KEYBOARD_INPUT_EDIT_LOOP_10		;no, branch
		;///////////////////////////////////
		;// process omega(special chars)
		;///////////////////////////////////
		call	fn_KEYBOARD_INPUT_EDIT_OMEGA
	fn_KEYBOARD_INPUT_EDIT_LOOP_10:
		call	fn_KEYBOARD_INPUT_INSERT_CHAR		;INSERT CHARACTER INTO BUFFER
		call	fn_KEYBOARD_INPUT_CURSOR_RIGHT		;update cusor position
		brcc	fn_KEYBOARD_INPUT_EDIT_LOOP
		call	fn_KEYBOARD_INPUT_CURSOR_DOWN
		rjmp	fn_KEYBOARD_INPUT_EDIT_LOOP		;else wait
	;/////////////////////////
	;//    CONTROL RIGHT	//
	;/////////////////////////
	fn_KEYBOARD_INPUT_EDIT_RIGHT:
		call	fn_KEYBOARD_INPUT_CURSOR_RIGHT		;cursor to right
		brcc	fn_KEYBOARD_INPUT_EDIT_RIGHT_10
		call	fn_KEYBOARD_INPUT_CURSOR_DOWN
	fn_KEYBOARD_INPUT_EDIT_RIGHT_10:
		rjmp	fn_KEYBOARD_INPUT_EDIT_LOOP		;DRAW CURSOR
	;/////////////////////////
	;//    CONTROL LEFT	//
	;/////////////////////////
	fn_KEYBOARD_INPUT_EDIT_LEFT:
		call	fn_KEYBOARD_INPUT_CURSOR_LEFT		;cursor to left
		brcc	fn_KEYBOARD_INPUT_EDIT_LEFT_10
		call	fn_KEYBOARD_INPUT_CURSOR_UP
	fn_KEYBOARD_INPUT_EDIT_LEFT_10:
		rjmp	fn_KEYBOARD_INPUT_EDIT_LOOP		;DRAW CURSOR
	;/////////////////////////
	;//    CONTROL UP	//
	;/////////////////////////
	fn_KEYBOARD_INPUT_EDIT_UP:
		call	fn_KEYBOARD_INPUT_CURSOR_UP		;cursor to up
		rjmp	fn_KEYBOARD_INPUT_EDIT_LOOP		;DRAW CURSOR
	;/////////////////////////
	;//    CONTROL DOWN	//
	;/////////////////////////
	fn_KEYBOARD_INPUT_EDIT_DOWN:
		call	fn_KEYBOARD_INPUT_CURSOR_DOWN		;cursor to down
		rjmp	fn_KEYBOARD_INPUT_EDIT_LOOP		;DRAW CURSOR
	;/////////////////////////
	;//    CONTROL END	//
	;/////////////////////////
	fn_KEYBOARD_INPUT_EDIT_END:
		ldi	Acc,KEYBOARD_INPUT_EDIT_COLS		;set to last col
		sts	KEYBOARD_INPUT_EDIT_COL,Acc
		rjmp	fn_KEYBOARD_INPUT_EDIT_LOOP		;DRAW CURSOR
	;/////////////////////////
	;//    CONTROL HOME	//
	;/////////////////////////
	fn_KEYBOARD_INPUT_EDIT_HOME:
		ldi	Acc,1					;set to first col
		sts	KEYBOARD_INPUT_EDIT_COL,Acc
		rjmp	fn_KEYBOARD_INPUT_EDIT_LOOP		;DRAW CURSOR
	;/////////////////////////
	;//    CONTROL BACK	//
	;/////////////////////////
	fn_KEYBOARD_INPUT_EDIT_BACK:
		call	fn_KEYBOARD_INPUT_CURSOR_LEFT		;back cursor
		ldi	Acc,' '					;insert space at this position
		call	fn_KEYBOARD_INPUT_INSERT_CHAR
		rjmp	fn_KEYBOARD_INPUT_EDIT_LOOP		;DRAW CURSOR
	;/////////////////////////
	;//    CONTROL MODE	//
	;/////////////////////////
	fn_KEYBOARD_INPUT_EDIT_MODE:
		call	_RASTER_GET_CHAR_MODE
		cpi	Acc,_RASTER_CHAR_MODE_NORMAL		;in normal mode ?
		breq	fn_KEYBOARD_INPUT_EDIT_MODE_10		;yes, branch
		ldi	Acc,_RASTER_CHAR_MODE_NORMAL		;else set normal then
		rjmp	fn_KEYBOARD_INPUT_EDIT_MODE_20
	fn_KEYBOARD_INPUT_EDIT_MODE_10:
		ldi	Acc,_RASTER_CHAR_MODE_INVERTED		;set inverted mode
	fn_KEYBOARD_INPUT_EDIT_MODE_20:
		sts	KEYBOARD_INPUT_MODE,Acc			;set new mode
		call	_RASTER_SET_CHAR_MODE
		rjmp	fn_KEYBOARD_INPUT_EDIT_LOOP		;DRAW CURSOR
	;/////////////////////////
	;//    CONTROL CAPS	//
	;/////////////////////////
	fn_KEYBOARD_INPUT_EDIT_CAPS:
		lds	Acc,KEYBOARD_INPUT_CAPS			;change caps state
		ldi	AccH,0xff
		eor	Acc,AccH
		sts	KEYBOARD_INPUT_CAPS,Acc
		rjmp	fn_KEYBOARD_INPUT_EDIT_LOOP		;DRAW CURSOR
	;/////////////////////////
	;//    CONTROL PREVIEW	//
	;/////////////////////////
	fn_KEYBOARD_INPUT_EDIT_PREVIEW:
		call	fn_KEYBOARD_INPUT_DRAW_PREVIEW
		call	_KEYBOARD_READ
		rjmp	fn_KEYBOARD_INPUT_EDIT_LOOP		;DRAW CURSOR
	;/////////////////////////
	;//    CONTROL NEW	//
	;/////////////////////////
	fn_KEYBOARD_INPUT_EDIT_NEW:
		pushw	Z
		pushw	X
		pushw	Y
		call	_RASTER_GET_CHAR_MODE			;get actual mode
		push	Acc					;save it
		ldi	Acc,_RASTER_CHAR_MODE_INVERTED		;set mode inverted
		call	_RASTER_SET_CHAR_MODE
		ldiw	Z,_MSG_CLEAR_PAGE			;input box clear page
		call	_MENU_TRANSLATE_MESSAGE
		call	_RASTER_GET_TEXT_WIDTH			;position on center horizontaly
		ldiw	X,SCREEN_WIDTH
		sub	XL,Acc
		sbc	XH,AccH
		lsr	XH
		ror	XL
		ldiw	Y,32
		ldi	Acc,_MENU_INPUT_BOX_TYPE_YES_NO		;set yes no input box type
		call	_MENU_INPUT_BOX				;get input box
		cpi	Acc,_MENU_INPUT_BOX_ANSWER_YES		;answer is YES?
		brne	fn_KEYBOARD_INPUT_EDIT_NEW_10		;no, branch
		call	fn_KEYBOARD_INPUT_EDIT_CLEAR_BUFFER	;else clear edit buffer
	fn_KEYBOARD_INPUT_EDIT_NEW_10:
		pop	Acc					;restore old mode
		call	_RASTER_SET_CHAR_MODE
		popw	Y
		popw	X
		popw	Z
		rjmp	fn_KEYBOARD_INPUT_EDIT_LOOP		;DRAW CURSOR
	;/////////////////////////////////////////////////
	;//    CONTROL OMEGA (SPECIAL CHARACTERS)	//
	;/////////////////////////////////////////////////
	fn_KEYBOARD_INPUT_EDIT_OMEGA:
		pushw	Z
		pushw	X
		pushw	Y
		push	Acc
		call	_RASTER_GET_CHAR_MODE			;get actual char mode
		pushaw						;save
		pushawt
		ldi	Acc,_RASTER_CHAR_MODE_INVERTED		;set mode inverted
		call	_RASTER_SET_CHAR_MODE
		ldi	AccT,0					;AccT=row=0
		ldi	AccTH,0					;AccTH=col=0
	fn_KEYBOARD_INPUT_EDIT_OMEGA_LOOP:
		;////////////////////////////
		;/// DRAW SPECIAL CHARS
		;////////////////////////////
		ldiw	Z,BMP_KEYBOARD_SPECIAL_1*2
		ldiw	X,KEYBOARD_INPUT_OMEGA_X
		ldiw	Y,KEYBOARD_INPUT_OMEGA_Y
		call	_RASTER_SET_BMP_FAST
		ldi	Acc,_RASTER_CHAR_WIDTH			;compute x,y
		mul	Acc,AccTH
		mov	XL,r0
		ldi	Acc,_RASTER_CHAR_HEIGHT
		mul	Acc,AccT
		mov	YL,r0
		ldi	XH,8
		ldi	YH,10
		subi	XL,-(3+KEYBOARD_INPUT_OMEGA_X)
		subi	YL,-(3+KEYBOARD_INPUT_OMEGA_Y)
		ldi	Temp,_OFF				;draw off
		call	_RASTER_SET_BOX_FAST			;draw selection
		dec	XL
		dec	YL
		subi	XH,-2
		subi	YH,-2
		call	_RASTER_SET_BOX_FAST
		dec	XL
		dec	YL
		subi	XH,-2
		subi	YH,-2
		ldi	Temp,_ON
		call	_RASTER_SET_BOX_FAST
		;////////////////////////////
		;/// GET KEYBOARD CONTROL
		;////////////////////////////
		call	_KEYBOARD_READ
		cpi	Acc,_KEY_LEFT				;LEFT key
		breq	fn_KEYBOARD_INPUT_EDIT_OMEGA_LEFT	;yes, branch
		cpi	Acc,_KEY_RIGHT				;RIGHT key
		breq	fn_KEYBOARD_INPUT_EDIT_OMEGA_RIGHT	;yes, branch
		cpi	Acc,_KEY_UP				;UP key
		breq	fn_KEYBOARD_INPUT_EDIT_OMEGA_UP		;yes, branch
		cpi	Acc,_KEY_DOWN				;DOWN key
		breq	fn_KEYBOARD_INPUT_EDIT_OMEGA_DOWN	;yes, branch
		cpi	Acc,_KEY_ENTER				;ENTER key
		breq	fn_KEYBOARD_INPUT_EDIT_OMEGA_ENTER	;yes, branch
		;////////////////////////////
		;/// LEFT ?
		;////////////////////////////
	fn_KEYBOARD_INPUT_EDIT_OMEGA_LEFT:
		cpi	AccTH,0					;col=0?
		brne	fn_KEYBOARD_INPUT_EDIT_OMEGA_LEFT_10	;no, branch
		ldi	AccTH,15				;else col=15
		rjmp	fn_KEYBOARD_INPUT_EDIT_OMEGA_LOOP
	fn_KEYBOARD_INPUT_EDIT_OMEGA_LEFT_10:
		dec	AccTH
		rjmp	fn_KEYBOARD_INPUT_EDIT_OMEGA_LOOP
		;////////////////////////////
		;/// RIGHT ?
		;////////////////////////////
	fn_KEYBOARD_INPUT_EDIT_OMEGA_RIGHT:
		cpi	AccTH,15				;col=15?
		brne	fn_KEYBOARD_INPUT_EDIT_OMEGA_RIGHT_10	;no, branch
		ldi	AccTH,0					;else col=0
		rjmp	fn_KEYBOARD_INPUT_EDIT_OMEGA_LOOP
	fn_KEYBOARD_INPUT_EDIT_OMEGA_RIGHT_10:
		inc	AccTH
		rjmp	fn_KEYBOARD_INPUT_EDIT_OMEGA_LOOP
		;////////////////////////////
		;/// UP ?
		;////////////////////////////
	fn_KEYBOARD_INPUT_EDIT_OMEGA_UP:
		cpi	AccT,0					;row=0?
		brne	fn_KEYBOARD_INPUT_EDIT_OMEGA_UP_10	;no, branch
		ldi	AccT,7					;else row=15
		rjmp	fn_KEYBOARD_INPUT_EDIT_OMEGA_LOOP
	fn_KEYBOARD_INPUT_EDIT_OMEGA_UP_10:
		dec	AccT
		rjmp	fn_KEYBOARD_INPUT_EDIT_OMEGA_LOOP
		;////////////////////////////
		;/// DOWN ?
		;////////////////////////////
	fn_KEYBOARD_INPUT_EDIT_OMEGA_DOWN:
		cpi	AccT,7					;row=15?
		brne	fn_KEYBOARD_INPUT_EDIT_OMEGA_DOWN_10	;no, branch
		ldi	AccT,0					;else row=0
		rjmp	fn_KEYBOARD_INPUT_EDIT_OMEGA_LOOP
	fn_KEYBOARD_INPUT_EDIT_OMEGA_DOWN_10:
		inc	AccT
		rjmp	fn_KEYBOARD_INPUT_EDIT_OMEGA_LOOP
		;////////////////////////////
		;/// ENTER ?
		;////////////////////////////
	fn_KEYBOARD_INPUT_EDIT_OMEGA_ENTER:
		mov	Acc,AccT				;get row
		andi	Acc,0x0f
		swap	Acc					;put into appropriate position
		andi	AccTH,0x0f
		or	Acc,AccTH
		ori 	Acc,0x80
		mov	Temp,Acc
	fn_KEYBOARD_INPUT_EDIT_OMEGA_EXIT:
		popawt
		popaw
		call	_RASTER_SET_CHAR_MODE			;restore old mode
		pop	Acc
		mov	Acc,Temp				;get character code
		popw	Y
		popw	X
		popw	Z
		ret
	;/////////////////////////
	;//    CONTROL ENTER	//
	;/////////////////////////
	fn_KEYBOARD_INPUT_EDIT_ENTER:
		ldi	Acc,1					;set to first col
		sts	KEYBOARD_INPUT_EDIT_COL,Acc
		call	fn_KEYBOARD_INPUT_CURSOR_DOWN		;then down cursor
		rjmp	fn_KEYBOARD_INPUT_EDIT_LOOP		;DRAW CURSOR
	;/////////////////////////
	;//    CONTROL EXIT	//
	;/////////////////////////
	fn_KEYBOARD_INPUT_EDIT_EXIT:
		ret						;return


	;/////////////////////////////////////////
	;//					//
	;// keyboard cursor right		//
	;// output: cy=1 if right reached	//
	;/////////////////////////////////////////
	fn_KEYBOARD_INPUT_CURSOR_RIGHT:
		push	Acc
		lds	Acc,KEYBOARD_INPUT_EDIT_COL		;get col
		cpi	Acc,KEYBOARD_INPUT_EDIT_COLS		;col=max col?
		brne	fn_KEYBOARD_INPUT_CURSOR_RIGHT_10	;no, branch
		ldi	Acc,1					;else col=1
		sec
		rjmp	fn_KEYBOARD_INPUT_CURSOR_RIGHT_20	;branch
	fn_KEYBOARD_INPUT_CURSOR_RIGHT_10:
		inc	Acc					;col++
		clc
	fn_KEYBOARD_INPUT_CURSOR_RIGHT_20:
		sts	KEYBOARD_INPUT_EDIT_COL,Acc
		pop	Acc
		ret
	;/////////////////////////////////////////
	;//					//
	;// keyboard cursor left		//
	;// output: cy=1 if left reached	//
	;/////////////////////////////////////////
	fn_KEYBOARD_INPUT_CURSOR_LEFT:
		push	Acc
		lds	Acc,KEYBOARD_INPUT_EDIT_COL		;get col
		cpi	Acc,1					;col=1 ?
		brne	fn_KEYBOARD_INPUT_CURSOR_LEFT_10	;no, branch
		ldi	Acc,KEYBOARD_INPUT_EDIT_COLS		;else col=max col
		sec
		rjmp	fn_KEYBOARD_INPUT_CURSOR_LEFT_20	;branch
	fn_KEYBOARD_INPUT_CURSOR_LEFT_10:
		dec	Acc					;col--
		clc
	fn_KEYBOARD_INPUT_CURSOR_LEFT_20:
		sts	KEYBOARD_INPUT_EDIT_COL,Acc
		pop	Acc
		ret
	;/////////////////////////////////////////
	;//					//
	;// keyboard cursor down		//
	;// output: cy=1 if down reached	//
	;/////////////////////////////////////////
	fn_KEYBOARD_INPUT_CURSOR_DOWN:
		push	Acc
		lds	Acc,KEYBOARD_INPUT_EDIT_ROW		;get row
		cpi	Acc,KEYBOARD_INPUT_EDIT_ROWS		;row=max row?
		brne	fn_KEYBOARD_INPUT_CURSOR_DOWN_10	;no, branch
		ldi	Acc,1					;else row=1
		sec
		rjmp	fn_KEYBOARD_INPUT_CURSOR_DOWN_20	;branch
	fn_KEYBOARD_INPUT_CURSOR_DOWN_10:
		inc	Acc					;row++
		clc
	fn_KEYBOARD_INPUT_CURSOR_DOWN_20:
		sts	KEYBOARD_INPUT_EDIT_ROW,Acc
		pop	Acc
		ret
	;/////////////////////////////////////////
	;//					//
	;// keyboard cursor up			//
	;// output: cy=1 if up reached		//
	;/////////////////////////////////////////
	fn_KEYBOARD_INPUT_CURSOR_UP:
		push	Acc
		lds	Acc,KEYBOARD_INPUT_EDIT_ROW		;get row
		cpi	Acc,1					;row=1 ?
		brne	fn_KEYBOARD_INPUT_CURSOR_UP_10		;no, branch
		ldi	Acc,KEYBOARD_INPUT_EDIT_ROWS		;else row=max row
		sec
		rjmp	fn_KEYBOARD_INPUT_CURSOR_UP_20		;branch
	fn_KEYBOARD_INPUT_CURSOR_UP_10:
		dec	Acc					;row--
		clc
	fn_KEYBOARD_INPUT_CURSOR_UP_20:
		sts	KEYBOARD_INPUT_EDIT_ROW,Acc
		pop	Acc
		ret
	;/////////////////////////////////////////
	;//					//
	;// keyboard insert char into buffer	//
	;//	      				//
	;// input : Acc Char			//
	;/////////////////////////////////////////
	fn_KEYBOARD_INPUT_INSERT_CHAR:
		pushTempW
		pushw	Z
		ldiw	Z,KEYBOARD_INPUT_EDIT_BUFFER		;Z-->PAGE(row,col)
		lds	Temp,KEYBOARD_INPUT_EDIT_ROW		;get row
		dec	Temp					;adjust 0..N
		ldi	TempH,KEYBOARD_INPUT_EDIT_COLS
		mul	Temp,TempH
		lds	Temp,KEYBOARD_INPUT_EDIT_COL		;get col
		dec	Temp					;adjust 0..N
		add	ZL,r0
		adc	ZH,r1
		add	ZL,Temp
		ldi	Temp,0
		adc	ZH,Temp
		st	Z,Acc
		popw	Z
		popTempW
		ret

	;/////////////////////////////////////////
	;//					//
	;// keyboard draw edit buffer preview 	//
	;// mode				//
	;//	      				//
	;/////////////////////////////////////////
	fn_KEYBOARD_INPUT_DRAW_PREVIEW:
		pushaw
		pushw	X					;save useds
		pushw	Y
		pushw	Z
		call	_VIDEO_OUT_CLS
		ldiw	X,0					;start at 0,0
		ldiw	Y,0
		ldiw	Z,KEYBOARD_INPUT_EDIT_BUFFER		;Z-->EDIT BUFFER(bar.row)
		ldi	AccH,KEYBOARD_INPUT_EDIT_ROWS		;number of lines to show
		rjmp	fn_KEYBOARD_INPUT_DRAW_EDIT_BUFFER_10	;perform the show

	;/////////////////////////////////////////
	;//					//
	;// keyboard draw edit buffer and 	//
	;// scroll bar				//
	;//	      				//
	;/////////////////////////////////////////
	fn_KEYBOARD_INPUT_DRAW_EDIT_BUFFER:
		pushaw
		pushw	X					;save useds
		pushw	Y
		pushw	Z
		lds	Acc,KEYBOARD_INPUT_EDIT_ROW		;get edit row
		lds	AccH,KEYBOARD_INPUT_BAR_ROW		;get bar row
		cp	Acc,AccH				;edit.row<bar.row ?
		brsh	fn_KEYBOARD_INPUT_DRAW_EDIT_BUFFER_10A	;no, branch
		sts	KEYBOARD_INPUT_BAR_ROW,Acc		;else bar.row<-edit.row
	fn_KEYBOARD_INPUT_DRAW_EDIT_BUFFER_10A:
		subi	AccH,-6					;bar.row+=5
		cp	Acc,AccH				;edit.row>(bar.row+5)?
		brlo	fn_KEYBOARD_INPUT_DRAW_EDIT_BUFFER_10B	;no, branch
		subi	Acc,5
		sts	KEYBOARD_INPUT_BAR_ROW,Acc		;else bar.row<-edit.row-6
	fn_KEYBOARD_INPUT_DRAW_EDIT_BUFFER_10B:
		ldiw	X,0					;start at 0,0
		ldiw	Y,0
		ldiw	Z,KEYBOARD_INPUT_EDIT_BUFFER		;Z-->EDIT BUFFER(bar.row)
		lds	Acc,KEYBOARD_INPUT_BAR_ROW		;get bar.row
		dec	Acc					;adjust 0..N
		ldi	AccH,KEYBOARD_INPUT_EDIT_COLS		;get edit.cols
		mul	Acc,AccH
		add	ZL,r0			
		adc	ZH,r1
		ldi	AccH,KEYBOARD_INPUT_EDIT_ROWS/2+1	;number of lines to show
	fn_KEYBOARD_INPUT_DRAW_EDIT_BUFFER_10:
		push	AccH
		pushw	X
		ldi	AccH,KEYBOARD_INPUT_EDIT_COLS		;number of cols to show
	fn_KEYBOARD_INPUT_DRAW_EDIT_BUFFER_20:
		ld	Acc,Z+					;get char
		call	_RASTER_SET_CHAR
		dec	AccH
		brne	fn_KEYBOARD_INPUT_DRAW_EDIT_BUFFER_20	;until row done
		popw	X
		pop	AccH
		addiw	Y,_RASTER_CHAR_HEIGHT			;next row
		dec	AccH
		brne	fn_KEYBOARD_INPUT_DRAW_EDIT_BUFFER_10
		popw	Z
		popw	Y
		popw	X
		popaw
		ret

	;/////////////////////////////////////////
	;//					//
	;// keyboard draw scroll bar		//
	;//					//
	;/////////////////////////////////////////
	fn_KEYBOARD_INPUT_DRAW_SCROLL_BAR:
		pushw	X
		pushw	Y
		pushw	Z
		pushaw
		ldiw	Z,BMP_TEXT_SCROLL_BAR_1*2			;Z-->BMP.scroll bar
		call	_RASTER_BMP_GET_WIDTH
		ldiw	X,SCREEN_WIDTH
		sub	XL,Acc
		sbc	XH,AccH
		ldiw	Y,0
		call	_RASTER_SET_BMP_FAST				;DRAW SCROLL BAR
		ldiw	Z,BMP_TEXT_SCROLL_BAR_BUTTON_1*2		;Z-->BMP.scroll bar button
		call	_RASTER_BMP_GET_WIDTH
		ldiw	X,SCREEN_WIDTH
		sub	XL,Acc
		sbc	XH,AccH
		lds	Acc,KEYBOARD_INPUT_BAR_ROW			;get scroll bar row
		dec	Acc
		ldi	AccH,5
		mul	Acc,AccH
		mov	YL,r0
		subi	YL,-7
		ldi	YH,0
		call	_RASTER_SET_BMP_FAST
		popaw
		popw	Z
		popw	Y
		popw	X
		ret
				
	;/////////////////////////////////////////
	;//					//
	;// keyboard draw cursor		//
	;//					//
	;// input   : Temp _ON/_OFF		//
	;//	      				//
	;/////////////////////////////////////////
	fn_KEYBOARD_INPUT_DRAW_CURSOR:
		pushw	X
		pushw	Y
		push	Temp
		push	Acc
		lds	XL,KEYBOARD_INPUT_EDIT_COL		;get col
		dec	XL					;adjust 0..N
		ldi	XH,_RASTER_CHAR_WIDTH			;get char width
		mul	XL,XH
		mov	XL,r0					;XL cursor position in pixels
		lds	YL,KEYBOARD_INPUT_EDIT_ROW		;get row
		lds	Acc,KEYBOARD_INPUT_BAR_ROW		;edit.row-bar.row
		sub	YL,Acc
		inc	YL
		ldi	XH,_RASTER_CHAR_HEIGHT			;get char height
		mul	YL,XH
		mov	YL,r0					;YL cursor position in pixels
		dec	YL
		ldi	XH,_RASTER_CHAR_WIDTH			;cursor width
		call	_RASTER_GET_CHAR_MODE			;get char mode
		cpi	Acc,_RASTER_CHAR_MODE_NORMAL		;normal ?
		breq	fn_KEYBOARD_INPUT_DRAW_CURSOR_10	;no, branch
		com	Temp
	fn_KEYBOARD_INPUT_DRAW_CURSOR_10:
		call	_RASTER_SET_HLINE_FAST			;draw cursor
		pop	Acc
		pop	Temp
		popw	Y
		popw	X
		ret


	;/////////////////////////////////////////
	;//					//
	;// keyboard fill buffer with space	//
	;//					//
	;// input   : NONE			//
	;//	      				//
	;/////////////////////////////////////////
	fn_KEYBOARD_INPUT_EDIT_CLEAR_BUFFER:
		pushaw
		pushw	Z
		push	Temp
		ldiw	Z,KEYBOARD_INPUT_EDIT_BUFFER			;Z-->EDIT BUFFER
		ldiaw	KEYBOARD_INPUT_EDIT_BUFFER_SIZE			;BUFFER SIZE
		ldi	Temp,' '	
	fn_KEYBOARD_INPUT_EDIT_CLEAR_BUFFER_10:
		st	Z+,Temp						;STORE SPACE
		subiaw	1
		brne	fn_KEYBOARD_INPUT_EDIT_CLEAR_BUFFER_10
		pop	Temp
		popw	Z
		popaw
		ret

;+------------------------------------------------------------------------------+
;| FUNCTION  : fn_GET_MS_VALUE							|
;+------------------------------------------------------------------------------+	
;| Description:									|
;|	Get value in ms(milliseconds)						|
;|	min value = 100ms							|
;|	max value = 30000ms							|
;|	step value= 100ms							|
;+------------------------------------------------------------------------------+
;| Input    : 	AccH:Acc input value						|
;|		Z-->TRANSLATE MESSAGE ID					|
;| Output   : 	AccH:Acc output value						|
;+------------------------------------------------------------------------------+


fn_GET_MS_VALUE:
		pushaw							;save input value
		ldiw	X,0
		ldiw	Y,_RASTER_CHAR_HEIGHT*2
		call	_MENU_TRANSLATE_MESSAGE
		call	_RASTER_GET_WIDTH				;center on screen
		movw	XL,Acc
		call	_RASTER_GET_TEXT_WIDTH
		addiaw	_RASTER_CHAR_WIDTH
		sub	XL,Acc
		sbc	XH,AccH
		lsrw	X
		call	_MENU_MESSAGE_BOX				;Show message box
		popawt							;restore value
fn_GET_MS_VALUE_00:
		pushawt							;save value
		movw	Acc,AccT					;put into correct register
		call	_UITOSUS					;convert to ASCII
		adiw	ZL,1						;avoid Plus sign
		call	_RASTER_GET_WIDTH				;get screen width
		movw	XL,Acc		
		subiw	X,_RASTER_CHAR_WIDTH*8				;center number horizontaly
		lsrw	X
		ldiw	Y,_RASTER_CHAR_HEIGHT*5				;at 5 row
		call	_RASTER_SET_TEXT_OUT_SRAM			;show number
		ldiw	Z,fn_GET_MS_VALUE_MS*2				;Z-->VALUE_MS
		call	_RASTER_SET_TEXT_OUT_FLASH			;show ms(millisecond)
		call	_KEYBOARD_READ
		popawt							;restore value
		cpi	Acc,_KEY_UP					;UP KEY ?
		brne	fn_GET_MS_VALUE_20				;no, branch
		;@----------------------@
		;| KEY UP - COUNT+=100	|
		;@----------------------@
		addiawt	100
		cpiawt	30000						;>30000ms?
		brlo	fn_GET_MS_VALUE_10				;no, branch
		ldiawt	30000						;else count=30000
fn_GET_MS_VALUE_10:
		rjmp	fn_GET_MS_VALUE_00				;wait new key command
fn_GET_MS_VALUE_20:
		cpi	Acc,_KEY_DOWN					;DOWN KEY ?
		brne	fn_GET_MS_VALUE_40				;no, branch
		;@----------------------@
		;| KEY DOW - COUNT+=100	|
		;@----------------------@
		subiawt	100
		cpiawt	100						;<100ms?
		brsh	fn_GET_MS_VALUE_30				;no, branch
		ldiawt	100						;else count=100
fn_GET_MS_VALUE_30:
		rjmp	fn_GET_MS_VALUE_00				;wait new key command
fn_GET_MS_VALUE_40:
		cpi	Acc,_KEY_ENTER					;ENTER KEY?
		brne	fn_GET_MS_VALUE_00				;wait new key command
		movw	Acc,AccT					;AccH:Acc return register
		ret
fn_GET_MS_VALUE_MS:
		.DB	" ms",0

;+------------------------------------------------------------------------------+
;| FUNCTION  : fn_SHOW_E_MESSAGE						|
;+------------------------------------------------------------------------------+	
;| Description:									|
;|		Show message into EEPROM					|
;+------------------------------------------------------------------------------+
;| Input    : 	Z--> MESSAGE IN EEPROM						|
;| Output   : 	None								|
;+------------------------------------------------------------------------------+
fn_SHOW_E_MESSAGE:
		call	_RASTER_GET_CHAR_MODE				;save actual mode
		push	Acc
		movw	YL,ZL						;Y-->message
		addiw	Y,KEYBOARD_INPUT_EDIT_ROWS*KEYBOARD_INPUT_EDIT_COLS
		call	_EEPROM_R_BYTE					;get mode
		call	_RASTER_SET_CHAR_MODE				;set new mode
		ldiw	Y,0						;start at Y=0
		ldi	Acc,KEYBOARD_INPUT_EDIT_ROWS			;rows counter
		mov	AccH,ZL						;AccH:AccT-->message
		mov	AccT,ZH
fn_SHOW_E_MESSAGE_10:
		push	Acc						;save row counter
		pushw	Y						;save Y
		ldiw	X,0						;start at X=0
		ldi	Acc,KEYBOARD_INPUT_EDIT_COLS			;cols counter
fn_SHOW_E_MESSAGE_20:
		push	Acc						;save cols
		call	_EEPROM_READ					;read message data
		call	_RASTER_SET_CHAR				;send to display
		subi	AccH,-1						;next eeprom address
		sbci	AccT,-1
		pop	Acc						;restore cols
		dec	Acc						;cols--
		brne	fn_SHOW_E_MESSAGE_20				;until all cols filled					
		popw	Y						;restore Y
		addiw	Y,_RASTER_CHAR_HEIGHT				;Y+=char height
		pop	Acc						;restore rows
		dec	Acc						;rows--
		brne	fn_SHOW_E_MESSAGE_10				;until all rows filled
		pop	Acc						;restore mode
		call	_RASTER_SET_CHAR_MODE
		ret

;	 -----------------------------------------------------------------------------------------------
;	|												|
;=======>      M O D U L E S  I N C L U S I O N								|
;	| 												|
;	 -----------------------------------------------------------------------------------------------

	.INCLUDE	"MODULE_BOOT.INC"			;BOOT MODULE
	.INCLUDE	"MODULE_SHOW_PRESENTATION.INC"	        ;SHOW MOTION_DETECTOR PRESENTATION
	.INCLUDE	"MODULE_RESTORE_SYSTEM_CONFIG.INC"	;RESTORE SYSTEM CONFIGURATION MODULE
	.INCLUDE	"MODULE_MENU.INC"			;MENU MODULE
	.INCLUDE	"MODULE_SET_NTSC.INC"			;SET NTSC MODULE
	.INCLUDE	"MODULE_SET_PAL.INC"			;SET PAL MODULE	
	.INCLUDE	"MODULE_DRAW_DOTS.INC"			;DRAW DOTS MODULE
	.INCLUDE	"MODULE_DRAW_GRID.INC"			;DRAW GRID MODULE
	.INCLUDE	"MODULE_DRAW_PALETTE.INC"		;DRAW PALETTE MODULE
	.INCLUDE	"MODULE_DRAW_GENLOCK.INC"		;DRAW GENLOCKED CROSS MODULE
	.INCLUDE	"MODULE_SHOW_DIGITIZED.INC"		;SHOW DIGITIZED IMAGE MODULE
	.INCLUDE	"MODULE_SHOW_DIGITIZED_MOTION.INC"	;SHOW DIGITIZED MOTION IMAGE MODULE
	.INCLUDE	"MODULE_DEFINE_CAPTURE_GRID.INC"	;DEFINE CAPTURE GRID MODULE
	.INCLUDE	"MODULE_CLEAR_GRID.INC"			;CLEAR CAPTURE GRID
	.INCLUDE	"MODULE_DEFINE_MOVE_SENSIBILITY.INC"	;DEFINE MOTION SENSIBILITY MODULE
	.INCLUDE	"MODULE_SET_TRACE_TIME.INC"		;SET TRACE TIME MODULE 
	.INCLUDE	"MODULE_SHOW_DIGITIZED_AREA.INC"	;SHOW DIGITIZED AREA MODULE
	.INCLUDE	"MODULE_DEFINE_IP.INC"			;DEFINE IP MODULE
	.INCLUDE	"MODULE_SHOW_MOTION_TYPES.INC"		;SHOW MOTION TYPES MODULE
	.INCLUDE	"MODULE_SET_CLOCK.INC"			;SET CLOCK MODULE
	.INCLUDE	"MODULE_SHOW_ANALOG_CLOCK.INC"		;SHOW ANALOG CLOCK MODULE
	.INCLUDE	"MODULE_SHOW_DIGITAL_CLOCK.INC"		;SHOW DIGITAL CLOCK MODULE
	.INCLUDE	"MODULE_TEST_MOTION_PARAMETERS.INC"	;TEST MOTION PARAMETERS MODULE
	.INCLUDE	"MODULE_SET_LANGUAGES.INC"		;SET LANGUAGES MODULE
	.INCLUDE	"MODULE_SET_SCREEN_MODE.INC"		;SET SCREEN MODE MODULE
	.INCLUDE	"MODULE_PRESENTATION_MESSAGE.INC"	;PRESENTATION MESSAGE MODULE
	.INCLUDE	"MODULE_ID_MESSAGE.INC"			;IDENTIFICATION MESSAGE MODULE
	.INCLUDE	"MODULE_ALARM_MESSAGE.INC"		;ALARM MESSAGE MODULE
	.INCLUDE	"MODULE_SET_ONE_ITEM.INC"		;SET ONE MENU ITEM MODULE
	.INCLUDE	"MODULE_DEFINE_ACTIVATION_MODE.INC"	;SET ACTIVATION MODE MODULE
	.INCLUDE	"MODULE_DEFINE_COMM_MODE.INC"		;SET COMM MODE MODULE
	.INCLUDE	"MODULE_DEFINE_TRACE_TYPE.INC"		;SET TRACE TYPE MODULE
	.INCLUDE	"MODULE_MOTION_DETECTION_LOOP.INC"	;MAIN MOTION DETECTION LOOP
	.INCLUDE	"MODULE_SET_PASSWORD.INC"		;SET A NEW PASSWORD
	.INCLUDE	"MODULE_CHECK_PASSWORD.INC"		;CHECK A PASSWORD 
	.INCLUDE	"MODULE_ACTIVATION_RELAY_REPEATS.INC"	;SET RELAY REPEATS
	.INCLUDE	"MODULE_DEFINE_RELAY_TIME_ON.INC"		;DEFINE RELAY ON TIME	
	.INCLUDE	"MODULE_DEFINE_RELAY_TIME_OFF.INC"		;DEFINE RELAY OFF TIME
	.INCLUDE	"MODULE_DEFINE_RELAY_TIME_ACTIVATION.INC"	;DEFINE RELAY ACTIVATION TIME
	.INCLUDE	"MODULE_PROGRAM_1.INC"			;PROGRAM NUMBER#1 USER DEFINED
        .INCLUDE	"MODULE_PROGRAM_2.INC"			;PROGRAM NUMBER#2 USER DEFINED
        .INCLUDE	"MODULE_PROGRAM_3.INC"			;PROGRAM NUMBER#3 USER DEFINED
        .INCLUDE	"MODULE_PROGRAM_4.INC"			;PROGRAM NUMBER#4 USER DEFINED
        .INCLUDE        "MODULE_RESET.INC"                      ;TO RESET TO COLD BOOT 



