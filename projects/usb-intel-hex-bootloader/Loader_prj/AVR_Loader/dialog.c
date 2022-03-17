#include <avr/io.h>
#include "Define/MACRO.H"
#include "Define/defConstant.h"
#include "Define/defStruct.h"
/****************************************************************/
/*
	��������� ������� � �����������.
	������ �� ������� "ASK" "GET" ("OK" "LET")
	���� ������� ������������� ����� ����������� ����������� � ���� USB - 
	�� ������ "ASK" ������� ����� "OK".
	����� ��������� ������� "GET" ������� ����� "LET" � ���� ������ �������������.
	����� �������������� ������ ����������� �������� �����.
	������� � ���� ������� ���������� ����� ��������� ����������������
	��� ����� ����� ������������ ������ ����������.
*/
/****************************************************************/
extern struct descWork dw;			// ��������� - ��������� ���� ������ ����������
extern struct Request rc;				//��������� ������� �� ��������� �������� ������ �� USB
extern struct Request tx;				//��������� ������� �� �������� ������ � ������� USB
/****************************************************************/
extern unsigned char rcBuff[USBB];	//����� ���������
extern unsigned char txBuff[USBB];	//����� �����������
/****************************************************************/
void resetState(unsigned char s);

void dialog(void)
{
	if(dw.phase != 1)							//������� �� ������� ���������� ���������� ����
	{
		dw.phDi=0;
		return;
	}
	/*-----------------------------*/
	switch(dw.phDi)
	{
		case 0:	//����� ������� A ��� G
		if(rc.req)
		{
			rc.req=0;
			if(rcBuff[0] == 'A')
			{
				dw.phDi=1;			//������� � ������� ������� ASK
			}
			if(rcBuff[0] == 'G')
			{
				dw.phDi=4;			//������� � ������� ������� GET
			}
		}
		break;
	/*-----------------------------*/	
		case 1:	//����� ������� S
		if(rc.req)
		{
			rc.req=0;
			if(rcBuff[0] == 'S')
			{
				dw.phDi=2;
				break;
			}
			else dw.phDi=0;
		}
		break;
	/*-----------------------------*/
		case 2:	//����� ������� K
		if(rc.req)
		{
			rc.req=0;
			if(rcBuff[0] == 'K')
			{
				dw.phDi=3;
				break;
			}
			else dw.phDi=0;
		}
		break;
	/*-----------------------------*/
		case 3:	//�������� ������ "OK"
		if(tx.req == 0)
		{
			tx.req=1;
			tx.len=2;
			txBuff[0]='O';
			txBuff[1]='K';
			dw.phDi=0;
		}
		break;
	/*-----------------------------*/
		case 4:	//����� ������� E
		if(rc.req)
		{
			rc.req=0;
			if(rcBuff[0] == 'E')
			{
				dw.phDi=5;
				break;
			}
			else dw.phDi=0;
		}
		break;
	/*-----------------------------*/
		case 5:	//����� ������� T
		if(rc.req)
		{
			rc.req=0;
			if(rcBuff[0] == 'T')
			{
				dw.phDi=6;
				break;
			}
			else dw.phDi=0;
		}
		break;
	/*-----------------------------*/
		case 6:	//�������� ������ "LET"
		if(tx.req == 0)
		{
			tx.req=1;
			tx.len=3;
			txBuff[0]='L';
			txBuff[1]='E';
			txBuff[2]='T';
			resetState(2);			//!!!��������� ������ ������ �� ���� ������� � ������� � ������ ����� ��������
		}
		break;
	/*-----------------------------*/
		default:
		dw.phDi=0;
		break;
	}
}

