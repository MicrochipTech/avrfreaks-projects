// AVR_Ports.h
// AVR chip ports info.

// Port info row length
#define PORTINFOLENGTH 7

// Port info for each chip & package
// DDR_ADR PORT_ADR PINS_ADR PIN_INDEX FIRST COUNT ORDER
int portsAT90S1200[] = {
	4,4,4,0,0,0,						// Number of rows per package
	// PDIP
	0x17, 0x18, 0x16, 11, 0, 8, 1,  // PORTB, Bits 0->7
	0x11, 0x12, 0x10, 1,  0, 2, 1,  // PORTD, Bits 0->1
	0x11, 0x12, 0x10, 5,  2, 4, 1,  // PORTD, Bits 2->5
	0x11, 0x12, 0x10, 10, 5, 1, 1,  // PORTD, Bit 6
	// SOIC
	0x17, 0x18, 0x16, 11, 0, 8, 1,  // PORTB, Bits 0->7
	0x11, 0x12, 0x10, 1,  0, 2, 1,  // PORTD, Bits 0->1
	0x11, 0x12, 0x10, 5,  2, 4, 1,  // PORTD, Bits 2->5
	0x11, 0x12, 0x10, 10, 5, 1, 1,  // PORTD, Bit 6
	// SSOP
	0x17, 0x18, 0x16, 11, 0, 8, 1,  // PORTB, Bits 0->7
	0x11, 0x12, 0x10, 1,  0, 2, 1,  // PORTD, Bits 0->1
	0x11, 0x12, 0x10, 5,  2, 4, 1,  // PORTD, Bits 2->5
	0x11, 0x12, 0x10, 10, 5, 1, 1}; // PORTD, Bit 6

int portsAT90S2313[] = {
	4,4,0,0,0,0,
	// PDIP
	0x17, 0x18, 0x16, 11, 0, 8, 1,  // PORTB, Bits 0->7
	0x11, 0x12, 0x10, 1,  0, 2, 1,  // PORTD, Bits 0->1
	0x11, 0x12, 0x10, 5,  2, 4, 1,  // PORTD, Bits 2->5
	0x11, 0x12, 0x10, 10, 5, 1, 1,  // PORTD, Bit 6
	// SOIC
	0x17, 0x18, 0x16, 11, 0, 8, 1,  // PORTB, Bits 0->7
	0x11, 0x12, 0x10, 1,  0, 2, 1,  // PORTD, Bits 0->1
	0x11, 0x12, 0x10, 5,  2, 4, 1,  // PORTD, Bits 2->5
	0x11, 0x12, 0x10, 10, 5, 1, 1}; // PORTD, Bit 6

int portsAT90S2323[] = {
	1,1,0,0,0,0,
	// PDIP
	0x17, 0x18, 0x16, 4,  0, 3, 1,  // PORTB, Bits 0->2
	// SOIC
	0x17, 0x18, 0x16, 4,  0, 3, 1}; // PORTB, Bits 0->2

int portsAT90S2343[] = {
	2,2,0,0,0,0,
	// PDIP
	0x17, 0x18, 0x16, 4,  0, 3, 1,  // PORTB, Bits 0->2
	0x17, 0x18, 0x16, 1,  3, 2, 1,  // PORTB, Bits 3->4
	// SOIC
	0x17, 0x18, 0x16, 4,  0, 3, 1,  // PORTB, Bits 0->2
	0x17, 0x18, 0x16, 1,  3, 2, 1}; // PORTB, Bits 3->4

int portsAT90S2333[] = {
	5,4,0,0,0,0,
	// TQFP
	0x17, 0x18, 0x16, 11, 0, 6, 1,  // PORTB, Bits 0->5
	0x14, 0x15, 0x13, 22, 0, 6, 1,  // PORTC, Bits 0->5
	0x11, 0x12, 0x10, 29, 0, 3, 1,  // PORTD, Bits 0->2
	0x11, 0x12, 0x10, 0,  3, 2, 1,  // PORTD, Bits 3->4
	0x11, 0x12, 0x10, 8,  5, 3, 1,  // PORTD, Bits 5->7
	// PDIP
	0x17, 0x18, 0x16, 13, 0, 6, 1,  // PORTB, Bits 0->5
	0x14, 0x15, 0x13, 22, 0, 6, 1,  // PORTC, Bits 0->5
	0x11, 0x12, 0x10, 1,  0, 5, 1,  // PORTD, Bits 0->4
	0x11, 0x12, 0x10, 10, 5, 3, 1}; // PORTD, Bits 5->7

