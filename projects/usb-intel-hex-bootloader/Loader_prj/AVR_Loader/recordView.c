/****************************************************************/
#include <avr/io.h>
#include "Define/MACRO.H"
#include "Define/defConstant.h"
#include "Define/defStruct.h"
#include <avr/pgmspace.h>
#include <avr/boot.h>
#include <string.h>
/****************************************************************/
/*
	��������� ���������� ������ Intel hex �����.
	�������������� 6 ���� hex �������:
	- ������ ������
	- ������ � ����� �����
	- ������ ���� ������� ��������� ������
	������ ���������� � �������� ��������� ������ - �� �� 32� ��������� ������� �����
	������ ����� �������� ��� ���������������� - 256 ����
	����� ������ ������ �������� ���������� ����������� ���������� � ���������
*/
/****************************************************************/
extern struct descWork dw;			// ��������� - ��������� ���� ������ ����������
extern struct Request tx;				//��������� ������� �� �������� ������ � ������� USB
/****************************************************************/
extern unsigned char dataFild[SL];	//����� ��� ���������� ������ �� ���� ������ ������
extern unsigned char txBuff[USBB];	//����� �����������
/****************************************************************/
extern unsigned char preparePage[PS];//����� ��� �������� ����� ������� �� Flash ��������
extern unsigned char controlPage[PS];//����� ��� �������� ���������� ����������� Flash �������� ����� ������
/****************************************************************/
/****************************************************************/
void resetState(unsigned char s);
void ansFault(unsigned char s);
void ansSuccess(void);			//������ ��������� �� �������� ���������� ����������������
/****************************************************************/
void boot_program_page (uint32_t page, uint8_t *buf);
void pageWriting(void);
void clear_page (void);
void set_limit (void);
void set_exline(void);
void set_line(void);
void set_exseg(void);
void set_seg(void);
void data_copy(void);
/****************************************************************/
/****************************************************************/

#define DATATYPE	0x00
#define ENDFILE		0x01
#define EXSEGM		0x02
#define SEGM		0x03
#define EXLINE		0x04
#define LINE		0x05

#define STARTLOADER		0x0001E000
#define SHIFT			0x100
#define MASK			0x000000FF
#define IMASK			0xFFFFFF00

