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
;+------------------------------------------------------+
;| DISPLAY DRIVE FOR SED1335 EPSON CHIP			|
;| BY JOÃO DARTAGNAN ANTUNES OLIVEIRA			|
;| DATE:12/12/2000 VERSION 1.0				|
;>------------------------------------------------------<
;| D I S P L A Y  D R I V E  3 2 0  X  2 4 0 		|
;+------------------------------------------------------+
;| THIS DRIVE HAS INTEND TO USE WITH AVRS WITH SRAM	|
;+------------------------------------------------------+
;| PART II ROUTINES IMPLEMENTATIONS			|
;+------------------------------------------------------+
;+-----------------------------------+
;| DISPLAY CONSTANTS AND DEFINITIONS |
;+===================================+==========================================================
.EQU	_DISP_WIDTH		= 320
.EQU	_DISP_HEIGHT		= 240
.EQU	_DISP_SCALE_WIDTH	= 320
.EQU	_DISP_SCALE_HEIGHT	= 240
.EQU	_DP_BIT_DR	= 7
.EQU	_DP_BIT_TL	= 6
.EQU	_DP_BIT_IV	= 5
.EQU	_DP_BIT_WS	= 3
.EQU	_DP_BIT_M2	= 2
.EQU	_DP_BIT_M1	= 1
.EQU	_DP_BIT_M0	= 0Ini
.EQU	_DP_BIT_WF	= 7
.EQU	_DP_BIT_CM	= 7
.EQU	_DP_BIT_OV	= 4
.EQU	_DP_BIT_DM2	= 3
.EQU	_DP_BIT_DM1	= 2
.EQU	_DP_BIT_MX1	= 1
.EQU	_DP_BIT_MX0	= 0
.EQU	_DP_BIT_FC0	= 0
.EQU	_DP_BIT_FC1	= 1
.EQU	_DP_BIT_FP0	= 2
.EQU	_DP_BIT_FP1	= 3
.EQU	_DP_BIT_FP2	= 4
.EQU	_DP_BIT_FP3	= 5
.EQU	_DP_BIT_FP4	= 6
.EQU	_DP_BIT_FP5	= 7
.EQU	_DISP_START_TEXT_ADDR	= 0
.EQU	_DISP_START_GRAPH_ADDR	= 2048
.EQU	_DISP_BYTES_PER_ROW	= 40
.EQU	_DISP_Y_RES_TEXT	= 240
.EQU	_DISP_Y_RES_GRAPH	= 240
.EQU	_DISP_Y_RES_HARD	= 240
.EQU	_DISP_CHARACTERS_PER_ROW= 40
.EQU	_DP_SLOW		= 0X01
.EQU	_DP_FAST		= 0X02
.EQU	_DP_FX		= 7				;8 Pixels Character Width
.EQU	_DP_FY		= 7				;8 Pixels Character Height
.EQU	_DP_CR		= _DISP_CHARACTERS_PER_ROW-1	;40 Characters Per Row
.EQU	_DP_TCR		= _DISP_CHARACTERS_PER_ROW+7	;47 = CR+8
.EQU	_DP_LF		= _DISP_Y_RES_HARD-1		;240 Pixels of Display Height
.EQU	_DP_APL		= low(_DISP_BYTES_PER_ROW)	;40 Horizontal Address Range
.EQU	_DP_APH		= high(_DISP_BYTES_PER_ROW)
.EQU	_DP_SAD1L	= low(_DISP_START_TEXT_ADDR)	;Start Address of Text Screen
.EQU	_DP_SAD1H	= high(_DISP_START_TEXT_ADDR)
.EQU	_DP_SAD2L	= low(_DISP_START_GRAPH_ADDR)	;Start Address of Graph Screen
.EQU	_DP_SAD2H	= high(_DISP_START_GRAPH_ADDR)
.EQU	_DP_SAD3L	= low(0)
.EQU	_DP_SAD3H	= high(0)
.EQU	_DP_SAD4L	= low(0)
.EQU	_DP_SAD4H	= high(0)
.EQU	_DP_SL1		= _DISP_Y_RES_TEXT		;240 Pixels SAD1 length
.EQU	_DP_SL2		= _DISP_Y_RES_GRAPH		;240 Pixels SAD2 length
.EQU	_DP_CRX		= 7				;8 Pixels of Cursor Size Width
.EQU	_DP_CRY		= 7				;8 Pixels of Cursor Size Height
.EQU	_DISP_CMD_SYSTEM_SET	= 0X40			;INITIALIZE DEVICE AND DISPLAY
.EQU	_DISP_CMD_SLEEP_IN	= 0X53			;ENTER STANDBY MODE
.EQU	_DISP_CMD_DISP_OFF	= 0X58			;ENABLE AND DISABLE DISPLAY AND DISPLAY FLASHING
.EQU	_DISP_CMD_DISP_ON	= 0X59			
.EQU	_DISP_CMD_SCROLL	= 0X44			;SET DISPLAY START ADDRESS AND DISPLAY REGIONS
.EQU	_DISP_CMD_CSRFORM	= 0X5D			;SET CURSOR TYPE
.EQU	_DISP_CMD_CGRAM_ADR	= 0X5C			;SET START ADDRESS OF CHARACTER GENERATOR RAM
.EQU	_DISP_CMD_CSRDIR_RIGHT	= 0X4C			;SET DIRECTION OF CURSOR MOVEMENT
.EQU	_DISP_CMD_CSRDIR_LEFT	= 0X4D
.EQU	_DISP_CMD_CSRDIR_UP	= 0X4E
.EQU	_DISP_CMD_CSRDIR_DOWN	= 0X4F
.EQU	_DISP_CMD_HDOT_SCR	= 0X5A			;SET HORIZONTAL SCROLL POSITION
.EQU	_DISP_CMD_OVLAY		= 0X5B			;SET DISPLAY OVERLAY FORMAT
.EQU	_DISP_CMD_CSRW		= 0X46			;SET CURSOR ADDRESS
.EQU	_DISP_CMD_CSRR		= 0X47			;READ CURSOR ADDRESS
.EQU	_DISP_CMD_MWRITE	= 0X42			;WRITE TO DISPLAY MEMORY
.EQU	_DISP_CMD_MREAD		= 0X43			;READ FROM DISPLAY MEMORY
.EQU	_DISP_BIT_BUSY =6				;INTERNAL BIT BUSY FLAG
;+--------------+
;| INIT DISPLAY |
;+==============+===============================================================================
_INIT_DISPLAY:
_DISP_INIT:						;THIS LINE INSERTED ONLY FOR COMPATIBILITY
	push	Acc
	push	AccH
	push	XL
	push	XH
	push	YL
	push	YH
	rcall	_DISP_R1M
	in	AccH,SREG				;save SREG status
	cli
	in	Acc,_DISP_PORT_CMD_DIR			;get actual direction of display command port
							;set display commands bits as output level one
	ori	Acc,(1<<_DISP_BIT_WR) | (1<<_DISP_BIT_RD) | (1<<_DISP_BIT_CS) | (1<<_DISP_BIT_A0) | (1<<_DISP_BIT_RESET)
							;send display command initialization
	out	_DISP_PORT_CMD_DIR,Acc
	ser	Acc
	out	_DISP_PORT_CMD_OUT,Acc			;set all bits high level
	clr	Acc					
	out	_DISP_PORT_DATA_DIR,Acc			;set display data port as input
	rcall	_DISPLAY_RESET				;reset a display
	out	SREG,AccH				;restore SREG status
