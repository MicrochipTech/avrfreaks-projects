#include <avr/io.h>
#include <util/delay_basic.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include "type.h"
#include "monitor.h"
#include "printf.h"
#include "1wire.h"

const BYTE PROGMEM String_reset[] = {"Will Reset...\0\r"};
const BYTE PROGMEM String_0[] = {"Start 1-wire monitor. ver 1.1 gHelius@gmail.com\rfor help type #h\0"};
const BYTE PROGMEM String_1[] = {"# - start record\0"};
const BYTE PROGMEM String_2[] = {"TR - transmit reset pulse\0"};
const BYTE PROGMEM String_3[] = {"RR - wait reset pulse\0"};
const BYTE PROGMEM String_4[] = {"TP - transmit presens pulse\0"};
const BYTE PROGMEM String_5[] = {"RP - wait presens pulse\0"};
//const BYTE PROGMEM String_6[] = {"C8 - send byte = iButton CRC8 previous byte sequence\0"};
//const BYTE PROGMEM String_7[] = {"RXNb (Nb - 0x00..0xFF) - number of receive byte\0"};
const BYTE PROGMEM String_8[] = {"0x00..0xFF set byte value to transmit (0xFF - also receive)\0"};
const BYTE PROGMEM String_9[] = {"press Enter - execute record\r\0"};

//const BYTE PROGMEM String_10[] = {"Setting:\0"};

//const BYTE PROGMEM String_11[] = {"#S [ par_string ] /enter/ - setting\r\0"};

const BYTE PROGMEM String_13[] = {"example:\0"};
const BYTE PROGMEM String_14[] = {"TR - transmit reset pulse\0"};
const BYTE PROGMEM String_15[] = {"#TR RP 33 FF FF FF FF FF FF FF FF  /enter/ - Touch Memory\0"};
const BYTE PROGMEM String_16[] = {"#RR TP R1 01 02 03 04 05 06 07 CRC /enter/ - as Touch Memory\0"};
const BYTE PROGMEM String_17[] = {"#h - this help list\0"};
const BYTE PROGMEM String_18[] = {"#r - reset device (monitor, not device on 1-Wire)\r\0"};

const BYTE PROGMEM String_rm[] = {"switch to receive mode\r\0"};
const BYTE PROGMEM String_sm[] = {"switch to single mode\r\0"};

//const BYTE PROGMEM String_test[] = {"\r\0"};
const BYTE PROGMEM String_err[] = {"unknown parametrs or value, type #h for help\r\0"};




BYTE ConsolBuf[255];
BYTE LineBuf[255];

BYTE Mode;
    #define _SINGLE_MODE    0
    #define _RECEIVE_MODE   1

extern h_Serial hSer;


void wait_05s(void)
{
WORD A = 64000;

    while(A)
    {
        A--;
        asm("nop\n nop\n nop\n nop\n nop\n");
    }

}


//*******************************************
// main function
//*******************************************
int main(void)
{
//------------------ init -----------------------
	GICR = 0;//GICR = (1<<INT0) | (1 << INT1); // External Interrupt Request 0 Enable

	SETBIT(PORTB, LED);
	SETBIT(DDRB, LED);

	CLRBIT(PORTD, TMi0);
	CLRBIT(PORTD, TMi1);
	CLRBIT(DDRD, TMi0);
	CLRBIT(DDRD, TMi1);

	wait_05s();

	// настройка прерывания setting interrupt
	MCUCR = (1<<ISC01) | (1<<ISC11) | (1<<ISC10); // The falling edge of INT0 generates an interrupt request

	// setting timer// настройка таймера который буду запускать в прерывании по заднему фронту для подсчета времени
    TIMSK = (1 << OCIE1A) | (1 << WGM12); // Timer/Counter1 Overflow Interrupt Enable 1 tick - 1 mcu clock
    OCR1A = 3000;  // - for reset detecting


	USART0_Init(); // инициализация usart для printf

    m_printf(String_0);

	CLRBIT(PORTB, LED);

	sei();


//--------------- body -------------------------
while(1)
{
BYTE ConsolBufErrCode = 0;
    if(hSer.Mode == _OK)
    {
        // ------- help message ------
        if(hSer.Command == _HELP)
        {
            m_printf(String_1);
            m_printf(String_2);
            m_printf(String_3);
            m_printf(String_4);
            m_printf(String_5);
            //m_printf(String_6);
            //m_printf(String_7);
            m_printf(String_8);
            m_printf(String_9);
            //m_printf(String_10);
            //m_printf(String_11);
            m_printf(String_13);
            m_printf(String_14);
            m_printf(String_15);
            m_printf(String_16);
            m_printf(String_17);
            m_printf(String_18);

            hSer.Command = _DATA;
        }
        // ------- reboot ------
//        if(hSer.Command == _RESET)
//        {
//            m_printf(String_reset);
//            asm("rjmp 0\n");
//        }
        // ---------- Setting --------
        if(hSer.Command == _SETTING)
        {
            switch(ConsolBuf[1])
            {
            case 'r':
                Mode = _RECEIVE_MODE;
                m_printf(String_rm);
            break;
            case 's':
                Mode = _SINGLE_MODE;
                m_printf(String_sm);
            break;
            default:
            m_printf(String_err);
            break;
            }

        }
        // ------- Enter data ------
        if(hSer.Command == _DATA)
        {
        // тут выполняем скрипт из консоли
        while ( !( UCSRA & (1<<UDRE)) );
		UDR = '>';
        s_printf(ConsolBuf);
        ConsolBufErrCode = Exeq1Wire(ConsolBuf);
        // запрещаем прерывания на ногах
        // GICR = 0;

            if(ConsolBufErrCode != 0)
            {
                m_printf(String_err);
            }
            else
            {
            while ( !( UCSRA & (1<<UDRE)) );
            UDR = '<';
            s_printf(LineBuf);
            }
        }


    hSer.Mode = _Rx;
    }

    // add your code here ;)

}
return 0;
}

