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
;=======>      M O D U L E  -  S Y S T E M  C H E C K							|
;	| 												|
;	 -----------------------------------------------------------------------------------------------
;	| Description:											|
;	| 	This Module check the system and show results on screen					|
;	 -----------------------------------------------------------------------------------------------
.DSEG
        _HEX_CHECKSUM:  .BYTE     6
.CSEG
	_MODULE_BOOT:
		ldi	Acc,_VIDEO_MODE_128X96X2N			;set screen with internal sync
		call	_VIDEO_OUT_SET_MODE
		call	_VIDEO_OUT_CLS
		ldiw	X,0
		ldiw	Y,11*_RASTER_CHAR_HEIGHT			;initial row position
		ldi	Acc,0						;number of lines
	_MODULE_BOOT_10:
		push	Acc
		pushw	X
		pushw	Y
		ldiw	Z,_MODULE_BOOT_MSG_1*2				;boot message
		call	_RASTER_SET_TEXT_OUT_FLASH
		popw	Y
		popw	X
		subiw	Y,_RASTER_CHAR_HEIGHT
		pop	Acc
		inc	Acc
		;///////////////////////////////////
		;// SHOW VERSION
		;///////////////////////////////////
		push	Acc
		pushw	X
		pushw	Y
		ldiw	X,12*_RASTER_CHAR_WIDTH				;show video state
		addiw	Y,8*_RASTER_CHAR_HEIGHT
		ldiw	Z,_MODULE_BOOT_MSG_2*2				;Z->Present
		call	_VIDEO_OUT_GET_SYNC_STATE			;get sync state
		cpi	Acc,_ON
		breq	_MODULE_BOOT_20					;yes, branch
		ldiw	Z,_MODULE_BOOT_MSG_3*2				;Z->No signal
	_MODULE_BOOT_20:
		call	_RASTER_SET_TEXT_OUT_FLASH
		popw	Y
		popw	X
		pushw	X
		pushw	Y
		ldiw	X,12*_RASTER_CHAR_WIDTH
		addiw	Y,10*_RASTER_CHAR_HEIGHT			;show version
		ldiaw	VERSION
		call	_UITOS
		ldd	Acc,Z+4						;put comma
		std	Z+5,Acc
		ldd	Acc,Z+3
		std	Z+4,Acc
		ldd	Acc,Z+2
		std	Z+3,Acc
		ldd	Acc,Z+1
		std	Z+2,Acc
		ldi	Acc,'.'
		std	Z+1,Acc
		call	_RASTER_SET_TEXT_OUT_SRAM
		_M_DELAY_MILLISECONDS	10
		popw	Y
		popw	X
		pop	Acc
		cpi	Acc,12
		lbrlo	_MODULE_BOOT_10
		;///////////////////////////////////
		;// SHOW APRESENTATION MESSAGE IF CHECKED
		;///////////////////////////////////
		ldiw	Z,_MENU_PORTUGUESE_MENSAGEM_APRESENTACAO*2	;check if presentation message is checked
		ldi	AccH,_MENU_MENSAGEM_APRENSENTACAO_ITEM_ATIVAR
		call	_MENU_GET_ITEM_CHECK
		cpi	Acc,_ON						;CHECKED
		brne	_MODULE_BOOT_30					;no, branch
		ldiw	Z,E_MSG_APRESENTACAO				;Z-->presentation message
		call	fn_SHOW_E_MESSAGE				;show message
		
	_MODULE_BOOT_30:
		;///////////////////////////////////
		;// COMPUTE AND SHOW CHECKSUM
		;///////////////////////////////////
                ldiw    x,8
                ldiw    y,8*11+2
                ldiaw   0                                               ;hold counter
                ldiawt  0                                               ;hold checksum
                ldiz    0                                               ;hold flash pointer
        _MODULE_BOOT_40:
                push    Acc
                lpm     Acc,Z+
                add     AccT,Acc
                ldi     Acc,0
                adc     AccTH,Acc
                pop     Acc
                ;check if in page
                cpi     Acc,0
                brne    _MODULE_BOOT_50                                 ;no in page, branch
                call    _MODULE_BOOT_SHOW_CHECKSUM                      ;SHOW CHECKSUM
                addiw   x,1
                pushaw
                pushawt
                pushw   x
                ldiawt  4
                lsrw    X
                ldi     Acc,255
                call    _RASTER_SET_VLINE
                _M_DELAY_MILLISECONDS   5
                popw    x
                popawt
                popaw
        _MODULE_BOOT_50:
                subi    Acc,-1
                sbci    AccH,-1
                brne    _MODULE_BOOT_40
                call    _MODULE_BOOT_SHOW_CHECKSUM                      ;SHOW FINAL CHECKSUM
                ;SHOW SYSTEM READY
                ldiw    x,6                                             ;clear percent bar
                ldiw    y,8*11+2
                ldiaw  128
                ldiawt  4
                ldi     Temp,0                                          ;black color border
                ldi     TempH,0                                         ;black color fill   
                call   _RASTER_SET_BOX_FULL

		ldiw    x,6                                             ;show ok
		ldiw    y,8*2
		ldiw	Z, _MODULE_BOOT_MSG_4*2				;boot ok
		call	_RASTER_SET_TEXT_OUT_FLASH
;                call    _KEYBOARD_READ                                  ;only for test place with comments when running
                _M_DELAY_MILLISECONDS	2000                            ;time to  show info
		ret
_MODULE_BOOT_SHOW_CHECKSUM:      
                pushaw                                                  ;save useds
                pushawt
                pushw   z
                pushw   x
                pushw   y
                ldiz    _HEX_CHECKSUM                                   ;convert to hex
                mov     Acc,AccT
                call    _DUMPHEX
                mov     Acc,AccTH
                call    _DUMPHEX
                ldi     Acc,0                                           ;insert zero terminator
                st      Z,Acc                                           ;plot check
                ldiw    X,6*12
                ldiw    Y,8*10
		ldiw	Z,_HEX_CHECKSUM 				;check sum in memory
		call    _RASTER_SET_TEXT_OUT_SRAM
                popw    y
                popw    x
                popw    z
                popawt
                popaw
                ret
	_MODULE_BOOT_MSG_1:
		.DB	">MOTION_DETECTOR      ",_cr,_lf
		.DB	">                     ",_cr,_lf
		.DB	">System booting...    ",_cr,_lf
		.DB	">                     ",_cr,_lf
		.DB	">Processor :ATMEGA64  ",_cr,_lf
		.DB	">Flash size:64K       ",_cr,_lf
		.DB	">SRAM size :4K        ",_cr,_lf
		.DB	">Video in  :          ",_cr,_lf
		.DB	">Bus       :ok        ",_cr,_lf
		.DB	">BIOS vers.:          ",_cr,_lf
		.DB	">Check sum :----      ",_cr,_lf
		.DB	">                     ",_cr,_lf
		.DB	">                     ",0,0
	_MODULE_BOOT_MSG_2:
		.DB	"Present",0
	_MODULE_BOOT_MSG_3:
		.DB	"No signal",0
        _MODULE_BOOT_MSG_4:
                .DB     "System Ready          ",0,0