int portsAT90S4433[] = {
	5,4,0,0,0,0,
	// TQFP
	0x17, 0x18, 0x16, 11, 0, 6, 1,  // PORTB, Bits 0->5
	0x14, 0x15, 0x13, 22, 0, 6, 1,  // PORTC, Bits 0->5
	0x11, 0x12, 0x10, 29, 0, 3, 1,  // PORTD, Bits 0->2
	0x11, 0x12, 0x10, 0,  3, 2, 1,  // PORTD, Bits 3->4
	0x11, 0x12, 0x10, 8,  5, 3, 1,  // PORTD, Bits 5->7
	// PDIP
	0x17, 0x18, 0x16, 13, 0, 6, 1,  // PORTB, Bits 0->5
	0x14, 0x15, 0x13, 22, 0, 6, 1,  // PORTC, Bits 0->5
	0x11, 0x12, 0x10, 1,  0, 5, 1,  // PORTD, Bits 0->4
	0x11, 0x12, 0x10, 10, 5, 3, 1}; // PORTD, Bits 5->7

int portsAT90S4414[] = {
	6,5,4,0,0,0,
	// TQFP
	0x1A, 0x1B, 0x19, 29, 0, 8, 0,  // PORTA, Bits 0->7
	0x17, 0x18, 0x16, 39, 0, 5, 1,  // PORTB, Bits 0->4
	0x17, 0x18, 0x16, 0,  5, 3, 1,  // PORTB, Bits 5->7
	0x14, 0x15, 0x13, 17, 0, 8, 1,  // PORTC, Bits 0->7
	0x11, 0x12, 0x10, 4,  0, 1, 1,  // PORTD, Bit 0
	0x11, 0x12, 0x10, 6,  1, 7, 1,  // PORTD, Bits 1->7
	// PLCC
	0x1A, 0x1B, 0x19, 35, 0, 8, 0,  // PORTA, Bits 0->7
	0x17, 0x18, 0x16, 1,  0, 8, 1,  // PORTB, Bits 0->7
	0x14, 0x15, 0x13, 23, 0, 8, 1,  // PORTC, Bits 0->7
	0x11, 0x12, 0x10, 10, 0, 1, 1,  // PORTD, Bit 0
	0x11, 0x12, 0x10, 12, 1, 7, 1,  // PORTD, Bits 1->7
	// PDIP
	0x1A, 0x1B, 0x19, 31, 0, 8, 0,  // PORTA, Bits 0->7
	0x17, 0x18, 0x16, 0,  0, 8, 1,  // PORTB, Bits 0->7
	0x14, 0x15, 0x13, 20, 0, 8, 1,  // PORTC, Bits 0->7
	0x11, 0x12, 0x10, 9,  0, 8, 1}; // PORTD, Bits 0->7

int portsAT90S4434[] = {
	5,4,4,0,0,0,
	// TQFP
	0x1A, 0x1B, 0x19, 29, 0, 8, 0,  // PORTA, Bits 0->7
	0x17, 0x18, 0x16, 39, 0, 5, 1,  // PORTB, Bits 0->4
	0x17, 0x18, 0x16, 0,  5, 3, 1,  // PORTB, Bits 5->7
	0x14, 0x15, 0x13, 18, 0, 8, 1,  // PORTC, Bits 0->7
	0x11, 0x12, 0x10, 8,  0, 8, 1,  // PORTD, Bits 0->7
	// PLCC
	0x1A, 0x1B, 0x19, 35, 0, 8, 0,  // PORTA, Bits 0->7
	0x17, 0x18, 0x16, 1,  0, 8, 1,  // PORTB, Bits 0->7
	0x14, 0x15, 0x13, 24, 0, 8, 1,  // PORTC, Bits 0->7
	0x11, 0x12, 0x10, 14, 0, 8, 1,  // PORTD, Bits 0->7
	// PDIP
	0x1A, 0x1B, 0x19, 32, 0, 8, 0,  // PORTA, Bits 0->7
	0x17, 0x18, 0x16, 0,  0, 8, 1,  // PORTB, Bits 0->7
	0x14, 0x15, 0x13, 21, 0, 8, 1,  // PORTC, Bits 0->7
	0x11, 0x12, 0x10, 13, 0, 8, 1}; // PORTD, Bits 0->7

