; Program SHA-1 (Secure Hashing)
; Original Author : Ahmad Husni (husni_111990290@yahoo.com)
; Microprocessor Laboratory STT Telkom Bandung - Indonesia
; Date : 19 July 2003
;
;
; General improvement and upgrade : LieBtrau (http://www.tiscali.be/~decoy)
;	-added possibility of processing multiple blocks
;	-free placement of code in memory (512-block and 160-bit hash can be
;		placed anywhere in memory)
;	-easy interfacing with C
;
; The A to E and H0 to H4 array must immediately follow each other!
; You can leave away some of the PUSH & POP's of the calcHash-routine if you
; are absolutely sure that your application doesn't use these registers.
; I believe it's better practice to leave the PUSH & POP's.  The small gain in
; speed will be small.
;
; Compiled and assembled with WinAVR (avr-gcc 3.3.2), debugged with 
; AVRStudio - AVR Simulator 4.09
;
; Cycle Counter : 36246 (IO-window -> Processor -> Cycle Counter)
; Using RAM 
;	code  				810 bytes
;	initialized data   	20  bytes
;	uninitialized data 	108 bytes
;	stack				26  bytes
; Using registers : R1(not changed), R6 to R15, R17 to R31

#include <avr/io.h>

;xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

.section .text

.global resetHash
resetHash:

; This subroutine fills variables A to E with the initial values H0 to H4

; Registers used:
; R25			temporary value
; R18			counter
; X (R27:R26)	point to source
; Z (R31:R30)	point to destination

	LDI ZH, hi8(H0)	;
	LDI	ZL, lo8(H0)	; Z = &H0
	
	LDI XH, hi8(resetValue)
	LDI XL, lo8(resetValue) ; X = &resetValue
	
	ldi R18,20
nextByte:			; A = H0, B = H1, C = H2, D = H3, E = H4 
	ld 	R25,X+
	std	Z+20,R25	; initialize digest
	st	Z+,R25		; initialize A to E
	dec R18
	BRNE nextByte
	
	RET
	
;_______________________________________________________________________________________

.global calcHash		; SHA-1 Computation
calcHash:

;This subroutine contains the main program loop:
;	It calls the subroutine that calculates TEMP
;	It calls the subroutine that updates A to E
;It receives a pointer to the base of the 512-bit block in R25:R24
;It returns a pointer to the digest in R25:R24.

;Used registers:
; R25			temporary value
; R17			t
; X(R27:R26)	points to the current base of the circular queue

	PUSH	YL
	PUSH	YH
	PUSH	R17
	PUSH	R15
	PUSH	R14
	PUSH	R13
	PUSH	R12
	PUSH	R11
	PUSH	R10
	PUSH	R9
	PUSH	R8
	PUSH	R7
	PUSH	R6
	PUSH	R5
	PUSH	R4
	PUSH	R3
	PUSH	R2
	PUSH	R1
	STS 	Wt,R24
	STS 	Wt+1,R25
	MOVW	XL,R24		; save pointer to 512-bit block in X
	LDI	R17, 00			; t = 0

LOOP_HASH:
	
	RCALL CalculateTEMP ; TEMP = S^5(A) + f(t;B,C,D) + E + W[s] + K(t)
	RCALL Update		; E = D
						; D = C
						; C = S^30(B)
						; B = A
						; A = TEMP  		
 	ADIW XL,4			; shift circular queue one position up
	INC	R17				; t++
	LDI	R25, 79+1	
	CPSE R17, R25		; until t=79
	RJMP LOOP_HASH
	RCALL ADD_INIT		; Compute (intermediate) hash value
	MOVW R24,ZL			; copy -pointer to digest- to the return value
	POP R1
	POP R2
	POP R3
	POP R4
	POP R5
	POP R6
	POP R7
	POP R8
	POP R9
	POP R10
	POP R11
	POP R12
	POP R13
	POP R14
	POP R15
	POP R17
	POP	YH
	POP YL
	RET

;_______________________________________________________________________________________
;This subroutine calls subroutines that calculate TEMP = S^5(A) + f(t;B,C,D) + 
;	E + W[s] + K(t)

;Used registers:
; R18 to R21	contains TEMP

CalculateTEMP:
	RCALL 	ROTa
	RCALL 	ADDe
	RCALL	ADDk
	RCALL	ADDWt
	RCALL 	ADDf
	RET
	
;______________________________________________________________________________________
;This subroutine rotates A three positions to the left and stores the result
;in R18 to R21

;Used registers:
; R18 to R21	S^5(A)
; R25			counter
; Z(R31:R30)	pointer to A

ROTa:
	LDI 	ZH,hi8(A)
	LDI		ZL,lo8(A)
	
	LDD		R18, Z+3
	LDD		R19, Z+2
	LDD		R20, Z+1
	LDD		R21, Z+0

	LDI 	R25, 0x5
LOOPROTL5:
	LSL		R18
	ROL		R19
	ROL		R20
	ROL		R21
	BRCC	NOTC3
	ORI		R18, 0x01
NOTC3:
	DEC		R25
	BRNE	LOOPROTL5
	RET
	
;______________________________________________________________________________________
; This routine adds E to TEMP

;Used registers:
; R18 to R21	TEMP
; R22 to R25	temporary double word
; Z(R31:R30)	pointer to E

ADDe:
	LDI		ZH,hi8(E)
	LDI		ZL,lo8(E)
	LDD		R22, Z+3
	LDD		R23, Z+2
	LDD		R24, Z+1
	LDD		R25, Z+0	; R22 to R25 = E

	ADD		R18, R22
	ADC		R19, R23
	ADC		R20, R24
	ADC		R21, R25	; TEMP += E
	RET

;____________________________________________________________________________________
;This subroutine calculates K(t) and adds that to TEMP

;Used registers:
; R22 to R25	temporary double word
; R17			t

ADDk:
	CPI		R17, 20		; t<20?
	BRLO	RuleK1
	CPI		R17, 40		; t<40?
	BRLO	RuleK2
	CPI		R17, 60		; t<60?
	BRLO	RuleK3
	CPI		R17, 80		; t<80?
	BRLO	RuleK4
		
RuleK1:
	LDI		R22, 0x99
	LDI		R23, 0x79
	LDI		R24, 0x82
	LDI		R25, 0x5A	; R22 to R25 = K(t) = 0x5A827999
	RJMP	FinishK

RuleK2:
	LDI		R22, 0xA1
	LDI		R23, 0xEB
	LDI		R24, 0xD9
	LDI		R25, 0x6E	; R22 to R25 = K(t) = 0xA1EBD96E
	RJMP	FinishK

RuleK3:
	LDI		R22, 0xdc
	LDI		R23, 0xbc
	LDI		R24, 0x1b
	LDI		R25, 0x8f	; R22 to R25 = K(t) = 0xDCBC1B8F
	RJMP	FinishK

RuleK4:
	LDI		R22, 0xD6
	LDI		R23, 0xC1
	LDI		R24, 0x62
	LDI		R25, 0xCA	; R22 to R25 = K(t) = 0xD6C162CA
	RJMP	FinishK

FinishK:
	ADD		R18, R22
	ADC		R19, R23
	ADC		R20, R24
	ADC		R21, R25	; TEMP += K(t)
	RET
	
;_____________________________________________________________________________________

; Used registers:
; R1			null-register (always 0 in C)
; R6 to R9		double word register (R9 = MSB)
; R13:R12		Wt+16*4 = begin of message	(R13 = MSB)
; R15:R14		Wt = end of message		(R15 = MSB)
; R17			t
; R18 to R21	TEMP
; R24			temporary value
; R25			temporary value
; R22 to R25	double word register (R25 = MSB)
; X				pointer to current Wt
; Y				pointer to W[s+q]
; Z				pointer to W[s]

ADDWt:
	CPI		R17, 16		; t>=16?
	BRSH	SecondRule

						; W[s] = W[t] for 0<=t<=15
	LD		R25, X+		; 
	LD		R24, X+
	LD		R23, X+
	LD		R22, X+		; R22 to R25 = W[s]
	SBIW	XL,4		; Restore X to Wt

	ADD		R18, R22
	ADC		R19, R23
	ADC		R20, R24
	ADC		R21, R25	; TEMP += W[s]
	RET

SecondRule:				; W[s]=S^1( W[s] XOR W[(s+2) AND MASK] XOR 
						; 	W[(s+8) AND MASK] XOR W[(s+13) AND MASK] )
	
	LDS		ZH,Wt+1
	LDS		ZL,Wt		; address of base of 512-bit block now in Z
	MOVW	R14,ZL		; R15_R14 = &Wt
	MOVW	R12,ZL		; R13_R12 = &Wt = start of message
						; (necessary for updating Y)
	
	LDI		R25,16*4
	ADD		R14,R25
	ADC		R15,R1		; R15_R14 = &(Wt+4*16) = end of message 
						; (necessary for updating Y)
	
	MOV		R25, R17	; R22=t
	LDI		R24, 0x0F
	AND		R25, R24	; R25=t AND MASK
	LSL		R25			; multiply with 4
	LSL		R25
	
	ADD		ZL,	R25
	ADC		ZH,R1		; Z = &W[s]
	MOVW	YL,ZL		; Y = Z
	
	LDD		R22, Y+3
	LDD		R23, Y+2
	LDD		R24, Y+1
	LDD		R25, Y+0	; R22 to R25 = W[s]
	
	ADIW	YL,2*4	
	RCALL	UpdateY	; don't overflow

	LDD		R6, Y+3
	LDD		R7, Y+2
	LDD		R8, Y+1
	LDD		R9, Y+0		; R6 to R9 = W[(s+2) & MASK]

	EOR		R22, R6
	EOR		R23, R7
	EOR		R24, R8
	EOR		R25, R9		; R22 to R25 = W[s] XOR W[(s+2) & MASK]

	ADIW	YL,6*4
	RCALL	UpdateY

	LDD		R6, Y+3
	LDD		R7, Y+2
	LDD		R8, Y+1
	LDD		R9, Y+0		; R6 to R9 = W[(s+8) & MASK]
	
	EOR		R22, R6
	EOR		R23, R7
	EOR		R24, R8
	EOR		R25, R9		; R22 to R25 = W[s] XOR W[(s+2) & MASK] XOR
						; 	W[(s+8) & MASK]

	ADIW	YL,5*4
	RCALL	UpdateY

	LDD		R6, Y+3
	LDD		R7, Y+2
	LDD		R8, Y+1
	LDD		R9, Y+0		; R6 to R9 = W[(s+13) & MASK]

	EOR		R22, R6
	EOR		R23, R7
	EOR		R24, R8
	EOR		R25, R9		; R22 to R25 = W[s] XOR W[(s+2) & MASK] XOR
						; 	W[(s+8) & MASK] XOR W[(s+13) & MASK]

	LSL		R22
	ROL		R23
	ROL		R24
	ROL		R25
	BRCC	NOTC4
	ORI		R22, 0x01
NOTC4:
						; R22 to R25 = S^1(W[s] XOR W[(s+2) & MASK] XOR
						; 	W[(s+8) & MASK] XOR W[(s+13) & MASK])

	STD		Z+3, R22
	STD		Z+2, R23
	STD		Z+1, R24
	STD		Z+0, R25	; W[s]= R22 to R25

	ADD		R18, R22
	ADC		R19, R23
	ADC		R20, R24
	ADC		R21, R25	; TEMP+=W[s]
	RET	


UpdateY:						
	;Check to see if Y reached end of message
	SUB		YL,R14				
	SBC		YH,R15				; Y-= end of message
	BRSH	UpdateY1			; if(Y>=0)UpdateY1();
	ADD		YL,R14				
	ADC		YH,R15				; else Y+=end of message
	RET							
UpdateY1:
	ADD		YL,R12				
	ADC		YH,R13				; Y+=begin of message
	RET
	
;________________________________________________________________________
;This subroutine calculates f(t;B,C,D) and adds it to TEMP.

;Used registers:
; R8 to R11			double word
; R12 to R15		double word
; R18 to R21		TEMP
; R22 to R25		double word
; Y(R29:R28)		pointer to double word

ADDf:	
	LDI		YH,hi8(B)
	LDI		YL,lo8(B)	; Y = &B
	
	LD		R15, Y+		; R12 to R15 = B
	LD		R14, Y+
	LD		R13, Y+
	LD		R12, Y+
	
						; Y = &C
	LD		R25, Y+		; R22 to R25 = C
	LD		R24, Y+
	LD		R23, Y+
	LD		R22, Y+

						; Y = &D
	LD		R11, Y+		; R8 to R11 = D
	LD		R10, Y+
	LD		R9, Y+
	LD		R8, Y+
	
	CPI		R17, 20		; for t=0 to 19
	BRLO	RuleF1		; f(t;B,C,D) = (B & C) XOR (~B & D)
	
	CPI		R17, 40		; for t=20 to 39
	BRLO	RuleF2		; f(t;B,C,D) = B XOR C XOR D
	
	CPI		R17, 60		; for t=40 to 59
	BRLO	RuleF3		; f(t;B,C,D) = (B & C) XOR (B & D) XOR (C & D)
	
	CPI		R17, 80		; for t=60 to 79
	BRLO	RuleF2		; f(t;B,C,D) = B XOR C XOR D
	
RuleF1:	
	AND		R22, R12
	AND		R23, R13
	AND		R24, R14
	AND		R25, R15	; R22 to R25 = B & C
	
	COM		R12
	COM		R13
	COM		R14
	COM		R15			; R12 to R15 = ~B
	
	AND		R12, R8
	AND		R13, R9
	AND		R14, R10
	AND		R15, R11	; R12 to R15 = ~B & D

	EOR		R22, R12
	EOR		R23, R13
	EOR		R24, R14
	EOR		R25, R15	; R22 to R25 = (B & C) XOR (~B & D)
	RJMP	FinishF

RuleF2:
	EOR		R22, R12
	EOR		R23, R13
	EOR		R24, R14
	EOR		R25, R15	; R22 to R25 = B XOR C

	EOR		R22, R8
	EOR		R23, R9
	EOR		R24, R10
	EOR		R25, R11	; R22 to R25 = B XOR C XOR D
	RJMP	FinishF

RuleF3:
	PUSH	R22
	PUSH	R23
	PUSH	R24
	PUSH	R25			; Save C on the STACK

	AND		R22, R12
	AND		R23, R13
	AND		R24, R14
	AND		R25, R15	; R22 to R25 = B & C

	AND		R12, R8
	AND		R13, R9
	AND		R14, R10
	AND		R15, R11	; R12 to R15 = B & D

	EOR		R22, R12
	EOR		R23, R13
	EOR		R24, R14
	EOR		R25, R15	; R22 to R25 = XX = (B & C) XOR (B & D)

	POP		R15
	POP		R14
	POP		R13
	POP		R12			; R12 to R15 = C (restored from STACK)

	AND		R15, R11
	AND		R14, R10
	AND		R13, R9
	AND		R12, R8		; R12 to R15 = C & D

	EOR		R22, R12
	EOR		R23, R13
	EOR		R24, R14
	EOR		R25, R15	; R22 to R25 = (B & C) XOR (B & D) XOR (C & D)
	RJMP	FinishF
			
FinishF:
	ADD		R18, R22
	ADC		R19, R23
	ADC		R20, R24
	ADC		R21, R25	; TEMP += f(t;B,C,D)
	RET
	
;_________________________________________________________________________
;This subroutine updates A to E

;Used registers:
; R18 to R21	TEMP
; R24			temporary value
; R25			temporary value
; R22 to R25	double word
; Y(R29:R28)	pointer to double word
; Z(R31:R30)	pointer to double word

Update:
	LDI	YL, lo8(E+4)
	LDI	YH,	hi8(E+4)		; Y = &E+4
	LDI	ZL, lo8(D+4)
	LDI ZH, hi8(D+4)		; Z = &D+4					
	RCALL GoTransfer		; E = D ; Y = &E ; Z = &D
						
	RCALL GoTransfer		; D = C ; Y = &D ; Z = &C
	
	RCALL calcS30B			; Rotate B left 30 positions (= rotate right 2 positions)
	ST  -Y, R22
	ST  -Y, R23
	ST	-Y, R24
	ST	-Y, R25				; C = S^30(B) ; Y=&C ; Z=&C
	
	RCALL GoTransfer		; B = A ; Y=&B ; Z=&A

	ST	-Y, R18
	ST	-Y, R19
	ST	-Y, R20
	ST	-Y, R21				; A = TEMP ; Y=&A
	RET

GoTransfer:					; *Y-1=*Z-1 ; *Y-2=*Z-2 ; *Y-3=*Z-3 ; *Y-4=*Z-4
	ldi R24,4				; counter = 4
nextByte2:				
	ld 	R25,-Z
	st	-Y,R25
	dec R24					; counter--
	BRNE nextByte2			; counter==0 ? 

	RET

;______________________________________________________________________________________
;This subroutine calculates S^30(B) by rotating B two positions to the left.  The 
;rotated value is returned in R22 to R25.  When calling this routine, Z must point 
;to &B+4.

;Used registers:
; R22 to R25	double word
; Z(R31:R30)	pointer to double word

calcS30B:

	LD	R22, -Z			; LSB (highest in memory)
	LD	R23, -Z
	LD	R24, -Z
	LD	R25, -Z			; MSB (lowest in memory) R22 to R25 = B

	LSR	R25
	ROR	R24
	ROR	R23
	ROR	R22
	BRCC NotC1
	ORI	R25, 0x80		; Rotated B right one position
NotC1:
	LSR	R25
	ROR	R24
	ROR	R23
	ROR	R22
	BRCC NotC2			; Rotated B right one position
	ORI	R25, 0x80		; R22 to R25 = S^30(B) = two times rotated right B
NotC2:
	RET
	
;______________________________________________________________________________________
;This subroutine calculates the sum of the intermediate hash value (H0 to H4) with 
;intermediate values (A to E).  And stores the sum as well to A to E as to the 
;intermediate hash value.
;When exiting this subroutine, Z points to H0, equal to the base of the digest.

;Used registers
; R17			counter
; R18 to R21	temporary dword
; R22 to R25	temporary dword
; Z(R31:R30)	pointer to dword's A to E

ADD_INIT:
	LDI	ZH,hi8(H4+4)
	LDI ZL,lo8(H4+4)		; initialize pointer Z to &H4+4
	ldi	R17,5				; counter = 5 (5 words to add)

nextdWord:
	LD	R18, -Z
	LD 	R19, -Z
	LD 	R20, -Z
	LD 	R21, -Z			; R18 to R21 = one out of A to E
	LDD R25, Z+20
	LDD R24, Z+21
	LDD R23, Z+22
	LDD R22, Z+23		; R22 to R25 = Hx

	ADD R18, R22
	ADC R19, R23
	ADC R20, R24
	ADC R21, R25		; R18 to R21 = one out of A to E + Hx

	STD Z+20, R21
	STD Z+21, R20
	STD Z+22, R19
	STD Z+23, R18		; one out of A to E += Hx
	STD Z+0, R21
	STD Z+1, R20
	STD Z+2, R19
	STD Z+3, R18		; Hx += one out of A to E
	
	dec R17				; counter--
	brne nextdWord		; counter==0 ?
	RET
	
;xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

.section .bss			; Unitialized data
						; "lcomm" defines a local symbol
.lcomm H0,4				; H1
.lcomm H1,4				; H2
.lcomm H2,4				; H3
.lcomm H3,4				; H4
.lcomm H4,4				; H5

.lcomm A,4				; A
.lcomm B,4				; B
.lcomm C,4				; C
.lcomm D,4				; D
.lcomm E,4				; E
.lcomm Wt,2				; Pointer to base of 512-bit block

;xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

.section .data			; initialized data

resetValue:					; initial values of A to E and of H0 to H4
.byte 0x67,0x45,0x23,0x01	; H0, A
.byte 0xEF,0xCD,0xAB,0x89	; H1, B
.byte 0x98,0xBA,0xDC,0xFE	; H2, C
.byte 0x10,0x32,0x54,0x76	; H3, D
.byte 0xC3,0xD2,0xE1,0xF0	; H4, E
