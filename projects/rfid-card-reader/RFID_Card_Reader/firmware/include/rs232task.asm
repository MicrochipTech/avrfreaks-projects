/*
Running at 11.0592 external quarz
*/

.def    argument=r17  
.def    return = r18
.def    axl=r19
.def    axh=r20

.def	bxl=r21
.def    bxh=r22
	

#define UBRR_VAL	71/*11.0592*/ //51/*8Mhz*/	


#define RESPONSE_OK 1
#define RESPONSE_FAILURE 0x80

.SET STATUS_VOID=0
.SET STATUS_RX=1
.SET STATUS_TX=2

.SET STATUS_ERROR=4
.SET STATUS_PREPARE=5	;ENTER is pressed/accepted

.dseg


RxByte: .byte 1

TxByte: .byte 1
TxCurrentRef: .byte 1

;rs232_output: .byte RS232_OUTPUT_BUFF_SIZE



TxRxStatus: .byte 1
.cseg


.include "include/rs232op.asm"

RS232_Tx_Task:

 ;reset status
 ldi temp,STATUS_VOID
 sts TxRxStatus,temp

 ;.SET RX_TX_TIMEOUT=7
 ;_INTERRUPT_DISPATCHER_INIT temp,RX_TX_TIMEOUT

 rcall usart_init_polling

 _THRESHOLD_BARRIER_WAIT InitTasksBarrier,TASKS_NUMBER 



txmain: 

    _EVENT_WAIT RS232_READY_EVENT
    

	lds argument,facilityCode
	rcall rs232_send_byte

    lds argument,cardCode
	rcall rs232_send_byte

    lds argument,cardCode+1
	rcall rs232_send_byte

					
		
rjmp txmain






RS232_Rx_Task:
  sbi DDRD,PORTD7
  cbi PORTD,PORTD7

.SET RX=5
_INTERRUPT_DISPATCHER_INIT temp,RX

_THRESHOLD_BARRIER_WAIT InitTasksBarrier,TASKS_NUMBER 

rxmain:

    clr bxl
_INTERRUPT_WAIT	RX
	lds temp,RxByte
	cpi temp,RESPONSE_FAILURE
    brne rsint_rx_exit
	
	//set the alarm on
    ldi bxl,RESPONSE_FAILURE
	

rsint_rx_exit:
_INTERRUPT_END RX

  cpi bxl,RESPONSE_FAILURE
  brne rxmain
;turn on the zumer -> error comes from PC
  cbi PORTD,PORTD7	
 _SLEEP_TASK 255
 _SLEEP_TASK 255
 _SLEEP_TASK 255
 _SLEEP_TASK 255
 _SLEEP_TASK 255
 sbi PORTD,PORTD7	
 _SLEEP_TASK 255
 _SLEEP_TASK 255
 _SLEEP_TASK 255
 _SLEEP_TASK 255
 cbi PORTD,PORTD7
 _SLEEP_TASK 255
 _SLEEP_TASK 255
 _SLEEP_TASK 255
 _SLEEP_TASK 255
 _SLEEP_TASK 255
 sbi PORTD,PORTD7	
 _SLEEP_TASK 255
 _SLEEP_TASK 255
 _SLEEP_TASK 255
 _SLEEP_TASK 255
 cbi PORTD,PORTD7


rjmp rxmain

/*****USART Init Interrupt polling mode********************
*@USAGE:temp
*/
usart_init_polling:
	ldi temp,high(UBRR_VAL)
	out UBRRH,temp 

	ldi temp,low(UBRR_VAL)
	out UBRRL,temp

	; Enable receiver interrupt
	ldi temp,(1 << RXCIE)|(1<<RXEN)|(1<<TXEN)
	out UCSRB,temp

	; Set frame format: 8data,EVEN parity check,1stop bit by default  PC communication
	;When BT is reprogram to parity EVEN use this setting
	ldi temp, (1<<URSEL)|(1<<UPM1)|(1 << UCSZ1) | (1 << UCSZ0)

	;default blue thooth setting 9600N81  BLUETOOTH communication
	;start with this to change parity to EVEN
	;ldi temp, (1<<URSEL)|(1 << UCSZ1) | (1 << UCSZ0)
	
	out UCSRC,temp

ret
/*****USART Init Interrupt driven mode********************
*@USAGE:temp
*/
/*
usart_init_interrupt:
	ldi temp,high(UBRR_VAL)
	out UBRRH,temp 

	ldi temp,low(UBRR_VAL)
	out UBRRL,temp

	; Enable receiver and tranciever interrupt
	ldi temp, (1 << RXCIE)|(1<<RXEN)|(1<<TXEN)
	out UCSRB,temp

	; Set frame format: 8data,EVEN parity check,1stop bit by default  PC communication
	;When BT is reprogram to parity EVEN use this setting
	ldi temp, (1<<URSEL)|(1<<UPM1)|(1 << UCSZ1) | (1 << UCSZ0)

	;default blue thooth setting 9600N81  BLUETOOTH communication
	;start with this to change parity to EVEN
	;ldi temp, (1<<URSEL)|(1 << UCSZ1) | (1 << UCSZ0)
	
	out UCSRC,temp

ret
*/

RxComplete:
_PRE_INTERRUPT
    
	//error check
    in temp,UCSRA
	sbrs temp,FE
	rjmp rxi_dor
	rjmp rxi_error 
rxi_dor:
	sbrs temp,DOR 
	rjmp rxi_pe
	;dor error
	rjmp rxi_error        
rxi_pe:
	sbrs temp,PE 
	rjmp rxi_char
	;pe error
	rjmp rxi_error

rxi_char: 
 	in temp, UDR
	sts RxByte,temp	

_keDISPATCH_DPC RX

rxi_error:
 rcall usart_flush 
_POST_INTERRUPT
reti

/***************flash buffers********************
*/
usart_flush:
	sbis UCSRA, RXC
ret
	in temp, UDR
	rjmp usart_flush
