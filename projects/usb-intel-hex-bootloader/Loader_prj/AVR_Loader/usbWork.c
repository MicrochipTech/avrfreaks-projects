#include <avr/io.h>
#include <avr/wdt.h>
#include "Define/MACRO.H"
#include "Define/defConstant.h"
#include "Define/defStruct.h"
/****************************************************************/
/*
	����� ��������� �� USB ���� � ����� ���������
	� �������� ��������� �� ������ ����������� � �/� USB ��������
	�������� �� ��������� time-out.
*/
/****************************************************************/
extern struct descWork dw;			// ��������� - ��������� ���� ������ ����������
extern struct Request rc;				//��������� ������� �� ��������� �������� ������ �� USB
extern struct Request tx;				//��������� ������� �� �������� ������ � ������� USB
/****************************************************************/
extern unsigned char rcBuff[USBB];	//����� ���������
extern unsigned char txBuff[USBB];	//����� �����������
extern unsigned char time_out[NT];	// ������ ��� ����������� ��������
/****************************************************************/
void resetState(unsigned char s);
void ansFault(unsigned char s);
/****************************************************************/
#define RCMASK	0x02
#define TXMASK	0x04

void usbWork(void)
{
	unsigned char stat;
	unsigned char i;

	if(dw.phase == 0)							//������� �� ������� ����������� �� USB
	{
		rc.req=0;								//����� ��������
		tx.req=0;								//����� ��������
		if(time_out[0] == 0)					//����� ����� ������� �������� ���������� � USB, ����� � ���������� ������������
		{
			wdt_disable();
			release();							// ����� � ���������������� ���������
		}
		return;
	}
	/**/
	if(time_out[0] == 0)
	{
		time_out[0]=DEL1000;
		resetState(1);							//������� � ���� ���������� ������� �� ����-����
		ansFault(2);							//��������� �� ������ ������� ��������
		return;	
	}
	/**/
	stat=getUSBstat();							//������ ����� ��������� -TXF -RXF
	/*-----------------------------*/
	i=stat & RCMASK;
	/* ��������� ����� 2� ������� - ���������� ��������� � ��� ������� �� ��������� ������ ������ */
	
	if((i | rc.req) == 0)
	{
		rcBuff[0]=getUSBdata();				//����������� ������
		rc.req=1;								//��������� ������� ���������
		rc.len=1;								//��������� ����� ������ �� 1 ����
		time_out[0]=DEL2000;					//������������� ������� ��������
	}
	/*-----------------------------*/
	i=stat & TXMASK;
	/* ��������� ����� 2� ������� - ���������� ����������� � ���� ������ �� �������� ������ */
	if((!i) && (tx.req))
	{
		sendUSBdata(txBuff[0]);
		
		if(--tx.len)
		{
			unsigned char n;
			for(n=0; n<tx.len; n++)
			{
				txBuff[n]=txBuff[n+1];
			}
		}
		else tx.req=0;							//����� ������� ��������
		time_out[0]=DEL2000;					//������������� ������� ��������
	}
}

