;
;		___  ___      _        ______                                    
;		|  \/  |     (_)       | ___ \                                   
;		| .  . | __ _ _ _ __   | |_/ / __ ___   __ _ _ __ __ _ _ __ ___  
;		| |\/| |/ _` | | '_ \  |  __/ '__/ _ \ / _` | '__/ _` | '_ ` _ \ 
;		| |  | | (_| | | | | | | |  | | | (_) | (_| | | | (_| | | | | | |
;		\_|  |_/\__,_|_|_| |_| \_|  |_|  \___/ \__, |_|  \__,_|_| |_| |_|
;               	        	               __/  |                    
;                       	        	       |___/                     
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
;	| Motion Detector ATMEGA64 is a Program Developed by MOTION_DETECTOR Technology company, this version |
;	| 1,20 is relesead for Educational purposes and the following license conditions are met.    |
;	|										  	     |
;	| Portions of MOTION_DETECTOR are Copyright (c) 2000-2008				  	     |
;	|										  	     |
;	| João D´Artagnan A. Oliveira Programmer and Author; 				  	     |
;	|										  	     |
;	| All rights reserved.								  	     |
;	|										  	     |
;	|   Redistribution and use in source and binary forms, with or without		  	     |
;	|  modification, in only permitted in High scholls, Universities and others that comply      |
;	|  use for Educational purpose in technical centers. are permitted provided that the 	     |
;	|  following conditions are met:	  	     	     				     |
;	|										  	     |
;	|   * Redistributions of source code must retain the above copyright		  	     |
;	|     notice, this list of conditions and the following disclaimer.		  	     |
;	|										  	     |
;	|   * Redistributions in binary form must reproduce the above copyright	  		     |
;	|     notice, this list of conditions and the following disclaimer in			     |
;	|     the documentation and/or other materials provided with the		  	     |
;	|     distribution.								  	     |
;	|										  	     |
;	|   * Neither the name of the copyright holders nor the names of		  	     |
;	|     contributors may be used to endorse or promote products derived            	     |
;	|     from this software without specific prior written permission.		  	     |
;	|											     |
;	|   * The comercial use of this software is definitely proihibited without		     |
;	|     permission of MOTION_DETECTOR Technology and the Author.				     |
;	|										  	     |
;	|   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"  	     |
;	|   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE    	     |
;	|   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE  	     |
;	|   ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE     	     |
;	|   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR	  		     |
;	|   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF		     |
;	|   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS	  	     |
;	|   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN	  	     |
;	|   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)	  	     |
;	|   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE   	     |
;	|   POSSIBILITY OF SUCH DAMAGE.						  		     |
;	|										  	     |
;	+--------------------------------------------------------------------------------------------+
;-------|											     |
;	|   			M  A  I  N    P  R  O  G  R  A  M    F  I  L  E			     |
;	| 											     |
; 	 --------------------------------------------------------------------------------------------


	;#define	__DEBUG_MODE__					;enable this line only for debug mode
	;#define	__TEST_MODE__					;enable this line only for test mode

; 	 -----------------------------------------------------------------------------------------------
;	|												|
;=======>   				P R O G R A M  L O G 						|
;	| 												|
; 	 -----------------------------------------------------------------------------------------------


; 	 -----------------------------------------------------------------------------------------------
;	|												|
;=======>   		      G L O B A L   D E F I N I T I O N S    I N C L U D E			|
;	| 												|
; 	 -----------------------------------------------------------------------------------------------


	.INCLUDE	"DEFS\GLOBAL_DEFINITIONS\GLOBDEFS.INC"		;GLOBAL DEFINITION
									;MUST BE A 1ST INCLUDE

;	 -----------------------------------------------------------------------------------------------
;	|												|
;=======>   		C O M P I L E R   A N D   S O F W A R E    S E T T I N G S	     		|
;	| 												|
;	 -----------------------------------------------------------------------------------------------
	


	.EQU	_SRAM_BOOT_TYPE=_SRAM_NOT_CLEAR				;NO CLEAR SRAM

	.EQU	_AVR_CLOCK	= 16000000				;AVR CLOCK, THIS SOFTWARE WAS DEVELOPED
									;TO WORK CORRECTLY IN THIS FREQUENCY, THEN
									;CHANGE MUST BE FOLLOWING CAREFULLY STUDY

;	 -----------------------------------------------------------------------------------------------
;	|												|
;=======>   		P R O C E S S O R   D E F I N I T I O N S   I N C L U D E  	     		|
;	| 												|
;	 -----------------------------------------------------------------------------------------------

	.INCLUDE 	"DEFS\M64_FILES\M64DEF.INC"			;AVR TYPE CHIP DEFINITIONS
	.INCLUDE	"M64HDC_ALTERED.INC"				;AVR HANDDLES INCLUSION
									;MUST BE 2ND AND 3ND INCLUDES
									;This include file was altered
									;with purpose to reduce interrupt
									;_HDC_OC1A_PROC clocks

;	 -----------------------------------------------------------------------------------------------
;	|												|
;=======>  					L I B R A R Y S	 					|
;	| 												|
;	 -----------------------------------------------------------------------------------------------

	;+------------------------------------
	;| MATH ROUTINES
	;+------------------------------------

	.INCLUDE	"DEFS\MATH_DEFINITIONS\MATHCONS.INC"		;MATH DEFINITIONS
        .INCLUDE        "MATH\INTEGER\DUMPHEX\DUMPHEX.INC"              ;Show hex
	.INCLUDE	"MATH\INTEGER\MULDIV_U8U8U8\MULDIV_U8U8U8U8.HUG";(a*b)/c unsigned 8 bits
	.INCLUDE	"MATH\INTEGER\DIV_U16U16U16\DIV_U16U16U16.HUG"	;unsigned integer divide 16 bits
	.INCLUDE	"MATH\INTEGER\DIV_U32U32U32\DIV_U32U32U32.HUG"	;unsigned integer divide 32 bits
	.INCLUDE	"MATH\INTEGER\UITOS\UITOS.HUG"			;unsigned integer to string
	.INCLUDE	"MATH\INTEGER\UBTOS\UBTOS.HUG"			;unsigned byte to string
	.INCLUDE	"MATH\ROTATION\ROTATE2D\ROTATION2D.HUG"		;2d point rotation

	;+------------------------------------
	;| TIME INCLUSION ROUTINES
	;+------------------------------------

	.INCLUDE	"TIME\DELAYS\DELAYS.HUG"			;delay  routines

	;+------------------------------------
	;| GRAPH ROUTINES
	;+------------------------------------

	.INCLUDE	"GRAPH\LINE\LINE.HUG"				;line 
	.INCLUDE	"GRAPH\RASTER\BASE\RASTER_BASE.INC"		;<--- DONT FORGET TO CHANGE TO *.HUG 

	;+------------------------------------
	;| EEPROM ROUTINES
	;+------------------------------------

	.INCLUDE	"EEPROMS\AVRE2P\A256_EX.HUG"			;eeprom routines
	

;	 -----------------------------------------------------------------------------------------------
;	|												|
;=======>  		H A R D W A R E  D E F I N I T I O N S  &  R O U T I N E S	 		|
;	| 												|
;	 -----------------------------------------------------------------------------------------------


	.INCLUDE	"HARDWARE_DEFINITIONS.INC"	

;	 -----------------------------------------------------------------------------------------------
;	|												|
;=======>   C O N S T A N T S , V A R I A B L E S , L O C A L  F U N C T I O N S  A N D  M O D U L E S	|
;	| 												|
;	 -----------------------------------------------------------------------------------------------


	.INCLUDE	"LOCAL_DEFINITIONS.INC"
	

;				 __________________________________________________________
;				|//////////////////////////////////////////////////////////|
;				|//+----------------------------------------------------+//|
;				|//|							|//|
;				|//|        M A I N   I N I T I A L I Z A T I O N	|//|
;				|//|							|//|
;-------------------------------|//+----------------------------------------------------+//|-----------------------------------
;-------------------------------|//////////////////////////////////////////////////////////|-----------------------------------
;				\----------------------------------------------------------/

_MAIN_INIT:
	call	_CPULED_INIT			;CPU LED INITIALIZATION
	call	_MOTION_RELAY_INIT		;MOTION RELAY INITIALIZATION
	call	_KEYBOARD_INIT			;KEYBOARD INITIALIZATION
	call	_VIDEO_OUT_INIT			;COMPOSITE VIDEO OUTPUT INITIALIZATION
	call	_MENU_INIT			;MENU INITIALIZATION
	call	_RTC_INIT			;RTC INITIALIZATION


;	ldiw	X,COPYRIGHT*2
;	ldiw	Y,E_MSG_APRESENTACAO
;	ldiaw	12*20
;	call	_EEPROM_W_STR_FLASH

	#ifndef	__DEBUG_MODE__
		sei				;ENABLE GLOBAL INTERRUPTS
	#endif

;				 __________________________________________________________
;				|//////////////////////////////////////////////////////////|
;				|//+----------------------------------------------------+//|
;				|//|							|//|
;				|//|        	M A I N   P R O G R A M			|//|
;				|//|							|//|
;-------------------------------|//+----------------------------------------------------+//|-----------------------------------
;-------------------------------|//////////////////////////////////////////////////////////|-----------------------------------
;				\----------------------------------------------------------/


	
_MAIN:	

	;jmp	_MODULE_PROGRAM_1

;==================================================================

	#ifdef __TEST_MODE__
		call	_MODULE_RESTORE_SYSTEM_CONFIGURATION		;restore configuration
		call	_MODULE_MENU					;call menu
	#endif

;==================================================================
	call	_BOOT_GET_STATE					;get boot state
	cpi	Acc,_ON						;HOT boot ?
	breq	_MAIN_10					;YES, branch
	call	_MODULE_RESTORE_SYSTEM_CONFIGURATION		;restore configuration
	;//////////////////////////////////////////////
	;// SHOW BOOT AND PRESENTATION IF COLD BOOT
	;//////////////////////////////////////////////
	ldi	Acc,0						;clear RTC
	call	_RTC_SET_HOUR
	call	_RTC_SET_MINUTE
	call	_RTC_SET_SECOND
	call	_MODULE_BOOT					;show boot
	call	_KEYBOARD_READ_FLASH				;read keyboard
	cpi	Acc,_KEY_ENTER					;enter pressed?
	breq	_MAIN_20					;yes, call menu
	call	_MODULE_PRESENTATION				;show apresentation
_MAIN_10:
	;//////////////////////////////////////////////
	;// RELOAD SYSTEM CONFIGURATION
	;//////////////////////////////////////////////
	call	_MODULE_RESTORE_SYSTEM_CONFIGURATION		;restore again because presentation was lose data
	;//////////////////////////////////////////////
	;// CHECK IF SYSTEM CONFIGURATION KEY IS PRESSED
	;//////////////////////////////////////////////
	call	_KEYBOARD_READ_FLASH
	cpi	Acc,_KEY_ENTER					;enter key pressed
	brne	_MAIN_30					;no, branch
_MAIN_20:
	call	_VIDEO_OUT_CLS					;clear screen
	ldi	Acc,_RASTER_CHAR_MODE_INVERTED
	call	_RASTER_SET_CHAR_MODE				;set inverted mode
	ldiw	Z,_MSG_RELEASE_ENTER				;wait key release
	call	_MENU_TRANSLATE_MESSAGE
	call	_RASTER_GET_WIDTH				;center message
	movw	XL,Acc
	call	_RASTER_GET_TEXT_WIDTH
	sub	XL,Acc
	sbc	XH,AccH
	lsrw	X
	ldiw	Y,0
	call	_MENU_MESSAGE_BOX				;box messasge
	ldi	Acc,_KEY_ENTER
	call	_KEYBOARD_READ_AND_WAIT				;wait enter release
	;//////////////////////////////////////////////
	;// RELOAD SYSTEM CONFIGURATION
	;//////////////////////////////////////////////
	call	_MODULE_RESTORE_SYSTEM_CONFIGURATION		;restore again because presentation was lose data
	call	_VIDEO_OUT_CLS					;clear screen
	call	_MODULE_CHECK_PASSWORD				;check password
	call	_MODULE_MENU					;call menu
	;//////////////////////////////////////////////
	;// MOTION DETECTOR MAIN LOOP
	;//////////////////////////////////////////////
_MAIN_30:
	call	_MODULE_MOTION_DETECTION_LOOP			;process motion detection
	rjmp	_MAIN_30



