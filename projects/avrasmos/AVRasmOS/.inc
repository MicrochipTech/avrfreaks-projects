#ifdef	THREAD_0
	#undef	THREAD_AMOUNT	
	#define	THREAD_AMOUNT 1
#endif
#ifdef	THREAD_1
	#undef	THREAD_AMOUNT	
	#define	THREAD_AMOUNT 2
#endif
#ifdef	THREAD_2
	#undef	THREAD_AMOUNT	
	#define	THREAD_AMOUNT 3
#endif
#ifdef	THREAD_3
	#undef	THREAD_AMOUNT	
	#define	THREAD_AMOUNT 4
#endif
#ifdef	THREAD_4
	#undef	THREAD_AMOUNT	
	#define	THREAD_AMOUNT 5
#endif
#ifdef	THREAD_5
	#undef	THREAD_AMOUNT	
	#define	THREAD_AMOUNT 6
#endif
#ifdef	THREAD_6
	#undef	THREAD_AMOUNT	
	#define	THREAD_AMOUNT 7
#endif
#ifdef	THREAD_7
	#undef	THREAD_AMOUNT	
	#define	THREAD_AMOUNT 8
#endif

.EQU	CONST_THREAD_CONTEXT_SIZE	=14
;*********************************************
;------------- MACROS for asmOS --------------
.MACRO	____OS_THREAD_STORE
	push	r0
	push	r1
	push	r16
	push	r17
	push	r18
	push	r19
	push	r20
	push	r21
	;push	r22	;reserved for REG_INT_TEMP
	;push	r23	;reserved for REG_OS_TEMP
	push	r24	
	push	r25
	push	r26
	push	r27
	push	r28
	push	r29

.ENDMACRO

.MACRO	____OS_THREAD_RESTORE
	pop	r29
	pop	r28
	pop	r27
	pop	r26
	pop	r25
	pop	r24
	;pop	r23
	;pop	r22
	pop	r21
	pop	r20
	pop	r19
	pop	r18
	pop	r17
	pop	r16
	pop	r1
	pop	r0
.ENDMACRO
//---------------------------------------------

.MACRO	____ENTER_KERNEL_LEVEL		;возможно стек нужно перемещать
	____OS_TIMER_INT_DISABLE
	sei
	____OS_PUSH_KERNEL_REGS
.ENDMACRO

.MACRO	____ALIVE_KERNEL_LEVEL
	cli
	____OS_TIMER_INT_ENABLE
.ENDMACRO

;**********************************************
//---------------------------------------------
.MACRO	____OS_TIMER_INIT
	clr	REG_TIMER_LOW
	clr	REG_TIMER_HIGH	
	sts	ADR_SYSTEM_TIME,REG_TIMER_LOW
	sts	ADR_SYSTEM_TIME+1,REG_TIMER_LOW
	sts	ADR_THREADS_TIMEOUT_MASK,REG_TIMER_LOW
#if defined(__ATmega16__)
	ldi	REG_OS_TEMP,0b00001010	;CTC-mode & prescaler =8
	out	TCCR0,REG_OS_TEMP
	ldi	REG_OS_TEMP,CONST_TIMER_COMPARE
	out	OCR0,REG_OS_TEMP

#elif defined(__ATmega48__)||defined(__ATmega88__)||defined(__ATmega168__)
	ldi	REG_OS_TEMP,0b00000010	;CTC-mode
	out	TCCR0A,REG_OS_TEMP
	ldi	REG_OS_TEMP,0b00000010	;prescaler =8
	out	TCCR0B,REG_OS_TEMP
	ldi	REG_OS_TEMP,CONST_TIMER_COMPARE
	out	OCR0A,REG_OS_TEMP
#endif
.ENDMACRO
//---------------------------------------------
.MACRO	____OS_TIMER_INT_ENABLE
#if defined(__ATmega16__)
	in	REG_OS_TEMP,TIMSK
	sbr	REG_OS_TEMP,1<<OCIE0
	out	TIMSK,REG_OS_TEMP
