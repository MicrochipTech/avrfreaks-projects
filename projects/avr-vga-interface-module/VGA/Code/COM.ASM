RJMP FIRST_START

COMMAND_MODE:
CBI PORTD,4		
SBI PORTD,4 		;clear pulse to the busy FF

FIRST_START:
LDI R16,0b00000000
OUT DDRB,R16		;SET PORTB AS INPUT
CBI PORTD,0		;enable command register output

WAIT_COMMAND:
SBIS PIND,3		;wait until a byte is received
RJMP WAIT_COMMAND
SBIC PIND,1		;check if the incoming byte is command
RJMP COMMAND_MODE

LDI XL,0x00
LDI XH,0x01
IN R16,PINB		;load command into R16
ANDI R16,0b00111111
ST X+,R16
MOV R19,R16
ANDI R19,0xf0
SWAP R19		;R19 contain group number

;load data counter in R20
LDI ZL,0x40
LDI ZH,0x0f
ADD ZL,R16
LPM R20,Z	; data counter in R20
CPI R20,0
BREQ EXECUTION_MODE

WAIT_DATA1:
CBI PORTD,4		
SBI PORTD,4 		;clear pulse to the busy FF
NOP

WAIT_DATA:
SBIS PIND,3		;wait until a byte is received
RJMP WAIT_DATA
SBIS PIND,1		;check if the incoming byte is data
RJMP WAIT_COMMAND

IN R16,PINB	
ST X+,R16

DEC R20
BRNE WAIT_DATA1