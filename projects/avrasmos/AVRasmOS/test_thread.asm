.EQU	ON  =1
.EQU	OFF =0

.MACRO	LAMP_RED
	.IF @0==ON
	sbi	PORTC,4		;включить первую
 	.ELIF @0==OFF
	cbi	PORTC,4		;выключить первую
	.ENDIF
.ENDMACRO

.MACRO	LAMP_GREEN
	.IF @0==ON
	sbi	PORTD,4		;включить первую
 	.ELIF @0==OFF
	cbi	PORTD,4		;выключить первую
	.ENDIF
.ENDMACRO

.CSEG
TEST_THREAD1:
//тут все прописано для макетной платы с mega16
	sbi	DDRC,7		;включить УАРТ в правильную сторону
	sbi	PORTC,7

	sbi	DDRD,4		;вывод первой лампочки
	sbi	DDRC,4		;вывод второй лампочки
	LAMP_GREEN	OFF
	____EVENT_SET LAMP_EVENT

rep_pop:
/*	ldi	r24,0x00
	ldi	r25,0x00
	____EVENT_WAIT	LAMP_EVENT
	____EVENT_CLEAR LAMP_EVENT
*/
	LAMP_RED	ON
	____THREAD_WAIT	500000; ждем 0,5 секунды
	LAMP_RED	OFF
	____THREAD_WAIT	500000; ждем 0,5 секунды

	rjmp	rep_pop	

	____EVENT_SET LAMP_EVENT

	____THREAD_WAIT	1000; ждем 1 милисекунду


	rjmp	rep_pop	
//--------------------------------------




TEST_THREAD0:

	ldi	r24,0x00
	ldi	r25,0x00
	____EVENT_WAIT	LAMP_EVENT
	____EVENT_CLEAR LAMP_EVENT
	LAMP_GREEN	ON
	ldi	r24,0x00
	lds	r25,ADR_SYSTEM_TIME
	andi	r25,0b00111111
	ori	r25,0b00000001
	RCALL	____OS_THREAD_WAIT
	LAMP_GREEN	OFF
	____EVENT_SET	LAMP_EVENT
	____THREAD_WAIT	1000; ждем 1 милисекунду
	
	rjmp 	TEST_THREAD0


string:
.DB 	"Hello! Device working right! It is test string",0x0d,0x0a,0x00

TEST_THREAD_WRITE:
	ldi	Xh,high(string<<1)
	ldi	Xl, low(string<<1)
	ldi	r18,0x00		
qweqwe:
	cli	
	movw	Zh:Zl,Xh:Xl
	lpm	r24,Z
	sei
	cpi	r24,0x00
	breq	qweqweqwe
	movw	r17:r16,Xh:Xl
	____QUEUE_PUSH_BACK TEST_QUEUE
	movw	Xh:Xl,r17:r16
	adiw	Xh:Xl,1
	inc	r18
;	cpi	r18,30
;	breq	qweqweqwe
	rjmp	qweqwe
qweqweqwe:		
	____QUEUE_ATTACH QUEUE_UART_TX,TEST_QUEUE
	____EVENT_SET EVENT_UART_TX	
//	____QUEUE_ATTACH QUEUE_RADIO_TX,TEST_QUEUE
//	____EVENT_SET EVENT_RADIO_TX	

/*	ldi	r18,0x00
zxc:
	____QUEUE_POP_FRONT UART_OUTPUT_QUEUE
	inc	r18
	rjmp	zxc	
*/

TEST_THREAD2:
	ldi	r24,0xFF
	ldi	r25,0xFF
	RCALL	____OS_THREAD_WAIT
	rjmp	TEST_THREAD2
.EXIT

TEST_THREAD:
	ldi	Yh,high(steps)
	ldi	Yl, low(steps)
	mov	r16,REG_EXEC_THREAD_NUM
	lsl	r16
	lsl	r16
	add	Yl,r16
	brcc	PC+2	
	inc 	Yh

	ldi	r16,0x00
	st	Y,r16		;2
	std	Y+1,r16		;2
	std	Y+2,r16		;2
	std	Y+3,r16		;2
	
TEST_THREAD_rep:	
	ldd	Xl,Y+3		;2
	ldd	Xh,Y+2		;2
	adiw	Xh:Xl,23	;2
	std	Y+2,Xh		;2
	std	Y+3,Xl		;2

	brcc	only_low	;1/2

	ldd	Xl,Y+1		;2
	ld	Xh,Y		;2
	adiw	Xh:Xl,1		;2
	std	Y+1,Xl		;2
	st	Y,Xh		;2
	rjmp	TEST_THREAD_rep	;2   23

only_low:			;12
	nop			;1
	nop			;1
	nop			;1
	nop			;1
	nop			;1
	nop			;1
	nop			;1
	nop			;1
	nop			;1
	rjmp	TEST_THREAD_rep	;2    23
