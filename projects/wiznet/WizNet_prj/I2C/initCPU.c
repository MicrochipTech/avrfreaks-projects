/**********************************************************************************
  Основные процессорные установки и установки для указателей буфера модуля ethernet
**********************************************************************************/

#include <avr/io.h>
#include <avr/wdt.h>
#include <compat/twi.h>
#include "headers/MACRO.H"
#include "headers/defConstant.h"
#include "headers/defStruct.h"
/****************************************************************/
#define TWI_TWBR	10			// TWI Bit rate Register setting. See Application note for detailed  information on setting this value.
/****************************************************************/
extern struct	TWI_status		TWI_stat;		// состояние интерфейса TWI
extern struct Descript		ETH_dsc;		// рабочая структура функции обслуживания модуля Ethernet
extern unsigned char time_out[NT];			// массив переменных для организации задержек

void initCPU(void)
{
	DDRA = 0x07;
	DDRC = 0x03; 	
	DDRD = 0x01;
	
	PORTD = 0xFF;
	
	TCCR0A = 0x05;				// установка прескайлера 1024 для таймера 0.
	TIMSK0 = 1;					// Разрешение прерывания по переполнению
	TCNT0 = 0;
	
	
	TWAR &= ~(1<<TWGCE);					// установка запрета анализа входного адреса для режима только мастер
	TWSR &= ~((1<<TWPS0) | (1<<TWPS1));	// установка предделителя скорости I2C
	TWBR=TWI_TWBR;							// установка битовой скорости 6MHz/(16+2(TWBR)*4^TWPS)=166.666kHz, где TWPS=0,TWBR=10
	TWCR |= (1<<TWEN);						// установка альтернативной функции I2C, прерывание пока запрещено
	TWI_stat.TWI_state=TWI_FREE;
	ETH_dsc.phase=0;						// признак-указатель на инициализацию модуля Ethernet
	
	time_out[3] = BLOCK_TWI_TIME;			// инициализация таймера ожидания завершения цикла TWI
	resetETH();
	
	wdt_enable(WDTO_1S);		// установка WDT на время 1с
	wdt_reset();				// сброс внутреннего сторожевого таймера
}

