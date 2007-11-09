/*******************************************************************/
/*          Header file of MMC_SD.c  for  MP3 Player               */
/*                                                                 */
/* Note: MMC is not tested                                         */
/*                                                                 */
/* Platform   : AVRStudio4.13 b528 + WinAVR20070122                */
/*              optimize -0s                                       */
/* Author     : bozai(Zhang Qibo)                                  */
/* E-mail     : sudazqb@163.com                                    */
/* MSN        : zhangqibo_1985@hotmail.com                         */
/* Date       : 2006-05-09                                         */
/*******************************************************************/
/*  2007-05-04: add read capacity function                         */
/*  2007-04-21:                                                    */
/*  Enable some code incase that when SD reset                     */
/*  faild program can't jump the loop                              */
/*******************************************************************/

#ifndef __MMC_SD_h__
#define __MMC_SD_h__

#define uint8  unsigned char
#define  int8    signed char
#define uint16 unsigned int
#define  int16   signed int
#define uint32 unsigned long
#define  int32   signed long



#define MMC_SD_PORT       PORTB                    //Òý½Å¶¨Òå
#define MMC_SD_CS_PIN     2     //mega8
#define DDR_INI() DDRB |= _BV(2)|_BV(3)|_BV(5)  //mega8
#define SPI_CS_Assert()   MMC_SD_PORT &= ~_BV(MMC_SD_CS_PIN)  
#define SPI_CS_Deassert() MMC_SD_PORT |=  _BV(MMC_SD_CS_PIN)


extern void MMC_SD_Init(void);
extern uint8 MMC_SD_Reset(void);
extern uint8 MMC_SD_ReadSingleBlock(uint32 sector, uint8* buffer);
extern uint8 MMC_SD_WriteSingleBlock(uint32 sector, uint8* buffer);
extern uint32 MMC_SD_ReadCapacity();
extern void SPI_High(void);


#endif
