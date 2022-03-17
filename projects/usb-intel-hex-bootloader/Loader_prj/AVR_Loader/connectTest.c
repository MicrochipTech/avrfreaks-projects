#include <avr/io.h>
#include "Define/MACRO.H"
#include "Define/defConstant.h"
#include "Define/defStruct.h"
/****************************************************************/
/*
	�������� ���������� � ����������� �� ���� USB
	��������� ������ ������� ����������
*/
/****************************************************************/
extern struct timer time;				// ��������a ��� ����������� ������� ���������� ������ ������
extern unsigned char time_out[NT];	// ������ ��� ����������� ��������
/****************************************************************/
/****************************************************************/
void resetState(unsigned char s);		//����� ��������� ���� ������ ����������
/****************************************************************/
#define NCONNECT	0
#define CONNECT		1

void connectTest(void)
{
	static unsigned char faseWork=0;
	unsigned char i;
	
	i=getUSBstat();								//������ ��������� ������ �������� ���������� � USB
	
	switch(faseWork)
	{
		/*-----------------------------*/
		case NCONNECT:							//��������� ������������
		if(i & 0x01) time.init = DEL1000;		//���� ����� �������� ����������, �� ��� ����������
		else
		{
			time.init = DEL500;					//��������� ������� �� 0.5�
			faseWork = CONNECT;
			time_out[0]=DEL1000;
			resetState(1);						//������������� ��� ������ ���������� � USB
		}
		break;
		/*-----------------------------*/
		case CONNECT:							//��������� co��������
		if(i & 0x01)							//���� ����� �������� ����������, �� ��� ����������
		{
			time.init = DEL1000;				//��������� ������� �� 1�
			faseWork = NCONNECT;
			resetState(0);						//������ ����� ������ ���������� � USB
		}
		else time.init = DEL500;				//��������� ������� �� 0.5�
		break;
		/*-----------------------------*/
		default:
		time.init = DEL1000;
		faseWork = NCONNECT;
		resetState(0);							//������ ����� ������ ���������� � USB
		break;		
	}
}

