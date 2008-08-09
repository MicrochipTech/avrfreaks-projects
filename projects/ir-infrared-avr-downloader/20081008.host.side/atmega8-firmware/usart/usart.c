#include "usart.h"

ISR(USART_RXC_vect)
{
	status= UCSRA;
	data= UDR;
	
	PORTC&= ~(1<<0);
	
	if((status & ((1<<FE) | (1<<DOR))) == 0)
	{
		buffer_rx[idx_tulis_buffer]= data;
		
		if(++idx_tulis_buffer == PANJANG_BUFFER_RX)
			idx_tulis_buffer= 0;
		
		if(++counter_buffer >= PANJANG_BUFFER_RX)
			counter_buffer= 0;
	}
	
	PORTC|= (1<<0);
}

void inisialisasiUSART(void)
{	
	// USART dengan mode asinkron, rx complete interrupt, tx, 8 bit data, 2 stop bit
	UCSRA= 0;
	UCSRB= 0x98;
	UCSRC= 0x8E;
	
	UBRRL= NILAI_UBRR;
	UBRRH= (NILAI_UBRR>>8);
}

void kirimDataSerial(uchar data)
{
	// Tunggu hingga buffer kirim kosong
	while(!(UCSRA & (1<<UDRE)));
	
	// Kirim data
	UDR= data;
}
