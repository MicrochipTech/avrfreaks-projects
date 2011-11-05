/*
 * dekoder_IR_echostar616.h
 *
 *  Created on: 2011-09-20
 *       Autor: £ukasz Uszko
 */

#ifndef DEKODER_IR_ECHOSTAR616_H_
#define DEKODER_IR_ECHOSTAR616_H_

#define IR_PORT B
#define IR_PIN 0
#define IR_IN (1<<IR_PIN)

// sta³e i zmienne potrzebne na wewnêtrzne cele procedury obs³ugi RC5

#define TIMER1_PRESCALER 8

// przeliczanie tykniêæ Timera1 przy danym preskalerze na mikrosekundy
#define micro_second(num) ((num)*(F_CPU/1000000)/TIMER1_PRESCALER)

// sta³e czasowe i tolerancja wyra¿one w us (nazwy  i wartoœci z lirc.remotes.org)
#define TOLERANCE 400
#define ONE  	micro_second(7372)                  //czas trwania jedynki
#define  ZERO	micro_second(4834)                  // czas trwania zera
#define TOOGLE_BIT 0x200
//w³aczenie przerwañ od ICP1
#define IR_ON  TIMSK|=(1 << TICIE1)            //w³¹czam zezwolenie na przerwania od ICP1
#define IR_OFF 	TIMSK&=~(1 << TICIE1)              //wy³¹czam zezwolenie na przerwania od ICP
// Makra upraszczaj¹ce dostêp do portów
// *** PORT
#define PORT(x) XPORT(x)
#define XPORT(x) (PORT##x)
// *** PIN
#define PIN(x) XPIN(x)
#define XPIN(x) (PIN##x)
// *** DDR
#define DDR(x) XDDR(x)
#define XDDR(x) (DDR##x)

// ------------------------------------------- definicje klawiszy PILOTA IR
#define TV 0x566

#define PAGE_PLUS /*0x55A*/0x568
#define PAGE_MINUS /*0x55F*/0x56A


// ------------------------------------------- definicje klawiszy KONIEC

// zmiennacode
extern volatile uint16_t code; // kod klawisza pilota    pierwsze 3 bity zawsze takie same

// flaga = 1 - informuje, ¿e odebrany zosta³ nowy kod z pilota
extern volatile uint8_t Ir_key_press_flag;

// deklaracje funkcji dla u¿ytkownika
void init_IR(void);

#endif /* DEKODER_IR_ECHOSTAR616_H_ */
