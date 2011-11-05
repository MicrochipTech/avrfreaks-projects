/*
 * dekoder_IR_echostar616.c
 *program dekoduj¹cy sygna³ z pilota od dekodera ECHOSTAR 616
 *
 *  Created on: 2011-09-20
 *       Autor: £ukasz Uszko
 */

#include <avr/io.h>
#include <avr/interrupt.h>

#include "dekoder_IR_echostar616.h"
volatile uint16_t code; // kod danego przycisku
volatile uint8_t Ir_key_press_flag; // flaga = 1 - informuje, ¿e odebrany zosta³ nowy kod z pilota
                                    // po odczytaniu danych ze zmiennych zerujê  flagê aby mo¿na by³o wczytaæ kolejne kody z pilota
volatile uint8_t counter; // licznik zboczy na ICP1(bitów z pilota)

void init_IR() {
	DDR(IR_PORT) &= ~IR_IN; // pin jako wejœcie
	PORT(IR_PORT) |= IR_IN; // podci¹gniêcie pinu do VCC
#if TIMER1_PRESCALER == 1
	TCCR1B |= (1<<CS10); // Timer1 / 1
#endif
#if TIMER1_PRESCALER == 8
	TCCR1B |= (1 << CS11); // Timer1 / 8
#endif
#if TIMER1_PRESCALER == 64
	TCCR1B |= (1<<CS11)|(1<<CS10); // Timer1 / 64
#endif
#if TIMER1_PRESCALER == 256
	TCCR1B |= (1<<CS12); // Timer1 / 256
#endif
	//TCCR1B |= (1 << ICNC1);        //filtr zak³óceñ
	TCCR1B &= ~(1 << ICES1);       // Zbocze opadaj¹ce na ICP
	IR_ON;      // zezwolenie na Przerwanie od ICP
	counter = 0;                   // zerowanie licznika wystêpuj¹ych zboczy
	Ir_key_press_flag = 0;         // zerowanie flagi otrzymania kodu z pilota
}

ISR(TIMER1_CAPT_vect)
// procedura obs³ugi przerwania ICP1 // bedzie reagowa³o na zbocze opadaj¹ce
{
#define FRAME_RESTART 0
#define FRAME_OK 1
#define FRAME_END 2
	// PlseWidth -- zmienna pokazuje czas miêdzy zboczami
	static uint16_t LastCapture; //ostatnio wystepuj¹ce zbocze potrzebne mi do liczenia czasu miêdzy zboczami
	 uint16_t PulseWidth; // razem z PulseWidth //wzi¹³em to z ////www.microsyl.com/modlamp/////
	static uint16_t IrData;
	static uint8_t frame_status;
	static uint8_t a;

	PulseWidth = ICR1- LastCapture; //s³u¿y mi to dzia³anie  do obliczenia czasu miêdzy zboczami
	LastCapture = ICR1; //bo licznik TCNT1 liczy normalnie ca³y czas do 65536(przepe³nienie)



	if (PulseWidth > (49359+TOLERANCE
)) { // sprawdzam czy to nie jakieœ zak³ócenie
		frame_status = FRAME_RESTART;
	}

else if (PulseWidth < (ZERO - (TOLERANCE))) { // gdy zak³ócenia (szpilki-krótkie) - RESTART
		frame_status = FRAME_RESTART;
	//	counter=0;
		}
	if (frame_status == FRAME_RESTART) {
		counter = 0;
	}


		if (counter == 0) {

		Ir_key_press_flag = 0;//}         // wyzerowanie flagi odbioru oraz wartoœci odebranych kodów */
		IrData = 0;

		counter++;
}
		if (counter> 0) {
					frame_status = FRAME_OK;
		}

	/////////////////////////////////////////////////////////////////////////////////////////////////////)
	if (frame_status == FRAME_OK) {

		if (PulseWidth < (ZERO - (TOLERANCE))) // gdy zak³ócenia (szpilki) - RESTART
			frame_status = FRAME_RESTART;

		else if (PulseWidth > (ONE + (TOLERANCE))) // gdy b³¹d ramki danych (mo¿e inny standard) RESTART
			frame_status = FRAME_RESTART;

		else if ((PulseWidth <= (ZERO + TOLERANCE)) && (PulseWidth >= (ZERO
				- TOLERANCE))) {

			IrData |= 0 << (11 - (counter)); //ustawiam zera w Irdata (od najstarszego bitu)

		}

		else if ((PulseWidth <= (ONE + TOLERANCE)) && (PulseWidth >= (ONE
				- TOLERANCE))) {

		IrData |= 1 << (11 - (counter));
		}

		if ((counter > 11) && (Ir_key_press_flag == 0)) {
			IrData &= ~(TOOGLE_BIT); // maskowanie toogle_bit zerem; pomine go ;zawsze bedzie 0
			code = IrData;

		Ir_key_press_flag = 1;

	///	IR_OFF ;
			frame_status = FRAME_RESTART;
		}

		counter++;

	}

}
