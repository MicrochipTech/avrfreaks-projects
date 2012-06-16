/*******************************************************************
 * io.h
 * written by      Ralf Meeh 23.12.2009
 * last changed by Ralf Meeh 26.12.2009
 * Function:       I/O Definitionen,  Projektabhängig
 *******************************************************************/


// Betriebs LED
#define LED_DDR		DDRB
#define LED_PIN		PINB
#define LED_PORT	PORTB
#define LED			PB0

// Sub Carrier Out
#define SC_DDR		DDRB
#define SC_PIN		PINB
#define SC_PORT		PORTB
#define SC			PB1


// 2 Tasten
#define KEYDDR		DDRC
#define KEYPORT		PORTC
#define KEYS 		PINC
#define KEY1		PC5
#define KEY2		PC4

// 1750Hz Ton mit höherem Pegel ansteuern (T1)
#define T1DDR		DDRC
#define T1PORT		PORTC
#define T1_IN 		PINC
#define T1			PC0

// Drehgeber
#define ROTARYDDR	DDRC
#define ROTARYPIN	PINC
#define ROTARYPORT	PORTC
#define	ROTARY1		PC2
#define ROTARY2		PC3



//PORT D0 - D5 ist für LCD belegt
//Port D6 und D7 sind noch frei

// SPS ähnliche Timer 1ms, diese max 16.5 sec
#define ANZ_TIMER 		5		//  1ms Timer
// SPS ähnliche Timer 10ms, diese max 1600.5 sec
#define ANZ_TIMER_10	5		// 10ms Timer


// diese Variablen müßen volatile bleiben sie werden
// sowohl von main wie auch von der ISR verwendet
extern volatile uint16_t 	tick;
volatile uint8_t 	cnt_10ms;
volatile uint8_t 	cnt_500ms;
volatile uint8_t 	FLAG_10ms;
volatile uint8_t 	FLAG_500ms;
volatile uint16_t 	T[ANZ_TIMER];
volatile uint16_t 	T_10[ANZ_TIMER_10];

void init_io(void);
unsigned char scan_key(void);
unsigned char get_debounced(void);