int portsAT90C8534[] = {
	3,0,0,0,0,0,
	// TQFP
	0x1A, 0x1B, 0x19, 43, 0, 4, 0,  // PORTA, Bits 0->3
	0x1A, 0x1B, 0x19, 37, 4, 2, 0,  // PORTA, Bits 4->5
	0x1A, 0x1B, 0x19, 32, 6, 1, 1}; // PORTA, Bit 6
	
int portsAT90S8515[] = {
	6,5,4,0,0,0,
	// TQFP
	0x1A, 0x1B, 0x19, 29, 0, 8, 0,  // PORTA, Bits 0->7
	0x17, 0x18, 0x16, 39, 0, 5, 1,  // PORTB, Bits 0->4
	0x17, 0x18, 0x16, 0,  5, 3, 1,  // PORTB, Bits 5->7
	0x14, 0x15, 0x13, 17, 0, 8, 1,  // PORTC, Bits 0->7
	0x11, 0x12, 0x10, 4,  0, 1, 1,  // PORTD, Bit 0
	0x11, 0x12, 0x10, 6,  1, 7, 1,  // PORTD, Bits 1->7
	// PLCC
	0x1A, 0x1B, 0x19, 35, 0, 8, 0,  // PORTA, Bits 0->7
	0x17, 0x18, 0x16, 1,  0, 8, 1,  // PORTB, Bits 0->7
	0x14, 0x15, 0x13, 23, 0, 8, 1,  // PORTC, Bits 0->7
	0x11, 0x12, 0x10, 10, 0, 1, 1,  // PORTD, Bit 0
	0x11, 0x12, 0x10, 12, 1, 7, 1,  // PORTD, Bits 1->7
	// PDIP
	0x1A, 0x1B, 0x19, 31, 0, 8, 0,  // PORTA, Bits 0->7
	0x17, 0x18, 0x16, 0,  0, 8, 1,  // PORTB, Bits 0->7
	0x14, 0x15, 0x13, 20, 0, 8, 1,  // PORTC, Bits 0->7
	0x11, 0x12, 0x10, 9,  0, 8, 1}; // PORTD, Bits 0->7

int portsAT90S8535[] = {
	5,4,4,5,0,0,
	// TQFP
	0x1A, 0x1B, 0x19, 29, 0, 8, 0,  // PORTA, Bits 0->7
	0x17, 0x18, 0x16, 39, 0, 5, 1,  // PORTB, Bits 0->4
	0x17, 0x18, 0x16, 0,  5, 3, 1,  // PORTB, Bits 5->7
	0x14, 0x15, 0x13, 18, 0, 8, 1,  // PORTC, Bits 0->7
	0x11, 0x12, 0x10, 8,  0, 8, 1,  // PORTD, Bits 0->7
	// PLCC
	0x1A, 0x1B, 0x19, 35, 0, 8, 0,  // PORTA, Bits 0->7
	0x17, 0x18, 0x16, 1,  0, 8, 1,  // PORTB, Bits 0->7
	0x14, 0x15, 0x13, 24, 0, 8, 1,  // PORTC, Bits 0->7
	0x11, 0x12, 0x10, 14, 0, 8, 1,  // PORTD, Bits 0->7
	// PDIP
	0x1A, 0x1B, 0x19, 32, 0, 8, 0,  // PORTA, Bits 0->7
	0x17, 0x18, 0x16, 0,  0, 8, 1,  // PORTB, Bits 0->7
	0x14, 0x15, 0x13, 21, 0, 8, 1,  // PORTC, Bits 0->7
	0x11, 0x12, 0x10, 13, 0, 8, 1,  // PORTD, Bits 0->7
	// MLF
	0x1A, 0x1B, 0x19, 29, 0, 8, 0,  // PORTA, Bits 0->7
	0x17, 0x18, 0x16, 39, 0, 5, 1,  // PORTB, Bits 0->4
	0x17, 0x18, 0x16, 0,  5, 3, 1,  // PORTB, Bits 5->7
	0x14, 0x15, 0x13, 18, 0, 8, 1,  // PORTC, Bits 0->7
	0x11, 0x12, 0x10, 8,  0, 8, 1}; // PORTD, Bits 0->7

