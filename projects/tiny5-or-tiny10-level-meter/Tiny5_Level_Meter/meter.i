
#pragma used+
sfrb PINB=0x00;
sfrb DDRB=0x01;
sfrb PORTB=0x02;
sfrb PUEB=0x03;
sfrb PORTCR=0x0c;
sfrb PCMSK=0x10;
sfrb PCIFR=0x11;
sfrb PCICR=0x12;
sfrb EIMSK=0x13;
sfrb EIFR=0x14;
sfrb EICRA=0x15;
sfrb DIDR0=0x17;
sfrb ADCL=0x19;
sfrb ADMUX=0x1b;
sfrb ADCSRB=0x1c;
sfrb ADCSRA=0x1d;
sfrb ACSR=0x1f;
sfrb ICR0L=0x22;
sfrb ICR0H=0x23;
sfrw ICR0=0x22; 
sfrb OCR0BL=0x24;
sfrb OCR0BH=0x25;
sfrw OCR0B=0x24; 
sfrb OCR0AL=0x26;
sfrb OCR0AH=0x27;
sfrw OCR0A=0x26; 
sfrb TCNT0L=0x28;
sfrb TCNT0H=0x29;
sfrw TCNT0=0x28; 
sfrb TIFR0=0x2a;
sfrb TIMSK0=0x2b;
sfrb TCCR0C=0x2c;
sfrb TCCR0B=0x2d;
sfrb TCCR0A=0x2e;
sfrb GTCCR=0x2f;
sfrb WDTCSR=0x31;
sfrb NVMCSR=0x32;
sfrb NVMCMD=0x33;
sfrb VLMCSR=0x34;
sfrb PRR=0x35;
sfrb CLKPSR=0x36;
sfrb CLKMSR=0x37;
sfrb OSCCAL=0x39;
sfrb SMCR=0x3a;
sfrb RSTFLR=0x3b;
sfrb CCP=0x3c;
sfrb SPL=0x3d;
sfrb SREG=0x3f;
#pragma used-

#asm
	#ifndef __SLEEP_DEFINED__
	#define __SLEEP_DEFINED__
	.EQU __se_bit=0x01
	.EQU __sm_mask=0xe0
	.EQU __sm_adc_noise_red=0x02
	.EQU __sm_powerdown=0x04
    .EQU __sm_standby=0x08
	.SET power_ctrl_reg=smcr
	#endif
#endasm

#pragma used+

void delay_us(unsigned int n);
void delay_ms(unsigned int n);

#pragma used-

void update_leds(void);     

unsigned char new_data;     

interrupt [6] void timer0_compa_isr(void)
{

update_leds();
new_data = 0;

}

unsigned char adc_data;

interrupt [11] void adc_isr(void)
{

adc_data=ADCL;
new_data = 1;    

delay_us(10);

ADCSRA|=0x40;
}

void update_leds()
{

if (adc_data <= 102      )
{   
PORTB = ~(1<<1       );
}

else if ( adc_data >= 153      )
{

PORTB = ~(1<<3       );
}

else 
{

PORTB = ~(1<<2       );
}

}

void main(void)
{

#pragma optsize-
CCP=0xD8;
CLKPSR=0x00;

VLMCSR=0x02;

PUEB=0x00;

PORTCR=0x00;

PORTB=0x0E;
DDRB=0x0E;

TCCR0A=0x00;
TCCR0B=0x02;
TCNT0=0x0000;
ICR0=0x0000;
OCR0A=0x0400;   
OCR0B=0x0000;

EICRA=0x00;
EIMSK=0x00;
PCICR=0x00;

TIMSK0=0x02;

ACSR=0x80;

DIDR0=0x01;
ADMUX=0;
ADCSRA=0xEF;
ADCSRB=0x00;

#asm("sei")

while (1)
{

};
}
