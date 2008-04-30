//
//    ServoCam.c
//    
//    R. Scott Coppersmith
//
//    Build 26 March 2007

#include "ServoCam.h"


int main()
{
    int ServoData;
	int CommandRx=0;
    Init();
	pwmInit(0x0053);//0x16,0x53,0x90 = 22,83,144
	while (1)
	{
      if (UCSRA & (1<<RXC))
	  {

	  ServoData = UDR;

        switch (ServoData)
          {
		  case 'a':
		    {
			AutoPan();
			}
		  case 0x31 :
		    {
			setOCR1A(0x0060);
			CommandRx=0;
            break;
			}
          case 0x32 :
		    {
			setOCR1A(0x0055);
			CommandRx=0;
            break;
			}
          case 0x33 :
		    {
			setOCR1A(0x0050);
			CommandRx=0;
            break;
			}
          case 0x34 : 
		    {
			setOCR1A(0x0045);
			CommandRx=0;
            break;
			}
          case 0x35 : 
		    {
			setOCR1A(0x0040);
			CommandRx=0;
            break;
			}
          case 0x36 : 
		    {
			setOCR0(0x16);
			CommandRx=0;
            break;
			}
          case 0x37 : 
		    {
			setOCR0(0x34);
			CommandRx=0;
            break;
			}
          case 0x38 : 
		    {
			setOCR0(0x53);
			CommandRx=0;
            break;
			}
          case 0x39 : 
		    {
			setOCR0(0x71);
			CommandRx=0;
            break;
			}
          case 0x30 : 
		    {
			setOCR0(0x90);
			CommandRx=0;
            break;
			}												        
        }
      }
 
    }	
	return 0;
}
void AutoPan(void)
{
  int loop1;
  int loop2;
  while (!(UCSRA & (1<<RXC)))
  {
  for (loop1=22;loop1<144;loop1=loop1+1)
    {
    setOCR0(loop1);
	for (loop2=1;loop2<6;loop2++)
	  {
      _delay_loop_2(0);
	  }
	}
  for (loop1=22;loop1<144;loop1=loop1+1)
    {
    setOCR0(166-loop1);
	for (loop2=1;loop2<6;loop2++)
	  {
      _delay_loop_2(0);
	  }
	}
  }
}  
void pwmInit(int count)
{
	cli();
	setOCR1A(0x0050);//was3c
	setOCR0(count);
	setICR1(0x0514);  //20.0ms
	// Initialize Timer1.
    // Enable timer1 compare interrupt
	TIMSK = (1<<OCIE1A)|(1<<OCIE0);
	// Set PWM 
	TCCR0  = 0x6b;
	TCCR1A = (1<<COM1A1)|(1<<WGM11);
	TCCR1B = (1<<WGM13)|(1<<WGM12)|(1<<CS11)|(1<<CS10);
    sei();
}

void setOCR0(int count)
{  
    unsigned char sreg;
	sreg = SREG;
	cli();
	// Sets the compare value
	OCR0 = count;
    SREG = sreg;
}

void setOCR1A(int count)
{  
    unsigned char sreg;
	sreg = SREG;
	cli();
	// Sets the compare value
	OCR1A = count;
    SREG = sreg;
}

void setICR1(int count)
{    
    unsigned char sreg;
	sreg = SREG;
	cli();
	// Sets the compare value
	ICR1 = count;
	TCNT1 = 0x0000;	
    SREG = sreg;
}


SIGNAL(SIG_INPUT_CAPTURE1)
{
     if (TIFR & ~(1<<ICF1)) //if Flag is set
       TIFR |= (1<<ICF1); //clear it by writing a one to ICF1

}

SIGNAL(SIG_OUTPUT_COMPARE0)
{
     if (TIFR & ~(1<<OCF0)) //if output compare flag is set
       {
	   TIFR |= (1<<OCF0); //clear it by writing a one to OCF1A
       TCCR0  = 0x4b;
	   }
}

SIGNAL(SIG_OUTPUT_COMPARE1A)
{
     if (TIFR & ~(1<<OCF1A)) //if output compare flag is set
       {
	   TCCR0  = 0x6b;
	   TCNT0  = 0;
       TIFR |= (1<<OCF1A); //clear it by writing a one to OCF1A
	   }
}

SIGNAL(SIG_OUTPUT_COMPARE1B)
{
     if (TIFR & ~(1<<OCF1B)) //if output compare flag is set
       {
	   //TCCR0  = 0x6b;
	   TIFR |= (1<<OCF1B); //clear it by writing a one to OCF1B
       }
}


void setbaud(BaudRate br)
   {
   UART_BAUD_REG = br;
   }



void Init(void)
   {
   cli();

   DDRA  = 0xFF;
   DDRB  = 0x48;
   SPCR = (1<<SPE);
   DDRC  = 0xFF;
   DDRD  = 0xB2;     
    
   PORTA = 0x00;
   PORTC = 0x00;
   PORTD = 0x00;

   
    // Enable receiver and transmitter
 //   UCSRB = (1<<RXEN)|(0<<TXEN)|(0<<RXCIE)|(0<<UDRIE);

    // Async. mode, 8N1
//    UCSRC = (0<<UMSEL)|(0<<UPM0)|(0<<USBS)|(3<<UCSZ0)|(0<<UCPOL);

   UART_CONTROL_REG = 0x18;   //Transmitter enabled, receiver enabled, no ints
   setbaud(BAUD9600/2);
   
   // Enable INT0 interrupt
   GICR = 0x40; 
   TCCR1A = 0;
   TCCR1B = 0;
   TIMSK = 0;
   TIFR = 0;
   sei();
   }

