//---- crttiny120.s 
// Number of interrupt vectors = 12
// Number of globals = 0

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

	rjmp  	Z_main
