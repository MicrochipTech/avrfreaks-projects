

#ifndef __ASM_MATH_H__ 
#define __ASM_MATH_H__ 


uint16_t   	num16;
uint32_t 	num32;
uint8_t	 	abuff[11];

void _itoa16_ASM();
void _itoa32_ASM();

#define _itoa16(n)num16=n;_itoa16_ASM();
#define _itoa32(n)num32=n;_itoa32_ASM();

#endif // __ASM_MATH_H__
