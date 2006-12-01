;--------------------UART DRIVER------------------------
;--------------------    NEW    ------------------------
;Это не просто обработчик - это мега быстрый обработчик!
;Не должен юзать общих регистров - выполняется только по прерыванию!
;Поддержка определения сигналов DTR и ответов DCR и запросов наоборот, 
;отслеживание временных задержек!
;номера совтовых перываний описываются в EBS_II

#include "UART.inc"

.EQU	CONST_RX_CACHE_SIZE	=0x20	;2 
.EQU	CONST_TX_CACHE_SIZE	=0x04

;*****************************************************
.DSEG
ADR_UART_CONFIG:		;адрес конфигурации UARTa (первые 4-ре - скорость) 
	.byte 1	

ADR_UART_TIMEOUT:
	.byte 2

ADR_UART_RX_CACHE:
	.byte	CONST_RX_CACHE_SIZE+3

ADR_UART_TX_CACHE:
	.byte	CONST_TX_CACHE_SIZE+3
;*****************************************************
.CSEG
//**************************************************
UART_RX_C:	//Cache Write
	IN	REG_INT_0,SREG		;1

	//read data
	____UART_READ	REG_INT_1	;1
	//store to cache
	____INT_CACHE_WRITE ADR_UART_RX_CACHE,CONST_RX_CACHE_SIZE,REG_INT_1	;24clock
	____EVENT_SET	EVENT_UART_INT_RX
	out	SREG,REG_INT_0		;1
	RETI				;4
					//31такт
;--------------------------------------
THREAD_UART_RECEIVE:
	____EVENT_CLEAR	EVENT_UART_INT_RX
	____FLAG_CLEAR	FLAG_UART_RX
	____CACHE_INIT	ADR_UART_RX_CACHE
	ldi	r16,CONST_DEFAULT_CONFIG	;скорость по умолчанию 9600
	sts	ADR_UART_CONFIG,r16
	____UART_INIT	ADR_UART_CONFIG
;--------------------------------------
THREAD_UART_RECEIVE_START:
	lds	r25,ADR_UART_TIMEOUT
	lds	r24,ADR_UART_TIMEOUT+1
	ori	r24,0x01
	movw	r1:r0,r25:r24	
	ldi	r25,0x00
	ldi	r24,0x00
	cpse	r24,r25
	//ждем события по приему(возможно циклически сбрасывая флаг)
__urx_wait_receive:
	movw	r25:r24,r1:r0
	____EVENT_WAIT	EVENT_UART_INT_RX	;ожидание с таймаутом
	brcs	__urx_check_flag
__urx_start_packet:
	____FLAG_SET	FLAG_UART_RX
__urx_load_data:
	____CACHE_READ ADR_UART_RX_CACHE,CONST_RX_CACHE_SIZE
	____EVENT_CLEAR EVENT_UART_INT_RX
	brcs	__urx_wait_receive
;------------------------------------
__urx_store_data:
	____QUEUE_PUSH_BACK QUEUE_UART_RX
	brcs	__urx_no_memory
	RJMP	__urx_load_data
;------------------------------------
__urx_check_flag:		
	____FLAG_SKIP_IF_SET FLAG_UART_RX
	rjmp	THREAD_UART_RECEIVE_START
__urx_end_packet:
	____FLAG_CLEAR	FLAG_UART_RX
	____QUEUE_ATTACH QUEUE_UART_TX,QUEUE_UART_RX
	____EVENT_SET	EVENT_UART_TX	
//____QUEUE_ATTACH QUEUE_RADIO_TX,QUEUE_UART_RX
//____EVENT_SET	EVENT_RADIO_TX	

	rjmp	THREAD_UART_RECEIVE_START
;------------------------------------
__urx_no_memory:	;пока что нет памяти - откидываем на передачу, а вообще ____QUEUE_FLUSH
	mov	r16,r24
	____QUEUE_ATTACH QUEUE_UART_TX,QUEUE_UART_RX	
	____EVENT_SET	EVENT_UART_TX
__urx_wait_mem:
	movw	r25:r24,r1:r0
	lsl	r25
	rol	r24
	rcall	____OS_THREAD_WAIT 
	mov	r24,r16
