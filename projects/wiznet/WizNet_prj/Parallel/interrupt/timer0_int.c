#include <avr/io.h>
#include <avr/interrupt.h>            
#include "headers/defConstant.h"
#include "headers/defStruct.h"
/****************************************************************************
	Обслуживание прерывания переполнения нулевого таймера
	Предделитель установлен на 1024. Период таймера = 236
	дает частоту прерываний - 25.04 Гц (40 мс период)
****************************************************************************/
#define TIMER0START	22

extern unsigned char time_out[NT];				// массив переменных для организации задержек

SIGNAL(SIG_OVERFLOW0)
{
	unsigned char i;
	
	TCNT0 = TIMER0START;
	for(i=0; i < NT; i++)
	{
		if(time_out[i] > 0) time_out[i]--;		// декремент переменных до предела
	}
}
