
/****************************************************************************
  Определение макросов для улучшения читабельности кода программы
****************************************************************************/
#define nop()				asm("nop"::)		// нет орерации

#define INT_enable()		asm("sei"::)		// разрешение прерываний глобальное
#define INT_disable()		asm("cli"::)		// запрещение прерываний глобальное

#define	GREEN_ON()			PORTA |= 0x02		// включение зеленого светодиода
#define	GREEN_OFF()			PORTA &= ~0x02		// выключение зеленого светодиода
#define	GREEN_TOG()			PORTA ^= 0x02		// переключение зеленого светодиода

#define	RED_ON()			PORTA |= 0x04		// включение красного светодиода
#define	RED_OFF()			PORTA &= ~0x04		// выключение красного светодиода
#define	RED_TOG()			PORTA ^= 0x04		// переключение зкрасного светодиода

#define	resetETH()			PORTC = 0x02		//сброс модуля Ethernet
#define	releaseETH()		PORTC = 0x01		//освобождение от сброса модуля Ethernet

#define	setTimer0(x)		TCNT0 = (x)			// запись кода в нулевой таймер для форирования задержки
#define TWI_Start()			TWCR=0xA5			//(1<<TWEN)|(1<<TWIE)|(1<<TWINT)|(1<<TWSTA) TWI Interface enabled.
