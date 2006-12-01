;****************************************
;********** SYSTEM SECTION **************
//#include <m16def.inc>
#include <m48def.inc>
//#include <m88def.inc>

#define SYSTEM_CLOCK		7372800 ;Hz	;18432000 

#define	SYSTEM_TICK		100 	;us

//#define OS_PREEMPTIVE_MODE		;по умолчанию раунд-робин планирование
;****************************************
;***********  FLAGS SECTION *************
#define	REG_FLAGS		r15	;регистр флагов - можно выбросить
.EQU	FLAG_UART_RX		=0
.EQU	FLAG_RADIO_RX		=1
//.EQU	FLAG_UART_TX		=2
//.EQU	FLAG_RADIO_TX		=3


;****************************************
;*********** EVENTS SECTION *************
#define	OS_EVENTS_SYSTEM

#define	EVENT_AMOUNT		12

.EQU	LAMP_EVENT		=0
.EQU	EVENT_UART_INT_RX	=1
.EQU	EVENT_UART_INT_TX	=2
.EQU	EVENT_UART_INT_TX_END 	=3
.EQU	EVENT_UART_TX		=4

.EQU	EVENT_MODULATOR_INT 	=5
.EQU	EVENT_DEMODULATOR_INT	=6

.EQU	EVENT_RADIO_TX		=6;?????

;****************************************
;************ CACHE SECTION *************
#define	OS_CACHE_SYSTEM



;****************************************
;************ QUEUE SECTION *************
#define	OS_FBS_SYSTEM

#define	QUEUE_AMOUNT		5 	;количество очередей

.EQU	QUEUE_UART_RX		=0
.EQU	QUEUE_UART_TX		=1
.EQU	QUEUE_RADIO_TX		=2
.EQU	QUEUE_RADIO_RX		=3
.EQU	TEST_QUEUE		=4
;****************************************
;*********** THREAD SECTION *************

#define	THREAD_STACK_SIZE 	0x0A



#define THREAD_0 	TEST_THREAD_WRITE
#define THREAD_1 	THREAD_UART_TRANSMIT
#define THREAD_2 	THREAD_UART_RECEIVE
#define THREAD_3 	TEST_THREAD2;0
//#define THREAD_3 	THREAD_MODULATOR;TEST_THREAD0
#define THREAD_4 	TEST_THREAD1
#define THREAD_5 	TEST_THREAD2
#define THREAD_6 	TEST_THREAD2
#define THREAD_7 	TEST_THREAD2
//#define THREAD_0 THREAD_UART_RECEIVE

.INCLUDE "INT_VECTORS.asm"
.INCLUDE "asmOS.asm"

.INCLUDE "test_thread.asm"

.INCLUDE "UART.asm"

//.INCLUDE "MODULATOR.asm"

;#define THREAD_0 UART_THREAD

;****************************************
;****************************************
#ifdef  OS_FBS_SYSTEM
.DSEG
ADR_FBS_START_MEM: .BYTE (RAMEND-ADR_FBS_START_MEM)
#endif
