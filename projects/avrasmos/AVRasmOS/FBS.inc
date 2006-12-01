.EQU	EVENT_FBS_ACCESS	=EVENT_AMOUNT	

.EQU	FLAG_FBS_HEAD		=7	

.EQU	CONST_FBS_BUF_SIZE	=32 ;may be 64,16,8, but not realized

.EQU	CONST_FBS_BUF_AMOUNT	=INT((RAMEND-ADR_FBS_START_MEM)/CONST_FBS_BUF_SIZE) ;#define работает

.EQU	ADR_FBS_BUF_START	=RAMEND-CONST_FBS_BUF_SIZE*CONST_FBS_BUF_AMOUNT+1   ;#define неработает

.EQU	CONST_FBS_QUEUE_TABLE_ENTRY_SIZE	=4
;--------------------------------------------------
.DSEG
ADR_FBS_QUEUE_TABLE:			;таблица очередей (цепочек буферов)
	.BYTE QUEUE_AMOUNT*CONST_FBS_QUEUE_TABLE_ENTRY_SIZE
.EQU	START_BUF_H	=0
.EQU	START_BUF_L	=1
.EQU	CURRENT_BUF_H	=2
.EQU	CURRENT_BUF_L	=3

ADR_FBS_EMPTY_QUEUE_H:	.BYTE 1		;указатель на очередь пустых буферов
ADR_FBS_EMPTY_QUEUE_L:	.BYTE 1		;указатель на очередь пустых буферов
;**************************************************************************
;--------------------------------------------------
.MACRO	____FBS_INIT
____FBS_INIT_buf_stack:
	ldi	Xh,high(ADR_FBS_BUF_START)	;1
	ldi	Xl, low(ADR_FBS_BUF_START)	;1
	sts	ADR_FBS_EMPTY_QUEUE_H,Xh	
	sts	ADR_FBS_EMPTY_QUEUE_L,Xl
	ldi	r24,CONST_FBS_BUF_AMOUNT-1
____FBS_INIT_cycle:
	movw	Yh:Yl,Xh:Xl
	adiw	X,CONST_FBS_BUF_SIZE	;only if size lower than 64
	st	Y,Xh			;указатель на следющий
	std	Y+1,Xl
	dec	r24
	brne	____FBS_INIT_cycle
	ldi	r24,0xFF
	ST	X,r24

____FBS_INIT_queue_table:
	ldi	r25,QUEUE_AMOUNT
	ldi	Yh,high(ADR_FBS_QUEUE_TABLE)
	ldi	Yl, low(ADR_FBS_QUEUE_TABLE)
____FBS_INIT_queue_table_cycle:
	st	Y,r24
	adiw	Y,CONST_FBS_QUEUE_TABLE_ENTRY_SIZE
	dec	r25
	brne	____FBS_INIT_queue_table_cycle
	____EVENT_SET EVENT_FBS_ACCESS
.ENDMACRO

;----------------------------------------
.MACRO	____FBS_LOCK_ACCESS
	cli
	____OS_EVENT_SKIP_IF_SET EVENT_FBS_ACCESS
	RCALL	____FBS_LOCK_ACCESS_wait_event		;если чистый, значит занят - снова ждем
	____EVENT_CLEAR EVENT_FBS_ACCESS
	sei
.ENDMACRO

;----------------------------------------
.MACRO	____FBS_UNLOCK_ACCESS
	____EVENT_SET	EVENT_FBS_ACCESS
.ENDMACRO

;----------------------------------------
.MACRO	____FBS_BUF_ALLOC		;@0 pointer
	____FBS_LOCK_ACCESS
	ldi	r25,0xFE		 ;1	хитрость
	lds	@0h,ADR_FBS_EMPTY_QUEUE_H;2
	cp	r25,@0h			 ;1
	brcs	____FBS_BA_end		 ;1
	lds	@0l,ADR_FBS_EMPTY_QUEUE_L;2
	ld	r25,@0+			 ;2
	sts	ADR_FBS_EMPTY_QUEUE_H,r25;2
	ld	r25,@0			 ;2
	sts	ADR_FBS_EMPTY_QUEUE_L,r25;2
	ldi	r25,0xFF		 ;1	;следующий нулевой
	st	-@0,r25			 ;2
____FBS_BA_end:				//18такта
	____FBS_UNLOCK_ACCESS
.ENDMACRO
;----------------------------------------
.MACRO	____FBS_BUF_FREE		 ;освобождает буфер Y, возвращает указатель на следующий
	ld	r25,Y			 ;2	\
	std	Y+2,r25			 ;2	|
	ldd	r25,Y+1			 ;2	|  - save next pointer
	andi	r25,~(CONST_FBS_BUF_SIZE-1);1	|
	std	Y+3,r25			 ;2	/
	lds	r25,ADR_FBS_EMPTY_QUEUE_H;2	\
	st	Y,r25			 ;2	|
	lds	r25,ADR_FBS_EMPTY_QUEUE_L;2	|  - store last free buf
	std	Y+1,r25			 ;2	/
	sts	ADR_FBS_EMPTY_QUEUE_H,Yh ;2	\
	sts	ADR_FBS_EMPTY_QUEUE_L,Yl ;2	/  -  store new free buf
	ldd	r25,Y+2			 ;2
	ldd	Yl,Y+3			 ;2
	mov	Yh,r25			 ;1 //26 clock
.ENDMACRO

.EXIT
;*****************************************************************************************
;*****************************************************************************************
;*****************************************************************************************
/*

.MACRO	____FBS_GET_BUF_ADR		;@0-указатель куда адрес надо запихать r25 не портится
	//номер буфера сдвинуть на 32 и + Yh:Yl
	mov	@0h,@1				;mov 	Yh,r24
	swap	@0h				;lsr	Yh
	mov	@0l,@0h				;lsl	Yl
	andi	@0h,0b00001111			;lsr	Yh
	andi	@0l,0b11110000			;lsl	Yl
	lsl	@0l				;lsr	Yh	;size=32
	rol	@0h				;lsl	Yl	;size=32
	;SUBI	@0l, (~low(ADR_FBS_BUF_START)+1)	работает но не понятно как
	;SBCI	@0h,(~high(ADR_FBS_BUF_START))		на крайние значения гонит
	ori	@0h,0b11100000	
	SUBI	@0l, low(0xE000 - ADR_FBS_BUF_START)
	SBCI	@0h,high(0xE000 - ADR_FBS_BUF_START)
.ENDMACRO	
*/
