.include "m8515def.inc"

; the port registers used to set the DB0-DB7 data pins
.equ	DDATAPORT	= DDRA
.equ	DATAPORT	= PORTA

; the port registers used to set the RS and the E signals
.equ	DCTRLPORT	= DDRD
.equ	CTRLPORT	= PORTD
.equ	RSPIN		= PD4
.equ	EPIN		= PD5

; data pins
.equ	DB0		= 0
.equ	DB1		= 1
.equ	DB2		= 2
.equ	DB3		= 3
.equ	DB4		= 4
.equ	DB5		= 5
.equ	DB6		= 6
.equ	DB7		= 7

; character table - these values come from a table in the HD44780 datasheet
.equ	A		= 0b01000001
.equ	B		= 0b01000010
.equ	C		= 0b01000011
.equ	D		= 0b01000100
.equ	E		= 0b01000101
.equ	F		= 0b01000110
.equ	G		= 0b01000111
.equ	H		= 0b01001000
.equ	I		= 0b01001001
.equ	J		= 0b01001010
.equ	K		= 0b01001011
.equ	L		= 0b01001100
.equ	M		= 0b01001101
.equ	N		= 0b01001110
.equ	O		= 0b01001111
.equ	P		= 0b01010000
.equ	Q		= 0b01010001
.equ	R		= 0b01010010
.equ	S		= 0b01010011
.equ	T		= 0b01010100
.equ	U		= 0b01010101
.equ	V		= 0b01010110
.equ	W		= 0b01010111
; X,Y,Z and are reserved in the 8515 inc file, so I give them alternate names
.equ	XX		= 0b01011000
.equ	YY		= 0b01011001
.equ	ZZ		= 0b01011010
.equ	newline		= 0b11000000
.equ	space		= 0b00100000
