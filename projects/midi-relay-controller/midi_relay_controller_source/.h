#define MIDI_UDR	UDR
#define	MIDI_UBRRH	UBRRH
#define MIDI_UBRRL	UBRRL
#define MIDI_UCSRA	UCSRA
#define MIDI_UCSRB	UCSRB
#define MIDI_UCSRC	UCSRC
#define MIDI_UDRIE	UDRIE

// UCSRA Stuff	(RXC,TXC,UDRE,FE,DOR,UPE,U2X,MPCM)
#define MIDI_RXC	RXC
#define MIDI_TXC	TXC
#define MIDI_FE		FE		
#define MIDI_DOR	DOR
#define MIDI_PE		UPE
#define MIDI_UDRE	UDRE

// UCSRB Stuff (RXCIE,TXCIE,UDRIE,RXEN,TXEN,UCSZ2,RXB8,TXB8)
#define MIDI_RXCIE	RXCIE
#define MIDI_TXCIE	TXCIE
#define MIDI_UDRIE	UDRIE
#define MIDI_RXEN	RXEN
#define MIDI_TXEN	TXEN	

// UCSRC Stuff (URSEL,UMSEL,UPM1,UPM0,USBS,UCSZ1,UCSZ0,UCPOL)
#define MIDI_URSEL	URSEL
#define MIDI_UMSEL	UMSEL
#define MIDI_UPM1	UPM1
#define MIDI_UPM0	UPM0
#define MIDI_USBS	USBS
#define MIDI_UCSZ1	UCSZ1
#define MIDI_UCSZ0	UCSZ0
#define MIDI_UCPOL	UCPOL	

// Interruptvectors
#define MIDI_USART_RX_vect		USART_RXC_vect
#define MIDI_USART_UDRE_vect	USART_UDRE_vect
#define MIDI_USART_TX_vect		USART_TXC_vect

//Baudrate
#define MIDI_BAUDRATEVAL 	31250




#define TXBUFFERMAX			10
#define RXBUFFERMAX			300

// Bitwise macros to make the code a bit easier to read
#define bit_get(p,m) ((p) & (m))
#define bit_set(p,m) ((p) |= (m))
#define bit_clear(p,m) ((p) &= ~(m))
#define bit_flip(p,m) ((p) ^= (m))
#define bit_write(c,p,m) (c ? bit_set(p,m) : bit_clear(p,m))
#define BIT(x) (0x01 << (x))
#define LONGBIT(x) ((unsigned long)0x00000001 << (x))

// Bit defines for MIDIInfoReceived byte
#define PROG_CHANGED		0x01
#define STATUS_CHANGED		0x02
#define BUTTON_CHANGED		0x04
#define STOMP_STATUS		0x08
#define MOD_STATUS			0x10
#define DELAY_STATUS		0x20

// Bit defines for the buttons
#define UP_BUTTON			0x01
#define DOWN_BUTTON			0x02
#define STOMP_BUTTON		0x04
#define MOD_BUTTON			0x08
#define DELAY_BUTTON		0x10
#define EXP_BUTTON			0x20
#define SPARE_BUTTON		0x40

#define DEBOUNCE			200U		// button debounce clock 200Hz = 5msec

void GetPodXTChannelSettings(int ProgramNumber);
void Init_Ports(void);
void MIDI_putc(unsigned char data);
unsigned char MIDI_getc (void);

