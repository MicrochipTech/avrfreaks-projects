#include <avr/io.h>
#include "Define/MACRO.H"
#include "Define/defConstant.h"
#include "Define/defStruct.h"
/****************************************************************/
/*
	Поддержка диалога с компьютером.
	Ответы на запросы "ASK" "GET" ("OK" "LET")
	Фаза диалога поддержиается после обнаружения подключения к шине USB - 
	на вопрос "ASK" следует ответ "OK".
	После получения запроса "GET" следует ответ "LET" и этот диалог заканчивается.
	Далее предполагается работа обработчика входного файла.
	Возврат к фазе диалога происходит после окончания программирования
	или после любой обнаруженной ошибки исполнения.
*/
/****************************************************************/
extern struct descWork dw;			// структура - описатель фазы работы загрузчика
extern struct Request rc;				//структура запроса на обработку входного пакета от USB
extern struct Request tx;				//структура запроса на передачу пакета в сторону USB
/****************************************************************/
extern unsigned char rcBuff[USBB];	//буфер приемника
extern unsigned char txBuff[USBB];	//буфер передатчика
/****************************************************************/
void resetState(unsigned char s);

void dialog(void)
{
	if(dw.phase != 1)							//возврат по причине завершения диалоговой фазы
	{
		dw.phDi=0;
		return;
	}
	/*-----------------------------*/
	switch(dw.phDi)
	{
		case 0:	//прием символа A или G
		if(rc.req)
		{
			rc.req=0;
			if(rcBuff[0] == 'A')
			{
				dw.phDi=1;			//переход к попытке принять ASK
			}
			if(rcBuff[0] == 'G')
			{
				dw.phDi=4;			//переход к попытке принять GET
			}
		}
		break;
	/*-----------------------------*/	
		case 1:	//прием символа S
		if(rc.req)
		{
			rc.req=0;
			if(rcBuff[0] == 'S')
			{
				dw.phDi=2;
				break;
			}
			else dw.phDi=0;
		}
		break;
	/*-----------------------------*/
		case 2:	//прием символа K
		if(rc.req)
		{
			rc.req=0;
			if(rcBuff[0] == 'K')
			{
				dw.phDi=3;
				break;
			}
			else dw.phDi=0;
		}
		break;
	/*-----------------------------*/
		case 3:	//передача текста "OK"
		if(tx.req == 0)
		{
			tx.req=1;
			tx.len=2;
			txBuff[0]='O';
			txBuff[1]='K';
			dw.phDi=0;
		}
		break;
	/*-----------------------------*/
		case 4:	//прием символа E
		if(rc.req)
		{
			rc.req=0;
			if(rcBuff[0] == 'E')
			{
				dw.phDi=5;
				break;
			}
			else dw.phDi=0;
		}
		break;
	/*-----------------------------*/
		case 5:	//прием символа T
		if(rc.req)
		{
			rc.req=0;
			if(rcBuff[0] == 'T')
			{
				dw.phDi=6;
				break;
			}
			else dw.phDi=0;
		}
		break;
	/*-----------------------------*/
		case 6:	//передача текста "LET"
		if(tx.req == 0)
		{
			tx.req=1;
			tx.len=3;
			txBuff[0]='L';
			txBuff[1]='E';
			txBuff[2]='T';
			resetState(2);			//!!!Важнейший момент отказа от фазы диалога и переход к приему файла прошивки
		}
		break;
	/*-----------------------------*/
		default:
		dw.phDi=0;
		break;
	}
}

