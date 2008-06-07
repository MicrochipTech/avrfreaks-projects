//////////////////////////PROJECT on atmega8/////////////////////////
//////////////////////////Displaying ADC input on PINC0 in 7seg connected on PORTD//////
//////////////////////////7seg is controled by PINB1-4///////////


#include<avr/io.h>
#include<util/delay.h>
#include<stdio.h>
#include<avr/interrupt.h>

void adc_init();
void start_convar();
int func_disp7g();
int value,temp;
int function_disp(); 
unsigned int value1,valuer,ab,ac,ad,ae;
uint16_t adc_value;	
float temp2,temp3;

int main()
{
DDRC=0x00;
DDRD=0xff;
DDRB=0xff;
adc_init();
temp=PINC;
start_convar();





return 1;
}


void adc_init()
{

ADMUX|=(0<<REFS1)|(1<<REFS0);
ADCSRA|=((1<<ADEN)|(1<<ADFR));
ADCSRA |=(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
ADMUX|=(0<<ADLAR);





}

void start_convar()
{


ADMUX=ADMUX&0xE0;
sei();
ADCSRA |= (1<<ADSC)|(1<<ADIE);





}

ISR(ADC_vect){	
for(int g=0;g>=g;g++){
temp2=ADCW;




		func_disp7g();	
	  				 }	

			 }




//////////////////////////DISPLAY////////////////////////////
int func_disp7g()
{
temp2=((temp2*5.0)*10.323/1.0344);

	float ZZ,Zx,Zy;
	ab=(temp2/10000);
	ZZ=(temp2/10000)-ab;
	ZZ*=10;
	ac=ZZ;
	Zx=ZZ-ac;
	Zx*=10;
	ad=Zx;
	Zy=Zx-ad;
	Zy*=10;
	ae=Zy;
	function_disp();

return 1;
}

//////////////////////////////////////////////////////////
int function_disp()
{

		for(int q=0;q<50;q++)
			{
			PORTD=func_check(ab);
			PORTB=0x02;
		
			wait_one_second(1);
		
			PORTB=0;
			PORTD=func_check(ac);
			PORTB=0x04;
		
			wait_one_second(1);
		
			PORTB=0;
			PORTD=func_check(ad);
			PORTB=0x08;
		
			wait_one_second(1);
		
			PORTB=0;
			
			PORTD=func_check(ae);
			PORTB=0x010;
		
			wait_one_second(1);
		
			PORTB=0;
			
		
			
			}
return 1;


		}


int func_check(int a)
{

switch(a)
{
case 0:
valuer=0xc0;
break;
case 1:
valuer=0xf9;
break;
case 2:
valuer=0xa4;
break;
case 3:
valuer=0xb0;
break;
case 4:
valuer=0x99;
break;
case 5:
valuer=0x92;
break;
case 6:
valuer=0x82;
break;
case 7:
valuer=0xf8;
break;
case 8:
valuer=0x80;
break;
case 9:
valuer=0x90;
break;

}
return valuer;
}



int wait_one_second(int value)

{
	if (value==1)
		
		{	_delay_ms(10) ; 	}
	else
		{	_delay_ms(40);		}
	return 1 ;

}


