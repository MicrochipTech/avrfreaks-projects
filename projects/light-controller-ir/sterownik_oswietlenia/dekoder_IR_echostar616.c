/*
 * dekoder_IR_echostar616.c
 *program dekoduj�cy sygna� z pilota od dekodera ECHOSTAR 616
 *
 *  Created on: 2011-09-20
 *       Autor: �ukasz Uszko
 */

#include <avr/io.h>
#include <avr/interrupt.h>

#include "dekoder_IR_echostar616.h"
volatile uint16_t code; // kod danego przycisku
volatile uint8_t Ir_key_press_flag; // flaga = 1 - informuje, �e odebrany zosta� nowy kod z pilota
                                    // po odczytaniu danych ze zmiennych zeruj�  flag� aby mo�na by�o wczyta� kolejne kody z pilota
volatile uint8_t counter; // licznik zboczy na ICP1(bit�w z pilota)

void init_IR() {
	DDR(IR_PORT) &= ~IR_IN; // pin jako wej�cie
	PORT(IR_PORT) |= IR_IN; // podci�gni�cie pinu do VCC
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
	//TCCR1B |= (1 << ICNC1);        //filtr zak��ce�
	TCCR1B &= ~(1 << ICES1);       // Zbocze opadaj�ce na ICP
	IR_ON;      // zezwolenie na Przerwanie od ICP
	counter = 0;                   // zerowanie licznika wyst�puj�ych zboczy
	Ir_key_press_flag = 0;         // zerowanie flagi otrzymania kodu z pilota
}

ISR(TIMER1_CAPT_vect)
// procedura obs�ugi przerwania ICP1 // bedzie reagowa�o na zbocze opadaj�ce
{
#define FRAME_RESTART 0
#define FRAME_OK 1
#define FRAME_END 2
	// PlseWidth -- zmienna pokazuje czas mi�dzy zboczami
	static uint16_t LastCapture; //ostatnio wystepuj�ce zbocze potrzebne mi do liczenia czasu mi�dzy zboczami
	 uint16_t PulseWidth; // razem z PulseWidth //wzi��em to z ////www.microsyl.com/modlamp/////
	static uint16_t IrData;
	static uint8_t frame_status;
	static uint8_t a;

	PulseWidth = ICR1- LastCapture; //s�u�y mi to dzia�anie  do obliczenia czasu mi�dzy zboczami
	LastCapture = ICR1; //bo licznik TCNT1 liczy normalnie ca�y czas do 65536(przepe�nienie)



	if (PulseWidth > (49359+TOLERANCE
)) { // sprawdzam czy to nie jakie� zak��cenie
		frame_status = FRAME_RESTART;
	}

else if (PulseWidth < (ZERO - (TOLERANCE))) { // gdy zak��cenia (szpilki-kr�tkie) - RESTART
		frame_status = FRAME_RESTART;
	//	counter=0;
		}
	if (frame_status == FRAME_RESTART) {
		counter = 0;
	}


		if (counter == 0) {

		Ir_key_press_flag = 0;//}         // wyzerowanie flagi odbioru oraz warto�ci odebranych kod�w */
		IrData = 0;

		counter++;
}
		if (counter> 0) {
					frame_status = FRAME_OK;
		}

	/////////////////////////////////////////////////////////////////////////////////////////////////////)
	if (frame_status == FRAME_OK) {

		if (PulseWidth < (ZERO - (TOLERANCE))) // gdy zak��cenia (szpilki) - RESTART
			frame_status = FRAME_RESTART;

		else if (PulseWidth > (ONE + (TOLERANCE))) // gdy b��d ramki danych (mo�e inny standard) RESTART
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
