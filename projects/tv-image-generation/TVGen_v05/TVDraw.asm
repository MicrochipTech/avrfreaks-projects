; /---------------------------------\
; | Draw on a TV, NTSC timing       |
; |                                 |
; | Runs at 8MHz on the ATMega16    |
; |                                 |
; |---------------------------------|
; | John Haskins                    |
; \---------------------------------/

.INCLUDE "m16def.inc"

.include "8MHzM16TVMacs.inc"			; the MARCROs that help the application draw stuff to the tv, should be included at the top of the file

.DEF py=r16
.DEF px=r17
.DEF dir=r18
.DEF btn=r19
.DEF temp1=r20

.EQU BOUND_T=1
.EQU BOUND_B=70
.EQU BOUND_L=1
.EQU BOUND_R=95

.EQU ML=0
.EQU MR=1
.EQU MU=3
.EQU MD=4
.EQU PEN=6
.EQU ERS=7

.EQU BUTTONS=PIND

;-------------------------------------------------------------
.CSEG						; code segment
.ORG $0000					; begining of code
	rjmp reset				; reset interrupt (and start)
.ORG  OVF0addr				; location of Timer0 Overflow ISR
	rjmp ISR_TOV0			; 2 Interrupt Service Routine for Timer0 Overflow
.ORG  OC0addr				; location of Timer0 Compare Match ISR (shouldn't need to do this but it is what works so...)
	rjmp ISR_COMP0			; 2 Interrupt Service Routine for Timer0 Overflow

;-------------------------------------------------------------

.ORG $0030					; after last interrupt

reset:
	SetupStack				; 4 Setup the Stack		! Required or do yourself
	SetupXPointer			; 2 Setup X Pointer		! Required or do yourself
	SetupYPointer			; 2 Setup X Pointer		! Required or do yourself
	SetupTVState			; 3 Setup TV State Mchn	! Required
	SetupTimer0				; 8 Setup Timer			! Required
	
;-------------------------------------------------------------

main:
	ldi  temp1,$ff			; 1 
	out  DDRB,temp1			; 1 
	ldi  temp1,$00			; 1
	out  DDRD,temp1			; 1

	; setup the picture
	FillImage $00		;   clear the image

	nop
	nop

;-----

	ClearTimer0				; ready to begin
	sei						; 1 enable interrupts 		! Required

init:
	ldi  px,0
	ldi  py,71
	ldi  temp1,96
line:
	SPX  px,py
	inc  px
	dec  temp1
	brne line
	
	ldi  py,10
	ldi  px,10
	ldi  dir,$00
	ldi  btn,$ff
	nop

	LDBMI cp1,0,73
	LDBMI cp2,1,73
	LDBMI cp3,2,73
	LDBMI ct,3,73
	LDBMI cv,4,73
	LDBMI cd,5,73
	LDBMI lr,6,73
	LDBMI la,7,73
	LDBMI lw,8,73
	
	nop
	nop

mainLoop:
;	rjmp mainLoop	; don't do anything

;	rcall WasteThousand
;	rcall WasteThousand
;	rcall WasteThousand
;	rcall WasteThousand
;	rcall WasteThousand
;	rcall WasteThousand
;	rcall WasteThousand
;	rcall WasteThousand
;	rcall WasteThousand
;	rcall WasteThousand

	nop
	nop
	TPX  px,py
	rcall LoadDir
	sbrc dir,PEN
	rcall PENPixelPre
	sbrc dir,ERS
	rcall ERSPixelPre
moveX:
	sbrc dir,ML
	rjmp moveLeft
	sbrc dir,MR
	rjmp moveRight
moveY:
	sbrc dir,MU
	rjmp moveUp
	sbrc dir,MD
	rjmp moveDown
moveDone:
	sbrc dir,PEN
	rcall PENPixelPost
	sbrc dir,ERS
	rcall ERSPixelPost
	
	rjmp mainLoop			; 2 keep it going

;-------------------------------------------------------------

MoveRight:
	LDBMI ar,10,73
	inc  px
	cpi  px,BOUND_R
	brlo mrd
	dec  px
	cbr  dir,MR
	LDBMI arx,10,73
mrd:
	rjmp moveY

;-------------------------------------------------------------

MoveLeft:
	LDBMI al,10,73
	dec  px
	cpi  px,BOUND_L
	brsh mld
	inc  px
	cbr  dir,ML
	LDBMI alx,10,73
mld:
	rjmp moveY

;-------------------------------------------------------------

MoveDown:
	LDBMI ad,10,73
	inc  py
	cpi  py,BOUND_B
	brlo mud
	dec  py
	cbr  dir,MU
	LDBMI adx,10,73
mud:
	rjmp moveDone

;-------------------------------------------------------------

MoveUp:
	LDBMI au,10,73
	dec  py
	cpi  py,BOUND_T
	brsh mdd
	inc  py
	cbr  dir,MD
	LDBMI aux,10,73
mdd:
	rjmp moveDone

;-------------------------------------------------------------

LoadDir:
	in   temp1,BUTTONS
	neg  temp1
	eor  btn,temp1
	;mov  btn,temp1

	bst  dir,PEN
	brts wasPEN
wasERS:
	bst  btn,PEN
	brts donePen

	sbr  btn,1<<ERS
	cbr  btn,1<<PEN
	rjmp donePEN
wasPEN:
	bst  btn,ERS
	brts donePEN

	sbr  btn,1<<PEN
	cbr  btn,1<<ERS
	rjmp donePEN
donePEN:
	mov  dir,btn
	in   btn,BUTTONS
	neg  btn
	ret

;-------------------------------------------------------------

PENPixelPre:
	LDBMI PENICON,11,73
	;LDBMI sp,11,73
	SPX  px,py
	ret

;-------------------------------------------------------------

ERSPixelPre:
	LDBMI ERSICON,11,73
	;LDBMI sp,11,73
	CPX  px,py
	ret

;-------------------------------------------------------------

PENPixelPost:
;	SPX  px,py
	ret

;-------------------------------------------------------------

ERSPixelPost:
;	CPX  px,py
	ret

;-------------------------------------------------------------


; Include the TV outpu routines
.include "8MHzM16TV.inc"			; The main API, has the interrupt routines and draws to the tv
.include "Delay.inc"				; Delay routines
.include "TVDrawChars.inc"			; EEPROM character data

