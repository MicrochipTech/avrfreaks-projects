

extern volatile u8 srx_done;

#define SUART_rxdone()	(srx_done)	// true if byte received

void SUART_init( void );
void SUART_putchar( u8 val );
u8 SUART_getchar( void );
void SUART_puts( u8 *txt );

