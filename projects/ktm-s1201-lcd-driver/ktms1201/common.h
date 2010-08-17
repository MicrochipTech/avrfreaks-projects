/*-------------------------------------------------------------------
  common.h   by Jack Botner
-------------------------------------------------------------------*/

#ifndef _COMMON_H_
#define _COMMON_H_ 1

#define BOOL    char

#define FALSE   0
#define TRUE    (!FALSE)
//#define NULL    0

#define nop() do { __asm__ __volatile__ ("nop"); } while (0)

// The following macros set/clear bits

#define set_bit(port,bit)  (port |= (1<<bit))
#define clear_bit(port,bit)  (port &= ~(1<<bit))
#define test_bit( reg, bitNumb )   ((reg) & (1 << (bitNumb)))

#endif

