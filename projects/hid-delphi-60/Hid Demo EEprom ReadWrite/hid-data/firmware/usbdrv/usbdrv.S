	.file	"usbdrv.c"
__SREG__ = 0x3f
__SP_H__ = 0x3e
__SP_L__ = 0x3d
__CCP__  = 0x34
__tmp_reg__ = 0
__zero_reg__ = 1
	.global __do_copy_data
	.global __do_clear_bss
	.text
.global	usbInit
	.type	usbInit, @function
usbInit:
/* prologue: function */
/* frame size = 0 */
	ldi r30,lo8(105)
	ldi r31,hi8(105)
	ld r24,Z
	ori r24,lo8(2)
	st Z,r24
	sbi 61-32,0
	ldi r24,lo8(75)
	sts usbTxStatus1+1,r24
	ldi r24,lo8(90)
	sts usbTxStatus1,r24
/* epilogue start */
	ret
	.size	usbInit, .-usbInit
.global	usbSetInterrupt
	.type	usbSetInterrupt, @function
usbSetInterrupt:
	push r17
/* prologue: function */
/* frame size = 0 */
	movw r26,r24
	mov r17,r22
	lds r24,usbTxStatus1
	sbrs r24,4
	rjmp .L4
	lds r24,usbTxStatus1+1
	ldi r25,lo8(-120)
	eor r24,r25
	sts usbTxStatus1+1,r24
	rjmp .L5
.L4:
	ldi r24,lo8(90)
	sts usbTxStatus1,r24
.L5:
	mov r25,r17
	ldi r30,lo8(usbTxStatus1+2)
	ldi r31,hi8(usbTxStatus1+2)
.L6:
	ld r24,X+
	st Z+,r24
	subi r25,lo8(-(-1))
	cp __zero_reg__,r25
	brlt .L6
	mov r22,r17
	ldi r24,lo8(usbTxStatus1+2)
	ldi r25,hi8(usbTxStatus1+2)
	call usbCrc16Append
	subi r17,lo8(-(4))
	sts usbTxStatus1,r17
/* epilogue start */
	pop r17
	ret
	.size	usbSetInterrupt, .-usbSetInterrupt
.global	usbPoll
	.type	usbPoll, @function
usbPoll:
	push r17
	push r28
	push r29
/* prologue: function */
/* frame size = 0 */
	lds r22,usbRxLen
	subi r22,lo8(-(-3))
	sbrc r22,7
	rjmp .L10
	lds r24,usbInputBufOffset
	ldi r28,lo8(12)
	ldi r29,hi8(12)
	sub r28,r24
	sbc r29,__zero_reg__
	subi r28,lo8(-(usbRxBuf))
	sbci r29,hi8(-(usbRxBuf))
	lds r24,usbRxToken
	cpi r24,lo8(45)
	breq .+2
	rjmp .L11
	cpi r22,lo8(8)
	breq .+2
	rjmp .L12
	ldi r24,lo8(-61)
	sts usbTxBuf,r24
	ldi r24,lo8(90)
	sts usbTxLen,r24
	sts usbMsgFlags,__zero_reg__
	ld r24,Y
	andi r24,lo8(96)
	breq .L13
	movw r24,r28
	call usbFunctionSetup
	mov r18,r24
	cpi r24,lo8(-1)
	brne .+2
	rjmp .L14
	rjmp .L15
.L13:
	ldd r25,Y+2
	sts usbTxBuf+9,__zero_reg__
	ldd r24,Y+1
	tst r24
	brne .L16
	sts usbTxBuf+10,__zero_reg__
	ldi r18,lo8(2)
	rjmp .L54
.L16:
	cpi r24,lo8(5)
	brne .L18
	sts usbNewDeviceAddr,r25
	rjmp .L56
.L18:
	cpi r24,lo8(6)
	breq .+2
	rjmp .L19
	ldd r24,Y+3
	cpi r24,lo8(1)
	brne .L20
	ldi r24,lo8(usbDescriptorDevice)
	ldi r25,hi8(usbDescriptorDevice)
	rjmp .L53
.L20:
	cpi r24,lo8(2)
	brne .L22
	ldi r24,lo8(usbDescriptorConfiguration)
	ldi r25,hi8(usbDescriptorConfiguration)
	sts (usbMsgPtr)+1,r25
	sts usbMsgPtr,r24
	ldi r18,lo8(34)
	rjmp .L21
.L22:
	cpi r24,lo8(3)
	brne .L23
	ldd r24,Y+2
	tst r24
	brne .L24
	ldi r24,lo8(usbDescriptorString0)
	ldi r25,hi8(usbDescriptorString0)
	sts (usbMsgPtr)+1,r25
	sts usbMsgPtr,r24
	ldi r18,lo8(4)
	rjmp .L21
