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
;>--------------------------------------------------------------<---+
;| B I G  N U M B E R S  F O R  D I S P L A Y  4 4 7 8 0 	|
;+--------------------------------------------------------------+
;| THIS DRIVE HAS INTEND TO USE WITH AVRS WITH SRAM		|
;| THIS DRIVE TRY TO KEEP FLASH MEMORY CONSUMPTION TO MINIMUN	|
;+--------------------------------------------------------------+
;| C O M M E N T S  A N D  U P D A T E S 			|
;+--------------------------------------------------------------+
#ifndef __DISP_HD44780_V2__
	.message "+--------------------------+
	.message "| !!! NEED HD44780 DRIVE !!!
	.message "+--------------------------+
#endif
#ifndef __DISP_BIG_NUMBER__
	#define __DISP_BIG_NUMBER__
	.message "[ (DISPLAY) BIG NUMBER ]"
	;>--------------------------------------------------------------<
	;| I M P L E M E N T E D  F U N C T I O N S  D E T A I L S	|
	;+--------------------------------------------------------------+-------------------------------+
	;|												|
	;| _DISP_BIG_NUMBER_INIT INITIALIZE BIG NUMBER DRIVER						|
	;|			 OBS: THIS DRIVE NEED OF COMPATIBLE DRIVE OF HD44780 			|
	;|----------------------------------------------------------------------------------------------|
	;| __DISP_PRINT_BIG	PRINT BIG NUMBER AT COLUMN						|
	;| 			SAMPLE USING DISP_PRINT_BIG TO SHOW '3' DIGIT AT 4ST COLUNM		|
	;|												|
	;|			LDI 	Acc,'3'								|
	;|			LDI	AccH,4								|
	;|			RCALL	_DISP_PRING_BIG							|
	;|												|
	;+----------------------------------------------------------------------------------------------+
	;| OBS: CALL FUNCTION DISP_BIG_NUMBER_INIT BEFORE USE ANY OTHER FUNCTION 			|
	;+----------------------------------------------------------------------------------------------+
	;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
	;+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
	;|   INITIALIZE BIG NUMBERS	  			|
	;|  				  			|
	;|\			          			|
	;| | Input  : none		      			|
	;|/  Output : none				        |
	;|   Destroy: none	          			|
	;|   Obs: disable interrupts during initialization	|
	;+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+ 
	;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
	;   \__\__\__\__\__\__\__\__\__\__\__\__\__\__\__\__\__\________________________________________________
	_DISP_BIG_NUMBER_INIT:
		push	Acc					;save SREG
		in	Acc,SREG
		cli						;disable interrupts
		push	Acc
		ldiz	_DISP_DCHAR0*2
		ldi	Acc,0
		rcall	_DISP_REDEFINE_CHAR
		ldiz	_DISP_DCHAR1*2
		ldi	Acc,1
		rcall	_DISP_REDEFINE_CHAR
		ldiz	_DISP_DCHAR2*2
		ldi	Acc,2
		rcall	_DISP_REDEFINE_CHAR
		ldiz	_DISP_DCHAR3*2
		ldi	Acc,3
		rcall	_DISP_REDEFINE_CHAR
		ldiz	_DISP_DCHAR4*2
		ldi	Acc,4
		rcall	_DISP_REDEFINE_CHAR
		ldiz	_DISP_DCHAR5*2
		ldi	Acc,5
		rcall	_DISP_REDEFINE_CHAR
		ldiz	_DISP_DCHAR6*2
		ldi	Acc,6
		rcall	_DISP_REDEFINE_CHAR
		ldiz	_DISP_DCHAR7*2
		ldi	Acc,7
		rcall	_DISP_REDEFINE_CHAR
		pop	ACC
		out	SREG,Acc				;restore sreg
		pop	Acc
		ret
	;+----------------------------------------------+
	;| BIG NUMBER CHARACTER REDEFINTION TABLE	|
	;+----------------------------------------------+
	_DISP_DCHAR0:							; |
		.DB 0X01,0X01,0X01,0X01,0X01,0X01,0X1F,0X00		; |
									;-+
	_DISP_DCHAR1:							;+-
		.DB 0X1F,0X10,0X10,0X10,0X10,0X10,0X10,0X10		;|
									;|
	_DISP_DCHAR2:							;-+
		.DB 0X1F,0X01,0X01,0X01,0X01,0X01,0X01,0X01		; |
									; |
	_DISP_DCHAR3:							; |
		.DB 0X01,0X01,0X01,0X01,0X01,0X01,0X01,0X00		; |
									; |
	_DISP_DCHAR4:							;-+
		.DB 0X1F,0X01,0X01,0X01,0X01,0X01,0X1F,0X00		; |
									;-+
	_DISP_DCHAR5:							;+-
		.DB 0X1F,0X10,0X10,0X10,0X10,0X10,0X1F,0X00		;|
									;+-
	_DISP_DCHAR6:							;--
		.DB 0X1F,0X00,0X00,0X00,0X00,0X00,0X1F,0X00		;
									;--
	_DISP_DCHAR7:							;--
		.DB 0X1F,0X00,0X00,0X00,0X00,0X00,0X00,0X00		;
	;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
	;+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
	;|   BIG NUMBER PRINTING	  			|
	;|  				  			|
	;|\			          			|
	;| | Input  : Acc number in Ascii      			|
	;| |	      AccH column				|
	;|/  Output : none				        |
	;|   Destroy: none	          			|
	;|   							|
	;+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+ 
	;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
	;   \__\__\__\__\__\__\__\__\__\__\__\__\__\__\__\__\__\________________________________________________
								;
	; numbers definitions
	;
	; +- -+     |   -- -+  -- -+ |   | +- -- +-    -- -+ +- -+ +- -+
	; |1 2| '' 3|    6 4|  6  4| |L 0| |5 6  |1 ''  7 2| |5 4| |5 4|
	; |   |     |   -- -+  -- -+ +- -+ +- -- |	   | +- -+ +- -+
	; |   |     |   |      	   |     |     | +- -+	   | |   |     |
	; |L 0| '' 3|   |L  _   _ 0| '' 3|  - 0| |5 4| '' 3| |L 0| '' 3|
	; +- -+     |   +- --  -- -+     | -- -+ +- -+     | +- -+     |
	_DISP_PRINT_BIG_NUMBER:
		push	ZL
		push	ZH
		push	AccH
		push	Acc					;Save number
		ldi	Acc,1					;Always row 1
		rcall	_DISP_LOCATE
		pop	Acc					;restore number
		ldiz	_DISP_BIG_NUM_TABLE_ROW1*2		;get big row 1
		rcall	_DISP_PUT_CHAR_DEF
		push	Acc					;Save number
		ldi	Acc,2					;Always row 2
		rcall	_DISP_LOCATE
		pop	Acc					;restore number
		ldiz	_DISP_BIG_NUM_TABLE_ROW2*2		;get big row 2
		rcall	_DISP_PUT_CHAR_DEF
		pop	AccH
		pop	ZH
		pop	ZL
		ret
	;+----------------------------------------------+
	;| PLOT ROW					|
	;| input: Z--> char definitions table		|
	;|        Acc: number is AScii			|
	;+----------------------------------------------+
	_DISP_PUT_CHAR_DEF:
		push	Acc					;save again
		subi	Acc,'0'					;convert ascii to byte
		lsl	Acc					;remember 2 coluns per number
		add	ZL,Acc
		ldi	Acc,0
		adc	ZH,Acc
		lpm						;get char definition
		mov	Acc,r0
		adiw	ZL,1					;next definition
		rcall	_DISP_DATA_WRITE			;write
		lpm						;get next char definition
		mov	Acc,r0
		rcall	_DISP_DATA_WRITE
		pop	Acc
		ret
	;+----------------------------------------------+
	;| BIG NUMBER ASSEMBLER DEFINITION TABLE	|
	;+----------------------------------------------+
	_DISP_BIG_NUM_TABLE_ROW1:
		.DB	  1,2,' ',3,  6,  4,  6,4,'L',0,  5,6,1,' ',  7,2,  5,4,  5,4
	_DISP_BIG_NUM_TABLE_ROW2:
		.DB	'L',0,' ',3,'L','_','_',0,' ',3,'_',0,5,  4,' ',3,'L',0,' ',3
#endif
.exit

