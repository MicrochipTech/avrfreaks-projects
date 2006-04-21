/* DISCLAIMER: I wrote this code a little while ago now, 
so please be kind to my coding techniques shown in this sloppy example. 
I'm not even sure this will work straight off the bat but I think it should 
(as another poster mentioned things have changed of late with 
Start Byte codes, so this code is by no means 100% reliable - check 
the latest DMX spec revision [if you can find it...]). 
Hopefully this example can help others.
Source: AVRFREAKS.NET
author: oddbudman
*/

/* Modified by Matt Edwards for Tiny 2313 */



#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/signal.h>


#define LED_PORT	PORTB
#define LED_CH8		PB7		// 
#define LED_CH7		PB6		// 
#define LED_CH6		PB5		// 
#define LED_CH5		PB4		// 
#define LED_CH4		PB3		// 
#define LED_CH3		PB2		// 
#define LED_CH2		PB1		// 
#define LED_CH1		PB0		// 

// #define CTL_PORT	PORTD
#define SPI_PORT	PORTD
#define SPI_IN	PIND
#define SC	PD2		// 
#define SD	PD1		// 
#define CS	PD3		// 

#define FOSC 12000000L		
#define BAUD 250000
#define MYUBRR FOSC/16/BAUD-1

#define sei()   __asm__ __volatile__ ("sei" ::)
#define cli()   __asm__ __volatile__ ("cli" ::)
#define NOP()   __asm__ __volatile__ ("nop" ::)

void init_usart(unsigned int ubrr);
int HC165_Receive(void);


volatile int count,bytecount,startdmx;
volatile int count,dmxcount,ch1,ch2,ch3,ch4,ch5,ch6,ch7,ch8, LED_REG;
volatile int i,j,k,ij;

SIGNAL (SIG_USART0_RX)
{
	uint8_t status, data;
	status=UCSRA;
	data=UDR;

	//Frame error | DOR error detects a break and resets dmx count
	if(status&0x10){dmxcount=0;return;}
	if(status&0x08){dmxcount=0;return;}

	//check for start condition
	if(dmxcount==0){
	 	if(data==0){
			dmxcount++;
			i=20000;
			return;
		} 
	}

	if(dmxcount==startdmx)ch1=data;
	if(dmxcount==(startdmx+1))ch2=data;
	if(dmxcount==(startdmx+2))ch3=data;
	if(dmxcount==(startdmx+3))ch4=data;
	if(dmxcount==(startdmx+4))ch5=data;
	if(dmxcount==(startdmx+5))ch6=data;
	if(dmxcount==(startdmx+6))ch7=data;
	if(dmxcount==(startdmx+7))ch8=data;
	dmxcount++;
	
}


void ioinit (void) 	{
	char temp_l, temp_h;
	int temp_hc165;
	char units, tens, hundreds;

	DDRB=0xff;   //porta as output
	PORTB=0x00;

//	DDRB = 0x00;  // set to input
//  PORTB = 0xff; // enable internal pull ups

//	DDRC = 0x00;  // set to input
//  PORTC = 0xff; // enable internal pull ups

	DDRD = 0b11111101;  // set to input/OUTPUT
    PORTD = 0b00000010; // enable internal pull ups

	temp_hc165 = HC165_Receive();
	temp_l = (unsigned char)(temp_hc165);
	temp_h = (unsigned char)(temp_hc165>>8);
	units    = ((temp_h & 0x08)>>3)|((temp_h & 0x04)<<1)|(temp_h & 0x02)|((temp_h & 0x01)<<2); 
	tens     = (((temp_h & 0x80)>>1)|((temp_h & 0x40)>>1)|((temp_h & 0x20)<<2)|((temp_h & 0x10)))>>4; 
	hundreds = (((temp_l & 0x80)>>1)|((temp_l & 0x40)>>1)|((temp_l & 0x20)<<2)|((temp_l & 0x10)))>>4; 
	startdmx = 100*hundreds+10*tens+units;
	}

