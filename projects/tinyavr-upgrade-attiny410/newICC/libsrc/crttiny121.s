//---- crttiny121.s 
// Number of interrupt vectors = 12
// Number of globals = 1

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

begin:
	ldi 	r30, 0x09
	ldi 	r20, 0x0

	rjmp  	Z_main
