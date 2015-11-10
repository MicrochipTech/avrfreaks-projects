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
;=======>      M O D U L E  -  F O R C E  S Y S T E M  R E S E T  ( C O L D )                           |
;	| 												|
;	 -----------------------------------------------------------------------------------------------
;	| Description:											|
;	| 	This Module Reset system and put into cold boot        					|
;	 -----------------------------------------------------------------------------------------------
	_MODULE_RESET:
                ;clear screen
                call    _RASTER_CLEAR
                ;show reseting messsage
                ldiw    X,0
                ldiw    Y,0
		ldiw	Z,_MSG_RESETING*2				;reset message
		call	_RASTER_SET_TEXT_OUT_FLASH
                ;draw time reset box
                ldiw    x,0
                ldiw    y,32
                ldiaw   SCREEN_WIDTH
                ldiawt  32
                ldi     Temp,255
                call    _RASTER_SET_BOX
                ;show animation that machine will reset
                ldiw    x,2                                     ;start x=1 inside the before drawed bo
                ldiw    y,34                                    ;start y=34
                ldiawt  28                                     ;height =28
        _MODULE_RESET_10:
                ldi     Acc,255                                 ;color=white
                call    _RASTER_SET_VLINE                       ;draw hline
                _M_DELAY_MILLISECONDS   6                       ;wait 6 milliseconds
                addiw   x,1                                     ;next x coord
                cpiw    x,SCREEN_WIDTH-2                        ;fill the box?
                brlo    _MODULE_RESET_10                        ;no, next x coord
                _M_DELAY_MILLISECONDS	100                    ;wait 100
                //define cold boot
                LDI     Acc,0
                STS     _BOOT_SIGNATURE_BYTE_1,ACC
                STS     _BOOT_SIGNATURE_BYTE_2,ACC
                STS     _BOOT_SIGNATURE_BYTE_3,ACC
                //reset
		JMP     0
        _MSG_RESETING:
                ;1234567890123456789012
        .DB	"     Reseting...      ",0,0