int portsATmega8[] = {
	6,6,6,0,0,0,
	// TQFP
	0x17, 0x18, 0x16, 11, 0, 6, 1,  // PORTB, Bits 0->5
	0x17, 0x18, 0x16, 6,  6, 2, 1,  // PORTB, Bits 6->7
	0x14, 0x15, 0x13, 22, 0, 7, 1,  // PORTC, Bits 0->6
	0x11, 0x12, 0x10, 29, 0, 3, 1,  // PORTD, Bits 0->2
	0x11, 0x12, 0x10, 0,  3, 2, 1,  // PORTD, Bits 3->4
	0x11, 0x12, 0x10, 8,  5, 3, 1,  // PORTD, Bits 5->7
	// PDIP
	0x17, 0x18, 0x16, 13, 0, 6, 1,  // PORTB, Bits 0->5
	0x17, 0x18, 0x16, 8,  6, 2, 1,  // PORTB, Bits 6->7
	0x14, 0x15, 0x13, 22, 0, 6, 1,  // PORTC, Bits 0->5
	0x14, 0x15, 0x13, 0,  6, 1, 1,  // PORTC, Bit 6
	0x11, 0x12, 0x10, 1,  0, 5, 1,  // PORTD, Bits 0->4
	0x11, 0x12, 0x10, 8,  5, 3, 1,  // PORTD, Bits 5->7
	// MLF
	0x17, 0x18, 0x16, 11, 0, 6, 1,  // PORTB, Bits 0->5
	0x17, 0x18, 0x16, 6,  6, 2, 1,  // PORTB, Bits 6->7
	0x14, 0x15, 0x13, 22, 0, 7, 1,  // PORTC, Bits 0->6
	0x11, 0x12, 0x10, 29, 0, 3, 1,  // PORTD, Bits 0->2
	0x11, 0x12, 0x10, 0,  3, 2, 1,  // PORTD, Bits 3->4
	0x11, 0x12, 0x10, 8,  5, 3, 1}; // PORTD, Bits 5->7

int portsATmega16[] = {
	5,4,0,0,0,0,
	// TQFP
	0x1A, 0x1B, 0x19, 29, 0, 8, 0,  // PORTA, Bits 0->7
	0x17, 0x18, 0x16, 39, 0, 5, 1,  // PORTB, Bits 0->4
	0x17, 0x18, 0x16, 0,  5, 3, 1,  // PORTB, Bits 5->7
	0x14, 0x15, 0x13, 18, 0, 8, 1,  // PORTC, Bits 0->7
	0x11, 0x12, 0x10, 8,  0, 8, 1,  // PORTD, Bits 0->7
	// PDIP
	0x1A, 0x1B, 0x19, 32, 0, 8, 0,  // PORTA, Bits 0->7
	0x17, 0x18, 0x16, 0,  0, 8, 1,  // PORTB, Bits 0->7
	0x14, 0x15, 0x13, 21, 0, 8, 1,  // PORTC, Bits 0->7
	0x11, 0x12, 0x10, 13, 0, 8, 1}; // PORTD, Bits 0->7

