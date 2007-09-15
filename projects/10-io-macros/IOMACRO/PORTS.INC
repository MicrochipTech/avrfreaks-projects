
;*****   IOPORTS.INC  *****
;*****    ValYor,07   *****
;***** AVR Assembler2 *****
;**************************

;This file must be placed
;on the top of program,
;before device pin definitions.

;Each Device Pin
;is defined as two char string

;Left  nibble is Port Name
;Right nibble is Pin Number 
;Valid name leters are A,B,C,D,E,F,G,H,J,K,L
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

.EQU	H0	= $20
.EQU	H1	= $21
.EQU	H2	= $22
.EQU	H3	= $23
.EQU	H4	= $24
.EQU	H5	= $25
.EQU	H6	= $26
.EQU	H7	= $27

.EQU	J0	= $30
.EQU	J1	= $31
.EQU	J2	= $32
.EQU	J3	= $33
.EQU	J4	= $34
.EQU	J5	= $35
.EQU	J6	= $36
.EQU	J7	= $37

.EQU	K0	= $40
.EQU	K1	= $41
.EQU	K2	= $42
.EQU	K3	= $43
.EQU	K4	= $44
.EQU	K5	= $45
.EQU	K6	= $46
.EQU	K7	= $47

.EQU	L0	= $50
.EQU	L1	= $51
.EQU	L2	= $52
.EQU	L3	= $53
.EQU	L4	= $54
.EQU	L5	= $55
.EQU	L6	= $56
.EQU	L7	= $57


;Temporary register for devices with Memory Mapped I/O Registers
.DEF	IOREG	= r16



;*****  Define port data register (PORT)
.MACRO  ioport

	.SET PNAME = (@0&$F0)
	.SET PNUMB = (@0&$0F)


	.IF    PNAME == $A0
		.SET IONAME	= PORTA
	.ELIF  PNAME == $B0
		.SET IONAME	= PORTB
	.ELIF  PNAME == $C0
		.SET IONAME	= PORTC
	.ELIF  PNAME == $D0
		.SET IONAME	= PORTD
	.ELIF  PNAME == $E0
		.SET IONAME	= PORTE
	.ELIF  PNAME == $F0
		.SET IONAME	= PORTF
	.ELIF  PNAME == $10
		.SET IONAME	= PORTG
	.ELIF  PNAME == $20
		.SET IONAME	= PORTH
	.ELIF  PNAME == $30
		.SET IONAME	= PORTJ
	.ELIF  PNAME == $40
		.SET IONAME	= PORTK
	.ELIF  PNAME == $50
		.SET IONAME	= PORTL
	.ELSE	
		.ERROR "Bad Port Name"

	.ENDIF

.ENDM


;*****  Define data direction register (DDR)
.MACRO  ioddr

	.SET PNAME = (@0&$F0)
	.SET PNUMB = (@0&$0F)


	.IF    PNAME == $A0
		.SET IONAME	= DDRA
	.ELIF  PNAME == $B0
		.SET IONAME	= DDRB
	.ELIF  PNAME == $C0
		.SET IONAME	= DDRC
	.ELIF  PNAME == $D0
		.SET IONAME	= DDRD
	.ELIF  PNAME == $E0
		.SET IONAME	= DDRE
	.ELIF  PNAME == $F0
		.SET IONAME	= DDRF
	.ELIF  PNAME == $10
		.SET IONAME	= DDRG
	.ELIF  PNAME == $20
		.SET IONAME	= DDRH
	.ELIF  PNAME == $30
		.SET IONAME	= DDRJ
	.ELIF  PNAME == $40
		.SET IONAME	= DDRK
	.ELIF  PNAME == $50
		.SET IONAME	= DDRL
	.ELSE	
		.ERROR "Bad Port Name"

	.ENDIF

.ENDM