;-----------------------------------------------------------------------------------------------
	ldi	Acc,_DISP_CMD_SYSTEM_SET		;initialize display device
	rcall	_DISP_CMD		
							;M0=0 Internal CG ROM
							;M1=0 CG RAM1
							;M2=0 8 Pixel Character Height
							;WS=0 Single Panel Display
							;IV=1 No Screen Top Line Correction
							;TL=0 LCD Mode
							;DR=0 No Additional Shift-Clock Cycles
	ldi	Acc,(0<<_DP_BIT_DR) | (0<<_DP_BIT_TL) | (1<<_DP_BIT_IV) | (1<<4) | (0<<_DP_BIT_WS) | (0<<_DP_BIT_M2) | (0<<_DP_BIT_M1) | (0<<_DP_BIT_M0)
	rcall	_DISP_DATA
							;WF=1 Two-Frame AC Drive
							;Pixels Character Width
	ldi	Acc,(0<<_DP_BIT_WF) | _DP_FX
	rcall	_DISP_DATA
							;Pixels Character Height
	ldi	Acc,_DP_FY
	rcall	_DISP_DATA
							;Character Per Row
	ldi	Acc,_DP_CR
	rcall	_DISP_DATA
							;Length of Line = CR+8
	ldi	Acc,_DP_TCR
	rcall	_DISP_DATA
							;Display Height
	ldi	Acc,_DP_LF				
	rcall	_DISP_DATA
							;Low byte of Horizontal Address Range of Virtual Screen
	ldi	Acc,_DP_APL
	rcall	_DISP_DATA
							;High byte of Horizontal Address Range of Virtual Screen
	ldi	Acc,_DP_APH
	rcall	_DISP_DATA
