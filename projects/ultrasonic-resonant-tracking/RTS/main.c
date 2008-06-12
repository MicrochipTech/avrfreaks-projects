#define F_CPU 16000000UL  
#define UART_BAUD_RATE 56000

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h> //for ltoa
#include <util/delay.h>
#include "uart.h"

unsigned short 	capture = 0x0000; 

// unsigned long i=0; // counter for for loop

unsigned long Current_freq0 = 289660;//289000;//33500;//24090;//28950; // startup freq0

char buffer[16]; 

unsigned short center_point;
unsigned short lambda;

signed int direction=0; 

unsigned short adc_value = 0;      	
unsigned short acc = 10; 	// fractional accuracy = 1/10 = 0.1 Hz


/*************************************************************************
Function: SPI_init()
Purpose:  initialize the SPI bus 
Input:    none
Returns:  none
**************************************************************************/
void SPI_init (void)
{
	DDRB 	= 0xFF;
	//PORTB 	= 0xFF;
	
	DDRB  |= _BV(PB0) | _BV(PB3) | _BV(PB5);	// 	set SCK,MOSI,PB0 as Fsync 
	PORTB |= _BV(PB5) | _BV(PB0);			// 	SCK and PB0 high 
	SPCR  |= _BV(SPE)| _BV(MSTR)| _BV(CPOL); // 	Enable SPI // Set Master mode //	Set clk to inv.
	// SPCR |= _BV(SPR0)					//	Clk speed = fck/4, SPR0,SPR1 = 0,0 // uncomment for fck/16
	
}


/*************************************************************************
Function: SPI_write16()
Purpose:  send a 16bit word to the AD9833 
Input:    unsigned short data = 16bits
Returns:  none
Comment:  uses 8bit filter and two consecutive writes while fsync stays low
**************************************************************************/
void SPI_write16 (unsigned short data)    	// 	send a 16bit word and use fsync
{

	unsigned char MSdata = ((data>>8) & 0x00FF);  	//filter out MS
	unsigned char LSdata = (data & 0x00FF);			//filter out LS

	PORTB &= ~_BV(PB0);						// 	Fsync Low --> begin frame
	
	SPDR = MSdata;							// 	send First 8 MS of data
	while (!(SPSR & (1<<SPIF)));			//	while busy

	SPDR = LSdata;							// 	send Last 8 LS of data
	while (!(SPSR & (1<<SPIF)));			//	while busy

	PORTB |= _BV(PB0);						// 	Fsync High --> End of frame
}


/*************************************************************************
Function: Freq_change()
Purpose:  change the frequency and select AD9833 onboard register
Input:    unsigned long freq_out = frequency, unsigned int select = register 0 or 1
Returns:  none
Comment:  uses 14 bit filter and adds control words, 
**************************************************************************/
void Freq_change ( unsigned long freq_out, uint8_t accuracy,  uint8_t select )  // take base10 frequency and do frequency hop
{

// make freq register from frequency // set for 20 MHz Mclk
unsigned long freq_reg = ((freq_out * 13.4217728)/accuracy);     //13.4217728 / 10; 	accuracy to 0.1 Hz,, with out using float
unsigned short MS_reg = ((freq_reg>>14) & 0x3FFF);  // filter out MS -- make 2 x 14 bit frequency words
unsigned short LS_reg = (freq_reg & 0x3FFF);		// filter out LS -- make 2 x 14 bit frequency words

MS_reg += 0x4000; 									// add control bits hex = 0x4000
LS_reg += 0x4000; 									// add control bits hex = 0x4000

if (select == 0 ) { SPI_write16(0x2000);}			// prep ad9833 to recieve full 28bit word for freq 0
if (select == 1 ) { SPI_write16(0x2800);}			// prep ad9833 to recieve full 28bit word for freq 1

SPI_write16(LS_reg);								// send the LS word first, to the ad9833
SPI_write16(MS_reg);								// send the MS word last,  to the ad9833
}


/*************************************************************************
Function: AD9833_init()
Purpose:  Init the AD9833
Input:    none
Returns:  none
Comment:  this function isn't nessecary, can be done manually
**************************************************************************/
void AD9833_init (void)
{

SPI_write16(0x2100);		// control word, set output to mid value voltage 

SPI_write16(0x7288);		// Freq0 registerdata MSB  = approx. xx khz
SPI_write16(0x4017);		// Freq0 registerdata LSB  = approx. xx khz

SPI_write16(0xACEA);		// Freq1 registerdata MSB  = approx. yy  khz
SPI_write16(0x8013); 		// Freq1 registerdata LSB  = approx. yy khz

SPI_write16(0xC000);		// Phase offset of Freq0 = 0
SPI_write16(0xE000);		// Phase offset of Freq1 = 0

SPI_write16(0x2000);		// control word, set output = sine

}




