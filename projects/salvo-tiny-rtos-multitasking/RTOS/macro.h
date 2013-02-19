/*****************************************************************************
*
* Description   : useful macros
*
****************************************************************************/

#define SetBit(x,y) (x|=(1u<<y))
#define ClrBit(x,y) (x&=~(1u<<y))
#define ToggleBit(x,y) (x^=(1u<<y))
#define TestBit(x,y) (x&(1u<<y))

#define WDR() 	asm("wdr")
#define SEI()	asm("sei")
#define CLI()	asm("cli")
#define NOP()	asm("nop")
#define _WDR() 	asm("wdr")
#define _SEI()	asm("sei")
#define _CLI()	asm("cli")
#define _NOP()	asm("nop")
#define SLEEP() asm("sleep")



