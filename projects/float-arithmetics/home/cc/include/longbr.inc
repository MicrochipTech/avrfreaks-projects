.macro	j_ne	dest		; Jump if non equal
	breq	.+2
	rjmp	\dest
.endm

.macro	j_eq	dest		; Jump if equal
	brne	.+2
	rjmp	\dest
.endm

.macro	j_pl	dest		; Jump if plus
	brmi	.+2
	rjmp	\dest
.endm

.macro	j_mi	dest		; Jump if minus
	brpl	.+2
	rjmp	\dest
.endm

.macro	j_cs	dest		; Jump if plus
	brcc	.+2
	rjmp	\dest
.endm

.macro	j_cc	dest		; Jump if minus
	brcs	.+2
	rjmp	\dest
.endm
