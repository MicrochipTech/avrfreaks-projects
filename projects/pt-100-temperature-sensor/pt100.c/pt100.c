#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <math.h>
#define USART_BAUDRATE 4800
#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)
#define F_CPU 1000000UL

//prototypes
char Init_Coms(void);
int Recive_String(FILE *stream);
int Transmit_String(char data, FILE *stream);
char Init_ADC(void);
unsigned long Read_ADC(char channel, char out);
unsigned long Pt100_Temp(void);
static FILE uartstr = FDEV_SETUP_STREAM(Transmit_String, Recive_String, _FDEV_SETUP_RW); 


int main(void){
unsigned int a=0;
Init_Coms();
stdout=stdin=&uartstr;
Init_ADC();
while(1){
	_delay_ms(1000);	
	a = Pt100_Temp();
	_delay_ms(10);
	printf("Temp=%u degrees C \r\n",a);
	_delay_ms(1000);	
	}
return (0);
}

char Init_Coms(void){ // 8 Data bits, 1 stop bit, no pairity
UCSRB |= (1 << RXEN) | (1 << TXEN);   // Turn on the transmission and reception circuitry
UCSRC |= (1 << URSEL) | (1 << UCSZ0) | (1 << UCSZ1); // Use 8-bit character sizes
UBRRL = BAUD_PRESCALE; // Load lower 8-bits of the baud rate value into the low byte of the UBRR register
UBRRH = (BAUD_PRESCALE >> 8); // Load upper 8-bits of the baud rate value into the high byte of the UBRR register 
return (0);
}

int Recive_String(FILE *stream){//Recive data from USART
char data;
while ((UCSRA & (1 << RXC)) == 0) {}; // Do nothing until data have been recieved and is ready to be read from UDR
data = UDR; // Fetch the recieved byte value into the variable "data" 
return (data);
}

int Transmit_String(char data, FILE *stream){//Send data to USART
while ((UCSRA & (1 << UDRE)) == 0){}; // Do nothing until UDR is ready for more data to be written to it
UDR = data; // send data to the computer 
return (0);
}

char Init_ADC(void){//Vcc AReff, 125KHz sample rate
ADCSRA |= (0 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Set ADC prescaler to 8;125KHz sample rate @ 1MHz
ADMUX  |= (0 << REFS1) | (1 << REFS0); // Set ADC reference to Vcc.
ADCSRA |= (1 << ADEN);  // Enable ADC	
return (0);
}

unsigned long Read_ADC(char channel, char out){//Channel 1-8, 'a' returns voltage & 'd' returns ADC value
unsigned int  adc_value=0;
unsigned long voltage=0;
switch (channel){
	case 1:
	ADMUX = 0b01000000;
		break;
	case 2:
	ADMUX = 0b01000001;
		break;
	case 3:
	ADMUX = 0b01000010;
		break;
	case 4:
	ADMUX = 0b01000011;
		break;
	case 5:
	ADMUX = 0b01000100;
		break;
	case 6:
	ADMUX = 0b01000101;
		break;
	case 7:
	ADMUX = 0b01000110;
		break;
	case 8:
	ADMUX = 0b01000111;
		break;
	default:
	ADMUX = 0b01000000;
		break;
}
ADCSRA |= (1 << ADSC);  // Start A2D Conversions
while (ADCSRA & (1 << ADSC)){;}
	adc_value = ADC;
	voltage = ((unsigned long) adc_value * 5000ul)/ 1024ul;
if (out == 'd')
	return (adc_value);
if (out == 'a')
	return(voltage);
return(0);
}

unsigned long Pt100_Temp(void){//Returns Tempreture from PT100 sensor
float N1,N2,N3, Ro,Temp,Ri = 100;

N1 = Read_ADC(1,'d');
N2 = Read_ADC(2,'d');
N3 = Read_ADC(3,'d');

Ro=2*N2;
Ro=Ro - N1;
Ro=Ro-N3;
Ro=fabs(Ro);
Ro=Ri*Ro;
Ro=Ro/N3;
Temp = Ro - 102.34;// Nominal value should be 100, calabrated useing Maxium App note 3450
Temp = Temp / 0.387;
return (Temp);
}














