;****************************************************
; ������� ������� ��� ������ � �������
; struct buffer
;	{
;	byte NEXT_ADR_HIGH		(if = 0xFF)
;		bit 7 - header flag
;		bits 0..6 - high pointer 
;	byte NEXT_ADR_LOW
;		low adr with mask ~(BUF_SIZE-1)	
;		buf_size with mask (BUF_SIZE-1)
;	byte DATA[30]
;	}
;the HADER flag was crear when transmitted to send_proc_buf
;****************************************************
#include "FBS.inc"
;**************************************************
;API  ������� ��� ������ � �������
;**************************************************
.CSEG
;--------------------------------------------------
;��� ���� ������ ��� - ��������� �����, ������ - ������� �������
.MACRO	____QUEUE_PUSH_BACK	;@0 - ����� �������
	ldi	Yh,high(ADR_FBS_QUEUE_TABLE+@0*CONST_FBS_QUEUE_TABLE_ENTRY_SIZE)
	ldi	Yl, low(ADR_FBS_QUEUE_TABLE+@0*CONST_FBS_QUEUE_TABLE_ENTRY_SIZE)
	RCALL	____FBS_PUSH_BACK
.ENDMACRO

____FBS_PUSH_BACK:	;r24		������ ��� ������;������ r24,r25,Xh,Xl,Yh,Yl
	;�������� ������� ������
	ld	r25,Y
	cpi	r25,0xFF
	breq	____FBS_PB_new_queue
	;-------
	;�������� ���������� �����
	ldd	Xh,Y+2
	ldd	Xl,Y+3
	mov	r25,Xl
	andi	r25,(CONST_FBS_BUF_SIZE-1)
	breq	____FBS_PB_buf_end	;���� ������� �� ������������ 
	;store buf size
	st	X,r24			;Store DATA
	inc	Xl			;Xh - do not change
	std	Y+3,Xl			;Store Xl POSITION (Xh not change)
	sub	Xl,r25
;	dec	r25
	subi	r25,1			;clear carry
	st	X,r25
;	clc
	RET
____FBS_PB_buf_end:
	SUBI	Xl,CONST_FBS_BUF_SIZE	;�������� CONST_FBS_BUF_SIZE ���� ��� ��������� �����
	std	Y+3,Xl			;������ ������� �������� ��� ���������
____FBS_PB_new_queue:			;����� �� ������������������
	____FBS_BUF_ALLOC X		;� Xh:Xl ������ ����� r25 ��������
	brcc	PC+2
	RET				;buf allocation fail
	inc	Xl
	ldi	r25,0x01
	st	X+,r25
	st	X+,r24			;Save data and stand in next position
	;cheking - initialize or append
	ld	r25,Y
	cpi	r25,0xFF
	breq	____FBS_PB_init_head
	movw	r25:r24,Xh:Xl
	ldd	Xh,Y+2	
	ldd	Xl,Y+3	
	std	Y+2,r25
	std	Y+3,r24
	st	X+,r25
	ld	r25,X
	andi	r24,~(CONST_FBS_BUF_SIZE-1)
	or	r25,r24
	st	X,r25
	clc
	RET
____FBS_PB_init_head:
	;Save HEADER buffer and current buffer
	std	Y+2,Xh
	std	Y+3,Xl
	andi	Xl,~(CONST_FBS_BUF_SIZE-1)
	std	Y+1,Xl
	ori	Xh,(1<<FLAG_FBS_HEAD)
	st	Y,Xh			
	RET
;**************************************************
;**************************************************
;��� ��� ������ ��� - ��������� �����, ������ - ������� �������
;����� �����: ���������� � ������� ������� ��������� �� �������
;������� ������� (��� ���������� ���������) �������������� ��������� 
;������� ��������� ��� ������, ��������� ��������� �� ����, ��������� ��������� � �������
;��������� �������� �� ����.
;��� ����������� � ������ ��������� ������ ����������������� �� �������+1
.MACRO	____QUEUE_POP_FRONT	;@0 - ����� �������
	ldi	Xh,high(ADR_FBS_QUEUE_TABLE+@0*CONST_FBS_QUEUE_TABLE_ENTRY_SIZE)
	ldi	Xl, low(ADR_FBS_QUEUE_TABLE+@0*CONST_FBS_QUEUE_TABLE_ENTRY_SIZE)
	RCALL	____FBS_POP_FRONT
.ENDMACRO 
;��������� � ������� ������ ������������, ����� ������ ����� �������
____FBS_POP_FRONT:			;��������� � r24 ������, CARRY - ����� �������, ����� �������
	;------ �������� ������� ������ ������������ ���������
	ld	Yh,X+
