
#ifndef _UART_H_
#define _UART_H_


#include <avr/io.h>
#include <avr/interrupt.h>


#define WAIT_UNTIL_BYTE_SENT    1      // wait until byte sent before returning when calling the sendByte() function
#define TXREG         	    	DDRB   // TXPORT REGISTER 
#define TXPORT        	    	PORTB  // port used for uart tx
#define TXPIN         	    	0      // pin used for uart tx
#define PACKETDELAY      		2      // null bits to sent between each byte sent
#define BAUD            		67     // baud rate value use below formula to change baud



//   BAUD FORMULA     baud rate value = ((IO CLK/PRESCALER)/1000000)*(1000000/BAUD)

//   NOTE: IF the baud rate value is larger than 255 then you'll have to change the prescaller
//   NOTE: Round Baud rate value to nearest 1 and if it dosn't work still try changing it up and down slightly.
//   NOTE: Current baud rate value is set to 115200 baud on a ATTiny 45 clocked at 20 MHZ with a 8 MHZ IO CLK

//   PRESCALER:  The timer prescaler setting
//   IO CLK:     NOT F_CPU, INTERNAL OSCILATOR e.g.. 4000000 MHZ, 8000000 MHZ or 9600000 MHZ










volatile uint8_t bit = (PACKETDELAY + 8); // start stopped



volatile uint8_t byt = 0;


/* Interrupt service routine */


ISR(TIM1_COMPA_vect)
{

// START BIT
if(!bit){


TXPORT &= ~(1 << TXPIN); // START BIT
bit++;


} else if(bit < 9){


//DATA BITS

if((byt & 0x1)){ //0b10000000, test left most bit // DATA BITS
TXPORT |= (1 << TXPIN);  // TXPEN HIGH
} else {
TXPORT &= ~(1 << TXPIN); // TXPEN LOW
}

byt >>= 1;
bit++;




} else {



//STOP BIT

TXPORT |= (1 << TXPIN);  // STOP BIT 

if(bit == (PACKETDELAY + 8)){ // TIME BETWEEN BYTES SENT

return; //return, All data is sent

} else {

bit++;

}


}


}

void sendByte(uint8_t byte)
{

while(!(bit == (PACKETDELAY + 8))){} //   wait/verify current byte sent

byt = byte; // load buffer
bit = 0;    // set current bit to 0

if(WAIT_UNTIL_BYTE_SENT){
while(!(bit == (PACKETDELAY + 8))){} //   wait until current byte sent 
}

} // end function




static inline void uartInit(void)
{
   TXREG |= (1 << TXPIN);   // Set TXPIN/PORT  to output. 
   TXPORT |= (1 << TXPIN);  // Set TXPIN HIGH (IDLE STATE)
   TCCR1  |= (1<<CTC1);     // Clear Timer/Counter on Compare Match
   TCCR1  |= (1<<CS10);     // Prescaller = 1      
   OCR1C = BAUD;            // Explanation above. set OCR1C to be value to reset TCNT1 68 = 8.6-8.7 us  - 115200 baud
   TCNT1 = 0;		        // Just set it to check the right bits are set
   TIMSK |= (1<<OCIE1A);    // Enable Interupt on Compare
   sei();			        // enable interrupts 
}

#endif

