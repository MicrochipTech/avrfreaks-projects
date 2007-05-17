/********************************************************************************************
  Поэтапная работа с модулем ethernet
  Функция проводит полную инициализацию модуля Ethernet и после этого
  производит последовательные вызовы функции чтения входных сообщений от модуля
  и выдачу модулю данных на передачу, если таковые появляются.
  Отсутствует контроль правильности функционирования модуля Ethernet, 
  поэтому модуль никогда не пересбрасываеся аппаратно после инициализации.
  Если в будущем появится критерий неустранимого сбоя модуля,
  то сброс модуля и повторная инициализация может быть проведена
  простой установкой в 1 запроса инициализации модуля из FLASH.
	Порядок инициализации и работы с модулем WIZnet:
	1. аппаратный сброс модуля
	2. задержка ~20мс на очухивание модуля
	3. программный сброс модуля
	4. ожидание завершения сброса с контролем времени
	5. установка сетевых параметров модуля - sys_init
	6. ожидание завершения установки с контролем времени исполнения
	7. установка параметров сокета - sock_init
	8. ожидание завершения установки с контролем времени исполнения
	_______________________________________________________________________________________
	Модуль имеет особенность. После сброса, первый отправленный пакет по непонятной причине
	оказывается искаженным.
	_______________________________________________________________________________________
	Работа с модулем следующая:
		Если программа пользователя имеет данные для передачи в сеть Ethernet, то она должна 
	проверить элемент структуры ETH_Send.req на равенство нулю. Если параметр не 0,
	значит предыдущий пакет еще не передан и сделать ничего нельзя. Если параметр обнулен,
	то пользовательская программа может загрузить данные для очередного UDP пакета в буфер
	передачи ethTRbuff[ETH_TR_SIZE], начиная с нулевого индекса. Далее в управляющую
	структуру ETH_Send нужо занести длину загруженного массива ETH_Send.leng и установить в 1 
	запрос на передачу ETH_Send.req. Далее уже функция ETH_work() позаботится о передаче
	пакета с сеть и сбросе запроса передачи ETH_Send.req. При необходимости отправить пакет по 
	новому IP назначения, нужно, до установки запроса передачи пакета, записать навый IP destination
	в массив UserIPdestination[4]; начиная со старшей цифры IP, и установить запрос на смену IP в
	структуре ETH_stat - элемент ETH_stat.initRequest=3. (Установка ETH_stat.initRequest=1 приведет к
	аппаратному сбросу модуля WIZnet и установке сетевых параметров по умолчанию. Установка 
	ETH_stat.initRequest=2 приводит к аппаратному сбросу и установке пользовательских сетевых
	настроек из массивов UNet_Settings[23]; USocket_Settings[19];).
		Прорамма пользователя дожна заботится об освобождении приемного буфера модуля, для
	создания условия приема следующего пакета. Для этого необходимо проверять состояние запроса
	обработки принятого пакета отраженного в элементе структуры ETH_Rec - ETH_Rec.req.
	При обнаружении, что признак запроса обработки не равен нулю, программа должна взять в
	элементе ETH_Rec.leng значение длины пришедшего в приемный буфер ethRCbuff[ETH_RC_SIZE] пакета, 
	обработать пакет, или по крайней мере переместить его из приемного буфера в безопасное место, 
	и сбросить признак запроса обработки ETH_Rec.req. После этого функция ETH_work() получит возможность
	заполнить буфер приемника данными от следующего принятого пакета.	
********************************************************************************************/
#include <avr/io.h>
#include "headers/defConstant.h"
#include "headers/defStruct.h"
#include "headers/MACRO.H"
#include "headers/W3100A.H"

#define FLASH_SET	1	//признак-требование установки модуля из FLASH через HW reset
#define USER_SET	2	//признак-требование установки модуля командой USB через HW reset
#define UIPDES_SET	3	//признак-требование установки destination IP командой USB

