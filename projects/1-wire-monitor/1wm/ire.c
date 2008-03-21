//************************************************************************************
//              Реализация базовых I/O функций 1-wire шины
//************************************************************************************
#include <avr/io.h>
#include <util/delay_basic.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include "type.h"
#include "monitor.h"



extern BYTE LineBuf[255];       // bus buffer

BYTE LBInd;                      // position in LineBuf
//BYTE LineIvent;                 // contain was_Reset, was_Presence flags;
#define _WAS_RESET          0
#define _WAS_PRESENCE       1
#define _LI_NO_BIT          2

// register variable
register unsigned char BitNumb asm("r3");
register unsigned char ByteVal asm("r4");
register unsigned char LineIvent asm("r5");


// decoding array
BYTE DeCodeArray[]= {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};



BYTE Hex2Byte(BYTE Lsb, BYTE Msb)
{
BYTE ret_val;

    if(((Msb - 0x30) >= 0) && (Msb - 0x30) <= 9 )
    {
        // имеем число 0..9
        ret_val = (Msb - 0x30);
    }
    else
    {
        // имеем A - F
        ret_val = (Msb - 'A' + 10);
    }

    ret_val <<= 4;

    if(((Lsb - 0x30) >= 0) && (Lsb - 0x30) <= 9 )
    {
        // имеем число 0..9
        ret_val |= (Lsb - 0x30) & 0x0F;
    }
    else
    {
        // имеем A - F
        ret_val |= (Lsb - 'A' + 10) & 0x0F;
    }

return ret_val;

}



//**************** Step 1 ******************
//  interrupt on faling edge (start timer)
//  запуск таймера
//******************************************
ISR(INT0_vect) // External Interrupt Request 0
{
    // запустить таймер на прерывания по таймауту (Reset Puls > 240 мкс)
	TCCR1B = (1<<CS10); // CLK/1
	TCNT1 = 0;
}


//**************** Step 2 ******************
//  interrupt on rising edge (capture timer value for analise)
//  Прерывания по переднему фронту
//  "захват" значения таймера, для анализа 1 или 0
//******************************************
ISR(INT1_vect) // External Interrupt Request 1
{

    // посмотреть значение таймера - по нему определить, был ли 0 или 1
	TCCR1B = 0;
    ByteVal >>= 1;

    if(TSTBIT(LineIvent, _LI_NO_BIT))
    {
        CLRBIT(LineIvent, _LI_NO_BIT);
        return;
    }

    if (TCNT1 > 50*Tpm) // 50мкс // it was presence
    {
        SETBIT(LineIvent, _WAS_PRESENCE);
        TCCR1B = 0;
        TCNT1 = 0;
        BitNumb = 0;
        ByteVal = 0;
        // если это произошло - имеем PRESENS PULS
        LineBuf[LBInd++] = 'P';
        LineBuf[LBInd++] = 'S';
        LineBuf[LBInd++] = ' ';
        return;
    }


	if (TCNT1 > 15*Tpm) // 15мкс * на тиков в мкс // it was 1 or 0
	{
         //CLRBIT(ByteVal, Bitumb);
         //ничего не делаем, в регистре и так ноль...
         // set Null
    }
    else
    {
         // Set One
         SETBIT(ByteVal,7);//SETBIT(ByteVal, (7 - BitNumb));
    }


    BitNumb++;

    if(BitNumb == 8)
    {
        LineBuf[LBInd++] = DeCodeArray[(ByteVal>>4)&0x0F];
        LineBuf[LBInd++] = DeCodeArray[ByteVal&0x0F];
        //LineBuf[LBInd++] = ByteVal;
        LineBuf[LBInd++] = ' ';
        ByteVal = 0;
        BitNumb = 0;
    }
	//
}

//**************** Step 3 ******************
//  Прерывания по истечению таймаута (250 мкс)
// timer overfl - wasRESET puls
//*********************************************
ISR(TIMER1_COMPA_vect)
{
    TCCR1B = 0;
    TCNT1 = 0;
    BitNumb = 0;
    ByteVal = 0;
    // если это произошло - имеем RESET PULS
    LineBuf[LBInd++] = 'R';
    LineBuf[LBInd++] = 'S';
    LineBuf[LBInd++] = ' ';
    SETBIT(LineIvent, _WAS_RESET);//was_reset = TRUE;
    CLRBIT(LineIvent, _WAS_PRESENCE);
    SETBIT(LineIvent, _LI_NO_BIT);
}





//******************************************
// RESET puls
//  Делаем Reset импульс 800 мкс
//  Возвращаем ошибку если был 0 на шине
//******************************************
BYTE TxResetPuls(void)
{
// ногу на выход
SETBIT(DDRC, TMo);
// ставим 0
CLRBIT(PORTC, TMo);
// делаем паузу
_delay_loop_2(130*Tpm); // 500 мкс
// ногу на вход
CLRBIT(DDRC, TMo);
// выходим
return 0;
}

