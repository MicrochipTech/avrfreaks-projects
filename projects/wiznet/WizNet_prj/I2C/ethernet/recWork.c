/*
	������� ��� ����������� � ������� ������ UDP ������ �� ������ Ethernet
	�������� ������ ���������� ������ ��� ������� ������ ����������� �������
	�� ��������� ������ �� ���� �������.
	��������� ����������� ������ ���������� ������� ���������.
	��������� ������� ��������� �������� ������� ������� � ������� ���� � ������ ������.
	������� ���������� ��� 0, ���� ������ �� ������ ������ �� ��������� � - ��� 1 �����
	����������� ��������� ������ ���������.
*/
#include <avr/io.h>
#include "headers/MACRO.h"
#include "headers/w3100a.h"
#include "headers/defConstant.h"
#include "headers/defStruct.h"

extern const unsigned char Socket_Settings[19];	// ������ ��������� ������, ����� ����� ��� ��������� IP and Port num �������� ������.

extern struct	TWI_status	TWI_stat;	//��������� ��������� TWI ����������
extern struct Descript	ETH_dsc;	//������� ��������� ������� ������������ ������ Ethernet
extern struct Request		ETHrc;		//��������� ��� ������� ��������� ������ �������� �� ������ Ethernet

extern unsigned char	ethRCbuff[ETH_RC_SIZE];	// 40 ����� ��� ���������� ������ ������� ������ �� ���� ethernet
extern unsigned char time_out[NT];	// ������ ���������� ��� ����������� ��������

#define HEADLEN	8						// ����� ��������� ��������� UDP ������
/*	����������� ��������� ���� ������ ��������� */
#define BREAD	13
#define CREAD	14
#define PMODIFY	15
#define FIXREAD	16

