#define NUM_LENGTH 5	// half-nibbles

unsigned char morseData[] PROGMEM = {
	0xFF, 0xC0,	// 0
	0xBF, 0xC0,	// 1
	0xAF, 0xC0,	// 2
	0xAB, 0xC0,	// 3
	0xAA, 0xC0,	// 4
	0xAA, 0x80,	// 5
	0xEA, 0x80,	// 6
	0xFA, 0x80,	// 7
	0xFE, 0x80,	// 8
	0xFF, 0x80	// 9
};

unsigned mags[5] PROGMEM = { 10000, 1000, 100, 10, 1 };
