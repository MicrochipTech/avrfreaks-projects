;*************************************************************************
;-------------------------------------------------------------------------
;				Constant definitions
;-------------------------------------------------------------------------
.equ	xtal		= 8000000	;CPU-Clock 8 MHz (max 11 MHz)
;----------------------------------------
; DS2408 ROM-Commands
.equ	READ_ROM	= 0x33
.equ	MATCH_ROM	= 0x55
.equ	SKIP_ROM	= 0xCC
.equ	SEARCH_ROM	= 0xF0
.equ	COND_SEARCH	= 0xEC
.equ	OVERDR_MATCH= 0x69
.equ	OVERDR_SKIP	= 0x3C
.equ	RESUME		= 0xA5
;-----------------------------------------
;DS2408 Function Commands
.equ	READ_PIO		= 0xF0	;Read PIO Registers
.equ	CHANN_ACC_READ	= 0xF5	;Read from Port Pins
.equ	CHANN_ACC_WRITE = 0x5A	;Write to Port Pins
.equ	WR_COND_SEARCH	= 0xCC	;Write to Conditional Search Registers
.equ	RES_ACT_LATCHES	= 0xC3	;Reset Activity Latches
;------------------------------------------
;DS2408 Registers
.equ	PIO_LOGIC_STATE		= 0x88	;I/O Pin Status
.equ	PIO_OUTPUT_LATCH	= 0x89	;Output-Register
.equ	PIO_ACTIVITY_LATCH	= 0x8A	;Input Register
.equ	COND_SEL_MASK		= 0x8B	;For Conditional Search
.equ	COND_POL_MASK		= 0x8C	;For Conditional Search
.equ	STATUS_REGISTER		= 0x8D	;Control/Status Register
;------------------------------------------
;DS2408 Status register Bits
.equ	PLS	= 0
.equ	CT	= 1
.equ	ROS	= 2
.equ	PORL= 3
.equ	VCCP= 4
;------------------------------------------
;1Wire Search Constants
.equ	SEARCH_FIRST	= 0xFF	; start new search
.equ	PRESENCE_ERR	= 0xFE
.equ	DATA_ERR		= 0xFE
.equ	LAST_DEVICE		= 0x00
						; 0x01 ... 0x7F: continue searching
.equ ROMCODE_SIZE = 8	; 8 Bytes of ROMCode 

;-------------------------------------------------------------------------
;				Using registers
;-------------------------------------------------------------------------
.def	Save    = r9		;Save Place for some Data
.def	a0		= r17		;working registers
.def	a1		= r18
.def	a2		= r19		
.def	a3		= r20
.def	Data	= r21		;Data output to DS2408
.def	Temp	= r22		;temporary Storage

;-------------------------------------------------------------------------
;				Port definitions
;-------------------------------------------------------------------------
.equ	w1_out		= PORTB
.equ	w1_ddr		= DDRB
.equ	w1_in		= PINB
.equ	w1_pin		= PB4		;1-wire data line
.equ	led_ddr		= DDRB
.equ	led_port	= PORTB
.equ	led_pin		= PB3		;Error-LED
.equ	led_on		= 0
.equ	led_off		= 8
;-------------------------------------------------------------------------
;              LCD Connection
;-------------------------------------------------------------------------
;   LCD-Pin.....connect to (LCD Power and Contrast not shown)  
;    |             |
;	RS   -> 	DS2408, Pin 11 (P4) 
;	R/W  -> 	GND
;	E    -> 	DS2408, Pin 10 (RSTZ) used as Strobe
;	DB4  -> 	DS2408, Pin  2 (P0)
;	DB5  -> 	DS2408, Pin 14 (P1)
;	DB6  -> 	DS2408, Pin 13 (P2)
;	DB7  -> 	DS2408, Pin 12 (P3)
;	DB0  ->     GND (or not connected)
;	DB1  ->     GND (or not connected)
;	DB2  ->     GND (or not connected)
;	DB3  ->     GND (or not connected)
;
; ********* Note : all above DS2408-Pins MUST be connected  ******
; *********        to 10 kOhm Pullup-Resistors              ******
; *********        PB4 (1-Wire Data Line) is connected      ******
; *********        to a 2k2 Pullup-Resistor                 ******
