/****************************************************************************
  Поэтапная работа с модулем ethernet.
  Функция проводит полную инициализацию модуля Ethernet и после этого
  производит последовательные вызовы функции чтения входных сообщений от модуля
  и функции выдачу модулю данных на передачу, если появляются таковые запросы.
  time_out[3] - используется для определения состояния зависания TWI интерфейса.
  Если обработчик прерывания интерфейса TWI возвращает код ошибки, то 
  инкрементируется счетчик ошибок структуры состояния модуля.
****************************************************************************/
#include <avr/io.h>
#include "headers/defConstant.h"
#include "headers/defStruct.h"
#include "headers/w3100a.h"
#include "headers/MACRO.H"
/****************************************************************************
 Определение некоторых фаз работы с модулем Ethernet
****************************************************************************/
#define ETH_RECEIVE			5			// Фаза работы с модулем Ethernet (чтение из модуля)	
#define ETH_TRANSMIT		6			// Фаза работы с модулем Ethernet (запись в модуль) 
/***************************************************************************/
extern struct Descript	ETH_dsc;	//рабочая структура функции обслуживания модуля Ethernet
extern struct Request		ETHtr;		//структура для запроса передачи данных пакета к модулю Ethernet
extern struct Request		ETHrc;		//структура для запроса приема данных пакета от модуля Ethernet

extern struct	TWI_status	TWI_stat;	//структура состояния TWI интерфейса

extern unsigned char time_out[NT];	// массив переменных для организации задержек
extern unsigned char resetState;		// описатель состояния цепи сброса модуля WizNet

void	netInit(void);					// сетевые установки
void	socketInit(void);				// установки сокета
void	pointInit(void);				// установки указателей памяти модуля
void	ethRe(char);					// контроль окончания процедуры очередной установки

unsigned char	sendWork(void);		// посылает часть пакета (возврат-0), или его хвост(возврат-1) из буфера передачи в модуль Ethernet.
unsigned char	recWork(void);			// принимает часть пакета (возврат-0), или его хвост(возврат-1) из модуля Ethernet в буфер приема.

/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/
void ETH_work(void)
{
	if(time_out[2] != 0) return;			// ожидание отработки сигнала сброс
	if(resetState == 0)
	{
		resetState=1;
		time_out[2]=12;
		releaseETH();						// снятие сигнала сброс с модуля
		ETH_dsc.phase=0;					// начнем с начала
	}
	if(TWI_stat.TWI_state == TWI_ERROR)
	{
		ETH_dsc.countError++;				//инкремент счетчика ошибок
		TWI_stat.TWI_state=TWI_FREE;
	}
/*-------------------------*/	
	switch(ETH_dsc.phase)
	{
		case 0:							// установка настроек сети
		netInit();
		break;
		case 1:							// ожидание завершения настроек сети
		ethRe(SSYS_INIT_OK);
		break;
		case 2:							// установка настроек сокета
		socketInit();
		break;
		case 3:							// ожидание завершения настроек сокета
		ethRe(SSOCK_INIT_OK);
		break;
		case 4:							// инициализация указателей памяти модуля ethernet
		pointInit();
		break;
		/*Случай приема информации от модуля Ethernet*/
		case ETH_RECEIVE:
		if(recWork()) ETH_dsc.phase=ETH_TRANSMIT;
		break;
		/*Случай предачи информации в модуль Ethernet*/
		case ETH_TRANSMIT:
		if(sendWork()) ETH_dsc.phase=ETH_RECEIVE;		// вызов шага передатчика.
		break;
		/* какой-то сбой.*/
		default:
		ETH_dsc.phase=ETH_RECEIVE;
		ETH_dsc.countError++;
		break;
	}
}

