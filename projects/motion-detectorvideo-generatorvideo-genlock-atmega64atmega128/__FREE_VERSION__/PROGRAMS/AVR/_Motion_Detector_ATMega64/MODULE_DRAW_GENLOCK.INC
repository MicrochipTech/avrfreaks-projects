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
;=======>     M O D U L E  -  D R A W  C R O S S  O N  O N  S U P E R I M P O S E   S C R E E N		|
;	| 												|
;	 -----------------------------------------------------------------------------------------------
	_MODULE_DRAW_SUPERIMPOSED:
		call	_VIDEO_OUT_GET_MODE				;get actual video mode
		push	Acc						;save this mode
		call	fn_VIDEO_PRESENT				;se if video present ?
		lbrcs	_MODULE_DRAW_SUPERIMPOSED_EXIT			;no, exit
		ldi	Acc,_VIDEO_MODE_128X96X2G			;set screen genlocked mode
		call	_VIDEO_OUT_SET_MODE
		call	_VIDEO_OUT_CLS
		ldiw	X,0
		ldiw	Y,0
		ldiaw	SCREEN_WIDTH
		ldiawt	SCREEN_HEIGHT
		ldi	Temp,_ON
		call	_RASTER_SET_BOX
		;//////////////////////////////
		;// 1st quadrant
		;//////////////////////////////
		ldiw	X,96						;1st quadrant
		ldiw	Y,12
		call	_GRAPH_MOVE_TO_EX
		ldiw	Y,36
		ldi	Acc,_ON
		call	_GRAPH_LINE_TO
		ldiw	X,72
		ldiw	Y,24
		call	_GRAPH_MOVE_TO_EX
		ldiw	X,120
		ldi	Acc,_ON
		call	_GRAPH_LINE_TO
		;//////////////////////////////
		;// 2nd quadrant
		;//////////////////////////////
		ldiw	X,32						;2nd quadrant
		ldiw	Y,12
		call	_GRAPH_MOVE_TO_EX
		ldiw	Y,36
		ldi	Acc,_ON
		call	_GRAPH_LINE_TO
		ldiw	X,8
		ldiw	Y,24
		call	_GRAPH_MOVE_TO_EX
		ldiw	X,56
		ldi	Acc,_ON
		call	_GRAPH_LINE_TO
		;//////////////////////////////
		;// 3rd quadrant
		;//////////////////////////////
		ldiw	X,32						;3rd quadrant
		ldiw	Y,60
		call	_GRAPH_MOVE_TO_EX
		ldiw	Y,84
		ldi	Acc,_ON
		call	_GRAPH_LINE_TO
		ldiw	X,8
		ldiw	Y,72
		call	_GRAPH_MOVE_TO_EX
		ldiw	X,56
		ldi	Acc,_ON
		call	_GRAPH_LINE_TO
		;//////////////////////////////
		;// 4rt quadrant
		;//////////////////////////////
		ldiw	X,96						;4nt quadrant
		ldiw	Y,60
		call	_GRAPH_MOVE_TO_EX
		ldiw	Y,84
		ldi	Acc,_ON
		call	_GRAPH_LINE_TO
		ldiw	X,72
		ldiw	Y,72
		call	_GRAPH_MOVE_TO_EX
		ldiw	X,120
		ldi	Acc,_ON
		call	_GRAPH_LINE_TO
		;//////////////////////////////
		;// central cross
		;//////////////////////////////
		ldiw	X,64						;central cross
		ldiw	Y,0
		call	_GRAPH_MOVE_TO_EX
		ldiw	Y,96
		ldi	Acc,_ON
		call	_GRAPH_LINE_TO
		ldiw	X,0
		ldiw	Y,48
		call	_GRAPH_MOVE_TO_EX
		ldiw	X,SCREEN_WIDTH
		ldi	Acc,_ON
		call	_GRAPH_LINE_TO
	
		call	_KEYBOARD_READ					;wait any key to return
	_MODULE_DRAW_SUPERIMPOSED_EXIT:
		pop	Acc
		call	_VIDEO_OUT_SET_MODE				;restore default mode
		call	fn_MENU_DRAW_BACKGROUND				;redraw background
		call	_MENU_DRAW_STACK				;redraw menus in stack
		ret