int portsATmega161[] = {
	8,5,0,0,0,0,
	// TQFP
	0x1A, 0x1B, 0x19, 29, 0, 8, 0,  // PORTA, Bits 0->7
	0x17, 0x18, 0x16, 39, 0, 5, 1,  // PORTB, Bits 0->4
	0x17, 0x18, 0x16, 0,  5, 3, 1,  // PORTB, Bits 5->7
	0x14, 0x15, 0x13, 17, 0, 8, 1,  // PORTC, Bits 0->7
	0x11, 0x12, 0x10, 4,  0, 1, 1,  // PORTD, Bit 0
	0x11, 0x12, 0x10, 6,  1, 7, 1,  // PORTD, Bits 1->7
	0x06, 0x07, 0x05, 28, 0, 1, 1,  // PORTE, Bit 0
	0x06, 0x07, 0x05, 25, 1, 2, 0,  // PORTE, Bits 1->2
	// PDIP
	0x1A, 0x1B, 0x19, 31, 0, 8, 0,  // PORTA, Bits 0->7
	0x17, 0x18, 0x16, 0,  0, 8, 1,  // PORTB, Bits 0->7
	0x14, 0x15, 0x13, 20, 0, 8, 1,  // PORTC, Bits 0->7
	0x11, 0x12, 0x10, 9,  0, 8, 1,  // PORTD, Bits 0->7
	0x06, 0x07, 0x05, 28, 0, 3, 0}; // PORTE, Bits 0->2

int portsATmega163[] = {
	5,4,0,0,0,0,
	// TQFP
	0x1A, 0x1B, 0x19, 29, 0, 8, 0,  // PORTA, Bits 0->7
	0x17, 0x18, 0x16, 39, 0, 5, 1,  // PORTB, Bits 0->4
	0x17, 0x18, 0x16, 0,  5, 3, 1,  // PORTB, Bits 5->7
	0x14, 0x15, 0x13, 18, 0, 8, 1,  // PORTC, Bits 0->7
	0x11, 0x12, 0x10, 8,  0, 8, 1,  // PORTD, Bits 0->7
	// PDIP
	0x1A, 0x1B, 0x19, 32, 0, 8, 0,  // PORTA, Bits 0->7
	0x17, 0x18, 0x16, 0,  0, 8, 1,  // PORTB, Bits 0->7
	0x14, 0x15, 0x13, 21, 0, 8, 1,  // PORTC, Bits 0->7
	0x11, 0x12, 0x10, 13, 0, 8, 1}; // PORTD, Bits 0->7

int portsATmega323[] = {
	5,4,0,0,0,0,
	// TQFP
	0x1A, 0x1B, 0x19, 29, 0, 8, 0,  // PORTA, Bits 0->7
	0x17, 0x18, 0x16, 39, 0, 5, 1,  // PORTB, Bits 0->4
	0x17, 0x18, 0x16, 0,  5, 3, 1,  // PORTB, Bits 5->7
	0x14, 0x15, 0x13, 18, 0, 8, 1,  // PORTC, Bits 0->7
	0x11, 0x12, 0x10, 8,  0, 8, 1,  // PORTD, Bits 0->7
	// PDIP
	0x1A, 0x1B, 0x19, 32, 0, 8, 0,  // PORTA, Bits 0->7
	0x17, 0x18, 0x16, 0,  0, 8, 1,  // PORTB, Bits 0->7
	0x14, 0x15, 0x13, 21, 0, 8, 1,  // PORTC, Bits 0->7
	0x11, 0x12, 0x10, 13, 0, 8, 1}; // PORTD, Bits 0->7

int portsATmega64[] = {
	9,0,0,0,0,0,
	// TQFP
	0x1A, 0x1B, 0x19, 43, 0, 8, 0,  // PORTA, Bits 0->7
	0x17, 0x18, 0x16, 9,  0, 8, 1,  // PORTB, Bits 0->7
	0x14, 0x15, 0x13, 34, 0, 8, 1,  // PORTC, Bits 0->7
	0x11, 0x12, 0x10, 24, 0, 8, 1,  // PORTD, Bits 0->7
	0x02, 0x03, 0x01, 1,  0, 8, 1,  // PORTE, Bits 0->7
	0x41, 0x42, 0x00, 53, 0, 8, 0,  // PORTF, Bits 0->7
	0x44, 0x45, 0x43, 32, 0, 2, 1,  // PORTG, Bits 0->1
	0x44, 0x45, 0x43, 42, 2, 1, 1,  // PORTG, Bit 2
	0x44, 0x45, 0x43, 17, 3, 2, 1}; // PORTG, Bits 3->4

