; /---------------------------------\
; | Macros for Mega16Timed.asm      |
; |                                 |
; |                                 |
; |                                 |
; |---------------------------------|
; | John Haskins                    |
; \---------------------------------/

.DEF tmp1=R19				; public use (push & pop)	SHARED
.DEF tmp0=R20				; public use (push & pop)	SHARED
.DEF WL=R24					; public use				RESERVED
.DEF WH=R25					; public use				RESERVED

;-------------------------------------------------------------
; CONSTANTS
;-------------------------------------------------------------

.EQU LINESFORPAINT=242			; 
.EQU LINESFORVSYNC=11			; 
.EQU MINROW=36					; 
.EQU MAXROW=200					; 
.EQU LINESIZE=12				; bytes per line
.EQU LINESPIX=2					; lines to be drawn per pixel row
.EQU MEMBASE=$0060				; base memory address

.EQU INBLANK=0					; flag bit
.EQU INPAINT=1					; flag bit
.EQU INVSYNC1=2					; flag bit
.EQU INVSYNC2=3					; flag bit

.SET SYNC= 0b00000000			; Sync, both off
.SET BLACK=0b00000001			; Black, low on
.SET GREY= 0b00000100			; Grey, high on
.SET WHITE=0b00000101			; White, low & high on
.SET VIDPORT=PORTB				; port rca jack is on
.SET WHITEBIT=2					; bit that indicates white
.SET BLACKBIT=0					; bit that indicates black

.SET COLUMNS=LINESIZE*8
.SET ROWS=(MAXROW-MINROW)/LINESPIX

;-------------------------------------------------------------

.MACRO nop2					; 2 - 2 cycle nop
	nop						; 1
	nop						; 1
.ENDMACRO

;-------------------------------------------------------------

.MACRO nop5					; 5 - 5 cycles of nop
	nop						; 1
	nop						; 1
	nop						; 1
	nop						; 1
	nop						; 1
.ENDMACRO

;-------------------------------------------------------------

.MACRO SetupStack			; 4 - setup Stack pointer
	ldi  tmp0,high(RAMEND)	; 1 stack pointer value
	out  SPH,tmp0			; 1 setup stack pointer
	ldi  tmp0,low(RAMEND)	; 1 stack pointer value
	out  SPL,tmp0			; 1 setup stack pointer
.ENDMACRO

;-------------------------------------------------------------

.MACRO SetupTVState				; 3 - Set TV Params
	ldi  vidflag,(1<<INPAINT)	; 1
	ldi  linectr,LINESFORPAINT	; 1
.ENDMACRO

;-------------------------------------------------------------
; X Pointer for application, use at your discression
.MACRO SetupXPointer		; 2 - setup X Pointer
	ldi  XH,HIGH(MEMBASE)	; 1 set high portion of X Pointer
	ldi  XL,LOW(MEMBASE)	; 1 set low portion of X Pointer
.ENDMACRO

.MACRO ResetX				; 2 - reset X Pinter to begining
	ldi  XH,HIGH(MEMBASE)	; 1 set high portion of X Pointer
	ldi  XL,LOW(MEMBASE)	; 1 set low portion of X Pointer
.ENDMACRO

;-------------------------------------------------------------
; Y Pointer for SYSTEM ! do not user in application !
.MACRO SetupYPointer		; 2 - setup Y Pointer
	ldi  YH,HIGH(MEMBASE)	; 1 set high portion of Y Pointer
	ldi  YL,LOW(MEMBASE)	; 1 set low portion of Y Pointer
.ENDMACRO

.MACRO ResetY				; 2 - reset Y Pinter to begining
	ldi  YH,HIGH(MEMBASE)	; 1 set high portion of Y Pointer
	ldi  YL,LOW(MEMBASE)	; 1 set low portion of Y Pointer
.ENDMACRO

;-------------------------------------------------------------

.MACRO SetupTimer0			; 12 - setup Timer0
	push tmp0				; 2
	ldi  tmp0,(1<<CS00)		; 1 
	out  TCCR0,tmp0			; 1 enable timer @ ck (no prescale)
	ldi  tmp0,(1<<TOV0)|(1<<OCF0)	; 1 
	out  TIFR,tmp0			; 1 clear pending
	ldi  tmp0,(1<<TOIE0)|(1<<OCIE0)	; 1 
	out  TIMSK,tmp0			; 1 enable timer0 overflow interrupt
	ldi  tmp0,0b00111111	; 1
	out  OCR0,tmp0			; 1 inital compare value
	pop  tmp0				; 2
.ENDMACRO

;-------------------------------------------------------------

