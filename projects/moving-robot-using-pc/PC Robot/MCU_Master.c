/*
 * ridaisai12_master.c
 *
 * Created: 2012/11/22 13:54:55
 *  Author: 123
 */ 


#include <avr/io.h>
#include <avr/interrupt.h>

#define F_CPU 8000000UL
#define USART_BAUDRATE 9600
#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)

#define DDR_SPI DDRB
#define DD_MISO 4
#define DD_MOSI 3
#define DD_SCK 5
#define DD_SS 2
#define normal 150

#define clockwise 1
#define anticlockwise 2

void SPI_MasterInit(void);
void SPI_MasterTransmit(char);
void PWM_Init();
void PWM_Speed(uint8_t ,uint8_t ,uint8_t ,uint8_t );
void usart_init();
void usart_putch(unsigned char send);
unsigned int usart_getch();

int main(void)
{
	PWM_Init();
	SPI_MasterInit();
	usart_init();
	DDRD |= 0x60 | (1<<PORTD3);
	DDRB |= (1<<PORTB1)|(1<<PORTB0);
	char receive;
	char slave_data = 'c';

	while(1)
	{
		SPI_MasterTransmit(slave_data);
		receive = usart_getch();
		switch(receive){
			case 'k':
			cli();
			//forward
			PWM_Speed(normal,0,normal,0);
			slave_data = 'c';
			
			break;
			
			case 'j' :
			cli();
			//anticlockwise
			PWM_Speed(normal,0,0,normal);
			slave_data = 'b';
			
			
			break;
			case  'l' :
			cli();
			//clockwise
			PWM_Speed(0,normal,normal,0);
			slave_data = 'a';
			
			
			break;
			
			case 's' :
			cli();
			//stop
			PWM_Speed(0,0,0,0);
			slave_data = 'c';
			
			break;
			case 'm' :
			//stop
			cli();
			PWM_Speed(0,0,0,0);
			slave_data = 'c';
			
			break;
			default:
			break;
		}
		
	}
}

void PWM_Init(void){
	//non-inverted mode
	//fast-PWM mode
	//prescalar clk/8
	//F_CPU 1MHz
	TCCR0A = (1<<COM0A1)|(0<<COM0A0);
	TCCR0A |= (1<<COM0B1)|(0<<COM0B0);
	TCCR0A |= (1<<WGM01)|(1<<WGM00);
	
	TCCR0B = 0x02;

	TCCR1A = (1<<COM1A1)|(0<<COM1A0);
	TCCR1A |= (0<<WGM11)|(1<<WGM10);
	TCCR1B = (1<<WGM12);
	TCCR1B |= 0x02;
	
	TCCR2A = (1<<COM2B1)|(0<<COM2B0);
	TCCR2A |= (1<<WGM21)|(1<<WGM20);
	
	TCCR2B = 0x02;
	
}

void PWM_Speed(uint8_t spd0B,uint8_t spd0A,uint8_t spd2B,uint8_t spd1A){
	OCR0A = spd0A;
	OCR0B = spd0B;
	OCR1A = spd1A;
	OCR2B = spd2B;
}

void usart_init()
{
	UCSR0B |= (1 << RXEN0) | (1 << TXEN0);
	// Turn on the transmission and reception circuitry
	UCSR0C |= (0<<UMSEL00)|(0<<UPM00)|(0<<USBS0) | (1 << UCSZ00) | (1 << UCSZ01);
	// Use 8-bit character sizes
	
	UBRR0L = BAUD_PRESCALE; // Load lower 8-bits of the baud rate value..
	// into the low byte of the UBRR register
	UBRR0H = 0x0F&(BAUD_PRESCALE >> 8); // Load upper 8-bits of the baud rate value..
	// into the high byte of the UBRR register
}

void usart_putch(unsigned char send)
{
	while (!(UCSR0A & (1 << UDRE0))); // Do nothing until UDR is ready..
	// for more data to be written to it
	UDR0 = send; // Send the byte
}

unsigned int usart_getch()
{
	while ((UCSR0A & (1 << RXC0)) == 0);
	// Do nothing until data has been received and is ready to be read from UDR
	return(UDR0); // return the byte
}

void SPI_MasterInit(void)
{
	/* Set MOSI and SCK output, all others input */
	DDR_SPI = (1<<DD_MOSI)|(1<<DD_SCK)|(1<<2);
	/* Enable SPI, Master, set clock rate fck/16 */
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0)|(1<<SPR1);
}
void SPI_MasterTransmit(char cData)
{
	/* Start transmission */
	SPDR = cData;
	/* Wait for transmission complete */
	while(!(SPSR & (1<<SPIF)))
	;
}
