/*******************************************************************/
/*          Head file of VS1003B.c  for  Mega8 MP3 Player          */
/*                                                                 */
/* Platform   : AVRStudio4.13 b528 + WinAVR20070525                */
/*              optimize -0s                                       */
/* Author     : bozai(Zhang Qibo)                                  */
/* E-mail     : sudazqb@163.com                                    */
/* MSN        : zhangqibo_1985@hotmail.com                         */
/* Date       : 2006-05-09                                         */
/*******************************************************************/
/*2007-05-04: add slow start up code, and add enough dealy         */
/*2007-04-21:                                                      */
/*******************************************************************/

#ifndef __VS1003B_H__
#define __VS1003B_H__

#define DefaultVolume   0x2828
#define CLOCK_REG       0xc000          //0xc00 is fow VS1003 or higher version
//#define CLOCK_REG       0x9800        //0x9800 is for VS1011 VS1002

#define VS1003B_PORT    PORTC
#define VS1003B_DDR      DDRC
#define VS1003B_PIN      PINC

#define VS1003B_XCS     3
#define VS1003B_XRESET  0
#define VS1003B_DREQ    1
#define VS1003B_XDCS    2


#define PORT_INI()        VS1003B_DDR |= _BV(VS1003B_XCS)|_BV(VS1003B_XRESET)|_BV(VS1003B_XDCS)

#define VS1003B_XCS_H()    VS1003B_PORT |=  _BV(VS1003B_XCS)
#define VS1003B_XCS_L()    VS1003B_PORT &= ~_BV(VS1003B_XCS)

#define VS1003B_XRESET_H()    VS1003B_PORT |=  _BV(VS1003B_XRESET)
#define VS1003B_XRESET_L()    VS1003B_PORT &= ~_BV(VS1003B_XRESET)

#define VS1003B_XDCS_H()    VS1003B_PORT |=  _BV(VS1003B_XDCS)
#define VS1003B_XDCS_L()    VS1003B_PORT &= ~_BV(VS1003B_XDCS)

extern void VS1003B_SPI_Low(void);//低速spi
extern void VS1003B_SPI_High(void);//高速spi
extern unsigned char VS1003B_WriteByte(unsigned char CH);//spi写字节
extern unsigned char VS1003B_ReadByte();//spi读字节
extern void VS1003B_WriteCMD(unsigned char addr, unsigned int dat);//写寄存器
extern unsigned int VS1003B_ReadCMD(unsigned char addr);//读寄存器
extern void VS1003B_WriteDAT(unsigned char dat);//写数据，音乐数据
extern unsigned char VS1003B_Init();//初始化
extern void VS1003B_SoftReset();//软件复位
#endif