__urx_resave_data:
	rjmp	__urx_store_data
	;rjmp	__urx_end_packet

//**************************************************
//**************************************************
UART_INIT:
;---------------------------------------------------
UART_INIT_SPEED:
	andi	r24,0x0F	;get speed
	cpi	r24,CONST_SPEED_TABLE_SIZE
	brcs	UART_INIT_SPEED_ok	
	ldi	r24,CONST_DEFAULT_CONFIG&0x0F
UART_INIT_SPEED_ok:	
	lsl	r24
	ldi	r25,low(UART_TIMEOUT_TABLE<<1)
	add	r24,r25
	ldi	r25,high(UART_TIMEOUT_TABLE<<1)
	brcc	PC+2
	inc	r25
	cli			
	movw	Zh:Zl,r25:r24	
	lpm	r0,Z+
	lpm	r1,Z
	sei
	sts	ADR_UART_TIMEOUT,r1
	sts	ADR_UART_TIMEOUT+1,r0
	sbiw	r25:r24,(CONST_SPEED_TABLE_SIZE*2)
	cli			
	movw	Zh:Zl,r25:r24	
	lpm	r24,Z+
	lpm	r25,Z
	sei
;---------------------------------------------------
	____UART_SET_SPEED	;(r25:r24 значения бодрейтового регистра)
	____UART_CONFIGURE r24
	____UART_RXC_ON	r24
	RET
//-------------------------------------
UART_SPEED_TABLE:
.DW	BOUD_1200,BOUD_2400,BOUD_4800,BOUD_9600,BOUD_14400,BOUD_19200
.DW	BOUD_28800,BOUD_33600,BOUD_57600,BOUD_76800,BOUD_115200
UART_TIMEOUT_TABLE:
.DW	TOut_1200,TOut_2400,TOut_4800,TOut_9600,TOut_14400,TOut_19200
.DW	TOut_28800,TOut_33600,TOut_57600,TOut_76800,TOut_115200
;*********************************************************************
;*******************************************************
UART_DR_E:	
	in 	REG_INT_0,SREG		;1
	____INT_CACHE_READ	ADR_UART_TX_CACHE,CONST_TX_CACHE_SIZE,REG_INT_1
	brcs	__uitx_cache_end	
	____UART_WRITE	REG_INT_1	;1
	____EVENT_SET EVENT_UART_INT_TX
	out	SREG,REG_INT_0		;1
	RETI	

__uitx_cache_end:

#if defined(__ATmega16__)
	in	REG_INT_TEMP,UCSRB
	cbr	REG_INT_TEMP,(1<<UDRIE)
	sbr	REG_INT_TEMP,(1<<TXCIE)
	out	UCSRB,REG_INT_TEMP

#elif defined(__ATmega48__)||defined(__ATmega88__)||defined(__ATmega168__)
	lds	REG_INT_TEMP,UCSR0B
	cbr	REG_INT_TEMP,(1<<UDRIE0)
	sbr	REG_INT_TEMP,(1<<TXCIE0)
	sts	UCSR0B,REG_INT_TEMP	
#endif
	out	SREG,REG_INT_0		;1
	RETI	
	
UART_TX_C:
	in 	REG_INT_0,SREG		;1		
	____CACHE_GET_SIZE ADR_UART_TX_CACHE,REG_INT_TEMP
	tst	REG_INT_TEMP
	breq	__utxc_end_trans	
	____UART_DRE_ON REG_INT_TEMP
	rjmp	UART_DR_E+1		;повторим	
__utxc_end_trans:
	____UART_TXC_OFF REG_INT_TEMP
	____EVENT_SET	EVENT_UART_INT_TX_END
	out	SREG,REG_INT_0		;1
	RETI				;4
;*******************************************************

THREAD_UART_TRANSMIT:
	____EVENT_CLEAR	EVENT_UART_TX
	____CACHE_INIT	ADR_UART_TX_CACHE

THREAD_UART_TRANSMIT_START:
	lds	r25,ADR_UART_TIMEOUT
	lds	r24,ADR_UART_TIMEOUT+1
	lsr	r25
	ror	r24
	ori	r24,0x01
	movw	r1:r0,r25:r24	
	ldi	r25,0x00
	ldi	r24,0x00
