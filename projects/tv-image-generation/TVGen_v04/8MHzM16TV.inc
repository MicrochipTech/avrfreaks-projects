; /---------------------------------\
; | Draw on a TV, NTSC timing       |
; |                                 |
; | Runs at 8MHz on ATMega16 AVR    |
; |                                 |
; |---------------------------------|
; | John Haskins                    |
; \---------------------------------/

.DEF pixel=R18					; pixel value,				SHARED
.DEF temp2=R19					; temp value,				SHARED
.DEF temp3=R20					; temp value,				SHARED
.DEF linectr=R21				; current line,				RESERVED
.DEF vidflag=R22				; flag store				RESERVED

;-------------------------------------------------------------

.CSEG							; code segment at next available location

ISR_TOV0:
	reti						; 

;-------------------------------------------------------------

ISR_COMP0:
	push pixel					; 2 store initial register
	in   pixel,SREG				; 1 get SREG
	push pixel					; 2 store SREG
	push temp2					; 2
	push temp3					; 2

	sbrc  vidflag,INPAINT		; 2:1
	rjmp HandlePaint			; 0:3+

	sbrc  vidflag,INVSYNC1		; 2:1
	rjmp HandleVSync1			; 0:3+
	
	sbrc  vidflag,INVSYNC2		; 2:1
	rjmp HandleVSync2			; 0:3+


ISR_COM0_DONE:
	ClearTimer0					; 4

	pop  temp3					; 2
	pop  temp2					; 2
	pop  pixel					; 2 retrieve SREG
	out  SREG,pixel				; 1 set SREG
	pop  pixel					; 2 restore initial register
	reti						; 4

;-------------------------------------------------------------
;
; Handle Methods,
;  Each handles a distinct state in the signal
;  Responsible for changing to next state and
;   setting OCR0 for time in current state
;  Lead in  21 CK 						including nop2s
;  Lead out 29 CK (16 to clock reset)	including ret
;  MAX Length 230 CK (without clearing timer internally)
;  MIN OCR0 16 for it to catch
;
;-------------------------------------------------------------

HandlePaint:
	;------
HSync:						; h sync pulse 4.7us=37.6ck
	ldi  pixel,SYNC				; 1 sync level
	out  VIDPORT,pixel			; 1 write out byte
	; wait 36
	rcall WasteTen				; 10
	rcall WasteTen				; 10
	rcall WasteSeven			; 7
	rcall WasteSeven			; 7
	nop							; 1
	nop							; 1
BackPorch:					; setup in 5.9us=47.2ck
	ldi  pixel,BLACK			; 1 sync level
	out  VIDPORT,pixel			; 1 write out byte
	;wait 23
	rcall WasteTen				; 10
	rcall WasteTen				; 10
	nop							; 1

	cpi  linectr,MINROW			; 1
	brlo OutLine1				; 2:1
	cpi  linectr,MAXROW			; 1 
	brsh OutLine				; 2:1
	rjmp InLine					; 2

OutLine1:
	nop2						; 2
OutLine:
	nop2						; 2 sync with others
	rcall WasteTen				; 10
	rcall WasteTen				; 10
	rcall WasteTen				; 10
	rcall WasteTen				; 10
	rcall WasteSeven			; 7
	rcall WasteSeven			; 7

	ldi  pixel,BLACK			; 1
	mov  temp3,linectr			; 1
	lsr  temp3					; 1
	lsr  temp3					; 1
	cpi  temp3,(MAXROW-0)/4		; 1
	brne t2						; 1:2 *
t1:	ldi  pixel,GREY				; 1:0
t2:	cpi  temp3,(MINROW-1)/4		; 1
	brne t4						; 1:2 *
t3:	ldi  pixel,GREY				; 1:0
t4: nop							; 1
	
	; write actuall pixels 334
	nop2						; 2
	out  VIDPORT,pixel			; 1
	; 48 wait
	rcall WasteTen				; 10
	rcall WasteTen				; 10
	rcall WasteTen				; 10
	rcall WasteTen				; 10
	rcall WasteSeven			; 7
	nop 						; 1

	cpi  pixel,BLACK			; 1
	brne itsGrey				; 2:1
	; it's black, just set timer and get it go
	rcall WasteFifty			; 50
	rcall WasteTen				; 10

	dec  linectr				; 1 line counter --				
	breq gotoVSync				; 1:2 if 0 then start v sync

	ldi  vidflag,(1<<INPAINT)	; 1 change state
	ldi  temp2,250				; 1 time 'till Front Porch starts
	out  OCR0,temp2				; 1 set compare
	rjmp ISR_COM0_DONE			; 2
