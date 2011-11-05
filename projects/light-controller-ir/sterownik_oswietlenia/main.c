/*projekt sterownik oœwietlenia sterowany pilotem od dekodera sat ECHOSTAR DSB-616  - - atmega 8/// F_CPU=8MHz  ZEWNÊTRZNY REZONATOR
 * main.c
 *
 *  Created on: 2011-11-01
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
#include "common.h"
#include "dekoder_IR_echostar616.h"
#include <stdbool.h>
typedef uint8_t u08;
typedef uint16_t u16;

void init_ADC(void); //deklaracje funkcji
void init_TIMER2(void);
//u16 code_from_pilot(u16 kod, u08 flaga_odbioru);
volatile u08 Timer1, Timer2, Timer3, Timer4, Timer5; // timery programowe 100Hz
volatile bool flaga_uspienia; // flagi przydatne do czasowego uspienia
volatile bool flaga_hour; //

volatile u16 value_ADC; // pomocnicza zmienna do obróbki wartoœci z ADCW
/*********************************************************************************************************/
int main(void) {
	DDRC |= (1 << PC0);
	init_ADC(); //inicjalizacja przetwornika ADC
	init_interruptINT0();
	init_IR();
	init_TIMER0();
	init_TIMER2();
	T_ZAR_OFF;

	DDRC &= ~(KL_ON_OFF | KL_UP | KL_DOWN); // wejœcia
	PORTC |= (KL_ON_OFF | KL_UP | KL_DOWN); //podci¹gam do pullupów , bede sprwdza³ stan niski
	DDRB |= (LED_GREEN | LED_RED | LED_YELLOW);
	enum stan_klawisza {
		nie_wcisniety, wcisniety, trzymany, oczekujacy
	};
	static enum stan_klawisza stan_KL_ON_OFF, stan_KL_UP, stan_KL_DOWN,
			stan_KL_IR_ON_OFF, stan_KL_IR_DOWN, stan_KL_IR_UP;
	static u08 flaga_on_off = 0;
	static u08 liczba_procent = 100;

	sei();

	ACSR |= (1 << ACD); // wy³¹czenie komparatora analogowego konieczne ze wzglêdu na maksymalne obni¿enie poboru pr¹du
	while (1) {

		switch (flaga_on_off) {

		case 0: /*(flaga_on_off==0)*///gdy wy³¹czony zgaœ diody , uœpij
		{

			if ((stan_KL_IR_ON_OFF == trzymany) || (stan_KL_ON_OFF == trzymany)) {
				Timer1 = 100; //opóŸnienie po powrocie z pêtli
				while (Timer1 != 0) {
				};
			}

			flaga_uspienia = true;
			/*		if (Ir_key_press_flag == 1) {      // nie dodaje jednak tej diody bo mog³aby przeszkadzaæ
			 PORTB |= LED_YELLOW;
			 Timer4 = 10;
			 while (Timer4 != 0) {
			 };
			 PORTB &= ~(LED_YELLOW);
			 }*/
			PORTB &= ~(LED_GREEN | LED_RED);
			stan_KL_IR_ON_OFF = nie_wcisniety;
			stan_KL_ON_OFF = oczekujacy;
			liczba_procent = 100;
		brightness_bulb = procent(0);
			IR_ON; // w celu zapobiegniêcia szybkiemu w³. wy³. urz¹dzenia



			/*	if (PINC & KL_ON_OFF) { // gdy on_off nie wciœniêty
			 stan_KL_ON_OFF = oczekujacy;
			 PORTB &= ~(LED_GREEN | LED_RED);
			GICR &= ~(1 << INT0);          //wy³¹czenie przerwañ od INT0
			 //continue;
			 }*/

			if (!(PINC & KL_ON_OFF) && (stan_KL_ON_OFF == oczekujacy)
					&& (value_ADC <= 900)) {
				//poczekam a¿ min¹ drgania styków
				if (!(PINC & KL_ON_OFF)) {
					Timer1 = 250;
					while (Timer1 != 0) {
					};
					if (!(PINC & KL_ON_OFF)) {
						stan_KL_ON_OFF = wcisniety;
					}
				} else
					stan_KL_ON_OFF = nie_wcisniety;
			} else if ((!(PINC & KL_ON_OFF) && (stan_KL_ON_OFF == oczekujacy)
					&& (value_ADC > 900))) {
				PORTB |= LED_RED;
				Timer1 = 100;
				while (Timer1 != 0) {
				};
				PORTB &= ~(LED_RED); //jak jest za jasno nie da rady w³¹czyæ
				Timer1 = 100;
				while (Timer1 != 0) {
				};
			}

			/*	if (TV == code_from_pilot(code, Ir_key_press_flag)
			 && (stan_KL_IR_ON_OFF == nie_wcisniety) && (value_ADC
			 >= 900)) {
			 stan_KL_IR_ON_OFF = wcisniety;
			 }

			 else if (TV == code_from_pilot(code, Ir_key_press_flag)
			 && (stan_KL_IR_ON_OFF == nie_wcisniety) && (value_ADC
			 < 900)) {
			 IR_OFF;
			 PORTB |= LED_RED;
			 Timer1 = 100;
			 while (Timer1 != 0) {
			 };
			 PORTB &= ~(LED_RED);
			 } */
			if (((Ir_key_press_flag == 1) && (code == TV))
					&& (stan_KL_IR_ON_OFF == nie_wcisniety) && (value_ADC
					<= 900)) {
				IR_OFF;
				stan_KL_IR_ON_OFF = oczekujacy;
			}

			else if (((Ir_key_press_flag == 1) && (code == TV))
					&& (stan_KL_IR_ON_OFF == nie_wcisniety) && (value_ADC //jak jest za jasno nie da rady w³¹czyæ
					> 900)) {
				IR_OFF;
				PORTB |= LED_RED;
				Timer1 = 100;
				while (Timer1 != 0) {
				};
				PORTB &= ~(LED_RED);
			}

			if (stan_KL_IR_ON_OFF == oczekujacy) {
				Timer2 = 200;
				while (Timer2 != 0) {
				};
				stan_KL_IR_ON_OFF = wcisniety;
			}

			if (stan_KL_ON_OFF == wcisniety || stan_KL_IR_ON_OFF == wcisniety) {
				//jeœli klawisz dalej wciœniêty
				flaga_on_off = 1;
				PORTB |= (LED_GREEN) | (LED_RED);
			}
			break;
		}

			//***************************************//if(flaga_on_off==1)//*********************************//
		case 1: {
			flaga_uspienia = false;
			if (stan_KL_ON_OFF == wcisniety || stan_KL_IR_ON_OFF == wcisniety) { //opó¿nienie
				Timer1 = 100;
				while (Timer1 != 0) {
				};

				stan_KL_ON_OFF = oczekujacy;
				stan_KL_IR_ON_OFF = nie_wcisniety;

				PORTB &= ~(LED_RED);// zostaw tylko zielon¹ diodeê
			} //dopóki Timer1 bêdzie równy 0 bêdê czeka³
			GICR |= (1 << INT0); //w³¹czenie przerwañ od INT0
			Ir_key_press_flag=0;code=0;
			brightness_bulb = procent(liczba_procent);
			IR_ON;

			if (!(PINC & KL_ON_OFF) && (stan_KL_ON_OFF == oczekujacy)) {
				if (!(PINC & KL_ON_OFF)) {
					Timer3 = 200;
					while (Timer3 != 0) { //zw³oka i drgania styków
					};
					if (!(PINC & KL_ON_OFF)) {
						stan_KL_ON_OFF = trzymany;
						//flaga_on_off = 0;
					}
				}
			} else
				stan_KL_ON_OFF = oczekujacy;

			/*		if (TV == code_from_pilot(code, Ir_key_press_flag)
			 && (stan_KL_IR_ON_OFF == nie_wcisniety)) {
			 stan_KL_IR_ON_OFF = trzymany;
			 IR_OFF;
			 }*/

			if (((Ir_key_press_flag == 1) && (code == TV))
					&& (stan_KL_IR_ON_OFF == nie_wcisniety)) {
				stan_KL_IR_ON_OFF = wcisniety;
				IR_OFF; //wy³¹czam zezwolenie na przerwania od ICP
			}
			if (stan_KL_IR_ON_OFF == wcisniety) {
				Timer2 = 10;
				while (Timer2 != 0) {
				};
				stan_KL_IR_ON_OFF = trzymany;
			}

			if (stan_KL_IR_ON_OFF == trzymany || stan_KL_ON_OFF == trzymany) {
				flaga_on_off = 0;
			}

			/*		if (Ir_key_press_flag == 1) {
			 PORTB |= LED_YELLOW;
			 Timer4 = 10;
			 while (Timer4 != 0) {
			 };
			 PORTB &= ~(LED_YELLOW);
			 }
			 */
			/*******************************************************************************/
			/*********************************licznik bezczynnoœci**************************/
			/*******************************************************************************/
			    if (Ir_key_press_flag == 1) {
				flaga_uspienia = true;
			}

			if (flaga_hour == true) {
				stan_KL_IR_ON_OFF = trzymany;
				stan_KL_ON_OFF = trzymany;
				flaga_on_off = 0;
				flaga_hour = false;
			}// po dwóch godzinach bez wciskania klawiszy siê wy³¹czy
			/*******************************************************************************/
			/*******************************************************************************/
			////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			//////////////////////////////////////////KL_DOWN & KL_UP///////////////////////////////////////////////////////


			///////////////////////////////////////////////KL_DOWN//////////////////////////////////////////////////////////

			if (!(PINC & KL_DOWN) && !(stan_KL_DOWN) && (liczba_procent > 0)) {
				if (!(PINC & KL_DOWN)) {
					Timer4 = 10;
					while (Timer4 != 0) { //odczekam sobie ma³¹ przerwê
					};
					if (!(PINC & KL_DOWN)) {

						liczba_procent--;
						if (liczba_procent <= 5) {
							liczba_procent = 0;

						}
					}
				}
			} else
				stan_KL_DOWN = nie_wcisniety;

///////////////////			//=========================================IR_DOWN=====================================================


			if ((Ir_key_press_flag == 1) && (code == PAGE_MINUS)
					&& (liczba_procent >= 5) && !(stan_KL_IR_DOWN)) {           //nie chce mi siê tu tych stanów sprawdzaæ , bo i po co
				Ir_key_press_flag=0;code=0;
					             stan_KL_IR_DOWN = wcisniety;

			}

	if (stan_KL_IR_DOWN == wcisniety) {
				Timer1 = 1;
				while (Timer1 != 0) {IR_OFF;
				};
				stan_KL_IR_DOWN = nie_wcisniety;
				liczba_procent-=2;
			//	brightness_bulb = procent(liczba_procent);
				stan_KL_IR_DOWN = nie_wcisniety;
				if (liczba_procent < 5) {
					liczba_procent = 0;
					break;
				}


			}

			/////////////////////////////////////////////////////////KL_UP//////////////////////////////////////////////////
			if (!(PINC & KL_UP) && !(stan_KL_UP) && liczba_procent < 100) {
				if (!(PINC & KL_UP)) {
					Timer5 = 10;
					while (Timer5 != 0) {
					};
					if (!(PINC & KL_UP)) {
						//stan_KL_DOWN = wcisniety;
						liczba_procent++;
						if (liczba_procent >= 97) {
							liczba_procent = 100;
							//	break;
						}
					}
				}
			} else
				stan_KL_UP = nie_wcisniety;
//////////////////////			//====================================================IR_UP=======================================
			if ((Ir_key_press_flag == 1) && (code == PAGE_PLUS)
					&& !(stan_KL_IR_UP) && (liczba_procent <= 95)) {
Ir_key_press_flag=0;code=0;
			stan_KL_IR_UP = wcisniety;


			}
			if (stan_KL_IR_UP == wcisniety) {
							Timer1 = 1;
							while (Timer1 != 0) {IR_OFF;
							};

							liczba_procent+=2;
						//	brightness_bulb = procent(liczba_procent);
							stan_KL_IR_UP = nie_wcisniety;
                          if (liczba_procent >=95) {
								liczba_procent = 100;
								break;
							}

						}





			/*	if (PAGE_PLUS == code_from_pilot(code, Ir_key_press_flag)
			 && !(stan_KL_IR_UP) && (liczba_procent < 100)) {
			 /////////////////IR_OFF;
			 Timer5 = 1;
			 while (Timer5 != 0) {
			 };

			 liczba_procent++;
			 //stan_KL_IR_UP = trzymany;
			 if (liczba_procent >= 98) {
			 liczba_procent = 0;
			 //break;
			 }
			 }*/
			break;
		}
		}

	}

}

