/* this is a IR detector */

#include <stdio.h>
#include <avr/io.h>
#include "lcd.h"

#include <avr\interrupt.h>
#include <stdlib.h>

#include <util/delay.h>
#define ICP PINB0

#define Initial_High_Delay    4000
#define Pulse_Delay           1110
#define Pulse_Delay_1         1000
#define Pulse_Delay_0         2000

#define true 1
#define false 0


volatile uint16_t ov_counter; //define ovrflow counter
volatile uint16_t rising, falling;//define times for start and end of signal
volatile uint32_t pulsewidth=3123;//define overall counts

volatile uint8_t Transmission_Started=false; //Condidional check if a packet is being received
volatile uint8_t Bit_Count=0;   //Number of bits received in current packet
volatile uint8_t datatemp; //temporary  data
volatile uint8_t locationtemp; //temporary location 
volatile uint8_t tempparitybit; //parity bit

//uint8_t jjj=1;

volatile unsigned char DATAStream [16] = "IR Comm Example "; //The array to store the packets and to display in the LCD
volatile unsigned char AA[] = "A                    ..\0";


//overflow counter interrupts service routine
ISR(TIMER1_OVF_vect){

  ov_counter++;

}

//Timer1 capture interrupt service subroutine
ISR(TIMER1_CAPT_vect){
	
	/*This subroutine checks was it start of pulse (rising edge)
	or was it end (fallingedge)and performs required operations*/
	
	if (PIND & 0b01000000) //if high level (I.E Rising Edge)
	
			{
				/* my code
				TIMSK =0x20;  // overflow interrupt disable
				TIFR &= 0x04; // overflow interrupt flag clear
				rising=ICR1;//save rising time
				ICR1 = 0;
				TCCR1B=TCCR1B&0xBF;//set to trigger on falling edge
				pulsewidth=(uint32_t)rising+(((uint32_t)ov_counter)<< 16);//the low pulse width
				*/

				
/*   				TIMSK &= ~(1<<TOIE1);   // overflow interrupt disable
   				if (TIFR & (1<<TOV1))      // check for any unserviced overflow interrupts
       				if (ICR1 < 0x8000)       // if the unserviced overflow happened before the capture
           				ov_counter++;      // then correct the overflow counter value
   				TIFR &= (1<<TOV1);      // overflow interrupt flag clear
				*/
   				rising=ICR1;         //save rising time
   				ICR1 = 0;
   				TCCR1B=TCCR1B&0xBF;   //set to trigger on falling edge
   				//pulsewidth=(uint32_t)rising+(((uint32_t)ov_counter)<< 16) ;//the low pulse width
				pulsewidth=(uint32_t)rising;//the low pulse width

      			// rising functions
      		}
				
						
		else //Falling Edge
				{
				TCNT1 = 0; //Reset Count
				ICR1 = 0;
				//ov_counter=0; //reset overflow counter
				TCCR1B=TCCR1B|0x40;//rising edge triggers next
				//TIMSK =0x24; // overflow interrupt enable
				//falling functions
	   }
	
	

}



int main(void){
	DDRD = 0x00;
	DDRD &= ~0b01000000; //set ICP as input
	DDRB = 0xFF;
	lcd_init(LCD_DISP_ON);
	lcd_clrscr();
	Transmission_Started=false;
	Bit_Count=0;
	TCCR1A = 0x00; 
    TCCR1B= (1<<ICNC1); //Noise canceller
	
	TCCR1B|= (1<<CS11); //clock select clk /8
	TCCR1B&= ~((1<<CS12)|(1<<CS10));
	
	
	TIMSK = (1<<TICIE1);   //0x20;  Input Capture IE
	TCCR1B &= 0xBF; //Set trigger to falling edge, just making sure I didn't screw it up previously.
	sei();
	

	
	

	while(1){
		lcd_clrscr();	
		ultoa((unsigned long)pulsewidth, AA, 10);
		lcd_puts(AA);
		_delay_ms(3000); //refresh every 3 secs
	}


	return 0;
}