;-----------------------------------------------------------------------------------------------
	ldi	Acc,_DISP_CMD_SCROLL			;Set Display Start Address and Display Regions
	rcall	_DISP_CMD
							;Display Text Start Address
	ldi	Acc,_DP_SAD1L
	rcall	_DISP_DATA
	ldi	Acc,_DP_SAD1H
	rcall	_DISP_DATA
							;Display SAD1 length
	ldi	Acc,_DP_SL1
	rcall	_DISP_DATA
							;Display Graph Start Address
	ldi	Acc,_DP_SAD2L
	rcall	_DISP_DATA
	ldi	Acc,_DP_SAD2H
	rcall	_DISP_DATA
	ldi	Acc,_DP_SL2				;Display SAD2 length
	rcall	_DISP_DATA
							;Remainder 4 Data are dummy bytes
	ldi	Acc,_DP_SAD3L
	rcall	_DISP_DATA
	ldi	Acc,_DP_SAD3H
	rcall	_DISP_DATA
	ldi	Acc,_DP_SAD4L
	rcall	_DISP_DATA
	ldi	Acc,_DP_SAD4H
	rcall	_DISP_DATA
;-----------------------------------------------------------------------------------------------
	ldi	Acc,_DISP_CMD_CSRFORM			;Set Cursor Type
	rcall	_DISP_CMD
							;Set Cursor Width
	ldi	Acc,_DP_CRX
	rcall	_DISP_DATA
							;CM=1 Set Block Cursor Type
							;Set Cursor Height
	ldi	Acc,(1<<_DP_BIT_CM) | _DP_CRY
	rcall	_DISP_DATA
;-----------------------------------------------------------------------------------------------
	ldi	Acc,_DISP_CMD_HDOT_SCR			;Set Horizontal Scroll Positon
	rcall	_DISP_CMD
							;Set No Screen Scroll
	ldi	Acc,0x00
	rcall	_DISP_DATA
;-----------------------------------------------------------------------------------------------
	ldi	Acc,_DISP_CMD_OVLAY			;Set Display Overlay Format
	rcall	_DISP_CMD
							;MX0=0 L1 or L2 or L3
							;MX1=0
							;DM1=0 L1 and L3 are Text Mode
							;DM2=0
							;OV=0  L1 and L2 are composite screen
	ldi	Acc,(0<<_DP_BIT_OV) | (0<<_DP_BIT_DM2) | (0<<_DP_BIT_DM1) | (0<<_DP_BIT_MX1) | (0<<_DP_BIT_MX0)
	rcall	_DISP_DATA
;-----------------------------------------------------------------------------------------------
	ldi	Acc,_DISP_CMD_DISP_ON			;Turn Display On
	rcall	_DISP_CMD
							;FC0=0	Cursor Flash Rate ~2Hz
							;FC1=1
							;FP0=1	Cursor SAD1 no Flashing  
							;FP1=0
							;FP2=1  Cursor SAD2,SAD4 no Flashing
							;FP3=0
							;FP4=0	Cursor SAD3 OFF
							;FP5=0
	ldi	Acc,(0<<_DP_BIT_FP5) | (0<<_DP_BIT_FP4) | (0<<_DP_BIT_FP3) | (1<<_DP_BIT_FP2) | (0<<_DP_BIT_FP1) | (1<<_DP_BIT_FP0) | (1<<_DP_BIT_FC1) | (0<<_DP_BIT_FC0)
	rcall	_DISP_DATA
