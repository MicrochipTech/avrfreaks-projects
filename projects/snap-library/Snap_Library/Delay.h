
#ifndef __delay_h
#define __delay_h

#define DELAY(us)			(Delay(((us*(XTAL/1000000.0))-16)/6))

#define MIN_DELAY_US		((1000000.0*(6+16))/XTAL)


void Delay(unsigned long us);

#endif
