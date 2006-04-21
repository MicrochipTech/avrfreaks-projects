// routines.h


#include "inttypes.h"
#include "avr/io.h"

#define AUX_PORT	PORTD
#define AUX_PIN	PIND
#define AUX_DDR DDRD
#define SPEAKER_PIN	(1<<2)
#define RECVLED_PIN (1<<3)

#define BUF_LEN	32
#define SIG_LEN	11	
#define DATA_LEN 5
#define OFF_DELAY 200000

//#define F_CPU 16000000UL

void init_receiver(void);
char recv_serial(void);
char buf_empty(void);

void long_delay(void);
void short_delay(void);
void BEEP(void);
void BOOP(void);
void disp_status(char s);