.MACRO FillImage			; 135 Fill Image Data
	; @0 = byte to fill with
	push tmp0
	push tmp1
	ldi  XH,HIGH(MEMBASE)	; 1 set high portion of X Pointer
	ldi  XL,LOW(MEMBASE)	; 1 set low portion of X Pointer
	ldi  tmp1,ROWS			; 1
	ldi  tmp0,@0

fillLine:
	st   X+,tmp0					; 2 ind 0
	st   X+,tmp0					; 2 ind 1
	st   X+,tmp0					; 2 ind 2
	st   X+,tmp0					; 2 ind 3
	st   X+,tmp0					; 2 ind 4
	st   X+,tmp0					; 2 ind 5
	st   X+,tmp0					; 2 ind 6
	st   X+,tmp0					; 2 ind 7
	st   X+,tmp0					; 2 ind 8
	st   X+,tmp0					; 2 ind 9
	st   X+,tmp0					; 2 ind 10
	st   X+,tmp0					; 2 ind 11
	dec tmp1
	brne fillLine

	pop  tmp1
	pop  tmp0
.ENDMACRO

;-------------------------------------------------------------

.MACRO SPXI					; 15 Set PiXel Immediant
	; @0 = x  0 <= x < 96
	; @1 = y  0 <= y < 82
	push tmp0				; 2 store initial value
	push tmp1				; 2 store initial value
	ldi  XH,HIGH(MEMBASE)	; 1 set high portion of X Pointer
	ldi  XL,LOW(MEMBASE)	; 1 set low portion of X Pointer
	ldi  tmp1,high(@1*LINESIZE)	; 1 
	ldi  tmp0,low(@1*LINESIZE)	; 1 
	add  XL,tmp0			; 1 
	adc  XH,tmp1			; 1 
	adiw XH:XL,(@0/8)		; 2 get byte
	ld   tmp0,X				; 2 load
	sbr  tmp0,(1<<(@0&$07))	; 1 change
	st   X,tmp0				; 2 store
	pop  tmp1				; 2 restore initial value
	pop  tmp0				; 2 restore initial value
.ENDMACRO

;-------------------------------------------------------------

.MACRO CPXI		 			; 15 Clear PiXel Immediant
	; @0 = x  0 <= x <= 96
	; @1 = y  0 <= y <= 82
	push tmp0				; 2 store initial value
	push tmp1				; 2 store initial value
	ldi  XH,HIGH(MEMBASE)	; 1 set high portion of X Pointer
	ldi  XL,LOW(MEMBASE)	; 1 set low portion of X Pointer
	ldi  tmp1,high(@1*LINESIZE)	; 1 
	ldi  tmp0,low(@1*LINESIZE)	; 1 
	add  XL,tmp0			; 1
	adc  XH,tmp1			; 1
	adiw XH:XL,(@0/8)		; 2 get byte X/8
	ld   tmp0,X				; 2 load
	cbr  tmp0,(1<<(@0&$7))	; 1 change bit
	st   X,tmp0				; 2 store	
	pop  tmp1				; 2 restore initial value
	pop  tmp0				; 2 restore initial value
.ENDMACRO

;-------------------------------------------------------------

.MACRO TPXI		 			; 15 Toggle PiXel Immediant
	; @0 = x  0 <= x <= 96
	; @1 = y  0 <= y <= 82
	push tmp0				; 2 store initial value
	push tmp1				; 2 store initial value
	ldi  XH,HIGH(MEMBASE)	; 1 set high portion of X Pointer
	ldi  XL,LOW(MEMBASE)	; 1 set low portion of X Pointer
	ldi  tmp1,high(@1*LINESIZE)	; 1 
	ldi  tmp0,low(@1*LINESIZE)	; 1 
	add  XL,tmp0			; 1
	adc  XH,tmp1			; 1
	adiw XH:XL,(@0/8)		; 2 get byte X/8
	ld   tmp0,X				; 2 load
	sbrs tmp0,(@0&$7)	; 1 skip if set
	rjmp sit
cit:cbr  tmp0,(1<<(@0&$7))	; 1 clear bit
	rjmp dit
sit:sbr  tmp0,(1<<(@0&$7))	; 1 set bit
dit:st   X,tmp0				; 2 store
	pop  tmp1				; 2 restore initial value
	pop  tmp0				; 2 restore initial value
.ENDMACRO

;-------------------------------------------------------------

