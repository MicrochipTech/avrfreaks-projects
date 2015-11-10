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
;=======> 	 	M O D U L E  -  D R A W  P A L E T T E  O N   S C R E E N			|
;	| 												|
;	 -----------------------------------------------------------------------------------------------
	_MODULE_DRAW_PALETTE:
		call	_VIDEO_OUT_GET_MODE				;get actual video mode
		push	Acc						;save this mode
		ldi	Acc,_VIDEO_MODE_24X24X16N
		call	_VIDEO_OUT_SET_MODE
		call	_VIDEO_OUT_CLS
		ldiw	X,0						;X=0
		ldi	Acc,0
	_MODULE_DRAW_PALETTE_10:
		ldiw	Y,0						;Y=0
		call	_GRAPH_MOVE_TO_EX
		ldiw	Y,16
		call	_GRAPH_LINE_TO
		addiw	X,1
		ldiw	Y,0						;Y=0
		call	_GRAPH_MOVE_TO_EX
		ldiw	Y,16
		call	_GRAPH_LINE_TO
		addiw	X,1
		ldiw	Y,0						;Y=0
		call	_GRAPH_MOVE_TO_EX
		ldiw	Y,16
		call	_GRAPH_LINE_TO
		addiw	X,1
		subi	Acc,-2
		cpiw	X,24						;x>width ?
		brlo	_MODULE_DRAW_PALETTE_10				;no, continue
		ldiw	X,0						;black box
		ldiw	Y,16
		ldiaw	12
		ldiawt	8
		call	_RASTER_BITBLT_SET_DEST
		ldi	Acc,_RASTER_BITBLT_FUNCTION_BLACK
		call	_RASTER_BITBLT
		ldiw	X,12						;white box
		ldiw	Y,16
		ldiaw	12
		ldiawt	8
		call	_RASTER_BITBLT_SET_DEST
		ldi	Acc,_RASTER_BITBLT_FUNCTION_WHITE
		call	_RASTER_BITBLT
		call	_KEYBOARD_READ					;wait any key to return
		pop	Acc
		call	_VIDEO_OUT_SET_MODE				;restore default mode
		call	fn_MENU_DRAW_BACKGROUND				;redraw background
		call	_MENU_DRAW_STACK				;redraw menus in stack
		ret