//***********************************************************************************************************
void init_ADC(void) { //wybranie kana³u ADC w tym przypadku 0101 - kana³ pi¹ty
	ADMUX |= (1 << REFS0 | 1 << REFS1 | 1 << MUX2 | 1 << MUX0); //wybór napiêcia odnesienia w postaci 2,56V
	ADCSRA |= (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1); //w³¹czenie modu³u ADC oraz ustawienie preskalera 64-->4MHZ/64
	ADCSRA |= 1 << ADFR | 1 << ADIE; // tryb free running mode i w³¹czenie przerwania
	ADCSRA |= 1 << ADSC; // start pomiaru
}
//************************************************************************************************************
void init_TIMER2(void) {
	TCCR2 |= (1 << CS22 | 1 << CS21 | 1 << CS20); //prescaler 1024 // Timer 100Hz//F_CPU=8MHz
	TCCR2 |= (1 << WGM21); // tryb CTC
	TIMSK |= (1 << OCIE2); // odblokowanie przerwania COMPARE MATCH
	OCR2 = 46;
}

//************************************************************************************************************
/*void init_TIMER1(void) {// Timer1 – inicjalizacja przerwania co 10ms //f_cpu=4Mhz
 TCCR1B |= ((1 << CS12) | (1 << CS10)); // preskaler = 1024
 TCCR1B |= 1 << WGM12; //tryb CTC
 OCR1A = 39; // przerwanie porównania co 10ms (100Hz)
 TIMSK |= (1 << OCIE1A); // Odblokowanie przerwania CompareMatch
 }*/
