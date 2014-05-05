/* defines for backward portability M(32/16/8)8 vs M8. */

/* allow some inter-device compatibility */
#if !defined TCCR0 && defined TCCR0B
#define TCCR0   TCCR0B
#endif
#if !defined TIFR && defined TIFR0
#define TIFR    TIFR0
#endif
/*------------------------------------- */

#define UDRIE0	UDRIE
#define UDR0	UDR
#define UDRE0	UDRE

#define UBRR0H	UBRRH
#define UBRR0L	UBRRL

#define RXEN0	RXEN
#define	TXEN0	TXEN

#define UCSR0A	UCSRA
#define UCSR0B	UCSRB

#define	TIMSK0	TIMSK
/* check this! */
#define	TCCR0B	TCCR0

#define	RXCIE0	RXCIE

#define TIFR1	TIFR
#define TIMSK1	TIMSK

#define U2X0	U2X
#define UCSR0A	UCSRA
#define UCSR0B	UCSRB
#define	UCSR0C	UCSRC
#define	UCSZ00	UCSZ0
#define UCSZ01	UCSZ1
#define	UCSZ02	UCSZ2
