// AVR_Pins.h
// AVR chip pin info (ports etc).

// Pin flag definitions (as used by Studio+)
#define PINN 0x01						// No pin
#define PINU 0x02						// Unknown
#define PINL 0x04						// Low
#define PINH 0x08						// High
#define PINX 0x10						// Crystal
#define PINA 0x20						// Analogue
#define PINI 0x40						// Input
#define PINO 0x80						// Output

// Number of pins for each package of each chip
int chipPins[] = {						// 1 row per ID
	0,  0,  0,  0,  0,  0,				// 0
	0,  0,  20, 0,  20, 20,				// AT90S1200
	0,  0,  20, 0,  20, 0,				// AT90S2313
	0,  0,  8,  0,  8,  0,				// AT90S2323
	0,  0,  8,  0,  8,  0,				// AT90S2343
	32, 0,  28, 0,  0,  0,				// AT90S2333
	32, 0,  28, 0,  0,  0,				// AT90S4433
	44, 44, 40, 0,  0,  0,				// AT90S4414
	44, 44, 40, 0,  0,  0,				// AT90S4434
	48, 0,  0,  0,  0,  0,				// AT90C8534
	44, 44, 40, 0,  0,  0,				// AT90S8515
	44, 44, 40, 44, 0,  0,				// AT90S8535
	32, 0,  28, 32, 0,  0,				// ATmega8
	44, 0,  40, 0,  0,  0,				// ATmega16
	44, 0,  40, 0,  0,  0,				// ATmega161
	44, 0,  40, 0,  0,  0,				// ATmega163
	44, 0,  40, 0,  0,  0,				// ATmega232
	64, 0,  0,  0,  0,  0,				// ATmega64
	64, 0,  0,  0,  0,  0,				// ATmega103
	64, 0,  0,  0,  0,  0,				// ATmega128
	0,  0,  8,  0,  8,  0,				// ATtiny11
	0,  0,  8,  0,  8,  0,				// ATtiny12
	0,  0,  8,  0,  8,  0,				// ATtiny15
	0,  0,  8,  0,  8,  0,				// ATtiny22
	32, 0,  28, 32, 0,  0};				// ATtiny28

