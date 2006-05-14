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

.DEF py=r16					; temp register
.DEF px=r17					; temp register
.DEF dx=r18					; temp register
.DEF dy=r19					; temp register
.DEF temp1=r20				; temp register

.EQU BOUND_T=56
.EQU BOUND_B=81
.EQU BOUND_L=1
.EQU BOUND_R=95

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

	; setup the picture
	FillImage $00		;   clear the image

	nop
	nop

	ldi  py,$00
	ldi  px,$00
	ldi  dx,$00
	ldi  dy,$00
	ldi  temp1,$00

;-----

	ClearTimer0				; ready to begin
	sei						; 1 enable interrupts 		! Required

init:
	ldi  py,BOUND_T
	ldi  px,BOUND_L
	ldi  dy,$81
	ldi  dx,$81
	nop

	; Capicals on top rows
	LDBMI ca,0,1
	LDBMI cb,1,1
	LDBMI cc,2,1
	LDBMI cd,3,1
	LDBMI ce,4,1
	LDBMI cf,5,1
	LDBMI cg,6,1
	LDBMI ch,7,1
	LDBMI ci,8,1
	LDBMI cj,9,1
	LDBMI ck,10,1
	LDBMI cl,11,1
	LDBMI cm,0,10
	LDBMI cn,1,10
	LDBMI co,2,10
	LDBMI cp,3,10
	LDBMI cq,4,10
	LDBMI cr,5,10
	LDBMI cs,6,10
	LDBMI ct,7,10
	LDBMI cu,8,10
	LDBMI cv,9,10
	LDBMI cw,10,10
	LDBMI cx,11,10
	LDBMI cy,0,19
	LDBMI cz,1,19
; Lower case on next rows
	LDBMI la,2,19
	LDBMI lb,3,19
	LDBMI lc,4,19
	LDBMI ld,5,19
	LDBMI le,6,19
	LDBMI lf,7,19
	LDBMI lg,8,19
	LDBMI lh,9,19
	LDBMI li,10,19
	LDBMI lj,11,19
	LDBMI lk,0,28
	LDBMI ll,1,28
	LDBMI lm,2,28
	LDBMI ln,3,28
	LDBMI lo,4,28
	LDBMI lp,5,28
	LDBMI lq,6,28
	LDBMI lr,7,28
	LDBMI ls,8,28
	LDBMI lt,9,28
	LDBMI lu,10,28
	LDBMI lv,11,28
	LDBMI lw,0,37
	LDBMI lx,1,37
	LDBMI ly,2,37
	LDBMI lz,3,37
; numbers on bottom row
	LDBMI n0,0,46
	LDBMI n1,1,46
	LDBMI n2,2,46
	LDBMI n3,3,46
	LDBMI n4,4,46
	LDBMI n5,5,46
	LDBMI n6,6,46
	LDBMI n7,7,46
	LDBMI n8,8,46
	LDBMI n9,9,46
	
	nop
	nop

mainLoop:
;	rjmp mainLoop	; don't do anything

	rcall WasteThousand
	rcall WasteThousand
	rcall WasteThousand
	rcall WasteThousand
	rcall WasteThousand
	rcall WasteThousand
	rcall WasteThousand
	rcall WasteThousand
	rcall WasteThousand
	rcall WasteThousand
;
;	rjmp moveX		; don't flash, just bounce
;
;
;	; toggle move indicator pixels
;	TPXI 45,40
;	TPXI 46,40
;	TPXI 47,40
;	TPXI 45,41
;	TPXI 46,41
;	TPXI 47,41
;	TPXI 45,42
;	TPXI 46,42
;	TPXI 47,42
;	
;	nop
;	nop
;
;	rjmp mainLoop	; don't bounce the ball
	
	nop
	nop
	TPX  px,py
moveX:
	sbrc dx,7
	rjmp moveLeft
	sbrs dx,7
	rjmp moveRight
moveY:
	sbrc dy,7
	rjmp moveUp
	sbrs dy,7
	rjmp moveDown
moveDone:
	; set new pixel
	TPX  px,py
	
	rjmp mainLoop			; 2 keep it going

;-------------------------------------------------------------

MoveRight:
	mov  temp1,dx
	inc  px
	cpi  px,BOUND_R
	brlo mrd
	dec  px
	sbr  dx,$80
mrd:rjmp moveY

;-------------------------------------------------------------

MoveLeft:
	mov  temp1,dx
	dec  px
	cpi  px,BOUND_L
	brsh mld
	inc  px
	cbr  dx,$80
mld:rjmp moveY

;-------------------------------------------------------------

MoveDown:
	mov  temp1,dy
	inc  py
	cpi  py,BOUND_B
	brlo mud
	dec  py
	sbr  dy,$80
mud:rjmp moveDone

;-------------------------------------------------------------

MoveUp:
	mov  temp1,dy
	dec  py
	cpi  py,BOUND_T
	brsh mdd
	inc  py
	cbr  dy,$80
mdd:rjmp moveDone

;-------------------------------------------------------------

; Include the TV outpu routines
.include "8MHzM16TV.inc"			; The main API, has the interrupt routines and draws to the tv
.include "Delay.inc"				; Delay routines
.include "Characters.inc"			; EEPROM character data