gotoVSync:						; skips if not last line
	ldi  vidflag,(1<<INVSYNC2)	; 1 change state
	ldi  linectr,LINESFORVSYNC	; 1 
	ldi  temp2,31				; 1 time 'till Front Porch starts
	out  OCR0,temp2				; 1 set compare
	;------
	rjmp ISR_COM0_DONE			; 2
	
itsGrey:
	rcall WasteHundred			; 100
	rcall WasteHundred			; 100
	rcall WasteTen				; 10
	rcall WasteTen				; 10
	rcall WasteTen				; 10
	rcall WasteTen				; 10
	nop2						; 2
	rjmp LineDone				; 2

InLine:
	rcall WasteSeven			; 7
	nop2						; 2
	;nop  						; 1
	LDPXLL linectr				; 43

	rcall WasteSeven			; 7
	rcall WasteSeven			; 7
	; write actuall pixels 334
	ldi  pixel,GREY				; 1
	out  VIDPORT,pixel			; 1
	nop2						; 2
	ldi  pixel,BLACK			; 1
	WriteByte R0				; 24
	WriteByte R1				; 24 
	WriteByte R2				; 24 
	WriteByte R3				; 24 
	WriteByte R4				; 24 
	WriteByte R5				; 24 
	WriteByte R6				; 24
	WriteByte R7				; 24
	WriteByte R8				; 24
	WriteByte R9				; 24
	WriteByte R10				; 24
	WriteByte R11				; 24
	nop2						; 2
	ldi  pixel,GREY				; 1
	out  VIDPORT,pixel			; 1 write out byte

LineDone:
	nop5						; 5
	;FrontPorch:				; setup/cleanum in 1.4us=11.2ck
	ldi  pixel,BLACK			; 1 sync level
	out  VIDPORT,pixel			; 1 write out byte
	;wait 68
	rcall WasteTen				; 10
	rcall WasteTen				; 10
	rcall WasteTen				; 10
	rcall WasteTen				; 10
	rcall WasteTen				; 10
	rcall WasteSeven			; 7

	dec  linectr				; 1 line counter --				
;	brne keepPainting			; 2 if last line then start vsync
	rcall WasteTen				; 10
	rcall WasteTen				; 10
	rcall WasteTen				; 10
	rjmp HandlePaint			; 2
	
;-------------------------------------------------------------

HandleVSync1:
	nop2						; 2 sync with others
	;------
	ldi  temp2,BLACK			; 1 sync level
	out  VIDPORT,temp2			; 1 write out byte
	; wait 36
	rcall WasteTen				; 10
	rcall WasteTen				; 10
	rcall WasteSeven			; 7
	rcall WasteSeven			; 7
	nop							; 1
	nop							; 1
	ldi  pixel,SYNC				; 1
	out  VIDPORT,pixel			; 1
	
	ldi  vidflag,(1<<INVSYNC2)	; 1 change state
	ldi  temp2,250				; time 'till VSync2 starts
	out  OCR0,temp2				; set compare
	;------
	rjmp ISR_COM0_DONE			; 2

;-------------------------------------------------------------

HandleVSync2:
	;------
	ldi  pixel,SYNC				; 1
	out  VIDPORT,pixel			; 1
	nop							; 1
	dec  linectr				; 1 line counter --				
	brne keepVSync				; 2 if last line then start vsync
gotoPaint:						; skips if not last line
	ldi  vidflag,(1<<INPAINT)	; 1 change state
	ldi  linectr,LINESFORPAINT	; 1 
	ldi  temp2,169				; 1 time 'till VSyncH starts
	out  OCR0,temp2				; 1 set compare
	;------
	rjmp ISR_COM0_DONE			; 2
keepVSync:						; if not last line ends up here
	ldi  vidflag,(1<<INVSYNC1)	; 1 change state
	ldi  temp2,164				; 1 time 'till VSyncH starts
	out  OCR0,temp2				; 1 set compare
	;------
	rjmp ISR_COM0_DONE			; 2


BitMaskTable:	.DB 1,2,4,8,16,32,64,128

;-------------------------------------------------------------

.DSEG
.ORG $0060
ImageSramData:	.BYTE (MAXROW-MINROW)/LINESPIX*LINESIZE