.L24:
	cpi r24,lo8(1)
	brne .L25
	ldi r24,lo8(usbDescriptorStringVendor)
	ldi r25,hi8(usbDescriptorStringVendor)
.L53:
	sts (usbMsgPtr)+1,r25
	sts usbMsgPtr,r24
	ldi r18,lo8(18)
	rjmp .L21
.L25:
	cpi r24,lo8(2)
	brne .L26
	ldi r24,lo8(usbDescriptorStringDevice)
	ldi r25,hi8(usbDescriptorStringDevice)
	sts (usbMsgPtr)+1,r25
	sts usbMsgPtr,r24
	ldi r18,lo8(20)
	rjmp .L21
.L23:
	cpi r24,lo8(33)
	brne .L27
	ldi r24,lo8(usbDescriptorConfiguration+18)
	ldi r25,hi8(usbDescriptorConfiguration+18)
	sts (usbMsgPtr)+1,r25
	sts usbMsgPtr,r24
	ldi r18,lo8(9)
	rjmp .L21
.L27:
	cpi r24,lo8(34)
	brne .L26
	ldi r24,lo8(usbDescriptorHidReport)
	ldi r25,hi8(usbDescriptorHidReport)
	sts (usbMsgPtr)+1,r25
	sts usbMsgPtr,r24
	ldi r18,lo8(22)
	rjmp .L21
.L26:
	ldi r18,lo8(0)
.L21:
	ldi r24,lo8(64)
	sts usbMsgFlags,r24
	rjmp .L15
.L19:
	cpi r24,lo8(8)
	brne .L28
	ldi r18,lo8(1)
	ldi r24,lo8(usbConfiguration)
	ldi r25,hi8(usbConfiguration)
	rjmp .L17
.L28:
	cpi r24,lo8(9)
	brne .L29
	sts usbConfiguration,r25
	rjmp .L56
.L29:
	cpi r24,lo8(10)
	brne .L30
	ldi r18,lo8(1)
	rjmp .L54
.L30:
	cpi r24,lo8(11)
	brne .L56
.L31:
	ldi r24,lo8(75)
	sts usbTxStatus1+1,r24
.L56:
	ldi r18,lo8(0)
.L54:
	ldi r24,lo8(usbTxBuf+9)
	ldi r25,hi8(usbTxBuf+9)
.L17:
	sts (usbMsgPtr)+1,r25
	sts usbMsgPtr,r24
	rjmp .L15
.L14:
	ld r24,Y
	sbrc r24,7
	ldd r18,Y+6
.L32:
	ldi r24,lo8(-128)
	sts usbMsgFlags,r24
	rjmp .L34
.L15:
	ldd r24,Y+7
	tst r24
	brne .L34
	ldd r24,Y+6
	cp r24,r18
	brlo .L33
.L34:
	mov r24,r18
.L33:
	sts usbMsgLen,r24
	rjmp .L12
.L11:
	lds r24,usbMsgFlags
	sbrs r24,7
	rjmp .L12
	movw r24,r28
	call usbFunctionWrite
	cpi r24,lo8(-1)
	brne .L35
	ldi r24,lo8(30)
	sts usbTxLen,r24
	rjmp .L12
.L35:
	tst r24
	breq .L12
	sts usbMsgLen,__zero_reg__
.L12:
	sts usbRxLen,__zero_reg__
.L10:
	lds r24,usbTxLen
	sbrs r24,4
	rjmp .L36
	lds r24,usbMsgLen
	cpi r24,lo8(-1)
	brne .+2
	rjmp .L36
	mov r17,r24
	cpi r24,lo8(9)
	brlo .L37
	ldi r17,lo8(8)
.L37:
	sub r24,r17
	sts usbMsgLen,r24
	lds r24,usbTxBuf
	ldi r25,lo8(-120)
	eor r24,r25
	sts usbTxBuf,r24
	tst r17
	breq .L38
	lds r24,usbMsgFlags
	sbrs r24,7
	rjmp .L39
	mov r22,r17
	ldi r24,lo8(usbTxBuf+1)
	ldi r25,hi8(usbTxBuf+1)
	call usbFunctionRead
	mov r17,r24
	cpi r24,lo8(9)
	brsh .L52
	rjmp .L38
.L39:
	lds r18,usbMsgPtr
	lds r19,(usbMsgPtr)+1
	sbrs r24,6
	rjmp .L41
	ldi r26,lo8(usbTxBuf+1)
	ldi r27,hi8(usbTxBuf+1)
	ldi r24,lo8(0)
	ldi r25,hi8(0)
.L42:
	movw r30,r18
	add r30,r24
	adc r31,r25
