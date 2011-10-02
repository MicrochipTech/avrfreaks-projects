#ifndef ENC_H
#define ENC_H 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define ENC_PORT 	PORTD
#define ENC_PIN 	PIND

#define STORE_PIN	0
#define ENCSW1_PIN 	4
#define KANAL_A_PIN 6
#define KANAL_B_PIN 5

#define RELAY 6
#define LICZBA_MAX = 255

//makrodefinicje mian portu dla enkodera i klawiatury
#define CLR_RELAY ENC_PORT &= ~_BV(RELAY)
#define SET_RELAY ENC_PORT |=  _BV(RELAY)
#define RELAY_PIN	PIND6

void int0_init(void);
uint8_t impulsator(uint8_t liczba);
uint8_t enc_dir(uint8_t tact);
uint8_t check_key(uint8_t laststate);

#endif 
