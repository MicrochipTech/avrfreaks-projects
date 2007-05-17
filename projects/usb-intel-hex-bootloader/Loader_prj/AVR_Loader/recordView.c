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
	Обработка полученной строки Intel hex файла.
	Обрабатываются 6 типа hex записей:
	- запись данных
	- запись о конце файла
	- четыре типа записей установки адреса
	Работа проводится с линейным счетчиком байтов - он же 32х разрядный текущий адрес
	Размер одной страницы для программирования - 256 байт
	После записи каждой страницы проводится контрольное считывание и сравнение
*/
/****************************************************************/
extern struct descWork dw;			// структура - описатель фазы работы загрузчика
extern struct Request tx;				//структура запроса на передачу пакета в сторону USB
/****************************************************************/
extern unsigned char dataFild[SL];	//буфер для накопленых байтов из поля данных записи
extern unsigned char txBuff[USBB];	//буфер передатчика
/****************************************************************/
extern unsigned char preparePage[PS];//буфер для страницы перед записью во Flash программ
extern unsigned char controlPage[PS];//буфер для контроля сравнением прочитанной Flash страницы после записи
/****************************************************************/
/****************************************************************/
void resetState(unsigned char s);
void ansFault(unsigned char s);
void ansSuccess(void);			//выдача сообщения об успешном завершении программирования
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
	
	if(dw.reqRecord == 0) return;	//возврат по причине отсутствия запроса на обработку записи
	
	dw.reqRecord=0;					//сброс запроса обработки
	/*-----------------------------*/
	
	switch(dw.tt)					//обработка адресных записей
	{
		case EXSEGM:				//запись указывает на установку адреса расширенного сегмента
		set_exseg();
		return;
		///////////////////
		case SEGM:					//запись указывает на установку адреса сегмента
		set_seg();
		return;
		///////////////////
		case EXLINE:				//запись указывает на установку расширенного линейного адреса
		set_exline();
		return;
		//////////////////
		case LINE:					//запись указывает на установку линейного адреса
		set_line();
		return;
		//////////////////
		default:
		break;
	}
	
	/* обработчик первой и возможно второй записи */
	if(dw.page == 0)
	{
		clear_page();				//очистка буфера страницы
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
			resetState(1);			//возврат к процедуре диалога
			ansFault(0);			//сообщение об ошибке формата
			//////////////////
			return;
		}
	}
	/* обработчик остальных строк */
	
	switch(dw.tt)
	{
		case DATATYPE:
		dw.page = dw.dop + dw.aaaa;
		if(dw.page >= dw.lim)
		{
			pageWriting();			//запись страницы
			set_limit();
		}
		data_copy();
		break;
		//////////////////
		case ENDFILE:
		if((dw.page & MASK) != 0)	//страница еще не закончилась
		{
			count = (unsigned char)dw.page;
			while(count != 0)
			{
				preparePage[count++] = 0xFF;
				dw.page++;
			}
			dw.reqWRend=1;			//признак окончания файла
			pageWriting();			//запись страницы
		}
		else dw.phase = 4;			//фаза установки бит защиты
		break;
		//////////////////
		default:
		resetState(1);			//возврат к процедуре диалога
		ansFault(0);			//сообщение об ошибке формата
		break;
	}
}
/*-----------------------------------------------*/
/*----Запись полученной страницы во Flash--------*/
/*---Взято из рекомендации к использованию библиотеки---*/
/*-----------------------------------------------*/
void pageWriting(void)
{
	PGM_P p;
	uint32_t b;
	
	if(dw.page > STARTLOADER)	//попытка записи в область загрузчика
	{
		resetState(1);		//возврат к процедуре диалога
		ansFault(0);		//сообщение об ошибке формата
		return;
	}
	
	RED_ON();
	b = dw.lim-SHIFT;
	boot_program_page(b, preparePage);
	RED_OFF();
	
	if(b < 0x1000)			//не работает для второй половины Flash
	{
		memcpy(&p, &b, sizeof(PGM_P));
		
		memcpy_P(controlPage, p, SPM_PAGESIZE);
		if(memcmp(controlPage, preparePage, SPM_PAGESIZE))
		{
			resetState(1);		//возврат к процедуре диалога
			ansFault(1);		//сообщение об ошибке программирования
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
			resetState(1);		//возврат к процедуре диалога
			ansFault(1);		//сообщение об ошибке программирования
		}
	}
	if(dw.reqWRend) dw.phase = 4;
	clear_page();			//подготовка страницы для заполнения данными}
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
/*---------Полезные функции----------------------*/
/*-----------------------------------------------*/
void clear_page(void)			//заполнение буфера подготовки символами - 0xFF
{
	unsigned int i;
	
	for(i=0; i<SPM_PAGESIZE; i++)
	{
		preparePage[i] = 0xFF;
	}
}
/*-----------------------------------------------*/
/*-----------------------------------------------*/
void set_limit(void)			//установка предельного адреса для вызова записи страницы
{
	dw.lim = (dw.dop + dw.aaaa + SHIFT) & IMASK;
}
/*-----------------------------------------------*/
/*-----------------------------------------------*/
void set_exline(void)			//запись указывает на установку расширенного линейного адреса, установка аддитивной части адреса
{
	dw.dop = ((uint32_t)dataFild[0]<<24) | ((uint32_t)dataFild[1]<<16);
}
/*-----------------------------------------------*/
/*-----------------------------------------------*/
void set_line(void)			//запись указывает на установку линейного адреса, установка аддитивной части адреса
{
	dw.dop = ((uint32_t)dataFild[0]<<24) | ((uint32_t)dataFild[1]<<16) | ((uint32_t)dataFild[2]<<8) | (uint32_t)dataFild[3];
}
/*-----------------------------------------------*/
/*-----------------------------------------------*/
void set_exseg(void)			//запись указывает на установку адреса расширенного сегмента, установка аддитивной части адреса
{
	dw.dop = ((uint32_t)dataFild[0]<<12) | ((uint32_t)dataFild[1]<<4);
}
/*-----------------------------------------------*/
/*-----------------------------------------------*/
void set_seg(void)				//запись указывает на установку адреса сегмента, установка аддитивной части адреса
{
	dw.dop = (((uint32_t)dataFild[0]<<12) | ((uint32_t)dataFild[1]<<4)) + (((uint32_t)dataFild[2]<<8) | (uint32_t)dataFild[3]);
}
/*-----------------------------------------------*/
/*-----------------------------------------------*/
void data_copy(void)			//копирование данных в страницу из поля данных записи и контроль заполнения страницы
{
	unsigned char di;
	unsigned char pa;
	
	pa = (unsigned char)dw.page;
	for(di=0; di<dw.ll; di++)
	{
		preparePage[pa++] = dataFild[di];
		dw.page++;
		if(pa == 0)			//конец страницы
		{
			pageWriting();		//запись страницы
			dw.lim += SHIFT;	//коррекция предельного адреса
		}
	}
}
/*-----------------------------------------------*/
/*-----------------------------------------------*/
