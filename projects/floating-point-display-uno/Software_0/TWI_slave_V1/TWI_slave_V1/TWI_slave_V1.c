


/*
Floating point display for UNO:  
TWI slave device, 
	illuminates 4 digits of the display
	under the control of the master device
	
Compiled using Atmel studio 7 and optimised for size (level -Os)
Device ATtiny 861/A or 461/A loaded on the top side of the pcb. Pins 1/20 face the vertical switch.
*/



#include "Project.h"

void Display_driver(void);
void Char_definition(void);
void USI_TWI_Slave_Initialise(unsigned char);

volatile int buf_ptr;
volatile int int_counter;

int main (void){

	setup_ATtiny_HW;

	set_digit_drivers;
	clear_digits;
	clear_display;
	TCCR0A |= 1 << TCW0;											//16 bit mode
	OCR0B =	0xE4;													//Used to control intensity
	OCR0A = 0x0;
	TCNT0H = 0xE0;													//Generates 4mS interrupt stream
	TCNT0L = 0x0;
	TIMSK |= (1 << TOIE0) | (1 << OCIE0A);							//Initialise Timer interrupts
	TCCR0B = 1;
	sei();

	USI_TWI_Slave_Initialise(4);									//Address of this slave is 4

	while(!(USI_busy));												//wait for master request
	while((USI_busy));												//Pause here while data is received
	for(int m = 0; m <= 3; m++)
	{display_intensity = Rx_data[m]; Rx_data[m] = 0;}

	while(1){														//Receive floating point number strings to drive display
		for(int m = 0; m <= 2; m++){
			while(!(USI_busy));										//wait for master request
			while((USI_busy));										//Pause here while data is received
			for(int m = 0; m <= 3; m++)
			{display_buf[3-m] = Rx_data[m]; Rx_data[m] = 0;}}
		}


		while(1);



	wdt_enable(WDTO_60MS); while(1);}



	/******************************************************************************************************/


	ISR (TIMER0_OVF_vect){TCNT0H = 0xE0;							//Generates interrupt every 4.096mS.
		TCNT0L = 0x0;
		
		int_counter ++;
		if (int_counter == 20){	int_counter = 0;}
		
		clear_digits;
		clear_display;
	}



	/******************************************************************************************************/
	void Display_driver()
	{buf_ptr++; buf_ptr = buf_ptr%4;
		
		switch(buf_ptr){
			case 0: {digit_0;} break;
			case 1: {digit_1;} break;
			case 2: {digit_2;} break;
			case 3: {digit_3;} break;}
		Char_definition();}




		/******************************************************************************************************/
		/******************************************************************************************************/
		ISR (TIMER0_COMPA_vect){									//Controls display intensity
			if(!(int_counter%display_intensity))					//1 or 4
			{Display_driver();}}



			/******************************************************************************************************/
			void Char_definition()
			{switch (display_buf[buf_ptr]){
				case '0': zero; break;
				case '1': one; break;
				case '2': two; break;
				case '3': three; break;
				case '4': four; break;
				case '5': five; break;
				case '6': six; break;
				case '7': seven; break;
				case '8': eight; break;
				case '9': nine; break;
				case '-': minus; break;
				case 'E': case 'e': exponent; break;
				
				
				case ('0' | 0x80): zero_point; break;
				case ('1' | 0x80): one_point; break;
				case ('2' | 0x80): two_point; break;
				case ('3' | 0x80): three_point; break;
				case ('4' | 0x80): four_point; break;
				case ('5' | 0x80): five_point; break;
				case ('6' | 0x80): six_point; break;
				case ('7' | 0x80): seven_point; break;
				case ('8' | 0x80): eight_point; break;
				case ('9' | 0x80): nine_point; break;
				case ('-' | 0x80): minus_point; break;
			}}
			
			
			

