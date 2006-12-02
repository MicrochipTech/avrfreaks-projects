

#include <avr\io.h>
#include <avr\interrupt.h>
#include <avr\iom128.h>

void port_init(void)
{
	PORTA = 0x00;
	DDRA  = 0x00;
	PORTB = 0x00;
	DDRB  = 0x80;
	PORTC = 0x00; //m103 output only
	DDRC  = 0x00;
	PORTD = 0x00;
	DDRD  = 0x00;
	PORTE = 0x00;
	DDRE  = 0x00;
	PORTF = 0x00;
	DDRF  = 0x00;
	PORTG = 0x00;
	DDRG  = 0x00;
}

//ADC initialize
void adc_init(void)
{
	ADCSRA = 0x00; //disable adc
	ADMUX = 0x22; //Set Left Adjust & Select ADC input 2
	ACSR  = 0x80; // ACSR -----> ACD ACBG ACO ACI ACIE ACIC ACIS1 ACIS0
	ADCSRA = 0xCF;// ADCSRA ---> ADEN ADSC ADFR ADIF ADIE ADPS2 ADPS1 ADPS0  
}

//#pragma interrupt_handler adc_isr:22
//void adc_isr(void)
//{
 //conversion complete, read value (int) using...
 // value=ADCL;            //Read 8 low bits first (important)
 // value|=(int)ADCH << 8; //read 2 high bits and shift into top byte
//}

ISR(ADC_vect){

unsigned char temp;
 
	temp = ADCH;
	if(ADCH>0x40) // Proximity sensor gives value between 0 to 255 Decimal
		PORTB = 0xFF; // if value is > 40 HEX then output BEEP
	else
		PORTB = 0x00; // else output NO BEEP

	ADMUX = 0x22; //Set Left Adjust & Select ADC input 2
	ADCSRA = ADCSRA | 0x40; // Star conversion

}


//call this routine to initialize all peripherals
void init_devices(void)
{
 //stop errant interrupts until set up
	cli(); //disable all interrupts
	XDIV  = 0x00; //xtal divider
	XMCRA = 0x00; //external memory
	port_init();
	adc_init();

	MCUCR = 0x00;
	EICRA = 0x00; //extended ext ints
	EICRB = 0x00; //extended ext ints
	EIMSK = 0x00;
	TIMSK = 0x00; //timer interrupt sources
	ETIMSK = 0x00; //extended timer interrupt sources
	sei(); //re-enable interrupts
	//all peripherals are now initialized
}

//
int main(void)
{
	init_devices();
	//insert your functional code here...
	while(1);
}
