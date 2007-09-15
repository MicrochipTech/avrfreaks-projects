
;****** IOMEGA64.INC ******
;******  ValYor,07   ******
;**************************

;This file must be placed
;on the top of program,
;before device pin definitions.

;Each Device Pin
;is defined as two char string
;or two digit hexadecimal number

;Left  nibble is Port Name
;Right nibble is Pin Number 
;Valid names are A,B,C,D,E,F,G(1 as a digit)
;Valid numbers are 0,1,2,3,4,5,6,7

;Redefine strings as digits

.EQU	A0	= $A0
.EQU	A1	= $A1
.EQU	A2	= $A2
.EQU	A3	= $A3
.EQU	A4	= $A4
.EQU	A5	= $A5
.EQU	A6	= $A6
.EQU	A7	= $A7

.EQU	B0	= $B0
.EQU	B1	= $B1
.EQU	B2	= $B2
.EQU	B3	= $B3
.EQU	B4	= $B4
.EQU	B5	= $B5
.EQU	B6	= $B6
.EQU	B7	= $B7

.EQU	C0	= $C0
.EQU	C1	= $C1
.EQU	C2	= $C2
.EQU	C3	= $C3
.EQU	C4	= $C4
.EQU	C5	= $C5
.EQU	C6	= $C6
.EQU	C7	= $C7

.EQU	D0	= $D0
.EQU	D1	= $D1
.EQU	D2	= $D2
.EQU	D3	= $D3
.EQU	D4	= $D4
.EQU	D5	= $D5
.EQU	D6	= $D6
.EQU	D7	= $D7

.EQU	E0	= $E0
.EQU	E1	= $E1
.EQU	E2	= $E2
.EQU	E3	= $E3
.EQU	E4	= $E4
.EQU	E5	= $E5
.EQU	E6	= $E6
.EQU	E7	= $E7

.EQU	F0	= $F0
.EQU	F1	= $F1
.EQU	F2	= $F2
.EQU	F3	= $F3
.EQU	F4	= $F4
.EQU	F5	= $F5
.EQU	F6	= $F6
.EQU	F7	= $F7

.EQU	G0	= $10
.EQU	G1	= $11
.EQU	G2	= $12
.EQU	G3	= $13
.EQU	G4	= $14
.EQU	G5	= $15
.EQU	G6	= $16
.EQU	G7	= $17


;Temporary register for devices with Memory Mapped I/O Registers
.DEF	IOREG	= r16


;Port Name	= @0&$F0	- Left  nibble
;Pin Number = @0&$0F	- Right nibble



;*****  Clear bit in port data register (PORT)
.MACRO  clp

	.IF   (@0&$0F)>7
		.ERROR "Pin Number out of range"

	.ELIF (@0&$F0)==$A0
		cbi PORTA,(@0&$0F)
	.ELIF (@0&$F0)==$B0
		cbi PORTB,(@0&$0F)
	.ELIF (@0&$F0)==$C0
		cbi PORTC,(@0&$0F)
	.ELIF (@0&$F0)==$D0
		cbi PORTD,(@0&$0F)
	.ELIF (@0&$F0)==$E0
		cbi PORTE,(@0&$0F)
	.ELIF (@0&$F0)==$F0
		lds IOREG,PORTF
		cbr IOREG,1<<(@0&$0F)
		sts PORTF,IOREG
	.ELIF (@0&$F0)==$10
		lds IOREG,PORTG
		cbr IOREG,1<<(@0&$0F)
		sts PORTG,IOREG
	.ELSE	
		.ERROR "Bad Port Name"

	.ENDIF

.ENDM


;*****  Set bit in port data register (PORT)
.MACRO  sep

	.IF   (@0&$0F)>7
		.ERROR "Pin Number out of range"

	.ELIF (@0&$F0)==$A0
		sbi PORTA,(@0&$0F)
	.ELIF (@0&$F0)==$B0
		sbi PORTB,(@0&$0F)
	.ELIF (@0&$F0)==$C0
		sbi PORTC,(@0&$0F)
	.ELIF (@0&$F0)==$D0
		sbi PORTD,(@0&$0F)
	.ELIF (@0&$F0)==$E0
		sbi PORTE,(@0&$0F)
	.ELIF (@0&$F0)==$F0
		lds IOREG,PORTF
		sbr IOREG,1<<(@0&$0F)
		sts PORTF,IOREG
	.ELIF (@0&$F0)==$10
		lds IOREG,PORTG
		sbr IOREG,1<<(@0&$0F)
		sts PORTG,IOREG
	.ELSE	
		.ERROR "Bad Port Name"

	.ENDIF

