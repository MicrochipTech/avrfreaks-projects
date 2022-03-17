/****************************************************************************
  ������� ���������� ������� ������ ������ � ������ ������ Ethernet
  � ������ �������� ������ � ����, ��� ��������� ���������������� ���� 
  ������� �������� � ��������� ETHtr. 
  ����� ��������� ������ ��� ��� �����, ������ �������� ���������.
  ��� ������ ����� ������� ������ ���������� �������� ������� 
  ��������� ������ � ������ Ethernet.
  ����������� �������� ������������ ���������� ������ ������,
  ��� ���� ���������������� ������� ������ ��������� �������� 
  ��������� ������ Ethernet.
  ������� ���������� ��� 0, ���� ������ �� �������� ������ �� ��������� � - ��� 1 �����
  ����������� ��������� ������ ���������.
 ****************************************************************************/

#include <avr/io.h>
#include "headers/macro.h"
#include "headers/w3100a.h"
#include "headers/defConstant.h"
#include "headers/defStruct.h"

#define MOVE		6
#define FIXPOINT	7
#define COMPL		8

extern struct	TWI_status	TWI_stat;	//��������� ��������� TWI ����������
extern struct Descript	ETH_dsc;	//������� ��������� ������� ������������ ������ Ethernet
extern struct Request		ETHtr;		//��������� ��� ������� �������� ������ ������ � ������ Ethernet

extern unsigned char ethTRbuff[ETH_TR_SIZE];	// 32 ����� ��� ���������� ������ ������� ���������� ����������� ��� �������� �� ethernet
extern unsigned char time_out[NT];	// ������ ���������� ��� ����������� ��������

