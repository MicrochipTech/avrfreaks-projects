#include <avr\io.h>
#include <avr\interrupt.h>
#include <avr\iom128.h>

unsigned char temp;

void port_init(void)
{
	PORTB = 0x00;
	DDRB  = 0xFF;
}

void timer1_init(void)
{
	TCCR1B = 0x00; //stop
	TCNT1H = 0x0B; //load counter HIGH value
	TCNT1L = 0xDC; //load counter LOW value
	
	TCCR1A = 0x00;
	TCCR1B = 0x03; //start Timer
}

void init_devices(void)
{
	//stop errant interrupts until set up
	cli(); //disable all interrupts
	
	port_init();
	timer1_init();
	
	TIMSK = 0x04; //timer interrupt sources
	ETIMSK = 0x00; //extended timer interrupt sources
	sei(); //re-enable interrupts
	//all peripherals are now initialized
}

ISR(TIMER1_OVF_vect){

 	//TIMER1 has overflowed
	temp = ~temp;
	PORTB = temp;
	TCNT1H = 0x0B; //reload counter high value
	TCNT1L = 0xDC; //reload counter low value

}

int main(void)
{
	init_devices();
	while(1); 
	//insert your functional code here...
}
