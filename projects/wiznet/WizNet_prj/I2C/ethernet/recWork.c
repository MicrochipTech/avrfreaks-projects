/*
	Функция для обнаружения и откачки нового UDP пакета от модуля Ethernet
	Полезная работа происходит только при условии снятия предыдущего запроса
	на обработку пакета от этой функции.
	Обработку полученного пакета производит внешняя программа.
	Структура запроса обработки включает признак запроса и счетчик байт в буфере приема.
	Функция возвращает код 0, если работа по чтению пакета не заверщена и - код 1 после
	прохождения финальной стадии обработки.
*/
#include <avr/io.h>
#include "headers/MACRO.h"
#include "headers/w3100a.h"
#include "headers/defConstant.h"
#include "headers/defStruct.h"

extern const unsigned char Socket_Settings[19];	// массив установок сокета, здесь нужен для сравнения IP and Port num входного пакета.

extern struct	TWI_status	TWI_stat;	//структура состояния TWI интерфейса
extern struct Descript	ETH_dsc;	//рабочая структура функции обслуживания модуля Ethernet
extern struct Request		ETHrc;		//структура для запроса обработки данных принятых от модулю Ethernet

extern unsigned char	ethRCbuff[ETH_RC_SIZE];	// 40 буфер для сохранения данных которые пришли по сети ethernet
extern unsigned char time_out[NT];	// массив переменных для организации задержек

#define HEADLEN	8						// длина заголовка принятого UDP пакета
/*	Именованные состояния фазы машины состояний */
#define BREAD	13
#define CREAD	14
#define PMODIFY	15
#define FIXREAD	16

