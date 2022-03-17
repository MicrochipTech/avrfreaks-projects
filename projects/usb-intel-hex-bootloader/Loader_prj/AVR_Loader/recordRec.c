#include <avr/io.h>
#include "Define/MACRO.H"
#include "Define/defConstant.h"
#include "Define/defStruct.h"
/****************************************************************/
/*
	���������� ������ Intel hex �����.
	
	An Intel HEX file is composed of any number of HEX records.
	Each record is made up of five fields that are arranged in the following format:
	:llaaaatt[dd...]cc
	* : is the colon that starts every Intel HEX record.
    * ll is the record-length field that represents the number of data bytes (dd) in the record.
    * aaaa is the address field that represents the starting address for subsequent data in the record.
    * tt is the field that represents the HEX record type, which may be one of the following:
      00 - data record
      01 - end-of-file record
      02 - extended segment address record
	  03 - segment Address Record
      04 - extended linear address record
	  05 - linear Address Record
    * dd is a data field that represents one byte of data. A record may have multiple data bytes.
	The number of data bytes in the record must match the number specified by the ll field.
    * cc is the checksum field that represents the checksum of the record.
	The checksum is calculated by summing the values of all hexadecimal digit pairs in the record
	modulo 256 and taking the two's complement.
*/
/****************************************************************/
extern struct descWork dw;			// ��������� - ��������� ���� ������ ����������
extern struct Request rc;				//��������� ������� �� ��������� �������� ������ �� USB
extern struct Request tx;				//��������� ������� �� �������� ������ � ������� USB
/****************************************************************/
extern unsigned char dataFild[SL];	//����� ��� ���������� ������ �� ���� ������ ������
extern unsigned char rcBuff[USBB];	//����� ���������
extern unsigned char txBuff[USBB];	//����� �����������
/****************************************************************/
void resetState(unsigned char s);
void ansFault(unsigned char s);
/****************************************************************/
unsigned char transform(unsigned char);
/****************************************************************/

#define ENDF	0x0D		//������ ���� ���������� ������
#define ENDS	0x0A		//������ ���� ���������� ������

void recordRec(void)
{
	static unsigned char store;	//���������� ��������
	static unsigned char ll;		//����� ���� ������ hex ������
	static unsigned char index;	//������� ��� ���� ������ hex ������
	static unsigned char tt;		//��� hex ������
	static unsigned char sum;	//����������� ����� hex ������
	static unsigned int aaaa;		//����� ������ � hex ������
	unsigned char tmpByte;		//���������� ��� ��������������
	
	if(dw.phase != 3)	return;	//������� �� ������� �� ���� ������������ ������ !=3
	if(dw.reqRecord) return;		//�� ���������� ���������� ������
	if(rc.req == 0) return;		//������� �� ���������� �������� �����
	
	rc.req=0;

	/*����� ������ ���� ������������� ��� ������������� ��������*/
	tmpByte = rcBuff[0];
	/*-----------------------------*/
	/* ���������� �������� ����� */
	switch(dw.phRecord)
	{
		case 0:
		index=0;
		sum=0;
		ll=0;
		aaaa=0;
		tt=0;
		if(tmpByte != ':')			//�������� ���������
		{
			resetState(1);			//������� � ��������� �������
			ansFault(0);			//��������� �� ������ �������
			dw.phRecord=0;
		}
		else dw.phRecord++;
		break;
		/*-----------------*/
		case 1:
		store=transform(tmpByte);
		break;
		/*-----------------*/
		case 2:
		tmpByte=transform(tmpByte);
		ll=(store<<4) | tmpByte;
		sum=ll;
		if(ll > SL)				//����������, �� ���������� ������� ������
		{
			resetState(1);			//������� � ��������� �������
			ansFault(0);			//��������� �� ������ �������
			dw.phRecord=0;
		}
		break;
		/*-----------------*/
		case 3:
		tmpByte=transform(tmpByte);
		aaaa = ((unsigned int)tmpByte<<12);
		sum += (tmpByte<<4);
		break;
		/*-----------------*/
		case 4:
		tmpByte=transform(tmpByte);
		aaaa |= ((unsigned int)tmpByte<<8);
		sum += tmpByte;
		break;
		/*-----------------*/
		case 5:
		tmpByte=transform(tmpByte);
		aaaa |= ((unsigned int)tmpByte<<4);
		sum += (tmpByte<<4);
		break;
		/*-----------------*/
		case 6:
		tmpByte=transform(tmpByte);
		aaaa |= ((unsigned int)tmpByte);
		sum += tmpByte;
		break;
		/*-----------------*/
		case 7:
		tmpByte=transform(tmpByte);
		tt = (tmpByte<<4);
		sum += (tmpByte<<4);
		break;
		/*-----------------*/
		case 8:
		tmpByte=transform(tmpByte);
		tt |= tmpByte;
		sum += tmpByte;
		if(ll == 0) dw.phRecord=11;	//���� ������ ���
		break;
		/*-----------------*/
		case 9:
		store=(transform(tmpByte)<<4);
		break;
		/*-----------------*/
		case 10:
		tmpByte=transform(tmpByte);
		tmpByte |= store;
		dataFild[index++]=tmpByte;
		sum += tmpByte;
		if(index < ll) dw.phRecord=9;
		break;
		/*-----------------*/
		case 11:
		store=transform(tmpByte);
		break;
		/*-----------------*/
		case 12:
		tmpByte=transform(tmpByte);
		tmpByte |= (store<<4);
		sum += tmpByte;
		if(sum != 0)				//������ ����������� �����
		{
			resetState(1);
			ansFault(0);
			dw.phRecord=0;
			break;
		}
		break;
		/*-----------------*/
		case 13:
		if(tmpByte != ENDF)		//������ ������� ����� ���������� ������
		{
			resetState(1);
			ansFault(0);
			dw.phRecord=0;
			break;
		}
		dw.phRecord++;
		break;
		/*-----------------*/
		case 14:
		if(tmpByte != ENDS)		//������  ����� ���������� ������
		{
			ansFault(0);
			dw.phRecord=0;
			break;
		}
		dw.phRecord=0;
		dw.reqRecord=1;
		dw.ll=ll;
		dw.aaaa=aaaa;
		dw.tt=tt;
		sum=0;
		break;
		/*-----------------*/
		default:
		resetState(1);
		ansFault(0);
		dw.phRecord=0;
		break;
	}
}
/*-----------------------------------------------*/
unsigned char transform(unsigned char aa)
{
	unsigned char bb;
	
	dw.phRecord++;
	if(aa < ':')
	{
		bb=aa-0x30;
		return (bb & 0x0F);
	}
	if(aa > '@')
	{
		bb=aa-0x37;
		return (bb & 0x0F);
	}
	return 0xFF;
}
/*-----------------------------------------------*/