;*****  Define input pins address (PIN)
.MACRO  iopin

	.SET PNAME = (@0&$F0)
	.SET PNUMB = (@0&$0F)


	.IF    PNAME == $A0
		.SET IONAME	= PINA
	.ELIF  PNAME == $B0
		.SET IONAME	= PINB
	.ELIF  PNAME == $C0
		.SET IONAME	= PINC
	.ELIF  PNAME == $D0
		.SET IONAME	= PIND
	.ELIF  PNAME == $E0
		.SET IONAME	= PINE
	.ELIF  PNAME == $F0
		.SET IONAME	= PINF
	.ELIF  PNAME == $10
		.SET IONAME	= PING
	.ELIF  PNAME == $20
		.SET IONAME	= PINH
	.ELIF  PNAME == $30
		.SET IONAME	= PINJ
	.ELIF  PNAME == $40
		.SET IONAME	= PINK
	.ELIF  PNAME == $50
		.SET IONAME	= PINL
	.ELSE	
		.ERROR "Bad Port Name"

	.ENDIF

.ENDM



;***** For MACROS setb,clrb,skbs & skbc 
;***** see Atmel AVR001 Application Note
;***** Arguments: Address, Bit, Register
;***** Arguments= IONAME, PNUMB, IOREG

.MACRO setb

	.IF @1>7
		.ERROR "Pin Number out of  range"
	
	.ELIF @0>0x3F
		lds  @2, @0
		sbr  @2, (1<<@1)
		sts  @0, @2
	.ELIF @0>0x1F
		in   @2, @0
		sbr  @2, (1<<@1)
		out  @0, @2
	.ELSE
		sbi  @0, @1
	.ENDIF
.ENDM

.MACRO clrb

	.IF @1>7
		.ERROR "Pin Number out of  range"

	.ELIF @0>0x3F
		lds  @2, @0
		cbr  @2, (1<<@1)
		sts  @0, @2
	.ELIF @0>0x1F
		in   @2, @0
		cbr  @2, (1<<@1)
		out  @0, @2
	.ELSE
		cbi  @0, @1
	.ENDIF
.ENDM


.MACRO skbs

	.IF @1>7
		.ERROR "Pin Number out of  range"

	.ELIF @0>0x3F
		lds  @2, @0
		sbrs @2, @1
	.ELIF @0>0x1F
		in   @2, @0
		sbrs @2, @1
	.ELSE
		sbis @0, @1
	.ENDIF
.ENDM

.MACRO skbc

	.IF @1>7
		.ERROR "Pin Number out of  range"

	.ELIF @0>0x3F
		lds  @2, @0
		sbrc @2, @1
	.ELIF @0>0x1F
		in   @2, @0
		sbrc @2, @1
	.ELSE
		sbic @0, @1
	.ENDIF
.ENDM

;*********************************************



;*****  CLear bit in Port data register (PORT)
.MACRO  clp
	ioport @0
	clrb IONAME, PNUMB, IOREG
.ENDM

;*****  SEt bit in Port data register (PORT)
.MACRO  sep
	ioport @0
	setb IONAME, PNUMB, IOREG
.ENDM

;*****  Skip if bit in Port data register (PORT) is Cleared
.MACRO  spc
	ioport @0
	skbc IONAME, PNUMB, IOREG
.ENDM

;*****  Skip if bit in Port data register (PORT) is Set
.MACRO  sps
	ioport @0
	skbs IONAME, PNUMB, IOREG
.ENDM

;*****  CLear bit in data Direction register (DDR)
.MACRO  cld
	ioddr @0
	clrb IONAME, PNUMB, IOREG
.ENDM

;*****  SEt bit in data Direction register (DDR)
.MACRO  sed
	ioddr @0
	setb IONAME, PNUMB, IOREG
.ENDM

;*****  Skip if bit in data Direction register (DDR) is Cleared
.MACRO  sdc
	ioddr @0
	skbc IONAME, PNUMB, IOREG
.ENDM

;*****  Skip if bit in data Direction register (DDR) is Set
.MACRO  sds
	ioddr @0
	skbs IONAME, PNUMB, IOREG
.ENDM

;*****  Skip if bit in Input pins address (PIN) is Cleared
.MACRO  sic
	iopin @0
	skbc IONAME, PNUMB, IOREG
.ENDM

;*****  Skip if bit in Input pins address (PIN) is Set
.MACRO  sis
	iopin @0
	skbs IONAME, PNUMB, IOREG
.ENDM