/*	cpi	Yh,0xFF
	breq	____FBS_PF_terminate			
	;------ �������� ���������
	cpi	Yh,(1<<FLAG_FBS_HEAD)
	brcs	____FBS_PF_bufok
	andi	Yh,~(1<<FLAG_FBS_HEAD)
	st	-X,Yh
	;������������� ���������� � ��������� ��������
	adiw	Xh:Xl,1
	ld	Yl,X
	inc 	Yl
	ld	r25,Y+		;�������� ������
	st	X+,Yl		;��������� ��������
	andi	r25,(CONST_FBS_BUF_SIZE-1)
	add	r25,Yl
	st	X,r25		;�������� ��������
____FBS_PF_terminate:
	sec
	RET*/
		sec
		sbrc	Yh,FLAG_FBS_HEAD
		RET
____FBS_PF_bufok:
	ld	Yl,X+		;������� ��������
	ld	r24,Y+
	ld	r25,X		;�������� ��������
	cp	r25,Yl
	breq	____FBS_PF_buf_free
	st	-X,Yl
	clc
	RET			;Carry clear	
____FBS_PF_buf_free:
	____FBS_LOCK_ACCESS
	ld	Yl,-X
	andi	Yl,~(CONST_FBS_BUF_SIZE-1)
	ld	Yh,-X			
	____FBS_BUF_FREE	;����������� ����� Y, �������� ���� ��������� �� ��������� �����
	st	X+,Yh
	st	X,Yl
	____FBS_UNLOCK_ACCESS
	clc
	sbrc	Yh,FLAG_FBS_HEAD
	RET			;������, ���� ������������ ��� �������
	;������������������ ����� ���� �� �� ������������
	inc 	Yl
	ld	r25,Y+
	st	X+,Yl		;��������� ��������
	andi	r25,(CONST_FBS_BUF_SIZE-1)
	add	r25,Yl
	st	X,r25		;�������� ��������
	clc
	RET
;**************************************************
.MACRO	____QUEUE_APPEND 		;�������� ������� � ��������� � ������� 
.ENDMACRO

;--------------------------------------------------
.MACRO	____QUEUE_ATTACH		;�������� ������� � ������� @0 - destinatio; @1 - sourse
	ldi	Yh,high(ADR_FBS_QUEUE_TABLE+@0*CONST_FBS_QUEUE_TABLE_ENTRY_SIZE)
	ldi	Yl, low(ADR_FBS_QUEUE_TABLE+@0*CONST_FBS_QUEUE_TABLE_ENTRY_SIZE)
	ldi	Xh,high(ADR_FBS_QUEUE_TABLE+@1*CONST_FBS_QUEUE_TABLE_ENTRY_SIZE)
	ldi	Xl, low(ADR_FBS_QUEUE_TABLE+@1*CONST_FBS_QUEUE_TABLE_ENTRY_SIZE)
	RCALL	____FBS_QUEUE_ATTACH
	____FBS_UNLOCK_ACCESS
.ENDMACRO 

____FBS_QUEUE_ATTACH:
	____FBS_LOCK_ACCESS
	ld	r25,X
	cpi	r25,0xFF
	brne	PC+2
	RET
	ldi	r24,0xFF
	st	X+,r24
	ld	Xl,X
	ori	r25,(1<<FLAG_FBS_HEAD)
	andi	Xl,~(CONST_FBS_BUF_SIZE-1)
	ld	r24,Y	 
	cpi	r24,0xFF	;�������� �� ������� �������
	brne	____FBS_QA_find_end
	cli
	st	Y,r25		;header ���������� � ��� ������ ������� �������� 	
	std	Y+1,Xl
	RETI
____FBS_QA_repeat:		;������� - ���� ������ ��������
	ld	r24,Y
	cpi	r24,0xFF	
	breq	____FBS_QA_attach
____FBS_QA_find_end:
	andi	r24,~(1<<FLAG_FBS_HEAD)
	ldd	Yl,Y+1
	andi	Yl,~(CONST_FBS_BUF_SIZE-1)
	mov	Yh,r24
	rjmp	____FBS_QA_repeat	
____FBS_QA_attach:
	ldd	r24,Y+1
	andi	r24,(CONST_FBS_BUF_SIZE-1)
	or	r24,Xl	
	cli		
	st	Y,r25		
	std	Y+1,r24
	RETI

