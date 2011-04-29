/*
 * LIPO_Final.c
 *
 * Created: 2011-04-18 14:16:25
 *  Author: jsvens64
 */ 

#define F_CPU 8000000UL     /* Clock Frequency = 8Mhz */ 
#include <avr/io.h> 
#include <util/delay.h> 



int main(void)
{
uint8_t channel; 
uint8_t voltageA = 0;
uint8_t voltageB = 0;
uint8_t voltageC = 0;
uint8_t cell1 = 0;
uint8_t cell2 = 0;
uint8_t cell3 = 0;
uint8_t cell = 0;
uint8_t status = 3;
uint8_t status1 = 3;
uint8_t status2 = 3;
uint8_t status3 = 3;

#define R_LED PB0 
#define G_LED PB1
#define RELAY PA3
#define OUTPORTA PORTA 
#define RELAY_ON OUTPORTA &=~(1<<RELAY)  
#define RELAY_OFF OUTPORTA |=(1<<RELAY)  
#define OUTPORTB PORTB 
#define R_LED_ON OUTPORTB |=(1<<R_LED) 
#define R_LED_OFF OUTPORTB &=~(1<<R_LED) 
#define G_LED_ON OUTPORTB |=(1<<G_LED) 
#define G_LED_OFF OUTPORTB &=~(1<<G_LED)

DDRA = 0x08;
DIDR0 |= (1<<ADC2D)|(1<<ADC1D)|(1<<ADC0D); 
PORTA = 0x0F; 
DDRB = 0x03; 
PORTB = 0x00; 
    

//************************************ 
//  ADC module initialization  
//************************************ 
void adc_init(void){ 

	//select reference voltage 
	ADMUX = 0x00; //Ref voltage set to VCC

	//set prescaler and enable ADC 
	ADCSRA|=(1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0); //Set prescaler to 128 -> 62.5 kHz 
	return;
	
}

//************************************** 
//  ADC single conversion routine  
//************************************** 
uint16_t readADC8(uint8_t channel){
	ADMUX = channel; // select current channel 
	_delay_ms(10);
	ADCSRA |= (1<<ADSC); // SETBIT(ADCSRA, ADSC)
	while (ADCSRA & (1<<ADSC));
	 
	ADCSRB |= (1<<ADLAR); // upper 8 bits in ADCH
    return ADCH; 
}
//************************************** 
//  Calculate cell voltages  
//**************************************  
void calc(void){ // Calculates individual cell voltages
	voltageA = readADC8(2); //Read voltageA 
	_delay_ms(10);
	voltageB = readADC8(1); //Read voltageB
	_delay_ms(10);
	voltageC = readADC8(0); //Read voltageC
	_delay_ms(10);
	cell1 = voltageA;
	cell2 = voltageB; //(voltageB - voltageA);
	cell3 = voltageC; //(voltageC - voltageB);
	return 0;
}
//************************************** 
//  Lets indicate status  
//**************************************
void indication(status){
	switch(status){
		case 0:{
			R_LED_ON;
			G_LED_OFF;
			RELAY_OFF;
			break;			
		}
		case 1:{
			R_LED_ON;
			G_LED_OFF;
			RELAY_ON;
			break;			
		}
		case 2:{
			R_LED_ON;
			G_LED_ON;
			RELAY_ON;
			break;			
		}
		case 3:{
			R_LED_OFF;
			G_LED_ON;
			RELAY_ON;
			break;			
		}
		default:{
			RELAY_OFF;
			break;
		}
	}
	return 0;	
}
//************************************** 
//  Check cell status  
//**************************************
void cell1_status(cell1){
	if (cell1 >= 63){			
		status1 = 3;
	}		
	else{
		if (cell1 < 63 && cell1 >= 61){
				status1 = 2;
			}		
			else{
				if (cell1 < 61 && cell1 >= 60){
					status1 = 1;
				}		
				else{
					status1 = 0;
				};
			};
		};
	return 0;
}	
void cell2_status(cell2){
	if (cell2 >= 63){			
		status2 = 3;
	}		
	else{
		if (cell2 < 63 && cell2 >= 61){
				status2 = 2;
			}		
			else{
				if (cell2 < 61 && cell2 >= 60){
					status2 = 1;
				}		
				else{
					status2 = 0;
				};
			};
		};
	return 0;
}	
void cell3_status(cell3){
	if (cell3 >= 63){			
		status3 = 3;
	}		
	else{
		if (cell3 < 63 && cell3 >= 61){
				status3 = 2;
			}		
			else{
				if (cell3 < 61 && cell3 >= 60){
					status3 = 1;
				}		
				else{
					status3 = 0;
				};
			};
		};
	return 0;
}
//************************************** 
//  Check battery status  
//**************************************
	
uint8_t combine_stat(status1, status2, status3){
	if ((status1==0) || (status2==0) || (status3==0))
	{
	status=0;
	}
	else if ((status1==1) || (status2==1) || (status3==1))
	{
	status=1;
	}
	else if ((status1==2) || (status2==2) || (status3==2))
	{
	status=2;
	}
	else
	{
	status=3;
	}
	return status;
	
}

	
	adc_init();
	
    while(1)
    {
       
	   calc();
	   cell1_status(cell1);	//Check status cell1
	   cell2_status(cell2);	//Check status cell2
	   cell3_status(cell3);	//Check status cell3
	   combine_stat(status1, status2, status3); //Combine status to battery status
	   indication(status);	//Indicate status again
	  
    }
}