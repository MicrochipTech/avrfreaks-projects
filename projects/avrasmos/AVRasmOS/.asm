/*������������� � ����� AVR Studio 4.12.472 SP2, OS Microsoft Windows XP SP2 */
.INCLUDE "asmOS.inc"
//*************************************************
//---------- CONSTANT DEFINITION TABLE ------------
#if defined(__ATmega16__)
	.EQU	ADR_SYSTEM_TIMER_INT	=OC0addr
	.EQU	RAMSTART		=0x60

#elif defined(__ATmega48__)||defined(__ATmega88__)||defined(__ATmega168__)
	.EQU	ADR_SYSTEM_TIMER_INT	=OC0Aaddr
	.EQU	RAMSTART		=0x100
#endif

#ifndef	SYSTEM_CLOCK
	.EQU 	SYSTEM_CLOCK	= 7372800	;Hz
#endif

#ifndef	SYSTEM_TICK
	#define	SYSTEM_TICK	100 		;(us)
#endif

.EQU	CONST_TIMER_COMPARE	=((SYSTEM_CLOCK/8)/(1000000/SYSTEM_TICK));Timer0 divider=8 const provide 1 tick= 0.1 ms

.EQU	CONST_THREAD_AMOUNT 	=THREAD_AMOUNT

;��� �������� ���������� �������� ������������ ������� � 0x100 ������ � 0�60 �� 0�F0 - ���
;���� ���������� �����!
#ifdef	THREAD_STACK_SIZE
	.EQU	CONST_THREAD_STACK_SIZE	=THREAD_STACK_SIZE
#else
	.EQU	CONST_THREAD_STACK_SIZE	=0x10	;16 ���� ��� ���� (���� ������� -7) -����� ����� ������ ��������!
#endif
	
.EQU	CONST_THREAD_MEM_SIZE	=CONST_THREAD_STACK_SIZE+CONST_THREAD_CONTEXT_SIZE

.EQU	CONST_KERNEL_STACK_SIZE	=8		;������� �� ����������� ����������
; ���������� ��������� ��� ���������� ��� - ������ ����������� ������ � �������
//*************************************************
//---------- REGISTER DEFINITION TABLE ------------
.DEF	REG_THREAD_MAP		=r2	;����� ����������� ���������
.DEF	REG_EXEC_THREAD		=r3

.DEF	REG_OS_0		=r4
.DEF	REG_OS_1		=r5

.DEF	REG_TIMER_LOW		=r6	;������� ���� �������� ����� ���������� �������
.DEF	REG_TIMER_HIGH		=r7	;������� ���� �������� ����� ���������� �������

.DEF	REG_INT_0		=r8	;��������� ��� ����������
.DEF	REG_INT_1		=r9	;��������� ��� ����������

.DEF	REG_INT_TEMP		=r22	;��������� ������� ��� ���������� �����
.DEF	REG_OS_TEMP		=r23	;��������� ������� ��� ������������ 
					;�������� Zh:Zl ���� ����� ��� ����������					

//*************************************************
//--------- OS DATA SEGMENT DISTRIBUTION ----------
.DSEG
ADR_SYSTEM_TIME:			;����� �������� ����� ���������� ������� 
	.BYTE	2			;2 bytes

ADR_THREADS_TIMEOUT_MASK:		;����� ������ ��������� ������ �� ��������
	.BYTE	1				

ADR_CURRENT_THREAD_CONTEXT:		;����� �������� ��������� �� ADR_CONTEXT_ADR_TABLE �������� THREAD
	.BYTE	1

ADR_KERNEL_STACK:			;���� ��� ���������� (2) ��������� �� �� ���� ��� sleep-mode
	.BYTE	CONST_KERNEL_STACK_SIZE	;��. MACRO ____OS_PUSH_KERNEL_REGS

ADR_CONTEXT_ADR_TABLE:			;����� ������� ������� ��������� � ����� ������
	.BYTE	CONST_THREAD_AMOUNT*2	;������� ��������� ��� �������, ����� ��� �������

ADR_TIMER_TABLE:			;����� ������� ��������� ���������, �� 2 ����� �� �������	
	.BYTE	CONST_THREAD_AMOUNT*2	;������� ��������� ��� �������, ����� ��� �������
					
ADR_CONTEXT_PAGE:
	.BYTE	CONST_THREAD_AMOUNT*CONST_THREAD_MEM_SIZE
//*************************************************
//-------- INTERRUPT ADDRESS TABLE SEGMENT --------
.CSEG		
.ORG	0x000	
	rjmp	____OS_INIT	;3

.ORG 	ADR_SYSTEM_TIMER_INT	
	rjmp	____OS_TIMER_INT;3

