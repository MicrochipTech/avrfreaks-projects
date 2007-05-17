#include <avr/io.h>
#include "Define/MACRO.H"
#include "Define/defConstant.h"
#include "Define/defStruct.h"
/****************************************************************/
/*
	Проверка соединения с компьютером по шине USB
	изменение режима мигания светодиода
*/
/****************************************************************/
extern struct timer time;				// структурa для организации мигания индикатора режима работы
extern unsigned char time_out[NT];	// массив для организации задержек
/****************************************************************/
/****************************************************************/
void resetState(unsigned char s);		//сброс установок фозы работы загрузчика
/****************************************************************/
#define NCONNECT	0
#define CONNECT		1

void connectTest(void)
{
	static unsigned char faseWork=0;
	unsigned char i;
	
	i=getUSBstat();								//чтение состояния вывода контроля соединения с USB
	
	switch(faseWork)
	{
		/*-----------------------------*/
		case NCONNECT:							//состояние разъединения
		if(i & 0x01) time.init = DEL1000;		//если линия осталась заряженной, то нет соединения
		else
		{
			time.init = DEL500;					//установка мигания на 0.5с
			faseWork = CONNECT;
			time_out[0]=DEL1000;
			resetState(1);						//предустановка для работы загрузчика с USB
		}
		break;
		/*-----------------------------*/
		case CONNECT:							//состояние coединения
		if(i & 0x01)							//если линия осталась заряженной, то нет соединения
		{
			time.init = DEL1000;				//установка мигания на 1с
			faseWork = NCONNECT;
			resetState(0);						//полный отбой работы загрузчика с USB
		}
		else time.init = DEL500;				//установка мигания на 0.5с
		break;
		/*-----------------------------*/
		default:
		time.init = DEL1000;
		faseWork = NCONNECT;
		resetState(0);							//полный отбой работы загрузчика с USB
		break;		
	}
}