;-----------------------------------------------------------------------------------------------
	ldi	Acc,_DISP_CMD_CSRDIR_RIGHT		;Set Direction of Cursor Movement is RIGHT
	rcall	_DISP_CMD
;-----------------------------------------------------------------------------------------------
	ldi	Acc,' '					;Fill Text Screen With Space Code
	ldi	XL,_DP_SAD1L				;Text Start Address
	ldi	XH,_DP_SAD1H
	ldi	YL,low(_DP_APL*_DP_SL1/(_DP_FY+1))		;Y number of Character in Text Screen
	ldi	YH,high(_DP_APL*_DP_SL1/(_DP_FY+1))
	rcall	_DISP_FILL				;Fill Text Area
	ldi	XL,_DP_SAD2L				;Graph Start Address
	ldi	XH,_DP_SAD2H
	ldi	Acc,0					;Fill Graph Screen With 0 Code (Pixels OFF)
	ldi	YL,low(_DP_APL*_DP_SL2)			;Y Number of Bytes in Graph Screen
	ldi	YH,high(_DP_APL*_DP_SL2)
	rcall	_DISP_FILL				;Fill Graph Area
;-----------------------------------------------------------------------------------------------
	clr	Acc
	clr	AccH
	rcall	_DISP_SET_CURSOR_ADDR			;Set Cursor Start Position = 0
;-----------------------------------------------------------------------------------------------
	ldi	Acc,_DP_SLOW				;Set SLOW Speed of Display
	rcall	_DISP_SPEED
	pop	YH
	pop	YL
	pop	XH
	pop	XL
	pop	AccH
	pop	Acc
	ret
;+---------------------+
;| DIPLAY RESET	       |
;+=====================+========================================================================
_DISPLAY_RESET:
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
;| 1 millisecond delay
;+----------------------
.EQU	_DISP_RESET_TIME	= (1*_AVR_CLOCK)/6000	;calculus for 1ms
_DISP_R1M:
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
	
;+-------------------------------+
;| DIPLAY FILL VRAM AREA 	 |
;| Input: Acc Pattern    	 |
;|	  X   Start Position	 |
;|	  Y   Number of bytes	 |
;+===============================+==================================================================
_DISP_FILL:
	push	r0					;Save Useds
	push	r1
	push	Acc
	push	AccH
	push	XL
	push	XH
	push	YL
	push	YH
	mov	r1,Acc					;r1=My pattern
	ldi	Acc,_DISP_CMD_CSRW			;Set Cursor Start Position
	rcall	_DISP_CMD
	mov	Acc,XL
	mov	AccH,XH
	rcall	_DISP_SET_CURSOR_ADDR
	ldi	Acc,_DISP_CMD_MWRITE			;Set Display to Write Data
	rcall	_DISP_CMD
	clr	r0
_DISP_F00:
	cp	YL,r0					;last byte ?
	cpc	YH,r0
	breq	_DISP_F10				;yes, done
	mov	Acc,r1					;get pattern
	rcall	_DISP_DATA				;set data
	sbiw	YL,1
	rjmp	_DISP_F00				;next pattern
_DISP_F10:
	pop	YH
	pop	YL
	pop	XH					;Restore Useds
	pop	XL
	pop	AccH
	pop	Acc
	pop	r1
	pop	r0
	ret
;+--------------------------+
;| SET CURSOR ADDRESS  	    |
;| Input: Acc,AccH Address  |	
;+==========================+===================================================================
_DISP_SET_CURSOR_ADDR:
	push	Acc
	ldi	Acc,_DISP_CMD_CSRW
	rcall	_DISP_CMD
	pop	Acc
	push	Acc
	rcall	_DISP_DATA
	mov	Acc,AccH
	rcall	_DISP_DATA
	pop	Acc
	ret
;+--------------------------+
;| GET CURSOR ADDRESS  	    |
;| Output: Acc,AccH Address |	
;+==========================+===================================================================
_DISP_GET_CURSOR_ADDR:
	ldi	Acc,_DISP_CMD_CSRR
	rcall	_DISP_CMD
	rcall	_DISP_DATAR
	push	Acc
	rcall	_DISP_DATAR
	mov	AccH,Acc
	pop	Acc
	ret