void recordView(void)
{
	unsigned char count;
	
	if(dw.reqRecord == 0) return;	//������� �� ������� ���������� ������� �� ��������� ������
	
	dw.reqRecord=0;					//����� ������� ���������
	/*-----------------------------*/
	
	switch(dw.tt)					//��������� �������� �������
	{
		case EXSEGM:				//������ ��������� �� ��������� ������ ������������ ��������
		set_exseg();
		return;
		///////////////////
		case SEGM:					//������ ��������� �� ��������� ������ ��������
		set_seg();
		return;
		///////////////////
		case EXLINE:				//������ ��������� �� ��������� ������������ ��������� ������
		set_exline();
		return;
		//////////////////
		case LINE:					//������ ��������� �� ��������� ��������� ������
		set_line();
		return;
		//////////////////
		default:
		break;
	}
	
	/* ���������� ������ � �������� ������ ������ */
	if(dw.page == 0)
	{
		clear_page();				//������� ������ ��������
		switch(dw.tt)
		{
			case DATATYPE:
			dw.page = dw.dop + dw.aaaa;
			set_limit();
			data_copy();
			return;
			//////////////////
			case ENDFILE:
			default:
			resetState(1);			//������� � ��������� �������
			ansFault(0);			//��������� �� ������ �������
			//////////////////
			return;
		}
	}
	/* ���������� ��������� ����� */
	
	switch(dw.tt)
	{
		case DATATYPE:
		dw.page = dw.dop + dw.aaaa;
		if(dw.page >= dw.lim)
		{
			pageWriting();			//������ ��������
			set_limit();
		}
		data_copy();
		break;
		//////////////////
		case ENDFILE:
		if((dw.page & MASK) != 0)	//�������� ��� �� �����������
		{
			count = (unsigned char)dw.page;
			while(count != 0)
			{
				preparePage[count++] = 0xFF;
				dw.page++;
			}
			dw.reqWRend=1;			//������� ��������� �����
			pageWriting();			//������ ��������
		}
		else dw.phase = 4;			//���� ��������� ��� ������
		break;
		//////////////////
		default:
		resetState(1);			//������� � ��������� �������
		ansFault(0);			//��������� �� ������ �������
		break;
	}
}
/*-----------------------------------------------*/
/*----������ ���������� �������� �� Flash--------*/
/*---����� �� ������������ � ������������� ����������---*/
/*-----------------------------------------------*/
void pageWriting(void)
{
	PGM_P p;
	uint32_t b;
	
	if(dw.page > STARTLOADER)	//������� ������ � ������� ����������
	{
		resetState(1);		//������� � ��������� �������
		ansFault(0);		//��������� �� ������ �������
		return;
	}
	
	RED_ON();
	b = dw.lim-SHIFT;
	boot_program_page(b, preparePage);
	RED_OFF();
	
	if(b < 0x1000)			//�� �������� ��� ������ �������� Flash
	{
		memcpy(&p, &b, sizeof(PGM_P));
		
		memcpy_P(controlPage, p, SPM_PAGESIZE);
		if(memcmp(controlPage, preparePage, SPM_PAGESIZE))
		{
			resetState(1);		//������� � ��������� �������
			ansFault(1);		//��������� �� ������ ����������������
		}
	}
	else
	{
		int i;
		for(i=0; i<SPM_PAGESIZE ; i++)
		{
			controlPage[i]=pgm_read_byte_far(b++);
		}
		if(memcmp(controlPage, preparePage, SPM_PAGESIZE))
		{
			resetState(1);		//������� � ��������� �������
			ansFault(1);		//��������� �� ������ ����������������
		}
	}
	if(dw.reqWRend) dw.phase = 4;
	clear_page();			//���������� �������� ��� ���������� �������}
}	
/*-----------------------------------------------*/
/*-----------------------------------------------*/
void boot_program_page (uint32_t page, uint8_t *buf)
{
	uint16_t i;
	
	eeprom_busy_wait();
	boot_page_erase(page);
	boot_spm_busy_wait();		// Wait until the memory is erased.

	for (i=0; i<SPM_PAGESIZE; i+=2)
	{
		// Set up little-endian word.
		uint16_t w = *buf++;
		w += (*buf++) << 8;
		boot_page_fill(page + i, w);
	}
	
	boot_page_write(page);		// Store buffer in flash page.
	boot_spm_busy_wait();		// Wait until the memory is written.

	// Reenable RWW-section again. We need this if we want to jump back
	// to the application after bootloading.
	boot_rww_enable();
}
/*-----------------------------------------------*/
/*---------�������� �������----------------------*/
/*-----------------------------------------------*/
void clear_page(void)			//���������� ������ ���������� ��������� - 0xFF
{
	unsigned int i;
	
	for(i=0; i<SPM_PAGESIZE; i++)
	{
		preparePage[i] = 0xFF;
	}
}
/*-----------------------------------------------*/
/*-----------------------------------------------*/
void set_limit(void)			//��������� ����������� ������ ��� ������ ������ ��������
{
	dw.lim = (dw.dop + dw.aaaa + SHIFT) & IMASK;
}
/*-----------------------------------------------*/
/*-----------------------------------------------*/
void set_exline(void)			//������ ��������� �� ��������� ������������ ��������� ������, ��������� ���������� ����� ������
{
	dw.dop = ((uint32_t)dataFild[0]<<24) | ((uint32_t)dataFild[1]<<16);
}
/*-----------------------------------------------*/
/*-----------------------------------------------*/
void set_line(void)			//������ ��������� �� ��������� ��������� ������, ��������� ���������� ����� ������
{
	dw.dop = ((uint32_t)dataFild[0]<<24) | ((uint32_t)dataFild[1]<<16) | ((uint32_t)dataFild[2]<<8) | (uint32_t)dataFild[3];
}
/*-----------------------------------------------*/
/*-----------------------------------------------*/
void set_exseg(void)			//������ ��������� �� ��������� ������ ������������ ��������, ��������� ���������� ����� ������
{
	dw.dop = ((uint32_t)dataFild[0]<<12) | ((uint32_t)dataFild[1]<<4);
}
/*-----------------------------------------------*/
/*-----------------------------------------------*/
void set_seg(void)				//������ ��������� �� ��������� ������ ��������, ��������� ���������� ����� ������
{
	dw.dop = (((uint32_t)dataFild[0]<<12) | ((uint32_t)dataFild[1]<<4)) + (((uint32_t)dataFild[2]<<8) | (uint32_t)dataFild[3]);
}
/*-----------------------------------------------*/
/*-----------------------------------------------*/
void data_copy(void)			//����������� ������ � �������� �� ���� ������ ������ � �������� ���������� ��������
{
	unsigned char di;
	unsigned char pa;
	
	pa = (unsigned char)dw.page;
	for(di=0; di<dw.ll; di++)
	{
		preparePage[pa++] = dataFild[di];
		dw.page++;
		if(pa == 0)			//����� ��������
		{
			pageWriting();		//������ ��������
			dw.lim += SHIFT;	//��������� ����������� ������
		}
	}
}
/*-----------------------------------------------*/
/*-----------------------------------------------*/
