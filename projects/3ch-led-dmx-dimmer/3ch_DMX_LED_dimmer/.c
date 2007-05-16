/*
===========================================================================
 Filemane	: 3ch_DMX_LED_Dimmer.c
 Function	: 3ch DMX reception dimmer function, LED dimmer uses PWM
 Compiler  	: WinAVR 20050214  (http://sourceforge.net/projects/winavr/)
 MCU       	: Tiny2313
 XTAL      	: 12 MHz
 Creation	: 12/05/2007 - V0.1



 ===========================================================================
*/





#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/signal.h>


#define CTL_PORT	PORTD
#define LED_CH1		PD3		
#define LED_CH2		PD4		 
#define LED_CH3		PD5 

#define FOSC 12000000L		
#define BAUD 250000
#define MYUBRR FOSC/16/BAUD-1

#define TIMER_CLOCK	10000		// LED flashing frequency in 1kHz

void init_usart(unsigned int ubrr);

/* module global variables */
volatile int tick;
volatile int count,bytecount,startdmx;
volatile int dmxcount,ch1, LED_REG;
volatile int i,j,k,ij;

uint8_t ChannelData[3];

//volatile int ChannelData;

void Timers_init(void)
{
/*
	The biggest pain in the neck with timers is the delcarations that follow.
	A couple of rules must apply:
	1.  Assume every chip is different to last one you used.  Therefore 
	the control registers need to be verified
	2.  Check the revision of GCC, you will either use INTERUPT, 
	SIGNAL or ISR for your interupt vector
*/
/*
// use CLK/64 prescale value, clear timer/counter on compareA match                               
    TCCR0B = (1<<CS10) | (1<<CS11) | (1<<WGM12);
    
// preset timer1 high/low byte
    OCR0A = ((FOSC/2/64/TIMER_CLOCK) - 1 );   

// enable Timer/Counter1,Output Compare A Match Interrupt
    TIMSK  = (1<<OCIE0A);  
*/



// use CLK/64 prescale value, clear timer/counter on compareA match                               
    TCCR1B = (1<<CS10) | (1<<CS11) | (1<<WGM12);
    
// preset timer1 high/low byte
    OCR1A = ((FOSC/2/64/TIMER_CLOCK) - 1 );   

// enable Timer/Counter1,Output Compare A Match Interrupt
    TIMSK  = (1<<OCIE1A);   
	
	        
}

SIGNAL (SIG_TIMER1_COMPA)    // handler for TIMER1 COMPA Timer/Counter1 Compare Match A
{
	tick++;
	return;
}

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


	if(dmxcount==startdmx)ChannelData[0]=data;
	if(dmxcount==(startdmx+1))ChannelData[1]=data;
	if(dmxcount==(startdmx+2))ChannelData[2]=data;
	dmxcount++;

	
}


void ioinit (void) 	{


	DDRB=0x00;   //portb as input
	PORTB=0xff;	 // enable internal pull up resistors



	DDRD = 0b10111000;  // Bits 0, 1, 2 and 6 are inputs
    PORTD = 0b01000110; // enable internal pulll up resistors


	}

int main (void)
{
	char units, tens, hundreds;
	uint8_t CNum;

	ioinit();
	Timers_init();
    tick =0;

	init_usart(MYUBRR);

// BCD Switch definitions

	/*
	Units
		Sw 1 - PB4 correct position
		Sw 2 - PB6 shift left 1
		Sw 4 - PB7 shift left 1
		Sw 8 - PB5 shift right 2
	*/
	units    = ((~PINB & 0x80)>>5)|((~PINB & 0x40)>>5)|((~PINB & 0x20)>>2)|((~PINB & 0x10)>>4); 

	/*
	Tens
		Sw 1 - PB0 correct position
		Sw 2 - PB2 shift left 1
		Sw 4 - PB3 shift left 1
		Sw 8 - PB1 shift right 2
	*/
	tens     = ((~PINB & 0x02)<<2)|((~PINB & 0x08)>>1)|((~PINB & 0x04)>>1)|((~PINB & 0x01)); 

	/*
	Hundreds
		Sw 1 - PD2 shift left 2
		Sw 2 - PD1 correct position
		Sw 4 - PD6 shift left 4
		Sw 8 - PD5 shift left 2
	*/
	hundreds = 0;

	hundreds = (((~PIND & 0x40)>>4)|((~PIND & 0x02))|((~PIND & 0x04)>>2)); 

	startdmx = 100*hundreds+10*tens+units;
//	startdmx = 10*tens+units;
//	startdmx = 151;

	sei();
	while(1){
		if (startdmx == 0) startdmx = 1;

		if (tick == ChannelData[0] ) {
			PORTD &= ~(1<<LED_CH1); // used for Common Anode
//			PORTD |= (1<< LED_CH1);	// used for Common Cathode
		} 
		if (tick == ChannelData[1] ) {
			PORTD &= ~(1<<LED_CH2);
//			PORTD |= (1<< LED_CH2);
		} 
		if (tick == ChannelData[2] ) {
			PORTD &= ~(1<<LED_CH3);
//			PORTD |= (1<< LED_CH3);
		} 

		if (tick == 255) {
			tick = 0;
			if (ChannelData[0] > 0) {
				PORTD |= (1<< LED_CH1);
//				PORTD &= ~(1<<LED_CH1);
			}
			if (ChannelData[1] > 0) {
				PORTD |= (1<< LED_CH2);
//				PORTD &= ~(1<<LED_CH2);
			}
			if (ChannelData[2] > 0) {
				PORTD |= (1<< LED_CH3);
//				PORTD &= ~(1<<LED_CH3);
			}
		}
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


/* 250kbps 8N2  */   
/* set baud rate */
   	UBRRH = (unsigned char)(ubrr>>8);
   	UBRRL = (unsigned char)ubrr;
} 