.ENDM


;*****  Skip if bit in port data register (PORT) is cleared
.MACRO  spc

	.IF   (@0&$0F)>7
		.ERROR "Pin Number out of range"

	.ELIF (@0&$F0)==$A0
		sbic PORTA,(@0&$0F)
	.ELIF (@0&$F0)==$B0
		sbic PORTB,(@0&$0F)
	.ELIF (@0&$F0)==$C0
		sbic PORTC,(@0&$0F)
	.ELIF (@0&$F0)==$D0
		sbic PORTD,(@0&$0F)
	.ELIF (@0&$F0)==$E0
		sbic PORTE,(@0&$0F)
	.ELIF (@0&$F0)==$F0
		lds IOREG,PORTF
		sbrc IOREG,(@0&$0F)
	.ELIF (@0&$F0)==$10
		lds IOREG,PORTG
		sbrc IOREG,(@0&$0F)
	.ELSE	
		.ERROR "Bad Port Name"

	.ENDIF

.ENDM


;*****  Skip if bit in port data register (PORT) is set
.MACRO  sps

	.IF   (@0&$0F)>7
		.ERROR "Pin Number out of range"

	.ELIF (@0&$F0)==$A0
		sbis PORTA,(@0&$0F)
	.ELIF (@0&$F0)==$B0
		sbis PORTB,(@0&$0F)
	.ELIF (@0&$F0)==$C0
		sbis PORTC,(@0&$0F)
	.ELIF (@0&$F0)==$D0
		sbis PORTD,(@0&$0F)
	.ELIF (@0&$F0)==$E0
		sbis PORTE,(@0&$0F)
	.ELIF (@0&$F0)==$F0
		lds IOREG,PORTF
		sbrs IOREG,(@0&$0F)
	.ELIF (@0&$F0)==$10
		lds IOREG,PORTG
		sbrs IOREG,(@0&$0F)
	.ELSE	
		.ERROR "Bad Port Name"

	.ENDIF

.ENDM


;*****  Clear bit in data direction register (DDR)
.MACRO  cld

	.IF   (@0&$0F)>7
		.ERROR "Pin Number out of range"

	.ELIF (@0&$F0)==$A0
		cbi DDRA,(@0&$0F)
	.ELIF (@0&$F0)==$B0
		cbi DDRB,(@0&$0F)
	.ELIF (@0&$F0)==$C0
		cbi DDRC,(@0&$0F)
	.ELIF (@0&$F0)==$D0
		cbi DDRD,(@0&$0F)
	.ELIF (@0&$F0)==$E0
		cbi DDRE,(@0&$0F)
	.ELIF (@0&$F0)==$F0
		lds IOREG,DDRF
		cbr IOREG,1<<(@0&$0F)
		sts DDRF,IOREG
	.ELIF (@0&$F0)==$10
		lds IOREG,DDRG
		cbr IOREG,1<<(@0&$0F)
		sts DDRG,IOREG

	.ELSE	
		.ERROR "Bad Port Name"

.ENDIF

.ENDM


;*****  Set bit in data direction register (DDR)
.MACRO  sed

	.IF   (@0&$0F)>7
		.ERROR "Pin Number out of range"

	.ELIF (@0&$F0)==$A0
		sbi DDRA,(@0&$0F)
	.ELIF (@0&$F0)==$B0
		sbi DDRB,(@0&$0F)
	.ELIF (@0&$F0)==$C0
		sbi DDRC,(@0&$0F)
	.ELIF (@0&$F0)==$D0
		sbi DDRD,(@0&$0F)
	.ELIF (@0&$F0)==$E0
		sbi DDRE,(@0&$0F)
	.ELIF (@0&$F0)==$F0
		lds IOREG,DDRF
		sbr IOREG,1<<(@0&$0F)
		sts DDRF, IOREG
	.ELIF (@0&$F0)==$10
		lds IOREG,DDRG
		sbr IOREG,1<<(@0&$0F)
		sts DDRG,IOREG
	.ELSE	
		.ERROR "Bad Port Name"

