
#ifndef __delay_h
#define __delay_h

#define DELAY(us)			(Delay(((us*(XTAL/1000000))-16)/6))

void Delay(unsigned long us);

#endif
