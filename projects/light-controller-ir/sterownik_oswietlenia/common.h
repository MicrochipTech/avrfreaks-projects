/*
 * common.h
 *
 *  Created on: 2011-09-09
 *       Autor: £ukasz Uszko
 */

#ifndef COMMON_H_
#define COMMON_H_

////////////////////////////////////////////////////PORTY DLA DIÓD////////////////////////////////////////////////////////////////////////////
#define KL_ON_OFF  1<<PC3
#define KL_UP  1<<PC2
#define KL_DOWN  1<<PC1
#define LED_GREEN 1<<PB1                                // pod³¹czenie diód sygnalizuj¹cych
#define LED_RED 1<<PB2
#define LED_YELLOW 1<<PB3

///////////////////////////////////////////////////DO TRIAK.C /////////////////////////////////////////////////////////////////////////////////
#define T_ZAR_ON PORTC |= (1<<PC0)
#define T_ZAR_OFF PORTC &= ~(1<<PC0)
//#define LICZBA_KROKOW 200
#define SZPILKA_MS 1.8



#endif /* COMMON_H_ */
