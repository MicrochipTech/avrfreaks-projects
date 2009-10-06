/*
 * hw.h
 *
 *  Created on: 2009-08-28
 *      Author: liku
 *
 *      Header file for low level operations. Contains constant values, some
 *      pseudofunctions (macros) and function headers.
 */

#ifndef HW_H_
#define HW_H_

#include <stdbool.h>

#include <avr/io.h>

/*
 * Set to 1 if debugWire is about to be used
 */
#define DEBUGWIRE 0

/*
 * Hardware definitions
 */
#define HD44780_PORT D
#define HD44780_RS   PD4
#define HD44780_RW   PD5
#define HD44780_E    PD7
#define HD44780_D4   PD0
#define HD44780_D5   PD1
#define HD44780_D6   PD2
#define HD44780_D7   PD3

#define LCDVCCPORT PORTC
#define LCDVCCDDR  DDRC
#define LCDVCC     PC4

#define SHUTPORT  PORTB
#define SHUTREL   PB0

#define BATTPROBE PC5

#define ENCPIN    PINC
#define ENCPORT   PORTC
#define ENCDDR    DDRC
#define ENCSW     PC1
#define ENC0      PC2
#define ENC1      PC3
#define ENCMASK   (_BV(ENCSW) | _BV(ENC0) | _BV(ENC1))

/*
 * Version string
 */
#define HWVERSIONSTRING "20091005"

/*
 * Pseudo functions
 */
#define hwLCDOn(x)        (LCDVCCPORT |= _BV(LCDVCC));
#define hwADCOn(x)        (ADCSRA |= _BV(ADEN));
#define hwADCOff(x)       (ADCSRA &= ~(_BV(ADEN)));
#define hwT1OvfIntOn(x)   (TIMSK1 |= _BV(TOIE1));
#define hwT1OvfIntOff(x)  (TIMSK1 &= ~(_BV(TOIE1)));
#define hwShutOn(x)       (SHUTPORT |= _BV(SHUTREL));
#define hwShutOff(x)      (SHUTPORT &= ~(_BV(SHUTREL)));
#define HWBATTVOL(x)      ((x*2560l)/1100l) // x = Vbatt * 100 (ex. 298) => converts that to ADC value
#define HWLOWBATTVAL      265l              // voltage which triggers "low battery" message (divide by 100)

/*
 * Function prototypes
 */
void hwSetupPorts(void);
void hwSetupADC(void);
void hwSetupT0(void);
void hwSetupT1(void);
void hwSetupT2(void);
void hwSetupPwr(void);
void hwSetupAll(void);
void hwEnterPwrSave(void);
bool hwIsPwrOn(void);
void hwLCDOff(void);
void hwEncOff(void);
void hwEncOn(void);
uint16_t hwGetADC(void);
uint16_t hwGetBattVol(void);
uint16_t hwMulCoeff(uint16_t in, uint16_t coeff1, uint16_t coeff2);


#endif /* HW_H_ */
