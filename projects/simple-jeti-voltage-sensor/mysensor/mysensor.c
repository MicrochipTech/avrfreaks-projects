#include <avr/io.h>		//IO ports
#define F_CPU 4000000L		//CPU speed 4MHz (internal RC osc)
#define BAUD 9600L		//Baudrate 9600
#include <util/delay.h>		// delay
#include <stdlib.h> 
#include <stdio.h> 


// scaling fators and alarm limits
#define scale_batt (6.166666)			// Voltage divider 1,2K/6,2K 
#define scale_bus (6.166666)			// Voltage divider 1,2K/6,2K
#define bus_low_voltage (4.4)			// Minimum voltage for bus 
#define batt_low_voltage (9.7)			// Minimum voltage for battery

//UART Calculate baud rates
#define UBRR_VAL ((F_CPU+BAUD * 8)/(BAUD*16)-1)		//calculated baud rate / strictly datasheet
#define BAUD_REAL (F_CPU/(16*(UBRR_VAL+1)))			//real Baud rate
#define BAUD_ERROR ((BAUD_REAL*1000)/BAUD-1000)		//Error in 1/10%

//UART Check error tolerance
#if ((BAUD_ERROR>10)||(BAUD_ERROR<-10))
	#error Systematic baud rate error larger than 1% - this might NOT work!
#endif

// ------------------------------------------------------- Downward Compatibility -----------------------------
#define sbi(ADDRESS,BIT) ((ADDRESS) |= (1<<(BIT))) 		// compatibility with old WINAVR versions
#define cbi(ADDRESS,BIT) ((ADDRESS) &= ~(1<<(BIT)))		// handy macros for "expired" old winavr code
#define outp(VAL,ADRESS) ((ADRESS) = (VAL))
#define inp(VAL) (VAL)
#define max(a,b) (a>b?a:b)								// max function


static char firstline[17]="                ";
static char secondline[17]="                ";
static uint16_t boxstatus=0;

void USART_Init(void)
{
   //Baudrate einstellen
	UBRRH = UBRR_VAL >> 8;
	UBRRL = UBRR_VAL & 0xFF;
	// UCSRB = (1<<RXEN)|(1<<TXEN)|(1<<UCSZ2);							//UART RX und TX einschalten
	UCSRB = (1<<UCSZ2);												//hier nicht - RX und TX werden von den Schreibroutinen bedient
	UCSRC = (1<<URSEL)|(1<<UPM1)|(1<<UPM0)|(1<<UCSZ1)|(1<<UCSZ0);	//Asynchron 9O1
}

void USART_Transmit(uint16_t data)		// Original 9-Bit code from Atmega8 datasheet, adapted to our needs
{
/*
Wait for empty transmit buffer
*/
while
( !( UCSRA & (1<<UDRE)) );
/*
Copy ninth bit to TXB8
*/
UCSRB &= ~(1<<TXB8);
if ( data & 0x0100 )
UCSRB |= (1<<TXB8);
/*
Put data into buffer, sends the data
*/
UDR = data;
}


uint16_t USART_Receive(void) 		// Original 9-Bit code from Atmega8 datasheet, adapted to our needs
{
 unsigned char status, resh, resl;
/*
Wait for data to be received
*/
while(!(UCSRA & (1<<RXC)));
/*
Get status and ninth bit, then data
*/
/*
from buffer
*/
status = UCSRA;
resh = UCSRB;
resl = UDR;

sprintf(secondline,"%d  %d",UCSRA, UCSRB);

/*
If error, return -1
*/
 if(status & ((1<<FE)|(1<<DOR)|(1<<PE))) return(-1);
/*
Filter the ninth bit, then return
*/
resh = (resh >> 1) & 0x01;
return((resh << 8) | resl);
}