;-----------------------------------------
	____EVENT_WAIT	EVENT_UART_TX
	//ждем буфера (циклически сбрасывая флаг лпроса - не реализовано)
___utr_test_queue:
	____QUEUE_TEST_EXIST QUEUE_UART_TX
	brcc	__utr_read_queue
	____EVENT_CLEAR	EVENT_UART_TX
	rjmp	THREAD_UART_TRANSMIT_START

__utr_read_queue:
	____QUEUE_POP_FRONT QUEUE_UART_TX
	brcs	___utr_test_queue

__utr_send_data:
	____CACHE_WRITE ADR_UART_TX_CACHE,CONST_TX_CACHE_SIZE
	____UART_DRE_ON r16	;включить передачу
	brcc	__utr_read_queue

__utr_wait_cache:
	mov	r16,r24
	____EVENT_CLEAR EVENT_UART_INT_TX
	movw	r25:r24,r1:r0
	____EVENT_WAIT	EVENT_UART_INT_TX
	brcs	UART_TX_ERROR
	mov	r24,r16
	rjmp	__utr_send_data
//---------------------------------------------------------
UART_TX_ERROR:
	//генерация ошибки модуля
	mov	r24,r16
	rjmp	__utr_send_data
	RET
.EXIT
//********************************************************************
void TaskUARTRcv(void)
{
    PACKET *p = 0;
    BOOL   slip_flag = FALSE;
    BOOL   bad_flag = FALSE;
    WORD   length = 0;

    UART_Init(Config.UARTSpeed);

    for(;;)
    {
        EVENT_MASK events = OS_WaitEventsTimeout(EV_UART_RXC, SLIP_TIMEOUT);

        if(!events)
        {
            slip_flag = FALSE;
            bad_flag = FALSE;
            length = 0;
            continue;
        }

        OS_ClearEvents(events);
        BYTE sym = UART_Read();

        if(sym == 0xC0)
        {
            if(length >= 4)
            {
                p->Size = length;

                if(!bad_flag && !slip_flag && CheckPacketCRC(p))
                {
                    p->Size = length - 2;

                    QueuePut(UART_RcvQueue, p);
                    OS_SetEvents(EV_UART_RCV);
                    p = 0;
                }
            }

            slip_flag = FALSE;
            bad_flag = FALSE;
            length = 0;
        }
        else if(sym == 0xDB)
        {
            bad_flag|= slip_flag;
            slip_flag = TRUE;
        }
        else if(!bad_flag)
        {
            if(slip_flag)
            {
                if(sym == 0xDC) sym = 0xC0;
                else if(sym == 0xDD) sym = 0xDB;
                else bad_flag = TRUE;

                slip_flag = FALSE;
            }

            if(!p) p = PoolAlloc(NetSndPacketPool);

            if(p)
            {
                if(length == NET_PACKET_SIZE) bad_flag = TRUE;
                else p->Data[length++] = sym;
            }
            else bad_flag = TRUE;
        }
    }
}

//********************************************************************
void TaskUARTSnd(void)
{
    for(;;)
    {
        OS_WaitEvents(EV_UART_SND);

        for(;;)
        {
            PACKET *pkt = QueueGet(UART_SndQueue);
            if(!pkt) break;

            AddPacketCRC(pkt);

            WORD length = pkt->Size;
            BYTE *p = pkt->Data;

            OS_ClearEvents(OS_WaitEvents(EV_UART_DRE));
            UART_Write(0xC0);
            do
            {
                BYTE sym = *p++;
                OS_ClearEvents(OS_WaitEvents(EV_UART_DRE));

                if(sym == 0xC0)
                {
                    UART_Write(0xDB);
                    OS_ClearEvents(OS_WaitEvents(EV_UART_DRE));
                    sym = 0xDC;
                }
                else if(sym == 0xDB)
                {
                    UART_Write(0xDB);
                    OS_ClearEvents(OS_WaitEvents(EV_UART_DRE));
                    sym = 0xDD;
                }

                UART_Write(sym);
            } while(--length);

            OS_ClearEvents(OS_WaitEvents(EV_UART_DRE));
            UART_Write(0xC0);

            PoolFree(pkt);
        }

        OS_ClearEvents(EV_UART_SND);
    }
}



