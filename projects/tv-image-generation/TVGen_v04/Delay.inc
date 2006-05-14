; /---------------------------------------------\
; |              Delay Subroutines              |
; | --------------------------------------------|
; |                                             |
; | Sample Usage                                |
; |                                             |
; | rcall WasteSeven        ; 7 cycles gone     |
; | rcall WasteHundred      ; 100 cycles gone   |
; |                                             |
; | --------------------------------------------|
; |  Writen by John Haskins                     |
; |   v1.0 01.23.05                             |
; \---------------------------------------------/

.CSEG

;-------------------------------------------------------------

WasteThousand:				; 3 for rcall
	nop						; 1
	nop						; 1
	nop						; 1
	rcall WasteHundred		; 100
	rcall WasteHundred		; 100
	rcall WasteHundred		; 100
	rcall WasteHundred		; 100
	rcall WasteHundred		; 100
	rcall WasteHundred		; 100
	rcall WasteHundred		; 100
	rcall WasteHundred		; 100
	rcall WasteHundred		; 100
	rcall WasteTen			; 10
	rcall WasteTen			; 10
	rcall WasteTen			; 10
	rcall WasteTen			; 10
	rcall WasteTen			; 10
	rcall WasteTen			; 10
	rcall WasteTen			; 10
	rcall WasteTen			; 10
	rcall WasteTen			; 10
	ret						; 4 return

;-------------------------------------------------------------

WasteHundred:				; 3 for rcall
	nop						; 1
	nop						; 1
	nop						; 1
	rcall WasteTen			; 10
	rcall WasteTen			; 10
	rcall WasteTen			; 10
	rcall WasteTen			; 10
	rcall WasteTen			; 10
	rcall WasteTen			; 10
	rcall WasteTen			; 10
	rcall WasteTen			; 10
	rcall WasteTen			; 10
	ret						; 4 return

;-------------------------------------------------------------

WasteFifty:					; 3 for rcall cost
	rcall WasteTen			; 10
	rcall WasteTen			; 10
	rcall WasteTen			; 10
	rcall WasteTen			; 10
	nop						; 1
	nop						; 1
	nop						; 1
	ret						; 4 return

;-------------------------------------------------------------

WasteTen:					; 3 for rcall cost
	nop						; 1
	nop						; 1
	nop						; 1
	ret						; 4 return

;-------------------------------------------------------------

WasteSeven:					; 3 for rcall
	ret;					; 4 return

;-------------------------------------------------------------