#define CNRI	0x81	//установка Indirect Bus mode с автоинкрементом
 
void 	dataTransfer(unsigned char);	//функция передачи данных CPU-WIZnet 0-чтение из модуля, 1-запись.
void 	setIPdest(void);				//установка нового, пользовательского IP destination

extern struct		TD					Tdata;

extern struct		ETH_status	ETH_stat;
extern struct		Request		ETH_Send;
extern struct		Request		ETH_Rec;

extern const unsigned char Net_Settings[23];	//массив сетевых установок по умолчанию
extern const unsigned char Socket_Settings[19];	//массив установок сокета по умолчанию
extern unsigned char UNet_Settings[23];			//массив пользовательских сетевых установок
extern unsigned char USocket_Settings[19];		//массив пользовательских установок сокета
extern unsigned char UserIPdestination[4];		//новый destination IP модуля
extern unsigned char TPoint_Settings[4];
extern unsigned char RPoint_Settings[4];

extern volatile unsigned char time_out[NT];		// массив переменных для организации задержек

extern unsigned char ethTRbuff[ETH_TR_SIZE];		// ETH_TR_SIZE=24 буфер для сохранения данных которые приложение приготовило для передачи по ethernet
extern unsigned char ethRCbuff[ETH_RC_SIZE];		// ETH_RC_SIZE=32 буфер для сохранения данных которые пришли по сети ethernet должен быть на 8 байт больше
													// буфера на передачу, так как включает заголовок: длина-2, ip-4, port-2.(длина всего пакета вместе с заголовком)

unsigned char* const ADR_IDM_OR =	(unsigned char*)0xfff0;	//адрес регистра управления indirect Bus mode
unsigned char* const ADR_IDM_AR0 =	(unsigned char*)0xfff1;	//indirect bus I/F mode address0 register
unsigned char* const ADR_IDM_AR1 =	(unsigned char*)0xfff2;	//indirect bus I/F mode address1 register
unsigned char* const ADR_IDM_DR = 	(unsigned char*)0xfff3;	//indirect bus I/F mode data register


