DIVIDE:

	; this function makes the following operation
	; (R5:R4:R3:R2) = (R9:R8:R7:R6) / (R13:R12)

LDI R21,32

div8:
CLR R2	 	; 
CLR R3	 	; clear result 
CLR R4
CLR R5

CLR R10
CLR R11
div8a:
CLC 		; clear carry-bit
ROL R6 	
ROL R7	 	 
ROL R8
ROL R9
ROL R10
ROL R11

CP R10,R12
CPC R11,R13	
brcs div8c 	; jump over subtraction, if smaller
div8b:
sub R10,R12
SBC R11,R13	
SEC 		; set carry-bit, result is a 1
rjmp div8d 	; jump to shift of the result bit
div8c:
CLC 		; clear carry-bit, resulting bit is a 0
div8d:
ROL R2	 	; rotate carry-bit into result registers
ROL R3
ROL R4
ROL R5
DEC R21
BRNE div8a


RET
