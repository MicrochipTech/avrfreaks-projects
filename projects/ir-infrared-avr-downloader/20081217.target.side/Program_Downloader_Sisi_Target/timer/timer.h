#ifndef __timer_h_included__
#define __timer_h_included__

#include <mega162.h>

#define AKTIFKAN_SINYAL_CARRIER()	TCCR2= 0x19
#define MATIKAN_SINYAL_CARRIER()	TCCR2= 0x00
#define AKTIFKAN_SCHEDULLING()      TCCR1B= 0x02
#define MATIKAN_SCHEDULLING()       TCCR1B= 0x00

#define USART_TIMEOUT			    68 

void inisialisasiTimer(void);

#endif