.ENDIF

.ENDM


;*****  Skip if bit in data direction register (DDR) is cleared
.MACRO  sdc

	.IF   (@0&$0F)>7
		.ERROR "Pin Number out of range"

	.ELIF (@0&$F0)==$A0
		sbic DDRA,(@0&$0F)
	.ELIF (@0&$F0)==$B0
		sbic DDRB,(@0&$0F)
	.ELIF (@0&$F0)==$C0
		sbic DDRC,(@0&$0F)
	.ELIF (@0&$F0)==$D0
		sbic DDRD,(@0&$0F)
	.ELIF (@0&$F0)==$E0
		sbic DDRE,(@0&$0F)
	.ELIF (@0&$F0)==$F0
		lds IOREG,DDRF
		sbrc IOREG,(@0&$0F)
	.ELIF (@0&$F0)==$10
		lds IOREG,DDRG
		sbrc IOREG,(@0&$0F)
	.ELSE	
		.ERROR "Bad Port Name"

	.ENDIF

.ENDM


;*****  Skip if bit in data direction register (DDR) is set
.MACRO  sds

	.IF   (@0&$0F)>7
		.ERROR "Pin Number out of range"

	.ELIF (@0&$F0)==$A0
		sbis DDRA,(@0&$0F)
	.ELIF (@0&$F0)==$B0
		sbis DDRB,(@0&$0F)
	.ELIF (@0&$F0)==$C0
		sbis DDRC,(@0&$0F)
	.ELIF (@0&$F0)==$D0
		sbis DDRD,(@0&$0F)
	.ELIF (@0&$F0)==$E0
		sbis DDRE,(@0&$0F)
	.ELIF (@0&$F0)==$F0
		lds IOREG,DDRF
		sbrs IOREG,(@0&$0F)
	.ELIF (@0&$F0)==$10
		lds IOREG,DDRG
		sbrs IOREG,(@0&$0F)
	.ELSE	
		.ERROR "Bad Port Name"

	.ENDIF

.ENDM


;*****  Skip if bit in input pins address (PIN) is cleared
.MACRO  sic

	.IF   (@0&$0F)>7
		.ERROR "Pin Number out of range"

	.ELIF (@0&$F0)==$A0
		sbic PINA,(@0&$0F)
	.ELIF (@0&$F0)==$B0
		sbic PINB,(@0&$0F)
	.ELIF (@0&$F0)==$C0
		sbic PINC,(@0&$0F)
	.ELIF (@0&$F0)==$D0
		sbic PIND,(@0&$0F)
	.ELIF (@0&$F0)==$E0
		sbic PINE,(@0&$0F)
	.ELIF (@0&$F0)==$F0
		sbic PINF,(@0&$0F)
	.ELIF (@0&$F0)==$10
		lds IOREG,PING
		sbrc IOREG,(@0&$0F)
	.ELSE	
		.ERROR "Bad Port Name"

	.ENDIF

.ENDM


;*****  Skip if bit in input pins address (PIN) is set
.MACRO  sis

	.IF   (@0&$0F)>7
		.ERROR "Pin Number out of range"

	.ELIF (@0&$F0)==$A0
		sbis PINA,(@0&$0F)
	.ELIF (@0&$F0)==$B0
		sbis PINB,(@0&$0F)
	.ELIF (@0&$F0)==$C0
		sbis PINC,(@0&$0F)
	.ELIF (@0&$F0)==$D0
		sbis PIND,(@0&$0F)
	.ELIF (@0&$F0)==$E0
		sbis PINE,(@0&$0F)
	.ELIF (@0&$F0)==$F0
		sbis PINF,(@0&$0F)
	.ELIF (@0&$F0)==$10
		lds IOREG,PING
		sbrs IOREG,(@0&$0F)
	.ELSE	
		.ERROR "Bad Port Name"

	.ENDIF

.ENDM
