/**************************************************************/
/*                  贪食蛇游戏 液晶程序  V1.0                 */
/*          硬件m64 + 128*64图形液晶（ks0108）                */
/*  环境：AVRSstudio4.12 SP4 + WINAVR20060421                 */
/*  作者：Bozai(章其波)                                       */
/*  日期: 2006-12-30                                          */
/**************************************************************/
/*           Driver of 128*64LCD  (KS0108) for SNAKE          */
/*  Author: Zhang Qibo                                        */
/*  E-mail: sudazqb@163.com                                   */
/*  MSN   : zhangqibo_1985@hotmail.com                        */
/*  Date  : 2006-12-31                                        */
/**************************************************************/

#ifndef __LCD_h__
#define __LCD_h__

#include<avr/io.h>
#include<avr/pgmspace.h>

#define uchar unsigned char
#define uint  unsigned int

//Address of the LCD
#define CMD_READ  (*(volatile unsigned char *)0x2008)
#define CMD_WRITE (*(volatile unsigned char *)0x2000)
#define DAT_READ  (*(volatile unsigned char *)0x2018)
#define DAT_WRITE (*(volatile unsigned char *)0x2010)

#define SET_RST_OUTPUT() DDRG |= _BV(PG4)
#define SET_RST()		PORTG |= _BV(PG4)
#define CLR_RST()		PORTG &=~_BV(PG4)

#define SET_CSA_OUTPUT() DDRD |= _BV(PD5)
#define SET_CSA()		PORTD |= _BV(PD5)
#define CLR_CSA()		PORTD &=~_BV(PD5)

#define SET_CSB_OUTPUT() DDRD |= _BV(PD6)
#define SET_CSB()		PORTD |= _BV(PD6)
#define CLR_CSB()		PORTD &=~_BV(PD6)


/****************底层操作程序声明**************************************/
extern void BusyOrNot(void);                          		//判忙             /
extern void WriteCommandToRightArea(uchar Command);		//向右区写命令     /
extern void WriteDataToRightArea(uchar _Data);	//向右区写数据     /
extern void WriteCommandToLeftArea(uchar Command);			//向左区写命令     /
extern void WriteDataToLeftArea(uchar _Data);  //向左区写数据     /
/**********************************************************************/

																										 
/**************************************************************/
extern void ClearDisBuf(uchar val);              //清内存
extern void ShowLcd(uchar on);               //开显示
extern void InitialLcd(void);			      //清液晶RAM,定位起始0行0列，开显示
extern void LocateLcdColumn(uchar Column);   //列定位
extern void LocateLcdPage(uchar Page);       //页定位
extern void LocateLcdPageColumn(uchar Page,uchar Column);//定位 页与列
extern void UpdateDis(void);

extern void OnePixel(uchar x,uchar y,uchar val);
extern uchar ReadPixel(uchar x,uchar y);

extern void LCDDelay(uint n);
//延时


#endif