;+---------------------+
;| DIPLAY STATUS READ  |
;| output: Acc status  |	
;+=====================+========================================================================
_DISP_STATUS_READ:
	cbi 	_DISP_PORT_CMD_OUT,_DISP_BIT_A0	;A0=low
_DISP_GETS:
	cbi	_DISP_PORT_CMD_OUT,_DISP_BIT_CS	;CS=low
	nop
	cbi	_DISP_PORT_CMD_OUT,_DISP_BIT_RD	;RD=low
	ldi	Acc,low(_AVR_CLOCK/3000000)
	dec	Acc
	brne	PC-1
	in	Acc,_DISP_PORT_DATA_IN		;status in Acc
	sbi	_DISP_PORT_CMD_OUT,_DISP_BIT_RD	;RD=high
	sbi	_DISP_PORT_CMD_OUT,_DISP_BIT_CS	;CS=high
	ret	
;+-------------------+
;| DIPLAY DATA READ  |
;| output: Acc data  |	
;+===================+==========================================================================
_DISP_DATA_READ:
	sbi 	_DISP_PORT_CMD_OUT,_DISP_BIT_A0	;A0=high
	rjmp	_DISP_GETS
;+----------------------+
;| DIPLAY COMMAND WRITE |
;| input: Acc data   	|	
;+======================+=======================================================================
_DISP_COMMAND_WRITE:
	out	_DISP_PORT_DATA_OUT,Acc		;set data to be write
	ser	Acc
	out	_DISP_PORT_DATA_DIR,Acc		;set DIRECTION PORT as output
	sbi	_DISP_PORT_CMD_OUT,_DISP_BIT_A0	;A0=high
_DISP_PUTD:
	cbi	_DISP_PORT_CMD_OUT,_DISP_BIT_CS	;CS=low
	nop
	cbi	_DISP_PORT_CMD_OUT,_DISP_BIT_WR	;WR=low
	ldi	Acc,low(_AVR_CLOCK/3000000)
	dec	Acc
	brne	PC-1
	sbi	_DISP_PORT_CMD_OUT,_DISP_BIT_WR	;WR=high
	sbi	_DISP_PORT_CMD_OUT,_DISP_BIT_CS	;CS=high
	clr	Acc
	out	_DISP_PORT_DATA_DIR,Acc		;set DIRECTION PORT  as input, my normal mode
	ser	Acc
	out	_DISP_PORT_DATA_OUT,Acc		;DATA PORT with pull up
	ret
;+-------------------+
;| DIPLAY DATA WRITE |
;| input: Acc data   |
;+===================+==========================================================================
_DISP_DATA_WRITE:
	out	_DISP_PORT_DATA_OUT,Acc		;set data to be write
	ser	Acc
	out	_DISP_PORT_DATA_DIR,Acc		;set DIRECTION PORT as output
	cbi	_DISP_PORT_CMD_OUT,_DISP_BIT_A0	;A0=low
	rjmp	_DISP_PUTD
;+----------------------+
;| SET DISPLAY SPEED 	|
;+======================+=======================================================================
_DISP_SPEED:
	sts	_DP_SPEED,Acc
	ret
;+----------------------+
;| READ BUSY FLAG 	|
;+======================+=======================================================================
.DSEG
_DP_SPEED:	.BYTE	1
.CSEG
_DISP_BUSY:
	lds	Acc,_DP_SPEED			;Fast speed display ?
	cpi	Acc,_DP_FAST
	brne	_DISP_BU00			;no, branch
	rcall	_DISP_STATUS_READ		;read status
	sbrs	Acc,_DISP_BIT_BUSY		;busy ? no, branch
	rjmp	_DISP_BUSY
	ret	
_DISP_BU00:
	rcall	_DISP_STATUS_READ		;read status
	sbrc	Acc,_DISP_BIT_BUSY		;busy ? no, branch
	rjmp	_DISP_BU00
	ret	
	
	
