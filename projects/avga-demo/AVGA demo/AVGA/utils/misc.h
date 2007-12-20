/*
	file: misc.h
	desc: some miscelanneous useful functions...

	author: Jaromir Dvorak (md@unicode.cz)

*/

#ifndef _MISC_H_
#define _MISC_H_


#define __device_reset()  	asm volatile("jmp 0")

static inline void strdump_P(char* dest, PGM_P src)
{
 while(1)
 {
	char c = pgm_read_byte(src++);
	if(!c) break;
	*dest++ = c;
 } 
}


static inline void strdump(char* dest, char* src)
{
 while(1)
 {
	char c = *src++;
	if(!c) break;
	*dest++ = c;
 } 
}


static inline signed char sgn(signed char i)
{
	if(i>=0) return 1;
	else     return -1;
}

#endif