unsigned char recWork(void)
{
	static unsigned char	phase=0;
	static unsigned char temp_size;
	static unsigned char error_size;
	static int	lengUDP=0;				// ����� ���������� � ������ ����� UDP ������.	
	static unsigned long	w,r,pr32;	// 4� �������� ��������� ��������� ������ ������ ������ Ethernet.
	
	unsigned int	pr16;	
	unsigned char	i;
	unsigned char	result;
	int mw,mr,adif,busy;
	
	result = 0;
	switch(phase)
	{
		/* ��������� ���������� ������ �������� �������� SRW_PR ��������� */
		case 0:
		if(ETHrc.req) return(1);						// ������� �� ���� ������, ���� �� ��������� ���������� ������.
		TWI_stat.TWI_state=TWI_BUSY;					// ������ ���������� TWI.
		pr16=SRW_PR;									// ��������� ����� ������ � ������ ������.
		TWI_stat.TWI_msgSize=3;							// leng message
		TWI_stat.TWI_buf[0]=SLA_W;						// �������+����� �������.
		TWI_stat.TWI_buf[1]=pr16>>8;					// high adress SRW_PR
		TWI_stat.TWI_buf[2]=pr16;						// low adress SRW_PR=0x1E0
		phase++;
		TWI_Start();									// ������ ��������� ����������� ���������� TWI.
		break;
		/* ������ �������� �������� (�������������� �� �����) */
		case 1:
		TWI_stat.TWI_state=TWI_BUSY;					// ������ ���������� TWI.												 
		TWI_stat.TWI_msgSize=2;							// leng message
		TWI_stat.TWI_buf[0]=SLA_R;						// �������+����� �������.
		phase++;
		TWI_Start();									// ������ ��������� ����������� ���������� TWI.
		break;
		/* ������ �������� ��� � ������������ */
		case 2:										// �������� ����� ������ ������� ���������
		phase++;
		break;
		/* ��������� ���������� ������ �������� �������� ��������� */
		case 3:
		TWI_stat.TWI_state=TWI_BUSY;					// ������ ���������� TWI.												 
		TWI_stat.TWI_msgSize=3;							// leng message
		TWI_stat.TWI_buf[0]=SLA_W;						// �������+����� �������.
		TWI_stat.TWI_buf[1]=0;							// high adress RW_PR
		TWI_stat.TWI_buf[2]=RW_PR;						// low adress RW_PR=0x10
		phase++;
		TWI_Start();									// ������ ��������� ����������� ���������� TWI.
		break;
		/* ������ ��������-��������� (4� ��������) */
		case 4:
		TWI_stat.TWI_state=TWI_BUSY;					// ������ ���������� TWI.												 
		TWI_stat.TWI_msgSize=5;							// leng message
		TWI_stat.TWI_buf[0]=SLA_R;						// �������+����� �������.
		phase++;
		TWI_Start();									// ������ ��������� ����������� ���������� TWI.
		break;
		/* ������������ ��������-��������� */
		case 5:
		w = (unsigned long)TWI_stat.TWI_buf[4];
		pr32=(unsigned long)TWI_stat.TWI_buf[3];
		w |= (pr32<<8);
		pr32=(unsigned long)TWI_stat.TWI_buf[2];
		w |= (pr32<<16);
		pr32=(unsigned long)TWI_stat.TWI_buf[1];
		w |= (pr32<<24);
		TWI_stat.TWI_state=TWI_BUSY;
		/* ��������� ���������� �������� �������� ��������� */
		pr16=SRR_PR;
		TWI_stat.TWI_msgSize=3;							// leng message
		TWI_stat.TWI_buf[0]=SLA_W;						// �������+����� �������.
		TWI_stat.TWI_buf[1]=pr16>>8;					// high adress SRW_PR
		TWI_stat.TWI_buf[2]=pr16;						// low adress SRR_PR=0x1E1
		phase++;
		TWI_Start();									// ������ ��������� ����������� ���������� TWI.
		break;
		/* ������ �������� �������� (�������������� �� �����) */
		case 6:
		TWI_stat.TWI_state=TWI_BUSY;												 
		TWI_stat.TWI_msgSize=2;							// leng message
		TWI_stat.TWI_buf[0]=SLA_R;						// �������+����� �������.
		phase++;
		TWI_Start();									// ������ ��������� ����������� ���������� TWI.		
		break;
		/* ������ �������� ��� � ������������ */
		case 7:										// �������� ����� ������ ������� ���������
		phase++;
		break;
		case 8:
		TWI_stat.TWI_state=TWI_BUSY;												 
		/* ��������� ���������� ������ �������� �������� ��������� */
		TWI_stat.TWI_msgSize=3;							// leng message
		TWI_stat.TWI_buf[0]=SLA_W;						// �������+����� �������.
		TWI_stat.TWI_buf[1]=0;							// high adress RW_PR
		TWI_stat.TWI_buf[2]=RR_PR;						// low adress RW_PR=0x10
		phase++;
		TWI_Start();									// ������ ��������� ����������� ���������� TWI.		
		break;
		/* ������ ������� ��������-��������� (4� ��������) */
		case 9:
		TWI_stat.TWI_state=TWI_BUSY;												 
		TWI_stat.TWI_msgSize=5;							// leng message
		TWI_stat.TWI_buf[0]=SLA_R;						// �������+����� �������.
		phase++;
		TWI_Start();									// ������ ��������� ����������� ���������� TWI.
		break;
		/* ����������� ������� ��������� � ������ ������ */
		case 10:
		r = (unsigned long)TWI_stat.TWI_buf[4];
		pr32=(unsigned long)TWI_stat.TWI_buf[3];
		r |= (pr32<<8);
		pr32=(unsigned long)TWI_stat.TWI_buf[2];
		r |= (pr32<<16);
		pr32=(unsigned long)TWI_stat.TWI_buf[1];
		r |= (pr32<<24);
		
		pr32=RECV_DATA_BUF+(r & PMASK);				// Pointer to W3100A receive buffer
		
		mw = (int)(w & PMASK);
		mr = (int)(r & PMASK);
		adif = mw-mr;
		if(adif >= 0) busy = adif;
		else busy = FULLSIZE + adif;					//������ ������� ���������� ������
		
		if(busy == 0)									//��� ��������� � �������� ������ ������ WIZnet
		{
			result = 1;									//����� ������, ������ ���.
			phase = 0;
			break;
		}
		lengUDP = busy;
		if(busy > ETH_RC_SIZE) error_size=1;			// �������� ������ ����� �������� ������
		else error_size=0;
		/* ���������� � ������ ������ */
		pr32=RECV_DATA_BUF+(r & PMASK);				// Pointer to W3100A receive buffer
		TWI_stat.TWI_state=TWI_BUSY;
		pr16=(unsigned int)pr32;
		TWI_stat.TWI_msgSize=3;							// leng message
		TWI_stat.TWI_buf[0]=SLA_W;
		TWI_stat.TWI_buf[1]=(pr16>>8);					// high adress
		TWI_stat.TWI_buf[2]=pr16;						// low adress
		phase++;
		TWI_Start();
		break;
		/* ������ ������ ������ */
		case 11:
		if(error_size == 0) i=(unsigned char)lengUDP;
		else i=ETH_RC_SIZE;
		TWI_stat.TWI_state=TWI_BUSY;
		TWI_stat.TWI_msgSize=i+1;						// leng message
		TWI_stat.TWI_buf[0]=SLA_R;						// �������+����� �������.
		r += i;											// ����������� ��������� ������ ������ ������
		phase++;
		TWI_Start();
		break;
		/* ���������� ��������� �������� ������ */
		case 12:
		if(error_size == 0)							// ����� ���������� �����
		{
			for(i=0; i<lengUDP; i++)					// ����������� ����������� ���������
			{
				ethRCbuff[i]=TWI_stat.TWI_buf[i+1];
			}
			ETHrc.leng=(unsigned char)lengUDP;
			ETHrc.req=1;
			phase=PMODIFY;
			break;
		}
		else lengUDP -= ETH_RC_SIZE;
		phase=BREAD;
		break;
		/* ����������� �������� �� ������ ����� �������� ������ */		
		case BREAD:
		pr32=RECV_DATA_BUF+(r & PMASK);			// Pointer to W3100A receive buffer
		TWI_stat.TWI_state=TWI_BUSY;
		pr16=(unsigned int)pr32;
		if(lengUDP <= ETH_RC_SIZE)					// ���� ����� ��������� � �����
		{
			r += lengUDP;							// ����������������� ������� ���������
			temp_size=(unsigned char)lengUDP;
			lengUDP=0;								// ������� �������
		}
		else
		{
			r+=ETH_RC_SIZE;							// ����������������� ������� ���������
			temp_size=ETH_RC_SIZE;
			lengUDP-=ETH_RC_SIZE;					// �������� �������
		}
		/* ��������� ������ ������ ������ */
		TWI_stat.TWI_msgSize=3;						// leng message
		TWI_stat.TWI_buf[0]=SLA_W;					// �������+����� �������.
		TWI_stat.TWI_buf[1]=(pr16>>8);				// high adress
		TWI_stat.TWI_buf[2]=pr16;					// low adress
		phase=CREAD;
		TWI_Start();
		break;
		/* ����������� ������ ��� ��� ����� */
		case CREAD:
		if(lengUDP) phase=BREAD;
		else phase=PMODIFY;
		TWI_stat.TWI_state=TWI_BUSY;
		TWI_stat.TWI_msgSize=temp_size+1;			// leng message
		TWI_stat.TWI_buf[0]=SLA_R;					// �������+����� �������.
		TWI_Start();
		break;
		/* ����������� ��������� */
		case PMODIFY:
		TWI_stat.TWI_state=TWI_BUSY;
		TWI_stat.TWI_msgSize=7;						// leng message
		TWI_stat.TWI_buf[0]=SLA_W;					// �������+����� �������.
		TWI_stat.TWI_buf[1]=0;						// high adress
		TWI_stat.TWI_buf[2]=RR_PR;					// low adress
		TWI_stat.TWI_buf[3]=(r>>24);
		TWI_stat.TWI_buf[4]=(r>>16);
		TWI_stat.TWI_buf[5]=(r>>8);
		TWI_stat.TWI_buf[6]=(r);
		phase=FIXREAD;
		TWI_Start();
		break;
		/* ������ ������� ������ ������ */
		case FIXREAD:
		TWI_stat.TWI_state=TWI_BUSY;												 
		TWI_stat.TWI_msgSize=4;						// leng message
		TWI_stat.TWI_buf[0]=SLA_W;	
		TWI_stat.TWI_buf[1]=0;						// high adress C0_CR
		TWI_stat.TWI_buf[2]=0;						// low adress C0_CR=0x00
		TWI_stat.TWI_buf[3]=CRECV;					// ������� ���������� ������ ������
		result = 1;
		time_out[1] = MC33;							// init
		RED_ON();									// �������� ��������� ��������� ������
		phase=0;
		TWI_Start();
		break;
		
		default:
		phase=0;
		result = 1;
		break;
	}
	return(result);
}

