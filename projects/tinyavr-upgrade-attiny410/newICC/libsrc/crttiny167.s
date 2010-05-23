//---- crttiny167.s 
// Number of interrupt vectors = 16
// Number of globals = 7

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
        reti
        reti
        reti

begin:
	ldi 	r30, 0x09
	ldi 	r20, 0x0
	ldi 	r21, 0x0
	ldi 	r22, 0x0
	ldi 	r23, 0x0
	ldi 	r24, 0x0
	ldi 	r25, 0x0
	ldi 	r26, 0x0

	rjmp  	Z_main
