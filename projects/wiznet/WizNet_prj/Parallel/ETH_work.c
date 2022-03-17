/********************************************************************************************
  ��������� ������ � ������� ethernet
  ������� �������� ������ ������������� ������ Ethernet � ����� �����
  ���������� ���������������� ������ ������� ������ ������� ��������� �� ������
  � ������ ������ ������ �� ��������, ���� ������� ����������.
  ����������� �������� ������������ ���������������� ������ Ethernet, 
  ������� ������ ������� �� ��������������� ��������� ����� �������������.
  ���� � ������� �������� �������� ������������� ���� ������,
  �� ����� ������ � ��������� ������������� ����� ���� ���������
  ������� ���������� � 1 ������� ������������� ������ �� FLASH.
	������� ������������� � ������ � ������� WIZnet:
	1. ���������� ����� ������
	2. �������� ~20�� �� ���������� ������
	3. ����������� ����� ������
	4. �������� ���������� ������ � ��������� �������
	5. ��������� ������� ���������� ������ - sys_init
	6. �������� ���������� ��������� � ��������� ������� ����������
	7. ��������� ���������� ������ - sock_init
	8. �������� ���������� ��������� � ��������� ������� ����������
	_______________________________________________________________________________________
	������ ����� �����������. ����� ������, ������ ������������ ����� �� ���������� �������
	����������� ����������.
	_______________________________________________________________________________________
	������ � ������� ���������:
		���� ��������� ������������ ����� ������ ��� �������� � ���� Ethernet, �� ��� ������ 
	��������� ������� ��������� ETH_Send.req �� ��������� ����. ���� �������� �� 0,
	������ ���������� ����� ��� �� ������� � ������� ������ ������. ���� �������� �������,
	�� ���������������� ��������� ����� ��������� ������ ��� ���������� UDP ������ � �����
	�������� ethTRbuff[ETH_TR_SIZE], ������� � �������� �������. ����� � �����������
	��������� ETH_Send ���� ������� ����� ������������ ������� ETH_Send.leng � ���������� � 1 
	������ �� �������� ETH_Send.req. ����� ��� ������� ETH_work() ����������� � ��������
	������ � ���� � ������ ������� �������� ETH_Send.req. ��� ������������� ��������� ����� �� 
	������ IP ����������, �����, �� ��������� ������� �������� ������, �������� ����� IP destination
	� ������ UserIPdestination[4]; ������� �� ������� ����� IP, � ���������� ������ �� ����� IP �
	��������� ETH_stat - ������� ETH_stat.initRequest=3. (��������� ETH_stat.initRequest=1 �������� �
	����������� ������ ������ WIZnet � ��������� ������� ���������� �� ���������. ��������� 
	ETH_stat.initRequest=2 �������� � ����������� ������ � ��������� ���������������� �������
	�������� �� �������� UNet_Settings[23]; USocket_Settings[19];).
		�������� ������������ ����� ��������� �� ������������ ��������� ������ ������, ���
	�������� ������� ������ ���������� ������. ��� ����� ���������� ��������� ��������� �������
	��������� ��������� ������ ����������� � �������� ��������� ETH_Rec - ETH_Rec.req.
	��� �����������, ��� ������� ������� ��������� �� ����� ����, ��������� ������ ����� �
	�������� ETH_Rec.leng �������� ����� ���������� � �������� ����� ethRCbuff[ETH_RC_SIZE] ������, 
	���������� �����, ��� �� ������� ���� ����������� ��� �� ��������� ������ � ���������� �����, 
	� �������� ������� ������� ��������� ETH_Rec.req. ����� ����� ������� ETH_work() ������� �����������
	��������� ����� ��������� ������� �� ���������� ��������� ������.	
********************************************************************************************/
#include <avr/io.h>
#include "headers/defConstant.h"
#include "headers/defStruct.h"
#include "headers/MACRO.H"
#include "headers/W3100A.H"