int portsATmega103[] = {
	5,0,0,0,0,0,
	// TQFP
	0x1A, 0x1B, 0x19, 43, 0, 8, 0,  // PORTA, Bits 0->7
	0x17, 0x18, 0x16, 9,  0, 8, 1,  // PORTB, Bits 0->7
	-1,   0x15, -1,   34, 0, 8, 1,  // PORTC, Bits 0->7, Output only
	0x11, 0x12, 0x10, 24, 0, 8, 1,  // PORTD, Bits 0->7
	0x02, 0x03, 0x01, 1,  0, 8, 1}; // PORTE, Bits 0->7

int portsATmega128[] = {
	9,0,0,0,0,0,
	// TQFP
	0x1A, 0x1B, 0x19, 43, 0, 8, 0,  // PORTA, Bits 0->7
	0x17, 0x18, 0x16, 9,  0, 8, 1,  // PORTB, Bits 0->7
	0x14, 0x15, 0x13, 34, 0, 8, 1,  // PORTC, Bits 0->7
	0x11, 0x12, 0x10, 24, 0, 8, 1,  // PORTD, Bits 0->7
	0x02, 0x03, 0x01, 1,  0, 8, 1,  // PORTE, Bits 0->7
	0x41, 0x42, 0x00, 53, 0, 8, 0,  // PORTF, Bits 0->7
	0x44, 0x45, 0x43, 32, 0, 2, 1,  // PORTG, Bits 0->1
	0x44, 0x45, 0x43, 42, 2, 1, 1,  // PORTG, Bit 2
	0x44, 0x45, 0x43, 17, 3, 2, 1}; // PORTG, Bits 3->4

int portsATtiny11[] = {
	3,3,0,0,0,0,
	// PDIP
	0x17, 0x18, 0x16, 4,  0, 3, 1,  // PORTB, Bits 0->2
	0x17, 0x18, 0x16, 1,  3, 2, 1,  // PORTB, Bits 3->4
	-1,   -1,   0x16, 0,  5, 1, 1,  // PORTB, Bit 5, Input only
	// SOIC
	0x17, 0x18, 0x16, 4,  0, 3, 1,  // PORTB, Bits 0->2
	0x17, 0x18, 0x16, 1,  3, 2, 1,  // PORTB, Bits 3->4
	-1,   -1,   0x16, 0,  5, 1, 1}; // PORTB, Bit 5, Input only

int portsATtiny12[] = {
	3,3,0,0,0,0,
	// PDIP
	0x17, 0x18, 0x16, 4,  0, 3, 1,  // PORTB, Bits 0->2
	0x17, 0x18, 0x16, 1,  3, 2, 1,  // PORTB, Bits 3->4
	-1,   -1,   0x16, 0,  5, 1, 1,  // PORTB, Bit 5, Input only
	// SOIC
	0x17, 0x18, 0x16, 4,  0, 3, 1,  // PORTB, Bits 0->2
	0x17, 0x18, 0x16, 1,  3, 2, 1,  // PORTB, Bits 3->4
	-1,   -1,   0x16, 0,  5, 1, 1}; // PORTB, Bit 5, Input only

int portsATtiny15[] = {
	3,3,0,0,0,0,
	// PDIP
	0x17, 0x18, 0x16, 4,  0, 3, 1,  // PORTB, Bits 0->2
	0x17, 0x18, 0x16, 1,  3, 2, 0,  // PORTB, Bits 3->4
	-1,   -1,   0x16, 0,  5, 1, 1,  // PORTB, Bit 5, Input only
	// SOIC
	0x17, 0x18, 0x16, 4,  0, 3, 1,  // PORTB, Bits 0->2
	0x17, 0x18, 0x16, 1,  3, 2, 0,  // PORTB, Bits 3->4
	-1,   -1,   0x16, 0,  5, 1, 1}; // PORTB, Bit 5, Input only