.MACRO SPX					; 15 Set PiXel
	; @0 = x register  0 <= x < 96
	; @1 = y register  0 <= y < 82
	push tmp0				; 2 store initial value
	push tmp1				; 2 store initial value
	ldi  XH,HIGH(MEMBASE)	; 1 set high portion of X Pointer
	ldi  XL,LOW(MEMBASE)	; 1 set low portion of X Pointer
	ldi  tmp1,LINESIZE		; 1 bytes per line (13)
	mov  tmp0,@1			; 1 get register y value
	mul  tmp0,tmp1			; 2 line number * bytes per line
	mov  tmp1,r1			; 1 move back to tmp1
	mov  tmp0,r0			; 1 move back to tmp0
	add  XL,tmp0			; 1	
	adc  XH,tmp1			; 1	
	mov  tmp0,@0			; 1 get register x value
	lsr  tmp0				; 1 shift right
	lsr  tmp0				; 1 shift right
	lsr  tmp0				; 1 shift right net result @0/8
	add  XL,tmp0			; 1
	ld   tmp0,X				; 2 load
	mov  ZL,@0				; 1
	andi ZL,$07			; 1 just last bits
	subi ZL, -(low(BitMaskTable*2)) 
	ldi  ZH,0x01
	sbci ZH, -(high(BitMaskTable*2)) 
	lpm  ZL,Z
	or   tmp0,ZL
	st   X,tmp0				; 2 store	
	pop  tmp1				; 2 restore initial value
	pop  tmp0				; 2 restore initial value
.ENDMACRO

;-------------------------------------------------------------

.MACRO CPX					; 15 Clear PiXel
	; @0 = x register  0 <= x < 96
	; @1 = y register  0 <= y < 82
	push tmp0				; 2 store initial value
	push tmp1				; 2 store initial value
	ldi  XH,HIGH(MEMBASE)	; 1 set high portion of X Pointer
	ldi  XL,LOW(MEMBASE)	; 1 set low portion of X Pointer
	ldi  tmp1,LINESIZE		; 1 bytes per line (13)
	mov  tmp0,@1			; 1 get register y value
	mul  tmp0,tmp1			; 2 line number * bytes per line
	mov  tmp1,r1			; 1 move back to tmp1
	mov  tmp0,r0			; 1 move back to tmp0
	add  XL,tmp0			; 1	
	adc  XH,tmp1			; 1	
	mov  tmp0,@0			; 1 get register x value
	lsr  tmp0				; 1 shift right
	lsr  tmp0				; 1 shift right
	lsr  tmp0				; 1 shift right net result @0/8
	add  XL,tmp0			; 1
	ld   tmp0,X				; 2 load
	mov  ZL,@0				; 1
	andi ZL,$07			; 1 just last bits
	subi ZL, -(low(BitMaskTable*2)) 
	ldi  ZH,0x01
	sbci ZH, -(high(BitMaskTable*2)) 
	lpm  ZL,Z
	com  ZL
	and  tmp0,ZL
	st   X,tmp0				; 2 store	
	pop  tmp1				; 2 restore initial value
	pop  tmp0				; 2 restore initial value
.ENDMACRO

;-------------------------------------------------------------

.MACRO TPX					; 15 Toggle PiXel
	; @0 = x register  0 <= x < 96
	; @1 = y register  0 <= y < 82
	push tmp0				; 2 store initial value
	push tmp1				; 2 store initial value
	ldi  XH,HIGH(MEMBASE)	; 1 set high portion of X Pointer
	ldi  XL,LOW(MEMBASE)	; 1 set low portion of X Pointer
	ldi  tmp1,LINESIZE		; 1 bytes per line (13)
	mov  tmp0,@1			; 1 get register y value
	mul  tmp0,tmp1			; 2 line number * bytes per line
	mov  tmp1,r1			; 1 move back to tmp1
	mov  tmp0,r0			; 1 move back to tmp0
	add  XL,tmp0			; 1	
	adc  XH,tmp1			; 1	
	mov  tmp0,@0			; 1 get register x value
	lsr  tmp0				; 1 shift right
	lsr  tmp0				; 1 shift right
	lsr  tmp0				; 1 shift right net result @0/8
	add  XL,tmp0			; 1
	ld   tmp0,X				; 2 load
	mov  ZL,@0				; 1
	andi ZL,$07				; 1 just last 3 bits
	subi ZL, -(low(BitMaskTable*2)) 
	ldi  ZH,0x01
	sbci ZH, -(high(BitMaskTable*2)) 
	lpm  ZL,Z
	
	com  ZL
	mov  tmp1,tmp0
	and  tmp1,ZL
	cpse tmp1,tmp0			; 2:1 if was 0 skip
	rjmp to0				; 0:2 set 0
	rjmp to1				; 2:0 set 1
to0:and  tmp0,ZL
	rjmp ts
to1:com  ZL
	or   tmp0,ZL

ts: st   X,tmp0				; 2 store	
	pop  tmp1				; 2 restore initial value
	pop  tmp0				; 2 restore initial value
.ENDMACRO

;-------------------------------------------------------------

