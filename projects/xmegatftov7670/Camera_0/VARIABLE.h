#ifndef _VARIABLE_H_
#define _VARIABLE_H_


#define uchar unsigned char
#define uint  unsigned int
#define ulong unsigned long

#define pgm_read_byte(x)  (*((uchar __flash *)(x)))
#define pgm_read_word(x)  (*((uint __flash *)(x)))

#endif
