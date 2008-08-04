#ifndef	_i2csniff_h_
#define	_i2csniff_h_

#define	START	's'
#define	ACK	'a'
#define	NACK	'n'
#define	STOP	'p'


void init_i2c( void );
void putnibble( u8 j );
void i2c_sniff( void );

#endif
