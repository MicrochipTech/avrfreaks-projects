	.file	"main.c"
__SREG__ = 0x3f
__SP_H__ = 0x3e
__SP_L__ = 0x3d
__CCP__  = 0x34
__tmp_reg__ = 0
__zero_reg__ = 1
	.global __do_copy_data
	.global __do_clear_bss
	.text
	.type	eeprom_read_byte, @function
eeprom_read_byte:
/* prologue: function */
/* frame size = 0 */
	movw r18,r24
.L2:
	sbic 63-32,1
	rjmp .L2
	out (65)+1-32,r19
	out 65-32,r18
	sbi 63-32,0
	in r24,64-32
/* epilogue start */
	ret
	.size	eeprom_read_byte, .-eeprom_read_byte
	.type	eeprom_write_byte, @function
eeprom_write_byte:
/* prologue: function */
/* frame size = 0 */
	movw r18,r24
.L6:
	sbic 63-32,1
	rjmp .L6
	out 63-32,__zero_reg__
	out (65)+1-32,r19
	out 65-32,r18
	out 64-32,r22
/* #APP */
 ;  252 "c:/progra~1/winavr/lib/gcc/../../avr/include/avr/eeprom.h" 1
	/* START EEPROM WRITE CRITICAL SECTION */
	in	r0, 63		
	cli				
	sbi	31, 2	
	sbi	31, 1	
	out	63, r0		
	/* END EEPROM WRITE CRITICAL SECTION */
 ;  0 "" 2
/* epilogue start */
/* #NOAPP */
	ret
	.size	eeprom_write_byte, .-eeprom_write_byte
.global	usbFunctionSetup
	.type	usbFunctionSetup, @function
usbFunctionSetup:
/* prologue: function */
/* frame size = 0 */
	movw r30,r24
	ld r24,Z
	andi r24,lo8(96)
	cpi r24,lo8(32)
	brne .L10
	ldd r24,Z+1
	cpi r24,lo8(1)
	breq .L14
.L11:
	cpi r24,lo8(9)
	brne .L10
.L14:
	ldi r24,lo8(-128)
	sts bytesRemaining,r24
	sts currentAddress,__zero_reg__
	ldi r24,lo8(-1)
	ret
.L10:
	ldi r24,lo8(0)
	ret
	.size	usbFunctionSetup, .-usbFunctionSetup
.global	main
	.type	main, @function
main:
/* prologue: function */
/* frame size = 0 */
	ldi r18,lo8(14)
	ldi r24,lo8(24)
	ldi r25,hi8(24)
/* #APP */
 ;  113 "main.c" 1
	in __tmp_reg__,__SREG__
	cli
	wdr
	sts 96,r24
	out __SREG__,__tmp_reg__
	sts 96,r18
 ;  0 "" 2
/* #NOAPP */
	call usbInit
	sbi 42-32,3
	ldi r18,lo8(0)
	ldi r20,lo8(4000)
	ldi r21,hi8(4000)
	rjmp .L16
.L17:
/* #APP */
 ;  127 "main.c" 1
	wdr
 ;  0 "" 2
/* #NOAPP */
	movw r24,r20
/* #APP */
 ;  105 "c:/progra~1/winavr/lib/gcc/../../avr/include/util/delay_basic.h" 1
	1: sbiw r24,1
	brne 1b
 ;  0 "" 2
/* #NOAPP */
.L16:
	subi r18,lo8(-(-1))
	brne .L17
	cbi 42-32,3
/* #APP */
 ;  131 "main.c" 1
	sei
 ;  0 "" 2
/* #NOAPP */
.L18:
/* #APP */
 ;  135 "main.c" 1
	wdr
 ;  0 "" 2
/* #NOAPP */
	call usbPoll
	rjmp .L18
	.size	main, .-main
.global	usbFunctionWrite
	.type	usbFunctionWrite, @function
usbFunctionWrite:
	push r17
/* prologue: function */
/* frame size = 0 */
	lds r18,bytesRemaining
	tst r18
	breq .L26
.L21:
	mov r17,r22
	cp r18,r22
	brsh .L23
	mov r17,r18
.L23:
	ldi r18,lo8(gs(eeprom_write_byte))
	ldi r19,hi8(gs(eeprom_write_byte))
	mov r20,r17
	ldi r21,lo8(0)
	movw r22,r24
	lds r24,currentAddress
	ldi r25,lo8(0)
	call __eewr_block
	lds r24,currentAddress
	add r24,r17
	sts currentAddress,r24
	lds r24,bytesRemaining
	sub r24,r17
	sts bytesRemaining,r24
	ldi r25,lo8(0)
	tst r24
	brne .L22
.L26:
	ldi r25,lo8(1)
.L22:
	mov r24,r25
/* epilogue start */
	pop r17
	ret
	.size	usbFunctionWrite, .-usbFunctionWrite
.global	usbFunctionRead
	.type	usbFunctionRead, @function
usbFunctionRead:
	push r17
/* prologue: function */
/* frame size = 0 */
	lds r18,bytesRemaining
	mov r17,r22
	cp r18,r22
	brsh .L28
	mov r17,r18
.L28:
	lds r22,currentAddress
	ldi r18,lo8(gs(eeprom_read_byte))
	ldi r19,hi8(gs(eeprom_read_byte))
	mov r20,r17
	ldi r21,lo8(0)
	ldi r23,lo8(0)
	call __eerd_block
	lds r24,currentAddress
	add r24,r17
	sts currentAddress,r24
	lds r24,bytesRemaining
	sub r24,r17
	sts bytesRemaining,r24
	mov r24,r17
/* epilogue start */
	pop r17
	ret
	.size	usbFunctionRead, .-usbFunctionRead
.global	usbDescriptorHidReport
	.section	.progmem.data,"a",@progbits
	.type	usbDescriptorHidReport, @object
	.size	usbDescriptorHidReport, 22
usbDescriptorHidReport:
	.byte	6
	.byte	0
	.byte	-1
	.byte	9
	.byte	1
	.byte	-95
	.byte	1
	.byte	21
	.byte	0
	.byte	38
	.byte	-1
	.byte	0
	.byte	117
	.byte	8
	.byte	-107
	.byte	-128
	.byte	9
	.byte	0
	.byte	-78
	.byte	2
	.byte	1
	.byte	-64
	.lcomm currentAddress,1
	.lcomm bytesRemaining,1
