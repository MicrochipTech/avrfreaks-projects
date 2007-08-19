// PC_Comm.c

#include "Osccal.h"
#include "Demonstrator.h"
#include "PC_Comm.h"


int main(void)
{
	char string[64];
	unsigned char count = 0;
	
	// run the initialization routines
	initializer();

   	 //Begin forever chatting with the PC
	 for(;;) 
	 {	
		// Check to see if a character is waiting
		if( isCharAvailable() == 1 )
		{

			// If a new character is received, get it
			string[count++] = receiveChar();
			
			// receive a packet up to 64 bytes long
			if(string[count-1] == '\n')// Hyperterminal string ends with \r\n
			{	
				string[count-2] = '\0'; //convert to a string
				parseInput(string);
				string[0] = '\0';
				count = 0;
			}
			else if(count > 64)
			{
				count = 0;
				string[0] = '\0';
				sendString("Error - received > 64 characters");			
			}		
		}
    }
	return 0;
}


char isCharAvailable()
{	
	// Does the RX0 bit of the USART Status and Control Register
	// indicate a char has been received?
	if ( (UCSRA & (0x80)) ) return 1;
	else return 0;
}

char receiveChar()
{
	// Return the char in the UDR register
	return UDR;
}

void sendChar(char data)
{
    int i = 0;

    // To send data with the USART put the data in the USART data register 
    UDR = data;

    // Check to see if the global interrupts are enabled
    if(SREG & 0x80)
    {
	// Wait until the byte is sent or we count out
		while ( !(UCSRA&0x40) && (i<10000) )
		{
			i++;
		}
    }
    else  // Wait until the byte is sent
        while( !(UCSRA&0x40) );
        
    	// Clear the TXCflag        
	UCSRA=UCSRA|0x40;          
}

void sendString(char s[])
{
	int i = 0;
	
	while(i < 64) // don't get stuck if it is a bad string
	{
		if( s[i] == '\0' ) break; // quit on string terminator
		sendChar(s[i++]);
	}
}

void USARTinit()
{
    // Increase the oscillator to 8 Mhz for the 115200 baudrate:  
    CLKPR = (1<<CLKPCE);        // set Clock Prescaler Change Enable
    // set prescaler = 1, Inter RC 8Mhz / 1 = 8Mhz
    CLKPR  = 0x00; 
   

/*
    // Increase the oscillator to 2 Mhz for the 19200 baudrate:  
    CLKPR = (1<<CLKPCE);        // set Clock Prescaler Change Enable
    // set prescaler = 4, Inter RC 8Mhz / 4 = 2Mhz
    CLKPR = (1<<CLKPS1);    
*/

    UBRRH = (uint8_t)(USART_BAUDVALUE >> 8); 
    UBRRL = (uint8_t)(USART_BAUDVALUE); 
/*
    // Set the USART baudrate registers for 19200
    UBRRH = 0;//(unsigned char)(baudrate>>8);
    UBRRL = 12;//(unsigned char)baudrate;

    // Enable 2x speed change
    UCSRA = (1<<U2X);
*/
    // Enable 1x speed change used for 115200
    UCSRA = (0<<U2X);
    // Enable receiver and transmitter
    UCSRB = (1<<RXEN)|(1<<TXEN)|(0<<RXCIE)|(0<<UDRIE);

    // Set the USART to asynchronous at 8 bits no parity and 1 stop bit
    UCSRC = (0<<UMSEL)|(0<<UPM0)|(0<<USBS)|(3<<UCSZ0)|(0<<UCPOL);
        
    // Enable interrupts
    sei(); 
    
    // Setup the interrupt mask and flags
	PCMSK1 = (1<<PINB6) | (1<<PINB4);       // set pin-change interrupt mask
  	EIFR   = (1<<PCIF1);                    // clear external intterupt flag 1
	EIMSK  = (1<<PCIE1);                // enable external interrupt 1    
}

/*

//Calibrate the internal OSCCAL byte, using the external 
//32,768 kHz crystal as reference
void OSCCAL_calibration(void)
{
    unsigned char calibrate = 0;//FALSE;
    int temp, i;
    unsigned char tempL;

    CLKPR = (1<<CLKPCE);        // set Clock Prescaler Change Enable
    // set prescaler = 8, Inter RC 8Mhz / 8 = 1Mhz
    CLKPR = (1<<CLKPS1) | (1<<CLKPS0);
    
    TIMSK2 = 0;             //disable OCIE2A and TOIE2

    ASSR = (1<<AS2);        //select asynchronous operation of timer2 (32,768kHz)
    
    OCR2A = 200;            // set timer2 compare value 

    TIMSK0 = 0;             // delete any interrupt sources
        
    TCCR1B = (1<<CS10);     // start timer1 with no prescaling
    TCCR2A = (1<<CS20);     // start timer2 with no prescaling

    while((ASSR & 0x01) | (ASSR & 0x04));       //wait for TCN2UB and TCR2UB to be cleared

    // wait for external crystal to stabilise
	for(i = 0; i < 10; i++)
			_delay_loop_2(30000);   
			
    while(!calibrate)
    {
        cli(); // mt __disable_interrupt();  // disable global interrupt
        
        TIFR1 = 0xFF;   // delete TIFR1 flags
        TIFR2 = 0xFF;   // delete TIFR2 flags
        
        TCNT1H = 0;     // clear timer1 counter
        TCNT1L = 0;
        TCNT2 = 0;      // clear timer2 counter
           
        while ( !(TIFR2 && (1<<OCF2A)) );   // wait for timer2 compareflag
    
        TCCR1B = 0; // stop timer1

        sei(); // __enable_interrupt();  // enable global interrupt
    
        if ( (TIFR1 && (1<<TOV1)) )
        {
            temp = 0xFFFF;      // if timer1 overflows, set the temp to 0xFFFF
        }
        else
        {   // read out the timer1 counter value
            tempL = TCNT1L;
            temp = TCNT1H;
            temp = (temp << 8);
            temp += tempL;
        }
    
        if (temp > 6250)
        {
            OSCCAL--;   // the internRC oscillator runs to fast, decrease the OSCCAL
        }
        else if (temp < 6120)
        {
            OSCCAL++;   // the internRC oscillator runs to slow, increase the OSCCAL
        }
        else
            calibrate = 1;//TRUE;   // the interRC is correct
    
        TCCR1B = (1<<CS10); // start timer1
    }
}
*/