;+---------------------+
;| DIPLAY SEND COMMAND |
;| input: Acc data     |	
;+=====================+========================================================================
_DISP_CMD:
	push 	Acc			;save command to be send
	rcall	_DISP_BUSY		;verify if can write
	pop 	Acc			;restore command
	rcall	_DISP_COMMAND_WRITE	;write command
	ret
;+---------------------+
;| DIPLAY SEND ONE DATA|
;| input: Acc data     |	
;+=====================+========================================================================
_DISP_DATA:
	push 	Acc			;save data to be send
	rcall	_DISP_BUSY		;verify if can write
	pop 	Acc			;restore command
	rcall	_DISP_DATA_WRITE		;write data
	ret
;+---------------------+
;| DIPLAY READ ONE DATA|
;| Output: Acc data    |	
;+=====================+========================================================================
_DISP_DATAR:
	rcall	_DISP_BUSY		;verify if can write
	rcall	_DISP_DATA_READ		;read data
	ret
;+---------------------------------------+
;| SET ADDRESS AT (LINE,COLUNM) POSITION |
;| input: XL --> colunm 		 |
;|	  YL --> line 			 |
;+=======================================+======================================================
; warning , this routine must be change according display number colunms
_DISP_SET_LC:
	push	Acc
	push	AccH
	push	XL
	push	XH
	push	YL
	push	YH
	dec	XL					;to adjust colunm for 1..
	dec	YL					;to adjust line for 1..
	clr	YH					;always 0 in text mode
	clr	XH
	rcall	_DISP_YMUL40				;Calculate byte position
	ldi	Acc,_DP_SAD1L				;set start text address
	add	YL,Acc
	ldi	Acc,_DP_SAD1H
	adc	YH,Acc
_DISP_SET_L00:
	ldi	Acc,_DISP_CMD_CSRW			;Set Cursor Start Position
	rcall	_DISP_CMD
	mov	Acc,YL
	rcall	_DISP_DATA
	mov	Acc,YH
	rcall	_DISP_DATA
	pop	YH
	pop	YL
	pop	XH
	pop	XL
	pop	AccH
	pop	Acc
	ret	
;+-------------------
;| Perform Y*40+X
;+-------------------
_DISP_YMUL40:
	mov	Acc,YL					;get LINE number
	mov	AccH,YH
	lsl	Acc					;Acc,AccH<1
	rol	AccH
	lsl	Acc					;Acc,AccH<1
	rol	AccH
	lsl	Acc					;Acc,AccH<1
	rol	AccH
	mov	YL,Acc
	mov	YH,AccH
	lsl	Acc					;Acc,AccH<1
	rol	AccH
	lsl	Acc					;Acc,AccH<1
	rol	AccH
	add	YL,Acc
	adc	YH,AccH	
	add	YL,XL					;Y=YL*40+XL
	clr	Acc
	adc	YH,XH
	ret
	
;+-------------------------------+
;| CALC ADDRESS AT (X,Y) POSITION|
;| input: X --> X Coord	 	 |
;|	  Y --> Y Coord		 |
;| output: Acc,AccH Address	 |
;+===============================+==============================================================
_DISP_CALC_XY:
	push	XL
	push	XH
	push	YL
	push	YH
	lsr	XH					;X=INT(X/8) extract bit address field
	ror	XL					;Translate X coords to X colunm
	lsr	XH
	ror	XL
	lsr	XH
	ror	XL
	rcall	_DISP_YMUL40
	ldi	Acc,_DP_SAD2L				;Set Start Graph Address
	add	YL,Acc
	mov	Acc,YL
	ldi	AccH,_DP_SAD2H
	adc	YH,AccH
	mov	AccH,YH
	pop	YH
	pop	YL
	pop	XH
	pop	XL
	ret
	
;+-------------------------------+
;| SET ADDRESS AT (X,Y) POSITION |
;| input: X --> X Coord	 	 |
;|	  Y --> Y Coord		 |
;+===============================+==============================================================
; warning , this routine must be change according display number colunms
_DISP_SET_XY:
	push	Acc
	push	AccH
	push	XL
	push	XH
	push	YL
	push	YH
	rcall	_DISP_CALC_XY
	mov	YL,Acc
	mov	YH,AccH
	rjmp	_DISP_SET_L00				;Set Cursor Start Position
