#include <avr/io.h>
#include <avr/interrupt.h>

#include "usart.h"

// circular buffer for data received from usasrt
#define usart_rx_buf_size  32
static volatile uint8_t usart_rx_buf_head = 0;
static volatile uint8_t usart_rx_buf_tail = 0;
static volatile uint8_t usart_rx_buf[usart_rx_buf_size];

//	ex : usart_init(&PORTC, &USARTC0, 9600);
void usart_init(PORT_t *port, USART_t *usart, uint32_t baudrate)
{
    //calculate UBRR value. It is 8UL if using double speed transfer
    //((F_CPU/(BSCALE**2 * BAUDRATE * 16UL))-1) 
    //uint32_t bselvalue = ( (F_CPU / (baudrate * 16)) - 1 );
    // this is supposed to be a better calculation of bselvalue 
    uint16_t bselvalue = ((F_CPU / 16 + baudrate / 2) / baudrate - 1);

    //XCLK is 0 for C0 and 5 for C1
	// RXD0 as input. this is PC2 for C0 and PC6 for USARTC1	 
	port->DIRCLR   = PIN2_bm;
  	// TXD0 as output. this is PC3 for C0 and PC7 for USARTC1
	port->DIRSET   = PIN3_bm;
	
	usart->CTRLC = USART_CMODE_ASYNCHRONOUS_gc | USART_PMODE_DISABLED_gc | USART_CHSIZE_8BIT_gc ; //Asynchronous, No parity , 8BIT, 
    //USARTC0.CTRLA =  USART_DREINTLVL_LO_gc ; //done in exampl e, not sure why
    usart->CTRLA |= USART_RXCINTLVL_LO_gc; //Enable interrup on receive

    usart->BAUDCTRLA = (uint8_t)bselvalue; //set baudrate
    usart->BAUDCTRLB = (0 << USART_BSCALE0_bp)|(bselvalue >> 8); // set scale factor
    usart->CTRLB &= ~USART_CLK2X_bm; // disable usart 2x 
    //usart->CTRLB |= USART_CLK2X_bm;
	usart->CTRLB |= USART_RXEN_bm ; // enable USART receiver
    usart->CTRLB |= USART_TXEN_bm ; // enable USART transmit
}

void usart_rx_buf_flush(void)
{
    usart_rx_buf_head = usart_rx_buf_tail;
}

uint8_t usart_rx_buf_has_data(void)
{
    return  (usart_rx_buf_size + usart_rx_buf_head - usart_rx_buf_tail ) % usart_rx_buf_size;
}

void usart_rx_buf_put_byte(uint8_t data)
{
	uint8_t idx = (usart_rx_buf_head + 1) % usart_rx_buf_size;
	if (idx != usart_rx_buf_tail) { // do not explode buffer
		usart_rx_buf[usart_rx_buf_head] = data;
		usart_rx_buf_head = idx;
	}
}

uint8_t usart_rx_buf_get_byte(void)
{
	if (usart_rx_buf_head == usart_rx_buf_tail) {
		return -1;
	} else {
		uint8_t data = usart_rx_buf[usart_rx_buf_tail];
		usart_rx_buf_tail = (usart_rx_buf_tail + 1) % usart_rx_buf_size;
		return data;
	}
}


void usart_send_byte(uint8_t byte)
{
    while ((USARTC0.STATUS & USART_DREIF_bm) == 0) ;
    USARTC0.DATA = byte ; 
}





