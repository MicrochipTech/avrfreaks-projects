//---- crttiny133.s 
// Number of interrupt vectors = 13
// Number of globals = 3

.extern		"Z_main"

.text
	rjmp	begin

	reti
	reti
	reti
	reti
	reti
	reti
	reti
	reti
	reti
        reti
	reti
        reti
        reti

begin:
	ldi 	r30, 0x09
	ldi 	r20, 0x0
	ldi 	r21, 0x0
	ldi 	r22, 0x0

	rjmp  	Z_main