#elif defined(__ATmega48__)||defined(__ATmega88__)||defined(__ATmega168__)
	lds	REG_OS_TEMP,TIMSK0
	sbr	REG_OS_TEMP,1<<OCIE0A
	sts	TIMSK0,REG_OS_TEMP
#endif
.ENDMACRO

//---------------------------------------------
.MACRO	____OS_TIMER_INT_DISABLE
#if defined(__ATmega16__)
	in	REG_OS_TEMP,TIMSK
	cbr	REG_OS_TEMP,1<<OCIE0
	out	TIMSK,REG_OS_TEMP
#elif defined(__ATmega48__)||defined(__ATmega88__)||defined(__ATmega168__)
	lds	REG_OS_TEMP,TIMSK0
	cbr	REG_OS_TEMP,1<<OCIE0A
	sts	TIMSK0,REG_OS_TEMP
#endif
.ENDMACRO

//---------------------------------------------
.MACRO	____OS_TIMER_CHECK_THREADS_TIMEOUT		//дополнительна€ оптимизаци€
	lds	REG_OS_TEMP,ADR_THREADS_TIMEOUT_MASK
	tst	REG_OS_TEMP
	breq	os_timer_no_event
;если прикинуть что все ждут таймеров, то лучше сразу все вычисл€ть - быстрее будет
//--------------
	ldi	Yh,high(ADR_TIMER_TABLE+CONST_THREAD_AMOUNT)
	ldi	Yl,low(ADR_TIMER_TABLE+CONST_THREAD_AMOUNT)
	ldi	REG_OS_TEMP,(1<<(CONST_THREAD_AMOUNT-1));сдвиговый дл€ перебора
	clr	REG_OS_1		;1 масочный
	ld	REG_OS_0,-Y		;2
	cp	REG_TIMER_LOW,REG_OS_0	;1	;не испортит флаги
	breq	os_local_test_start	;1
os_timer_test_start:
	lsr	REG_OS_TEMP		;1
	ld	REG_OS_0,-Y		;2
	cpse	REG_TIMER_LOW,REG_OS_0	;1	;не испортит флаги
	brne	os_timer_test_start	;2	6 тактов!!!
	breq	os_timer_test_end
os_local_test_start:			;+6
	ldd	REG_OS_0,Y+CONST_THREAD_AMOUNT;2 
	cp	REG_TIMER_HIGH,REG_OS_0	;1
	brne	os_local_test_end	;1			
	or	REG_OS_1,REG_OS_TEMP	;1	 
os_local_test_end:			
	lsr	REG_OS_TEMP
	brne	os_timer_test_start+1	;7/13 +экономим регистры
os_timer_test_end:			;5+ 49/103=54/108 (было 61/107 мин)
//--------------
	lds	REG_OS_TEMP,ADR_THREADS_TIMEOUT_MASK
	and	REG_OS_1,REG_OS_TEMP		;вычислили реальные
	eor	REG_OS_TEMP,REG_OS_1		;сборосили реальные
	sts	ADR_THREADS_TIMEOUT_MASK,REG_OS_TEMP
	or	REG_THREAD_MAP,REG_OS_1	;добавили в карту запуска
os_timer_no_event:			;5/11+(49/108)

.ENDMACRO
;**********************************************
//---------------------------------------------
.MACRO	____OS_EXEC_THREAD_GET_STACK_ADR
	ldi	Yh,high(ADR_CONTEXT_ADR_TABLE)
	lds	Yl,ADR_CURRENT_THREAD_CONTEXT
.ENDMACRO
//---------------------------------------------
.MACRO	____OS_PUSH_KERNEL_REGS
	sts	ADR_KERNEL_STACK+0,Yl
	sts	ADR_KERNEL_STACK+1,Yh
.ENDMACRO

.MACRO	____OS_POP_KERNEL_REGS
	lds	Yl,ADR_KERNEL_STACK+0
	lds	Yh,ADR_KERNEL_STACK+1
.ENDMACRO

//----------------------------------------
.MACRO 	____OS_SET_SLEEP_MODE