#define FLASH_SET	1	//�������-���������� ��������� ������ �� FLASH ����� HW reset
#define USER_SET	2	//�������-���������� ��������� ������ �������� USB ����� HW reset
#define UIPDES_SET	3	//�������-���������� ��������� destination IP �������� USB

#define CNRI	0x81	//��������� Indirect Bus mode � ���������������
 
void 	dataTransfer(unsigned char);	//������� �������� ������ CPU-WIZnet 0-������ �� ������, 1-������.
void 	setIPdest(void);				//��������� ������, ����������������� IP destination

extern struct		TD					Tdata;

extern struct		ETH_status	ETH_stat;
extern struct		Request		ETH_Send;
extern struct		Request		ETH_Rec;

extern const unsigned char Net_Settings[23];	//������ ������� ��������� �� ���������
extern const unsigned char Socket_Settings[19];	//������ ��������� ������ �� ���������
extern unsigned char UNet_Settings[23];			//������ ���������������� ������� ���������
extern unsigned char USocket_Settings[19];		//������ ���������������� ��������� ������
extern unsigned char UserIPdestination[4];		//����� destination IP ������
extern unsigned char TPoint_Settings[4];
extern unsigned char RPoint_Settings[4];

extern volatile unsigned char time_out[NT];		// ������ ���������� ��� ����������� ��������

extern unsigned char ethTRbuff[ETH_TR_SIZE];		// ETH_TR_SIZE=24 ����� ��� ���������� ������ ������� ���������� ����������� ��� �������� �� ethernet
extern unsigned char ethRCbuff[ETH_RC_SIZE];		// ETH_RC_SIZE=32 ����� ��� ���������� ������ ������� ������ �� ���� ethernet ������ ���� �� 8 ���� ������
													// ������ �� ��������, ��� ��� �������� ���������: �����-2, ip-4, port-2.(����� ����� ������ ������ � ����������)

unsigned char* const ADR_IDM_OR =	(unsigned char*)0xfff0;	//����� �������� ���������� indirect Bus mode
unsigned char* const ADR_IDM_AR0 =	(unsigned char*)0xfff1;	//indirect bus I/F mode address0 register
unsigned char* const ADR_IDM_AR1 =	(unsigned char*)0xfff2;	//indirect bus I/F mode address1 register
unsigned char* const ADR_IDM_DR = 	(unsigned char*)0xfff3;	//indirect bus I/F mode data register


