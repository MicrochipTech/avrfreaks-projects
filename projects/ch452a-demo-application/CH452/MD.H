//
#ifndef __CH452CMD_H
#define __CH452CMD_H

#define CH452_NOP		0x0000					// 
#define CH452_RESET     0x0201					// 
#define CH452_LEVEL		0x0100					// 
#define CH452_CLR_BIT	0x0180					// 
#define CH452_SET_BIT	0x01C0					// 
#define CH452_SLEEP		0x0202					// 
#define CH452_LEFTMOV   0x0300		            // 
#define CH452_LEFTCYC   0x0301		            // 
#define CH452_RIGHTMOV  0x0302		            // 
#define CH452_RIGHTCYC  0x0303		            // 
#define CH452_SELF_BCD	0x0380					// 
#define CH452_SYSOFF    0x0400					// 
#define CH452_SYSON1    0x0401					// 
#define CH452_SYSON2    0x0403					// 
#define CH452_SYSON2W   0x0423					// 
#define CH452_NO_BCD    0x0500					// 
#define CH452_BCD       0x0580					// 
#define CH452_TWINKLE   0x0600		            // 
#define CH452_GET_KEY	0x0700					// 
#define CH452_DIG0      0x0800					// 
#define CH452_DIG1      0x0900		            // 
#define CH452_DIG2      0x0a00		            // 
#define CH452_DIG3      0x0b00		            // 
#define CH452_DIG4      0x0c00		            // 
#define CH452_DIG5      0x0d00					// 
#define CH452_DIG6      0x0e00					// 
#define CH452_DIG7      0x0f00		            // 

#define		CH452_BCD_SPACE		0x10
#define		CH452_BCD_PLUS		0x11
#define		CH452_BCD_MINUS		0x12
#define		CH452_BCD_EQU		0x13
#define		CH452_BCD_LEFT		0x14
#define		CH452_BCD_RIGHT		0x15
#define		CH452_BCD_UNDER		0x16
#define		CH452_BCD_CH_H		0x17
#define		CH452_BCD_CH_L		0x18
#define		CH452_BCD_CH_P		0x19
#define		CH452_BCD_DOT		0x1A
#define		CH452_BCD_SELF		0x1E
#define		CH452_BCD_TEST		0x88
#define		CH452_BCD_DOT_X		0x80


// 
extern	unsigned char CH452_Read(void);		
extern  void CH452_Write(unsigned short cmd);	
extern  void CH452_Init(void);

#endif
