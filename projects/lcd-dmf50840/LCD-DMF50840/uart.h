/*
   Based on code from Jesper Hansen & Volker Oth
  
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software Foundation, 
  Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

  This module provides semi-interrupt driven Serial Out and
  non-buffered Serial In (optional)

  Add sram based string output.

*/


#ifndef __UART_H__
#define __UART_H__

#define UART_IN 0

//
//					20 Mhz:		16 Mhz:		8Mhz:
//		rate		value		value		value
//		9600:		129			103			51
//		19200:		64			51
//		28800:		42			34
//		38400:		32						12
//		115200:		10
//	
// At Serial Input speed of 115200, time per byte is 86 uSec
//
#if F_CPU==20000000UL
#define UART_115200 	10
#define UART_57600		21
#define UART_38400		32
#define UART_250000		9		// in DoubleBaud mode
#elif F_CPU==8000000UL
#define UART_38400		12
#elif F_CPU==1000000UL
#define UART_4800		12
#elif
#elif
error: F_CPU undefined
#endif

#define	UART_SPEED		UART_115200		//with larger RxBuffer or USB

/* Global functions */
extern void UART_SendByte       (uint8 Data);
extern void UART_PrintfProgStr  (prog_char* pBuf);
extern void UART_PrintfStr		(char* pBuf);
extern void UART_PrintfProgStr_I(prog_char* pBuf);
extern void UART_PrintfEndOfLine(void);
extern void UART_Printfuint8      (uint8 Data);
extern void UART_Printfint(int d);
extern void UART_Printfuint(uint Data);
extern void UART_Printfu16      (uint Data);
extern void UART_Printfu32      (ulong Data);
extern void UART_Init           (void);
extern void EnableUartIn(void);
extern void DisableUartIn(void);
extern void UART_Puts  		(uint8* pBuf);
extern void UART_Putsln		(uint8* pBuf);
extern unsigned char UART_Is_Ready();

#if UART_IN
extern unsigned char UART_HasChar(void);
extern uint8  UART_ReceiveByte    (void);
#endif


/* Macros */
#define PRINT(string) (UART_PrintfProgStr(PSTR(string)))
#define EOL           UART_PrintfEndOfLine
#endif
