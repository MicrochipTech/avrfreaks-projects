/*

Software para seguidor solar.
Autor: J.D. Hdez Hdez.
E-mail: cuninganreset@gmail.com	
Web: www.simplificate.es

Fecha 09-09-2011

 */ 
#include <avr/io.h>
#include "fportb.h"
#include "adc.h"

int main(void)
{
	setddrb(0,OUTPUT);
	setddrb(1,OUTPUT);
	setddrb(4,INPUT);
	int valor = 0;
	int varianza = 0;
    while(1)
    {
		adc_config(1,3);
		varianza = analogread();
		varianza = varianza/2;
		adc_config(1,1);
		valor = analogread();
		if(valor>512)
		{
			if(valor>(512+varianza))
			{
				setportb(1,LOW);
				setportb(0,HIGH);										
			}
			else
			{
				setportb(0,LOW);
				setportb(1,LOW);
			}											
		}	
		else if(valor<512)
		{
			if(valor<(512-varianza))
			{
				setportb(0,LOW);
				setportb(1,HIGH);
			}
			else
			{
				setportb(1,LOW);
				setportb(0,LOW);
			}			
		}															
    }
}