void ETH_work(void)
{
	static unsigned char *pNet;
	static unsigned char *pSoc;

	volatile unsigned int address;
	volatile unsigned char halfAdr;
	unsigned char prime2prt[8];	//буфер для временного хранения 2-х указателей памяти модуля WIZnet в разбивку по байтам
	unsigned long	w,r,pr32;		//указатели адреса в буфере WIZnet
	int mw,mr,adif,free,busy;
	unsigned int dop;
	
	switch(ETH_stat.initRequest)	//обработка внешнего запроса на инициализацию модуля
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
	ETH_stat.initRequest=0;			// сброс запроса установки инициализации модуля.
	
	switch(ETH_stat.phase)
	{
		case 0:					//модуль еще не прошел аппаратный сброс
		time_out[2]=50;				//установка таймера лимита времени инициализации (2с)
		resEthernet();				//активизация сигнала сброса для модуля WIZnet
		ETH_stat.phase = 10;		//установка признака прохождения аппаратного сброса
		break;
		/*||||||||||||||||||||||||||*/
		case 1:					//выполнение программного сброса
		*ADR_IDM_OR = CNRI;			//установка режима indirect bus I/F, Big-endian, auto-increment.
		*ADR_IDM_AR0 = 0;			//старшая часть адреса
		*ADR_IDM_AR1 = 0;			//младшая часть адреса
		*ADR_IDM_DR = CSW_RESET;	//S/W Reset
		ETH_stat.phase = 2;
		break;
		/*||||||||||||||||||||||||||*/
		case 2:					//ожидание выполнения программного сброса
		if(time_out[2]== 0) ETH_stat.phase = 0;	//повтор сброса по истечению лимита времени
		*ADR_IDM_AR0 = 0;			//старшая часть адреса
		*ADR_IDM_AR1 = 0;			//младшая часть адреса
		if(!(*ADR_IDM_DR & CSW_RESET)) ETH_stat.phase = 3;
		break;
		/*||||||||||||||||||||||||||*/
		case 3:			//выполнение сетевых установок
		Tdata.pIData = (unsigned char*)NET_SETTINGS;
		Tdata.pEData = pNet;
		Tdata.lengs = 8;
		dataTransfer(1);	//вызов функции записи данных в модуль
		/* пропускаем установку пользовательского MAC адреса (6 байт) */
		Tdata.pIData = (unsigned char*)SRC_IP;
		Tdata.pEData = pNet+14;
		Tdata.lengs = 9;
		dataTransfer(1);	//вызов функции записи данных в модуль
		
		*ADR_IDM_AR0 = 0;			//старшая часть адреса
		*ADR_IDM_AR1 = 0;			//младшая часть адреса
		*ADR_IDM_DR = CSYS_INIT;	//Sys_Init
		ETH_stat.phase = 4;
		break;
		/*||||||||||||||||||||||||||*/
		case 4:			//ожидание выполнения сетевых установок
		if(time_out[2]== 0) ETH_stat.phase = 0;	//повтор сброса
		*ADR_IDM_AR0 = 0;			//старшая часть адреса
		*ADR_IDM_AR1 = 0;			//младшая часть адреса
		if(!(*ADR_IDM_DR & CSYS_INIT)) ETH_stat.phase = 5;
		break;
		/*||||||||||||||||||||||||||*/
		case 5:			//выполнение установок сокета
		Tdata.pIData = (unsigned char*)OPT_PROTOCOL;
		Tdata.pEData = pSoc;
		Tdata.lengs = 19;
		dataTransfer(1);	//вызов функции записи данных в модуль
		
		*ADR_IDM_AR0 = 0;			//старшая часть адреса
		*ADR_IDM_AR1 = 0;			//младшая часть адреса
		*ADR_IDM_DR = CSOCK_INIT;	//CSOCK_INIT
		ETH_stat.phase = 6;
		break;
		/*||||||||||||||||||||||||||*/
		case 6:			//ожидание выполнения установок сокета
		if(time_out[2]== 0) ETH_stat.phase = 0;	//повтор сброса
		*ADR_IDM_AR0 = 0;			//старшая часть адреса
		*ADR_IDM_AR1 = 0;			//младшая часть адреса
		if(!(*ADR_IDM_DR & CSOCK_INIT)) ETH_stat.phase = 7;
		break;
		/*||||||||||||||||||||||||||*/
		case 7:			//установка указателей адреса в памяти модуля
		Tdata.pIData = (unsigned char*)RW_PR;
		Tdata.pEData = (unsigned char*)RPoint_Settings;
		Tdata.lengs = 4;
		dataTransfer(1);	//вызов функции записи данных в модуль
		
		Tdata.pIData = (unsigned char*)RR_PR;
		Tdata.pEData = (unsigned char*)RPoint_Settings;
		Tdata.lengs = 4;
		dataTransfer(1);	//вызов функции записи данных в модуль
		
		Tdata.pIData = (unsigned char*)TW_PR;
		Tdata.pEData = (unsigned char*)TPoint_Settings;
		Tdata.lengs = 4;
		dataTransfer(1);	//вызов функции записи данных в модуль
		
		Tdata.pIData = (unsigned char*)TR_PR;
		Tdata.pEData = (unsigned char*)TPoint_Settings;
		Tdata.lengs = 4;
		dataTransfer(1);	//вызов функции записи данных в модуль
		
		ETH_stat.phase = 8;	
		break;
		/*||||||||||||||||||||||||||*/
		case 8:			//прием пакета
		if(ETH_Rec.req)	//признак не обработанности предыдущего пакета
		{
			ETH_stat.phase = 9;	//переход к передаче пакета
			break;
		}
		/*.........................*/
		address = SRW_PR;
		halfAdr = (unsigned char)(address >> 8);
		*ADR_IDM_AR0 = halfAdr;		//старшая часть адреса
		halfAdr = (unsigned char)(address);
		*ADR_IDM_AR1 = halfAdr;		//младшая часть адреса
		halfAdr = *ADR_IDM_DR;		//чтение теневых регистров
		/*.........................*/
		Tdata.pIData = (unsigned char*)RW_PR;		//adress RW_PR=0x10
		Tdata.pEData = (unsigned char*)prime2prt;
		Tdata.lengs = 4;
		dataTransfer(0);	//вызов функции чтения указателя приемника RW_PR из модуля
		/*.........................*/
		address = SRR_PR;
		halfAdr = (unsigned char)(address >> 8);
		*ADR_IDM_AR0 = halfAdr;		//старшая часть адреса
		halfAdr = (unsigned char)(address);
		*ADR_IDM_AR1 = halfAdr;		//младшая часть адреса
		halfAdr = *ADR_IDM_DR;		//чтение теневых регистров
		/*.........................*/
		Tdata.pIData = (unsigned char*)RR_PR;		//adress RR_PR
		Tdata.pEData = (unsigned char*)(prime2prt+4);
		Tdata.lengs = 4;
		dataTransfer(0);	//вызов функции чтения указателя приемника RR_PR из модуля
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
		else busy = FULLSIZE + adif;	//размер занятой сообщением памяти
		if(busy == 0)					//нет сообщения в приемном буфере модуля WIZnet
		{
			ETH_stat.phase = 9; 	//уходим на передачу
			break;
		}
		/*.........................*/
		if(busy > ETH_RC_SIZE)		//размер пакета превышает размер входного буфера
		{
			dop = (unsigned int)pr32;
			*ADR_IDM_AR0 = (unsigned char)(dop >> 8);	//старшая часть адреса
			*ADR_IDM_AR1 = (unsigned char)(dop);			//младшая часть адреса
			for(dop=0; dop<busy; dop++) halfAdr = *ADR_IDM_DR;	//чтение
		}
		else
		{
			dop = (unsigned int)pr32;
			Tdata.pIData = (unsigned char*)dop;
			Tdata.pEData = (unsigned char*)ethRCbuff;
			Tdata.lengs = (unsigned char)busy;
			dataTransfer(0);			//вызов функции чтения данных из модуля
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
		ETH_stat.phase = 9; 		//уходим на передачу
		break;
		/*||||||||||||||||||||||||||*/
		case 9:					//передача пакета
		if(ETH_Send.req == 0)		// Если нет запроса к модулю ethernet на передачу
		{
			ETH_stat.phase = 8; 	//уходим на прием
			break;
		}
		if(ETH_Send.leng > ETH_TR_SIZE)	//неверная длина сообщения
		{
			ETH_Send.req = 0;		//сброс запроса передачи
			ETH_stat.phase = 8; 	//уходим на прием
			break;
		}
		*ADR_IDM_AR0 = 0;			//старшая часть адреса
		*ADR_IDM_AR1 = 0;			//младшая часть адреса
		if(*ADR_IDM_DR & CSOCK_INIT) //новый IP destination еще не установился
		{
			ETH_stat.phase = 8; 	//уходим на прием
			break;
		}
		/*.........................*/
		address = STW_PR;
		halfAdr = (unsigned char)(address >> 8);
		*ADR_IDM_AR0 = halfAdr;		//старшая часть адреса
		halfAdr = (unsigned char)(address);
		*ADR_IDM_AR1 = halfAdr;		//младшая часть адреса
		halfAdr = *ADR_IDM_DR;		//чтение теневых регистров
		/*.........................*/
		Tdata.pIData = (unsigned char*)TW_PR;		//adress TW_PR=0x40
		Tdata.pEData = (unsigned char*)prime2prt;
		Tdata.lengs = 4;
		dataTransfer(0);	//вызов функции чтения указателей передатчика TW_PR из модуля
		/*.........................*/
		address = STR_PR;
		halfAdr = (unsigned char)(address >> 8);
		*ADR_IDM_AR0 = halfAdr;		//старшая часть адреса
		halfAdr = (unsigned char)(address);
		*ADR_IDM_AR1 = halfAdr;		//младшая часть адреса
		halfAdr = *ADR_IDM_DR;		//чтение теневых регистров
		/*.........................*/
		Tdata.pIData = (unsigned char*)TR_PR;		//adress TW_PR=0x44
		Tdata.pEData = (unsigned char*)prime2prt+4;
		Tdata.lengs = 4;
		dataTransfer(0);	//вызов функции чтения указателей передатчика TR_PR из модуля
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
		else free = -adif;			//размер свободной памяти
		if(free < ETH_Send.leng)	//размер сообщения превышает размер свободной памяти модуля
		{
			ETH_stat.phase = 8; 	//уходим на прием
			break;
		}
		/*.........................*/
		*ADR_IDM_AR0 = 0;			//старшая часть адреса
		*ADR_IDM_AR1 = 0;			//младшая часть адреса
		if((*ADR_IDM_DR & SSEND_OK))	//проверка завершения предыдущей команды передачи
		{
			ETH_stat.phase = 8; 	//уходим на прием
			break;
		}
		/*.........................*/
		pr32=SEND_DATA_BUF+(w & PMASK);	// Pointer to W3100A send buffer
		dop = (unsigned int)pr32;
		Tdata.pIData = (unsigned char*)dop;
		Tdata.pEData = (unsigned char*)ethTRbuff;
		Tdata.lengs = ETH_Send.leng;
		dataTransfer(1);			//вызов функции записи данных в модуль
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
		ETH_Send.req = 0;			//снять запрос на передачу
		ETH_stat.phase = 8;			//переход к приему пакета	
		break;
		/*||||||||||||||||||||||||||*/
		case 10:					//задержка сигнала сброс на модуль Wiznet
		if(time_out[2] < 48)
		{
			setEthernet();			//снятие сброса с модуля WIZnet
			ETH_stat.phase = 11;	//установка признака прохождения аппаратного сброса
		}
		break;
		/*||||||||||||||||||||||||||*/
		case 11:					//задержка после сброса
		if(time_out[2] < 46)
		{

			ETH_Send.req = 0;
			ETH_Rec.req = 0;
			ETH_stat.phase = 1;		//установка признака прохождения аппаратного сброса
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
void dataTransfer(unsigned char cod)	//функция передачи данных CPU-WIZnet 0-чтение из модуля, 1-запись.
{
	unsigned char i,part;
	unsigned int conv;
	

	conv = (unsigned int)Tdata.pIData;
	part = (unsigned char)(conv >> 8);
	*ADR_IDM_AR0 = part;
	part = (unsigned char)conv;
	*ADR_IDM_AR1 = part;
	
	if(cod)		//запись в модуль
	{
		for(i=0; i<Tdata.lengs; i++)
		{
			*ADR_IDM_DR = *(Tdata.pEData++);
		}
	}
	else		//чтение из модуля
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
void setIPdest(void)	//функция установки нового destination IP.
{
	*ADR_IDM_AR0 = 0;
	*ADR_IDM_AR1 = DST_IP;
	*ADR_IDM_DR = UserIPdestination[0];
	*ADR_IDM_DR = UserIPdestination[1];
	*ADR_IDM_DR = UserIPdestination[2];
	*ADR_IDM_DR = UserIPdestination[3];
	
	*ADR_IDM_AR0 = 0;			//старшая часть адреса
	*ADR_IDM_AR1 = 0;			//младшая часть адреса
	*ADR_IDM_DR = CSOCK_INIT;	//CSOCK_INIT
	ETH_stat.initRequest = 0;
	return;
}
/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/

