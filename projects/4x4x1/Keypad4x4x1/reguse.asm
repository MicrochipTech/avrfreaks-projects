; Register list and useage

; r0					; Reserved for multiplication math (not used here)
; r1					; Reserved for multiplication math (not used here)

.def	Temp	=r16	; A register that is always garbage
.def	Temp2	=r17	; See above.  But they're great for temp variables
.def 	TxByte 	=r18		; Serial port registers.
.def 	RxByte 	=r19		; Could be used for other things.
;.def	r21				; Used for keypad routines.
;.def	r22				; Used for keypad routines
.def	LoopC0	=r23	; Loop Counter, outside loop.  I like to 
						; define LoopC1 and LoopC2 for nesting.
						; if required.

; r26 and r27 reserved for X reg
; r28 and r29 reserved for Y reg
; r30 and r31 reserved for Z reg


