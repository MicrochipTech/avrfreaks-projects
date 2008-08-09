#ifndef __usart_h_included__
#define __usart_h_included__

#include <avr/io.h>
#include <avr/interrupt.h>

#ifndef F_CPU
	#define F_CPU 					12000000UL
#endif

#ifndef uchar
	#define uchar					unsigned char
#endif

#define BAUDRATE					2400
#define NILAI_UBRR					((F_CPU/(BAUDRATE*16L))-1)

#define aktifkanTransmitter()		UCSRB|= (1<<TXEN)
#define matikanTransmitter()		UCSRB&= ~(1<<TXEN)

#define aktifkanReceiver()			UCSRB|= (1<<RXEN)
#define matikanReceiver()			UCSRB&= ~(1<<RXEN)

#define aktifkanReceiveInterrupt()	UCSRB|= (1<<RXCIE)
#define matikanReceiveInterrupt()	UCSRB&= ~(1<<RXCIE)

#define PANJANG_BUFFER_RX			256

volatile uchar status, data;
volatile uchar buffer_rx[PANJANG_BUFFER_RX];
volatile uchar idx_buffer_terima;
/*volatile unsigned int idx_tulis_buffer;
volatile unsigned int idx_baca_buffer;
volatile unsigned int counter_buffer;*/

volatile uchar idx_tulis_buffer;
volatile uchar idx_baca_buffer;
volatile uchar counter_buffer;

void inisialisasiUSART(void);
void kirimDataSerial(uchar data);

#endif
