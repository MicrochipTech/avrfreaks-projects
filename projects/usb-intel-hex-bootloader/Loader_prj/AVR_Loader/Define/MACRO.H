
/****************************************************************************
  Определение макросов для улучшения читабельности кода программы
****************************************************************************/
#define nop()				asm("nop");			// не орерации
#define enable()			asm("sei");			// разрешение прерываний глобальное
#define disable()			asm("cli");			// запрещение прерываний глобальное
#define release()			TCCR0A=0; PORTE &= ~0x04; asm("jmp 0");	// останов таймера,переход на вектор сброса


#define	GREEN_ON()			PORTE |= 0x08		// включение зеленого светодиода
#define	GREEN_OFF()			PORTE &= ~0x08		// выключение зеленого светодиода
#define	GREEN_TOG()			PORTE ^= 0x08		// переключение зеленого светодиода

#define	RED_ON()			PORTE |= 0x04		// включение красного светодиода
#define	RED_OFF()			PORTE &= ~0x04		// выключение красного светодиода
#define	RED_TOG()			PORTE ^= 0x04		// переключение зкрасного светодиода

#define	EWDT_reset()		PORTD ^= 0x80		// сброс внешнего сторожевого таймера

#define sendUSBdata(x)		*((volatile unsigned char*)0xfff4) = (x)	//регистр для записи данных в буфер USB
#define getUSBdata()		*((volatile unsigned char*)0xfff4)			//регистр для чтения данных из буфер USB (праводопустимое выражение)
#define getUSBstat()		*((volatile unsigned char*)0xfff5)			//регистр для чтения битов состояния FTDI USB драйвера
#define getEXTdata()		*((volatile unsigned char*)0xfff6)			//регистр для чтения состояния переключателей и некоторых внешних сигналов (d2=s2 d1=s1 d0=s0)

