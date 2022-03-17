/****************************************************************************
  ��������� ������ � ������� ethernet.
  ������� �������� ������ ������������� ������ Ethernet � ����� �����
  ���������� ���������������� ������ ������� ������ ������� ��������� �� ������
  � ������� ������ ������ ������ �� ��������, ���� ���������� ������� �������.
  time_out[3] - ������������ ��� ����������� ��������� ��������� TWI ����������.
  ���� ���������� ���������� ���������� TWI ���������� ��� ������, �� 
  ���������������� ������� ������ ��������� ��������� ������.
****************************************************************************/
#include <avr/io.h>
#include "headers/defConstant.h"
#include "headers/defStruct.h"
#include "headers/w3100a.h"
#include "headers/MACRO.H"
/****************************************************************************
 ����������� ��������� ��� ������ � ������� Ethernet
****************************************************************************/
#define ETH_RECEIVE			5			// ���� ������ � ������� Ethernet (������ �� ������)	
#define ETH_TRANSMIT		6			// ���� ������ � ������� Ethernet (������ � ������) 
/***************************************************************************/
extern struct Descript	ETH_dsc;	//������� ��������� ������� ������������ ������ Ethernet
extern struct Request		ETHtr;		//��������� ��� ������� �������� ������ ������ � ������ Ethernet
extern struct Request		ETHrc;		//��������� ��� ������� ������ ������ ������ �� ������ Ethernet

extern struct	TWI_status	TWI_stat;	//��������� ��������� TWI ����������

extern unsigned char time_out[NT];	// ������ ���������� ��� ����������� ��������
extern unsigned char resetState;		// ��������� ��������� ���� ������ ������ WizNet

void	netInit(void);					// ������� ���������
void	socketInit(void);				// ��������� ������
void	pointInit(void);				// ��������� ���������� ������ ������
void	ethRe(char);					// �������� ��������� ��������� ��������� ���������

unsigned char	sendWork(void);		// �������� ����� ������ (�������-0), ��� ��� �����(�������-1) �� ������ �������� � ������ Ethernet.
unsigned char	recWork(void);			// ��������� ����� ������ (�������-0), ��� ��� �����(�������-1) �� ������ Ethernet � ����� ������.

/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/
void ETH_work(void)
{
	if(time_out[2] != 0) return;			// �������� ��������� ������� �����
	if(resetState == 0)
	{
		resetState=1;
		time_out[2]=12;
		releaseETH();						// ������ ������� ����� � ������
		ETH_dsc.phase=0;					// ������ � ������
	}
	if(TWI_stat.TWI_state == TWI_ERROR)
	{
		ETH_dsc.countError++;				//��������� �������� ������
		TWI_stat.TWI_state=TWI_FREE;
	}
/*-------------------------*/	
	switch(ETH_dsc.phase)
	{
		case 0:							// ��������� �������� ����
		netInit();
		break;
		case 1:							// �������� ���������� �������� ����
		ethRe(SSYS_INIT_OK);
		break;
		case 2:							// ��������� �������� ������
		socketInit();
		break;
		case 3:							// �������� ���������� �������� ������
		ethRe(SSOCK_INIT_OK);
		break;
		case 4:							// ������������� ���������� ������ ������ ethernet
		pointInit();
		break;
		/*������ ������ ���������� �� ������ Ethernet*/
		case ETH_RECEIVE:
		if(recWork()) ETH_dsc.phase=ETH_TRANSMIT;
		break;
		/*������ ������� ���������� � ������ Ethernet*/
		case ETH_TRANSMIT:
		if(sendWork()) ETH_dsc.phase=ETH_RECEIVE;		// ����� ���� �����������.
		break;
		/* �����-�� ����.*/
		default:
		ETH_dsc.phase=ETH_RECEIVE;
		ETH_dsc.countError++;
		break;
	}
}

