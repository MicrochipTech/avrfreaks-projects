//************************************************************************************
//              ���������� ������� I/O ������� 1-wire ����
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
        // ����� ����� 0..9
        ret_val = (Msb - 0x30);
    }
    else
    {
        // ����� A - F
        ret_val = (Msb - 'A' + 10);
    }

    ret_val <<= 4;

    if(((Lsb - 0x30) >= 0) && (Lsb - 0x30) <= 9 )
    {
        // ����� ����� 0..9
        ret_val |= (Lsb - 0x30) & 0x0F;
    }
    else
    {
        // ����� A - F
        ret_val |= (Lsb - 'A' + 10) & 0x0F;
    }

return ret_val;

}



//**************** Step 1 ******************
//  interrupt on faling edge (start timer)
//  ������ �������
//******************************************
ISR(INT0_vect) // External Interrupt Request 0
{
    // ��������� ������ �� ���������� �� �������� (Reset Puls > 240 ���)
	TCCR1B = (1<<CS10); // CLK/1
	TCNT1 = 0;
}


//**************** Step 2 ******************
//  interrupt on rising edge (capture timer value for analise)
//  ���������� �� ��������� ������
//  "������" �������� �������, ��� ������� 1 ��� 0
//******************************************
ISR(INT1_vect) // External Interrupt Request 1
{

    // ���������� �������� ������� - �� ���� ����������, ��� �� 0 ��� 1
	TCCR1B = 0;
    ByteVal >>= 1;

    if(TSTBIT(LineIvent, _LI_NO_BIT))
    {
        CLRBIT(LineIvent, _LI_NO_BIT);
        return;
    }

    if (TCNT1 > 50*Tpm) // 50��� // it was presence
    {
        SETBIT(LineIvent, _WAS_PRESENCE);
        TCCR1B = 0;
        TCNT1 = 0;
        BitNumb = 0;
        ByteVal = 0;
        // ���� ��� ��������� - ����� PRESENS PULS
        LineBuf[LBInd++] = 'P';
        LineBuf[LBInd++] = 'S';
        LineBuf[LBInd++] = ' ';
        return;
    }


	if (TCNT1 > 15*Tpm) // 15��� * �� ����� � ��� // it was 1 or 0
	{
         //CLRBIT(ByteVal, Bitumb);
         //������ �� ������, � �������� � ��� ����...
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
//  ���������� �� ��������� �������� (250 ���)
// timer overfl - wasRESET puls
//*********************************************
ISR(TIMER1_COMPA_vect)
{
    TCCR1B = 0;
    TCNT1 = 0;
    BitNumb = 0;
    ByteVal = 0;
    // ���� ��� ��������� - ����� RESET PULS
    LineBuf[LBInd++] = 'R';
    LineBuf[LBInd++] = 'S';
    LineBuf[LBInd++] = ' ';
    SETBIT(LineIvent, _WAS_RESET);//was_reset = TRUE;
    CLRBIT(LineIvent, _WAS_PRESENCE);
    SETBIT(LineIvent, _LI_NO_BIT);
}





//******************************************
// RESET puls
//  ������ Reset ������� 800 ���
//  ���������� ������ ���� ��� 0 �� ����
//******************************************
BYTE TxResetPuls(void)
{
// ���� �� �����
SETBIT(DDRC, TMo);
// ������ 0
CLRBIT(PORTC, TMo);
// ������ �����
_delay_loop_2(130*Tpm); // 500 ���
// ���� �� ����
CLRBIT(DDRC, TMo);
// �������
return 0;
}

//******************************************
// PRESET PULS
//  ������ Peesence ������� 20 ���
//  ���������� ������ ���� ��� 0 �� ����
//******************************************
BYTE TxPresenPuls(void)
{
// ���� �� �����
SETBIT(DDRC, TMo);
// ������ 0
CLRBIT(PORTC, TMo);
// ������ �����
_delay_loop_1(16*Tpm); // 50 ���
// ���� �� ����
CLRBIT(DDRC, TMo);
// �������
return 0;
}

//******************************************
//  SEND BYTE (orreceive if 0xFF)
//  �������� ���� ������ �� 1wire
//******************************************
BYTE TxByte(BYTE Data)
{
BYTE i;
for(i = 0; i < 8; i ++)
{
// ������� �����
    // ���� �� �����
SETBIT(DDRC, TMo);
    // ������ 0
CLRBIT(PORTC, TMo);
    // ������ �����
_delay_loop_1(1*Tpm); // 5 ���
    // ���� �� ����
    if (Data & (1 << i))
    {
        // ���� �� ����
        CLRBIT(DDRC, TMo);
    }
    else
    {
        // ��� � ��� 0
    }

_delay_loop_1(10*Tpm); // 30 ���

CLRBIT(DDRC, TMo);

_delay_loop_1(4*Tpm); // 15 ���
}
return 0;
}




//***************************************
// ��������� ������������������ ������
// Exequte user entered string
//***************************************
BYTE Exeq1Wire(BYTE * Buff)
{
BYTE i;

// �������� ��������� ������ ���������� �� ����
LBInd = 0;
// � ��� �����
for(i = 0; i < 255; i++) LineBuf[i] = 0;
// �������� ���������� ����
BitNumb = 0;
ByteVal = 0;
// ��� �� �������� ������� ����� (��������� ����������)
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

    // ----------- ���� �������� ----------
    if(Buff[i] == 'T')
    {
        i ++;
        if(Buff[i] == 'R')
        {
            // ������������� �����
            TxResetPuls();
            _delay_loop_2(50*Tpm); // 100 ���
            i ++;
            i ++;
            goto _nextTwice2;
        }
        if(Buff[i] == 'P')
        {
            // ������������� ����������� �� ����
            TxPresenPuls();
            _delay_loop_1(5*Tpm); // 500 ���
            i ++;
            i ++;
            goto _nextTwice2;
        }
        // ���� �� R � �� P - ���� ������� ������ � ������� i
        return i;
    }

    // ------------ ���� ����� ----------
    if(Buff[i] == 'R')
    {
        BYTE time;
        i ++;
        if(Buff[i] == 'R')
        {
            // ��������� ������ �� ����

            CLRBIT(LineIvent, _WAS_RESET);//was_reset = TRUE;

            while(!TSTBIT(LineIvent, _WAS_RESET))//was_reset = TRUE;);
            {
                while(!TSTBIT(PIND,TMi0))
                {
                    //������� ��� �����
                    _delay_loop_1(1*Tpm); // 5 ���
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
            // ��������� ����������� �� ����
            //was_presens = FALSE;
            //CLRBIT(LineIvent, _WAS_PRESENCE);//was_reset = TRUE;

            while(!TSTBIT(LineIvent, _WAS_PRESENCE))//was_reset = TRUE;);
            {
                while(!TSTBIT(PIND,TMi0))
                {
                    //������� ��� �����
                    _delay_loop_1(1*Tpm); // 5 ���
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
    // ------------ ���� ���� ����� ��� �������� ----------
    _delay_loop_1(20*Tpm);

    TxByte(Hex2Byte(Buff[i++], Buff[i++]));

    i ++;

    BitNumb = 0;

    }

return 0;
}



