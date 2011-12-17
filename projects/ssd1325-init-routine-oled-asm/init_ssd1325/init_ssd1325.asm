initSSD:ldi	temp, 0xaf			;Set display On
	rcall	wrtCmmd				;0xaf = On, 0xae = Sleep Mode

	ldi	temp, 0xb3			;Set clock divide
	rcall	wrtCmmd
	ldi	temp, 0x02			;[3:0]:1~16, [7:4]:0~16, 70Hz
	rcall	wrtCmmd

	ldi	temp, 0xa8			;Set multiplex ratio
	rcall	wrtCmmd
	ldi	temp, 0x3f			;0x3f=64 rows, 0x4f=80 rows
	rcall	wrtCmmd

	ldi	temp, 0xa2			;Set display offset
	rcall	wrtCmmd
	ldi	temp, 0x4c			;No offset (0x4c)
	rcall	wrtCmmd

	ldi	temp, 0xa1			;Set display start line
	rcall	wrtCmmd
	ldi	temp, 0x00			;Top
	rcall	wrtCmmd

	ldi	temp, 0xa0			;Set re-map
	rcall	wrtCmmd
	ldi	temp, 0b01010010		;Nibble=1, Com=1, Com Split Odd/Even=1
	rcall	wrtCmmd

	ldi	temp, 0xad			;Set dc-dc
	rcall	wrtCmmd
	ldi	temp, 0x03			;03=On, 02=Off
	rcall	wrtCmmd

	ldi	temp, 0xcf			;Set dc-dc bias
	rcall	wrtCmmd
	ldi	temp, 0xf0			;
	rcall	wrtCmmd

	ldi	temp, 0x86			;Set current
	rcall	wrtCmmd				;84h: Qrtr 85h: Half 86h: Full
	
	ldi	temp, 0x81			;Set contrast control
	rcall	wrtCmmd
	ldi	temp, 100			;0 ~ 127
	rcall	wrtCmmd

	ldi	temp, 0xb2			;Set row period
	rcall	wrtCmmd
	ldi	temp, 0x25			;18~255, K=P1+P2+GS15
	rcall	wrtCmmd

	ldi	temp, 0xb1			;Set phase lenght
	rcall	wrtCmmd
	ldi	temp, 0x53			;
	rcall	wrtCmmd

	ldi	temp, 0xbc			;Set VP
	rcall	wrtCmmd
	ldi	temp, 0x10			;[7:0]:VP, (0.67*14V=9.38V)
	rcall	wrtCmmd

	ldi	temp, 0xbe			;Set VCOMH
	rcall	wrtCmmd
	ldi	temp, 0x02			;[7:0]:VCOMH, (0.51*14V=7.14V)
	rcall	wrtCmmd

	ldi	temp, 0xbf			;Set VSL
	rcall	wrtCmmd
	ldi	temp, 0x0d			;[3:0]:VSL
	rcall	wrtCmmd

	ldi	temp, 0xa4			;Display mode = normal
;	ldi	temp, 0xa5			;Display mode = all on
;	ldi	temp, 0xa6			;Display mode = blank
;	ldi	temp, 0xa7			;Display mode = inverse
	rcall	wrtCmmd

	ldi	temp, 0x23			;Enable rectangle mode
	rcall	wrtCmmd
	ldi	temp, 0b00000001		;Filled rectangle
	rcall	wrtCmmd
	ldi	temp, 0x24			;Draw rectangle (cls)
	rcall	wrtCmmd
	ldi	temp, 0				;Set x coord (column)
	rcall	wrtCmmd
	ldi	temp, 0				;Set y coord (row)
	rcall	wrtCmmd
	ldi	temp, 63			;Set x1 coord (column)
	rcall	wrtCmmd
	ldi	temp, 63			;Set y1 coord (row)
	rcall	wrtCmmd
	ldi	temp, 0b00000000		;Set gamma (00)
	rcall	wrtCmmd

	ldi	temp, 0xaf			;Set display On
	rcall	wrtCmmd				;0xaf = On, 0xae = Sleep Mode

; <let SSD settle for 100 msec>