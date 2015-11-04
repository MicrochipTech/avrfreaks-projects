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
;+--------------------------------------------------------------+---+
;+------------------------------------------------------+
;| DISPLAY DRIVE FOR T6963C TOSHIBA CHIP		|
;| BY JOÃO DARTAGNAN ANTUNES OLIVEIRA			|
;| DATE:12/12/2000 VERSION 1.0				|
;>------------------------------------------------------<
;| D I S P L A Y  D R I V E  1 2 8  X  1 2 8 		|
;+------------------------------------------------------+
;| THIS DRIVE HAS INTEND TO USE WITH AVRS WITH SRAM	|
;+------------------------------------------------------+
;| PART II ROUTINES IMPLEMENTATIONS			|
;+------------------------------------------------------+
;+----------------------------------------------------------------------+
;| LCD DISPLAY ROTINES FOR T6963C MICROCONTROLES USING 128X128 PIXELS	|
;| 6X8 CHARACTER SIZE							|
;+----------------------------------------------------------------------+
;+--------------+
;| INIT DISPLAY |
;+==============+===============================================================================
_INIT_DISPLAY:
_DISP_INIT:
	in	Acc,_DISP_PORT_CMD_DIR			;get actual direction of display command port
							;set display commands bits as output level one
	ori	Acc,(1<<_DISP_BIT_WR) | (1<<_DISP_BIT_RD) | (1<<_DISP_BIT_CE) | (1<<_DISP_BIT_CD) | (1<<_DISP_BIT_RESET)
							;send display command initialization
	out	_DISP_PORT_CMD_DIR,Acc
	ser	Acc
	out	_DISP_PORT_CMD_OUT,Acc			;set all bits high level
	clr	Acc					
	out	_DISP_PORT_DATA_DIR,Acc			;set display data port as input
	rcall	DISPLAY_RESET				;reset a display
;-----------------------------------------------------------------------------------------------
;SET TEXT HOME ADDRESS
	push	Acc				;save useds
	ldi	Acc,low(_DISP_TEXT_ADDRESS)	;text home address
	rcall	_DISP_DATA
	ldi	Acc,high(_DISP_TEXT_ADDRESS)
	rcall	_DISP_DATA
	ldi	Acc,SETCONTROLWORD+TEXTHOMEADDR
	rcall	_DISP_CMD
;SET TEXT AREA
	ldi	Acc,_DISP_HARD_COLUNMS		;set columns
	rcall	_DISP_DATA
	ldi	Acc,0
	rcall	_DISP_DATA
	ldi	Acc,SETCONTROLWORD+TEXTAREA
	rcall	_DISP_CMD
;SET GRAPH HOME ADDRESS
	ldi	Acc,low(_DISP_GRAPH_ADDRESS)	;graph home address
	rcall	_DISP_DATA
	ldi	Acc,high(_DISP_GRAPH_ADDRESS)
	rcall	_DISP_DATA
	ldi	Acc,SETCONTROLWORD+GRAPHHOMEADDR
	rcall	_DISP_CMD
;SET GRAPH AREA
	ldi	Acc,_DISP_HARD_COLUNMS		;set columns
	rcall	_DISP_DATA
	ldi	Acc,0
	rcall	_DISP_DATA
	ldi	Acc,SETCONTROLWORD+GRAPHAREA
	rcall	_DISP_CMD
;SET MODE SET
	ldi	Acc,MODESET+CGROM+ORMODE	;internal caracter generator 
	rcall	_DISP_CMD
;SET ADDRESS POINTER
	ldi	Acc,0				;address pointer start at 0
	rcall 	_DISP_DATA
	ldi	Acc,0
	rcall	_DISP_DATA
	ldi	Acc,REGISTERSSETTING+ADDRESSPOINTER
	rcall	_DISP_CMD
;SET OFFSET
	ldi	Acc,0				;offset at zero
	rcall	_DISP_DATA
	ldi	Acc,0
	rcall	_DISP_DATA
	ldi	Acc,REGISTERSSETTING+OFFSETREGISTER
	rcall	_DISP_CMD
	
;SET TEXT ON,GRAPHICS OFF,CURSOR OFF
	ldi	Acc,DISPLAYMODE+TEXTON+GRAPHON+CURSOROFF+BLINKOFF
	rcall	_DISP_CMD
;CLEAR SCREEN TEXT AND GRAPHICS
	ldi	Acc,' '
	rcall	_DISP_CLS_TEXT
	rcall	_DISP_CLS_GRAPH
	pop	Acc
	ret
