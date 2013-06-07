/*
 * CX80.h
 *
 * Created: 20/02/2013 11:24:46
 *  Author: Calogiuri Enzo Antonio
 */ 


#ifndef CX80_H_
#define CX80_H_

#define F_CPU					16000000UL
#define BAUD					38400UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util/setbaud.h>
#include <avr/pgmspace.h>

/*CX80 SYSTEM LED CONTROL PORT*/
#define LED_PORT				PORTB
#define LED_PORT_DDR			DDRB
#define PWR_LED					0
#define SDW_LED					1
#define SDR_LED					2

/*CX80 IOREQ CONTROL PORT*/
#define IOREQ_PORT				LED_PORT
#define IOREQ_PORT_DDR			LED_PORT_DDR
#define IOREQ					3

/*CX80 ADDRESS BUS CONTROL PORT*/
#define ADDRESS_BUS_PORT		PORTA
#define ADDRESS_BUS_PORT_DDR	DDRA

/*CX80 DATA BUS CONTROL PORT*/
#define DATA_BUS_PORT			PORTC
#define DATA_BUS_PORT_DDR		DDRC
#define DATA_BUS_PORT_READ		PINC

/*CX80 SYSTEM BUS CONTROL PORT*/
#define SYSTEM_BUS_PORT			PORTD
#define SYSTEM_BUS_PORT_DDR		DDRD
#define SYSTEM_LATCH			0
#define SYSTEM_RAM_SEL			4
#define SYSTEM_RAM_WR			5
#define SYSTEM_SD_SEL			6
#define SYSTEM_SND				7

#define SRam_Select()			(SYSTEM_BUS_PORT &= ~(1 << SYSTEM_RAM_SEL))
#define SRam_Deselect()			(SYSTEM_BUS_PORT |= (1 << SYSTEM_RAM_SEL))
#define SRam_WE_High()			(SYSTEM_BUS_PORT |= (1 << SYSTEM_RAM_WR))
#define SRam_WE_Low()			(SYSTEM_BUS_PORT &= ~(1 << SYSTEM_RAM_WR))

#define Latch_LE_Low()			(SYSTEM_BUS_PORT &= ~(1 << SYSTEM_LATCH))
#define Latch_LE_High()			(SYSTEM_BUS_PORT |= (1 << SYSTEM_LATCH))

#define SD_CS_Low()				(SYSTEM_BUS_PORT &= ~(1 << SYSTEM_SD_SEL))
#define SD_CS_High()			(SYSTEM_BUS_PORT |= (1 << SYSTEM_SD_SEL))

#define HiByte(x)				(unsigned char)(x >> 8)
#define LoByte(x)				(unsigned char)(x & 0xFF)

typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned short uInt;
typedef unsigned long DWord;

void Setup_CX80_Led_Port(void);
void Enable_CX80_Led(unsigned char nLed);
void Disable_CX80_Led(unsigned char nLed);

/*ADDRESS, DATA AND SYSTEM BUS CONTROL FUNCTIONS*/
void Setup_CX80_Address_Bus(void);
void Setup_CX80_Data_Bus_Input(void);
void Setup_CX80_Data_Bus_Output(void);
void Setup_CX80_System_Bus(void);
void Setup_CX80_SD_Bus(void);

/*EXTERNAL SRAM I/O FUNCTIONS*/
byte RMem(register word Addr);
word RMem16(register word Addr);
void WMem(register word Addr, register byte Value);
void WMem16(register word Addr, register word Value);

/*USART CONTROL FUNCTION - FOR VIDEO OUTPUT*/
void Seput_CX80_Usart(void);
void CX80_Usart_SendChar(char c);

/*VIDEO CONTROL FUNCTIONS*/
void CX80_Do_PrintString(const char *str);
void CX80_Do_CursorUp(void);
void CX80_Do_CursorDown(void);
void CX80_Do_CursorRight(void);
void CX80_Do_CursorLeft(void);
void CX80_Do_ClearScreen(void);
void CX80_Do_CursorHome(void);
void CX80_Do_ReverseLineFeed(void);
void CX80_Do_ClearToEndOfScreen(void);
void CX80_Do_ClearToEndOfLine(void);
void CX80_Do_MoveCursor(char r, char c);
void CX80_Do_SetLineDimension(char c);
void CX80_Do_ClearToStartOfScreen(void);
void CX80_Do_CursorShow(char c);
void CX80_Do_SaveCursorPosition(void);
void CX80_Do_RestoreCursorPosition(void);
void CX80_Do_ClearLine(void);
void CX80_Do_ClearToStartOfLine(void);
void CX80_Do_LineOverflow(char c);
void CX80_Do_BlockCursor(void);
void CX80_Do_UnderlineCursor(void);
void CX80_Do_VideoOutput(char c);

/*CX80 Z80 CPU DEFINITIONS*/
#define CX80_CPU_STARTRAM					0x0008
#define CX80_SYSTEM_CALL_ADDR				0x0007
#define CX80_SYSTEM_CALL_FNC				0x0000
#define CX80_SYSTEM_REG1L					0x0001
#define CX80_SYSTEM_REG1H					0x0002
#define CX80_SYSTEM_REG2L					0x0003
#define CX80_SYSTEM_REG2H					0x0004
#define CX80_SYSTEM_REG3L					0x0005
#define CX80_SYSTEM_REG3H					0x0006

// registers
#define B									0
#define C									1
#define D									2
#define E									3
#define H									4
#define L									5
#define PHL									6   // (HL)
#define A									7
#define IXH									8   // H+4
#define IXL									9
#define IYH									10  // H+6
#define IYL									11

// register pairs
#define BC									0
#define DE									1
#define HL									2
#define rSP									3       // alternatively AF
#define IX									4       // HL+ixy
#define IY									5       // HL+ixy

// FLAGS
#define FNZ									0
#define FZ									1
#define FNC									2
#define FC									3
#define FPO									4
#define FPE									5
#define FP									6
#define FM									7

extern byte reg[12], reg2[8];
extern byte fC, fN, fP, fU3, fH, fU5, fZ, fS;
extern byte fC_, fN_, fP_, fU3_, fH_, fU5_, fZ_, fS_;
extern word regPC, regSP;
extern byte CPU_Stop_Run;

void CX80_InitZ80CPU(void);
void CX80_Z80Run(void);

/*SYSTEM CALLS STUFF*/
extern char OsCmdLine[38];
extern char argv2[20], argv3[20];
void CX80_Do_SystemCalls(void);

#endif /* CX80_H_ */