#if defined(__ATmega16__)
	in	REG_OS_TEMP,MCUCR
	andi	REG_OS_TEMP,0b00001111
	ori	REG_OS_TEMP,0b01000000
	out	MCUCR,REG_OS_TEMP
#elif defined(__ATmega48__)||defined(__ATmega88__)||defined(__ATmega168__)
	ldi	REG_OS_TEMP,0b00000001
	out	SMCR,REG_OS_TEMP
#endif

.ENDMACRO

.MACRO	____THREAD_WAIT	; @0 - количество микросекунд ожидани€
	ldi	r24,low (@0/SYSTEM_TICK+SYSTEM_TICK) 	; чтоб не повисло прибавим 1 тик
	ldi	r25,high(@0/SYSTEM_TICK+SYSTEM_TICK) 
	RCALL	____OS_THREAD_WAIT
.ENDMACRO

;****************************************
#ifdef	REG_FLAGS 
.MACRO	____FLAG_SET
	set
	bld	REG_FLAGS,@0
.ENDMACRO
.MACRO	____FLAG_CLEAR
	clt
	bld	REG_FLAGS,@0
.ENDMACRO
.MACRO	____FLAG_SKIP_IF_SET		;Next inctruction RJMP FLAG_is_CLEAR
	sbrs	REG_FLAGS,@0
.ENDMACRO

.MACRO	____FLAG_SKIP_IF_CLEAR		;Next inctruction RJMP FLAG_is_SET
	sbrc	REG_FLAGS,@0
.ENDMACRO
.MACRO	____FLAG_CHECK			;set carry according the flag
	clc
	sbrc	REG_FLAGS,@0
	sec
.ENDMACRO
#endif

.EXIT
/*********************************************************************************************/
/*
.MACRO 	ADDB
	subi	@0,((~@1)+1)
.ENDMACRO

.MACRO 	ADDW 
	subi	@1,((~low(@2))+1)
	sbci	@0,(~high(@2))
.ENDMACRO
*/
//**********************************************************
//*********************************************************
предпослен€€ вариаци€ на тему таймеров
	ldi	Yh,high(ADR_TIMER_TABLE)
	ldi	Yl,low(ADR_TIMER_TABLE)
	mov	os_reg_1,timer_wait_reg
	ldi	os_reg_0,0x01
timer_test_start:
	lsr	os_reg_1
	brcs	local_test_start
	breq	timer_test_end		
	lsl	os_reg_0		
	inc	Yl	
	rjmp	timer_test_start	;7
local_test_start:
	;3 такта
	ld	os_temp_reg,Y+		;2
	cp	os_timer0,os_temp_reg	;1
	brne	local_test_end		;2 экономим такты!!!
	ldd	os_temp_reg,Y+7 	;thread_count
	cp	os_timer1,os_temp_reg
	brne	local_test_end
	or	os_thread_map,os_reg_0	;1	 
	eor	timer_wait_reg,os_reg_0	;1 выбрасываем
local_test_end:				;8/11/12
	lsl	os_reg_0
	brne	timer_test_start		
timer_test_end:				;11/15/16
;было 10/15/18 стало 7/11/15/16  т.о.  если все ждут это вычислитьс€ всреднем за 8*11~=90тактов	

//*************************************************************
/****************************************************************/
	lds	REG_OS_TEMP,ADR_THREADS_TIMEOUT_MASK
	tst	REG_OS_TEMP
	breq	os_timer_no_event
;если прикинуть что все ждут таймеров, то лучше сразу все вычисл€ть - быстрее будет
//--------------
	ldi	Yh,high(ADR_TIMER_TABLE+CONST_THREAD_AMOUNT)
	ldi	Yl,low(ADR_TIMER_TABLE+CONST_THREAD_AMOUNT)
	ldi	REG_OS_TEMP,(1<<(CONST_THREAD_AMOUNT-1));сдвиговый дл€ перебора
	clr	REG_OS_1		;масочный
//--------------
os_timer_test_start:
	ld	REG_OS_0,-Y		;2
	cp	REG_TIMER_LOW,REG_OS_0	;1
	breq	os_local_test_start	;1 экономим такты!!!
	lsr	REG_OS_TEMP
	brne	os_timer_test_start	;7
	rjmp	os_timer_test_end