;+---------------------+
;| DIPLAY RESET	       |
;+=====================+========================================================================
DISPLAY_RESET:
	push	Acc
	push	AccH
	push	r0
	rcall	_DISP_R1M
	cbi	_DISP_PORT_CMD_OUT,_DISP_BIT_RESET	;active RESET
	rcall	_DISP_R1M
	sbi	_DISP_PORT_CMD_OUT,_DISP_BIT_RESET	;deactive RESET
	rcall	_DISP_R1M
	pop	r0
	pop	AccH
	pop	Acc
	ret
;+----------------------
;| 1 milesecond delay
;+----------------------
.EQU	_DISP_RESET_TIME	= (1*_AVR_CLOCK)/6000	;calculus for 1ms
_DISP_R1M:
	#ifdef __DEBUG_MODE__
		ret
	#endif
	ldi	Acc,low(_DISP_RESET_TIME)
	ldi	AccH,high(_DISP_RESET_TIME)
	clr	r0
_DISP_R100:
	subi	Acc,1
	sbci	AccH,0
	cp	Acc,r0
	cpc	AccH,r0
	brne	_DISP_R100
	ret
	
;+---------------------+
;| DIPLAY STATUS READ  |
;| output: Acc status  |	
;+=====================+========================================================================
_DISP_STATUS_READ:
	#ifdef __DEBUG_MODE__
		ret
	#endif
	sbi 	_DISP_PORT_CMD_OUT,_DISP_BIT_CD		;cd=command
_DISP_GETS:
	cbi	_DISP_PORT_CMD_OUT,_DISP_BIT_CE		;ce=low
	cbi	_DISP_PORT_CMD_OUT,_DISP_BIT_RD		;rd=low
	ldi	Acc,low(_AVR_CLOCK/3000000)
	dec	Acc
	brne	PC-1
	in	Acc,_DISP_PORT_DATA_IN			;status in Acc
	sbi	_DISP_PORT_CMD_OUT,_DISP_BIT_RD		;rd=high
	sbi	_DISP_PORT_CMD_OUT,_DISP_BIT_CE		;ce=high
;	LDI	ACC,0X0F
	ret	
;+-------------------+
;| DIPLAY DATA READ  |
;| output: Acc data  |	
;+===================+==========================================================================
_DISP_DATA_READ:
	#ifdef __DEBUG_MODE__
		ret
	#endif
	cbi 	_DISP_PORT_CMD_OUT,_DISP_BIT_CD		;cd=data
	rjmp	_DISP_GETS
;+----------------------+
;| DIPLAY COMMAND WRITE |
;| input: Acc data   	|	
;+======================+=======================================================================
_DISP_COMMAND_WRITE:
	#ifdef __DEBUG_MODE__
		ret
	#endif
	out	_DISP_PORT_DATA_OUT,Acc		;set data to be write
	ser	Acc
	out	_DISP_PORT_DATA_DIR,Acc		;set PORT as output
	sbi	_DISP_PORT_CMD_OUT,_DISP_BIT_CD	;cd=command
_DISP_PUTD:
	cbi	_DISP_PORT_CMD_OUT,_DISP_BIT_CE	;ce=low
	cbi	_DISP_PORT_CMD_OUT,_DISP_BIT_WR	;wr=low
	ldi	Acc,low(_AVR_CLOCK/3000000)
	dec	Acc
	brne	PC-1
	sbi	_DISP_PORT_CMD_OUT,_DISP_BIT_WR	;wr=high
	sbi	_DISP_PORT_CMD_OUT,_DISP_BIT_CE	;ce=high
	clr	Acc
	out	_DISP_PORT_DATA_DIR,Acc		;set PORT as input, my normal mode
	ser	Acc
	out	_DISP_PORT_DATA_OUT,Acc		;PORT with pull up
	ret
;+-------------------+
;| DIPLAY DATA WRITE |
;| input: Acc data   |
;+===================+==========================================================================
_DISP_DATA_WRITE:
	#ifdef __DEBUG_MODE__
		ret
	#endif
	out	_DISP_PORT_DATA_OUT,Acc		;set data to be write
	ser	Acc
	out	_DISP_PORT_DATA_DIR,Acc		;set PORT as output
	cbi	_DISP_PORT_CMD_OUT,_DISP_BIT_CD	;cd=data
	rjmp	_DISP_PUTD
;+----------------+
;| CAN READ/WRITE |
;+================+=============================================================================
_DISP_CANRW:
	#ifdef __DEBUG_MODE__
		ret
	#endif
	rcall	_DISP_STATUS_READ		;get display status
	andi	Acc,(_DISP_STA0M+_DISP_STA1M) 	;only status to check
	cpi	Acc,(_DISP_STA0M+_DISP_STA1M) 	;good to write ?
	brne	_DISP_CANRW
	ret	
	
