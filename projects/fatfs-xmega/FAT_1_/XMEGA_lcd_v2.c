// Modified by Ozhan KD (www.knowledgeplus.ir)
#include "XMEGA_lcd_v2.h"

void LCDDataLines(unsigned char Data)
{
 unsigned char temp;
 temp=(Data&0x01);
 LCD_D4_DATA=((LCD_D4_DATA&~(1<<LCD_D4_BIT))|(temp<<LCD_D4_BIT));
 temp=((Data&0x02)>>1);
 LCD_D5_DATA=((LCD_D5_DATA&~(1<<LCD_D5_BIT))|(temp<<LCD_D5_BIT));
 temp=((Data&0x04)>>2);
 LCD_D6_DATA=((LCD_D6_DATA&~(1<<LCD_D6_BIT))|(temp<<LCD_D6_BIT));
 temp=((Data&0x08)>>3);
 LCD_D7_DATA=((LCD_D7_DATA&~(1<<LCD_D7_BIT))|(temp<<LCD_D7_BIT));
}

void LCDDirOut(void)
{
 LCD_D4_DIR|=(1<<LCD_D4_BIT);
 LCD_D5_DIR|=(1<<LCD_D5_BIT);
 LCD_D6_DIR|=(1<<LCD_D6_BIT);
 LCD_D7_DIR|=(1<<LCD_D7_BIT);
}

void LCDDirIn(void)
{
 LCD_D4_DIR&=~(1<<LCD_D4_BIT);
 LCD_D5_DIR&=~(1<<LCD_D5_BIT);
 LCD_D6_DIR&=~(1<<LCD_D6_BIT);
 LCD_D7_DIR&=~(1<<LCD_D7_BIT);
}

void LCDSendData(unsigned char Data)
{
 #ifdef LCD_BUSY_CHECK
  LCDBusyWait();
  RW_Low();
 #else
  delay_us(100);
 #endif
 LCDDataLines(Data>>4);
 RS_High();
 E_High();
 delay_us(20);
 E_Low();
 delay_us(20);
 LCDDataLines(Data);
 E_High();
 delay_us(20);
 E_Low();
}
void LCDSendCommand(unsigned char Command)
{
 #ifdef LCD_BUSY_CHECK
  LCDBusyWait();
  RW_Low();
 #else
  if ((Command==LCD_CLR)||(Command==LCD_HOME))
   delay_ms(5);
  else delay_ms(1);

 #endif
 LCDDataLines((Command>>4));
 RS_Low();
 E_High();
 delay_us(20);
 E_Low();
 delay_us(20);
 LCDDataLines(Command);
 E_High();
 delay_us(20);
 E_Low();
}
void LCDInit(void)
{
 E_Low();
 LCDDirIn();
 delay_ms(40);
 LCDDirOut();
 LCDDataLines(0x30>>4);
 LCD_RS_DIR|=(1<<LCD_RS_BIT);
 LCD_RW_DIR|=(1<<LCD_RW_BIT);
 RS_Low();
 RW_Low();
 LCD_E_DIR|=(1<<LCD_E_BIT);
 delay_us(20);
 E_High();
 delay_us(20);
 E_Low();
 delay_ms(20);
 E_High();
 delay_us(20);
 E_Low();
 delay_us(100);
 E_High();
 delay_us(20);
 E_Low();
 delay_us(100);
 LCDDataLines(0x20>>4);
 delay_us(20);
 E_High();
 delay_us(20);
 E_Low();
 delay_us(100);
 LCDSendCommand(0x28);
 LCDSendCommand(0x08);
 LCDSendCommand(0x01);
 LCDSendCommand(0x06);
 LCDSendCommand(0x0C);
}			

void LCDClr(void) 	//Clears LCD
{
 LCDSendCommand(LCD_CLR);
}

void LCDHome(void)	//LCD cursor home
{
 LCDSendCommand(LCD_HOME);
}

void LCDStringRam(char *data)	
{
 char k;	
 while (k=*data++) LCDSendData(k);
}
void LCDStringFlash(char __flash *data)	
{
 char k;
 while (k=*data++) LCDSendData(k);
}
void LCDGotoXY(unsigned char x, unsigned char y)	//Cursor to XY position
{
 unsigned char DDRAMAddr;	
#if (LCD_SIZE==0) || (LCD_SIZE==2) || (LCD_SIZE==7)
 DDRAMAddr = LCD_LINE0_DDRAMADDR+x;
#else 
 switch(y)
 {
  case 0: DDRAMAddr = LCD_LINE0_DDRAMADDR+x; break;
  case 1: DDRAMAddr = LCD_LINE1_DDRAMADDR+x; break;

#if (LCD_SIZE==4) || (LCD_SIZE==6)
  case 2: DDRAMAddr = LCD_LINE2_DDRAMADDR+x; break;
  case 3: DDRAMAddr = LCD_LINE3_DDRAMADDR+x; break;
#endif  
  
  default: DDRAMAddr = LCD_LINE0_DDRAMADDR+x;
 }	
#endif   
 LCDSendCommand(1<<LCD_DDRAM | DDRAMAddr);	
}
void LCDdefinechar(uint8_t __flash *pc,uint8_t char_code){
	uint8_t a, pcc;
	uint16_t i;
	a=(char_code<<3)|0x40;
	for (i=0; i<8; i++){
		pcc=pgm_read_byte(&pc[i]);
		LCDSendCommand(a++);
		LCDSendData(pcc);
		}
}

void LCDShiftLeft(uint8_t n)	//Scroll n of characters Right
{
	for (uint8_t i=0;i<n;i++)
	{
		LCDSendCommand(0x1E);
	}
}
void LCDShiftRight(uint8_t n)	//Scroll n of characters Left
{
	for (uint8_t i=0;i<n;i++)
	{
		LCDSendCommand(0x18);
	}
}
void LCDCursorOn(void) //displays LCD cursor
{
	LCDSendCommand(0x0E);	
}
void LCDCursorOnBlink(void)	//displays LCD blinking cursor
{
	LCDSendCommand(0x0F);
}
void LCDCursorOffBlink(void)	
{
	LCDSendCommand(0x0D);
}
void LCDCursorOFF(void)	//turns OFF cursor
{
	LCDSendCommand(0x0C);
}
void LCDBlank(void)		//blanks LCD
{
	LCDSendCommand(0x08);
}
void LCDVisible(void)		//Shows LCD
{
	LCDSendCommand(0x0C);
}
void LCDCursorLeft(uint8_t n)	//Moves cursor by n poisitions left
{
	for (uint8_t i=0;i<n;i++)
	{
		LCDSendCommand(0x10);
	}
}
void LCDCursorRight(uint8_t n)	//Moves cursor by n poisitions left
{
	for (uint8_t i=0;i<n;i++)
	{
LCDSendCommand(0x14);
	}
}

void LCDBusyWait(void)
{
 RS_Low();
 LCDDirIn();
 RW_High();
 E_High();
 delay_us(1);
 while(LCD_D7_IN&(1<<LCD_D7_BIT))
 {
  E_Low();
  delay_us(1);
  E_High();
  delay_us(1);
  E_Low();
  delay_us(1);
  E_High();
  delay_us(1);
 }
 E_Low();
 LCDDirOut();
}