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

#include <avr\io.h>
#include <avr\interrupt.h>
#include <avr\pgmspace.h>
#include ".\config.h"
#include ".\uart.h"

/* UART output global variables */
volatile uint8   UART_Ready;
         prog_char*  pUART_Buffer;

#if UART_IN
volatile uint8   UART_ReceivedChar;
         uint8   UART_RxChar;
#endif

#define DOUBLEBAUD 0

/* end-of-line string = 'Line End' + 'Line Feed' character */
prog_char UART_pszEndOfLine[3] = {0x0d,0x0a,0};

void UART_Init(void)
{
    UART_Ready        = 1;
    pUART_Buffer      = 0;

#if UART_IN
    UART_ReceivedChar = 0;
#endif
	UBRR0 = UART_SPEED;		//value depends on Clock speed
	UCSR0A = 0;
// enable RxD/TxD and interrupts. PD0 and PD1 disabled 
    UCSR0B = _BV(RXCIE0)|_BV(TXCIE0)|_BV(RXEN0)|_BV(TXEN0);
// set default 8 bits, no parity, 1 stop.
	UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);
#if DOUBLEBAUD
	UCSR0C |= _BV(U2X0);	//Double Speed
#endif
}

void EnableUartIn(void)
{
// enable rx interrupt
	sbi(UCSR0B, RXCIE0);
}

void DisableUartIn(void)
{
// disabe rx interrupt
	cbi(UCSR0B, RXCIE0);
}

/* UART Transmit Complete Interrupt Function */
ISR(USART0_TX_vect)
{
//	ToggleLed();

    /* Test if a string is being sent */
    if (pUART_Buffer!=0)
    {
        /* Go to next character in string */
        pUART_Buffer++;
        /* Test if the end of string has been reached */
        if (pgm_read_byte(pUART_Buffer)==0)
        {
            /* String has been sent */
            pUART_Buffer = 0;
            /* Indicate that the UART is now ready to send */
            UART_Ready   = 1;
            return;
        }
        /* Send next character in string */
        UDR0 = pgm_read_byte(pUART_Buffer);
        return;
    }
    /* Indicate that the UART is now ready to send */
    UART_Ready = 1;
}

void UART_SendByte(uint8 Data)
{
    /* wait for UART to become available */
    while(!UART_Ready);
    UART_Ready = 0;
    /* Send character */
    UDR0 = Data;
}

void UART_PrintfProgStr(prog_char* pBuf)
{
    /* wait for UART to become available */
    while(!UART_Ready);
    UART_Ready = 0;
    /* Indicate to ISR the string to be sent */
    pUART_Buffer = pBuf;
    /* Send first character */
    UDR0 = pgm_read_byte(pUART_Buffer);
}
//
// Immediate version. Should work with interrupts off too,
// provided UART_SendByte() has not been called in isolation.
//
void UART_PrintfProgStr_I(prog_char* pBuf)
{
	uint8 ch;
	while ((ch = pgm_read_byte(pBuf)) != 0) {
		UART_SendByte(ch);
		++pBuf;
		while ((UART_Ready == 0) && ((UCSR0A & _BV(TXC0)) == 0)) ;
		UART_Ready = 1;
		UCSR0A |= _BV(TXC0);
	}
}

void UART_PrintfStr(char *pBuf)
{
	char ch;
	while ((ch = *pBuf++) != 0) {
		UART_SendByte(ch);
	}
}


void UART_PrintfEndOfLine(void)
{
    /* wait for UART to become available */
    while(!UART_Ready);
    UART_Ready = 0;
    /* Indicate to ISR the string to be sent */
    pUART_Buffer = UART_pszEndOfLine;
    /* Send first character */
    UDR0 = pgm_read_byte(pUART_Buffer);
}

void UART_PrintfU4(uint8 Data)
{
    /* Send 4-bit hex value */
    uint8 Character = Data&0x0f;
    if (Character>9)
    {
        Character+='A'-10;
    }
    else
    {
        Character+='0';
    }
    UART_SendByte(Character);
}

void UART_Printfuint8(uint8 Data)
{
    /* Send 8-bit hex value */
    UART_PrintfU4(Data>>4);
    UART_PrintfU4(Data);
}

void UART_Printfu16(uint Data)
{
    /* Send 16-bit hex value */
    UART_Printfuint8(Data>>8);
    UART_Printfuint8(Data);
}

void UART_Printfuint(uint Data)
{
    /* Send 16-bit hex value */
    UART_Printfuint8(Data>>8);
    UART_Printfuint8(Data);
}

void UART_Printfint(int d)
{
    if (d < 0)
    {
        UART_SendByte('-');
        d = -d;
    }
    
    uint8 buf[16];
    uint8 i = 0;
    
    while (d > 0) {
        buf[i++] = d % 10;
        d /= 10;
    }
    while (i)
        UART_SendByte(buf[--i]);
}

void UART_Printfulong(ulong Data)
{
    /* Send 32-bit hex value */
    UART_Printfuint(Data>>16);
    UART_Printfuint(Data);
}
//
// non-interrupt driven input
//
#if UART_IN
uint8 UART_ReceiveByte(void)
{
    /* wait for UART to indicate that a character has been received */
    while(!UART_ReceivedChar);
    UART_ReceivedChar = 0;
    /* read byte from UART data buffer */
    return UART_RxChar;
}

uint8 UART_HasChar(void)
{
	return UART_ReceivedChar;
}

/* UART Receive Complete Interrupt Function */
ISR(USART_RX_vect)      
{
    /* Indicate that the UART has received a character */
    UART_ReceivedChar = 1;
    /* Store received character */
    UART_RxChar = inp(UDR0);
}
#endif

void UART_Puts(uint8* pBuf)
{
	while (*pBuf)
		UART_SendByte(*pBuf++);
}

void UART_Putsln(uint8* pBuf)
{
	UART_Puts(pBuf);
	UART_PrintfEndOfLine();
}

unsigned char UART_Is_Ready()
{
	return UART_Ready;
}
