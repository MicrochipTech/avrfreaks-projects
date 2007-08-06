//Deprecated commands that I prefer to use.
#define inp(port)   (port) 
#define outp(val, port)   (port) = (val) 
#define inb(port)   (port) 
#define outb(port, val)   (port) = (val) 
#define sbi(port, bit)   (port) |= (1 << (bit)) 
//sbi(PORTB, PB1); is now PORTB |= _BV(PB1);
#define cbi(port, bit)   (port) &= ~(1 << (bit)) 
//cbi (sfr,bit) is now sfr &= ~(_BV(bit)); 
