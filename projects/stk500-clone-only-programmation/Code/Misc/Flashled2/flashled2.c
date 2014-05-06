/******************************************************************************
Flash led
*
* Jidan Al-Eryani
*******************************************************************************/
#include <avr/io.h>

#define LED_PIN PA0


//function prototypes
void delay_ms(unsigned int ms);

int main(void)
{    

	DDRA |= (1<<LED_PIN);
	PORTA = 0x00;
	
    while (1){    // loop forever
		delay_ms(1000);
		PORTA ^= (1<<LED_PIN);
	}
		
}

// Delay loop
void delay_ms(unsigned int ms)
{
    unsigned int counter;
   
    while (ms) {
        counter = F_CPU / 5000;
       
        while (counter) {
            asm volatile("nop");
            counter--;
        }
        ms--;
    }
}