;+--------------------------------+
;| Display send string   	  |
;| input: Z --> string into FLASH |
;+--------------------------------+-----------------------------------------
_DISP_SEND_STR:
	push	r0				;save used
	push	Acc
	ldi	Acc,_DISP_CMD_MWRITE		;Set Display to Write Data
	rcall	_DISP_CMD
_DISP_SEND00:
	lpm					;get data from program memory
	mov	Acc,r0				;end string ?
	cpi	Acc,0
	breq	_DISP_SEND_DONE			;yes , done
	rcall	_DISP_DATA			;send data to display
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
	ldi	Acc,_DISP_CMD_MWRITE		;Set Display to Write Data
	rcall	_DISP_CMD
_DISP_SENDM00:
	ld	Acc,Z+				;get data from program memory
	cpi	Acc,0				;end string ?
	breq	_DISP_SENDM_DONE			;yes , done
	rcall	_DISP_DATA			;send data to display
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
;| CLEAR SCREEN TEXT  			 |
;| input: Acc character to fill screen   |
;+=======================================+======================================================
_DISP_CLS_TEXT:
	push	Acc
	push	XL
	push	XH
	push	YL
	push	YH
	ldi	XL,_DP_SAD1L				;Text Start Address
	ldi	XH,_DP_SAD1H
	ldi	YL,low(_DP_APL*_DP_SL1/(_DP_FY+1))		;Y number of Character in Text Screen
	ldi	YH,high(_DP_APL*_DP_SL1/(_DP_FY+1))
	rcall	_DISP_FILL				;Fill Text Area
	pop	YH
	pop	YL
	pop	XH
	pop	XL
	pop	Acc
	ret
;+---------------------------------------+
;| CLEAR SCREEN GRAPH  			 |
;+=======================================+======================================================
_DISP_CLS_GRAPH:
	push	Acc
	push	XL
	push	XH
	push	YL
	push	YH
	ldi	XL,_DP_SAD2L				;Graph Start Address
	ldi	XH,_DP_SAD2H
	ldi	Acc,0					;Fill Graph Screen With 0 Code (Pixels OFF)
	ldi	YL,low(_DP_APL*_DP_SL2)			;Y Number of Bytes in Graph Screen
	ldi	YH,high(_DP_APL*_DP_SL2)
	rcall	_DISP_FILL				;Fill Graph Area
	pop	YH
	pop	YL
	pop	XH
	pop	XL
	pop	Acc
	ret	
;+---------------------------------------+
;| Put pixel	 			 |
;| input: X = X coordinate	 	 |
;|	  Y = Y coordinate		 |
;|	  Acc 0 reset 0xff set pixel	 |
;+=======================================+=========================================================
;this routine not optimized
_DISP_PSET:
	push	AccH
	push	AccT
	push	AccTH
	push	Acc
	tst	XH					;X<0 ?
	brmi	_DISP_PSET_EXIT				;yes, exit
	tst	YH					;Y<0 ?
	brmi	_DISP_PSET_EXIT				;yes, exit
	ldi	Acc,low(_DISP_SCALE_WIDTH)		;X>=DISPLAY_WIDTH ?
	cp	XL,Acc
	ldi	Acc,high(_DISP_SCALE_WIDTH)
	cpc	XH,Acc
	brsh	_DISP_PSET_EXIT				;yes, exit
	ldi	Acc,low(_DISP_SCALE_HEIGHT)		;Y>=DISPLAY_HEIGHT ?
	cp	YL,Acc
	ldi	Acc,high(_DISP_SCALE_HEIGHT)
	cpc	YH,Acc
	brsh	_DISP_PSET_EXIT				;yes, exit
	rcall	_DISP_CALC_XY				;calculate byte pixel insertion
	mov	AccT,Acc
	mov	AccTH,AccH
	rcall	_DISP_SET_CURSOR_ADDR
	ldi	AccH,0b10000000				;AccH=MASK bits
	mov	Acc,XL					;Extract bit field
_DISP_PS00:
	andi	Acc,7					;BIT 0 ?
	breq	_DISP_PS10				;no, branch
	dec	Acc					;bit count --
	lsr	AccH					;next bit mask
	rjmp	_DISP_PS00