.ORG	INT_VECTORS_SIZE
//-------------------------------------------------
//*************************************************
//-------------- OS KERNEL SEGMENT ----------------
//���� ����������� � ������ ���������� ����������
//������� ���� ����������� �� ������� ��������� ������. 
//��� ���������� ������ ����������� ����� ������ (<<256 clock)

#ifdef  OS_EVENTS_SYSTEM
#include "EVENT.asm"
#endif

#ifdef  OS_CACHE_SYSTEM
#include "CACHE.asm"
#endif

#ifdef  OS_FBS_SYSTEM
#include "FBS.asm"
#endif

.CSEG
//------------ SYSTEM TIMER INTERRUPT -------------
;���� ����������� ����� ����� ������ �� ������ ����� 
;����� ��������� ���������� �������� ����������  
____OS_TIMER_INT:
	//��������� � ����� ���������� ����������
	IN	REG_INT_TEMP,SREG
	PUSH	REG_INT_TEMP
	____ENTER_KERNEL_LEVEL
	//update system time
	inc	REG_TIMER_LOW
	brne	____OTI_end_update		;���� ���� ������ ������������		
	inc	REG_TIMER_HIGH
	brne	____OTI_end_update		;���� ���� ������ ������������		
	//������ ��������� �� ��� � ������ �����
	lds	REG_OS_TEMP,ADR_SYSTEM_TIME	;2
	inc	REG_OS_TEMP			;1
	sts	ADR_SYSTEM_TIME,REG_OS_TEMP	;2
	brne	____OTI_end_update		;1
	lds	REG_OS_TEMP,ADR_SYSTEM_TIME+1	;2
	inc	REG_OS_TEMP			;1
	sts	ADR_SYSTEM_TIME+1,REG_OS_TEMP	;2
____OTI_end_update:
	//check timeout thread event
	____OS_TIMER_CHECK_THREADS_TIMEOUT	;����� 5/15+(57/103)
	;��������� ��������� �����������

//*************************************************
//--------------- KERNEL SHELDULER ----------------
____OS_SCHEDULER:				//�������� � ������ ���������� ����������

#ifdef  OS_EVENTS_SYSTEM
	____OS_EVENTS_CHECK
____OS_SCHEDULER_events_check_disable:
#endif
	//�������� ��� �� ���������� ����� SLEEP
	tst	REG_EXEC_THREAD
	breq	____OS_SCHEDULER_store_context_ok

	____OS_POP_KERNEL_REGS			;��� �����������

	//�������� �� �� �� ������ 
	cp	REG_THREAD_MAP,REG_EXEC_THREAD

#ifdef OS_PREEMPTIVE_MODE	
	;��� ������������ ���� ������������ ��������� ����� ��������� ����  	
	;���� ����� �����, ������ ���� ��������� ��������� �� ����������, 
	;��������� ������ 
	brcs	____OS_SCHEDULER_new_thread
	;���� �� �� ���� ��� ����
	mov	REG_OS_TEMP,REG_EXEC_THREAD
	lsl	REG_OS_TEMP
	cpc	REG_THREAD_MAP,REG_EXEC_THREAD
	brcc	____OS_SCHEDULER_new_thread
	 
#else	;round_robin
	brne	____OS_SCHEDULER_new_thread
#endif
	;����� ��������� �� �� ������
	rjmp	____OS_SCHEDULER_same_thread
//---------------------------------
____OS_SCHEDULER_new_thread:
	____OS_THREAD_STORE
	____OS_EXEC_THREAD_GET_STACK_ADR
	in	REG_OS_TEMP,SPL
	st	Y,REG_OS_TEMP
	in	REG_OS_TEMP,SPH
	std	Y+CONST_THREAD_AMOUNT,REG_OS_TEMP
//---------------------------------
____OS_SCHEDULER_store_context_ok:

	//�������� ����� ���� �����
	tst	REG_THREAD_MAP
	breq	____OS_SLEEP_MODE

	//����� ��������� ������
#ifdef OS_PREEMPTIVE_MODE
	ldi	Yl,low(ADR_CONTEXT_ADR_TABLE+CONST_THREAD_AMOUNT-1)
	ldi	REG_OS_TEMP,(1<<CONST_THREAD_AMOUNT-1)
	rjmp	OS_S_start_test
OS_S_select_next:
	dec	Yl
	lsr	REG_OS_TEMP
OS_S_start_test:
	cp	REG_THREAD_MAP,REG_OS_TEMP	;REG_THREAD_MAP �� ������� ������������, ���� ��� ����������� �����
	brcs	OS_S_select_next
	mov	REG_EXEC_THREAD,REG_OS_TEMP
		
#else	;round_robin
	lds	Yl,ADR_CURRENT_THREAD_CONTEXT
