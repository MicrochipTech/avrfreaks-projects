;	Main routines
;
;	4 bytes memory usage
;
;	Michal Strug 2004
;
;		include this file at .org 0x00b


;---------------------------------
; Initialize stack in at90s2313
;---------------------------------
.MACRO InitStack
	ldi		R16, low(RAMEND)	
	out		SPL, R16
.ENDM
;=================================



;---------------------------------
; Store byte to sram memory
;---------------------------------
.MACRO StoreMem 
	ldi	XL, low( @0 << 1)	
	ldi	XH, high( @0 <<1 )	
	st	X, @1
.ENDM
;=================================

;---------------------------------
; Load byte from sram memory
;---------------------------------
.MACRO LoadMem 
	ldi	XL, low( @0 << 1)	
	ldi	XH, high( @0 <<1 )	
	ld	@1, X
.ENDM
;=================================




;---------------------------------
; Load byte from sram memory
;---------------------------------
;outmod:  
;         sbi portb,0
;         sbi ddrb,0
;         ret
;inmod:   
;         cbi ddrb,0
;         cbi portb,0
;         ret
;=================================