;+---------------------+
;| DIPLAY SEND COMMAND |
;| input: Acc data     |	
;+=====================+========================================================================
_DISP_CMD:
	#ifdef __DEBUG_MODE__
		ret
	#endif
	push 	Acc			;save command to be send
	rcall	_DISP_CANRW		;verify if can write
	pop 	Acc			;restore command
	rcall	_DISP_COMMAND_WRITE	;write command
	ret
;+---------------------+
;| DIPLAY SEND ONE DATA|
;| input: Acc data     |	
;+=====================+========================================================================
_DISP_DATA:
	#ifdef __DEBUG_MODE__
		ret
	#endif
	push 	Acc			;save data to be send
	rcall	_DISP_CANRW		;verify if can write
	pop 	Acc			;restore command
	rcall	_DISP_DATA_WRITE		;write data
	ret
;+---------------------+
;| DIPLAY GET ONE DATA |
;| output:Acc data     |	
;+=====================+========================================================================
_DISP_DATAR:
	#ifdef __DEBUG_MODE__
		ret
	#endif
	rcall	_DISP_CANRW		;verify if can write
	rcall	_DISP_DATA_READ		;write data
	ret
;+---------------------+
;| DIPLAY AUTO WRITE   |
;| input: Acc data     |	
;+=====================+========================================================================
_DISP_AUTOWRITE:
	#ifdef __DEBUG_MODE__
		ret
	#endif
	push	Acc
_DISP_A00:
	rcall	_DISP_STATUS_READ
	andi	Acc,_DISP_STA3M
	cpi	Acc,_DISP_STA3M
	brne	_DISP_A00
	pop	Acc
	rcall	_DISP_DATA_WRITE
	ret
;+---------------------+
;| DIPLAY AUTO WRITE   |
;| Output:Acc data     |	
;+=====================+========================================================================
_DISP_AUTOREAD:
	#ifdef __DEBUG_MODE__
		ret
	#endif
	rcall	_DISP_STATUS_READ
	andi	Acc,_DISP_STA2M
	cpi	Acc,_DISP_STA2M
	brne	_DISP_AUTOREAD
	rcall	_DISP_DATA_READ
	ret
;+---------------------------------------+
;| SET ADDRESS AT (LINE,COLUNM) POSITION |
;| input: XL --> colunm 		 |
;|	  YL --> line 			 |
;+=======================================+======================================================
; warning , this routine must be change according display number colunms
_DISP_SET_LC:
	push	XL			;save registers
	push	XH
	push	YL
	push 	YH
	push	Acc
	dec	XL
	dec	YL
;--------------------------------------------------------------
	ldi	YH,0			;Y*32+X 
	ldi	XH,0
	lsl	YL
	rol	YH
	lsl	YL
	rol	YH
	lsl	YL
	rol	YH
	lsl	YL
	rol	YH
	lsl	YL
	rol	YH
	add	YL,XL
	adc	YH,XH
;--------------------------------------------------------------
	mov	Acc,YL			;set address pointer
	rcall 	_DISP_DATA
	mov	Acc,YH
	rcall	_DISP_DATA
	ldi	Acc,REGISTERSSETTING+ADDRESSPOINTER
	rcall	_DISP_CMD
	pop	Acc
	pop	YH			;restore registers
	pop	YL
	pop	XH
	pop	XL
	ret
;+-------------------------------+
;| SET ADDRESS AT (X,Y) POSITION |
;| input: XL --> X	 	 |
;|	  YL --> Y 		 |
;+===============================+==============================================================
; warning , this routine must be change according display number colunms
_DISP_SET_XY:
	push	XL					;save registers
	push	XH
	push	YL
	push 	YH
	push	Acc
	push	AccH
;-------------------------------------------------------------
	mov	Acc,XL
	rcall	DIV6					;translate X to colunm X=X/6
	mov	XL,Acc
	ldi	YH,0					;Y*32+X 
	ldi	XH,0
	lsl	YL
	rol	YH
	lsl	YL
	rol	YH
	lsl	YL
	rol	YH
	lsl	YL
	rol	YH
	lsl	YL
	rol	YH
	add	YL,XL
	adc	YH,XH
;-------------------------------------------------------------
	ldi	Acc,low(_DISP_GRAPH_ADDRESS) 		;add graph offset
	add	YL,Acc
	ldi	Acc,high(_DISP_GRAPH_ADDRESS)
	adc	YH,Acc
	mov	Acc,YL					;set address pointer
	rcall 	_DISP_DATA
	mov	Acc,YH
	rcall	_DISP_DATA
	ldi	Acc,REGISTERSSETTING+ADDRESSPOINTER
	rcall	_DISP_CMD
	pop	AccH
	pop	Acc
	pop	YH					;restore registers
	pop	YL
	pop	XH
	pop	XL
	ret