OS_S_select_next:
	dec	Yl				;1
	lsr	REG_EXEC_THREAD			;1
	brne	OS_S_start_test			;2
		//�����������������
		ldi	REG_OS_TEMP,1<<(CONST_THREAD_AMOUNT-1)
		mov	REG_EXEC_THREAD,REG_OS_TEMP
		ldi	Yl,low(ADR_CONTEXT_ADR_TABLE+CONST_THREAD_AMOUNT-1)
OS_S_start_test:
	mov	REG_OS_TEMP,REG_EXEC_THREAD	;1
	and	REG_OS_TEMP,REG_THREAD_MAP	;1
	breq	OS_S_select_next		;2 (8 ����� = 56-1+3 = 58 ������ ��������)
#endif	
	
	STS	ADR_CURRENT_THREAD_CONTEXT,Yl
//---------------------------------
//	������ ������� ���������
____OS_SCHEDULER_load_curent_thread:
	ldi	Yh,high(ADR_CONTEXT_ADR_TABLE)	;____OS_EXEC_THREAD_STACK_ADR - Yl ��� ����
	cli
	ld	REG_OS_TEMP,Y	
	out	SPL,REG_OS_TEMP
	ldd	REG_OS_TEMP,Y+CONST_THREAD_AMOUNT
	out	SPH,REG_OS_TEMP
	sei
	____OS_THREAD_RESTORE

____OS_SCHEDULER_same_thread:			;�� �� ����� ������
	____ALIVE_KERNEL_LEVEL
	pop	REG_INT_TEMP
	out	SREG,REG_INT_TEMP
	RETI

//---------------------------------
____OS_SLEEP_MODE:		//���� �� ����� ���������� ��������� - ���� ����� ����� �����
	clr	REG_EXEC_THREAD
	____OS_SET_SLEEP_MODE
	cli	
	ldi	REG_OS_TEMP, low(ADR_KERNEL_STACK+CONST_KERNEL_STACK_SIZE-1) ;���� ��� ���� ������
	out	SPL,REG_OS_TEMP
	ldi	REG_OS_TEMP,high(ADR_KERNEL_STACK+CONST_KERNEL_STACK_SIZE-1)
	out	SPH,REG_OS_TEMP
	____OS_TIMER_INT_ENABLE
____OS_SM_int_exec:

#ifdef  OS_EVENTS_SYSTEM
	.IF CONST_EVENTS_AMOUNT<9
	mov	REG_OS_0,REG_EVENTS_low
	.ELSE
	movw	REG_OS_1:REG_OS_0,REG_EVENTS_high:REG_EVENTS_low
	.ENDIF
#endif
	sei
	SLEEP
	//���� ������� ����� �����
#ifdef  OS_EVENTS_SYSTEM
	cli				
	____OS_TIMER_INT_DISABLE
	sei
	cpse	REG_OS_0,REG_EVENTS_low
	RJMP	____OS_SCHEDULER
	.IF CONST_EVENTS_AMOUNT>8
	cpse	REG_OS_1,REG_EVENTS_high
	RJMP	____OS_SCHEDULER
	.ENDIF
	____OS_TIMER_INT_ENABLE
#endif
	RJMP	____OS_SM_int_exec	;____OS_SCHEDULER ��� ������ ������������ - ������� ����� ����� � ����

//---------------------------------
;������� ������������� ��������� �������� THREAD �� r25:r24 SYSEM_TICK
;���� r24:r25=0000, �� ��������������� �������. 
____OS_THREAD_WAIT:
	cli
	in	REG_OS_TEMP,SREG
	PUSH	REG_OS_TEMP
	____ENTER_KERNEL_LEVEL
	//��������� ����� �������
	;or	REG_THREAD_MAP,REG_EXEC_THREAD	;��� ���������� ����� ���-�� ������� ��� ������� ������� ???? ���������
	eor	REG_THREAD_MAP,REG_EXEC_THREAD
	cp	r24,r25
	brne	____OTW_timeout_enable
	tst	r24	
	breq	____OTW_timeout_disable		;�� ���� ��� ���� ����� �������� - ����� ��������
____OTW_timeout_enable:
	lds	REG_OS_TEMP,ADR_THREADS_TIMEOUT_MASK
	or	REG_OS_TEMP,REG_EXEC_THREAD	;
	sts	ADR_THREADS_TIMEOUT_MASK,REG_OS_TEMP
	//��������� ����� ��� �������� ����� 
	ldi	Yh,high(ADR_CONTEXT_ADR_TABLE)
	lds	Yl,ADR_CURRENT_THREAD_CONTEXT
	add	r24,REG_TIMER_LOW
	adc	r25,REG_TIMER_HIGH
	std	Y+CONST_THREAD_AMOUNT*2,r24	;��������� �������
	std	Y+CONST_THREAD_AMOUNT*2+8,r25	;��������� �������
