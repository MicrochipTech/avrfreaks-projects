#ifndef __timer_h_included__
#define __timer_h_included__

#include <avr/io.h>
#include "inframerah.h"

#ifndef uchar
	#define uchar						unsigned char
#endif

#define TIMEOUT_USART					68

#define aktifkanSinyal38kHz()			TCCR2= 0x19
#define matikanSinyal38kHz()			TCCR2= 0x00

#define matikanInframerah()				PORTD|= (1<<1)

volatile uchar counterTimeout;

void inisialisasiSinyal38kHz(void);
void inisialisasiTimerTimeout(void);
void matikanTimerTimeout(void);

#endif