_DISP_PS10:
	ldi	Acc,_DISP_CMD_MREAD			;Set Display to Read Data
	rcall	_DISP_CMD
	rcall	_DISP_DATAR				;read byte ad coordinates X,Y
	push	Acc					;save read data
	push	AccH
	mov	Acc,AccT
	mov	AccH,AccTH
	rcall	_DISP_SET_CURSOR_ADDR			;set address to be write again
	ldi	Acc,_DISP_CMD_MWRITE
	rcall	_DISP_CMD
	pop	AccH
	pop	AccT					;restore old data
	pop	Acc					;get pixel to be set
	push	Acc
	tst	Acc					;pixel set
	brne	_DISP_PS20				;branch if yes
	com	AccH					;else complement mask bit
	and	AccT,AccH				;clear pixel
	rjmp	_DISP_PS30
_DISP_PS20:
	or	AccT,AccH				;set pixel
_DISP_PS30:
	mov	Acc,AccT
	rcall	_DISP_DATA				;end pixel function
_DISP_PSET_EXIT:
	pop	Acc
	pop	AccTH
	pop	AccT
	pop	AccH
	ret
;+---------------------------------------+
;| Get pixel	 			 |
;| input: X = X coordinate	 	 |
;|	  Y = Y coordinate		 |
;| Output:				 |
;|	  Acc 0 reset 0xff pixel set	 |
;+=======================================+=========================================================
_DISP_POINT:
	push	AccH
	push	AccT
	push	AccTH
	tst	XH					;X<0 ?
	brmi	_DISP_POINT_EXIT				;yes, exit
	tst	YH					;Y<0 ?
	brmi	_DISP_POINT_EXIT				;yes, exit
	ldi	Acc,low(_DISP_WIDTH)			;X>=DISPLAY_WIDTH ?
	cp	XL,Acc
	ldi	Acc,high(_DISP_WIDTH)
	cpc	XH,Acc
	brsh	_DISP_POINT_EXIT				;yes, exit
	ldi	Acc,low(_DISP_HEIGHT)			;Y>=DISPLAY_HEIGHT ?
	cp	YL,Acc
	ldi	Acc,high(_DISP_HEIGHT)
	cpc	YH,Acc
	brsh	_DISP_POINT_EXIT				;yes, exit
	rcall	_DISP_CALC_XY				;calculate byte pixel insertion
	mov	AccT,Acc
	mov	AccTH,AccH
	rcall	_DISP_SET_CURSOR_ADDR
	ldi	AccH,0b10000000				;AccH=MASK bits
	mov	Acc,XL					;Extract bit field
_DISP_PT00:
	andi	Acc,7					;BIT 0 ?
	breq	_DISP_PT10				;no, branch
	dec	Acc					;bit count --
	lsr	AccH					;next bit mask
	rjmp	_DISP_PT00
_DISP_PT10:
	ldi	Acc,_DISP_CMD_MREAD			;Set Display to Read Data
	rcall	_DISP_CMD
	rcall	_DISP_DATAR				;read byte ad coordinates X,Y
	and	Acc,AccH				;bit = 0
	breq	_DISP_POINT_EXIT				;yes, branch
	ser	Acc					;else set acc= 0xff
_DISP_POINT_EXIT:
	pop	AccTH
	pop	AccT
	pop	AccH
	ret
;+-------------------------------+
;| GRAPH POKE		 	 |
;| Input: Acc Pattern    	 |
;|	  X   Start Position	 |
;+===============================+==================================================================
_DISP_GRAPH_POKE:
	push	Acc
	push	AccH
	push	AccT
	ldi	AccT,_DP_SAD2L				;Acc:AccH->graph area
	mov	Acc,XL
	add	Acc,AccT
	ldi	AccT,_DP_SAD2H
	mov	AccH,XH
	adc	AccH,AccT
	rcall	_DISP_SET_CURSOR_ADDR			;set position at VRAM GRAPH AREA
	ldi	Acc,_DISP_CMD_MWRITE			;Set Display to Write Data
	rcall	_DISP_CMD
	pop	AccT
	pop	AccH
	pop	Acc
	rcall	_DISP_DATA
	ret

