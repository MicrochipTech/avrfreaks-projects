#ifndef __SERIAL_INCLUDED
#define __SERIAL_INCLUDED
 
#define TXC 0x40
#define RXC 0x80       
#define UDRE 0x20 


#define UDRIE 5



void init_uart(void);
int putchar(int c);
//void clr(void);

#endif
//End of GETPUT.H