;--------------------------------------------------
.MACRO	____QUEUE_TEST_EXIST
	ldi	Yh,high(ADR_FBS_QUEUE_TABLE+@0*CONST_FBS_QUEUE_TABLE_ENTRY_SIZE)
	ldi	Yl, low(ADR_FBS_QUEUE_TABLE+@0*CONST_FBS_QUEUE_TABLE_ENTRY_SIZE)
	RCALL	____FBS_TEST
.ENDMACRO

____FBS_TEST:		;��������� ���� ���� � ������ ������
	____FBS_LOCK_ACCESS
	ld	Xh,Y
	cpi	Xh,0xFF
	sec
	breq	____FBS_TEST_end2
	cpi	Xh,(1<<FLAG_FBS_HEAD)
	brcs	____FBS_TEST_end1
	andi	Xh,~(1<<FLAG_FBS_HEAD)	
	st	Y,Xh
	//������������� ��������� �� ������ ������
	ldd	Xl,Y+1
	adiw	Xh:Xl,1
	ld	r24,X+
	std	Y+1,Xl	;��������� �� ��������� �������
	andi 	r24,(CONST_FBS_BUF_SIZE-1)
	add	Xl,r24
	std	Y+2,Xl	;��������� �� �������� �������
____FBS_TEST_end1:
	clc
____FBS_TEST_end2:
	____FBS_UNLOCK_ACCESS
	RET
;**************************************************
;--------------------------------------------------
____FBS_FLUSH_QUEUE:			;���������� ������ (� �������� �������� ��������� �� ���������� �������������)	
	RET
;--------------------------------------------------
____FBS_BUF_READ:	;r25,r24	����� �����
	RET
;--------------------------------------------------
____FBS_BUF_WRITE:	;r25,r24	����� �����
	RET
;**************************************************
;   ������� ��� ������ � �������� ������� ������ 
;  ������ �25 � Y	
;**************************************************

//----------------------------------------------------
;*****************************************************
;	���������� �������� � ������� (����� �������) (������������� ������� ����� �������)
;*****************************************************
____FBS_LOCK_ACCESS_wait_event:
	PUSH	r24
	ldi	r24,0x00
	ldi	r25,0x00
____FBS_LA_repeat:
	____EVENT_WAIT EVENT_FBS_ACCESS	;���� ����������
	cli
	____OS_EVENT_SKIP_IF_SET EVENT_FBS_ACCESS
	rjmp	____FBS_LA_repeat		;���� ������, ������ ����� - ����� ����
	POP	r24
	RET
.EXIT
/******************************************************/
����� ���� ������� ���������:
P*= Yl|Yh

.MACRO	GET_BUF_ADR ;@0,@1 destination,sourse
	ld	@0l,@1				;2
	mov	@0h,@0l				;1
	andi	@0h,(CONST_FBS_BUF_SIZE-1)	;1
	andi	@0l,~(CONST_FBS_BUF_SIZE-1)	;1
	;��� ��������� ����������� �� 5 ������
	;�� 1 ����� ������ 1 ����
	;����� ����� ���������� ��������
	;������� ������ ��� ����� ��� ���������� - ����� �� ����� 
.ENDMACRO

.MACRO	INC_BUF_ADR ;@0 address			//check ZERO flag
	bst	@0l,LOG2(CONST_FBS_BUF_SIZE)
	inc	@0l				;1
	sbrs	@0l,CONST_FBS_BUF_SIZE		;���������� ���� ����������
	rjmp	bit_is_change	
	sbrc	@0l,CONST_FBS_BUF_SIZE		;���������� ���� ����������
	rjmp	bit_is_change	

bit_in_true:
	inc	@0l				;1
	sbrc	@0l,CONST_FBS_BUF_SIZE		;���������� ���� ����
	rjmp	bit_is_change	
							;��������� ���� ������
bit_is_change:


.ENDMACRO

.MACRO	DEC_BUF_ADR ;@0 address			//check ZERO flag
	dec	@0l				;1
	mov	r25,@0l				;1
	andi	r25,(CONST_FBS_BUF_SIZE-1)	;1
.ENDMACRO
	
	???????????
	mov	@0h,@0l				;1
	andi	@0h,(CONST_FBS_BUF_SIZE-1)	;1
	andi	@0l,~(CONST_FBS_BUF_SIZE-1)	;1
	;��� ��������� ����������� �� 5 ������
	;�� 1 ����� ������ 1 ����
	;����� ����� ���������� ��������
	;������� ������ ��� ����� ��� ���������� - ����� �� ����� 
.ENDMACRO
