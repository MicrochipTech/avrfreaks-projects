#ifndef _PORT_H_
#define _PORT_H_
#include <avr/io.h>
void port_set_portbit(unsigned char portchar, unsigned char pin, unsigned char val);
unsigned char port_get_portbit(unsigned char portchar, unsigned char pin);
#endif

