
/****************************************************************************
  Определение макросов для улучшения читабельности кода программы
****************************************************************************/
#define nop()				asm("nop"::)		// нет орерации
#define INT_enable()		asm("sei"::)		// разрешение прерываний глобальное
#define INT_disable()		asm("cli"::)		// запрещение прерываний глобальное

#define	GREEN_ON()			PORTE |= 0x08		// включение зеленого светодиода
#define	GREEN_OFF()			PORTE &= ~0x08		// выключение зеленого светодиода
#define	GREEN_TOG()			PORTE ^= 0x08		// переключение зеленого светодиода
#define	RED_ON()			PORTE |= 0x04		// включение красного светодиода
#define	RED_OFF()			PORTE &= ~0x04		// выключение красного светодиода
#define	RED_TOG()			PORTE ^= 0x04		// переключение зкрасного светодиода

#define	WDText()			PORTD ^= 0x80		// сброс внешнего сторожевого таймера

#define	getSens()			*((volatile unsigned char*)0xfff6)			//чтение d6=reset_ethernet
#define	resEthernet()		*((volatile unsigned char*)0xfff6)=0x40		//установка сигнала сброс на модуль Ethernet
#define	setEthernet()		*((volatile unsigned char*)0xfff6)=0x00		//снятие сигнала сброс на модуль Ethernet
