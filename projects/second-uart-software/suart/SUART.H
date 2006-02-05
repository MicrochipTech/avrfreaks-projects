#ifdef _AVR_IOM8_H_
#define SRX     PB0     // ICP on Mega8
#define SRXPIN  PINB

#define STX     PB1     // OC1A on Mega8
#define STXDDR  DDRB
#else
#error
#error Please add the defines:
#error
#error SRX, SRXPIN, STX, STXDDR
#error
#error for the new target !
#error
#endif

extern volatile u8 srx_done;

#define kbhit()	(srx_done)	// true if byte received

void suart_init( void );
void sputchar( u8 val );
u8 sgetchar( void );
void sputs( u8 *txt );

