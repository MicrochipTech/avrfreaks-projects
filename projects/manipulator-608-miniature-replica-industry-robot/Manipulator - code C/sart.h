/*
 * usart.h
 *
 * Created: 2013-01-17 21:37:01
 *  Author: tmf
 */ 


#ifndef USART_H_
#define USART_H_

#include <avr/io.h>
#include <stdint.h>
#include <stdbool.h>
#include "Globals_Defs.h"


extern volatile uint8 cmdrec;
extern volatile bool TxFlag;

bool usart_set_baudrate(USART_t * const usart, uint32 baud, uint32 cpu_hz);        //Ustaw prêdkoœæ transmisji USART
void USART_putchar(USART_t * const usart, char ch);                                    //Wyœlij znak
void USART_send(USART_t * const usart, const char *txt);                               //Wyœlij ³añcuch w formacie NULLZ
void USART_send_F(USART_t *const usart, const char *txt);                              //Wyœlij ³añcuch w formacie NULLZ z FLASH
void USART_send_block(USART_t * const usart, const uint8 *block, uint8 size);      //Wyœlij blok o d³ugoœci size z pamiêci SRAM
void USART_send_block_F(USART_t * const usart, const uint8 *block, uint8 size);    //Wyœlij blok z pamiêci FLASH
void USART_send_int(USART_t* const usart, const uint16 data); 						/*Send uint16 data*/
void USART_MergingData(void); 														/*Make a uint16 from 2 packet from USART*/
void Error(void);

static inline void waitforTx(USART_t *usart)            //Zaczekaj na koniec nadawania znaku
{
	while(!(usart->STATUS & USART_TXCIF_bm));
	usart->STATUS=USART_TXCIF_bm;
}

#endif /* USART_H_ */
