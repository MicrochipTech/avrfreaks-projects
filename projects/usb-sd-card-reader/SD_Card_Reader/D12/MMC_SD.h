#ifndef __MMC_SD_h__
#define __MMC_SD_h__

#define uint8  unsigned char
#define  int8    signed char
#define uint16 unsigned int
#define  int16   signed int
#define uint32 unsigned long
#define  int32   signed long



#define MMC_SD_PORT       PORTB                    //Òý½Å¶¨Òå
//#define MMC_SD_CS_PIN     4
#define MMC_SD_CS_PIN     2     //mega8
//#define DDR_INI() DDRB |=_BV(4)|_BV(5)|_BV(7)
#define DDR_INI() DDRB |= _BV(2)|_BV(3)|_BV(5)  //mega8
#define SPI_CS_Assert()   MMC_SD_PORT &= ~_BV(MMC_SD_CS_PIN)  
#define SPI_CS_Deassert() MMC_SD_PORT |=  _BV(MMC_SD_CS_PIN)


extern void MMC_SD_Init(void);
extern uint8 MMC_SD_Reset(void);
extern uint8 MMC_SD_ReadSingleBlock(uint32 sector, uint8* buffer);
extern uint8 MMC_SD_WriteSingleBlock(uint32 sector, uint8* buffer);
extern void SPI_High(void);
extern uint8 MMC_SD_SendCommand(uint8 cmd, uint32 arg);
extern uint8 SPI_WriteByte(uint8 val);
extern uint32 MMC_SD_ReadCapacity(void);
#endif