/* #APP */
 ;  504 "usbdrv/usbdrv.c" 1
	lpm r30, Z
	
 ;  0 "" 2
/* #NOAPP */
	st X+,r30
	adiw r24,1
	cp r17,r24
	brne .L42
	rjmp .L55
.L41:
	mov r25,r17
	movw r26,r18
	ldi r30,lo8(usbTxBuf+1)
	ldi r31,hi8(usbTxBuf+1)
.L44:
	ld r24,X+
	st Z+,r24
	subi r25,lo8(-(-1))
	brne .L44
.L55:
	subi r17,lo8(-(-1))
	mov r24,r17
	ldi r25,lo8(0)
	subi r17,lo8(-(1))
	adiw r24,1
	add r24,r18
	adc r25,r19
	sts (usbMsgPtr)+1,r25
	sts usbMsgPtr,r24
.L38:
	mov r22,r17
	ldi r24,lo8(usbTxBuf+1)
	ldi r25,hi8(usbTxBuf+1)
	call usbCrc16Append
	mov r22,r17
	subi r22,lo8(-(4))
	cpi r22,lo8(12)
	breq .L45
	ldi r24,lo8(-1)
	sts usbMsgLen,r24
	rjmp .L45
.L52:
	ldi r24,lo8(-1)
	sts usbMsgLen,r24
	ldi r22,lo8(30)
.L45:
	sts usbTxLen,r22
.L36:
	ldi r25,lo8(20)
.L47:
	in r24,41-32
	andi r24,lo8(12)
	brne .L48
	subi r25,lo8(-(-1))
	brne .L47
	sts usbNewDeviceAddr,__zero_reg__
	sts usbDeviceAddr,__zero_reg__
.L48:
/* epilogue start */
	pop r29
	pop r28
	pop r17
	ret
	.size	usbPoll, .-usbPoll
.global	usbTxLen
	.data
	.type	usbTxLen, @object
	.size	usbTxLen, 1
usbTxLen:
	.byte	90
.global	usbDescriptorString0
	.section	.progmem.data,"a",@progbits
	.type	usbDescriptorString0, @object
	.size	usbDescriptorString0, 4
usbDescriptorString0:
	.byte	4
	.byte	3
	.byte	9
	.byte	4
.global	usbDescriptorStringVendor
	.type	usbDescriptorStringVendor, @object
	.size	usbDescriptorStringVendor, 18
usbDescriptorStringVendor:
	.word	786
	.word	111
	.word	98
	.word	100
	.word	101
	.word	118
	.word	46
	.word	97
	.word	116
.global	usbDescriptorStringDevice
	.type	usbDescriptorStringDevice, @object
	.size	usbDescriptorStringDevice, 20
usbDescriptorStringDevice:
	.word	788
	.word	68
	.word	97
	.word	116
	.word	97
	.word	83
	.word	116
	.word	111
	.word	114
	.word	101
.global	usbDescriptorDevice
	.type	usbDescriptorDevice, @object
	.size	usbDescriptorDevice, 18
usbDescriptorDevice:
	.byte	18
	.byte	1
	.byte	16
	.byte	1
	.byte	0
	.byte	0
	.byte	0
	.byte	8
	.byte	-64
	.byte	22
	.byte	-33
	.byte	5
	.byte	0
	.byte	1
	.byte	1
	.byte	2
	.byte	0
	.byte	1
.global	usbDescriptorConfiguration
	.type	usbDescriptorConfiguration, @object
	.size	usbDescriptorConfiguration, 34
usbDescriptorConfiguration:
	.byte	9
	.byte	2
	.byte	34
	.byte	0
	.byte	1
	.byte	1
	.byte	0
	.byte	-128
	.byte	10
	.byte	9
	.byte	4
	.byte	0
	.byte	0
	.byte	1
	.byte	3
	.byte	0
	.byte	0
	.byte	0
	.byte	9
	.byte	33
	.byte	1
	.byte	1
	.byte	0
	.byte	1
	.byte	34
	.byte	22
	.byte	0
	.byte	7
	.byte	5
	.byte	-127
	.byte	3
	.byte	8
	.byte	0
	.byte	100
	.data
	.type	usbMsgLen, @object
	.size	usbMsgLen, 1
usbMsgLen:
	.byte	-1
	.lcomm usbMsgFlags,1
	.comm usbMsgPtr,2,1
	.comm usbRxToken,1,1
	.comm usbConfiguration,1,1
	.comm usbTxStatus1,12,1
	.comm usbRxBuf,22,1
	.comm usbInputBufOffset,1,1
	.comm usbDeviceAddr,1,1
	.comm usbNewDeviceAddr,1,1
	.comm usbRxLen,1,1
	.comm usbCurrentTok,1,1
	.comm usbTxBuf,11,1