//*************************************************************************************************************
ISR(ADC_vect)
{
	value_ADC = ADCW;
}
//*************************************************************************************************************
ISR(TIMER2_COMP_vect)
{
	u08 n;
	static u16 liczba_sek; //1 min gdy TCNT1=6000;
	static u08 liczba_min;

	n = Timer1; /* 100Hz Timer1 */
	if (n)
		Timer1 = --n;

	n = Timer2;
	if (n > 0) {
		--n;
		Timer2 = n;
	}

	n = Timer3; /* 100Hz Timer3 */
	if (n > 0) {
		--n;
		Timer3 = n;
	}
	n = Timer4; /* 100Hz Timer4 */
	if (n > 0) {
		--n;
		Timer4 = n;
	}
	n = Timer5; /* 100Hz Timer4 */
	if (n > 0) {
		--n;
		Timer5 = n;
	}/* 100Hz Timer5 */
	if (flaga_uspienia == false) {
		liczba_sek++;
		if (liczba_sek == 6000) {
			liczba_min++;
			liczba_sek = 0;
		}
		if (liczba_min == 120) { //minê³y 2 godziny
			flaga_hour = true;
			liczba_min = 0;
			liczba_sek = 0;
		}
	}
	if (flaga_uspienia == true) {
		liczba_min = 0;
		liczba_sek = 0;
		flaga_hour = false;
		flaga_uspienia = false;
	}
}

