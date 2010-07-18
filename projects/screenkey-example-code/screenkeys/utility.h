#ifndef UTILITY_H
#define UTILITY_H

#include <avr/io.h>


/*
 *	Convieninece macro to combine I/O DDR/PORT/PIN and pin assignments together
 *
 *  Usage example:
 *
 *		#define LED2(reg) BIT(D,5,reg)
 *		#define SW2(reg) BIT(D,2,reg)
 * 
 *		int main()
 *		{
 *	    	SW2( DDR) = 0;
 *	    	SW2( PORT) = 1; //pullup on
 *	    	LED2( DDR) = 1;   
 *	    	while( 1)
 *			{
 *	        	if( SW2( PIN) == 0)
 *				{
 *	            	LED2( PORT) = 1;
 *	        	}
 *	        	else
 *				{
 *	            	LED2( PORT) = 0;
 *	        	}
 *	    	}
 *		} 
 */
struct bits {
  uint8_t b0:1;
  uint8_t b1:1;
  uint8_t b2:1;
  uint8_t b3:1;
  uint8_t b4:1;
  uint8_t b5:1;
  uint8_t b6:1;
  uint8_t b7:1;
} __attribute__((__packed__));
#define BIT(name,pin,reg) \
 ((*(volatile struct bits*)&reg##name).b##pin)


// Logic constants
#define FALSE 0
#define TRUE !FALSE
#define HIGH 1
#define LOW 0
#define OUT 1
#define IN 0
#define ON 1
#define OFF 0


#endif  //UTILITY_H
