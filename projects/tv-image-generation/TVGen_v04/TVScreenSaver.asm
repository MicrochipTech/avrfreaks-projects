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

.EQU BOUND_B=1
.EQU BOUND_T=82
.EQU BOUND_L=1
.EQU BOUND_R=96

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

; numbers on bottom row
	LDBMI n0,0,7
	LDBMI n1,1,7
	LDBMI n2,2,7
	LDBMI n3,3,7
	LDBMI n4,4,7
	LDBMI n5,5,7
	LDBMI n6,6,7
	LDBMI n7,7,7
	LDBMI n8,8,7
	LDBMI n9,9,7
; Capicals on top row
	LDBMI ca,0,81
	LDBMI cb,1,81
	LDBMI cc,2,81
	LDBMI cd,3,81
	LDBMI ce,4,81
	LDBMI cf,5,81
	LDBMI cg,6,81
	LDBMI ch,7,81
	LDBMI ci,8,81
	LDBMI cj,9,81
	LDBMI ck,10,81
	LDBMI cl,11,81
	LDBMI cm,0,72
	LDBMI cn,1,72
	LDBMI co,2,72
	LDBMI cp,3,72
	LDBMI cq,4,72
	LDBMI cr,5,72
	LDBMI cs,6,72
	LDBMI ct,7,72
	LDBMI cu,8,72
	LDBMI cv,9,72
	LDBMI cw,10,72
	LDBMI cx,11,72
	LDBMI cy,0,63
	LDBMI cz,1,63
; Lower case on next row
	LDBMI la,0,54
	LDBMI lb,1,54
	LDBMI lc,2,54
	LDBMI ld,3,54
	LDBMI le,4,54
	LDBMI lf,5,54
	LDBMI lg,6,54
	LDBMI lh,7,54
	LDBMI li,8,54
	LDBMI lj,9,54
	LDBMI lk,10,54
	LDBMI ll,11,54
	LDBMI lm,0,45
	LDBMI ln,1,45
	LDBMI lo,2,45
	LDBMI lp,3,45
	LDBMI lq,4,45
	LDBMI lr,5,45
	LDBMI ls,6,45
	LDBMI lt,7,45
	LDBMI lu,8,45
	LDBMI lv,9,45
	LDBMI lw,10,45
	LDBMI lx,11,45
	LDBMI ly,0,36
	LDBMI lz,1,36


;-----

;-----


	ClearTimer0				; ready to begin
	sei						; 1 enable interrupts 		! Required

init:
	ldi  py,0;((BOUND_T-BOUND_B)/3+BOUND_B)
	ldi  px,0;((BOUND_R-BOUND_L)/3+BOUND_L)
	ldi  dy,81
	ldi  dx,81
	nop

mainLoop:
	rjmp mainLoop	; don't do anything
;
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
;	
;	nop
;	nop
;
;moveX:
;	sbrc dx,7
;	rjmp moveLeft
;	sbrs dx,7
;	rjmp moveRight
;moveY:
;	sbrc dy,7
;	rjmp moveDown
;	sbrs dy,7
;	rjmp moveUp
;moveDone:
;	; set new pixel
;	TPX  px,py
;	
	rjmp mainLoop			; 2 keep it going
;
;-------------------------------------------------------------
;
;MoveRight:
;	mov  temp1,dx
;	inc  px
;	; bound check
;	cpi  px,BOUND_R
;	brlo mrd
;	sbr  dx,$80
;mrd:rjmp moveY
;
;-------------------------------------------------------------
;
;MoveLeft:
;	mov  temp1,dx
;	dec  px
;	; bound check
;	cpi  px,BOUND_L
;	brsh mld
;	cbr  dx,$80
;mld:rjmp moveY
;
;-------------------------------------------------------------
;
;MoveUp:
;	mov  temp1,dy
;	inc  py
;	; bound check
;	cpi  py,BOUND_T
;	brlo mud
;	sbr  dy,$80
;mud:rjmp moveDone
;
;-------------------------------------------------------------
;
;MoveDown:
;	mov  temp1,dy
;	dec  py
;	; bound check
;	cpi  py,BOUND_B
;	brsh mdd
;	cbr  dy,$80
;mdd:rjmp moveDone
;
;-------------------------------------------------------------

; Include the TV outpu routines
.include "8MHzM16TV.inc"			; The main API, has the interrupt routines and draws to the tv
.include "Delay.inc"				; Delay routines
.include "Characters.inc"			; EEPROM character data
