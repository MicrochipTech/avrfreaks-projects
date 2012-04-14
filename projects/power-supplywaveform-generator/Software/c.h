/*
	Libreria para el uso de los pines con ADC
	
	Autor: J.D. Hdez Hdez
	E-mail: cuninganreset@gmail.com
	Web:	www.simplificate.es
*/

#ifndef adc_h
#define adc_h
	
//	ADC en modo continuo
void adc_free_running(uint8_t adc_pin)
{
	ADCSRA = 0x00;
	ADCSRB = 0x00;
	ADMUX = 0x00;
	switch (adc_pin)
	{
	case 0:
		ADMUX = 0b00;
		break;
	case 1:
		ADMUX = 0b01;
		break;
	case 2:
		ADMUX = 0b10;
		break;
	case 3:
		ADMUX = 0b11;
		break;
	}
	ADCSRB = 0x00;
	ADCSRA |= 1<<ADATE;
	ADCSRA |= 1<<ADEN;
	ADCSRA |= 1<<ADSC;
}

//	Activar ADC en modo normal.
void adc_on(uint8_t adc_pin)
{
	ADMUX = 0x00;
	switch (adc_pin)
	{
		case 0:
			ADMUX = 0b00;
		break;
		
		case 1:
			ADMUX = 0b01;
		break;
		
		case 2:
			ADMUX = 0b10;
		break;
		
		case 3:
			ADMUX = 0b11;
		break;
	}
	ADCSRA =0x00;
	ADCSRA = 1<<ADEN;
}	

/*
Lee el valor analogico,
Devuelve un int.
*/
int analogread()
{
	if((ADCSRA&1<<ADEN)>1)
	{
		ADCSRA |= 1<<ADSC;
		volatile uint8_t i=0;
		while(1)
		{
			if((ADCSRA & 1<<ADSC))
			{
				i=i+1;
			}
			else
			{				
				break;
			}			
		}			
	}		
	int resultado=0;
	resultado = ADCL;
	resultado |= ADCH<<8;
	return(resultado);
}

/*
Configuración del ADC:
Modos:
1- Modo normal.
2- Modo continuo.
Pin:
Pin adc segun Datasheet.
*/
void adc_config(uint8_t modo,uint8_t adc_pin)
{
	switch (modo)
	{
	case 1:
		adc_on(adc_pin);
		break;
	case 2:
		adc_free_running(adc_pin);
		break;
	default:
		adc_on(adc_pin);
		break;
	}
}
#endif