void adc_init(void) /* initializes the ADC */
{
        ADMUX = 0 | _BV(ADLAR); // channel 0, left-justified result
        ADCSRA = _BV(ADEN) | _BV(ADPS2);
}

/* starts an ADC conversion */
void adc_start_conversion(void)
{
    ADCSRA |= _BV(ADSC);
}

/* returns boolean value indicating if the ADC conversion is still in prgress */
int adc_conversion_in_progress(void)
{
    return !(ADCSRA & _BV(ADIF));
}

/* clears the ADC interrupt flag */
void adc_clear_interrupt_flag(void)
{
    ADCSRA |= _BV(ADIF);
}


int main(void)
		{		

		adc_init(); //initialize ADC
		SPI_init ();	//init the spi bus	
		AD9833_init (); //init the ad9833 device
		Freq_change (Current_freq0,acc,0); //initialize starting freq 0
		uart_init( UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU) );  // init uart
		

		PORTD |= (1<<PD2) | (1<<PD3);	//	ENABLE INPUTS		
		ACSR |=(1<<ACD);  				// 	analog comparotor on icp - turn off
		TCCR1B = (1<<CS10);				//  prescaler =8 // for timer 1 
		GICR |= ((1<<INT1)|(1<<INT0)); 	//	enable int0 and int1
		MCUCR |= (1<<ISC01) | (0<<ISC00); 	// int1 1,0 = falling; 1,1 = rising //10
		MCUCR |= (1<<ISC11) | (1<<ISC10);  	// int0 1,0 = falling; 1,1 = rising //11
		
		// MCUCR |= (1<<SE);	//se sleep enable 

		
		sei();							//ENaBLE INTERRUPTS
 				

/*
			
			// sweep ... used for measuring transducer 
			// will ouput thru rs232, alot of data to be captured...
			// formatted and imported to excel for charting
			
			for (i=288500; i<290500; i++)
				{
		
				_delay_ms(5);

				// cli();
				end=capture;
				
				//end_l = capture_l ;
				//end_h = capture_h ;
				// sei();			
				
			
				Freq_change (i,acc,0);
		

				// * get new ADC value * /
	            adc_start_conversion();
	            while (adc_conversion_in_progress());
	            adc_value = ADCH;
	           	adc_clear_interrupt_flag();
				
				// put the tick value to pc
				//uart_puts("(");
				
				ltoa (end, buffer, 10);				
				uart_puts(buffer);
				uart_puts(",");
								
				//ltoa (end_l, buffer, 10);				
				//uart_puts(buffer);
				//uart_puts(",");

				// Put the adc value to pc
				itoa (adc_value, buffer, 10);				
				uart_puts(buffer);
				uart_puts(",");
				
				// PUT THE FREQ TO PC
				ltoa (i, buffer, 10);				
				uart_puts(buffer);
				//uart_puts(")   ");
				uart_puts(";");
			//	uart_puts("\n\r");
		
				}// end of for

*/

		while (1) 
			{

			
			// phase = capture; // make phase from capture, cus capture can change... 
			
			//lambda is cpu ticks of wavelength...
			lambda = (F_CPU/(Current_freq0/acc));	
			
			// calculating the resonant center point value 
			center_point = (lambda/2) + (lambda/16);	
				
		
				// * get new ADC value * /
	            adc_start_conversion();
	            while (adc_conversion_in_progress());
	            adc_value = ADCH;
	            adc_clear_interrupt_flag();
				
				
				
				// start with bracket
				uart_puts("(");
			
				// PUT THE FREQ TO PC
				ltoa (Current_freq0, buffer, 10);				
				uart_puts(buffer);			
				uart_puts(",		");

				// Put the adc value to pc
				itoa (adc_value, buffer, 10);				
				uart_puts(buffer);
				uart_puts(",	");
			
				// put the phase value to pc
				ltoa (TCNT1, buffer, 10);				
				uart_puts(buffer);
				uart_puts(",	");
				
				// put the center_value to pc
				ltoa (center_point, buffer, 10);				
				uart_puts(buffer);
				//uart_puts(",	");
		
				//end with bracket
				uart_puts(")   ");
				//end with new line
				uart_puts("\n\r");

			
			
				// direction is proportional to difference from center
				// direction also contains magnitude
				direction = center_point - capture ;
				
				// change the current freq 
				Current_freq0 = (Current_freq0 + direction);
				
				// send changes thru spi
				Freq_change (Current_freq0,acc,0);
	
				
			}//end of while 
        
		return 0;
} //end of main function


ISR(INT1_vect)	//when pd3 (voltage zero cross) active on rise	//interrupt function for int0
	{
    TCNT1 = 0x0000; // clear timer &
	}

ISR(INT0_vect)	//when pd2 (current zero cross) active on rise	//interrupt function for int1
	{
	capture = TCNT1;
	}