void ETH_work(void)
{
	static unsigned char *pNet;
	static unsigned char *pSoc;

	volatile unsigned int address;
	volatile unsigned char halfAdr;
	unsigned char prime2prt[8];	//����� ��� ���������� �������� 2-� ���������� ������ ������ WIZnet � �������� �� ������
	unsigned long	w,r,pr32;		//��������� ������ � ������ WIZnet
	int mw,mr,adif,free,busy;
	unsigned int dop;
	
	switch(ETH_stat.initRequest)	//��������� �������� ������� �� ������������� ������
	{
		case FLASH_SET:
		ETH_stat.phase = 0;
		pNet = (unsigned char*)Net_Settings;
		pSoc = (unsigned char*)Socket_Settings;
		break;
		case USER_SET:
		ETH_stat.phase = 0;
		pNet = (unsigned char*)UNet_Settings;
		pSoc = (unsigned char*)USocket_Settings;
		break;
		case UIPDES_SET:
		setIPdest();
		break;
		default:
		break;
	}
	ETH_stat.initRequest=0;			// ����� ������� ��������� ������������� ������.
	
	switch(ETH_stat.phase)
	{
		case 0:					//������ ��� �� ������ ���������� �����
		time_out[2]=50;				//��������� ������� ������ ������� ������������� (2�)
		resEthernet();				//����������� ������� ������ ��� ������ WIZnet
		ETH_stat.phase = 10;		//��������� �������� ����������� ����������� ������
		break;
		/*||||||||||||||||||||||||||*/
		case 1:					//���������� ������������ ������
		*ADR_IDM_OR = CNRI;			//��������� ������ indirect bus I/F, Big-endian, auto-increment.
		*ADR_IDM_AR0 = 0;			//������� ����� ������
		*ADR_IDM_AR1 = 0;			//������� ����� ������
		*ADR_IDM_DR = CSW_RESET;	//S/W Reset
		ETH_stat.phase = 2;
		break;
		/*||||||||||||||||||||||||||*/
		case 2:					//�������� ���������� ������������ ������
		if(time_out[2]== 0) ETH_stat.phase = 0;	//������ ������ �� ��������� ������ �������
		*ADR_IDM_AR0 = 0;			//������� ����� ������
		*ADR_IDM_AR1 = 0;			//������� ����� ������
		if(!(*ADR_IDM_DR & CSW_RESET)) ETH_stat.phase = 3;
		break;
		/*||||||||||||||||||||||||||*/
		case 3:			//���������� ������� ���������
		Tdata.pIData = (unsigned char*)NET_SETTINGS;
		Tdata.pEData = pNet;
		Tdata.lengs = 8;
		dataTransfer(1);	//����� ������� ������ ������ � ������
		/* ���������� ��������� ����������������� MAC ������ (6 ����) */
		Tdata.pIData = (unsigned char*)SRC_IP;
		Tdata.pEData = pNet+14;
		Tdata.lengs = 9;
		dataTransfer(1);	//����� ������� ������ ������ � ������
		
		*ADR_IDM_AR0 = 0;			//������� ����� ������
		*ADR_IDM_AR1 = 0;			//������� ����� ������
		*ADR_IDM_DR = CSYS_INIT;	//Sys_Init
		ETH_stat.phase = 4;
		break;
		/*||||||||||||||||||||||||||*/
		case 4:			//�������� ���������� ������� ���������
		if(time_out[2]== 0) ETH_stat.phase = 0;	//������ ������
		*ADR_IDM_AR0 = 0;			//������� ����� ������
		*ADR_IDM_AR1 = 0;			//������� ����� ������
		if(!(*ADR_IDM_DR & CSYS_INIT)) ETH_stat.phase = 5;
		break;
		/*||||||||||||||||||||||||||*/
		case 5:			//���������� ��������� ������
		Tdata.pIData = (unsigned char*)OPT_PROTOCOL;
		Tdata.pEData = pSoc;
		Tdata.lengs = 19;
		dataTransfer(1);	//����� ������� ������ ������ � ������
		
		*ADR_IDM_AR0 = 0;			//������� ����� ������
		*ADR_IDM_AR1 = 0;			//������� ����� ������
		*ADR_IDM_DR = CSOCK_INIT;	//CSOCK_INIT
		ETH_stat.phase = 6;
		break;
		/*||||||||||||||||||||||||||*/
		case 6:			//�������� ���������� ��������� ������
		if(time_out[2]== 0) ETH_stat.phase = 0;	//������ ������
		*ADR_IDM_AR0 = 0;			//������� ����� ������
		*ADR_IDM_AR1 = 0;			//������� ����� ������
		if(!(*ADR_IDM_DR & CSOCK_INIT)) ETH_stat.phase = 7;
		break;
		/*||||||||||||||||||||||||||*/
		case 7:			//��������� ���������� ������ � ������ ������
		Tdata.pIData = (unsigned char*)RW_PR;
		Tdata.pEData = (unsigned char*)RPoint_Settings;
		Tdata.lengs = 4;
		dataTransfer(1);	//����� ������� ������ ������ � ������
		
		Tdata.pIData = (unsigned char*)RR_PR;
		Tdata.pEData = (unsigned char*)RPoint_Settings;
		Tdata.lengs = 4;
		dataTransfer(1);	//����� ������� ������ ������ � ������
		
		Tdata.pIData = (unsigned char*)TW_PR;
		Tdata.pEData = (unsigned char*)TPoint_Settings;
		Tdata.lengs = 4;
		dataTransfer(1);	//����� ������� ������ ������ � ������
		
		Tdata.pIData = (unsigned char*)TR_PR;
		Tdata.pEData = (unsigned char*)TPoint_Settings;
		Tdata.lengs = 4;
		dataTransfer(1);	//����� ������� ������ ������ � ������
		
		ETH_stat.phase = 8;	
		break;
		/*||||||||||||||||||||||||||*/
		case 8:			//����� ������
		if(ETH_Rec.req)	//������� �� �������������� ����������� ������
		{
			ETH_stat.phase = 9;	//������� � �������� ������
			break;
		}
		/*.........................*/
		address = SRW_PR;
		halfAdr = (unsigned char)(address >> 8);
		*ADR_IDM_AR0 = halfAdr;		//������� ����� ������
		halfAdr = (unsigned char)(address);
		*ADR_IDM_AR1 = halfAdr;		//������� ����� ������
		halfAdr = *ADR_IDM_DR;		//������ ������� ���������
		/*.........................*/
		Tdata.pIData = (unsigned char*)RW_PR;		//adress RW_PR=0x10
		Tdata.pEData = (unsigned char*)prime2prt;
		Tdata.lengs = 4;
		dataTransfer(0);	//����� ������� ������ ��������� ��������� RW_PR �� ������
		/*.........................*/
		address = SRR_PR;
		halfAdr = (unsigned char)(address >> 8);
		*ADR_IDM_AR0 = halfAdr;		//������� ����� ������
		halfAdr = (unsigned char)(address);
		*ADR_IDM_AR1 = halfAdr;		//������� ����� ������
		halfAdr = *ADR_IDM_DR;		//������ ������� ���������
		/*.........................*/
		Tdata.pIData = (unsigned char*)RR_PR;		//adress RR_PR
		Tdata.pEData = (unsigned char*)(prime2prt+4);
		Tdata.lengs = 4;
		dataTransfer(0);	//����� ������� ������ ��������� ��������� RR_PR �� ������
		/*.........................*/
		
		pr32= (unsigned long)prime2prt[0];
		w = (pr32<<24);
		pr32= (unsigned long)prime2prt[1];
		w |= (pr32<<16);
		pr32= (unsigned long)prime2prt[2];
		w |= (pr32<<8);
		w |= (unsigned long)prime2prt[3];
		
		pr32= (unsigned long)prime2prt[4];
		r = (pr32<<24);
		pr32= (unsigned long)prime2prt[5];
		r |= (pr32<<16);
		pr32= (unsigned long)prime2prt[6];
		r |= (pr32<<8);
		r |= (unsigned long)prime2prt[7];
		
		pr32=RECV_DATA_BUF+(r & PMASK);	// Pointer to W3100A receive buffer
		
		mw = (int)(w & PMASK);
		mr = (int)(r & PMASK);
		adif = mw-mr;
		if(adif >= 0) busy = adif;
		else busy = FULLSIZE + adif;	//������ ������� ���������� ������
		if(busy == 0)					//��� ��������� � �������� ������ ������ WIZnet
		{
			ETH_stat.phase = 9; 	//������ �� ��������
			break;
		}
		/*.........................*/
		if(busy > ETH_RC_SIZE)		//������ ������ ��������� ������ �������� ������
		{
			dop = (unsigned int)pr32;
			*ADR_IDM_AR0 = (unsigned char)(dop >> 8);	//������� ����� ������
			*ADR_IDM_AR1 = (unsigned char)(dop);			//������� ����� ������
			for(dop=0; dop<busy; dop++) halfAdr = *ADR_IDM_DR;	//������
		}
		else
		{
			dop = (unsigned int)pr32;
			Tdata.pIData = (unsigned char*)dop;
			Tdata.pEData = (unsigned char*)ethRCbuff;
			Tdata.lengs = (unsigned char)busy;
			dataTransfer(0);			//����� ������� ������ ������ �� ������
			ETH_Rec.req = 1;	
			ETH_Rec.leng = (unsigned char)busy;
		}
		/*.........................*/
		pr32 = r + (unsigned long)busy;
		prime2prt[0] = (unsigned char) (pr32 >> 24);
		prime2prt[1] = (unsigned char) (pr32 >> 16);
		prime2prt[2] = (unsigned char) (pr32 >> 8);
		prime2prt[3] = (unsigned char) (pr32);
		
		Tdata.pIData = (unsigned char*) RR_PR;
		Tdata.pEData = (unsigned char*)prime2prt;
		Tdata.lengs = 4;
		dataTransfer(1);			//update ponter RR_PR
		/*.........................*/
		*ADR_IDM_AR0 = 0;
		*ADR_IDM_AR1 = 0;
		*ADR_IDM_DR = CRECV;		//To receive data 0x40
		ETH_stat.phase = 9; 		//������ �� ��������
		break;
		/*||||||||||||||||||||||||||*/
		case 9:					//�������� ������
		if(ETH_Send.req == 0)		// ���� ��� ������� � ������ ethernet �� ��������
		{
			ETH_stat.phase = 8; 	//������ �� �����
			break;
		}
		if(ETH_Send.leng > ETH_TR_SIZE)	//�������� ����� ���������
		{
			ETH_Send.req = 0;		//����� ������� ��������
			ETH_stat.phase = 8; 	//������ �� �����
			break;
		}
		*ADR_IDM_AR0 = 0;			//������� ����� ������
		*ADR_IDM_AR1 = 0;			//������� ����� ������
		if(*ADR_IDM_DR & CSOCK_INIT) //����� IP destination ��� �� �����������
		{
			ETH_stat.phase = 8; 	//������ �� �����
			break;
		}
		/*.........................*/
		address = STW_PR;
		halfAdr = (unsigned char)(address >> 8);
		*ADR_IDM_AR0 = halfAdr;		//������� ����� ������
		halfAdr = (unsigned char)(address);
		*ADR_IDM_AR1 = halfAdr;		//������� ����� ������
		halfAdr = *ADR_IDM_DR;		//������ ������� ���������
		/*.........................*/
		Tdata.pIData = (unsigned char*)TW_PR;		//adress TW_PR=0x40
		Tdata.pEData = (unsigned char*)prime2prt;
		Tdata.lengs = 4;
		dataTransfer(0);	//����� ������� ������ ���������� ����������� TW_PR �� ������
		/*.........................*/
		address = STR_PR;
		halfAdr = (unsigned char)(address >> 8);
		*ADR_IDM_AR0 = halfAdr;		//������� ����� ������
		halfAdr = (unsigned char)(address);
		*ADR_IDM_AR1 = halfAdr;		//������� ����� ������
		halfAdr = *ADR_IDM_DR;		//������ ������� ���������
		/*.........................*/
		Tdata.pIData = (unsigned char*)TR_PR;		//adress TW_PR=0x44
		Tdata.pEData = (unsigned char*)prime2prt+4;
		Tdata.lengs = 4;
		dataTransfer(0);	//����� ������� ������ ���������� ����������� TR_PR �� ������
		/*.........................*/
		pr32= (unsigned long)prime2prt[0];
		w = (pr32<<24);
		pr32= (unsigned long)prime2prt[1];
		w |= (pr32<<16);
		pr32= (unsigned long)prime2prt[2];
		w |= (pr32<<8);
		w |= (unsigned long)prime2prt[3];
		
		pr32= (unsigned long)prime2prt[4];
		r = (pr32<<24);
		pr32= (unsigned long)prime2prt[5];
		r |= (pr32<<16);
		pr32= (unsigned long)prime2prt[6];
		r |= (pr32<<8);
		r |= (unsigned long)prime2prt[7];
		
		mw = (int)(w & PMASK);
		mr = (int)(r & PMASK);
		adif = mw-mr;
		if(adif >= 0) free = (int)PMASK - adif;
		else free = -adif;			//������ ��������� ������
		if(free < ETH_Send.leng)	//������ ��������� ��������� ������ ��������� ������ ������
		{
			ETH_stat.phase = 8; 	//������ �� �����
			break;
		}
		/*.........................*/
		*ADR_IDM_AR0 = 0;			//������� ����� ������
		*ADR_IDM_AR1 = 0;			//������� ����� ������
		if((*ADR_IDM_DR & SSEND_OK))	//�������� ���������� ���������� ������� ��������
		{
			ETH_stat.phase = 8; 	//������ �� �����
			break;
		}
		/*.........................*/
		pr32=SEND_DATA_BUF+(w & PMASK);	// Pointer to W3100A send buffer
		dop = (unsigned int)pr32;
		Tdata.pIData = (unsigned char*)dop;
		Tdata.pEData = (unsigned char*)ethTRbuff;
		Tdata.lengs = ETH_Send.leng;
		dataTransfer(1);			//����� ������� ������ ������ � ������
		/*.........................*/
		pr32 = w + (unsigned long)ETH_Send.leng;
		prime2prt[0] = (unsigned char) (pr32 >> 24);
		prime2prt[1] = (unsigned char) (pr32 >> 16);
		prime2prt[2] = (unsigned char) (pr32 >> 8);
		prime2prt[3] = (unsigned char) (pr32);
		
		Tdata.pIData = (unsigned char*) TW_PR;
		Tdata.pEData = (unsigned char*)prime2prt;
		Tdata.lengs = 4;
		dataTransfer(1);			//update ponter TW_PR
		/*.........................*/
		*ADR_IDM_AR0 = 0;
		*ADR_IDM_AR1 = 0;
		*ADR_IDM_DR = CSEND;		//send command
		ETH_Send.req = 0;			//����� ������ �� ��������
		ETH_stat.phase = 8;			//������� � ������ ������	
		break;
		/*||||||||||||||||||||||||||*/
		case 10:					//�������� ������� ����� �� ������ Wiznet
		if(time_out[2] < 48)
		{
			setEthernet();			//������ ������ � ������ WIZnet
			ETH_stat.phase = 11;	//��������� �������� ����������� ����������� ������
		}
		break;
		/*||||||||||||||||||||||||||*/
		case 11:					//�������� ����� ������
		if(time_out[2] < 46)
		{

			ETH_Send.req = 0;
			ETH_Rec.req = 0;
			ETH_stat.phase = 1;		//��������� �������� ����������� ����������� ������
		}
		break;
		/*||||||||||||||||||||||||||*/
		default:
		ETH_stat.phase=0;
		break;
	}
}
/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/
void dataTransfer(unsigned char cod)	//������� �������� ������ CPU-WIZnet 0-������ �� ������, 1-������.
{
	unsigned char i,part;
	unsigned int conv;
	

	conv = (unsigned int)Tdata.pIData;
	part = (unsigned char)(conv >> 8);
	*ADR_IDM_AR0 = part;
	part = (unsigned char)conv;
	*ADR_IDM_AR1 = part;
	
	if(cod)		//������ � ������
	{
		for(i=0; i<Tdata.lengs; i++)
		{
			*ADR_IDM_DR = *(Tdata.pEData++);
		}
	}
	else		//������ �� ������
	{
		for(i=0; i<Tdata.lengs; i++)
		{
			*(Tdata.pEData++) = *ADR_IDM_DR;
		}
	}
	return;
}
/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/
void setIPdest(void)	//������� ��������� ������ destination IP.
{
	*ADR_IDM_AR0 = 0;
	*ADR_IDM_AR1 = DST_IP;
	*ADR_IDM_DR = UserIPdestination[0];
	*ADR_IDM_DR = UserIPdestination[1];
	*ADR_IDM_DR = UserIPdestination[2];
	*ADR_IDM_DR = UserIPdestination[3];
	
	*ADR_IDM_AR0 = 0;			//������� ����� ������
	*ADR_IDM_AR1 = 0;			//������� ����� ������
	*ADR_IDM_DR = CSOCK_INIT;	//CSOCK_INIT
	ETH_stat.initRequest = 0;
	return;
}
/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/

