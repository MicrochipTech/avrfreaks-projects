#ifndef _suart_h_
#define _suart_h_

			// size must be in range 2 .. 256
#define STX_SIZE	20
#define	SRX_SIZE	100

#define	uputs(x)	uputs_((u8*)(x))	// avoid char warning


void suart_init( void );
void uputchar( u8 c );			// send byte
void uputs_( u8 *s );			// send string from SRAM
u8 kbhit( void );			// check incoming data
u8 ugetchar( void );			// get byte


#endif
