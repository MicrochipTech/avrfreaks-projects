/*
 * triak.h
 *
 *  Created on: 2011-09-13
 *       Autor: £ukasz Uszko
 */

#ifndef TRIAK_H_
#define TRIAK_H_

//ustawienia portów
#define T_ZAR_ON PORTC |= (1<<PC0)
#define T_ZAR_OFF PORTC &= ~(1<<PC0)
//ustawienia dimmera

#define LICZBA_KROKOW 200


// start i stop TIMER2
////#define TIMER2_START TCCR2 |= (1<<CS21)	// prescaler=8
////#define TIMER2_STOP TCCR2 &= ~(1<<CS21)
#define TIMER0_START TCCR0|= (1<<CS01)
#define TIMER0_STOP TCCR0&= ~(1<<CS01)
//zmienne

extern volatile uint8_t brightness_bulb;   // dostêpny  we wszystkich plikach
extern volatile uint8_t kroki;


//deklaracje funkcji
uint8_t procent(uint8_t wartosc_procentowa);
void init_interruptINT0(void);
void init_TIMER0(void);

#endif /* TRIAK_H_ */
