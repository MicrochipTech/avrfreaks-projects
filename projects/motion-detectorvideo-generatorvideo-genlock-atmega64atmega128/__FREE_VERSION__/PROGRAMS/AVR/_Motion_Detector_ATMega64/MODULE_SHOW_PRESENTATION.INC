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
;	 -----------------------------------------------------------------------------------------------
;	|												|
;=======>      M O D U L E  -  S H O W   P R E S E N T A T I O N  A N D  L O G O 		        |
;	| 												|
;	 -----------------------------------------------------------------------------------------------
;	| Description:											|
;	| 	This Module show MOTION_DETECTOR logo at max resolution screen with simple animation and return	|
;	 -----------------------------------------------------------------------------------------------

	;////////////////////////
	;// PRESENTATION BMPS
	;////////////////////////
	.INCLUDE	"BMPS\BMP_MOTION_DETECTOR_8.INC"		;MOTION DETECTOR IMAGE
	.INCLUDE	"BMPS\BMP_CAMERA_7.INC"				;CAMERA
	.INCLUDE	"BMPS\BMP_VERSION_120.INC"			;VERSION BACKGROUND

	_MODULE_PRESENTATION:
		;///////////////////////////////////////
		;// SHOW MOTION_DETECTOR LOGO
		;///////////////////////////////////////
		call	_VIDEO_OUT_GET_MODE				;get actual mode
		push	Acc						;save
		ldi	Acc,_VIDEO_MODE_128X96X2N			;set max resolution screen
		call	_VIDEO_OUT_SET_MODE				;set mode
		ldi	Acc,_VIDEO_OUT_LEVEL_BLACK			;set white level to min
		call	_VIDEO_OUT_SET_WHITE_LEVEL
		ldiw	X,0
		ldiw	Y,0
		ldiw	Z,BMP_MOTION_DETECTOR_8*2			;Z-->MOTION DETECTOR IMAGE
		call 	_RASTER_SET_BMP_FAST				;show MOTION DETECTOR IMAGE
		ldiaw	70
		call	fn_FADE_IN					;fade in
		_M_DELAY_MILLISECONDS	500
		;///////////////////////////////////////
		;// SCROLL DOWN SHOW MOTION_DETECTOR LOGO 
		;///////////////////////////////////////

		ldiw	X,0
		ldiw	Y,0
		ldiaw	SCREEN_WIDTH
		ldiawt	178
		call	_RASTER_BITBLT_SET_DEST
		ldi	Acc,_RASTER_BITBLT_FUNCTION_SCROLL_DOWN
		ldiw	X,178
		ldiw	Y,0
		call	_RASTER_BITBLT
	
		;///////////////////////////////////////
		;// SHOW MOVE DETECTOR LOGO
		;///////////////////////////////////////
;		ldiw	X,0
;		ldiw	Y,42
;		ldiw	Z,BMP_VERSION_120*2				;Z-->VERSION
;		call 	_RASTER_SET_BMP_FAST				;show VERSION
;		ldiw	Z,_MSG_MOVE_DETECTOR				;show move detector message
;		call	_MENU_TRANSLATE_MESSAGE				;translate
;		call	_RASTER_GET_TEXT_WIDTH				;get width
;		ldiw	X,SCREEN_WIDTH
;		sub	XL,Acc
;		sbc	XH,AccH
;		lsr	XH
;		ror	XL
;		ldiw	Y,0
;		call	_RASTER_SET_TEXT_OUT_FLASH
;		_M_DELAY_MILLISECONDS	1000
;		ldiaw	60
;		call	fn_FADE_OUT					;fade out
		call	_VIDEO_OUT_CLS
		ldi	Acc,_VIDEO_OUT_LEVEL_WHITE_TOP			;restore normal white level
		call	_VIDEO_OUT_SET_WHITE_LEVEL
		;///////////////////////////////////////
		;// SHOW Release
		;///////////////////////////////////////
;		ldi	Acc,_VIDEO_MODE_96x72x16N			;set screen with internal sync
;		call	_VIDEO_OUT_SET_MODE
;		call	_VIDEO_OUT_CLS					;clear screen
;		call 	_MODULE_RESTORE_SYSTEM_CONFIGURATION		;because this screen destroy last configuration
;		_M_DELAY_MILLISECONDS	2000
		pop	Acc						;restore old mode
		call	_VIDEO_OUT_SET_MODE
		ret