unsigned char recWork(void)
{
	static unsigned char	phase=0;
	static unsigned char temp_size;
	static unsigned char error_size;
	static int	lengUDP=0;				// длина оставшейся в памяти части UDP пакета.	
	static unsigned long	w,r,pr32;	// 4х байтовые указатели приемного буфера внутри модуля Ethernet.
	
	unsigned int	pr16;	
	unsigned char	i;
	unsigned char	result;
	int mw,mr,adif,busy;
	
	result = 0;
	switch(phase)
	{
		/* установка стартового адреса теневого регистра SRW_PR приемника */
		case 0:
		if(ETHrc.req) return(1);						// возврат из фазы старта, если не обработан предыдущий запрос.
		TWI_stat.TWI_state=TWI_BUSY;					// захват интерфейса TWI.
		pr16=SRW_PR;									// указатель конца пакета в буфере приема.
		TWI_stat.TWI_msgSize=3;							// leng message
		TWI_stat.TWI_buf[0]=SLA_W;						// команда+адрес девайса.
		TWI_stat.TWI_buf[1]=pr16>>8;					// high adress SRW_PR
		TWI_stat.TWI_buf[2]=pr16;						// low adress SRW_PR=0x1E0
		phase++;
		TWI_Start();									// Запуск механизма обработчика прерываний TWI.
		break;
		/* чтение теневого регистра (использоваться не будет) */
		case 1:
		TWI_stat.TWI_state=TWI_BUSY;					// захват интерфейса TWI.												 
		TWI_stat.TWI_msgSize=2;							// leng message
		TWI_stat.TWI_buf[0]=SLA_R;						// команда+адрес девайса.
		phase++;
		TWI_Start();									// Запуск механизма обработчика прерываний TWI.
		break;
		/* просто задержка как в документации */
		case 2:										// задержка после чтения теневых регистров
		phase++;
		break;
		/* установка стартового адреса рабочего регистра приемника */
		case 3:
		TWI_stat.TWI_state=TWI_BUSY;					// захват интерфейса TWI.												 
		TWI_stat.TWI_msgSize=3;							// leng message
		TWI_stat.TWI_buf[0]=SLA_W;						// команда+адрес девайса.
		TWI_stat.TWI_buf[1]=0;							// high adress RW_PR
		TWI_stat.TWI_buf[2]=RW_PR;						// low adress RW_PR=0x10
		phase++;
		TWI_Start();									// Запуск механизма обработчика прерываний TWI.
		break;
		/* чтение регистра-указателя (4х байтного) */
		case 4:
		TWI_stat.TWI_state=TWI_BUSY;					// захват интерфейса TWI.												 
		TWI_stat.TWI_msgSize=5;							// leng message
		TWI_stat.TWI_buf[0]=SLA_R;						// команда+адрес девайса.
		phase++;
		TWI_Start();									// Запуск механизма обработчика прерываний TWI.
		break;
		/* формирование регистра-указателя */
		case 5:
		w = (unsigned long)TWI_stat.TWI_buf[4];
		pr32=(unsigned long)TWI_stat.TWI_buf[3];
		w |= (pr32<<8);
		pr32=(unsigned long)TWI_stat.TWI_buf[2];
		w |= (pr32<<16);
		pr32=(unsigned long)TWI_stat.TWI_buf[1];
		w |= (pr32<<24);
		TWI_stat.TWI_state=TWI_BUSY;
		/* установка следующего теневого регистра приемника */
		pr16=SRR_PR;
		TWI_stat.TWI_msgSize=3;							// leng message
		TWI_stat.TWI_buf[0]=SLA_W;						// команда+адрес девайса.
		TWI_stat.TWI_buf[1]=pr16>>8;					// high adress SRW_PR
		TWI_stat.TWI_buf[2]=pr16;						// low adress SRR_PR=0x1E1
		phase++;
		TWI_Start();									// Запуск механизма обработчика прерываний TWI.
		break;
		/* чтение теневого регистра (использоваться не будет) */
		case 6:
		TWI_stat.TWI_state=TWI_BUSY;												 
		TWI_stat.TWI_msgSize=2;							// leng message
		TWI_stat.TWI_buf[0]=SLA_R;						// команда+адрес девайса.
		phase++;
		TWI_Start();									// Запуск механизма обработчика прерываний TWI.		
		break;
		/* просто задержка как в документации */
		case 7:										// задержка после чтения теневых регистров
		phase++;
		break;
		case 8:
		TWI_stat.TWI_state=TWI_BUSY;												 
		/* установка стартового адреса рабочего регистра приемника */
		TWI_stat.TWI_msgSize=3;							// leng message
		TWI_stat.TWI_buf[0]=SLA_W;						// команда+адрес девайса.
		TWI_stat.TWI_buf[1]=0;							// high adress RW_PR
		TWI_stat.TWI_buf[2]=RR_PR;						// low adress RW_PR=0x10
		phase++;
		TWI_Start();									// Запуск механизма обработчика прерываний TWI.		
		break;
		/* чтение второго регистра-указателя (4х байтного) */
		case 9:
		TWI_stat.TWI_state=TWI_BUSY;												 
		TWI_stat.TWI_msgSize=5;							// leng message
		TWI_stat.TWI_buf[0]=SLA_R;						// команда+адрес девайса.
		phase++;
		TWI_Start();									// Запуск механизма обработчика прерываний TWI.
		break;
		/* определение наличия сообщения в памяти модуля */
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
		else busy = FULLSIZE + adif;					//размер занятой сообщением памяти
		
		if(busy == 0)									//нет сообщения в приемном буфере модуля WIZnet
		{
			result = 1;									//конец приема, пакета нет.
			phase = 0;
			break;
		}
		lengUDP = busy;
		if(busy > ETH_RC_SIZE) error_size=1;			// фиксация ошибки длины входного пакета
		else error_size=0;
		/* подготовка к чтению пакета */
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
		/* запуск чтения пакета */
		case 11:
		if(error_size == 0) i=(unsigned char)lengUDP;
		else i=ETH_RC_SIZE;
		TWI_stat.TWI_state=TWI_BUSY;
		TWI_stat.TWI_msgSize=i+1;						// leng message
		TWI_stat.TWI_buf[0]=SLA_R;						// команда+адрес девайса.
		r += i;											// модификация указателя начала буфера чтения
		phase++;
		TWI_Start();
		break;
		/* сохранение заголовка входного пакета */
		case 12:
		if(error_size == 0)							// пакет нормальной длины
		{
			for(i=0; i<lengUDP; i++)					// копирование полученного заголовка
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
		/* продолжение действий по приему очень длинного пакета */		
		case BREAD:
		pr32=RECV_DATA_BUF+(r & PMASK);			// Pointer to W3100A receive buffer
		TWI_stat.TWI_state=TWI_BUSY;
		pr16=(unsigned int)pr32;
		if(lengUDP <= ETH_RC_SIZE)					// если пакет умещается в буфер
		{
			r += lengUDP;							// предусмотрительно изменим указатель
			temp_size=(unsigned char)lengUDP;
			lengUDP=0;								// обнулим счетчик
		}
		else
		{
			r+=ETH_RC_SIZE;							// предусмотрительно изменим указатель
			temp_size=ETH_RC_SIZE;
			lengUDP-=ETH_RC_SIZE;					// уменьшим счетчик
		}
		/* установка нового адреса чтения */
		TWI_stat.TWI_msgSize=3;						// leng message
		TWI_stat.TWI_buf[0]=SLA_W;					// команда+адрес девайса.
		TWI_stat.TWI_buf[1]=(pr16>>8);				// high adress
		TWI_stat.TWI_buf[2]=pr16;					// low adress
		phase=CREAD;
		TWI_Start();
		break;
		/* вычитывание пакета или его части */
		case CREAD:
		if(lengUDP) phase=BREAD;
		else phase=PMODIFY;
		TWI_stat.TWI_state=TWI_BUSY;
		TWI_stat.TWI_msgSize=temp_size+1;			// leng message
		TWI_stat.TWI_buf[0]=SLA_R;					// команда+адрес девайса.
		TWI_Start();
		break;
		/* модификация указателя */
		case PMODIFY:
		TWI_stat.TWI_state=TWI_BUSY;
		TWI_stat.TWI_msgSize=7;						// leng message
		TWI_stat.TWI_buf[0]=SLA_W;					// команда+адрес девайса.
		TWI_stat.TWI_buf[1]=0;						// high adress
		TWI_stat.TWI_buf[2]=RR_PR;					// low adress
		TWI_stat.TWI_buf[3]=(r>>24);
		TWI_stat.TWI_buf[4]=(r>>16);
		TWI_stat.TWI_buf[5]=(r>>8);
		TWI_stat.TWI_buf[6]=(r);
		phase=FIXREAD;
		TWI_Start();
		break;
		/* выдача команды чтения пакета */
		case FIXREAD:
		TWI_stat.TWI_state=TWI_BUSY;												 
		TWI_stat.TWI_msgSize=4;						// leng message
		TWI_stat.TWI_buf[0]=SLA_W;	
		TWI_stat.TWI_buf[1]=0;						// high adress C0_CR
		TWI_stat.TWI_buf[2]=0;						// low adress C0_CR=0x00
		TWI_stat.TWI_buf[3]=CRECV;					// команда завершения приема пакета
		result = 1;
		time_out[1] = MC33;							// init
		RED_ON();									// включить индикатор принятого пакета
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