//******************************************
// PRESET PULS
//  Делаем Peesence импульс 20 мкс
//  Возвращаем ошибку если был 0 на шине
//******************************************
BYTE TxPresenPuls(void)
{
// ногу на выход
SETBIT(DDRC, TMo);
// ставим 0
CLRBIT(PORTC, TMo);
// делаем паузу
_delay_loop_1(16*Tpm); // 50 мкс
// ногу на вход
CLRBIT(DDRC, TMo);
// выходим
return 0;
}

//******************************************
//  SEND BYTE (orreceive if 0xFF)
//  Передаем байт данных по 1wire
//******************************************
BYTE TxByte(BYTE Data)
{
BYTE i;
for(i = 0; i < 8; i ++)
{
// сделать строб
    // ногу на выход
SETBIT(DDRC, TMo);
    // ставим 0
CLRBIT(PORTC, TMo);
    // делаем паузу
_delay_loop_1(1*Tpm); // 5 мкс
    // ногу на вход
    if (Data & (1 << i))
    {
        // ногу на вход
        CLRBIT(DDRC, TMo);
    }
    else
    {
        // она и так 0
    }

_delay_loop_1(10*Tpm); // 30 мкс

CLRBIT(DDRC, TMo);

_delay_loop_1(4*Tpm); // 15 мкс
}
return 0;
}




//***************************************
// выполнить последовательность команд
// Exequte user entered string
//***************************************
BYTE Exeq1Wire(BYTE * Buff)
{
BYTE i;

// обнуляем указатель буфера безобразий на шине
LBInd = 0;
// и сам буфер
for(i = 0; i < 255; i++) LineBuf[i] = 0;
// обнуляем переменные шины
BitNumb = 0;
ByteVal = 0;
// тут же начинаем слушать линию (разрешаем прерывания)
GICR = (1<<INT0) | (1 << INT1); // External Interrupt Request 0 Enable

i = 0;
while(Buff[i] != 0)
{
/*
RR - wait reset pulse
RP - wait presens pulse
0x00..0xFF send byte value (0xFF - receive byte)
C8 - send byte = iButton CRC8 previous byte sequenc.*/
_nextTwice2:
if(Buff[i] == 0) return 0;

    // ----------- если передача ----------
    if(Buff[i] == 'T')
    {
        i ++;
        if(Buff[i] == 'R')
        {
            // сформированть ресет
            TxResetPuls();
            _delay_loop_2(50*Tpm); // 100 мкс
            i ++;
            i ++;
            goto _nextTwice2;
        }
        if(Buff[i] == 'P')
        {
            // сформированть присутствие на шине
            TxPresenPuls();
            _delay_loop_1(5*Tpm); // 500 мкс
            i ++;
            i ++;
            goto _nextTwice2;
        }
        // если не R и не P - надо сделать ошибку в позиции i
        return i;
    }

    // ------------ если прием ----------
    if(Buff[i] == 'R')
    {
        BYTE time;
        i ++;
        if(Buff[i] == 'R')
        {
            // дождаться ресета на шине

            CLRBIT(LineIvent, _WAS_RESET);//was_reset = TRUE;

            while(!TSTBIT(LineIvent, _WAS_RESET))//was_reset = TRUE;);
            {
                while(!TSTBIT(PIND,TMi0))
                {
                    //считаем тут время
                    _delay_loop_1(1*Tpm); // 5 мкс
                    time ++;
                    if(time > 100)
                    {
                        SETBIT(LineIvent, _WAS_RESET);//was_reset = TRUE;
                        break;
                    }
                }
                time = 0;
            }

            CLRBIT(LineIvent, _WAS_RESET);//was_reset = TRUE;

            i ++;
            i ++;
            goto _nextTwice2;
        }
        if(Buff[i] == 'P')
        {
            // дождаться присутствия на шине
            //was_presens = FALSE;
            //CLRBIT(LineIvent, _WAS_PRESENCE);//was_reset = TRUE;

            while(!TSTBIT(LineIvent, _WAS_PRESENCE))//was_reset = TRUE;);
            {
                while(!TSTBIT(PIND,TMi0))
                {
                    //считаем тут время
                    _delay_loop_1(1*Tpm); // 5 мкс
                    time ++;
                    if(time > 10)
                    {
                        SETBIT(LineIvent, _WAS_PRESENCE);//was_reset = TRUE;
                        break;
                    }
                }
                time = 0;
            }

            CLRBIT(LineIvent, _WAS_PRESENCE);//was_reset = TRUE;

            i ++;
            i ++;
            goto _nextTwice2;
        }
        return i;
    }
    // ------------ если тупо байты для передачи ----------
    _delay_loop_1(20*Tpm);

    TxByte(Hex2Byte(Buff[i++], Buff[i++]));

    i ++;

    BitNumb = 0;

    }

return 0;
}



