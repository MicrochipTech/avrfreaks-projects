// On Air Tool Kit (OnAirTK)®
// © Portia CyberLogic.
// www.portia-cyberlogic.com
// mail@portia-cyberlogic.com
//
// Main.cpp
// Service entry point.
// By Mahmoud Attaher.

#include "MainInc.h"

int16_t Counter;

// Entry point..
int main(void)
{
	// Intialize LCD..
	LCDInit();
	LCDClear();

	LCDPrintStringXY(0, 0, "3 wire LCD");
	LCDPrintStringXY(0, 1, "Initializing...");

	Counter = 0;

	// Initialize timer1..
	// I need TIMER1 to be fired every 1 second..
	// F_CPU = 4MHz, Prescaler = 256 => The cpmare value = 4000000 / 256 = 15625 < (65535 ie 16bits)
	// so no more time keeping variables needed.
	TCCR1B=(1<<WGM12)|(1<<CS12)|(0<<CS11)|(0<<CS10); // Prescaler = F_CPU / 256.
	OCR1A = (F_CPU / 256); // Compare value.
	TIMSK|=(1<<OCIE1A); //Enable CTC interrupt.

	// Enable inturrupts..
	sei();

	// Main loop..
	while(1);

	return 0;
}

ISR(TIMER1_COMPA_vect)
{
	LCDClear(); // Clear display.

	LCDPrintStringXY(0, 0, "Poria CyberLogic");
	LCDPrintStringXY(0, 1, "Counter:        ");

	LCDPrintIntXY(12, 1, Counter, 4);

	Counter++;
	if(Counter >= 10000)
		Counter = 0;
}
