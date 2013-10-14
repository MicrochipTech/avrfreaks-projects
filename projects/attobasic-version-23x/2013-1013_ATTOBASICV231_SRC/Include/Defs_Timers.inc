;*************************************************************
; TIMER clock divider definitions.  Covers the divider rates
;  for Mega88/168/328 and Mega32U4.
;*************************************************************
.ifdef TIMSK0
; TIMER0 Prescaler Options
.equ	CK0_0	= 0					; stop clock
.equ	CK0_1	= 1					; Prescaler 1x
.equ	CK0_8	= 2					; Prescaler 8x
.equ	CK0_64	= 3					; Prescaler 64x
.equ	CK0_256	= 4					; Prescaler 256x
.equ	CK0_1K	= 5					; Prescaler 1024x
.endif
;-----------------------------------
.ifdef TIMSK1
; TIMER1 Prescaler Options
.equ	CK1_0	= 0					; stop clock
.equ	CK1_1	= 1					; Prescaler 1x
.equ	CK1_8	= 2					; Prescaler 8x
.equ	CK1_64	= 3					; Prescaler 64x
.equ	CK1_256	= 4					; Prescaler 256x
.equ	CK1_1K	= 5					; Prescaler 1024x
.endif
;-----------------------------------
.ifdef TIMSK2
; TIMER2 Prescaler Options
.equ	CK2_0	= 0					; stop clock
.equ	CK2_1	= 1					; Prescaler 1x
.equ	CK2_8	= 2					; Prescaler 8x
.equ	CK2_32	= 3					; Prescaler 32x
.equ	CK2_64	= 4					; Prescaler 64x
.equ	CK2_128	= 5					; Prescaler 128x
.equ	CK2_256	= 6					; Prescaler 256x
.equ	CK2_1K	= 7					; Prescaler 1024x
.endif
;-----------------------------------
.ifdef TIMSK3
; TIMER3 Prescaler Options
.equ	CK3_0	= 0					; stop clock
.equ	CK3_1	= 1					; Prescaler 1x
.equ	CK3_8	= 2					; Prescaler 8x
.equ	CK3_64	= 3					; Prescaler 64x
.equ	CK3_256	= 4					; Prescaler 256x
.equ	CK3_1K	= 5					; Prescaler 1024x
.endif
;-----------------------------------
.ifdef TIMSK4
; TIMER4 Prescaler Options
.equ	CK4_0	= 0					; stop clock
.equ	CK4_1	= 1					; Prescaler 1x
.equ	CK4_2	= 2					; Prescaler 2x
.equ	CK4_4	= 3					; Prescaler 4x
.equ	CK4_8	= 4					; Prescaler 8x
.equ	CK4_16	= 5					; Prescaler 16x
.equ	CK4_32	= 6					; Prescaler 32x
.equ	CK4_64	= 7					; Prescaler 64x
.equ	CK4_128	= 8					; Prescaler 128x
.equ	CK4_256	= 9					; Prescaler 256x
.equ	CK4_512	= 10				; Prescaler 512x
.equ	CK4_1K	= 11				; Prescaler 1024x
.equ	CK4_2K	= 12				; Prescaler 2048x
.equ	CK4_4K	= 13				; Prescaler 4096x
.equ	CK4_8K	= 14				; Prescaler 8192x
.equ	CK4_16K	= 15				; Prescaler 16384x
.endif
;-----------------------------------
.ifdef TIMSK5
; TIMER5 Prescaler Options
.equ	CK5_0	= 0					; stop clock
.equ	CK5_1	= 1					; Prescaler 1x
.equ	CK5_8	= 2					; Prescaler 8x
.equ	CK5_64	= 3					; Prescaler 64x
.equ	CK5_256	= 4					; Prescaler 256x
.equ	CK5_1K	= 5					; Prescaler 1024x
.endif
;-----------------------------------
