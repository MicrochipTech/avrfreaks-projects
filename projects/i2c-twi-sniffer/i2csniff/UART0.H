#ifndef _uart0_h_
#define _uart0_h_

					// size must be in range 2 .. 256
#define TX0_SIZE	256		// usable: TX0_SIZE - 1 (1 .. 255)

#define	uputs0(x)	uputs0_((u8*)(x))	// avoid char warning


void uputchar0( u8 c );			// send byte
void uputs0_( u8 *s );			// send string from SRAM
void init_uart0( void );


#endif
