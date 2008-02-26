//Datei: PROTOS.H
//
//Nur Prototypen
//
//04.10.2003
//
//Compiler AVR-GCC

#ifndef PROTOS_H
#define PROTOS_H

#include "display.h"
#include "ks108.h"

//circle.c
extern void Circle(unsigned int xpos, unsigned int ypos, unsigned int radius, unsigned char step);
extern void FillCircle(unsigned int xpos, unsigned int ypos, unsigned int radius, unsigned char step);

//fontsoft.c
extern void SetFont(unsigned char num);
extern void LCDSoftChar(unsigned char ch, unsigned int xpos, unsigned int ypos);
extern void LCDSoftCharV(unsigned char ch, unsigned int xpos, unsigned int ypos);
extern void LCDSoftText(char *__putstr, unsigned int xpos, unsigned int ypos);
extern void LCDSoftTextV(char *__putstr, unsigned int xpos, unsigned int ypos);
extern void ShowSoftFont(unsigned char num);
extern void ShowSoftFontV(unsigned char num);

//draw.c
extern volatile unsigned int xcursor,ycursor;
//extern void SetGCursor(unsigned int x, unsigned int y);
//extern void DrawLineRel(unsigned int dx, unsigned int dy, unsigned char mode);
//extern void DrawLineAbs(unsigned int x, unsigned int y, unsigned char mode);
extern void DrawLine(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2);
extern void DrawYLineLen(unsigned int y, unsigned int len, unsigned int x);
extern void DrawXLineLen(unsigned int x, unsigned int len, unsigned int y);
extern void DrawYLine(unsigned int y1, unsigned int y2, unsigned int x);
extern void DrawXLine(unsigned int x1, unsigned int x2, unsigned int y);
extern void FillRect(unsigned int x, unsigned int y, unsigned int width, unsigned int height);
extern void DrawRect(unsigned int x, unsigned int y, unsigned int width, unsigned int height);

//Bitmap.c
extern void LoadBitmap(unsigned char *bitmap,unsigned int xpos , unsigned int ypos, unsigned int width, unsigned int height);

//Hardwareeinstellung beim Start
extern void InitDisplay(void);
extern void Delay1ms(unsigned int);
extern void Delay100us(unsigned char);

#endif
