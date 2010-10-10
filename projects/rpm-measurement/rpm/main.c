// Target : M8535
// Crystal: 8.0000Mhz

#include <iom8535v.h>
#include <macros.h>

unsigned int count_1 = 0,RPM = 0;
unsigned char j = 0, iter = 0;

void port_init(void)
{
 PORTA = 0x00;
 DDRA  = 0x00;
 DDRB  = 0xFF;
 PORTB = 0xFF;
 PORTC = 0x00;
 DDRC  = 0xFF;
 PORTD = 0x00;
 DDRD  = 0x00;
}

void timer2_init(void)
{
 TCCR2 = 0x00;
 OCR2  = 0x53;
 ASSR  = 0x00;
 TCNT2 = 0x00;
 //do not start d timer here..
}

#pragma interrupt_handler timer2_comp_isr:iv_TIMER2_COMPA
void timer2_comp_isr(void)
{
    count_1++;
}


#pragma interrupt_handler int0_isr:iv_INT0
void int0_isr(void)
{
    iter++;
    if (j == 0)
    {
    count_1 = 0;
    TCCR2 = 0x0A;//start timer 2
    j = 1;
    }
    else if (iter > 60)
    {
      TCCR2 = 0x00;    //stop timer 2
      RPM = 12287/count_1;
      j = 0;
      iter = 0;
    }

}

void init_devices(void)
{
 CLI();
 port_init();
 timer2_init();

MCUCR = 0x02;
 GIMSK = 0x40;
 TIMSK = 0x80;
 SEI();
}




void main(void)
{
 init_devices();
}