/*
 * W PROGRAMIE NIE U¯YWANA TA FUNKCJA, W CELU UZYCIA TEJ FUNKCJI MOZNA ODKOMENTOWAC JA ODKOMENTOWAC , ORAZ ZAKOMENTOWAÆ WTEDY
 * KOD ODPOWIEDZIALNY ZA ODBIERANIE DANYCH Z PILOTA W FUNKCJI MAIN
 *
 *
 *
 *
 * u16 code_from_pilot(u16 kod, u08 flaga_odbioru) {

 u16 flaga_odebranoTV = 0;
 u16 flaga_odebranoPAGE_MINUS = 0;
 u16 flaga_odebranoPAGE_PLUS = 0;
 switch (flaga_odbioru) {
 case 1: {

 if (kod == TV) {
 //		flaga_odebranoTV = 1;
 //	if(flaga_odebranoTV == 1){
 Timer5 = 255;
 while (Timer5 != 0) {
 };
 flaga_odebranoTV = kod;//}
 flaga_odbioru = 0;
 kod = 0;
 return (flaga_odebranoTV);

 }

 if (kod == PAGE_MINUS) {
 //	flaga_odebranoPAGE_MINUS = 1;
 //		if (flaga_odebranoPAGE_MINUS == 1) {
 Timer5 = 1;
 while (Timer5 != 0) {
 };
 flaga_odebranoPAGE_MINUS = kod;
 flaga_odbioru = 0;
 kod = 0;
 return (flaga_odebranoPAGE_MINUS);

 }

 if (kod == PAGE_PLUS) {
 flaga_odebranoPAGE_PLUS=1;
 if (flaga_odebranoPAGE_PLUS == 1) {
 Timer5 = 1;
 while (Timer5 != 0) {
 };
 flaga_odebranoPAGE_PLUS = kod;//}
 flaga_odbioru = 0;
 kod = 0;
 return (flaga_odebranoPAGE_PLUS);

 }
 }
 break;
 case 0: {
 return 0;
 }
 break;
 }

 }*/
