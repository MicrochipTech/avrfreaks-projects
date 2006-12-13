#include <avr\interrupt.h>

#define CH1Bit 4    // bit 2
#define CH2Bit 8	//bit 3
#define CH3Bit	16	//bit 4
#define CHECKBIT(x,y) (x & (y)) /* Check bit y in byte x*/
extern  uint8_t status;
 extern uint8_t BuffIndex;
extern char USARTBuffer[80];

ISR(USART_RX_vect)

 {
 char data=UDR0;
 switch(data)
 {
	case ('$'):
	BuffIndex=0;
	status=1;
	break;
	
	case ('*'):
	USARTBuffer[BuffIndex++]=data;
	status=0;
	break;
	
	default:
	if(!status) break;
	USARTBuffer[BuffIndex++]=data;
 
 }   
 }
