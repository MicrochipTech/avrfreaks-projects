#ifndef	OS_EVENTS_SYSTEM
	#define	OS_EVENTS_SYSTEM
#endif

//16 or 8 bit event
.DEF	REG_EVENTS_low		=r10	
.DEF	REG_EVENTS_high		=r11

#ifdef  OS_FBS_SYSTEM
.EQU	CONST_EVENTS_AMOUNT	=EVENT_AMOUNT+1	//количество СОБЫТИЙ еще одно для FBS
#elif
.EQU	CONST_EVENTS_AMOUNT	=EVENT_AMOUNT	//количество СОБЫТИЙ
#endif


//***************************************************
.DSEG

ADR_EVENT_TABLE:		 //адрес таблицы СОБЫТИЙ
	.BYTE	CONST_EVENTS_AMOUNT			
.CSEG
//***************************************************
.MACRO	____EVENT_SET			//входной параметр номер собтия short T bit bad
	SET				;1
.IF @0<8
	BLD	REG_EVENTS_low,@0	;1
.ELSE
	BLD	REG_EVENTS_high,(@0-8)	;1
.ENDIF
	//возможно надо вызвать перепланировщика
.ENDMACRO

//-------------------------------------
.MACRO	____EVENT_CLEAR			//входной параметр номер собтия short T bit bad
	CLT				;1
.IF @0<8
	BLD	REG_EVENTS_low,@0	;1
.ELSE
	BLD	REG_EVENTS_high,(@0-8)	;1
.ENDIF
.ENDMACRO

//---------------------------------------
.MACRO	____EVENT_WAIT			//входной параметр номер события
.IF @0<8				//регистры r25 r24 содержат таймаут
	sbrc	REG_EVENTS_low,@0		;1
.ELSE
	sbrc	REG_EVENTS_high,(@0-8)		;1
.ENDIF
	rjmp	____EVENT_WAIT_OK		;2
	cli					;1
	lds	REG_OS_TEMP,ADR_EVENT_TABLE+@0	;2
	or	REG_OS_TEMP,REG_EXEC_THREAD	;1 регистр с текущим процессом
	sts	ADR_EVENT_TABLE+@0,REG_OS_TEMP	;2
	RCALL	____OS_THREAD_WAIT
	//сюда вернется из прерывания
	sec				;выставляем карри - есть тайм-аут
	lds	r25,ADR_EVENT_TABLE+@0
	and	r25,REG_EXEC_THREAD
	brne	PC+2			;если стоит 1 значит не сбросилось
____EVENT_WAIT_OK:
	clc				;чистим КАРРИ если есть или случилось событие
.ENDMACRO				;иначе был ТАЙМУАТ

//--------------------------------------
.MACRO	____OS_EVENT_SKIP_IF_SET
.IF @0<8				
	sbrs	REG_EVENTS_low,@0	
.ELSE
	sbrs	REG_EVENTS_high,(@0-8)	
.ENDIF					
.ENDMACRO				

//------------------------------------
.MACRO	____OS_WAIT_EVENTS		//ждет одного из нескольких или всех событий сразу
					//@0-события, @1 - тип ожидания, r1:r0 - тайм аут
.ENDMACRO				//пока не сделано - нет надобности

//--------------------------------------
.MACRO	____EVENTS_INIT
	ldi	REG_OS_TEMP,0x00
	mov	REG_EVENTS_high,REG_OS_TEMP
	mov	REG_EVENTS_low,REG_OS_TEMP
	ldi	Zh,high(ADR_EVENT_TABLE+CONST_EVENTS_AMOUNT)
	ldi	Zl,low(ADR_EVENT_TABLE+CONST_EVENTS_AMOUNT)
____EVENT_INIT_repeat:
	st	-Z,REG_OS_TEMP
	cpi	Zl,low(ADR_EVENT_TABLE)
	brne 	____EVENT_INIT_repeat
.ENDMACRO
//------------------------------------
.MACRO	____OS_EVENTS_CHECK			//для простоты если событие ждет только 1 процесс
						//можно сделать все через LDI (экономия памяти и тактов)
	mov	REG_OS_0,REG_EVENTS_low
	ldi	REG_OS_TEMP,0x00
	ldi	Yh,high(ADR_EVENT_TABLE)	;1
	ldi	Yl,low(ADR_EVENT_TABLE)		;1
	rjmp	____OS_EVENTS_CHECK_repeat_low+1

