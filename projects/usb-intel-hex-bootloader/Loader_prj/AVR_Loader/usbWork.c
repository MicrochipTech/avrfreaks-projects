#include <avr/io.h>
#include <avr/wdt.h>
#include "Define/MACRO.H"
#include "Define/defConstant.h"
#include "Define/defStruct.h"
/****************************************************************/
/*
	Прием побайтный из USB шины в буфер приемника
	и передача побайтная из буфера передатчика в м/с USB драйвера
	слежение за счетчиком time-out.
*/
/****************************************************************/
extern struct descWork dw;			// структура - описатель фазы работы загрузчика
extern struct Request rc;				//структура запроса на обработку входного пакета от USB
extern struct Request tx;				//структура запроса на передачу пакета в сторону USB
/****************************************************************/
extern unsigned char rcBuff[USBB];	//буфер приемника
extern unsigned char txBuff[USBB];	//буфер передатчика
extern unsigned char time_out[NT];	// массив для организации задержек
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

	if(dw.phase == 0)							//возврат по причине дисконнекта по USB
	{
		rc.req=0;								//сброс запросов
		tx.req=0;								//сброс запросов
		if(time_out[0] == 0)					//истек лимит времени ожидания соединения с USB, выход в приложение пользовалеля
		{
			wdt_disable();
			release();							// выход в пользовательскую программу
		}
		return;
	}
	/**/
	if(time_out[0] == 0)
	{
		time_out[0]=DEL1000;
		resetState(1);							//возврат к фазе начального диалога по тайм-ауту
		ansFault(2);							//сообщение об ошибке времени ожидания
		return;	
	}
	/**/
	stat=getUSBstat();							//чтение битов гоовности -TXF -RXF
	/*-----------------------------*/
	i=stat & RCMASK;
	/* выполнено сразу 2а условия - готовность приемника и нет запроса на обраротку буфера приема */
	
	if((i | rc.req) == 0)
	{
		rcBuff[0]=getUSBdata();				//перемещение данных
		rc.req=1;								//установка запроса обработки
		rc.len=1;								//установка длины пакета на 1 байт
		time_out[0]=DEL2000;					//инициализация таймера ожидания
	}
	/*-----------------------------*/
	i=stat & TXMASK;
	/* выполнено сразу 2а условия - готовность передатчика и есть запрос на передачу буфера */
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
		else tx.req=0;							//сброс запроса передачи
		time_out[0]=DEL2000;					//инициализация таймера ожидания
	}
}

