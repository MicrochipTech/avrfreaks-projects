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
;=======> 	      			M O D U L E  -  C L E A R   G R I D		 		|
;	| 												|
;	 -----------------------------------------------------------------------------------------------
;	| Description:											|
;	| 	clear capture grid, set all cells to transparent					|
;	 -----------------------------------------------------------------------------------------------

	_MODULE_CLEAR_GRID:	
		ldiw	Z,_MSG_REALLY_CLEAR				;really clear?
		call	fn_INPUT_BOX_CONFIRM				;input box confirm msg
		cpi	Acc,_MENU_INPUT_BOX_ANSWER_YES			;YES?
		brne	_MODULE_CLEAR_GRID_EXIT_B			;no, branch
		;+-----------------------
		;| clear grid
		;+-----------------------
		ldi	AccT,0
		ldi	AccTH,0
	_MODULE_CLEAR_GRID_10:
		ldi	Temp,_OFF
		call	fn_SET_GRID_CELL
		inc	AccTH						;col++
		cpi	AccTH,_VIDEO_DIGITIZING_MASK_COLS
		brlo	_MODULE_CLEAR_GRID_10
		ldi	AccTH,0						;col=0
		inc	AccT						;row++
		cpi	AccT,_VIDEO_DIGITIZING_MASK_ROWS
		brlo	_MODULE_CLEAR_GRID_10
		call	fn_SAVE_CAPTURE_GRID				;else save data into EEPROM
		rjmp	_MODULE_CLEAR_GRID_EXIT_C			;and exit
	_MODULE_CLEAR_GRID_EXIT_B:
		cpi	Acc,_MENU_INPUT_BOX_ANSWER_NO			;NO?
		brne	_MODULE_CLEAR_GRID
	_MODULE_CLEAR_GRID_EXIT_C:
		call	fn_MENU_DRAW_BACKGROUND				;redraw background
		call	_MENU_DRAW_STACK				;redraw menus in stack
		ret
