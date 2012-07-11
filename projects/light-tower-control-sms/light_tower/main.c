#include <avr/io.h>
#include <string.h>

#ifndef FOSC
#define FOSC 8000000UL
#endif
#ifndef BAUD
#define BAUD 9600UL
#endif

int U, j;
char cmd_buff[80];							// buffer to construct parsable string terminating with CR
		
void main(void)								//main function starts here
{  
DDRB=0xFF;									//make PORTB an output
UCR |= (1 << RXEN) | (1 << TXEN); 			// Turn on the transmission and reception circuitry 
UCR = (1<<RXCIE)|(1<<TXEN)|(1<<RXEN);		// 8 Databits, receive and transmit enabled, receive complete interrupt enabled
UBRR = FOSC / (16 * BAUD) - 1;				// Set baudrate
PORTB = 0xFF;
while (1==1){								//cycle forever - main loop with no exit option
	
	while ((USR & (1 << RXC)) == 0) {  };	// Do nothing until data have been received and is ready to be read from UDR 
	U = UDR;								// move the RXed character from UDR register to U variable
	if (U == 0x0D)							// carriage return? if yes end of line and compare for cmd_buff
		{				
		if (strncmp(cmd_buff,"\x0Ayellow",6) == 0 ) {PORTB &= ~(1<<PINB1);}	//turn ON yellow loght by LOW on pin (Opto board needs sink)
		if (strncmp(cmd_buff,"\x0Ared",3) == 0 ) {PORTB &= ~(1<<PINB2);}				//turn ON red light by LOW on pin PORTB
		if (strncmp(cmd_buff,"\x0Agreen",5) == 0 ) {PORTB &= ~(1<<PINB3);}				//turn ON blue light by LOW on pin PORTB
		if (strncmp(cmd_buff,"\x0Awhite",5) == 0 ) {PORTB &= ~(1<<PINB4);}			//turn ON white light by LOW on pin PORTB
		if (strncmp(cmd_buff,"\x0Aoff",3) == 0 ) {PORTB = 0xFF;}						//turn ON white light by LOW on pin PORTB
		j=0;																		// reset buffer counter to start from begining
		} 	else	{	
			cmd_buff[j] = U;												//copy RXd character to the buffer array
			j++;															// increase the char counter for the buff array
			if (j >= 79) {j=0;}												// if buffer is at 80 then reset back to zero
					}	
			}
}
