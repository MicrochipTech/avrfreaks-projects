.INCLUDE "m16def.inc"

.org SMALLBOOTSTART
ldi r16,0xff
out portc,r16
nop
nop
nop

sbic pinc,6
jmp 0x0			;go to reset
jmp 0x16d0		;go to stk500 word address 
