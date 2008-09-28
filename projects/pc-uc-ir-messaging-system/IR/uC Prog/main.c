/* this is a IR detector */

#include <stdio.h>
#include <avr/io.h>
#include "lcd.h"

#include <avr\interrupt.h>
#include <stdlib.h>

#include <util/delay.h>
#define ICP PINB0

#define Initial_High_Delay    4000
#define Pulse_Delay_1         2000
#define Pulse_Delay_0         1000

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
volatile uint8_t calcparity ;

volatile uint8_t deviceID;
volatile uint8_t receivedID;

//uint8_t jjj=1;

//volatile unsigned char DATAStream [16] = "IR Comm Example "; //The array to store the packets and to display in the LCD
volatile unsigned char DATAStream1 [17] = "  IR RX Module  "; //The array to store the packets and to display in the LCD
volatile unsigned char DATAStream2 [17] = "Sajid Muhaimin C"; //The array to store the packets and to display in the LCD
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
				if (Transmission_Started== true)
				{
				// 8 data + 4 location + 1 parity 
					if (pulsewidth > 3* (Pulse_Delay_1>>2) && pulsewidth < 5* (Pulse_Delay_1>>2))
						// a 1 occured
					{
						if (Bit_Count <= 2) //device ID
							receivedID |= 1<<Bit_Count;
						if (Bit_Count <= 10) //device ID
							datatemp |= 1<<(Bit_Count-3);
						else if (Bit_Count <= 15)
							locationtemp |= 1<<(Bit_Count-11);
						else if (Bit_Count == 16)//datasteam is over, get parity bit
							tempparitybit = 1;
						Bit_Count++;
					}
					else if (pulsewidth > 3* (Pulse_Delay_0>>2) && pulsewidth < 5* (Pulse_Delay_0>>2))
						// a 0 occured
					{
						if (Bit_Count <= 2)
							receivedID &= ~(1<<Bit_Count);
						if (Bit_Count <= 10)
							datatemp &= ~(1<<(Bit_Count-3));
						else if (Bit_Count <= 15)
							locationtemp &= ~(1<<(Bit_Count-11));
						else if (Bit_Count == 16) //datasteam is over, get parity bit
							tempparitybit = 0;	
						Bit_Count++;
					}	
						
					else  // this is an unknown pulse or error pulse. 
						//	This indicates the data sequence is corrupted and we will discard this packet.
					{
						Transmission_Started = false;
					}

					if (Bit_Count == 3 && receivedID != deviceID) //device ID mismatch, error or no error
					{
					//	Transmission_Started = false;
					}
					
						
					// now check if bit count is 13, then transmission is over and we need to check parity	

					if (Bit_Count == 17) //bit count was erraneously increased one while exiting the loop
					{
						Transmission_Started = false;
						//go for paritycheck of the data
						calcparity = (receivedID) ^ (receivedID >>1) ^ (receivedID >>2);
						calcparity ^= (datatemp>>7)^(datatemp>>6)^(datatemp>>5)^(datatemp>>4);
						calcparity ^= (datatemp>>3)^(datatemp>>2)^(datatemp>>1)^(datatemp);
						calcparity ^= (locationtemp>>3)^(locationtemp>>2)^(locationtemp>>1)^(locationtemp);
						calcparity ^= (locationtemp>>4);
						calcparity = ~(calcparity); // if there is even number of 1's, paritybit is 1, else paritybit is zero
						calcparity &= 0x01;
						//if calculated parity ==  tempparitybit then packet is ok and write data
						
						if (calcparity == tempparitybit) //calcparity == tempparitybit
						{
							if (locationtemp <=15)
								DATAStream1[locationtemp] = datatemp;
							else if (locationtemp <=31)
								DATAStream2[locationtemp-16] = datatemp;	
								
						}
					}
				}
				else // transmission was not going on, if the pulse length comparable to start pulse within 25% tolerance we need to start a new data stream
				{
					if ((pulsewidth > 3* (Initial_High_Delay>>2)) && (pulsewidth < 5* (Initial_High_Delay>>2)))
					{
						Transmission_Started = true;
						Bit_Count = 0;
						locationtemp = 0;
						datatemp=0;
						
					}
				}

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
	DDRD &= ~0b01111000; //set ICP as input, 
						//	and get the device id through PD5, PD4, DPD3
	deviceID = (PIND & 0b00111000)>>3;	

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
		lcd_puts(DATAStream1);
		lcd_gotoxy(0, 1);
		lcd_puts(DATAStream2);
		_delay_ms(3000); //refresh every 3 secs
	}


	return 0;
}
