#include <avr/pgmspace.h>

#define SER_TX	0			// set serial port pin
#define BAUD	9600		// set port speed

#define BAUD_US	1000000/BAUD

/*** ss_putc ******************************************************************
 *	Software Serial Port: Transmit char (8n1)
 *****************************************************************************/
void ss_putc(uint8_t tx_buf) {
	uint8_t i;
	
//	DDRB |= _BV(SER_TX);		// set to output
	PORTB &= ~(_BV(SER_TX));	// set low (start bit)
	_delay_us(BAUD_US);			// delay for start bit
	
	for(i=0; i<8; i++) {
		if(tx_buf&1)
			PORTB |= _BV(SER_TX);		// transmit 1
		else
			PORTB &= ~(_BV(SER_TX));	// transmit 0
		
		_delay_us(BAUD_US);				// delay for last data bit
		
		tx_buf >>= 1;					// shift in next bit
	}
	
	PORTB |= _BV(SER_TX);		// set high (stop bit)
//	DDRB &= ~(_BV(SER_TX));	// set to input (with pull-up)
	_delay_us(BAUD_US);			// delay for stop bit, in case user sends another byte immediately on return
}
// ss_putc(uint8_t tx_buf)

/*** ss_putr ******************************************************************
 *	Software Serial Port: send line termination (CR+LF)
 *****************************************************************************/
void ss_putr(void) {
	ss_putc('\n');
	ss_putc('\r');
}
// void ss_putr(void)

/*** ss_puts ******************************************************************
 *	Software Serial Port: Transmit string followed by CR+LF
 *****************************************************************************/
//const char str[]="Hello World!\n\r";// ss_puts(str);
//const PROGMEM char *str="Hello World!\n\r"; ss_puts(str);
//ss_puts(PSTR("Hello World!"));
/*
void ss_puts(const char *tx_byte) {   
	unsigned char c;  

	while( (c=pgm_read_byte_near(tx_byte++)) )
		ss_putc(c);
	ss_putr();
}
*/
// ss_puts(const char *)

/*** ss_putn ******************************************************************
 *	Software Serial Port: Transmit 16-bit number (dec)
 *****************************************************************************/
/*
void ss_putn(uint16_t num) {   
	uint8_t *cc="     ";
	
	itoa(num,cc,10);
	
	for(num=0;num<5;num++)
		ss_putc(cc[num]);
}
*/
// ss_putn(uint8_t)

/*** ss_putn ******************************************************************
 *	Software Serial Port: Transmit 8-bit number (dec)
 *****************************************************************************/
/*
void ss_putn(uint8_t num) {   
	uint8_t *cc="   ";
	
	itoa(num,cc,10);
	
	ss_putc(cc[0]);
	ss_putc(cc[1]);
	ss_putc(cc[2]);
}
*/
// ss_putn(uint8_t)

/*** ss_putn ******************************************************************
 *	Software Serial Port: Transmit 8-bit number (hex)
 *****************************************************************************/
/*
void ss_putn(uint8_t num) {   
	uint8_t *cc="  ";
	
	itoa(num,cc,16);
	
	if(cc[1] == '\0')	// optional; right align / '0' pad
		ss_putc('0');	// pad with 0 if only 1
	
	ss_putc(cc[0]);
	ss_putc(cc[1]);
}
*/
// ss_putn(uint8_t)

/*** ss_getc ******************************************************************
 *	Software Serial Port: Receive char (8n1)
 *****************************************************************************/
/*
uint8_t ss_getc(void) {
	return 0;//rx_data;
}
*/
// uint8_t ss_getc(void)