uint16_t Read_ADC(uint8_t mux)
{
  uint8_t i;
  uint16_t result;
  //													// ADMUX Register decomposition: /REFS1/REFS0/ADLAR/N.C./MUX3/MUX2/MUX1/MUX0 
  ADMUX = mux;                      					// Select channel, RH orientation of bit sequence
  ADMUX |= (1<<REFS1) | (1<<REFS0);					// Select internal reference
  // 													// ADCSRA Register decomposition: /ADEN/ADSC/ADFR/ADIF/ADIE/ADPS2/ADPS1/ADPS0 
  ADCSRA = (1<<ADEN) | (1<<ADPS2) | (1<<ADPS0);    	// Clock divider set to 32 and activate ADC
  //													// Sampling rate 125kHz (=4MHz/32), hardware limit is 200kHz
  ADCSRA |= (1<<ADSC);              					// start single conversion, no interrupt
    
  while ( ADCSRA & (1<<ADSC) ); 						// wait for sample to be completed
  result = ADCW;  										// Read ADCW in order to clear registers, otherwise next sample will not be read
  result = 0; 											// Discard dummy read
  for( i=0; i<4; i++ )
  {
    ADCSRA |= (1<<ADSC);            					// single conversion
    while ( ADCSRA & (1<<ADSC) );						// wait for conversion to finish
    result += ADCW;		    							// add up results (attn: only suitable for up to 14 bit data, check for overflow 
  }
  ADCSRA &= ~(1<<ADEN);             					// deactivate ADC
  result /= 4;                     						// Divide by 4 = average - division instead of 2-bit-shift for rounding instead of truncating
  return(result);										// Return conversion result (10-Bit-value in 16-bit-integer)
}

void CW_msg(char letter, uint8_t alarm)

{ 
  cbi(UCSRB,RXEN);
  sbi(UCSRB,TXEN);
  USART_Transmit(0x07E);
  USART_Transmit(0x192);
  USART_Transmit(0x122+alarm);
  USART_Transmit(letter|(1<<8));
  cbi(UCSRB,TXEN);
  sbi(UCSRB,RXEN);
}

void updatescreen(void)
{
 uint8_t i;
	cbi(UCSRB,RXEN);
    sbi(UCSRB,TXEN);
	USART_Transmit(0xFE);							// send header byte
	for (i=0; i<16; i++)
		{
			USART_Transmit((firstline[i])|(1<<8));  //setze Bit 9
		}
		
					
		for (i=0; i<16; i++)
		{
			USART_Transmit((secondline[i])|(1<<8));  //setze Bit 9
		}
    USART_Transmit(0xFF);								// send trailer byte
    cbi(UCSRB,TXEN);
    sbi(UCSRB,RXEN);
}



int main (void)
{
	
	uint16_t adc_raw;
	double battvoltage_scaled, busvoltage_scaled;
// 	uint16_t i=0;
// 	uint8_t j=0;
    uint8_t flash=0;
	uint8_t alarm;
	USART_Init();
	
	_delay_ms(50);

	
	while (1)											// Endless main loop			
	{
	
	   
        flash++;
	    alarm=0;
	    adc_raw=Read_ADC(0);						//
	    battvoltage_scaled=2.56*scale_batt*(double)adc_raw/1024.0; 
	    
		adc_raw=Read_ADC(1);	
		busvoltage_scaled=2.56*scale_bus*(double)adc_raw/1024.0; 
		if(busvoltage_scaled<bus_low_voltage)
        { 
		  sbi(alarm,1);
          if(bit_is_clear(flash,2))   
			sprintf(firstline,"BUS:%5.2f<%5.2fV", busvoltage_scaled,bus_low_voltage);
		    else sprintf(firstline,"BUS:     <%5.2fV", bus_low_voltage);
        }
		else sprintf(firstline,"BUS:%5.2f V     ",busvoltage_scaled);
	
		

		if(battvoltage_scaled<batt_low_voltage) 
		{
		 sbi(alarm,2);
		 if(bit_is_set(flash,2))
         sprintf(secondline,"BAT:%5.2f<%5.2fV", battvoltage_scaled,batt_low_voltage);
		 else sprintf(secondline,"BAT:     <%5.2fV", batt_low_voltage);    
		}
		else sprintf(secondline,"BAT:%5.2f V     ",battvoltage_scaled);
		
		
		if(alarm>0)
		{
		  CW_msg('U',1);
		}
		
		
		
		
		updatescreen();
		
		
		
		_delay_ms(34);
		boxstatus=USART_Receive();
	}
							
}

