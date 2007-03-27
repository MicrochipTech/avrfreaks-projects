#ifndef __D12_Operation_h__
#define __D12_Operation_h__

#include<avr/io.h>

#define U8  unsigned char
#define U16 unsigned int
#define U32 unsigned long

/// dma parameter
#define	D12Dma0		0
#define	D12Dma1		1
#define	D12Dma2		2
#define	D12Dma3		3
#define	D12DmaEn	4
#define	D12DmaWr	8
#define	D12DmaRld	0x10
#define	D12SofIntEn	0x20
#define	D12Ep4dInt	0x40
#define	D12Ep5dInt	0x80

/// mode parameter
#define D12NoLazyClk	2
#define	D12ClkRun	4
#define	D12AllInt	8
#define	D12SoftConnect	0x10
#define	D12EpCfgMask	0x3f
#define	D12EpCfg0	0x00
#define	D12EpCfg1	0x40
#define	D12EpCfg2	0x80
#define	D12EpCfg3	0xc0
#define	D12Pll4M	11
#define	D12Pll6M	7
#define	D12Pll8M	5
#define	D12Pll12M	3
#define	D12Pll24M	1
#define	D12Pll48M	0

/// interrupt bits
#define	D12Ep0Int	1
#define	D12Ep1Int	2
#define	D12Ep2Int	4
#define	D12Ep3Int	8
#define	D12Ep4Int	0x10
#define	D12Ep5Int	0x20
#define	D12BusRst	0x40
#define	D12SuspChg	0x80
#define	D12DmaEot	1

/// endpoint status
#define	D12EpStart	0
#define	D12EpStall	1

/**************************************************************/

#define D12_DATA_PORT_DDR DDRD
#define D12_DATA_PORT     PORTD
#define D12_DATA_PIN      PIND

#define D12_CS_PORT PORTC
#define D12_CS_DDR  DDRC
#define D12_CS_BIT  5

#define D12_INT_PORT PORTC
#define D12_INT_DDR  DDRC
#define D12_INT_PIN  PINC
#define D12_INT_BIT  4

#define D12_WR_PORT PORTC
#define D12_WR_DDR  DDRC
#define D12_WR_BIT  3

#define D12_RD_PORT PORTC
#define D12_RD_DDR  DDRC
#define D12_RD_BIT  2

#define D12_A0_PORT PORTC
#define D12_A0_DDR  DDRC
#define D12_A0_BIT  1

#define D12_CMD_WR_H() PORTC = (1<<D12_CS_BIT)|(1<<D12_INT_BIT)|(1<<D12_WR_BIT)|(1<<D12_RD_BIT)|(1<<D12_A0_BIT)
#define D12_CMD_WR_L() PORTC = (0<<D12_CS_BIT)|(1<<D12_INT_BIT)|(0<<D12_WR_BIT)|(1<<D12_RD_BIT)|(1<<D12_A0_BIT)

#define D12_CMD_RD_H() PORTC = (1<<D12_CS_BIT)|(1<<D12_INT_BIT)|(1<<D12_WR_BIT)|(1<<D12_RD_BIT)|(1<<D12_A0_BIT)
#define D12_CMD_RD_L() PORTC = (0<<D12_CS_BIT)|(1<<D12_INT_BIT)|(1<<D12_WR_BIT)|(0<<D12_RD_BIT)|(1<<D12_A0_BIT)

#define D12_DAT_WR_H() PORTC = (1<<D12_CS_BIT)|(1<<D12_INT_BIT)|(1<<D12_WR_BIT)|(1<<D12_RD_BIT)|(0<<D12_A0_BIT)
#define D12_DAT_WR_L() PORTC = (0<<D12_CS_BIT)|(1<<D12_INT_BIT)|(0<<D12_WR_BIT)|(1<<D12_RD_BIT)|(0<<D12_A0_BIT)

#define D12_DAT_RD_H() PORTC = (1<<D12_CS_BIT)|(1<<D12_INT_BIT)|(1<<D12_WR_BIT)|(1<<D12_RD_BIT)|(0<<D12_A0_BIT)
#define D12_DAT_RD_L() PORTC = (0<<D12_CS_BIT)|(1<<D12_INT_BIT)|(1<<D12_WR_BIT)|(0<<D12_RD_BIT)|(0<<D12_A0_BIT)

#define D12_CS_H() PORTC |= 1<<D12_CS_BIT
#define D12_CS_L() PORTC &= ~(1<<D12_CS_BIT)

#define D12_WR_H() PORTC |= 1<<D12_WR_BIT
#define D12_WR_L() PORTC &= ~(1<<D12_WR_BIT)

#define D12_RD_H() PORTC |= 1<<D12_RD_BIT
#define D12_RD_L() PORTC &= ~(1<<D12_RD_BIT)

#define D12_A0_H() PORTC |= 1<<D12_A0_BIT
#define D12_A0_L() PORTC &= ~(1<<D12_A0_BIT)

#define D12_INT (D12_INT_PIN & (1<<D12_INT_BIT))

#define D12_INPUT_MODE()  D12_DATA_PORT_DDR = 0x00
#define D12_OUTPUT_MODE() D12_DATA_PORT_DDR = 0xff

#define D12_INPUT  D12_DATA_PIN
#define D12_OUTPUT D12_DATA_PORT

//#define	D12_CMD	*(volatile U8 *)(0x2000001)
//#define	D12_DAT	*(volatile U8 *)(0x2000000)

/**************************************************************/

#define	MyD12DmaCfg		D12Ep4dInt
#define	MyD12EpCfgOff	D12EpCfg0|D12AllInt|D12ClkRun|D12NoLazyClk	
#define	MyD12EpCfgOn	MyD12EpCfgOff|D12SoftConnect

/***********************************************************/
extern void D12_PORT_INI(void);
extern U16  D12_Read_Interrupt_Register(void);
extern void D12_Write_Endpoint(U8 Endpoint, U8 * Buffer, U8 Count);
extern U8   D12_Read_Endpoint(U8 Endpoint,U8 * Buffer,U8 Count);
extern U16  D12_Read_Chip_ID(void);
extern void D12_Send_Resume(void);
extern void D12_Ack_Endpoint(U8 Endpoint);
extern void D12_Set_Mode(U8 Val1, U8 Val2);
extern void D12_Set_DMA(U8 DMA);
extern U8   D12_Get_DMA(void);
extern void D12_Set_Endpoint_Enable(U8 Enable);
extern U8   D12_Select_Endpoint(U8 Endpoint);
extern void D12_Set_Endpoint_Status(U8 Endpoint,U8 Status);
extern void D12_Set_Address_Enable(U8 Addr,U8 Enable);
extern void D12_Clear_Buffer(void);
extern void D12_Enable_Buffer(void);
extern U16  D12_Read_Current_Frame_Number(void);
extern U8   D12_Read_Last_Transaction_Status(U8 Endpoint);
extern void D12_Write_Endpoint_SD(U8 Endpoint);
extern U8 D12_Read_Endpoint_SD(U8 Endpoint);

extern U8 D12_Read_Endpoint_SD1(U8 Endpoint);
#endif
