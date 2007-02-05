#ifndef __XYPLOTTER_H__
#define __XYPLOTTER_H__

int PutChar(const unsigned char R24);
int GetChar(void);
int Puts(const unsigned char *R25_R24);
void UART_delay(void);
void Get_adc(const unsigned char R24);
void PrepADC(const unsigned char R24);
void WaitForADC(void);

#endif
