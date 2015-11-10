;+------------------------------------------------------+
;| DISPLAY DRIVE FOR HD44780 CHIP DA HITACHI		|
;| 4 BITS INTERFACE					|
;| NEED FILE TO WORK "GLOBDEFS.INC" 			|
;| BY JOÃO DARTAGNAN ANTUNES OLIVEIRA			|
;| DATE:12/12/2000 VERSION 1.0				|
;>------------------------------------------------------<
;| C O M M O N  F U N C T I O N S			|
;+------------------------------------------------------+
;| THIS DRIVE HAS INTEND TO USE WITH AVRS WITH SRAM	|
;+------------------------------------------------------+

;+----------------------------+
;| Display Delay mileseconds  |
;+----------------------------+-------------------------------------------------------------------------------

.EQU	_DISP_DEL_1MS	= (_AVR_CLOCK*1)/6000		;for 1ms delay

_DISP_DELAY:
	push	r0
	push	Acc
	push	AccH
	clr	r0
_DISP_DE00:
	tst	Acc					;last milesecond ?
	breq	_DISP_DE20				;yes, done
	push	Acc
	ldi	Acc,low(_DISP_DEL_1MS)
	ldi	AccH,high(_DISP_DEL_1MS)
_DISP_DE10:
	subi	Acc,1
	sbci	AccH,0
	cp	Acc,r0
	cpc	AccH,r0
	brne	_DISP_DE10
	pop	Acc
	dec	Acc					;next milesecond delay
	rjmp	_DISP_DE00
_DISP_DE20:
	pop	AccH
	pop	Acc
	pop	r0
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
	push	Acc				;save used
	push	AccH
	dec	Acc				;adjust system coordinates 0..n --> 1..n
	dec	AccH
	ror	Acc				;transfer line bit to bit 6
	ror	Acc
	ror	Acc
	andi	Acc,0b01000000			;mask line bit
	ori	Acc,0b10000000			;add locate command
	andi	AccH,0b00111111			;mask columns
	add	Acc,AccH			;composite locate command
	rcall	_DISP_CMD			;send data
	pop	AccH				;pop used
	pop	Acc
	ret

;+------------------------+
;| Display Clear Screen	  |
;+------------------------+-----------------------------------------------------------------------------------
_DISP_CLS:
	push	Acc
	ldi	Acc,_DISP_CMD_CLEAR		;Set Clear Command
	rcall	_DISP_CMD
	pop	Acc
	ret

;+------------------------+
;| Display Home Address   |
;+------------------------+-----------------------------------------------------------------------------------
_DISP_HOME:
	push	Acc
	ldi	Acc,_DISP_CMD_HOME		;Set Home Command
	rcall	_DISP_CMD
	pop	Acc
	ret


