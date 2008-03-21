#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "type.h"
#include "printf.h"
#include "monitor.h"


extern BYTE ConsolBuf[255];
h_Serial hSer;
//***********************************************************************
//                         инициализация
//***********************************************************************
void USART0_Init(void)
{
	// USART0 settings: 115200 baud 8-n-1
	UBRRH = 0;
	UBRRL = UBRR_val;
	UCSRB = (1<<RXCIE) | (1<<RXEN) | (1<<TXEN);
	UCSRC = (1<<URSEL) | (1<<UCSZ1) | (1<<UCSZ0);
	//UDR = 0xAB;
}

//***********************************************************************
//                     Прерывание по приему байта
//***********************************************************************
ISR(USART_RXC_vect) // USART, Rx Complete
{
BYTE Data = UDR;

    if (Data == '#')
    {
        BYTE i;
        hSer.Count = 0;
        hSer.Command = _DATA;

        for(i = 0; i < 255; i++) ConsolBuf[i] = 0;

        return;
    }

    if ((Data == 'h') && (hSer.Count == 0))
    {
        hSer.Mode = _OK;
        hSer.Command = _HELP;
        return;
    }

    if ((Data == 'r') && (hSer.Count == 0))
    {
        //hSer.Mode = _OK;
        //hSer.Command = _RESET;
        asm("rjmp 0\n");
    }

    if ((Data == 'S') && (hSer.Count == 0))
    {
        hSer.Command = _SETTING;
        hSer.Count = 1;
        return;
    }


    if (Data == 13)
    {
        hSer.Mode = _OK;
        ConsolBuf[hSer.Count + 1] = 0;
        return;
    }

    // ------ тут приходят данные с консоли, забиваем их в буфер ------
    ConsolBuf[hSer.Count++] = Data;
    //hSer.Command = _DATA;
}

//***********************************************************************
//                   Вывод на экран строки из памяти программ
//***********************************************************************
void m_printf(BYTE * Str)
{
BYTE i = 0;

    i = pgm_read_byte(Str);
	while(i != '\0')
	{
		while ( !( UCSRA & (1<<UDRE)) );
		UDR = i;
		Str++;
        i = pgm_read_byte(Str);
	}
		while ( !( UCSRA & (1<<UDRE)) );
		UDR = 0x0D;
}




//***********************************************************************
//                   Вывод на экран строки из памяти данных
//***********************************************************************
void s_printf(BYTE * Str)
{
BYTE i = 0;

//    while ( !( UCSRA & (1<<UDRE)) );
//		UDR = '>';

	while(Str[i] != '\0')
	{
		while ( !( UCSRA & (1<<UDRE)) );
		UDR = Str[i];
		i++;
	}
		while ( !( UCSRA & (1<<UDRE)) );
		UDR = 0x0D;
}




//***********************************************************************
//              Вывод на экран входного буфера интерфейса
//***********************************************************************
void print_in_buff(BYTE * Str)
{
BYTE i = 0;

	while(Str[i] != '\n')
	{
		while ( !( UCSRA & (1<<UDRE)) );
		UDR = Str[i];
		i++;
	}
}

//***********************************************************************
//                              ToDo
//***********************************************************************
/*












*/