____OS_EVENTS_CHECK_low_ok:
	ld	REG_OS_1,Y			;2
	or	REG_THREAD_MAP,REG_OS_1		;1
	st	Y,REG_OS_TEMP			;2

____OS_EVENTS_CHECK_repeat_low:
	inc	Yl	
	lsr	REG_OS_0
	brcs	____OS_EVENTS_CHECK_low_ok
	brne	____OS_EVENTS_CHECK_repeat_low
	
.IF CONST_EVENTS_AMOUNT>8
	cpi	Yl,low(ADR_EVENT_TABLE+9)
	brcc	____OS_EVENTS_CHECK_end	
	ldi	Yl,low(ADR_EVENT_TABLE+8)	;1
	mov	REG_OS_0,REG_EVENTS_high
	lsr	REG_OS_0
	brcs	____OS_EVENTS_CHECK_low_ok
	brne	____OS_EVENTS_CHECK_repeat_low	
____OS_EVENTS_CHECK_end:
.ENDIF

.ENDMACRO

.EXIT
//-----------------------------------------------------------------------------------------------
.MACRO	____OS_EVENTS_CHECK			//для простоты если событие ждет только 1 процесс
	mov	REG_OS_0,REG_EVENTS_low		;1
	ldi	Yh,high(ADR_EVENT_TABLE)	;1
	ldi	Yl,low(ADR_EVENT_TABLE)		;1
.IF CONST_EVENTS_AMOUNT<9
	RCALL	OE_check_0e
.ELIF CONST_EVENTS_AMOUNT>8
	RCALL	OE_check_0e
	mov	REG_OS_0,REG_EVENTS_high	;1
	ldi	Yl,low(ADR_EVENT_TABLE+8)	;1
	RCALL	OE_check_0e
.ENDIF
.ENDMACRO
	
OE_check_0e:
	sbrc	REG_OS_0,0		
	rjmp	OE_check_0
OE_check_1e:
	sbrc	REG_OS_0,1		
	rjmp	OE_check_1
OE_check_2e:
	sbrc	REG_OS_0,2		
	rjmp	OE_check_2
OE_check_3e:
	sbrc	REG_OS_0,3		
	rjmp	OE_check_3
OE_check_4e:
	sbrc	REG_OS_0,4		
	rjmp	OE_check_4
OE_check_5e:
	sbrc	REG_OS_0,5		
	rjmp	OE_check_5
OE_check_6e:
	sbrc	REG_OS_0,6		
	rjmp	OE_check_6
OE_check_7e:
	sbrs	REG_OS_0,7		
	RET
	ldd	REG_OS_1,Y+7	
	or	REG_THREAD_MAP,REG_OS_1
	RET

OE_check_0:	;7тактов
ldd	REG_OS_1,Y+0	
or	REG_THREAD_MAP,REG_OS_1
sbrs	REG_OS_0,1		
rjmp	OE_check_2e

OE_check_1:
ldd	REG_OS_1,Y+1	
or	REG_THREAD_MAP,REG_OS_1
sbrs	REG_OS_0,2		
rjmp	OE_check_3e

OE_check_2:
ldd	REG_OS_1,Y+2	
or	REG_THREAD_MAP,REG_OS_1
sbrs	REG_OS_0,3		
rjmp	OE_check_4e

OE_check_3:
ldd	REG_OS_1,Y+3	
or	REG_THREAD_MAP,REG_OS_1
sbrs	REG_OS_0,4		
rjmp	OE_check_5e

OE_check_4:
ldd	REG_OS_1,Y+4	
or	REG_THREAD_MAP,REG_OS_1
sbrs	REG_OS_0,5		
rjmp	OE_check_6e

OE_check_5:
ldd	REG_OS_1,Y+5	
or	REG_THREAD_MAP,REG_OS_1
sbrs	REG_OS_0,6		
rjmp	OE_check_7e

OE_check_6:
ldd	REG_OS_1,Y+6	
or	REG_THREAD_MAP,REG_OS_1
sbrs	REG_OS_0,7		
RET

OE_check_7:
ldd	REG_OS_1,Y+7	
or	REG_THREAD_MAP,REG_OS_1
RET
