/****************************************************************************
  Функция производит перенос данных пакета в память модуля Ethernet
  и запуск передачи пакета в сеть, при получении соответствующего кода 
  запроса передачи в структуре ETHtr. 
  После обработки пакета или его части, запрос передачи снимается.
  Для первой части полного пакета проводится проверка размера 
  доступной памяти в модуле Ethernet.
  Фиксируется ситуация переполнения внутренней памяти модуля,
  при этом инкрементируется счетчик ошибок структуры описания 
  состояния модуля Ethernet.
  Функция возвращает код 0, если работа по передаче пакета не заверщена и - код 1 после
  прохождения финальной стадии обработки.
 ****************************************************************************/

#include <avr/io.h>
#include "headers/macro.h"
#include "headers/w3100a.h"
#include "headers/defConstant.h"
#include "headers/defStruct.h"

#define MOVE		6
#define FIXPOINT	7
#define COMPL		8

extern struct	TWI_status	TWI_stat;	//структура состояния TWI интерфейса
extern struct Descript	ETH_dsc;	//рабочая структура функции обслуживания модуля Ethernet
extern struct Request		ETHtr;		//структура для запроса передачи данных пакета к модулю Ethernet

extern unsigned char ethTRbuff[ETH_TR_SIZE];	// 32 буфер для сохранения данных которые приложение приготовило для передачи по ethernet
extern unsigned char time_out[NT];	// массив переменных для организации задержек

unsigned char sendWork(void)
{
	static unsigned char	phase;		// фаза работы функции
	static unsigned long	w,r,pr32;	//указатели адреса в буфере WIZnet
	unsigned int	pr16;	
	unsigned char	j,result;
	int mw,mr,adif,free;
	
	result=0;
	switch(phase)
	{
		/* установка стартового адреса теневых регистров передатчика */
		case 0:
		if(ETHtr.req == 0) return(1);				// возврат из фазы старта, если нет запроса передачи.
		TWI_stat.TWI_state=TWI_BUSY;				// захват интерфейса TWI.												 
		pr16=STW_PR;								// указатель хвоста пакета в буфере передачи.
		TWI_stat.TWI_msgSize=3;						// leng message
		TWI_stat.TWI_buf[0]=SLA_W;					// команда+адрес девайса.
		TWI_stat.TWI_buf[1]=pr16>>8;				// high adress STW_PR
		TWI_stat.TWI_buf[2]=pr16;					// low adress STW_PR=0x1F0
		phase++;
		TWI_Start();								// Запуск механизма обработчика прерываний TWI.
		break;
		/* чтение 2х теневых регистров (использоваться не будут) */
		case 1:
		TWI_stat.TWI_state=TWI_BUSY;												 
		TWI_stat.TWI_msgSize=3;						// leng message
		TWI_stat.TWI_buf[0]=SLA_R;					// команда+адрес девайса.
		phase++;									// Запуск механизма обработчика прерываний TWI.
		TWI_Start();								// Запуск механизма обработчика прерываний TWI.
		break;
		/* просто задержка как в документации */
		case 2:									// задержка после чтения теневых регистров
		phase++;
		break;
		/* установка стартового адреса рабочих регистров передатчика */
		case 3:
		TWI_stat.TWI_state=TWI_BUSY;												 
		TWI_stat.TWI_msgSize=3;						// leng message
		TWI_stat.TWI_buf[0]=SLA_W;					// команда+адрес девайса.
		TWI_stat.TWI_buf[1]=0;						// high adress w
		TWI_stat.TWI_buf[2]=TW_PR;					// low adress TW_PR=0x40
		phase++;
		TWI_Start();								// Запуск механизма обработчика прерываний TWI.		
		break;
		/* чтение 2х регистров-указателей (4х байтных) */		
		case 4:
		TWI_stat.TWI_state=TWI_BUSY;												 
		TWI_stat.TWI_msgSize=9;						// leng message
		TWI_stat.TWI_buf[0]=SLA_R;					// команда+адрес девайса.
		phase++;
		TWI_Start();								// Запуск механизма обработчика прерываний TWI.	
		break;
		/* определение свободного пространства в памяти модуля */		
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
		else free = -adif;							//размер свободной памяти
		if(free < ETHtr.leng)						//размер сообщения превышает размер свободной памяти модуля
		{
			/* возврат=1 по ошибке */
			result = 1;
			phase = 0; 								//уходим на исходное состояние.
			ETH_dsc.countError++;					//инкремент счетчика ошибок.
			break;
		}
		case MOVE:
		if(ETHtr.req == 0) return(1);				// возврат из фазы старта, если нет запроса передачи.
		pr32=SEND_DATA_BUF+(w & PMASK);			// Pointer to W3100A send buffer
		w += (unsigned long)ETHtr.leng;			// предусмотрительно изменим указатель
		/* действия по инициализации передачи содержания буфера в память передатчика */
		TWI_stat.TWI_state=TWI_BUSY;
		pr16=(unsigned int)pr32;
		TWI_stat.TWI_msgSize=ETHtr.leng+3;			// leng message
		TWI_stat.TWI_buf[0]=SLA_W;
		TWI_stat.TWI_buf[1]=(pr16>>8);				// high adress
		TWI_stat.TWI_buf[2]=pr16;					// low adress
		for(j=0; j < ETHtr.leng; j++)
		{
			TWI_stat.TWI_buf[j+3]=ethTRbuff[j];	// копирование данных для передачи
		}
		phase=FIXPOINT;								// завершение передачи
		ETHtr.req=0;								// сброс запроса передачи
		TWI_Start();								// запуск механизма обработки TWI прерывания
		break;
		/* модификация указателя */		
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
		phase=COMPL;								// послать команду передачи пакета
		TWI_Start();								// запуск механизма обработки TWI прерывания
		break;
		/* передача команды пакета */
		case COMPL:
		TWI_stat.TWI_state=TWI_BUSY;												 
		TWI_stat.TWI_msgSize=4;						// leng message
		TWI_stat.TWI_buf[0]=SLA_W;
		TWI_stat.TWI_buf[1]=0;						// high adress C0_CR
		TWI_stat.TWI_buf[2]=0;						// low adress C0_CR=0x00
		TWI_stat.TWI_buf[3]=CSEND;
		phase=0;
		result = 1;
		TWI_Start();								// запуск механизма обработки TWI прерывания
		break;
		/* сброс запроса передачи, возврат=1 по какой-то ошибке */
		default:
		ETHtr.req=0;
		phase=0;
		result = 1;
		break;
	}
	return(result);								// возврат 1 разрешает переход к обработчику приема пакета от модуля Ethernet
}

