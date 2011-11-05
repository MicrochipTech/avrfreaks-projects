/*
 * triak.c
 *program do sterowania k¹tem zap³onu triaka //w main.c wpisuje do zmiennej: brightness_bulb; wartoœæ jasnoœci triaka
 *  Created on: 2011-09-07
 *       Autor: £ukasz Uszko
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "triak.h"

//zmienne globalne
volatile uint8_t kroki;
volatile uint8_t brightness_bulb; // dostpny we wszystkich plikach
//volatile uint8_t ika1;

uint8_t procent(uint8_t wartosc_procentowa) {
	return (wartosc_procentowa * (LICZBA_KROKOW - 1)) / 100;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void init_interruptINT0(void) {
	MCUCR |= (1 << ISC01) | (1 << ISC00); // zbocze narastaj¹ce
	//GICR |= (1 << INT0); // odblokowanie INT0

		//GIFR |= (1<<INTF0);         // skasowanie flagi wyst¹pienia przerwania INT0
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////

void init_TIMER0(void)
{//TCCR0 = (1 << CS01);         // odpalany jest w przerwaniu
TIMSK |= (1 << TOIE0);
TCNT0 = 255-46;
	}///////////////////////////////////////
/*void init_TIMER2(void) {
	// prescaler off
	TCCR2 |= (1 << WGM21); // tryb CTC
	TIMSK |= (1 << OCIE2); // odblokowanie przerwania COMPARE MATCH
	OCR2 = 46;*/
	/*OCR2=46 dlatego ¿e:
	 * -czas po³ówki sinusoidy wynosi 10ms-->100Hz
	 * -szpilka na wejœciu INT0 przyjmujê ¿e trwaæ 0,8ms---> czyli czas do podzia³u: 10-0,8= 9,2ms
	 * -czas ten dzielê na LICZBÊ KROKÓW (czyli 9,2/200=46us--->21,739kHz)
	 * -czyli przerwanie Compare_Match co 46us
	 * -F_CPU(8MHz)/Prescaler(8)/21,739kHz=46

}*/////////////////////////////////////////////////

ISR(INT0_vect)
{
	TIMER0_STOP;
	T_ZAR_OFF;
	//ika1=brightness_bulb;
	kroki = LICZBA_KROKOW;
   // TCNT0 = 255-46;
	TIMER0_START;
}

ISR(TIMER0_OVF_vect)
{TCNT0 = 255-46;                    // za ka¿dym razem wpisuje w przerwaniu wartoœæ TCNT0
if (kroki <= brightness_bulb)
		T_ZAR_ON;
	kroki--;
}