int portsATtiny22[] = { // **** THIS IS A GUESS!! NEED DATASHEET
	3,3,0,0,0,0,
	// PDIP
	0x17, 0x18, 0x16, 4,  0, 3, 1,  // PORTB, Bits 0->2
	0x17, 0x18, 0x16, 1,  3, 2, 1,  // PORTB, Bits 3->4
	-1,   -1,   0x16, 0,  5, 1, 1,  // PORTB, Bit 5, Input only
	// SOIC
	0x17, 0x18, 0x16, 4,  0, 3, 1,  // PORTB, Bits 0->2
	0x17, 0x18, 0x16, 1,  3, 2, 1,  // PORTB, Bits 3->4
	-1,   -1,   0x16, 0,  5, 1, 1}; // PORTB, Bit 5, Input only

int portsATtiny28[] = {
	7,6,7,0,0,0,
	// TQFP
	0x1A, 0x1B, 0x19, 26, 0, 2, 0,  // PORTA, Bits 0->1
	0x1A, 0x1B, 0x19, 24, 2, 2, 1,  // PORTA, Bits 2->3
	0x17, 0x18, 0x16, 11, 0, 6, 1,  // PORTB, Bits 0->5
	0x17, 0x18, 0x16, 22, 6, 2, 1,  // PORTB, Bits 6->7
	0x11, 0x12, 0x10, 29, 0, 3, 1,  // PORTD, Bits 0->2
	0x11, 0x12, 0x10, 0,  3, 2, 1,  // PORTD, Bits 3->4
	0x11, 0x12, 0x10, 8,  5, 3, 1,  // PORTD, Bits 5->7
	// PDIP
	0x1A, 0x1B, 0x19, 26, 0, 2, 0,  // PORTA, Bits 0->1
	0x1A, 0x1B, 0x19, 24, 2, 2, 1,  // PORTA, Bits 2->3
	0x17, 0x18, 0x16, 13, 0, 6, 1,  // PORTB, Bits 0->5
	0x17, 0x18, 0x16, 22, 6, 2, 1,  // PORTB, Bits 6->7
	0x11, 0x12, 0x10, 1,  0, 5, 1,  // PORTD, Bits 0->4
	0x11, 0x12, 0x10, 10, 5, 3, 1,  // PORTD, Bits 5->7
	// MLF
	0x1A, 0x1B, 0x19, 26, 0, 2, 0,  // PORTA, Bits 0->1
	0x1A, 0x1B, 0x19, 24, 2, 2, 1,  // PORTA, Bits 2->3
	0x17, 0x18, 0x16, 11, 0, 6, 1,  // PORTB, Bits 0->5
	0x17, 0x18, 0x16, 22, 6, 2, 1,  // PORTB, Bits 6->7
	0x11, 0x12, 0x10, 29, 0, 3, 1,  // PORTD, Bits 0->2
	0x11, 0x12, 0x10, 0,  3, 2, 1,  // PORTD, Bits 3->4
	0x11, 0x12, 0x10, 8,  5, 3, 1}; // PORTD, Bits 5->7

// Port info pointers
int * portInfoPointers[] = {			// Must be 1 entry per ID
	0,
	portsAT90S1200,
	portsAT90S2313,
	portsAT90S2323,
	portsAT90S2343,
	portsAT90S2333,
	portsAT90S4433,
	portsAT90S4414,
	portsAT90S4434,
	portsAT90C8534,
	portsAT90S8515,
	portsAT90S8535,
	portsATmega8,
	portsATmega16,
	portsATmega161,
	portsATmega163,
	portsATmega323,
	portsATmega64,
	portsATmega103,
	portsATmega128,
	portsATtiny11,
	portsATtiny12,
	portsATtiny15,
	portsATtiny22,
	portsATtiny28};
