#include <avr/io.h>
#include <avr/interrupt.h>            
#include "headers/defConstant.h"
#include "headers/defStruct.h"
/****************************************************************************
	������������ ���������� ������������ �������� �������
	������������ ���������� �� 1024. ������ ������� = 236
	���� ������� ���������� - 25.04 �� (40 �� ������)
****************************************************************************/
#define TIMER0START	22

extern unsigned char time_out[NT];				// ������ ���������� ��� ����������� ��������

SIGNAL(SIG_OVERFLOW0)
{
	unsigned char i;
	
	TCNT0 = TIMER0START;
	for(i=0; i < NT; i++)
	{
		if(time_out[i] > 0) time_out[i]--;		// ��������� ���������� �� �������
	}
}