____OTW_timeout_disable:			
	RJMP	____OS_SCHEDULER

/********************************************************/
/*		INIT asmOS				*/
/********************************************************/
____OS_INIT:
/*	��� ������� ����48
	ldi	r16,0x80
	ldi	r17,0x00
	sts	CLKPR,r16
	sts	CLKPR,r17
*/	
//------------- ������� ������
	ldi	Xl,low(RAMEND+1)
	ldi	Xh,high(RAMEND+1)
	ldi	REG_OS_TEMP,0x00
initos_p0:
	st	-X,REG_OS_TEMP
	cpi	r27,high(RAMSTART)
	brne	initos_p0
	cpi	r26,low(RAMSTART)
	brne	initos_p0
//------------- ������������� �����
	ldi	REG_OS_TEMP,high(RAMEND)
	out	SPH,REG_OS_TEMP 
	ldi	REG_OS_TEMP, low(RAMEND)
	out	SPL,REG_OS_TEMP

//*************************************************
;������������� ��������� � ������� ���������
	ldi	REG_OS_TEMP,0x00	;������������� ����
	ldi	Yh,high(ADR_CONTEXT_PAGE+(CONST_THREAD_STACK_SIZE-4));
	ldi	Yl, low(ADR_CONTEXT_PAGE+(CONST_THREAD_STACK_SIZE-4));
	ldi	Zh,high(ADR_CONTEXT_ADR_TABLE)
	ldi	Zl, low(ADR_CONTEXT_ADR_TABLE)

.IFDEF	THREAD_0
	ldi	r25,high(THREAD_0)
	ldi	r24, low(THREAD_0)
	rcall	____INIT_THREAD_CONTEXT
.ENDIF
.IFDEF	THREAD_1
	ldi	r25,high(THREAD_1)
	ldi	r24, low(THREAD_1)
	rcall	____INIT_THREAD_CONTEXT
.ENDIF
.IFDEF	THREAD_2
	ldi	r25,high(THREAD_2)
	ldi	r24, low(THREAD_2)
	rcall	____INIT_THREAD_CONTEXT
.ENDIF
.IFDEF	THREAD_3
	ldi	r25,high(THREAD_3)
	ldi	r24, low(THREAD_3)
	rcall	____INIT_THREAD_CONTEXT
.ENDIF
.IFDEF	THREAD_4
	ldi	r25,high(THREAD_4)
	ldi	r24, low(THREAD_4)
	rcall	____INIT_THREAD_CONTEXT
.ENDIF
.IFDEF	THREAD_5
	ldi	r25,high(THREAD_5)
	ldi	r24, low(THREAD_5)
	rcall	____INIT_THREAD_CONTEXT
.ENDIF
.IFDEF	THREAD_6
	ldi	r25,high(THREAD_6)
	ldi	r24, low(THREAD_6)
	rcall	____INIT_THREAD_CONTEXT
.ENDIF
.IFDEF	THREAD_7
	ldi	r25,high(THREAD_7)
	ldi	r24, low(THREAD_7)
	rcall	____INIT_THREAD_CONTEXT
.ENDIF

;------------------------------------------------
#ifdef OS_EVENTS_SYSTEM
	____EVENTS_INIT
#endif

#ifdef	OS_FBS_SYSTEM
	____FBS_INIT
#endif

	ldi	REG_OS_TEMP,((1<<CONST_THREAD_AMOUNT)-1)
	mov	REG_THREAD_MAP,REG_OS_TEMP
	ldi	REG_OS_TEMP,0x00
	mov	REG_EXEC_THREAD,REG_OS_TEMP

ldi	r24,low(ADR_CONTEXT_ADR_TABLE+CONST_THREAD_AMOUNT-1) ;��������� � ������������� �����
sts	ADR_CURRENT_THREAD_CONTEXT,r24	
____OS_TIMER_INIT

	RJMP	____OS_SCHEDULER

;*************************************************
____INIT_THREAD_CONTEXT:
	std	Y+CONST_THREAD_CONTEXT_SIZE+3,r24	;save SPL
	std	Y+CONST_THREAD_CONTEXT_SIZE+2,r25	;save SPH
	std	Y+CONST_THREAD_CONTEXT_SIZE+1,REG_OS_TEMP ;save SREG
	std	Z+CONST_THREAD_AMOUNT,Yh	
	st	Z+,Yl	
	adiw	Y,CONST_THREAD_MEM_SIZE
	RET
;------------------------------------------------

OS_SEGMENT_END:
.EXIT
