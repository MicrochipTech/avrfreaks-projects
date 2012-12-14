/*
 * ridaisai12_test.c
 *
 * Created: 2012/11/21 7:47:44
 *  Author: 123
 */ 


#include <avr/io.h>

#define F_CPU 8000000UL

#define DDR_SPI DDRB
#define DD_MISO 4
#define DD_MOSI 3
#define DD_SCK 5
#define DD_SS 2

#define MAXangle 150
#define MINangle 50


void PWM_Init();
void PWM_Speed(uint8_t,uint8_t,uint8_t,uint8_t);
void SPI_SlaveInit(void);
char SPI_SlaveReceive(void);


int main(void)
{
	DDRD = 0xFF;
	PWM_Init();
	SPI_SlaveInit();
	DDRB |= (1<<PORTB1);
	
	char receive;
    while(1)
    {
		receive = SPI_SlaveReceive();
		switch(receive){
			case 'a':
			//clockwise
				PWM_Speed(150,0,0,0);
				break;
			case 'b':
			//anti-clockwise
				PWM_Speed(0,150,0,0);
				break;
			case 'c':
				PWM_Speed(0,0,0,0);
				break;
			case 'd':
			PWM_Speed(0,0,0,MAXangle);
			break;
			case 'e':
			PWM_Speed(0,0,0,MINangle);
			break;
			
			default:
				break;
		}				
    }
}

void PWM_Speed(uint8_t spd0B,uint8_t spd0A,uint8_t spd2B,uint8_t spd1A){
	OCR0A = spd0A;
	OCR0B = spd0B;
	OCR1A = spd1A;
	OCR2B = spd2B;
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
	//timer1 and 2 for servo
	//prescale clk/256
	TCCR1A = (1<<COM1A1)|(0<<COM1A0);
	TCCR1A |= (0<<WGM11)|(1<<WGM10);
	TCCR1B = (1<<WGM12);
	TCCR1B |= 0x04;
	
	TCCR2A = (1<<COM2B1)|(0<<COM2B0);
	TCCR2A |= (1<<WGM21)|(1<<WGM20);
	
	TCCR2B = 0x06;
	
}

void SPI_SlaveInit(void)
{
	/* Set MISO output, all others input */
	DDR_SPI = (1<<DD_MISO);
	/* Enable SPI */
	SPCR = (1<<SPE)|(1<<SPR0)|(1<<SPR1);
}

char SPI_SlaveReceive(void)
{
	while(!(SPSR & (1<<SPIF)));
	
	return SPDR;
}