.MACRO LDBMI				; Load BitMap Immediant
	; @0 = bitmap lable
	; @1 = x byte 0 <= x < 12 (byte number)
	; @2 = y      0 <= y < 82
	push tmp0
	push tmp1
;	push WL
;	push WH

	; setup X Pointer to correct byte for first char
	ldi  XH,HIGH(MEMBASE)	; 1 set high portion of X Pointer
	ldi  XL,LOW(MEMBASE)	; 1 set low portion of X Pointer
	ldi  tmp1,high(@2*LINESIZE)	; 1 
	ldi  tmp0,low(@2*LINESIZE)	; 1 
	add  XL,tmp0			; 1
	adc  XH,tmp1			; 1
	adiw XH:XL,(@1)			; 2 get byte X
	; setup EEPROM Read to first byte of char
	ldi  WH,high(@0)
	ldi  WL, low(@0)
	out  EEARH,WH
	out  EEARL,WL

	ldi  tmp0,8				; ctr for bytes

readLoop:
	sbi  EECR,EERE
_checkRead:
	in   tmp1,EECR
	andi tmp1,EERE
	tst  tmp1
	brne _checkRead
	in   tmp1,EEDR
	; now tmp1 has byte from table
	
	st   X,tmp1				; store the byte of pixels
	sbiw XH:XL,LINESIZE		; move to next line

	adiw WH:WL,1
	out  EEARL,WL			; move to next byte of char
	out  EEARH,WH			; carry if needed

	dec  tmp0
	brne readLoop
	
;	pop  WH
;	pop  WL
	pop  tmp1
	pop  tmp0
.ENDMACRO

;-------------------------------------------------------------

;-------------------------------------------------------------
; Used in TV Output Machine
;-------------------------------------------------------------

.MACRO ClearTimer0			; 8 - Clear Timer0
	push tmp0				; 2
	clr  tmp0				; 1 
	out  TCNT0,tmp0			; 1 
	ldi  tmp0,(1<<TOV0)|(1<<OCF0)	; 1 
	out  TIFR,tmp0			; 1 clear pending
	pop  tmp0				; 2
.ENDMACRO

;-------------------------------------------------------------

.MACRO LDPXLL				; 43 LoaD Pixel Line into R0-7 row in register
	; @0 = line value in register  MINROW(36) <= line <= MAXROW(204)
	push tmp0				; 2
	push tmp1				; 2
	ldi  YH,HIGH(MEMBASE)	; 1 set high portion of Y Pointer
	ldi  YL,LOW(MEMBASE)	; 1 set low portion of Y Pointer
	mov  tmp0,@0			; 1
	subi tmp0,MINROW		; 1
	lsr  tmp0				; 1
	ldi  tmp1,LINESIZE		; 1-
	mul  tmp1,tmp0			; 1
	mov  tmp1,r1			; 1
	mov  tmp0,r0			; 1
	add  YL,tmp0			; 1
	adc  YH,tmp1			; 1
	ld   R0,Y+				; 2
	ld   R1,Y+				; 2-
	ld   R2,Y+				; 2-
	ld   R3,Y+				; 2
	ld   R4,Y+				; 2
	ld   R5,Y+				; 2
	ld   R6,Y+				; 2-
	ld   R7,Y+				; 2-
	ld   R8,Y+				; 2
	ld   R9,Y+				; 2
	ld   R10,Y+				; 2
	ld   R11,Y+				; 2-
	pop  tmp1				; 2-
	pop  tmp0				; 2
.ENDMACRO

;-------------------------------------------------------------

.MACRO WriteByte			; 24 - write byte out to screen
	bst  @0,0						; 1
	bld  pixel,WHITEBIT				; l
	out  VIDPORT,pixel				; 1 write out byte

	bst  @0,1						; 1
	bld  pixel,WHITEBIT				; 1
	out  VIDPORT,pixel				; 1 write out byte

	bst  @0,2						; 1
	bld  pixel,WHITEBIT				; 1
	out  VIDPORT,pixel				; 1 write out byte

	bst  @0,3						; 1
	bld  pixel,WHITEBIT				; 1
	out  VIDPORT,pixel				; 1 write out byte

	bst  @0,4						; 1
	bld  pixel,WHITEBIT				; 1
	out  VIDPORT,pixel				; 1 write out byte

	bst  @0,5						; 1
	bld  pixel,WHITEBIT				; 1
	out  VIDPORT,pixel				; 1 write out byte

	bst  @0,6						; 1
	bld  pixel,WHITEBIT				; 1
	out  VIDPORT,pixel				; 1 write out byte

	bst  @0,7						; 1
	bld  pixel,WHITEBIT				; 1
	out  VIDPORT,pixel				; 1 write out byte
.ENDMACRO

;-------------------------------------------------------------