unsigned char sendWork(void)
{
	static unsigned char	phase;		// ���� ������ �������
	static unsigned long	w,r,pr32;	//��������� ������ � ������ WIZnet
	unsigned int	pr16;	
	unsigned char	j,result;
	int mw,mr,adif,free;
	
	result=0;
	switch(phase)
	{
		/* ��������� ���������� ������ ������� ��������� ����������� */
		case 0:
		if(ETHtr.req == 0) return(1);				// ������� �� ���� ������, ���� ��� ������� ��������.
		TWI_stat.TWI_state=TWI_BUSY;				// ������ ���������� TWI.												 
		pr16=STW_PR;								// ��������� ������ ������ � ������ ��������.
		TWI_stat.TWI_msgSize=3;						// leng message
		TWI_stat.TWI_buf[0]=SLA_W;					// �������+����� �������.
		TWI_stat.TWI_buf[1]=pr16>>8;				// high adress STW_PR
		TWI_stat.TWI_buf[2]=pr16;					// low adress STW_PR=0x1F0
		phase++;
		TWI_Start();								// ������ ��������� ����������� ���������� TWI.
		break;
		/* ������ 2� ������� ��������� (�������������� �� �����) */
		case 1:
		TWI_stat.TWI_state=TWI_BUSY;												 
		TWI_stat.TWI_msgSize=3;						// leng message
		TWI_stat.TWI_buf[0]=SLA_R;					// �������+����� �������.
		phase++;									// ������ ��������� ����������� ���������� TWI.
		TWI_Start();								// ������ ��������� ����������� ���������� TWI.
		break;
		/* ������ �������� ��� � ������������ */
		case 2:									// �������� ����� ������ ������� ���������
		phase++;
		break;
		/* ��������� ���������� ������ ������� ��������� ����������� */
		case 3:
		TWI_stat.TWI_state=TWI_BUSY;												 
		TWI_stat.TWI_msgSize=3;						// leng message
		TWI_stat.TWI_buf[0]=SLA_W;					// �������+����� �������.
		TWI_stat.TWI_buf[1]=0;						// high adress w
		TWI_stat.TWI_buf[2]=TW_PR;					// low adress TW_PR=0x40
		phase++;
		TWI_Start();								// ������ ��������� ����������� ���������� TWI.		
		break;
		/* ������ 2� ���������-���������� (4� �������) */		
		case 4:
		TWI_stat.TWI_state=TWI_BUSY;												 
		TWI_stat.TWI_msgSize=9;						// leng message
		TWI_stat.TWI_buf[0]=SLA_R;					// �������+����� �������.
		phase++;
		TWI_Start();								// ������ ��������� ����������� ���������� TWI.	
		break;
		/* ����������� ���������� ������������ � ������ ������ */		
		case 5:
		w=(unsigned long)TWI_stat.TWI_buf[4];
		pr32=(unsigned long)TWI_stat.TWI_buf[3];
		w |= (pr32<<8);
		pr32=(unsigned long)TWI_stat.TWI_buf[2];
		w |= (pr32<<16);
		pr32=(unsigned long)TWI_stat.TWI_buf[1];
		w |= (pr32<<24);
		
		r=(unsigned long)TWI_stat.TWI_buf[8];
		pr32=(unsigned long)TWI_stat.TWI_buf[7];
		r |= (pr32<<8);
		pr32=(unsigned long)TWI_stat.TWI_buf[6];
		r |= (pr32<<16);
		pr32=(unsigned long)TWI_stat.TWI_buf[5];
		r |= (pr32<<24);
		
		mw = (int)(w & PMASK);
		mr = (int)(r & PMASK);
		adif = mw-mr;
		if(adif >= 0) free = (int)PMASK - adif;
		else free = -adif;							//������ ��������� ������
		if(free < ETHtr.leng)						//������ ��������� ��������� ������ ��������� ������ ������
		{
			/* �������=1 �� ������ */
			result = 1;
			phase = 0; 								//������ �� �������� ���������.
			ETH_dsc.countError++;					//��������� �������� ������.
			break;
		}
		case MOVE:
		if(ETHtr.req == 0) return(1);				// ������� �� ���� ������, ���� ��� ������� ��������.
		pr32=SEND_DATA_BUF+(w & PMASK);			// Pointer to W3100A send buffer
		w += (unsigned long)ETHtr.leng;			// ����������������� ������� ���������
		/* �������� �� ������������� �������� ���������� ������ � ������ ����������� */
		TWI_stat.TWI_state=TWI_BUSY;
		pr16=(unsigned int)pr32;
		TWI_stat.TWI_msgSize=ETHtr.leng+3;			// leng message
		TWI_stat.TWI_buf[0]=SLA_W;
		TWI_stat.TWI_buf[1]=(pr16>>8);				// high adress
		TWI_stat.TWI_buf[2]=pr16;					// low adress
		for(j=0; j < ETHtr.leng; j++)
		{
			TWI_stat.TWI_buf[j+3]=ethTRbuff[j];	// ����������� ������ ��� ��������
		}
		phase=FIXPOINT;								// ���������� ��������
		ETHtr.req=0;								// ����� ������� ��������
		TWI_Start();								// ������ ��������� ��������� TWI ����������
		break;
		/* ����������� ��������� */		
		case FIXPOINT:
		TWI_stat.TWI_state=TWI_BUSY;
		TWI_stat.TWI_msgSize=7;						// leng message
		TWI_stat.TWI_buf[0]=SLA_W;
		TWI_stat.TWI_buf[1]=0;						// high adress
		TWI_stat.TWI_buf[2]=TW_PR;					// low adress TW_PR
		TWI_stat.TWI_buf[3]=(w >> 24);
		TWI_stat.TWI_buf[4]=(w >> 16);
		TWI_stat.TWI_buf[5]=(w >> 8);
		TWI_stat.TWI_buf[6]=(w);
		phase=COMPL;								// ������� ������� �������� ������
		TWI_Start();								// ������ ��������� ��������� TWI ����������
		break;
		/* �������� ������� ������ */
		case COMPL:
		TWI_stat.TWI_state=TWI_BUSY;												 
		TWI_stat.TWI_msgSize=4;						// leng message
		TWI_stat.TWI_buf[0]=SLA_W;
		TWI_stat.TWI_buf[1]=0;						// high adress C0_CR
		TWI_stat.TWI_buf[2]=0;						// low adress C0_CR=0x00
		TWI_stat.TWI_buf[3]=CSEND;
		phase=0;
		result = 1;
		TWI_Start();								// ������ ��������� ��������� TWI ����������
		break;
		/* ����� ������� ��������, �������=1 �� �����-�� ������ */
		default:
		ETHtr.req=0;
		phase=0;
		result = 1;
		break;
	}
	return(result);								// ������� 1 ��������� ������� � ����������� ������ ������ �� ������ Ethernet
}