os_local_test_start:			;+5
	ldd	REG_OS_0,Y+CONST_THREAD_AMOUNT;2 
	cp	REG_TIMER_HIGH,REG_OS_0	;1
	brne	os_local_test_end	;
	or	REG_OS_1,REG_OS_TEMP	;1	 
os_local_test_end:			
	lsr	REG_OS_TEMP
	brne	os_timer_test_start	;7/13 +экономим регистры
os_timer_test_end:			;4+ 57/103 (мин и макс дл€ 8 тредов)
//--------------
	lds	REG_OS_TEMP,ADR_THREADS_TIMEOUT_MASK
	and	REG_OS_1,REG_OS_TEMP		;вычислили реальные
	eor	REG_OS_TEMP,REG_OS_1		;сборосили реальные
	sts	ADR_THREADS_TIMEOUT_MASK,REG_OS_TEMP
	or	REG_THREAD_MAP,REG_OS_1	;добавили в карту запуска
os_timer_no_event:			;итого 5/15+(57/103)
/*******************************************************************/
//******** TEST SECTION***************
.MACRO	____OS_TEST_TIMER
	in	REG_OS_TEMP,TCNT0
	sts	ADR_TEST_TIMER_TEMP,REG_OS_TEMP
	tst	REG_EXEC_THREAD
	brne	____OT_TIMER_end
	lds	REG_OS_0,ADR_TEST_SLEEP_TEMP
	ldi	REG_OS_TEMP,CONST_TIMER_COMPARE
	sub	REG_OS_TEMP,REG_OS_0

	lds	REG_OS_0,ADR_TEST_SLEEP_8
	add	REG_OS_0,REG_OS_TEMP
	sts	ADR_TEST_SLEEP_8,REG_OS_0
	brcc 	____OT_TIMER_end
	lds	REG_OS_0,ADR_TEST_SLEEP_16
	inc	REG_OS_0
	sts	ADR_TEST_SLEEP_16,REG_OS_0
____OT_TIMER_end:
.ENDMACRO

.MACRO	____OS_TEST_THREAD_WAIT
	in	REG_OS_TEMP,TCNT0
	sts	ADR_TEST_TIMER_TEMP,REG_OS_TEMP
.ENDMACRO

.MACRO	____OS_TEST_RUN_TREAD
	lds	REG_OS_0,ADR_TEST_TIMER_TEMP
	in	REG_OS_TEMP,TCNT0
	sub	REG_OS_TEMP,REG_OS_0

	lds	REG_OS_0,ADR_TEST_TIMER_8
	add	REG_OS_0,REG_OS_TEMP
	sts	ADR_TEST_TIMER_8,REG_OS_0
	brcc 	____OT_RUN_THREAD_end
	lds	REG_OS_0,ADR_TEST_TIMER_16
	inc	REG_OS_0
	sts	ADR_TEST_TIMER_16,REG_OS_0
____OT_RUN_THREAD_end:
.ENDMACRO

.MACRO	____OS_TEST_ENTER_SLEEP
	in	REG_OS_TEMP,TCNT0
	sts	ADR_TEST_SLEEP_TEMP,REG_OS_TEMP

.ENDMACRO

.MACRO	____OS_TEST_EXIT_SLEEP
	in	REG_OS_TEMP,TCNT0
	lds	REG_OS_0,ADR_TEST_SLEEP_TEMP
	sub	REG_OS_TEMP,REG_OS_0
	lds	REG_OS_0,ADR_TEST_SLEEP_8
	add	REG_OS_0,REG_OS_TEMP
	sts	ADR_TEST_SLEEP_8,REG_OS_0
	brcc 	____OT_EXIT_SLEEP_end
	lds	REG_OS_0,ADR_TEST_SLEEP_16
	inc	REG_OS_0
	sts	ADR_TEST_SLEEP_16,REG_OS_0
____OT_EXIT_SLEEP_end:
.ENDMACRO