int main (void)
{

	ioinit();
	init_usart(MYUBRR);
//	startdmx = 5;
	i=20000;
	sei();
	while(1){
		if (startdmx == 0) startdmx = 1;
		i--;

		if (ch1 >= 127)	{
				LED_REG |= (1<<LED_CH1);
			}
			else LED_REG &= ~(1<<LED_CH1);		
		if (ch2 >= 127)	{
				LED_REG |= (1<<LED_CH2);
			}
			else LED_REG &= ~(1<<LED_CH2);
		if (ch3 >= 127)	{
				LED_REG |= (1<<LED_CH3);
			}
			else LED_REG &= ~(1<<LED_CH3);
		if (ch4 >= 127)	{
				LED_REG |= (1<<LED_CH4);
			}
			else LED_REG &= ~(1<<LED_CH4);
		if (ch5 >= 127)	{
				LED_REG |= (1<<LED_CH5);
			}
			else LED_REG &= ~(1<<LED_CH5);
		if (ch6 >= 127)	{
				LED_REG |= (1<<LED_CH6);
			}
			else LED_REG &= ~(1<<LED_CH6);
		if (ch7 >= 127)	{
			LED_REG |= (1<<LED_CH7);
			}
			else LED_REG &= ~(1<<LED_CH7);
		if (ch8 >= 127)	{
			LED_REG |= (1<<LED_CH8);
			}
			else LED_REG &= ~(1<<LED_CH8);
		if (i == 0)	{
			LED_REG = 0x00;
			ch1=0;ch2=0;ch3=0;ch4=0;ch5=0;ch6=0;ch7=0;ch8=0;
		}

		LED_PORT = LED_REG;

	}
	return 1;
}

void init_usart(unsigned int ubrr) {
//USART Control and Status Register A   UCSRA - Not used in init
   
//USART Control and Status Register B   UCSRB
/*
Bit 7   RXCIE: RX Complete Interrupt Enable         1
Bit 6   TXCIE: TX Complete Interrupt Enable         0
Bit 5   UDRIE: USART Data Register Empty Interrupt Enable   0
Bit 4   RXEN: Receiver Enable           1
Bit 3   TXEN: Transmitter Enable    	0   
Bit 2   UCSZ2: Character Size           1
Bit 1   RXB8: Receive Data Bit 8        0
Bit 0   TXB8: Transmit Data Bit 8       0
*/
	UCSRB = 0x94;

/*USART Control and Status Register C   UCSRC
Bit 7   URSEL: 							0
Bit 6   UMSEL: USART Mode Select   		0
Bit 5               					0
Bit 4   UPM1:0: Parity Mode      		0
Bit 3   USBS: Stop Bit Select      		1       2 stop bits
Bit 2   UCSZ1:0: Character Size      	1		8 data bits
Bit 1                					1
Bit 0   UCPOL: Clock Polarity      		0
*/
	UCSRC=0x0e;


/*  250kbps 8N2  */   
/* set baud rate */
   	UBRRH = (unsigned char)(ubrr>>8);
   	UBRRL = (unsigned char)ubrr;
} 


int HC165_Receive(void)
{
	unsigned int temp =0, mask;		//16-bit
	SPI_PORT &=~(1<<CS);				//Make CS low so HC165 loads data
//	__asm__ __volatile__ ("nop"::);
	SPI_PORT &=~(1<<SC);				//Set clock low
//	__asm__ __volatile__ ("nop"::);
	SPI_PORT |= (1<<CS);				//Make CS high so HC165 sharts sending data
	for (mask=(1<<15); mask!=0; mask>>=1)	//16 bits
	{
//		__asm__ __volatile__ ("nop"::);
//		__asm__ __volatile__ ("nop"::);
//		__asm__ __volatile__ ("nop"::);
		SPI_PORT &=~(1<<SC);				//Clock low
//		__asm__ __volatile__ ("nop"::);
//		__asm__ __volatile__ ("nop"::);
//		__asm__ __volatile__ ("nop"::);
		if (SPI_IN &(1<<SD))				//Sample data bit
			temp |= mask;					//Must be a one
		SPI_PORT |=(1<<SC);					//Clock high
	}
//	PORTD &=~(1<<CS);				//Make CS low so HC165 loads data
//   PORTD |= (1<<CS);				//Make CS high so HC165 stops sending data
	return(~temp);
} 
