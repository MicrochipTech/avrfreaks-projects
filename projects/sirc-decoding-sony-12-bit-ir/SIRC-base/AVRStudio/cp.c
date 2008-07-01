//#define F_CPU 7372800L /*CPU Frequency*/
#define B(X)		_BV(X) /*For Bit Manupulation*/
#define REG(X,Y)	register unsigned char X asm(Y)

/*
 * Calculation:
 * f = 7372800Hz
 * Divider = 1024
 * Unit Count Duration = 0.00013888 sec
 * Total Count for START LOW = 18 (0.0025000 sec) range 17(0.0023)0x11 - 20(0.0027)0x14
 * Total Count for LOGICAL 1 = 13 (0.0018055 sec) range 12(0.0016)0x0c - 15(0.0020)0x0f
 * Total Count for LOGICAL 0 = 9  (0.0012500 sec) range  7(0.0009)0x07 - 11(0.0015)0x0b
 * Max Time : 0.0241666 sec (174)0xae
 */

#include <avr/io.h>
#include <avr/interrupt.h>


#define FALL_EDGE_TRIGGER 0x85|0x08
#define RISE_EDGE_TRIGGER 0xC5|0x08
#define STOP_CLOCK        0x80|0x08
//Output Compare Interrupt Control
#define ENABLE_OCI        0x30 
#define DISABLE_OCI       0x20
//Max Timeout for the Full Protocol to be Received
#define Time_out          0xAF 

void vInit_Input_Cap(void);
void vInit_Serial(void);
void send(unsigned char dta);
void bigdelay(void);

register union {
		unsigned char byte;
		struct{
				char b0:1;
				char b1:1;
				char b2:1;
				char b3:1;
				char b4:1;
				char b5:1;
				char b6:1;
				char b7:1;
		}bit;
	}flag asm("r2");
  
#define start		flag.bit.b0

unsigned int uiTimer;
REG(ucBit_Count,"r3");//register unsigned char ucBit_Count asm("r3");
 
int main(void)
{
	vInit_Input_Cap();
	vInit_Serial();
	start = 0;
	uiTimer = 0x00;
	ucBit_Count = 0X00;
	DDRD|=_BV(PD7);
	sei();
	while(1){
		PORTD^=_BV(PD7);
		bigdelay();
	}
	return 0;
}

void vInit_Input_Cap(void)
{
	TCCR1A = 0X00;
	TCCR1B = FALL_EDGE_TRIGGER;
	TIMSK  = DISABLE_OCI;//Only ICP is enabled
	OCR1A  = Time_out;//Set for Max Packet
	TIFR   = 0X00;
}

/* BAUD 115200 */
void vInit_Serial(void)
{
	UBRRH = 0;
	UBRRL = 3;
	UCSRB = _BV(RXEN) | _BV(TXEN);
	UCSRC = _BV(URSEL) | 6;
}

void send(unsigned char dta)
{
	while( !( UCSRA & (1<<UDRE)) );
	UDR = dta;
}

void bigdelay(void)
{
	unsigned int i,j;
	for(i=0;i<32000;i++)
	{
		for(j=0;j<6;j++)
		{
			asm("nop");
		}
	}
}

ISR(TIMER1_CAPT_vect)
{
	cli();
	TCCR1B = STOP_CLOCK;
	TCNT1 = 0X00;

	if(start){
		uiTimer = ICR1;
		UDR = (unsigned char)uiTimer;
		ucBit_Count++;
		
		if(ucBit_Count == 13){
			start = 0;
			TIMSK=DISABLE_OCI;//Clear the OCR Interrrupt Flag
			TIFR=B(OCF1A);//Clear the OCRI Flag
			TCCR1B = FALL_EDGE_TRIGGER;
		}else{
			TCCR1B = RISE_EDGE_TRIGGER;
		}
	}else{
		start = 1;
		ucBit_Count = 0;
		TIFR=B(OCF1A);//Clear the Flag
		TIMSK=ENABLE_OCI;//Set the OCR Interrrupt Flag
		TCCR1B = RISE_EDGE_TRIGGER;
	}
	sei();
}

ISR(TIMER1_COMPA_vect)
{
	TCCR1B = STOP_CLOCK;
	TCNT1 = 0X00;
	start = 0;
	ucBit_Count = 0;
	TIMSK=DISABLE_OCI;//Clear the OCR Interrrupt Flag
	//send(0xBA);
	TCCR1B = FALL_EDGE_TRIGGER;
}
