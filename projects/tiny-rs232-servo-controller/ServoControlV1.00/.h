#ifndef __ServoControl_H__
#define __ServoControl_H__

int PutChar(const unsigned char R24);
int GetChar(void);
void PutCRNL(void);
void Puts(unsigned char *R25_R24);
void PutHex(unsigned char R25_R24);
unsigned char EepromRead(const unsigned char Addres);
void EepromWrite(const unsigned char Addres,const unsigned char Data);

extern unsigned char Servo1;
extern unsigned char Servo2;
extern unsigned char Servo3;
extern unsigned char Prescaler8;
extern unsigned char Prescaler64;
extern unsigned char Prescaler20ms;
extern unsigned char Wating20ms;
extern unsigned char TXBuffer;
extern unsigned char TXCountBuffer;

#endif
