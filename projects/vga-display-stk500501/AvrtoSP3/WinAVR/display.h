/*****************************************************
** VGA display Driver
******************************************************/

#ifndef DISPLAY__H
#define DISPLAY__H

#define VGA_CHARMEM		((volatile unsigned int *)0x4000)
#define VGA_COLOR_LUT	((volatile unsigned char *)0x7000)

#define BLACK	0
#define RED		1
#define GREEN	2
#define YELLOW	3
#define BLUE	4
#define VIOLET	5
#define CYAN	6
#define WHITE	7

#define FG(x)	(x << 11)
#define BG(x)	(x << 8)

extern void InitVGA(void);
extern void PutS(int x,int y,char c, char fg,char bg);
extern void SPutC(char c);
extern void SWrite(char *s);

#endif
