
#include <Mega16.h>
#include <Delay.h> 
//-----------------------------------------
void setcounter0(unsigned char d)
{
TIMSK=0x00;        //Fast PWM Mode
TCCR0=0x7B;
OCR0=d;
TCNT0=0x00;
}
//------------------------------------------

unsigned char i=0x00;
unsigned char j=0x00;
//-----------------------
void main (void)
{
//----------------------
DDRA=0x00;
DDRB=0x08;
DDRC=0x00;
DDRD=0x00;
//-----------------------
setcounter0(i);

//TIMSK=0x00;        //normal mode
//TCCR0=0x15;
//OCR0=0xFF;
//TCNT0=0x00;
//---------------------- 
done:
for (i=0x01; i<0xff; i++)
    {
     setcounter0(i);
     delay_ms(5);
    }
    
for (j=0xFE; j>0x01; j--)
    {
     setcounter0(j);
     delay_ms(5);
    }
goto done;
	
}