;+---------------------------------------+
;| CLEAR SCREEN TEXT  			 |
;| input: Acc character to fill screen   |
;+=======================================+======================================================
_DISP_CLS:
	push	Acc
	ldi	Acc,' '
	rcall	_DISP_CLS_TEXT
	pop	Acc
	ret
_DISP_CLS_TEXT:
	push	XL				;save registers
	push	XH
	push	YL
	push 	YH
	push	Acc
	push	AccH
	push	AccT
	ldi	XL,1				;set first colunm
	ldi	YL,1				;set first line
DISPCA0:
	rcall	_DISP_SET_LC
	push	Acc
	ldi	Acc,DATAAUTO+DATAAUTOWRITE	;set auto write
	rcall	_DISP_CMD
	pop 	Acc
	ldi	AccH,_DISP_HARD_COLUNMS		;number off characters to be clead
_DISP_C00:
	push	Acc
	subi	Acc,0x20			;to convert to non ascii code
	rcall	_DISP_AUTOWRITE			;write data
	pop	Acc
	dec	AccH				;next position
	brne	_DISP_C00			;until all characters into line filled
	push	Acc
	ldi	Acc,DATAAUTO+AUTORESET		;reset auto write
	rcall	_DISP_CMD
	pop	Acc
	inc	YL				;next line
	cpi	YL,_DISP_HARD_LINES+1
	brlo	DISPCA0				;until all lines filled
	pop	AccT
	pop	AccH
	pop	Acc
	pop	YH				;restore registers
	pop	YL
	pop	XH
	pop	XL
	ret
;+---------------------------------------+
;| CLEAR SCREEN GRAPH  			 |
;| GPAGE : grafics page active		 |
;+=======================================+======================================================
_DISP_CLS_GRAPH:
	push	XL				;save registers
	push	XH
	push	YL
	push 	YH
	push	Acc
	push	AccH
	push 	r18
	ldi	Acc,low(_DISP_GRAPH_ADDRESS)	;set address pointer to graph area
	rcall 	_DISP_DATA
	ldi	Acc,high(_DISP_GRAPH_ADDRESS)
	rcall	_DISP_DATA
	ldi	Acc,REGISTERSSETTING+ADDRESSPOINTER
	rcall	_DISP_CMD
	ldi	Acc,DATAAUTO+DATAAUTOWRITE	;set auto write
	rcall	_DISP_CMD
	ldi	AccH,_DISP_HARD_LINES*8		;number off characters in graph mode to be clead
_DISP_CG00:
	ldi	r18,_DISP_HARD_COLUNMS
_DISP_CG01:
	ldi	Acc,0x00
	rcall	_DISP_AUTOWRITE			;write data
	dec	r18				;next position
	brne	_DISP_CG01
	dec	AccH				;next position
	brne	_DISP_CG00			;until all display filled
	ldi	Acc,DATAAUTO+AUTORESET		;reset auto write
	rcall	_DISP_CMD
	pop	r18
	pop	AccH
	pop	Acc
	pop	YH				;restore registers
	pop	YL
	pop	XH
	pop	XL
	ret
;+---------------------------------------+
;| Output data to display with autowrite |
;| input: Acc data to be output 	 |
;+=======================================+=======================================================
_DISP_DATAAUTO:
	push	Acc
	ldi	Acc,DATAAUTO+DATAAUTOWRITE	;set auto write
	rcall	_DISP_CMD
	pop	Acc
	push	Acc
	subi	Acc,0x20			;translate ASCII to TOSHIBA
	rcall	_DISP_AUTOWRITE			;write data	
	ldi	Acc,DATAAUTO+AUTORESET		;reset auto write
	rcall	_DISP_CMD
	pop	Acc
	ret
;+--------------------------------+
;| Display send string   	  |
;| input: Z --> string into FLASH |
;+--------------------------------+-----------------------------------------
_DISP_SEND_STR:
	push	r0				;save used
	push	Acc
_DISP_SEND00:
	lpm					;get data from program memory
	mov	Acc,r0				;end string ?
	cpi	Acc,0
	breq	_DISP_SEND_DONE			;yes , done
	rcall	_DISP_DATAAUTO			;send data to display
	adiw	ZL,1
	rjmp	_DISP_SEND00
_DISP_SEND_DONE:
	pop	Acc				;restore used
	pop	r0
	ret
