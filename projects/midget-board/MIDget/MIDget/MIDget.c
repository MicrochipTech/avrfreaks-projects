//code for the MIDget MIDI interface
//simple ADC sensor-midi conversion
//using the USI on the ATTiny26 to do MIDI bytes
//by Andrew Benson

//uses code from USI_UART Atmel Application Note
//ported for use with GCC

#include "avr/io.h"
#include "avr/interrupt.h"
#include "compat/ina90.h" //might not need this
#include "USI_UART_config.h"
#include "global.h"

//send MIDI CC byte
void mSendIt(unsigned char cc, unsigned char val){
	USI_UART_Transmit_Byte(0xB1);
	USI_UART_Transmit_Byte(0x7F & cc);
	USI_UART_Transmit_Byte(0x7F & val);
	return 0;
}

#define P_SCALE 4 //1/16 clock prescaler
//#define P_SCALE 5 //1/32 clock prescaler
//#define P_SCALE 6 //1/64 clock prescaler

void adcinit(){
	ADMUX = (1<<REFS0);
//	ADMUX = 0;
	ADCSR = (1<<ADEN) | (1<<ADSC) |(1<<ADIF)|(P_SCALE<<ADPS0);
	do{
		}while (ADCSR & (1<<ADSC));
}

unsigned short adConv(unsigned char ch){
	ADMUX = (1<<REFS0) + ch;
//	ADMUX = ch;
	ADCSR |= (1<<ADSC);
	do{
		}while (ADCSR & (1<<ADSC));
	return ADC;
}

void main( void )
{
	DDRA = 0x00;
	PORTA = 0xFF;

	adcinit();
	unsigned char blink = 0;
	
  	unsigned char midi_hi[8];
  	unsigned char midi_lo[8];
    
    USI_UART_Flush_Buffers();
	USI_UART_Initialise_Transmitter();                       // Initialisation for USI_UART receiver
    
	DDRB |= (1<<6);                                            
      
    for( ; ; )                                                              // Run forever
  	{
			unsigned char i = 0;
			while(i<8){
				unsigned short value=0;
				value=adConv(i);
				midi_hi[i] = value>>7; //put 3 MSB in separate message
				midi_lo[i] = value & 127;
				i++;
				}
			i = 0;
		
			mSendIt(1,midi_lo[0]);
			mSendIt(2,midi_hi[0]);
			mSendIt(3,midi_lo[1]);
			mSendIt(4,midi_hi[1]);
			mSendIt(5,midi_lo[2]);
			mSendIt(6,midi_hi[2]);
			mSendIt(7,midi_lo[3]); 
			mSendIt(8,midi_hi[3]); 
			mSendIt(9,midi_lo[4]);
			mSendIt(10,midi_hi[4]);
			mSendIt(11,midi_lo[5]);
			mSendIt(12,midi_hi[5]);
			mSendIt(13,midi_lo[6]);
			mSendIt(14,midi_hi[6]);
			mSendIt(15,midi_lo[7]); 
			mSendIt(16,midi_hi[7]);

			//blink led
			if (blink>4) PORTB |= (1<<6);
			else PORTB &= ~(1<<6);
			if (blink > 5) blink = 0;
			blink++;
			}
  
}