;+--------------------------------+
;| Display send string   	  |
;| input: Z --> string into SRAM  |
;+--------------------------------+-----------------------------------------
_DISP_SEND_STR_S:
	push	Acc				;save used
_DISP_SENDM00:
	ld	Acc,Z+				;get data from program memory
	cpi	Acc,0				;end string ?
	breq	_DISP_SENDM_DONE			;yes , done
	rcall	_DISP_DATAAUTO			;send data to display
	rjmp	_DISP_SENDM00
_DISP_SENDM_DONE:
	pop	Acc				;restore used
	ret
;+--------------------+
;| Display locate     |
;| input: Acc line    |
;|        AccH column |
;+--------------------+------------------------------------------------------
_DISP_LOCATE:
	push	XL				;save useds
	push	YL
	mov	YL,Acc
	mov	XL,AccH
	rcall	_DISP_SET_LC			;call function
	pop	YL	
	pop	XL
	ret
;+---------------------------------------+
;| Put pixel	 			 |
;| input: XL = X coordinate	 	 |
;|	  YL = Y coordinate		 |
;|	  Acc 0 reset 0xff set pixel	 |
;+---------------------------------------+
; warning , this routine must be change according display number colunms
_DISP_PSET:
	push	YL
	cpi	XH,0			;x in range ?
	brne	_DISP_EXIT
	cpi	XL,128
	brsh	_DISP_EXIT
	cpi	YH,0			;y in range ?
	brne	_DISP_EXIT
	cpi	YL,128
	brsh	_DISP_EXIT
	rcall	_DISP_SET_XY		;set address
	push	XL
	push	Acc
	push	AccH
;	com	XL			;because counting bit is right to left
;	andi	XL,MASKBITS		;extract bit to be set
	push	Acc
	mov	Acc,XL
	rcall	DIV6
	pop	Acc
	ldi	XL,5
	sub	XL,AccH
	andi	Acc,BITSET		;select operation set/reset pixel
	or	Acc,XL			;join with bit to be set
	ori	Acc,BITSETRESET
	rcall	_DISP_CMD		;issue set/reset bit command
	pop	AccH
	pop	Acc
	pop	XL
_DISP_EXIT:	
	pop	YL
	ret
;+---------------------------------------+
;| Get pixel	 			 |
;| input: XL = X coordinate	 	 |
;|	  YL = Y coordinate		 |
;| Output:				 |
;|	  Acc 0 reset 0xff pixel set	 |
;+---------------------------------------+
; warning , this routine must be change according display number colunms
_DISP_POINT:
	push	YL
	ldi	Acc,0XFF
	cpi	XH,0			;x in range ?
	brne	POINT_EXIT
	cpi	XL,128
	brsh	POINT_EXIT
	cpi	YH,0			;y in range ?
	brne	POINT_EXIT
	cpi	YL,128
	brsh	POINT_EXIT
	rcall	_DISP_SET_XY		;set address
	push	XL
	push	AccH
	ldi	Acc,DATAAUTO+DATAAUTOREAD	;set auto read
	rcall	_DISP_CMD
	mov	Acc,XL
	rcall	DIV6
	ldi	XL,5
	sub	XL,AccH			;xl bit to be test
	rcall	_DISP_AUTOREAD		;read a graph data
	ldi	AccH,0b00000001		;mask bits
POINT00:
	tst	XL			;bit 0 ?
	breq	POINT10			;yes , done
	lsl	AccH
	dec	XL
	rjmp	POINT00
POINT10:
	and	Acc,AccH
	breq	POINT20	
	ldi	Acc,0XFF
POINT20:
	push	Acc
	ldi	Acc,DATAAUTO+AUTORESET		;reset auto write
	rcall	_DISP_CMD
	pop	Acc
	pop	AccH
	pop	XL
POINT_EXIT:	
	pop	YL
	ret
;+---------------------------------------+
;| Divide Acc by 6 used for PSET ROTINE	 |
;| input: Acc number to divide		 |
;| Output: Acc quotient			 |
;|	   AccH remainder		 |
;+=======================================+=======================================================
DIV6:
	push	r18
	clr	Acch			;clear quotient
	clr	r18
DIV600:
	subi	Acc,6			;number-=6
	sbci	r18,0
	cpi	r18,0			;number>=0
	brlt	DIV6DONE		;yes, end rotine
	inc	AccH			;quotient++
	rjmp	DIV600
DIV6DONE:
	subi	Acc,-6			;restore remainder
	push	r0
	mov	r0,AccH			;put result in orden
	mov	AccH,Acc
	mov	Acc,r0
	pop	r0
	pop	r18
	ret
.EXIT

